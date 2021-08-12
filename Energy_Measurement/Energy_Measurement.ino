#include "EmonLib.h"             // Include Emon Library
#include <SPI.h>
#include <SD.h>

File myFile;
EnergyMonitor emon1;             // Create an instance
double Rpower,Apower,V,I,Pf,samples;     //V and I are in RMS values.
int stablize = 45;                      // takes # samples to stablize the arduino readings on start up
int Time;
bool initial;
void setup()
{
  Serial.begin(9600);
  emon1.voltage(A3, 1065.98,1.15);  // Voltage: input pin, calibration, phase_shift
  emon1.current(A2, 367.2);       // Current: input pin, calibration.
  // new calibration = old * (correct reading/arduino reading)
  initial =  true;
  Rpower = 0;
  Apower = 0;
  V = 0;
  I = 0;
  Pf = 0;
  samples = 0;
  Time = millis();
}

void loop()
{ 
  if (initial){
    Serial.println("Starting to stablize...");
    for( int i = 0; i < stablize;i++){
      emon1.calcVI(20,2000);
      Serial.print((double)i/stablize*100);
      Serial.println("% done..");
    }
    Serial.println("Output has stablized");
  initial = false;
  }
  //__________________________________________
  //avg values
  emon1.calcVI(20,2000);
  Rpower += emon1.realPower;
  Apower += emon1.apparentPower;
  V += emon1.Vrms;
  I += emon1.Irms;
  Pf += emon1.powerFactor;
  ++samples;
  emon1.serialprint();
  if(millis() - Time >15000){// log avg values every #s
    emon1.realPower = Rpower/samples;
    emon1.apparentPower = Apower/samples;
    emon1.Vrms = V/samples;
    emon1.Irms = I/samples;
    emon1.powerFactor = Pf/samples;
    
    Store(emon1,myFile);
    
    Rpower = 0;
    Apower = 0;
    V = 0;
    I = 0;
    Pf = 0;
    samples = 0;
    Time = millis();
    Serial.print("logged @:");
    Serial.println(Time);
  }
}
void Store(EnergyMonitor emon1,File myFile){//function to store in SD card
  
//  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
//    Serial.println("initialization failed!");
    while (1);
  }
//  Serial.println("initialization done.");
  myFile = SD.open("test.txt", FILE_WRITE);

  if (myFile) {
//    Serial.print("Writing to test.txt...");
    myFile.print(millis()/1000);
    myFile.print(",");
    myFile.print(emon1.realPower);
    myFile.print(",");
    myFile.print(emon1.apparentPower);
    myFile.print(",");
    myFile.print(emon1.Vrms);
    myFile.print(",");
    myFile.print(emon1.Irms);
    myFile.print(",");
    myFile.println(emon1.powerFactor);
    
    myFile.close();
//    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
