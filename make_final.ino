#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"


#define ONE_WIRE_BUS 2
#define echoPin 5
#define trigPin 6
#define REPORTING_PERIOD_MS     1000

const int DHTPIN = 4;
//const int buzzerPin = 8;
const int ledPin1 = 9;
const int ledPin2 = 10;
const int ledPin3 = 11;

int SoundPin = A0;
long duration; 
int distance;

float bodyTemp;
float temp;
float humidity;



PulseOximeter pox;
uint32_t tsLastReport = 0;


DHT dht(DHTPIN, DHT11);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress insideThermometer;



void onBeatDetected()
{
    Serial.println("Beat!");
}



void setup() {
  // put your setup code here, to run once:
  pinMode (SoundPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
//  pinMode(buzzerPin, OUTPUT);
  
  
  
  
  dht.begin();
 // hc05.begin(9600);
  Serial.begin(9600);
  



  Serial.println("Dallas Temperature IC Control Library Demo");
  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();



  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
        Serial.println("FAILED");
//        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
     pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
  
  
}



// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  
  bodyTemp = sensors.getTempC(deviceAddress);
  if(bodyTemp == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.println();
  Serial.print("body Temp: ");
  Serial.print(bodyTemp);
  Serial.print(" C");
  
}




void loop() {
  // put your main code here, to run repeatedly:

//  delay(2000);
  
  temp = dht.readTemperature();
  humidity = dht.readHumidity();

  int soundLevel = analogRead(SoundPin);


  // request to all devices on the bus
//  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
//  Serial.println("DONE");




  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;


  pox.update();


  

  printTemperature(insideThermometer); // Use a simple function to print out the data
  Serial.print("      Temp: ");
  Serial.print(temp);
  Serial.print(" C");
  Serial.print("         Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.println();
  Serial.print("Sound Level: ");
  Serial.print(soundLevel);
  Serial.print(" dB");
  Serial.print("      Distance: ");
  Serial.print(distance);
  Serial.print(" cm");
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("      space ");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2: ");
        Serial.print(pox.getSpO2());
        Serial.println("%");
 
        tsLastReport = millis();

    }
  Serial.println();
  Serial.println();
  Serial.println("-----------------------------------------------------------------------------");
  



  
  if(distance < 50)
  {
    digitalWrite(ledPin1, HIGH);
//    digitalWrite(buzzerPin, HIGH); 
  }else 
  {
    digitalWrite(ledPin1, LOW);
//    digitalWrite(buzzerPin, LOW);
  }


  if(bodyTemp < 36)
  {
    digitalWrite(ledPin2, HIGH);
//    digitalWrite(buzzerPin, HIGH); 
  }else 
  {
    digitalWrite(ledPin2, LOW);
//    digitalWrite(buzzerPin, LOW);
  }

  if( temp > 28){
    digitalWrite(ledPin3, HIGH);
  }else{
    digitalWrite(ledPin3, LOW);
  }
  
  
//hc05.print((float)DHT11.temperature);
}


// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

