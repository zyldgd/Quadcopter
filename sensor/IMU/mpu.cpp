#include "MPU.h"

MPU::MPU() {
  devAddr = MPU_DEFAULT_ADDRESS;
}

bool MPU::initialize(uint8_t id) {
  
  I2Cdev::writeBits(devAddr, MPU_RA_PWR_MGMT_1, MPU_PWR1_CLKSEL_BIT, MPU_PWR1_CLKSEL_LENGTH, MPU_CLOCK_PLL_XGYRO);      // setClockSource(MPU_CLOCK_PLL_XGYRO);
  delay(5);
  I2Cdev::writeBits(devAddr, MPU_RA_GYRO_CONFIG, MPU_GCONFIG_FS_SEL_BIT, MPU_GCONFIG_FS_SEL_LENGTH, MPU_GYRO_FS_250);   //setFullScaleGyroRange(MPU_GYRO_FS_250);
  delay(5);  
  I2Cdev::writeBits(devAddr, MPU_RA_ACCEL_CONFIG, MPU_ACONFIG_AFS_SEL_BIT, MPU_ACONFIG_AFS_SEL_LENGTH, MPU_ACCEL_FS_2); //setFullScaleAccelRange(MPU_ACCEL_FS_2);
  delay(5);  
  I2Cdev::writeBit(devAddr, MPU_RA_PWR_MGMT_1, MPU_PWR1_SLEEP_BIT, false);                                              //setSleepEnabled(false);
  delay(5);
  I2Cdev::writeByte(devAddr, MPU_RA_INT_PIN_CFG, 0x02);                                                                 //set i2c bypass enable pin to true to access magnetometer
  delay(5);
  I2Cdev::writeByte(MPU9150_RA_MAG_ADDRESS, 0x0A, 0x01);                                                                //enable the magnetometer
  delay(5);

  return (getDeviceID() == id);
}

uint8_t MPU::getDeviceID() {
  I2Cdev::readByte(devAddr, MPU_RA_WHO_AM_I, buffer);
  return buffer[0];
}

void MPU::getMotion9() {
  this->getAccGyro();
  this->getMag();
}

void MPU::getAccGyro() {
  I2Cdev::readBytes(devAddr, MPU_RA_ACCEL_XOUT_H, 14, buffer);
  this->aX = (buffer[0] << 8) | buffer[1];
  this->aY = (buffer[2] << 8) | buffer[3];
  this->aZ = (buffer[4] << 8) | buffer[5];
//this->tempRaw = (buffer[6] << 8) | buffer[7];
  this->gX = (buffer[8] << 8) | buffer[9];
  this->gY = (buffer[10] << 8) | buffer[11];
  this->gZ = (buffer[12] << 8) | buffer[13];
}

void MPU::getMag() {
  I2Cdev::readBytes(MPU9150_RA_MAG_ADDRESS, MPU9150_RA_MAG_XOUT_L, 6, buffer);//read mag
  this->mX = (buffer[0] << 8) | buffer[1];
  this->mY = (buffer[2] << 8) | buffer[3];
  this->mZ = (buffer[4] << 8) | buffer[5];
}


void MPU::getAngleRaw() { 
  
  this->pitchRaw = atan2(-this->aX, this->aZ) * RAD_TO_DEG;
  this->rollRaw	 = atan2(this->aY, this->aZ) * RAD_TO_DEG;//(0,180)U(0,-180)
 // this->pitchRaw = atan((float)this->aX / sqrt((float)this->aY * (float)this->aY + (float)this->aZ * (float)this->aZ)) * RAD_TO_DEG; //(0,90)U(0,-90)

  this->yawRaw = 0;
}
void MPU::getRate() {
  this->Yrate = ((float)this->gY / 131.0)-1.0;
  this->Xrate = ((float)this->gX / 131.0)-1.4;  
  this->Zrate = (float)this->gZ / 131.0;
}

void MPU::getAngle() {
  this->getAccGyro();
  this->getAngleRaw();
  this->getRate();

   this->dt = (double)(micros() - this->timer) / 1000000.00;
   this->timer = micros();// Calculate delta time  (Δt)
   
   this->pitch = this->kalmanY.getAngle(this->pitchRaw, this->Yrate, this->dt);
   this->roll = this->kalmanX.getAngle(this->rollRaw, this->Xrate, this->dt);
   

}


