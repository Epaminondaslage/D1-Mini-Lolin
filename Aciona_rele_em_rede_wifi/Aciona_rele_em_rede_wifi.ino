

/**************************************************************************
 *                 Placa de desenvolvimento Wemos D1 Mini Lolin           *
 *               Acionamento de um módulo relé com página web             *
 *                Necessita de shild relé para Wemos d1 mini              *
 **************************************************************************/
//teste 

#include <ESP8266WiFi.h>

// Declara SSID e Senha de rede Wifi
const char* ssid = "xxxxxxxx";
const char* password = "xxxxxxxxxx";


// Declara senha de acesso ao acionamento http
const char* senhaHttp = "xxxx";

// Declara variáveis de Led e Rele
int ledPin = D4;  // GPIO2 Led build in para monitoramento
int relePin = D1; // GPIO5 acesso ao módulo relé

int value = LOW;

  
// Inicializa servidor web porta 80
WiFiServer server(80);

void setup() {

  //Inicializa porta serial
  Serial.begin(9600);
  delay(10);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  pinMode(relePin, OUTPUT);
  digitalWrite(relePin, LOW);
  Serial.println();
  Serial.println();
  Serial.print("Conectando a Rede ");
  Serial.println(ssid);
  
  // inicializa WiFi
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Conectado");

  // Inicia o Servidor
  server.begin();
  Serial.println("Servidor Inicializado");

  // Mostra na serial o Endereço IP
  Serial.print("Use este endereço para conectar: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {

  // Verifica se um cliente conectou
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Aguarda até um cliente enviar dados
  Serial.println("Novo Cliente");
  while (!client.available()) {
    delay(1);
  }

  // Le a primeira linha da requisição
  client.setTimeout(100);
  String request = client.readString();
  //Serial.println(request);
  client.flush();

  // Marca a requisição
  String senhaMontada = "senha=";
  senhaMontada += senhaHttp;
  senhaMontada += "&url";
  bool autenticado = request.indexOf(senhaMontada) != -1;

  if (autenticado && request.indexOf("Ativar") != -1) {
    digitalWrite(ledPin, LOW);
    digitalWrite(relePin, HIGH);
    value = HIGH;
  }
  if (autenticado && request.indexOf("Desativar") != -1) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(relePin, LOW);
    value = LOW;
  }

  // Aciona o Ledpin conforme a requisição
  // Retorna a resposta

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<meta charset ='utf-8'/>");
  client.println("<meta name = 'viewport' content = 'width=device-width, initial-scale=1,user-scalable=0'>");
  client.println(F("<link href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.1/css/bootstrap.min.css' rel='stylesheet'></link>"));
  client.println("<div class='jumbotron'>");
  client.println("<h2 class='text-center'>Desbloqueio de Senha</h2>");
  client.println("<h3 class='text-center'>   Elevador Social </h3>");
  client.println("</div>");
  client.println("<div class='col-md-6'>");
  client.println("<div class='alert alert-info text-center' role='alert'>7&ordm andar</div>");
  client.println("<form method='POST' action='/'>");
  client.println("<input type='password' placeholder='senha' name='senha' class='form-control' style='margin-bottom: 18px; padding-top: 22px; padding-bottom: 22px;'>");
  client.println("<input type='submit' class='btn btn-danger btn-lg center-block' name='url' value='Desativar' style='width: 100%'>");
  client.println("<br>");
  client.println("<input type='submit' class='btn btn-success btn-lg center-block' name='url' value='Ativar' style='width: 100%'>");
  client.println("</form>");
  client.println("<br><center>");
  if (!autenticado && request.indexOf("url=") != -1) { // mensagem de falha de autenticacao
    client.println("<span class='alert alert-danger'>Senha incorreta</span>");
  } else {
    client.println("<span class='alert alert-warning'>Sistema ");
    client.println(value == LOW ? "desativado</span>" : "ativado</span>");
  }
  client.println("</center>");
  client.println("</div>");
  Serial.println("Cliente desconectado");
  Serial.println("");
}
