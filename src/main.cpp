#ifdef ARDUINO_UNOWIFIR4 // If using UNO WiFi R4 change the WiFi lib (better compatibility)
#include "WiFiS3.h"
#else
#include "WiFi.h"
#endif
#include <Wire.h>
#include "wManager.h"

// If not ESP32-S3 define LED pin.
#ifndef LED_BUILTIN
#define LED_BUILTIN 22 // For WeMos Lolin32 Lite
#endif

int led = LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
WiFiClient client;
#if defined(LOADCELLS) || defined(LED_SCORE_BAR)
uint8_t bags = 0;
bool statusSent = false;
uint8_t bagsSent = 0;
uint8_t assumedBagsSent = 0;
bool shouldTare = false;
#endif

#ifdef LED_SCORE_BAR
bool LED_Status = false;
#include <Adafruit_NeoPixel.h>
// LED Data pin from LED_SCORE_BAR platormio.ini definition
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_SIZE, LED_SCORE_BAR, NEO_GRB + NEO_KHZ800);
uint32_t teamColor = 0;

void powerOffLeds()
{
  strip.clear();
  strip.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait)
{
  strip.setBrightness(100);
  for (int j = 0; j < 10; j++)
  { // do 10 cycles of chasing
    for (int q = 0; q < 3; q++)
    {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        if (c > 0)
        {
          strip.setPixelColor(i + q, c);
        }
        else
        {
          strip.setPixelColor(i + q, Wheel(random(0, 255))); // turn every third pixel on
        }
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0); // turn every third pixel off
      }
    }
  }
  powerOffLeds();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait)
{
  for (uint16_t i = 1; i < strip.numPixels() - 1; i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void setLEDScore(int score)
{

  if (score > bagsSent)
  {
    if (assumedBagsSent <= 5)
    {
      theaterChase(strip.Color(50, 255, 0), 20);
      assumedBagsSent++;
    }
  }
  else
  {
    assumedBagsSent = 0;
  }

  if (score >= 4)
  {
    theaterChase(teamColor, 40);
  }

  // Map bags to the LED_SIZE (how many LEDs per bag)
  int scoreLED = (int)map(score, 0, 4, 1, LED_SIZE - 1);

  powerOffLeds();
  for (size_t i = 1; i < scoreLED; i++)
  {
    strip.setPixelColor(i, teamColor); // Score LEDs
  }
  if (LED_Status)
  { // Status LEDs 0 and LED_SIZE
    strip.setPixelColor(0, strip.Color(0, 255, 0));
    strip.setPixelColor(LED_SIZE - 1, strip.Color(0, 255, 0));
  }
  else
  {
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    strip.setPixelColor(LED_SIZE - 1, strip.Color(255, 0, 0));
  }
  strip.show();
}

void init_LEDS()
{
  strip.begin();
  strip.show();
  LED_Status = true;
  if (TEAM_COLOR)
  {
    teamColor = strip.Color(255, 0, 0);
  }
  else
  {
    teamColor = strip.Color(0, 0, 255);
  }
  theaterChase(teamColor, 30);
}
#endif

// If using VL53L5CX LIDAR
#ifdef VL53L5CX
#include <SparkFun_VL53L5CX_Library.h>
SparkFun_VL53L5CX mySensor;
void init_VL53L5CX()
{
  // Initialize I2C communication
  Wire.begin();  // Initialize default I2C bus (Wire) - Used for soldered connections; I am not using this
  Wire1.begin(); // Initialize second I2C bus (Wire1) - Needed for Arduino Uno R4 with Qwicc cables

  // Initialize the VL53L5CX sensor on Wire1 with the default I2C address 0x29
  if (!mySensor.begin(0x29, Wire1))
  { // Use Wire1 for Qwiic connections
    // Use the following line instead if you're using a soldered connection or a
    // microcontroller that unlike the Uno R4 doesn't get confused about Quicc cables lol
    // if (!mySensor.begin()) {
    Serial.println("Failed to communicate with VL53L5CX on Wire1. Check wiring.");
    while (1)
      ; // Halt the program if sensor initialization fails
  }

  Serial.println("VL53L5CX detected on Wire1!");

  mySensor.setResolution(8 * 8); // Set the sensor resolution to 8x8
  mySensor.startRanging();       // Start measuring distance
}

void readSensor(int distances[24])
{
  VL53L5CX_ResultsData results;

  for (int i = 0; i < 24; i++)
  {
    distances[i] = -1;
  }

  if (mySensor.isDataReady())
  {
    mySensor.getRangingData(&results);

    // Key zones you want to track, don't need the entire grid of 64 data points
    int keyZones[24] = {0, 3, 7, 8, 12, 15, 16, 19, 23, 24, 28, 31, 32, 35, 39, 40, 44, 47, 48, 51, 55, 56, 60, 63};

    for (int i = 0; i < 24; i++)
    {
      distances[i] = results.distance_mm[keyZones[i]] / 10; // Convert to cm
    }
  }
}
#endif

// If using load cells
#ifdef LOADCELLS
// Uncomment to slow down the ESP
#include "soc/rtc.h"
#include "HX711.h"
HX711 myScale;

uint8_t dataPin = 16;
uint8_t clockPin = 4;

void calibrate()
{
  Serial.println("\n\nCALIBRATION\n===========");
  Serial.println("remove all weight from the loadcell");
  //  flush Serial input
  while (Serial.available())
    Serial.read();

  Serial.println("and press enter\n");
  while (Serial.available() == 0)
    ;

  Serial.println("Determine zero weight offset");
  myScale.tare(20); // average 20 measurements.
  uint32_t offset = myScale.get_offset();

  Serial.print("OFFSET: ");
  Serial.println(offset);
  Serial.println();

  Serial.println("place a weight on the loadcell");
  //  flush Serial input
  while (Serial.available())
    Serial.read();
  Serial.println("enter the weight in (whole) grams and press enter");
  uint32_t weight = 0;
  while (Serial.peek() != '\n')
  {
    if (Serial.available())
    {
      char ch = Serial.read();
      if (isdigit(ch))
      {
        weight *= 10;
        weight = weight + (ch - '0');
      }
    }
  }
  Serial.print("WEIGHT: ");
  Serial.println(weight);
  myScale.calibrate_scale(weight, 20);
  float scale = myScale.get_scale();
  Serial.print("SCALE:  ");
  Serial.println(scale, 6);
  Serial.print("\nuse scale.set_offset(");
  Serial.print(offset);
  Serial.print("); and scale.set_scale(");
  Serial.print(scale, 6);
  Serial.print(");\n");
  Serial.println("in the init_loadcells() of your project");
  Serial.println("\n\n");
}
void recalibrateScale()
{
  bags = 0;
  LED_Status = false;
#ifdef LED_SCORE_BAR
  setLEDScore(bags);
#endif
  myScale.tare(20);
  LED_Status = true;
}

void readBagsFromScale(void *task_id)
{
  unsigned long currentMillis = 0;
  unsigned int scale_id = (uint32_t)task_id;
  Serial.printf("[SCALE]  %d  Started scale monitor Task!\n", scale_id);

  while (1)
  {
    if (millis() > currentMillis + 500)
    {
      currentMillis = millis();
      /* float weightSum = 0.0;
      for (size_t i = 0; i < 10; i++)
      {
        weightSum += myScale.get_units(1);
        delay(5);
      }
      int weight =(int)(weightSum/10); */
      int weight = (int)myScale.get_units(5);
      if (weight < -20)
      {
        Serial.println("Out of scale recalibrating...");
        recalibrateScale();
      }
      else if (shouldTare)
      {
        Serial.println("Client requested tare...");
        recalibrateScale();
        shouldTare = false;
      }
      else
      {
        int bagsEstimated = (int)((weight / (BAGWEIGHT - TOLERANCE)) + 0.6);
        float bagmin = bagsEstimated * BAGWEIGHT - bagsEstimated * TOLERANCE;
        float bagmax = bagsEstimated * BAGWEIGHT + bagsEstimated * TOLERANCE;
        if (weight >= bagmin && weight <= bagmax)
        {
          bags = bagsEstimated;
        }
        else
        {
          bags = weight / BAGWEIGHT + 0.5;
        }
        Serial.printf("W:%d(%.2f:%.2f) Bags:%d Bags Sent:%d\n", weight, bagmin, bagmax, bags, bagsSent);
        // Restart game; bag compartment is empty
        if ((bags == 0) && (bagsSent > 0))
        {
          Serial.println("Recalibrate for a new game.");
          recalibrateScale();
        }
      }
// Show bar status;
#ifdef LED_SCORE_BAR
      setLEDScore(bags);
#endif
    }
  }
}

void init_loadcells()
{
  Serial.println("Starting Load cells.");

  // Slow down ESP to improve accuracy.
  // rtc_cpu_freq_config_t config;
  // rtc_clk_cpu_freq_get_config(&config);
  // rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_160M, &config);
  // rtc_clk_cpu_freq_set_config_fast(&config);

  myScale.begin(dataPin, clockPin, true);

  // Use to calibrate your load cell; comment and update the code after.
  // calibrate();
// Update offset and scale after call calibrate()
// Each scale has to be calibrated
#if (TEAM_COLOR == 1)
  myScale.set_offset(525338);    // Red
  myScale.set_scale(-21.905506); // Red
#else
  myScale.set_offset(537889);    // Blue
  myScale.set_scale(-21.596563); // Blue
#endif
  myScale.tare(20);

  char *name = (char *)malloc(32);
  sprintf(name, "(%s)", "scaleMonitor");
  BaseType_t res1 = xTaskCreatePinnedToCore(readBagsFromScale, "scaleMonitor", 10000, (void *)name, 1, NULL, 1);
}

// int cheat = 0;
void readSensor(int distances[24])
{
  if (bags == bagsSent)
  {
    // Send 15 as a value to fill frontend player.baselineValues[].
    // Bag is detected if the baseline[] - sensorValues[] > threshold (default:6)
    std::fill(distances, distances + 24, 15);
    Serial.println("Sending baseline values.");
    /* cheat++;
    if (cheat>10) {
      bagsSent++;
      cheat=0;
    } */
  }
  else
  {
    std::fill(distances, distances + 24, bags);
    Serial.printf("Sending bean bags information. %d Beans Bags", bags);
    bagsSent = bags;
  }
}

#endif

void printWiFiStatus()
{
  // Serial.println("Connected to WiFi!");
  IPAddress ip = WiFi.localIP();
  Serial.print("Your API endpoint is: http://");
  Serial.print(ip);
  Serial.println("/readSensor");
}

void printHeaders()
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println("Access-Control-Allow-Origin: *"); // Allows any origin
  client.println();
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ; // Wait for serial port to connect (only needed for native USB)
  }
  Serial.println("********* Starting Program ********* ");

// Start LIDAR
#ifdef VL53L5CX
  init_VL53L5CX();
#endif

#ifdef LOADCELLS
  init_loadcells();
#endif

  pinMode(led, OUTPUT);

  init_WifiManager();

  // Start the web server on port 80
  server.begin();
  printWiFiStatus();

#ifdef LED_SCORE_BAR
  init_LEDS();
#endif
}

