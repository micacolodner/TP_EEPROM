//GRUPO 2 5MB
//COLODNER, BOCCI, TOLEDO, BRAVAR 

#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Preferences.h>

//defino estados
#define PANTALLA1 1
#define CONFIRM_PANTALLA1 2
#define PANTALLA2 3
#define CONFIRM_PANTALLA2 4
#define GUARDADO 5
#define SUMA 6
#define RESTA 7


//configuación de la pantalla
#define LONGITUD 128 // longitud
#define ALTURA 64 // altura
Adafruit_SSD1306 display(LONGITUD, ALTURA, &Wire, -1);

//dht
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//defino pines
#define SW1 34
int bot1;
#define SW2 35
int bot2;

//variables globales
float VU = 24.0; //valor umbral
int estadoActual = PANTALLA1;
unsigned long contando = 0;

Preferences VU_eeprom;



void setup() {
  Serial.begin(115200);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  dht.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  VU_eeprom.begin("valor-VU", true);
  VU = VU_eeprom.getFloat("umbral", 0);
  Serial.print("Umbral guardado: ");
  Serial.println(VU);
  VU_eeprom.end();
}


void loop() {

  float VA = dht.readTemperature(); //temperatura actual

  bot1 = digitalRead(SW1);
  bot2 = digitalRead(SW2);

  switch (estadoActual) {
    case PANTALLA1:
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);

      display.setCursor(0, 10);
      display.println("VA: ");
      display.setCursor(60, 10);
      display.print(VA);

      display.setCursor(0, 50);
      display.println("VU: ");
      display.setCursor(60, 50);
      display.print(VU);
  
      display.display();  

      if (bot1 == LOW) {
        contando = millis();
        estadoActual = CONFIRM_PANTALLA1;
      }
      break;

    case CONFIRM_PANTALLA1:

      if ((millis() - contando) >= 5000 && bot1 == HIGH) {
        contando = 0;
        estadoActual = PANTALLA2;
      }
      else {
        estadoActual = PANTALLA1;
      }
      break;
    
    case PANTALLA2:

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);

      display.setCursor(0, 10);
      display.println("NUEVO UMBRAL: ");
      display.setCursor(60, 10);
      display.print(VU);
      
      display.display();

      if (bot2 == LOW) {
        contando = millis();
        estadoActual = CONFIRM_PANTALLA2;
      }

      if (bot1 == LOW) {
        estadoActual = SUMA;
      }
    
      break;
    
    case CONFIRM_PANTALLA2:

      if ((millis() - contando) >= 5000 && bot2 == HIGH) {
        contando = 0;
        VU_eeprom.begin("valor-VU", false);
        VU_eeprom.putFloat("umbral", VU);
        VU_eeprom.end();
        Serial.print("nuevo umbral: ");
        Serial.println(VU);
        estadoActual = GUARDADO;  
      } else if ((millis() - contando) <= 5000 && bot2 == HIGH) {
          estadoActual = RESTA;
      }
      break;

      case GUARDADO:
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 20);
        display.println("GUARDADO");
        display.display();

        if ((millis() - contando) >= 2000) {
          estadoActual = PANTALLA1;
        }
        break;

    case SUMA:

      if (bot1 == HIGH) {
        VU += 1;
        estadoActual = PANTALLA2;
      }
      break;

    case RESTA:

      if (bot2 == HIGH) {
        VU -= 1;
        estadoActual = PANTALLA2;
      }
      break;
  }
