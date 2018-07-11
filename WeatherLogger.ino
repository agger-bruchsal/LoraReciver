#include <Adafruit_TSL2591.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <VEML6075.h>
#include <Wire.h>

const char* host = "WeatherStation";
#define ssid      "..."       // WiFi SSID
#define password  "..."  // WiFi password
#define SEALEVELPRESSURE_HPA 1013.25 
double humidity    = 0.0;  // Feuchtigkeit
double temperature = 0.0;  // Temperatur
double pressure    = 0.0;  // Druck
double gas         = 0.0;  // Gas/Luftqualitaet
double est_altitude= 0.0;  // Ungefaehre Hoehe
uint16_t luminosity;
float uv_index;
float uva_radiation;
float uvb_radiation;
Adafruit_BME680 bme;
Adafruit_TSL2591 tsl;
VEML6075 veml6075;

// Création des objets / create Objects
ESP8266WebServer server ( 80 );

String getPage(){
  String page = "{";
  page += "\"weatherdata\": {";
  page += "  \"temperature\": \"";
  page += temperature;
  page += "\",";
  page += "\"pressure\": \"";
  page += pressure;
  page += "\",";
  page += "\"humidity\": \"";
  page += humidity;
  page += "\",";
  page += "\"gas\": \"";
  page += gas;
  page += "\",";
  page += "\"lux\": \"";
  page += luminosity;
  page += "\",";
  page += "\"UVA\": \"";
  page += uva_radiation;
  page += "\",";
  page += "\"UVB\": \"";
  page += uvb_radiation;
  page += "\",";
  page += "\"UV_index\": \"";
  page += uv_index;
  page += "\"";
  page += "}";
  page += "}";
  return page;
}

void handleRoot(){ 
  if ( server.hasArg("power") ) {
    //handlePower();
  } else if ( server.hasArg("D6") ) {
    //handleBreeze();
  } else if ( server.hasArg("D7") ) {
    //handleSpeed();
  } else if ( server.hasArg("D8") ) {
    //handleD8();
  } else {
    server.send ( 200, "text/html", getPage() );
  }  
}

void setup() {
  Serial.begin ( 115200 );
  Wire.begin();                                 // I2C-Bus initialisieren GPIO2-Data GPIO0-Clock
  WiFi.begin ( ssid, password );
  //  Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 ); Serial.print ( "." );
  }
  //  WiFi connexion is OK
  Serial.println ( "" ); 
  Serial.print ( "Connected to " ); Serial.println ( ssid );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );
  /*return index page which is stored in serverIndex */
  server.on ( "/", handleRoot );
  server.begin();
  Serial.println ( "HTTP server started" );
   if (tsl.begin()) 
  {
    Serial.println(F("Found a TSL2591 sensor"));
  } 
  else 
  {
    Serial.println(F("No sensor found ... check your wiring?"));
    while (1);
  }
  if (bme.begin(0x76))
  {
    Serial.println(F("Found a BME680 sensor"));
  } 
  else 
  {
    Serial.println(F("No BME680 sensor found ... check your wiring?"));
    while (1);
  }
  if (veml6075.begin())
  {
    Serial.println(F("Found a veml6075 sensor"));
  } 
  else 
  {
    Serial.println(F("No veml6075 sensor found ... check your wiring?"));
    while (1);
  }
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  veml6075.poll();
  luminosity    = tsl.getLuminosity(TSL2591_VISIBLE);
  temperature   = bme.temperature;
  pressure      = bme.pressure / 100.0;
  humidity      = bme.humidity;
  gas           = bme.gas_resistance / 1000.0;
  est_altitude  = bme.readAltitude(SEALEVELPRESSURE_HPA);
  uva_radiation = veml6075.getUVA();
  uvb_radiation = veml6075.getUVB();
  uv_index      = veml6075.getUVIndex();
  Serial.print("Temperatur = ");
  Serial.print(temperature);
  Serial.println(" *C");
  
  Serial.print("Luftdruck = ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Feuchtigkeit = ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(gas);
  Serial.println(" KOhms");
  
  Serial.print(F("Luminosity: "));
  Serial.println(luminosity, DEC);

  Serial.print(F("UVA = "));
  Serial.println(uva_radiation, 2);

  Serial.print(F("UVB = "));
  Serial.println(uvb_radiation, 2);

  Serial.print(F("UV Index = "));
  Serial.println(uv_index, 1);
  delay(100);
}
