# 🌱 Agrow

O **Agrow** é um projeto voltado para o processamento e inserção automatizada de dados agrícolas em ambientes em nuvem.  
Ele foi idealizado com o objetivo de facilitar a integração entre dispositivos IoT (como sensores de solo) e bancos de dados hospedados na AWS.

A ideia central é permitir que dados coletados por sensores sejam tratados e armazenados de forma eficiente, possibilitando análises posteriores sobre o solo, clima e produtividade.

---

## 🧠 Estrutura do Projeto

Atualmente o projeto possui o seguinte arquivo principal:

### `lambda_split_to_insert.py`

Este script foi desenvolvido para ser executado dentro de uma **AWS Lambda Function**.  
Sua principal função é **dividir um conjunto de dados recebido (normalmente em JSON)** em partes menores e realizar a **inserção organizada** dessas informações em um banco de dados relacional (como o **Amazon RDS**).

### ⚙️ Funcionalidades

- Conexão com banco de dados RDS (MySQL);
- Leitura e interpretação de eventos enviados via AWS IoT Core ou API Gateway;
- Separação dos registros em blocos menores para otimizar o desempenho da inserção;
- Execução de comandos `INSERT` de forma controlada, garantindo integridade e eficiência;
- Retorno de mensagens de status (sucesso, falha, quantidade de registros processados).

---

## 📦 Tecnologias Utilizadas

- **Python 3.x**
- **AWS Lambda**
- **AWS RDS (MySQL)**
- **pymysql**
- **json**

---

## 🚀 Objetivo do Projeto

O objetivo do **Agrow** é servir como base para soluções de **agricultura inteligente (Smart Farming)**, onde os dados são coletados automaticamente em campo e enviados para a nuvem, permitindo análises em tempo real sobre fatores que impactam a produtividade agrícola.

---

## 🧩 Próximos Passos

- Adicionar validações dos dados antes da inserção;
- Criar logs detalhados em CloudWatch;
- Integrar com uma camada de visualização em **Power BI** ou **Grafana**;
- Implementar autenticação e controle de permissões via AWS IAM.

---

## 👨‍💻 Autor

**Igor Tsunoda**  
Desenvolvedor e pesquisador em soluções de integração IoT + Cloud.  
GitHub: [@IgorTsunoda](https://github.com/IgorTsunoda)
