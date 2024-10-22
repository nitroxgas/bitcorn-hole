#ifdef ARDUINO_UNOWIFIR4 // If using UNO WiFi R4 change the WiFi lib (better compatibility)
  #include "WiFiS3.h"
#else
  #include "WiFi.h"
#endif
#include <Wire.h>
#include "wManager.h"

// If not ESP32-S3 define LED pin.
#ifndef LED_BUILTIN
  #define LED_BUILTIN 13;
#endif

int led =  LED_BUILTIN; 
int status = WL_IDLE_STATUS;
WiFiServer server(80);
WiFiClient client;


// If using VL53L5CX LIDAR
#ifdef VL53L5CX
#include <SparkFun_VL53L5CX_Library.h>
SparkFun_VL53L5CX mySensor; 
void init_VL53L5CX(){
// Initialize I2C communication
  Wire.begin();   // Initialize default I2C bus (Wire) - Used for soldered connections; I am not using this 
  Wire1.begin();  // Initialize second I2C bus (Wire1) - Needed for Arduino Uno R4 with Qwicc cables
  
  // Initialize the VL53L5CX sensor on Wire1 with the default I2C address 0x29
  if (!mySensor.begin(0x29, Wire1)) { // Use Wire1 for Qwiic connections
  // Use the following line instead if you're using a soldered connection or a 
  // microcontroller that unlike the Uno R4 doesn't get confused about Quicc cables lol
  // if (!mySensor.begin()) {
    Serial.println("Failed to communicate with VL53L5CX on Wire1. Check wiring.");
    while (1); // Halt the program if sensor initialization fails
  }

  Serial.println("VL53L5CX detected on Wire1!");
  
  mySensor.setResolution(8 * 8); // Set the sensor resolution to 8x8
  mySensor.startRanging(); // Start measuring distance
}

void readSensor(int distances[24]) {
  VL53L5CX_ResultsData results;

  for (int i = 0; i < 24; i++) {
    distances[i] = -1;
  }

  if (mySensor.isDataReady()) { 
    mySensor.getRangingData(&results); 

    // Key zones you want to track, don't need the entire grid of 64 data points
    int keyZones[24] = {0, 3, 7, 8, 12, 15, 16, 19, 23, 24, 28, 31, 32, 35, 39, 40, 44, 47, 48, 51, 55, 56, 60, 63};

    for (int i = 0; i < 24; i++) {
      distances[i] = results.distance_mm[keyZones[i]] / 10; // Convert to cm
    }
  }
}
#endif

// If using load cells
#ifdef LOADCELLS
void readSensor(int distances[24]) {

}
#endif

void printWiFiStatus() {
  Serial.println("Connected to WiFi!");
  IPAddress ip = WiFi.localIP();
  Serial.print("Your API endpoint is: http://");
  Serial.print(ip);
  Serial.println("/readSensor");
}

void printHeaders() {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println("Access-Control-Allow-Origin: *"); // Allows any origin
  client.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect (only needed for native USB)
  }
  Serial.println("********* Starting Program ********* ");

  init_WifiManager();
  
  // Start LIDAR
  #ifdef VL53L5CX
    init_VL53L5CX();
  #endif

  pinMode(led, OUTPUT);      

  // Start the web server on port 80
  server.begin();
  printWiFiStatus();
}

void loop() {

  // Listen for incoming clients
  client = server.available(); 
  unsigned long clientConnectedTime = millis();

  if (client) { 
    String currentLine = ""; // Make a String to hold incoming data from the client

    while (client.connected()) { // Loop while the client's connected

      // Close the connection if they've been connected for more than 1 second
      if (millis() - clientConnectedTime > 1000) {
        client.stop();
        break; 
      }

      delayMicroseconds(10); // Prevent the loop from running too fast

      if (client.available()) { // If there's bytes to read from the client,
        char c = client.read(); // Read a byte
        // Serial.write(c); // Uncomment to print incoming characters to Serial

        if (c == '\n') { // If the byte is a newline character
          if (currentLine.length() == 0) {
            // End of HTTP request, send a response
            break;
          } else { // If you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') { // If you got anything else but a carriage return character,
          currentLine += c; // Add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H", "GET /L", or "GET /readSensor"

        if (currentLine.endsWith("GET /H")) {
          Serial.println("Received /H request.");
          printHeaders();
          client.println("{\"status\":\"on\"}");
          digitalWrite(led, HIGH); 
          Serial.println("LED turned on.");
        }

        if (currentLine.endsWith("GET /L")) {
          Serial.println("Received /L request.");
          printHeaders();
          client.println("{\"status\":\"off\"}");
          digitalWrite(led, LOW); 
          Serial.println("LED turned off.");
        }

        if (currentLine.endsWith("GET /readSensor")) {
          printHeaders(); 
          int distances[24];
          readSensor(distances);

          client.print("[");
          for (int i = 0; i < 24; i++) {
            client.print(distances[i]); 

            if (i < 23) { 
              client.print(",");
            }
          }
          client.print("]");
        }
      }
    }
    delay(1); // Short delay to ensure data is sent
    client.stop();
  }
}
