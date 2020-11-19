#include "FirebaseESP8266.h" // 반드시 파이어베이스 라이브러리를 먼저 추가해야 합니다.
#include <ESP8266WiFi.h>
#include <Servo.h>
#define FIREBASE_HOST "project-d8544.firebaseio.com" // http달린거 빼고 적어야 됩니다.
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
 
  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);
  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);
  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
 
  myservo.attach(12); // 출력 핀 잡아주기.
}
String temp = "-1", value; 
void loop()
{
  
    if (Firebase.getString(firebaseData, "Status/value")) // 해당하는 key가 있으면 (앱 인벤터에서는 tag)
    {
      value = firebaseData.stringData(); // 값을 문자열로 받아와서 value에
      Serial.println(value); // 시리얼모니터에 값을 출력
      if(temp != "-1"){
          if (value == "0"&& temp != value)
          {
            myservo.write(90); // 정지
            delay(400); // 3초간 지속
            myservo.write(80); // 역회전
            delay(500); // 3초간 지속
            myservo.write(90); // 정지
          }
          else if (value == "1" && temp!= value)
          {
            myservo.write(90); // 정지
            delay(1); // 3초간 지속
            myservo.write(100); // 정회전
            delay(480); // 3초간 지속
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
