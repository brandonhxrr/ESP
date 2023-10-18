#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const char* ssid = "ESP";
const char* password = "uwu152537";

WiFiServer server(80);
int potPin = 34;
int irPin = 32;
int hallPin = 35;
OneWire oneWire(27);
DallasTemperature sensors(&oneWire);

unsigned long tiempoAnterior = 0;
const unsigned long intervalo = 2000;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  server.begin();
  Serial.println("Dispositivo conectado.");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Servidor iniciado.");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error al inicializar la pantalla OLED"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("ELECTRONICA RPM");
  display.display();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Cliente conectado.");

    unsigned long tiempoActual = millis();
    int irValue = analogRead(irPin);
    float denValue = obtenerDensidad();
    float tmpValue = obtenerTemperatura();
    float vltValue = map(analogRead(potPin), 0, 4095, 0, 3300) / 1000.0;

    String response = "{";
    response += "\"rpm\":" + String(irValue) + ",";
    response += "\"capacitancia\":" + String(denValue) + ",";
    response += "\"temperatura\":" + String(tmpValue) + ",";
    response += "\"voltaje\":" + String(vltValue, 2);
    response += "}";

    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Connection: close\r\n");
    client.print("Content-Length:" + String(response.length()) + "\r\n");
    client.print("\r\n");
    client.print(response);

    Serial.println(obtenerDensidad());

    if(tiempoActual - tiempoAnterior >= intervalo){
      actualizarDisplays(irValue, denValue, tmpValue, vltValue);
      tiempoAnterior = tiempoActual;
    }
  }

  delay(100);
}

float obtenerDensidad() {
  int hallValue = analogRead(hallPin);
  float densidadCampoMagnetico = map(hallValue, 0, 4095, 0, 100);
  return densidadCampoMagnetico;
}

float obtenerTemperatura() {
  sensors.requestTemperatures();
  float temperatura = sensors.getTempCByIndex(0);
  if (temperatura != -127.00) {
    return temperatura;
  } else {
    return 0.0;
  }
}

void actualizarDisplays(int irValue, float denValue, float tmpValue, float vltValue){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 19);
    display.println("RPM:");
    display.setCursor(80, 19);
    display.println(String(irValue));
    display.display();
    delay(1000);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 19);
    display.println("TMP:");
    display.setCursor(80, 19);
    display.println(String(tmpValue,2));
    display.display();
    delay(1000);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 19);
    display.println("HALL:");
    display.setCursor(80, 19);
    display.println(String(denValue,2));
    display.display();
    delay(1000);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 19);
    display.println("VLT:");
    display.setCursor(80, 19);
    display.println(String(vltValue, 2));
    display.display();
    delay(1000);
}