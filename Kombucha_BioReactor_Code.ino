/******************************************************************************
Kombucha Reactor using SparkFunTSL2561, SparkFun Si7021, and mini photocell - this code
is used for sensor setup within the reactor.  This combines code from sensor examples
within the library and uses an I2C bus for two of the sensors, while the mini photocell
uses it’s own ports on the Arduino.  Built for Arduino Uno R3.

Hardware Connections (Si7021):
(-) to GND
(+) to 3.3V (VCC)
CL to SCL
DA to SDA

Hardware Connections (TSL2561):
3V3 to 3.3V
GND to GND
SDA to SDA
SCL to SCL
(WARNING: do not connect 3V3 to 5V
or the sensor will be damaged!)

Hardware Connections (mini photocell):
Prong 1: 5v power through breadboard
Prong 2: A0 + 10 Ohm resistor + neutral (complete circuit) Please see this link for diagram:
https://www.circuito.io/app?components=512,9088,11021

These sensors are connected to a breadboard and all SDA connections are united and one single
wire returns to the SDA board.  The same goes for the 3v power, GND, and SCL connection:
Please note: the code will not function if your device addresses are not the same - see
documentation on Controller Reader here for more details:
https://docs.arduino.cc/learn/communication/wire
Written by Joshua Coffie and Mary Etta West , CU Boulder ATLAS 2022
*******************************************************************************/

#include <SparkFun_Si7021_Breakout_Library.h>
#include <SparkFunTSL2561.h>
#include <Wire.h>


SFE_TSL2561 light; //Light sensor object
Weather sensor; //Si7021 sensor object

float humidity = 0;
float tempf = 0;
boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
unsigned char ID;

// photocell
const int analogInPin = A0;  
const int analogOutPin = 11; 
int sensorValue = 0;        
int pitch = 0;        

void setup() {
  Serial.begin(9600);
  Serial.println("Let's hope this works");

  light.begin(); //Start light sensor
  sensor.begin(); //Start humidity sensor

  if (light.getID(ID))
  {
    Serial.print("Got factory ID: 0X");
    Serial.print(ID, HEX);
    Serial.println(", should be 0X5X");
  }
  else
  {
    byte error = light.getError();
    printError(error);
  }

  gain = 0;
  unsigned char time = 2;
  light.setTiming(gain, time, ms);
  light.setPowerUp();

}

void loop() {
  
  sensorValue = analogRead(analogInPin);
  // map it to the range of the analog out:
  pitch = map(sensorValue, 600, 1023, 120, 1500); //maps values from photosensor to sound, 120-1500Hz range
  // change the analog out value:
  tone(11, pitch);
  
  getWeather();
  printInfo();
  delay(ms);
  unsigned int data0, data1;
  if (light.getData(data0, data1))
  {
    // getData() returned true, communication was successful

    Serial.print("data0: ");
    Serial.print(data0);
    Serial.print(" data1: ");
    Serial.print(data1);
    double lux;    // Resulting lux value
    boolean good;  // True if neither sensor is saturated
    good = light.getLux(gain, ms, data0, data1, lux);
    Serial.print(" lux: ");
    Serial.print(lux);
    if (good) Serial.println(" (good)"); else Serial.println(" (BAD)");
  }
  else
  {
    byte error = light.getError();
    printError(error);
  }
}

void printError(byte error)
// If there's an I2C error, this function will
// print out an explanation.
{
  Serial.print("I2C error: ");
  Serial.print(error, DEC);
  Serial.print(", ");

  switch (error)
  {
    case 0:
      Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }
}
void getWeather()
{
  // Measure Relative Humidity from the HTU21D or Si7021
  humidity = sensor.getRH();

  // Measure Temperature from the HTU21D or Si7021
  tempf = sensor.getTempF();
  // Temperature is measured every time RH is requested.
  // It is faster, therefore, to read it from previous RH
  // measurement with getTemp() instead with readTemp()
}
void printInfo()
{
//This function prints the weather data out to the default Serial Port

  Serial.print("Temp:");
  Serial.print(tempf);
  Serial.print("F, ");

  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.println("%");

  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
}
