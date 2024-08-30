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

MatterTemperature matter_temp_sensor;

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into 
#define ONE_WIRE_BUS D10
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup()
{
  Serial.begin(115200);
  Matter.begin();
  matter_temp_sensor.begin();


  // Start up the Dallas Temperature library
  sensors.begin();

  Serial.println("Matter temperature sensor");

  if (!Matter.isDeviceCommissioned()) {
    Serial.println("Matter device is not commissioned");
    Serial.println("Commission it to your Matter hub with the manual pairing code or QR code");
    Serial.printf("Manual pairing code: %s\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR code URL: %s\n", Matter.getOnboardingQRCodeUrl().c_str());
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
    delay(200);
  }
  Serial.println("Matter device is now online");
}

void loop()
{
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
 //   Serial.print("Temperature: ");
 //   Serial.println(tempC);
  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
  }

  matter_temp_sensor.set_measured_value_celsius(tempC);
  Serial.printf("Temperature: %.02f C\n", tempC);
  delay(2000);
}
