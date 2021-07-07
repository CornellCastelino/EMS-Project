#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instanc
void setup()
{
  Serial.begin(9600);
  //emon1.voltage(2, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(A2, 111.1);       // Current: input pin, calibration.

}
//int getMax(){
//  int Max = 0;
//  for(int i = 0; i < 100; i++){
//    int sensorVal = analogRead(A2);
//    if(sensorVal > Max){
//      Max = sensorVal;
//    }
//    delay(10);
//  }
//  return Max;
//}
void loop()
{ 
//  float v = getMax()* (3.3/1023.0);
//  
//  float Pow = v*v*4000.0/2.0;                 // ~v*(v*10 = I), 400(CT ratio)
//  Serial.print("Value of V: ");
//  Serial.println(v);
//  Serial.print("Approx. Power Consumption: ");
//  Serial.print(Pow);
//  Serial.println("W");
  delay(10);
  double Irms = emon1.calcIrms(1480);         // Calculate all. No.of wavelengths, time-out
  Serial.print("Irms : ");
  Serial.println(Irms);
  delay(100);
//  emon1.serialprint();           // Print out all variables
}
