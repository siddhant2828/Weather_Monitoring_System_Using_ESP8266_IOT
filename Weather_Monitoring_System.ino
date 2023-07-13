#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

// Replace with your network credentials
const char* ssid = "Redmi10";
const char* password = "dsid111111";

// Replace with your Thingspeak channel details
unsigned long channelID = 2166825;
const char* writeAPIKey = "GRQHLPJPS95WYWXF";

Adafruit_BMP085 bmp;

WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
 
  Serial.begin(115200);
  delay(10);
  dht.begin();

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client);
  bmp.begin();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float pressure = bmp.readPressure() ;// 100.0F;
  float altitude = bmp.readAltitude();

   if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
   WiFiClient client;
  HTTPClient http;
  String url = "http://api.thingspeak.com/update?api_key=" + String(THINGSPEAK_API_KEY) +
               "&field1=" + String(temperature) + "&field2=" + String(humidity);

  Serial.println(url);

  http.begin(client, url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.println("Error sending data to ThingSpeak");
  }

  http.end();
  

  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Altitude = ");
  Serial.print(altitude);
  Serial.println(" m");

  ThingSpeak.setField(1, pressure);
  ThingSpeak.setField(2, altitude);

  int httpCode = ThingSpeak.writeFields(channelID, writeAPIKey);

  if (httpCode == 200) {
    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.println("Error sending data to ThingSpeak");
  }

  delay(20000); // Send data every 20 seconds
}
