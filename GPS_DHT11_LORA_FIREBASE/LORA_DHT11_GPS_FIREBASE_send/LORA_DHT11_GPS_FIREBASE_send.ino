#include <SoftwareSerial.h>
#include <DHT.h>
#include <TinyGPS++.h>
#define DHTPIN 2    // Chân dữ liệu của DHT 11 , với NodeMCU chân D5 GPIO là 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


SoftwareSerial mySerial(12, 14); //TX, RX
TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5);
// #define M0 8
// #define M1 9                 
unsigned long sendDataPrevMillis = 0;
int count = 0;
float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;
float lastLatitude = 0.0;
float lastLongitude = 0.0;
unsigned long lastTime = 0;
float speed = 0.0;  // Thêm biến tốc độ
void setup() {
  Serial.begin(9600);
  dht.begin();
  SerialGPS.begin(9600);
  mySerial.begin(9600);
  
  // pinMode(M0, OUTPUT);        
  // pinMode(M1, OUTPUT);
  // digitalWrite(M0, LOW);       // Set 2 chân M0 và M1 xuống LOW 
  // digitalWrite(M1, LOW);       // để hoạt động ở chế độ Normal
  
}

void loop() {

  float humidity = dht.readHumidity();
  float  temperature= dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
    }  // Đọc nhiệt độ theo độ C
  // if(Serial.available() > 0){
  //  String input = Serial.readString();
      
  //   mySerial.println(input);
 
  // }
 
  // if(mySerial.available() > 1){
   
  //   String input = mySerial.readString();
  //   Serial.println(input);  

  // }
  // delay(20);
  
  Serial.println(String(temperature) + "," + String(humidity));
  mySerial.println(String(temperature) + "," + String(humidity));
//  while (SerialGPS.available() > 0) {
//     if (gps.encode(SerialGPS.read())) {
//       if (gps.location.isValid()) {
//         Latitude = gps.location.lat();
//         LatitudeString = String(Latitude , 6);
//         Longitude = gps.location.lng();
//         LongitudeString = String(Longitude , 6);
//         Serial.print("Latitude: ");
//         Serial.println(Latitude, 6);
//         Serial.print("Longitude: ");
//         Serial.println(Longitude, 6);
//         //sendLocationToFirebase(Latitude, Longitude);
//         if (lastLatitude != 0.0 && lastLongitude != 0.0) {
//           unsigned long currentTime = millis();
//           unsigned long deltaTime = currentTime - lastTime;
//           // Tính khoảng cách dựa trên haversine formula
//           float distance = haversine(Latitude, Longitude, lastLatitude, lastLongitude);
//           // Tính vận tốc (tính toán khoảng cách trong mét và thời gian trong giây)
//           speed = (distance / 1000) / (deltaTime / 1000);  // Đơn vị km/h
//         }
        
//         lastLatitude = Latitude;
//         lastLongitude = Longitude;
//         lastTime = millis();
//       }
//       if (gps.date.isValid()) {
//         DateString = "";
//         date = gps.date.day();
//         month = gps.date.month();
//         year = gps.date.year();
//         if (date < 10)
//           DateString = '0';
//         DateString += String(date);
//         DateString += " / ";
//         if (month < 10)
//           DateString += '0';
//         DateString += String(month);
//         DateString += " / ";
//         if (year < 10)
//           DateString += '0';
//         DateString += String(year);
//       }
//       if (gps.time.isValid()) {
//         TimeString = "";
//         hour = gps.time.hour() + 5;  //adjust UTC
//         minute = gps.time.minute();
//         second = gps.time.second();
//         if (hour < 10)
//           TimeString = '0';
//         TimeString += String(hour);
//         TimeString += " : ";
//         if (minute < 10)
//           TimeString += '0';
//         TimeString += String(minute);
//         TimeString += " : ";
//         if (second < 10)
//           TimeString += '0';
//         TimeString += String(second);
//       }
//       //sendDateTimeToFirebase(year,month,date,hour,minute,second);
//     }
    
//   }
//   Serial.println(String(Latitude) + "," + String(Longitude));
//   mySerial.println(String(Latitude) + "," + String(Longitude));
//   Serial.println(String(year) + "," + String(month) + "," +String(date) + ","+String(hour) +","+String(minute) +","+String(second));
//   mySerial.println(String(year) + "," + String(month) + "," +String(date) + ","+String(hour) +","+String(minute) +","+String(second));
  delay(500);
}
// float haversine(float lat1, float lon1, float lat2, float lon2) {
//   float R = 6371000;  // Bán kính trái đất (đơn vị mét)
//   float dLat = radians(lat2 - lat1);
//   float dLon = radians(lon2 - lon1);
//   float a = sin(dLat / 2) * sin(dLat / 2) + cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon / 2) * sin(dLon / 2);
//   float c = 2 * atan2(sqrt(a), sqrt(1 - a));
//   float distance = R * c;
//   return distance;
// }