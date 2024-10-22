#include <Wire.h> // I2C library
#include <Adafruit_SHT31.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h> 
#include <WebServer.h>
#include <EEPROM.h>
#include "ccs811.h"  // CCS811 library
#include <HTTPClient.h> // Biblioteca para realizar chamadas HTTP

// Defina os dados da rede Wi-Fi
const char* ssid = "LIVE TIM_A8DB_2G";
const char* password = "Br@st13mpp";

String serverIP = "http://localhost:8080";

// Wiring for ESP32 NodeMCU boards: VDD to 3V3, GND to GND, SDA to 21, SCL to 22, nWAKE to D3 (or GND)
CCS811 ccs811(23); // nWAKE on 23

// Pinos dos relés
#define PIN_LED_RELAY           25
#define PIN_HUMIDIFIER_RELAY    26
#define PIN_FAN_RELAY           27
#define PIN_EXHAUST_FAN_RELAY   33

// Endereços na EEPROM para armazenar os setpoints e o valor de controle
#define EEPROM_ADDRESS_TEMPERATURE_SETPOINT 0
#define EEPROM_ADDRESS_HUMIDITY_SETPOINT 4
#define EEPROM_ADDRESS_CONTROL 8
#define EEPROM_ADDRESS_IP 12 // Endereço para armazenar o IP na EEPROM

// Valor de controle para verificar se os setpoints estão presentes
const uint32_t CONTROL_VALUE = 0x12345678;

// Setpoints para temperatura e umidade
float temperatureSetpoint = 25.0; // Em graus Celsius
float humiditySetpoint = 60.0;    // Em percentual de umidade relativa

// Limites de tolerância
float temperatureTolerance = 0.5; // Tolerância de temperatura em graus Celsius
float humidityTolerance = 0.5;    // Tolerância de umidade em percentual de umidade relativa

// Crie uma instância do servidor na porta 80
WebServer server(80);

// Instância do sensor SHT31
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Instância do display I2C 16x2
LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Endereço I2C 0x3F

// Variáveis para controlar a exibição
unsigned long previousMillis = 0;
const long intervalDisplay = 5000; // Intervalo de 5 segundos para exibição de cada ciclo
int displayMode = 0; // 0: temperatura/umidade, 1: setpoints, 2: endereço IP