void loop()
{

  // Listen for incoming clients
  client = server.available();
  unsigned long clientConnectedTime = millis();
  int distances[24] = {0};

  if (client)
  {
    String currentLine = ""; // Make a String to hold incoming data from the client
    wifiManagerProcess();

    while (client.connected())
    { // Loop while the client's connected

      // Close the connection if they've been connected for more than 1 second
      if (millis() - clientConnectedTime > 1000)
      {
        client.stop();
        break;
      }

      delayMicroseconds(10); // Prevent the loop from running too fast

      if (client.available())
      {                         // If there's bytes to read from the client,
        char c = client.read(); // Read a byte
        // Serial.write(c); // Uncomment to print incoming characters to Serial

        if (c == '\n')
        { // If the byte is a newline character
          if (currentLine.length() == 0)
          {
            // End of HTTP request, send a response
            break;
          }
          else
          { // If you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // If you got anything else but a carriage return character,
          currentLine += c; // Add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H", "GET /L", or "GET /readSensor"

        if (currentLine.endsWith("GET /H"))
        {
          Serial.println("Received /H request.");
          printHeaders();
          client.println("{\"status\":\"on\"}");
          digitalWrite(led, HIGH);
          Serial.println("LED turned on.");
#ifdef LED_SCORE_BAR
          LED_Status = true;
#endif
        }

        if (currentLine.endsWith("GET /L"))
        {
          Serial.println("Received /L request.");
          printHeaders();
          client.println("{\"status\":\"off\"}");
          digitalWrite(led, LOW);
          Serial.println("LED turned off.");
#ifdef LED_SCORE_BAR
          LED_Status = false;
#endif
        }

        if (currentLine.endsWith("GET /readSensor"))
        {
          Serial.println("Received request.");
          printHeaders();
          readSensor(distances);

          client.print("[");
          for (int i = 0; i < 24; i++)
          {
            client.print(distances[i]);

            if (i < 23)
            {
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