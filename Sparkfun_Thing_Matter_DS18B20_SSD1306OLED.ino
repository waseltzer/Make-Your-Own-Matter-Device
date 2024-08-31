/*
   Source: https://github.com/SiliconLabs/arduino/tree/main/libraries/Matter/examples/matter_temp_sensor
   Modified by Wayne Seltzer Sep 2024
   - replaced CPU temperature sensor with external DS18B20 
   - DS18B20 based on the "simple" example in the "Dallas Temperature" library
     https://github.com/milesburton/Arduino-Temperature-Control-Library/tree/master/examples/Simple
   - need to install OneWire library that works with the SparkFun Thing Plus MGM240P:
     https://github.com/silabs-bozont/OneWire
     Download the .zip file
     Include Library … Add .zip library
   - SSD1306 OLED - using the u8g2 library
   ---
   Matter temperature sensor example

   The example shows how to create a temperature sensor with the Arduino Matter API.

   The example creates a Matter temperature sensor device and publishes the DS18B20 temperature
   The device has to be commissioned to a Matter hub first.

   Compatible boards:
   - Arduino Nano Matter
   - SparkFun Thing Plus MGM240P
   - xG24 Explorer Kit
   - xG24 Dev Kit

   Author: Tamas Jozsi (Silicon Labs)
 */
#include <Matter.h>
#include <MatterTemperature.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Arduino.h>
#include <U8x8lib.h>  // SSD1306 OLED

MatterTemperature matter_temp_sensor;

// Data wire is plugged into
#define ONE_WIRE_BUS D10
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// SSD1306 0.96" OLED using I2C
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  Matter.begin();
  matter_temp_sensor.begin();

  // Start up the Dallas Temperature library
  sensors.begin();

  // SSD1306 OLED library startup
  u8x8.begin();
  u8x8.setPowerSave(0);

  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 0, "Matter temp");
  Serial.println("Matter temperature sensor");
  delay(1000);

  if (!Matter.isDeviceCommissioned()) {
    Serial.println("Matter device is not commissioned");
    Serial.println("Commission it to your Matter hub with the manual pairing code or QR code");
    Serial.printf("Manual pairing code: %s\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR code URL: %s\n", Matter.getOnboardingQRCodeUrl().c_str());
    u8x8.clear();
    u8x8.drawString(0, 0, "Commissioning");
  }
  while (!Matter.isDeviceCommissioned()) {
    delay(200);
  }

  Serial.println("Waiting for Thread network...");
  while (!Matter.isDeviceThreadConnected()) {
    delay(200);
  }
  Serial.println("Connected to Thread network");

  Serial.println("Waiting for Matter device discovery...");
  while (!matter_temp_sensor.is_online()) {
    u8x8.clear();
    u8x8.drawString(0, 0, "Matter");
    u8x8.drawString(0, 1, "Discovery...");
    delay(200);
  }
  Serial.println("Matter device is now online");
  u8x8.clear();
  u8x8.drawString(0, 0, "online");
}

char buf[50];

void loop() {
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all DS18B20 devices on the OneWire bus
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();  // Send the command to get temperatures
  //Serial.println("DONE");
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(0);
  float tempF = (tempC * 1.8) + 32;
  float prevtempF = tempF;

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    //   Serial.print("Temperature: ");
    //   Serial.println(tempC);
  } else {
    Serial.println("Error: Could not read temperature data");
  }

  matter_temp_sensor.set_measured_value_celsius(tempC);
  Serial.printf("Temperature: %.02f C\n", tempC);

  if (prevtempF != tempF) {  //update the display if the temperature has changed
    // Display the temperature on the OLED
    u8x8.setFont(u8x8_font_inr33_3x6_r);
    u8x8.clear();
    dtostrf(tempF, 3, 0, buf);
    u8x8.drawString(1, 1, buf);
    u8x8.drawString(28, 1, "F");
  }

  delay(2000);
}