// Variável para controlar o intervalo de 1 minuto para envio do post para o servidor web chart
unsigned long previousPostMillis = 0;
const long intervalPost = 60000; // Intervalo de 60 segundos

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  // Inicialize o temporizador para envio de dados
  previousPostMillis = millis();

  // Recupere os setpoints da EEPROM
  EEPROM.begin(512); // Inicialize a EEPROM com um tamanho arbitrário
  // Verifique se o valor de controle está presente
  uint32_t controlValue = 0;
  EEPROM.get(EEPROM_ADDRESS_CONTROL, controlValue);
  if (controlValue == CONTROL_VALUE) {
    // Recupere os setpoints da EEPROM
    temperatureSetpoint = readFloatFromEEPROM(EEPROM_ADDRESS_TEMPERATURE_SETPOINT);
    humiditySetpoint = readFloatFromEEPROM(EEPROM_ADDRESS_HUMIDITY_SETPOINT);
  }

  // Tente recuperar o IP salvo na EEPROM
  String storedIP = readStringFromEEPROM(EEPROM_ADDRESS_IP, 50); // 50 é o comprimento máximo da string
  if (storedIP.length() > 0) {
    serverIP = storedIP;
  }
  Serial.print("Servidor web charts configurado para: ");
  Serial.println(serverIP);

  // Inicialize o barramento I2C
  Wire.begin(21, 22);

  // Inicialize o sensor SHT31
  if (!sht31.begin(0x44)) {  // Endereço padrão do SHT31: 0x44
    Serial.println("Falha ao iniciar o sensor SHT31!");
    while (1) delay(1);
  }
  Serial.println("Sensor SHT31 inicializado com sucesso.");

  // Inicializa o sensor CO2 ccs811 
  Serial.println("setup: Starting CCS811 basic demo");
  Serial.print("setup: ccs811 lib  version: "); 
  Serial.println(CCS811_VERSION);
  // Enable CCS811
  ccs811.set_i2cdelay(50); // Needed for ESP8266 because it doesn't handle I2C clock stretch correctly
  bool ok= ccs811.begin();
  if( !ok ) Serial.println("setup: CCS811 begin FAILED");
  // Print CCS811 versions
  Serial.print("setup: hardware    version: "); Serial.println(ccs811.hardware_version(),HEX);
  Serial.print("setup: bootloader  version: "); Serial.println(ccs811.bootloader_version(),HEX);
  Serial.print("setup: application version: "); Serial.println(ccs811.application_version(),HEX);
  // Start measuring
  ok= ccs811.start(CCS811_MODE_1SEC);
  if( !ok ) Serial.println("setup: CCS811 start FAILED");

  // Inicialize o display com número de colunas e linhas
  lcd.begin(16, 2);

  // Ative a luz do display
  lcd.backlight();

  // Inicialize os pinos dos relés como saídas
  pinMode(PIN_LED_RELAY, OUTPUT);
  pinMode(PIN_HUMIDIFIER_RELAY, OUTPUT);
  pinMode(PIN_FAN_RELAY, OUTPUT);
  pinMode(PIN_EXHAUST_FAN_RELAY, OUTPUT);

  // Desligue todos os relés inicialmente
  digitalWrite(PIN_LED_RELAY, LOW);
  digitalWrite(PIN_HUMIDIFIER_RELAY, LOW);
  digitalWrite(PIN_FAN_RELAY, LOW);
  digitalWrite(PIN_EXHAUST_FAN_RELAY, LOW);

  // Conecte-se à rede Wi-Fi
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(1000);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFalha ao conectar ao Wi-Fi!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Falha ao conectar");
    lcd.setCursor(0, 1);
    lcd.print("no WIFI");
  } else {
    Serial.println("\nConectado ao Wi-Fi com sucesso!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
  }

  // Rota principal
  server.on("/", handleRoot);
  
  // Rota para atualização dos setpoints e tolerâncias
  server.on("/setpoints", HTTP_POST, handleSetpoints);

  // Rotas para controle dos relés
  server.on("/led/on", HTTP_GET, []() {
    digitalWrite(PIN_LED_RELAY, HIGH);
  });
  
  server.on("/led/off", HTTP_GET, []() {
    digitalWrite(PIN_LED_RELAY, LOW);
  });
  
  server.on("/exhaust/on", HTTP_GET, []() {
    digitalWrite(PIN_EXHAUST_FAN_RELAY, HIGH);
    server.send(200, "text/plain", "Exaustor ligado");
  });
  
  server.on("/exhaust/off", HTTP_GET, []() {
    digitalWrite(PIN_EXHAUST_FAN_RELAY, LOW);
    server.send(200, "text/plain", "Exaustor desligado");
  });

  // Nova rota para controlar o backlight do LCD
  server.on("/lcd/backlight/on", HTTP_GET, []() {
    lcd.backlight();
    server.send(200, "text/plain", "Backlight ligado");
  });

  server.on("/lcd/backlight/off", HTTP_GET, []() {
    lcd.noBacklight();
    server.send(200, "text/plain", "Backlight desligado");
  });

  // Rota para atualizar o IP do servidor
  server.on("/set-ip", HTTP_POST, handleSetIP);

  // Inicie o servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= intervalDisplay) {
    displayMode++;
    displayMode %= 4; // Existem 4 modos de exibição
    switch (displayMode) {
      case 0:
        showTemperatureHumidity();
        break;
      case 1:
        showSetpoints();
        break;
      case 2:
        showIPAddress();
        break;
      case 3:
        readCo2();
        break;
    }
    // Verifica se é hora de enviar os dados
    if (currentMillis - previousPostMillis >= intervalPost) {
      sendData();
    }
    previousMillis = currentMillis;
  }

  controlRelays(); // controle de reles

  server.handleClient();
}
  
void readCo2(){
  // Read
  uint16_t eco2, etvoc, errstat, raw;
  ccs811.read(&eco2,&etvoc,&errstat,&raw); 
  
  // Print measurement results based on status
  if( errstat==CCS811_ERRSTAT_OK ) { 
    Serial.print("CCS811: ");
    Serial.print("eco2=");  Serial.print(eco2);     Serial.print(" ppm  ");
    Serial.print("etvoc="); Serial.print(etvoc);    Serial.print(" ppb  ");
    Serial.println();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CO2: ");
    lcd.print(eco2);
    lcd.print(" PPM");
    lcd.setCursor(0, 1);
    lcd.print("TVOC: ");
    lcd.print(etvoc);
    lcd.print(" PPB");
  } else if( errstat==CCS811_ERRSTAT_OK_NODATA ) {
    Serial.println("CCS811: waiting for (new) data");
  } else if( errstat & CCS811_ERRSTAT_I2CFAIL ) { 
    Serial.println("CCS811: I2C error");
  } else {
    Serial.print("CCS811: errstat="); Serial.print(errstat,HEX); 
    Serial.print("="); Serial.println( ccs811.errstat_str(errstat) ); 
  }
}

void showTemperatureHumidity() {
  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  //seta os valores de humidade e temperatura para o sensor CCS811 
  ccs811.set_envdata_Celsius_percRH(temperature, humidity);

  if (!isnan(temperature) && !isnan(humidity)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("Umid: ");
    lcd.print(humidity);
    lcd.print(" %");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro na leitura");
  }
}

