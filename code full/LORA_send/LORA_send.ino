#include <SoftwareSerial.h>
#include <DHT.h>
#define DHTPIN 2    // Chân dữ liệu của DHT 11 , với NodeMCU chân D5 GPIO là 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


SoftwareSerial mySerial(12, 14); //TX, RX

SoftwareSerial SerialGPS(4, 5);

const int mc38aPin = 2; 
void setup() {
  Serial.begin(9600);
  dht.begin();
  mySerial.begin(9600);
  pinMode(mc38aPin, INPUT);
  
}

void loop() {
  int doorState = digitalRead(mc38aPin);
    if (doorState == HIGH) {
    Serial.println("Cửa đã mở");
    sendDoorStatus(true);
    // Thực hiện các hành động khi cửa mở
  } else {
    Serial.println("Cửa đã đóng");
     sendDoorStatus(false); 
    // Thực hiện các hành động khi cửa đóng
  }
  float humidity = dht.readHumidity();
  float  temperature= dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
    }  
  
  Serial.println(String(temperature) + "," + String(humidity));
  mySerial.println(String(temperature) + "," + String(humidity));

  delay(1000);
}
void sendDoorStatus(bool isDoorOpen) {
  if (isDoorOpen) {
    mySerial.println("Door is open");
  } else {
    mySerial.println("Door is closed");
  }
}