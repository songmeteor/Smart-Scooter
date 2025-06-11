#include <DFRobot_TFmini.h>
#include "HX711.h"  
#include <SoftwareSerial.h> 

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN_1 = 22; //첫 번째 로드셀의 DOUT 핀 정의
const int LOADCELL_SCK_PIN_1 = 2;   //첫 번째 로드셀의 CLK 핀 정의

const int LOADCELL_DOUT_PIN_2 = 23;  //두 번째 로드셀의 DOUT 핀 정의
const int LOADCELL_SCK_PIN_2 = 3;    //두 번째 로드셀의 CLK 핀 정의

int SensorPin = A0; //헬멧 압력센서 output 핀 정의

HX711 scale1;    //첫번째 로드셀을 scale1, 두번째 로드셀을 scale2 로 정의
HX711 scale2;

SoftwareSerial mySerial(11, 10); // RX, TX  *****메가에서는 serial 통신할때 10~15까지만 가능*******
SoftwareSerial hc05(12, 13); // 블루투스 

DFRobot_TFmini  TFmini;
uint16_t distance, strength;   // 거리와 강도를 담는 변수

void setup() {
  pinMode(34,1);             // 모터 핀 정의
  pinMode(35,1);
 
  Serial.begin(115200);     // pc-아두이노 전송 속도 설정
  hc05.begin(9600);         // 아두이노-블루투스 모듈 전송 속도 설정
  TFmini.begin(mySerial);     //라이다 센서 정의
  scale1.begin(LOADCELL_DOUT_PIN_1, LOADCELL_SCK_PIN_1);  //로드셀 센서 정의
  scale2.begin(LOADCELL_DOUT_PIN_2, LOADCELL_SCK_PIN_2);

  scale1.set_scale(2280.f);                     
  scale1.tare();
  scale2.set_scale(2280.f);                     
  scale2.tare();
}

void loop() {
  
  int SensorReading = analogRead(SensorPin);      //압력센서 변수 정의
  int helmet_sensor = map(SensorReading, 0, 1024, 0, 255);
  
  double x = scale1.get_units(1);     //첫번째 로드셀 값 x에 저장
  double y = scale2.get_units(1);     //두번째 로드셀 값 y에 저장

  double weight1 = 57.013*x - 101.44 ;       //센서값 g단위로 변환
  double weight2 = 57.013*y - 101.44 ;   
  double weight3 = (weight1 + weight2)/1000;              

  if (TFmini.measure()) {                  // 거리와 신호의 강도를 측정합니다. 성공하면 을 반환하여 if문이 작동합니다.
    distance = TFmini.getDistance();       // 거리값을 cm단위로 불러옵니다.
    strength = TFmini.getStrength();       // 신호의 강도를 불러옵니다. 측정 대상이 넓으면 강도가 커집니다.
  }
    float dis = float(distance)/100;
 
   String sensor = String(helmet_sensor) + "," + String(weight3) + "," + String(dis) + ",";
   Serial.println(sensor);
   hc05.println(sensor);

  if(helmet_sensor>50)          // 압력센서 값 30 이상일때 수동부저 울리
  {
    tone(4,261,500);
    delay(500);
  }
   
    if(weight3>100)             //로드셀 7kg이상일때 모터 속도 제한& 수동부저 울리기
  {
    tone(4,350,1000);
    digitalWrite(34,1);
    digitalWrite(35,0);
    analogWrite(5,50);
  }
  else{
     digitalWrite(34,1);      //모터 최대 출력
     digitalWrite(35,0);
     analogWrite(5,255);
  }
  delay(100);
}