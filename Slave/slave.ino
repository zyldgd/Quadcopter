//#define Channel_Roll_H 0x01
//#define Channel_Roll_L 0x02
//#define Channel_Pitch_H 0x03
//#define Channel_Pitch_L 0x04
//#define Channel_Throttle_H 0x05
//#define Channel_Throttle_L 0x06
//#define Channel_Yaw_H 0x07
//#define Channel_Yaw_L 0x08

#include <Wire.h>
uint8_t Buffer[6];
uint16_t duration[6];

//////////////////////////////////////////////////////////////////////////////////////////////

void setup() {  
  Wire.begin(8);  //dev_addr  0x08;            
  Wire.onRequest(RequestEvent); 
  initPin();
}

void loop() {
  getRoll();
  getPitch();
  getThrottle();
  getYaw(); 
}

//////////////////////////////////////////////////////////////////////////////////////////////

void RequestEvent() {      
    Wire.write(Buffer[0]);
    Wire.write(Buffer[1]);
    Wire.write(Buffer[2]);
    Wire.write(Buffer[3]);
}

void initPin(){
  pinMode ( 12 , INPUT );
  pinMode ( 11 , INPUT );
  pinMode ( 10 , INPUT );
  pinMode ( 9 , INPUT );
  pinMode ( 8 , INPUT );
  pinMode ( 7 , INPUT );
}


uint16_t f0=0,sum0=0,cache0[10]={0};
void getRoll(){
  duration[0] =  pulseIn ( 12 , HIGH );
  if(duration[0]<1000) duration[0]=1000;
  else if(duration[0]>2000) duration[0]=2000;
  
  if(f0>4){
     f0=0;   
  }
  sum0 -= cache0[f0];
  sum0 += cache0[f0++] = duration[0];
  Buffer[0]=sum0/20-250;
}


uint16_t f1=0,sum1=0,cache1[10]={0};
void getPitch(){
  duration[1] =  pulseIn ( 11 , HIGH );
  if(duration[1]<1000) duration[1]=1000;
  else if(duration[1]>2000) duration[1]=2000;
  
  if(f1>4){
     f1=0;   
  }
  sum1 -= cache1[f1];
  sum1 += cache1[f1++] = duration[1];
  Buffer[1]=sum1/20-250;
}


uint16_t f2=0,sum2=0,cache2[10]={0};
void getThrottle(){
  duration[2] =  pulseIn( 10 , HIGH );
  if(duration[2]<1030) duration[2]=1000;
  else if(duration[2]>2000) duration[2]=2000;
  
  if(f2>4) f2=0;   
  sum2 -= cache2[f2];
  sum2 += duration[2];
  cache2[f2++] = duration[2];
  Buffer[2]=sum2/20-250;
}


uint16_t f3=0,sum3=0,cache3[10]={0};
void getYaw(){
  duration[3] =  pulseIn ( 9 , HIGH );
  if(duration[3]<1000) duration[3]=1000;
  else if(duration[3]>2000) duration[3]=2000;
  
  if(f3>4){
     f3=0;   
  }
  sum3 -= cache3[f3];
  sum3 += cache3[f3++] = duration[3];
  Buffer[3]=sum3/20-250;
}


