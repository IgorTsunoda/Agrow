import json
import pymysql
import os

# Conexão com RDS
def connect_rds():
    return pymysql.connect(
        host=os.environ['DB_HOST'],
        user=os.environ['DB_USER'],
        password=os.environ['DB_PASS'],
        database=os.environ['DB_NAME'],
        cursorclass=pymysql.cursors.DictCursor
    )

def lambda_handler(event, context):
    # Se o IoT Core enviar o payload como string JSON, converte
    if isinstance(event, str):
        event = json.loads(event)
    elif 'body' in event:
        event = json.loads(event['body'])

    # Garante que temos uma lista
    if not isinstance(event, list):
        event = [event]

    connection = connect_rds()

    with connection:
        with connection.cursor() as cursor:
            for record in event:
                sql = """
                    INSERT INTO dados_solo
                    (sensor_id, Temperatura, Umidade, Nitrogenio, Fosforo, Potassio, Ph, Ec, UmidadeSolo, TemperaturaSolo)
                    VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
                """
                cursor.execute(sql, (
                    record['mac_Id'],
                    record['Temperatura'],
                    record['Umidade'],
                    record['Nitrogenio'],
                    record['Fosforo'],
                    record['Potassio'],
                    record['pH'],
                    record['EC'],
                    record['UmidadeSolo'],
                    record['TemperaturaSolo']
                ))
        connection.commit()

    return {
        'statusCode': 200,
        'body': json.dumps({'message': 'Dados inseridos com sucesso!'})
    }