void showSetpoints() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set: T:");
  lcd.print(temperatureSetpoint);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Set: H:");
  lcd.print(humiditySetpoint);
  lcd.print(" %");
}

void showIPAddress() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Endereco IP:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
}

void controlRelays() {
  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  if (!isnan(temperature) && !isnan(humidity)) {
    if (temperature > temperatureSetpoint + temperatureTolerance) {
      digitalWrite(PIN_FAN_RELAY, HIGH); // Liga o ventilador
    } else if (temperature < temperatureSetpoint - temperatureTolerance) {
      digitalWrite(PIN_FAN_RELAY, LOW); // Desliga o ventilador
    }

    if (humidity < humiditySetpoint - humidityTolerance) {
      digitalWrite(PIN_HUMIDIFIER_RELAY, HIGH); // Liga o umidificador
    } else if (humidity > humiditySetpoint + humidityTolerance) {
      digitalWrite(PIN_HUMIDIFIER_RELAY, LOW); // Desliga o umidificador
    }
  } else {
    Serial.println("Falha ao ler dados do sensor SHT31");
  }
}

// Função para ler um valor float da EEPROM
float readFloatFromEEPROM(int address) {
  float value = 0.0;
  EEPROM.get(address, value);
  return value;
}

// Função para escrever um valor float na EEPROM
void writeFloatToEEPROM(int address, float value) {
  EEPROM.put(address, value);
  EEPROM.commit(); // Necessário para salvar os dados na EEPROM
}

// Função para ler uma string da EEPROM
String readStringFromEEPROM(int address, int length) {
  char data[length + 1]; // +1 para o terminador nulo
  for (int i = 0; i < length; i++) {
    data[i] = EEPROM.read(address + i);
  }
  data[length] = '\0'; // Adiciona o terminador nulo
  return String(data);
}

// Função para escrever uma string na EEPROM
void writeStringToEEPROM(int address, String value, int length) {
  for (int i = 0; i < length; i++) {
    if (i < value.length()) {
      EEPROM.write(address + i, value[i]);
    } else {
      EEPROM.write(address + i, 0); // Preenche com zeros
    }
  }
  EEPROM.commit(); // Necessário para salvar os dados na EEPROM
}

void handleRoot() {
  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  uint16_t eco2, etvoc, errstat, raw;
  ccs811.read(&eco2,&etvoc,&errstat,&raw); 


  int ledState = digitalRead(PIN_LED_RELAY);
  int humidifierState = digitalRead(PIN_HUMIDIFIER_RELAY);
  int fanState = digitalRead(PIN_FAN_RELAY);
  int exhaustFanState = digitalRead(PIN_EXHAUST_FAN_RELAY);

  String html = "<html><head><meta http-equiv=\"refresh\" content=\"10\"></head><body><h1>Valores do SHT31</h1>";
  if (!isnan(temperature) && !isnan(humidity)) {
    html += "<p>Temperatura: " + String(temperature) + " C</p>";
    html += "<p>Umidade: " + String(humidity) + " %</p>";
    html += "<p>eco2: " + String(eco2) + " PPM</p>";
    html += "<p>etvoc: " + String(etvoc) + " PPB</p>";
  } else {
    html += "<p>Erro ao ler sensor SHT31</p>";
  }
  html += "<h2>Setpoints:</h2>";
  html += "<form action=\"/setpoints\" method=\"post\">";
  html += "Temperatura (C): <input type=\"text\" name=\"temperature\" value=\"" + String(temperatureSetpoint) + "\"><br>";
  html += "Umidade (%): <input type=\"text\" name=\"humidity\" value=\"" + String(humiditySetpoint) + "\"><br>";
  html += "Tol. Temperatura (C): <input type=\"text\" name=\"temperatureTolerance\" value=\"" + String(temperatureTolerance) + "\"><br>";
  html += "Tol. Umidade (%): <input type=\"text\" name=\"humidityTolerance\" value=\"" + String(humidityTolerance) + "\"><br>";
  html += "<input type=\"submit\" value=\"Atualizar\">";
  html += "</form>";

  // Adicionando o campo para alterar o IP do servidor
  html += "<h2>Atualizar IP do Servidor:</h2>";
  html += "<form action=\"/set-ip\" method=\"post\">";
  html += "IP do Servidor: <input type=\"text\" name=\"ip\" value=\"" + serverIP + "\"><br>";
  html += "<input type=\"submit\" value=\"Atualizar IP\">";
  html += "</form>";

  // Adicionando botões para ligar/desligar os relés
  html += "<h2>Controle dos relés:</h2>";
  html += "<button onclick=\"toggleLED()\">" + String(ledState ? "Desligar" : "Ligar") + " LED</button><br>";
  html += "<button onclick=\"toggleExhaust()\">" + String(exhaustFanState ? "Desligar" : "Ligar") + " Exaustor</button><br>";

  // Adicionando botões para ligar/desligar o backlight do LCD
  html += "<h2>Controle do Backlight do LCD:</h2>";
  html += "<button onclick=\"toggleLCDBacklight()\">Ligar/Desligar Backlight</button><br>";

  html += "<h2>Estado dos relés:</h2>";
  html += "LED: " + String(ledState) + "<br>";
  html += "Humidificador: " + String(humidifierState) + "<br>";
  html += "Ventilador: " + String(fanState) + "<br>";
  html += "Exaustor: " + String(exhaustFanState) + "<br>";
  html += "<script>";
  html += "function toggleLED() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/led/' + (" + String(ledState) + " ? 'off' : 'on'), true);";
  html += "  xhr.send();";
  html += "  location.reload();"; // Atualiza a página após ligar/desligar o relé
  html += "}";
  html += "function toggleExhaust() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/exhaust/' + (" + String(exhaustFanState) + " ? 'off' : 'on'), true);";
  html += "  xhr.send();";
  html += "  location.reload();"; // Atualiza a página após ligar/desligar o relé
  html += "}";
  html += "function toggleLCDBacklight() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/lcd/backlight/on', true);";
  html += "  xhr.send();";
  html += "}";
  html += "</script></body></html>";

  server.send(200, "text/html", html);
}

