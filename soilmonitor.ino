#define BLYNK_TEMPLATE_ID "TMPL3ylp2ltVi"
#define BLYNK_TEMPLATE_NAME "PLANT MONITER"
#define BLYNK_AUTH_TOKEN "OTet4G7fpxPYDYAGn-YCIXKTlCNv_L87"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] = "OTet4G7fpxPYDYAGn-YCIXKTlCNv_L87";  // Enter your Blynk Auth token
char ssid[] = "sanyanth";         // Enter your WIFI SSID
char pass[] = "toybase@132";     // Enter your WIFI Password

DHT dht(14, DHT11);  // DHT11 Temperature Sensor connected to D4
BlynkTimer timer;

#define soil A0  // A0 Soil Moisture Sensor

void setup() {
  Serial.begin(9600);
  
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();
  
  timer.setInterval(100L, soilMoistureSensor);
  timer.setInterval(100L, DHT11sensor);
}

// Get the DHT11 sensor values
void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
}

// Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(soil);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;

  Blynk.virtualWrite(V3, value);
}

void loop() {
  Blynk.run();  // Run the Blynk library
  timer.run();  // Run the Blynk timer
}
