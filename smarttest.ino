#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <Servo.h>
#define FIREBASE_HOST "project-d8544.firebaseio.com"
#define FIREBASE_AUTH "J6tZw5IOqTRSaK3okCriDqHYvi0x9wwuASV6mDX3" // 데이터베이스 비밀번호
#define WIFI_SSID "aa" // 연결 가능한 wifi의 ssid
#define WIFI_PASSWORD "00000000" // wifi 비밀번호
 
FirebaseData firebaseData;
FirebaseJson json;
Servo myservo;
void printResult(FirebaseData &data);
void setup() // wifi 접속 과정.
{
  Serial.begin(115200);
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println();
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
 
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
 
  myservo.attach(12); // 출력 핀 잡아주기.
}
String temp = "-1", value; 
void loop()
{
  
    if (Firebase.getString(firebaseData, "Status/value")) // 해당하는 key가 있으면
    {
      value = firebaseData.stringData(); // 값을 문자열로 받아와서 value에
      Serial.println(value); // 시리얼모니터에 값을 출력
      if(temp != "-1"){
          if (value == "0"&& temp != value)
          {
            myservo.write(90); // 정지
            delay(400);
            myservo.write(80); // 역회전
            delay(500);
            myservo.write(90); // 정지
          }
          else if (value == "1" && temp!= value)
          {
            myservo.write(90); // 정지
            delay(1);
            myservo.write(100); // 정회전
            delay(480);
            myservo.write(90); // 정지
          }
       }
    }
     else
      {
        Serial.println(firebaseData.errorReason()); // 에러 메시지 출력
      }
      delay(1000); // 1초마다 반복
      temp = value;  
    
 }
