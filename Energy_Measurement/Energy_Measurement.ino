#include "EmonLib.h"             // Include Emon Library
#include <SPI.h>
#include <SD.h>

File myFile;
EnergyMonitor emon1;             // Create an instance
double Rpower,Apower,V,I,Pf,samples;     //V and I are in RMS values.
int stablizeNum =45;                // takes # samples to stablize the arduino readings on start up
double Vcal = 692.52;//692.52
double Ical = 144.55;//144.55
double Pfcal = 1.15;
int Time;
bool initial;
void setup()
{
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  emon1.voltage(A6, Vcal,Pfcal);  // Voltage: input pin, calibration, phase_shift
  emon1.current(A1, Ical);       // Current: input pin, calibration.
  // new calibration = old * (correct reading/arduino reading)
  initial =  true;
  Rpower = 0;
  Apower = 0;
  V = 0;
  I = 0;
  Pf = 0;
  samples = 0;
  Time = millis();

  stablize(emon1,stablizeNum);

}

void loop()
{ 
 StoreEmon(emon1,Rpower,Apower,V,I,Pf,samples,Time);
  //__________________________________________
 
}
void StoreEmon(EnergyMonitor emon1,double &Rpower,double &Apower,double &V,double &I,double &Pf,double &samples,int &Time){
   //avg values
  digitalWrite(2, LOW);   // turn the LED on (HIGH is the voltage level) 
  emon1.calcVI(20,2000);
  digitalWrite(2, HIGH);    // turn the LED off by making the voltage LOW


  
  Rpower += emon1.realPower;
  Apower += emon1.apparentPower;
  V += emon1.Vrms;
  I += emon1.Irms;
  Pf += emon1.powerFactor;
  ++samples;
  
  emon1.serialprint();
  Store(emon1,myFile);
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
    Serial.println((double)Time/1000);S
  }
}
void stablize(EnergyMonitor emon1, int count){
  Serial.println("Starting to stablize...");
    for( int i = 0; i < count;i++){
      digitalWrite(2, LOW);   // turn the LED on (HIGH is the voltage level) 
      emon1.calcVI(20,2000);
      digitalWrite(2, HIGH);
      Serial.print((double)i/count*100);
      Serial.println("% done..");
    }
    Serial.println("Output has stablized");
}
void Store(EnergyMonitor emon1,File myFile){//function to store in SD card
  
//  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level) 
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
