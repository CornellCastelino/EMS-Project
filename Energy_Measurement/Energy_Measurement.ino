#include "EmonLib.h"             // Include Emon Library
#include <SPI.h>
#include <SD.h>

File myFile;
EnergyMonitor emon1;             // Create an instance
int stablize = 30;
bool initial;
void setup()
{
  Serial.begin(9600);
  emon1.voltage(A3, 1080.6,1.15);  // Voltage: input pin, calibration, phase_shift
  emon1.current(A2, 355.9);       // Current: input pin, calibration.
  // new calibration = old * (correct reading/arduino reading)
  initial =  true;
}

void loop()
{ 
  if (initial){
    Serial.println("Starting to stablize...");
    for( int i = 0; i < stablize;i++){
      emon1.calcVI(20,2000);
      Serial.print((double)i/stablize*100);
      Serial.println("% done..");
      emon1.serialprint();
    }
    Serial.println("Output has stablized");
  initial = false;
  }
  emon1.calcVI(20,2000);
  //_____________________________________________
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  myFile = SD.open("test.txt", FILE_WRITE);

  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.print(millis());
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
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  delay(1000);
}
