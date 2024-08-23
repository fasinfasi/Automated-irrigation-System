// Blynk Template ID for the project
#define BLYNK_TEMPLATE_ID "TMPL3Cu0x8NkZ"
// Name of the Blynk template
#define BLYNK_TEMPLATE_NAME "PLANT MONITOR"
// Blynk Auth Token for connecting to the Blynk platform
#define BLYNK_AUTH_TOKEN "vD8tKMQNmUmZltAqV8fLH5eJMYxu1Kn2"

#include <WiFi.h>                  // Include library for WiFi connectivity
#include <BlynkSimpleEsp32.h>       // Include Blynk library for ESP32
#include <DHT.h>                    // Include DHT sensor library for temperature and humidity
#include <Wire.h>                   // Include Wire library for I2C communication
#include <Adafruit_GFX.h>           // Include Adafruit GFX library for graphics on OLED display
#include <Adafruit_SSD1306.h>       // Include Adafruit SSD1306 library for OLED display

// Below define sensor's pins and types
#define DHTPIN 18
// Define the type of DHT sensor (DHT11)
#define DHTTYPE DHT11               
#define SOIL_PIN_1 34
#define RAIN_PIN 35
#define RELAY_PIN 5

// Define OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// WiFi credentials for connecting to the network
char auth[] = "vD8tKMQNmUmZltAqV8fLH5eJMYxu1Kn2";  // Blynk Auth Token
char ssid[] = "agroassistwifi";                    // WiFi SSID (network name)
char pass[] = "agroassist@132";                    // WiFi password

// Create display object for OLED screen
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create DHT object for temperature and humidity sensor
DHT dht(DHTPIN, DHTTYPE);

String Status = "";  // String to hold the status of soil moisture ("Dry soil" or "Wet soil")

void setup() {
  Serial.begin(115200);  // Initialize serial communication at a baud rate of 115200

  // Set pin modes
  pinMode(RELAY_PIN, OUTPUT);         // Set relay pin as output (for controlling motor pump)
  pinMode(RAIN_PIN, INPUT_PULLUP);    // Set rain sensor pin as input with pull-up resistor

  dht.begin();  // Initialize the DHT sensor

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));  // Display an error if the OLED fails to initialize
    for (;;) ;  // Stop the program if the display initialization fails
  }

  // Display scrolling animation on the OLED screen
  display.clearDisplay();
  display.setTextSize(2);                   // Set text size
  display.setTextColor(SSD1306_WHITE);      // Set text color to white
  display.setCursor(0, 0);                  // Set cursor position
  display.println("AgroAssist");            // Display project name
  display.println("System");                // Display project system
  display.display();                        // Display the text on the screen
  delay(2000);                              // Wait for 2 seconds

  // Clear the display after the animation
  display.clearDisplay();
  display.setTextSize(1);                   // Set text size
  display.setTextColor(SSD1306_WHITE);      // Set text color to white
  display.setCursor(0, 0);                  // Reset cursor position
  display.display();                        // Update the display
  delay(3000);                              // Wait for 3 seconds

  // Connect to the Blynk platform using WiFi credentials
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();  // Run the Blynk background tasks

  // Read humidity and temperature from DHT sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if the readings are valid, otherwise print an error
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;  // Exit the function if the readings are invalid
  }

  // Print the sensor readings to the serial monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  // Read soil moisture level from the analog pin and map it to a percentage
  int soilMoisture = analogRead(SOIL_PIN_1);
  int moisturePercentage = map(soilMoisture, 0, 4095, 100, 0);

  // Print the soil moisture percentage to the serial monitor
  Serial.print("Soil Moisture: ");
  Serial.print(moisturePercentage);
  Serial.println("%");

  // Read the state of the rain sensor
  bool isRaining = !digitalRead(RAIN_PIN);

  // Print the rain sensor status to the serial monitor
  Serial.print("Rain Sensor: ");
  Serial.println(isRaining ? "Raining" : "Not Raining");

  // Clear the OLED display and update it with sensor data
  display.clearDisplay();
  display.setCursor(7, 0);
  display.print("AGROASSIST MONITORING...");
  display.setCursor(0, 13);
  display.print("Humidity: ");
  display.print(humidity);
  display.println("%");
  display.print("Temperature: ");
  display.print(temperature);
  display.println(" C");
  display.print("Soil Moisture: ");
  display.print(moisturePercentage);
  display.println("%");
  display.print("Rain Sensor: ");
  display.println(isRaining ? "Raining" : "Not Raining");
  display.display();

  // Control the relay (motor pump) based on soil moisture and rain sensor
  if (moisturePercentage < 30 && !isRaining) {
    digitalWrite(RELAY_PIN, HIGH);  // Turn the motor pump on if soil is dry and it's not raining
  } else {
    digitalWrite(RELAY_PIN, LOW);   // Turn the motor pump off if soil is wet or it's raining
  }

  // Send sensor data to Blynk for remote monitoring
  Blynk.virtualWrite(V1, temperature);       // Send temperature to virtual pin V1
  Blynk.virtualWrite(V2, humidity);          // Send humidity to virtual pin V2
  Blynk.virtualWrite(V3, moisturePercentage);// Send soil moisture to virtual pin V3

  // Update Blynk LED widget based on rain status
  Blynk.virtualWrite(V4, isRaining ? 255 : 0);  // Turn on LED if raining, else turn off

  // Update the status of soil moisture ("Dry soil" or "Wet soil")
  if (moisturePercentage < 30) {
    Status = "Dry soil";
  } else {
    Status = "Wet soil";
  }

  // Send soil moisture status to Blynk for display
  Blynk.virtualWrite(V5, Status);

  delay(1000);  // Wait for 1 second before the next loop iteration
}