void handleSetpoints() {
  if (server.hasArg("temperature") && server.hasArg("humidity") && server.hasArg("temperatureTolerance") && server.hasArg("humidityTolerance")) {
    temperatureSetpoint = server.arg("temperature").toFloat();
    humiditySetpoint = server.arg("humidity").toFloat();
    temperatureTolerance = server.arg("temperatureTolerance").toFloat();
    humidityTolerance = server.arg("humidityTolerance").toFloat();

    
    // Após atualizar os setpoints, salve-os na EEPROM
    writeFloatToEEPROM(EEPROM_ADDRESS_TEMPERATURE_SETPOINT, temperatureSetpoint);
    writeFloatToEEPROM(EEPROM_ADDRESS_HUMIDITY_SETPOINT, humiditySetpoint);
  // Escreva o valor de controle na EEPROM
    EEPROM.put(EEPROM_ADDRESS_CONTROL, CONTROL_VALUE);
    EEPROM.commit(); // Necessário para salvar os dados na EEPROM

    // Enviar resposta HTTP com o código JavaScript para exibir o popup
    String popupScript = "<script>alert('Setpoints e tolerâncias atualizados com sucesso!');";
    popupScript += "window.location='/';</script>"; // Redirecionar para a página principal após clicar em OK

    server.send(200, "text/html", popupScript);
  } else {
    server.send(400, "text/plain", "Parâmetros ausentes para atualização dos setpoints.");
  }
}

// Função para atualizar o IP do servidor
void handleSetIP() {
  if (server.hasArg("ip")) {
    serverIP = server.arg("ip");

    // Salva o novo IP na EEPROM
    writeStringToEEPROM(EEPROM_ADDRESS_IP, serverIP, 50); // 50 é o comprimento máximo da string

    // Enviar resposta HTTP com o código JavaScript para exibir o popup
    String popupScript = "<script>alert('IP atualizado com sucesso!');";
    popupScript += "window.location='/';</script>"; // Redirecionar para a página principal após clicar em OK

    server.send(200, "text/html", popupScript);
  } else {
    server.send(400, "text/plain", "Parâmetro de IP ausente.");
  }
}

// Função para enviar os dados de temperatura e umidade
void sendData() {
  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  if (!isnan(temperature) && !isnan(humidity)) {
    HTTPClient http;
    String serverPath = "http://" + serverIP + "/smartgrowtent/api/grow-values?temperature=" + String(temperature) + "&humidity=" + String(humidity);
    
    http.begin(serverPath.c_str());
    int httpResponseCode = http.POST(""); // Faz o POST com o corpo vazio
    Serial.print("chamada post para web server chart: ");
    Serial.println(serverPath);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erro na chamada HTTP POST: ");

      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    
    http.end(); // Fecha a conexão
  } else {
    Serial.println("Erro ao ler os valores do sensor.");
  }
}

void debug(boolean debug){
  if(debug){
    Serial.print("estado do rele LED: ");
    Serial.println(PIN_LED_RELAY);
    Serial.print("estado do rele Humidificador: ");
    Serial.println(PIN_HUMIDIFIER_RELAY);
    Serial.print("estado do rele Velilação: ");
    Serial.println(PIN_FAN_RELAY);
    Serial.print("estado do rele Exaustor: ");
    Serial.println(PIN_EXHAUST_FAN_RELAY);
  }
}
