#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "SH1106.h"  // Include the SH1106Wire library
//Provide the token generation process info.
#include <SD.h>
#include <SPI.h>
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
bool doorStatus = false;
SoftwareSerial mySerial(D3, D0); //TX, RX

TinyGPSPlus gps;
SoftwareSerial SerialGPS(3, 2); 
SH1106 display(0x3C, D2, D1); 
const int signal = D4;
const int chipSelect = 15;
const char* ssid = "Ty Ty";
const char* password = "khongcomatkhau";
#define DATABASE_URL "https://gps-tracking-7ec2d-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define API_KEY "AIzaSyBPGox7HPoz_hMhCxWDVz1iq3NXYL7mmQM"
 
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;
float lastLatitude = 0.0;
float lastLongitude = 0.0;
unsigned long lastTime = 0;
float speed = 0.0;  // Thêm biến tốc độ

WiFiServer server(80);
               
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  SerialGPS.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  display.init();
  display.flipScreenVertically();
  display.clear();
  Serial.println();
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");
}


void loop() {

 String inputReceive = "";
float temperature = 0.0;
float humidity = 0.0;
  if (SD.begin(chipSelect)) {
    // Log data to SD card
    logDataToSD(temperature, humidity, hour, minute, second, date, month, year, Longitude, Latitude, speed);
    // Ensure to close the SD card connection
    SD.end();
  } else {
    Serial.println("SD card initialization failed!");
  }
if (mySerial.available() > 0)
{
  inputReceive = mySerial.readStringUntil('\n');
  Serial.println(inputReceive);

  // Split the received data into temperature and humidity
  int separatorIndex = inputReceive.indexOf(',');
  if (separatorIndex != -1)
  {
    String temperatureString = inputReceive.substring(0, separatorIndex);
    String humidityString = inputReceive.substring(separatorIndex + 1);
    temperature = temperatureString.toFloat();
    humidity = humidityString.toFloat();
  }

  // Print temperature and humidity separately
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" degrees Celsius");
  
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  sendTemperatureAndHumidityToFirebase(temperature, humidity);
}
receiveDoorStatus();
   while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read())) {
      if (gps.location.isValid()) {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude , 6);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude , 6);
        Serial.print("Latitude: ");
        Serial.println(Latitude, 6);
        Serial.print("Longitude: ");
        Serial.println(Longitude, 6);
        sendLocationToFirebase(Latitude, Longitude);
        if (lastLatitude != 0.0 && lastLongitude != 0.0) {
          unsigned long currentTime = millis();
          unsigned long deltaTime = currentTime - lastTime;
          // Tính khoảng cách dựa trên haversine formula
          float distance = haversine(Latitude, Longitude, lastLatitude, lastLongitude);
          // Tính vận tốc (tính toán khoảng cách trong mét và thời gian trong giây)
          speed = (distance / 1000) / (deltaTime / 1000);  // Đơn vị km/h
        }
        Serial.print("Speed: ");
        Serial.print(speed);
        Serial.println(" km/h");
        lastLatitude = Latitude;
        lastLongitude = Longitude;
        lastTime = millis();
        // Gửi tốc độ lên Firebase
        sendSpeedToFirebase(speed);
      }
      if (gps.date.isValid()) {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        if (date < 10)
          DateString = '0';
        DateString += String(date);
        DateString += " / ";
        if (month < 10)
          DateString += '0';
        DateString += String(month);
        DateString += " / ";
        if (year < 10)
          DateString += '0';
        DateString += String(year);
        Serial.println("Date: " + DateString);
      }
      if (gps.time.isValid()) {
        TimeString = "";
        hour = gps.time.hour() + 7;  //adjust UTC
        minute = gps.time.minute();
        second = gps.time.second();
        if (hour < 10)
          TimeString = '0';
        TimeString += String(hour);
        TimeString += " : ";
        if (minute < 10)
          TimeString += '0';
        TimeString += String(minute);
        TimeString += " : ";
        if (second < 10)
          TimeString += '0';
        TimeString += String(second);
      }
      sendDateTimeToFirebase(year,month,date,hour,minute,second);
    }
  }
  displayInfoOnOLED(DateString, TimeString, temperature, humidity, speed);
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  //Response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>NEO-6M GPS Readings</title> <style>";
  s += "table, th, td {border: 1px solid blue;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER>NEO-6M GPS Readings</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += LatitudeString;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += LongitudeString;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += DateString;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += TimeString;
  s += "</td> </tr> <tr>  <th>temperature</th> <td ALIGN=CENTER >";
  s += temperature;
  s += "</td></tr> <tr> <th>humidity</th> <td ALIGN=CENTER >";
  s += humidity;
  s += "</td></tr> <tr> <th>speed</th> <td ALIGN=CENTER >";
  s += speed;  // In tốc độ
  s += "</td>  </tr> </table> ";
  if (gps.location.isValid()) {
    s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += LatitudeString;
    s += "+";
    s += LongitudeString;
    s += """ target=""_top"">Click here</a> to open the location in Google Maps.</p>";
  }
  s += "</body> </html> \n";
  client.print(s);
  
  delay(1000);
}
float haversine(float lat1, float lon1, float lat2, float lon2) {
  float R = 6371000;  // Bán kính trái đất (đơn vị mét)
  float dLat = radians(lat2 - lat1);
  float dLon = radians(lon2 - lon1);
  float a = sin(dLat / 2) * sin(dLat / 2) + cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon / 2) * sin(dLon / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  float distance = R * c;
  return distance;
}
void sendLocationToFirebase(float latitude, float longitude) {
  if (Firebase.ready() && signupOK) {
    // Gửi Latitude lên Firebase
    if (Firebase.RTDB.setFloat(&fbdo, "Location/Latitude", latitude)) {
      Serial.print("Latitude : ");
      Serial.println(latitude);
    }
    else {
      Serial.println("Failed to send Latitude to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Gửi Longitude lên Firebase
    if (Firebase.RTDB.setFloat(&fbdo, "Location/Longitude", longitude)) {
      Serial.print("Longitude : ");
      Serial.println(longitude);
    }
    else {
      Serial.println("Failed to send Longitude to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
void sendDateTimeToFirebase(int year, int month, int date, int hour, int minute, int second) {
  if (Firebase.ready() && signupOK) {
    String dateTimePath = "DateTime"; // Đường dẫn trên Firebase Realtime Database

    // Gửi năm lên Firebase
    if (Firebase.RTDB.setInt(&fbdo, dateTimePath + "/Year", year)) {
      Serial.print("Year: ");
      Serial.println(year);
    }
    else {
      Serial.println("Failed to send Year to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Gửi tháng lên Firebase
    if (Firebase.RTDB.setInt(&fbdo, dateTimePath + "/Month", month)) {
      Serial.print("Month: ");
      Serial.println(month);
    }
    else {
      Serial.println("Failed to send Month to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Gửi ngày lên Firebase
    if (Firebase.RTDB.setInt(&fbdo, dateTimePath + "/Date", date)) {
      Serial.print("Date: ");
      Serial.println(date);
    }
    else {
      Serial.println("Failed to send Date to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Gửi giờ lên Firebase
    if (Firebase.RTDB.setInt(&fbdo, dateTimePath + "/Hour", hour)) {
      Serial.print("Hour: ");
      Serial.println(hour);
    }
    else {
      Serial.println("Failed to send Hour to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Gửi phút lên Firebase
    if (Firebase.RTDB.setInt(&fbdo, dateTimePath + "/Minute", minute)) {
      Serial.print("Minute: ");
      Serial.println(minute);
    }
    else {
      Serial.println("Failed to send Minute to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Gửi giây lên Firebase
    if (Firebase.RTDB.setInt(&fbdo, dateTimePath + "/Second", second)) {
      Serial.print("Second: ");
      Serial.println(second);
    }
    else {
      Serial.println("Failed to send Second to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
void sendTemperatureAndHumidityToFirebase(float temperature, float humidity) {
  if (Firebase.ready() && signupOK) {
    if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Temperature", temperature)) {
      Serial.print("Temperature : ");
      Serial.println(temperature);
    }
    else {
      Serial.println("Failed to send temperature to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Humidity", humidity)) {
      Serial.print("Humidity : ");
      Serial.println(humidity);
    }
    else {
      Serial.println("Failed to send humidity to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
void sendSpeedToFirebase(float speed) {
  if (Firebase.ready() && signupOK) {
    if (Firebase.RTDB.setFloat(&fbdo, "Speed", speed)) {
      Serial.print("Speed sent to Firebase: ");
      Serial.println(speed);
    }
    else {
      Serial.println("Failed to send speed to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
void displayInfoOnOLED(String date, String time, float temperature, float humidity, float speed) {
  display.clear();  // Clear the display before updating information
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);



  // Display Date and Time
  display.drawString(0, 0, "Date: " + date);
  display.drawString(0, 12, "Time: " + time);

  // Display Temperature and Humidity
  display.drawString(0,24 , "Temp: " + String(temperature) + " C");
  display.drawString(0,36 , "Humidity: " + String(humidity) + " %");

  // Display Speed
  display.drawString(0,48 , "Speed: " + String(speed) + " km/h");

  display.display();  // Update the display
}
void logDataToSD(float temperature, float humidity, int hour, int minute, int second, int date, int month, int year, float longitude, float latitude, float speed) {
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    // Log data in CSV format
    dataFile.print(temperature);
    dataFile.print(",");
    dataFile.print(humidity);
    dataFile.print(",");
    dataFile.print(hour);
    dataFile.print(":");
    dataFile.print(minute);
    dataFile.print(":");
    dataFile.print(second);
    dataFile.print(",");
    dataFile.print(date);
    dataFile.print("/");
    dataFile.print(month);
    dataFile.print("/");
    dataFile.print(year);
    dataFile.print(",");
    dataFile.println(speed);
    dataFile.print(",");
    dataFile.println(doorStatus);
    dataFile.print(",");
    dataFile.print(longitude, 6);
    dataFile.print(",");
    dataFile.print(latitude, 6);


    dataFile.close();
    Serial.println("Data logged to SD card.");
    // Print to terminal when data is successfully logged
    Serial.println("Data saved to datalog.csv: " + String(temperature) + "," + String(humidity) + "," + String(hour) + ":" + String(minute) + ":" + String(second) + "," + String(date) + "/" + String(month) + "/" + String(year) + "," + String(longitude, 6) + "," + String(latitude, 6) + "," + String(speed));
  } else {
    Serial.println("Error opening datalog.csv on SD card.");
  }
}
void receiveDoorStatus() {
  if (mySerial.available() > 0) {
    String doorStatusString = mySerial.readStringUntil('\n');
    Serial.println(doorStatusString);

    // Kiểm tra nếu dữ liệu chứa thông tin trạng thái cửa
    if (doorStatusString.startsWith("Door:")) {
      // Tách thông tin trạng thái cửa từ chuỗi
      String statusValue = doorStatusString.substring(5);

      // Chuyển đổi chuỗi thành giá trị boolean
      doorStatus = (statusValue.toInt() == 1);

      // Gửi trạng thái cửa lên Firebase
      sendDoorStatusToFirebase(doorStatus);

    }
  }
}

// Hàm gửi trạng thái cửa lên Firebase
void sendDoorStatusToFirebase(bool doorStatus) {
  if (Firebase.ready() && signupOK) {
    if (Firebase.RTDB.setBool(&fbdo, "DoorStatus", doorStatus)) {
      Serial.print("Door Status sent to Firebase: ");
      Serial.println(doorStatus ? "Open" : "Closed");
    } else {
      Serial.println("Failed to send Door Status to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
