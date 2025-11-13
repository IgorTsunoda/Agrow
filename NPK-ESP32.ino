#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <SPIFFS.h>
#include <ModbusMaster.h>
#include <time.h>

const char* ssid = "SEU_WIFI";
const char* password = "SENHA_WIFI";
const char* mqtt_server = "xxxxxx-ats.iot.us-east-1.amazonaws.com";
const int mqtt_port = 8883;

String sensor_id = "sensor_01";

WiFiClientSecure espClient;
PubSubClient client(espClient);

#define RX_PIN 16
#define TX_PIN 17
ModbusMaster node;
uint8_t sensorAddress = 0x01;

const uint64_t deepSleepTime = 17280000000ULL;
const char* offlineFile = "/pendentes.txt";

void salvarOffline(String linha) {
  File f = SPIFFS.open(offlineFile, FILE_APPEND);
  if (f) { f.println(linha); f.close(); }
}

void enviarPendentes() {
  if (!SPIFFS.exists(offlineFile)) return;
  File f = SPIFFS.open(offlineFile, FILE_READ);
  if (!f) return;
  while (f.available()) {
    String linha = f.readStringUntil('\n');
    if (linha.length() < 5) continue;
    client.publish("npk7/sensores", linha.c_str());
    delay(300);
  }
  f.close();
  SPIFFS.remove(offlineFile);
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  unsigned long t = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t < 15000) { delay(200); }
}

void reconnectMQTT() {
  unsigned long t = millis();
  while (!client.connected() && millis() - t < 15000) {
    client.connect(sensor_id.c_str());
    delay(1000);
  }
}

String gerarTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "";
  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  return String(buffer);
}

void iniciarNTP() {
  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  delay(1500);
}

bool lerNPK7(uint16_t &n, uint16_t &p, uint16_t &k, float &t, float &h, uint16_t &ec, float &ph) {
  uint8_t r = node.readInputRegisters(0x0001, 7);
  if (r != node.ku8MBSuccess) return false;
  n = node.getResponseBuffer(0);
  p = node.getResponseBuffer(1);
  k = node.getResponseBuffer(2);
  t = node.getResponseBuffer(3) / 10.0;
  h = node.getResponseBuffer(4) / 10.0;
  ec = node.getResponseBuffer(5);
  ph = node.getResponseBuffer(6) / 10.0;
  return true;
}

void setup() {
  Serial.begin(115200);
  SPIFFS.begin(true);

  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  node.begin(sensorAddress, Serial2);

  connectWiFi();
  iniciarNTP();

  espClient.setCACertFile("/aws_root_ca.pem");
  espClient.setCertificateFile("/device_cert.pem.crt");
  espClient.setPrivateKeyFile("/private.pem.key");

  client.setServer(mqtt_server, mqtt_port);

  uint16_t n, p, k, ec;
  float st, sh, ph;

  if (!lerNPK7(n, p, k, st, sh, ec, ph)) {
    ESP.deepSleep(deepSleepTime);
  }

  String timestamp = gerarTimestamp();

  String json = "{";
  json += "\"id\":\"" + sensor_id + "\",";
  json += "\"timestamp\":\"" + timestamp + "\",";
  json += "\"ph\":" + String(ph, 5) + ",";
  json += "\"nitrogenio\":" + String(n, 5) + ",";
  json += "\"fosforo\":" + String(p, 5) + ",";
  json += "\"potassio\":" + String(k, 5) + ",";
  json += "\"ec\":" + String(ec / 1000.0, 5) + ",";
  json += "\"umidade\":" + String(sh, 5) + ",";
  json += "\"temperatura\":" + String(st, 5);
  json += "}";

  if (WiFi.status() == WL_CONNECTED) {
    reconnectMQTT();
    if (client.connected()) {
      enviarPendentes();
      client.publish("npk7/sensores", json.c_str());
      ESP.deepSleep(deepSleepTime);
    } else {
      salvarOffline(json);
      ESP.deepSleep(deepSleepTime);
    }
  } else {
    salvarOffline(json);
    ESP.deepSleep(deepSleepTime);
  }
}

void loop() {}
