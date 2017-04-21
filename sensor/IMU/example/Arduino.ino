#include "mpu.h"
#include "Kalman.h"

MPU mpu;// class default I2C address is 0x68

void setup() {
	bool b;
  Wire.begin();
  Serial.begin(115200); 
  //mpu92：0x71 | mpu65:0x70   
  mpu.initialize(0x71)?Serial.print("get Connected"):Serial.print("failed");
  
}

void loop() {


 mpu.getAngle();
 
 Serial.print(mpu.Yrate);Serial.print("\t");
 Serial.print(mpu.pitchRaw);Serial.print("\t"); 
 Serial.print(mpu.pitch);Serial.print("\t");
 Serial.print("\t|\t");
 Serial.print(mpu.Xrate);Serial.print("\t");
 Serial.print(mpu.rollRaw);Serial.print("\t"); 
 Serial.print(mpu.roll);Serial.println("\t");
}
