
#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define PH_PIN A5
float voltage,phValue,temperature = 25;
DFRobot_PH ph;
LiquidCrystal lcd = LiquidCrystal(2, 3, 4, 5, 6, 7);

void setup()
{
   lcd.begin(16, 2);  
    ph.begin();
}

void loop()
{
    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                  //time interval: 1s
        timepoint = millis();
        //temperature = readTemperature();         // read your temperature sensor to execute temperature compensation
        voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
        phValue = ph.readPH(voltage,temperature);  // convert voltage to pH with temperature compensation
//        Serial.print("temperature:");
//        Serial.print(temperature,1);
//        Serial.print("^C  pH:");
//        Serial.println(phValue,2);
        lcd.setCursor(0, 0);
        lcd.print("TEMP: ");
        lcd.print(temperature);
        lcd.setCursor(0, 1);
        lcd.print("PH VALUE: ");
        lcd.print(phValue);
        
    }
    ph.calibration(voltage,temperature);           // calibration process by Serail CMD
}

float readTemperature()
{
  //add your code here to get the temperature from your temperature sensor
}
