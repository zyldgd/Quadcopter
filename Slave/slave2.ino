#define cacheLen 5
#include <Wire.h>
uint8_t Buffer[6];
int duration[6], current[6], zero[6],zeroCa[6][20];
int Min[6] = {928, 919, 929, 925, 1100, 1100};
int Max[6] = {1855, 1866, 1836, 1827, 1700, 1700};
int Mid[6];
int cache[6][cacheLen];


void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(RequestEvent); // register event
  Serial.begin(115200);           // start serial for output
  ini();
 // while(!getMid());

}

int dt;
void loop() {

   dt=millis();
   getVal2(0);
   dt=millis() - dt;
   if (dt<100){
    getVal2(1);
    getVal2(2);
    getVal2(3);
   }
   else
   {
    Buffer[0]=0;
    Buffer[1]=0;
    Buffer[2]=0;
    Buffer[3]=0;
   }


  




#if 0
  Serial.print(dt);Serial.print("\t");
  Serial.print(duration[0]); Serial.print("\t");
  Serial.print(duration[1]); Serial.print("\t");
  Serial.print(duration[2]); Serial.print("\t");
  Serial.print(duration[3]); Serial.println("\t");
#endif

#if 0
  Serial.print(dt); Serial.print("\t");
  Serial.print(current[0]); Serial.print("\t");
  Serial.print(current[1]); Serial.print("\t");
  Serial.print(current[2]); Serial.print("\t");
  Serial.print(current[3]); Serial.println("\t");
#endif

#if 0
  Serial.print(dt); Serial.print("\t");
  Serial.print(Buffer[0]); Serial.print("\t");
  Serial.print(Buffer[1]); Serial.print("\t");
  Serial.print(Buffer[2]); Serial.print("\t");
  Serial.print(Buffer[3]); Serial.println("\t");
#endif

}


void RequestEvent() {
  Wire.write(Buffer[0]);
  Wire.write(Buffer[1]);
  Wire.write(Buffer[2]);
  Wire.write(Buffer[3]);
}

void ini() {
  pinMode ( 12 , INPUT );
  pinMode ( 11 , INPUT );
  pinMode ( 10 , INPUT );
  pinMode ( 9 , INPUT );
  pinMode ( 8 , INPUT );
  pinMode ( 7 , INPUT );
  delay(220);
  while (!getMap()); //等待解锁  
  delay(500);
//  Serial.println("OK");
//  for (int i = 0 ; i < 4; i++) {
//    Serial.print(Min[i]); Serial.print("\t"); Serial.print(Max[i]); Serial.print("\t$\t");
//  }

}

bool getMid(){
  unsigned long  dt=millis();
  while(1){
  getVal2(0);
  getVal2(1);
  getVal2(2);
  getVal2(3);
 
  dt=millis() - dt;
  if(dt>500){
    zero[0]=duration[0];
    zero[1]=duration[1];
    zero[2]=duration[2];
    zero[3]=duration[3];
    return true;
  }
  }
}


void getVal2(int i){
  duration[i] = restrain(pulseIn(12-i , HIGH),Min[i], Max[i]); //取pwm高电平值
  leftMove(cache[i],cacheLen, duration[i]);                         //将该值插入到缓存最后面
  int val = average(cache[i],cacheLen);                            //求平均值
  if(abs(current[i]-zero[i])>abs(val-zero[i]) || abs(val-current[i])>15)
    {
    current[i] = val;
    }
  Buffer[i] = map(current[i], Min[i], Max[i], 0, 255);
  }



void getVal(int i) {
  duration[i] = restrain(pulseIn(12 - i , HIGH), Min[i], Max[i]);  //取pwm高电平值
  int err = abs((int)duration[i] - (int)zero[i]);

  /******接近0点，当0点处理******/
  if (err < 3)
    current[i] = zero[i];
  /******向0点移动取最小值，向外移动间隔为10******/
  else if ((abs(current[i] - zero[i]) > err) || (abs(duration[i] - current[i]) >= 10))
    current[i] = duration[i];

  Buffer[i] = map(current[i], Min[i], Max[i], 0, 255);
}



bool getMap() {
   unsigned long dt;
  while (1) {
    /**************是否接受到遥控器信号********/
    dt = millis();
    pulseIn(12  , HIGH);
    if(millis() - dt>60) return false;
    /**************确认频道范围****************/
    for (int i = 0; i < 4; i++) {
      duration[i] = pulseIn(12 - i , HIGH);
      /**************最小值****************/
      if (Min[i] > 800 && Min[i] < 1300)
      {
        if (Min[i] > duration[i]) Min[i] = duration[i];
      }
      else
        Min[i] = 1100;
      /**************最大值****************/
      if (Max[i] > 1700 && Max[i] < 2100)
      {
        if (Max[i] < duration[i]) Max[i] = duration[i];
      }
      else
        Max[i] = 1750;
    }
    /****************等待解锁******************/
    if (comfirmSign(2, Min[2], 3, Max[3], 1000))  return true;
  }
}


bool comfirmSign( int channle1, int channle1Val, int channle2, int channle2Val, int timer) {
  readChannle();
  /****************1.判断信号******************/
  if (abs(duration[channle1] - channle1Val) < 100 && abs(duration[channle2] - channle2Val) < 100) {
    unsigned long dt = millis();
    /****************2.持续确认******************/
    while (abs(duration[channle1] - channle1Val) < 100 && abs(duration[channle2] - channle2Val) < 100) {
      //Serial.println("comfirming");
      readChannle();
      if (millis() - dt > timer)  return true;
    }
  }
  return false;//信号不符合 或者 确认时长不够，都会返回失败
}


void readChannle() {
  duration[0] = pulseIn(12 , HIGH);
  duration[1] = pulseIn(11 , HIGH);
  duration[2] = pulseIn(10 , HIGH);
  duration[3] = pulseIn(9 , HIGH);
}


int restrain(int v, int Min, int Max) {
  /********效果与constrained()函数相同，但是其比3元操作符跟快**********/
  if (v > Max)
    return Max;
  else if (v < Min)
    return Min;
  else
    return v;
}


//////****************************数组操作*******************************//////////
int average(int *arr,int len) {
  unsigned long s=0;
 // int len = sizeof(arr) / sizeof(arr[0]);
  for (int i = 0; i < len; i++) {
    s += arr[i];
  } 
  return s/len;
  
}

int getMax(int *arr) {
  int Max = arr[0];
  int len = sizeof(arr) / sizeof(arr[0]);
  for (int i =0; i < len; i++) {
    if (Max < arr[i])
      Max = arr[i];
  }
  return Max;
}

int getMin(int *arr) {
  int Min = arr[0];
  int len = sizeof(arr) / sizeof(arr[0]);
  for (int i = 0; i < len; i++) {
    if (Min > arr[i])
      Min = arr[i];
  }
  return Min;
}

void leftMove(int *arr,int len, int val) {
  
  for (int i = 1; i <len; i++) {
    arr[i - 1] = arr[i];
  }
  arr[len-1] = val;
}

