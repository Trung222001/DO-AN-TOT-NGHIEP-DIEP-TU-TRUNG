#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 4    // Chân dữ liệu của DHT 11, với NodeMCU chân D5 GPIO là 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define BUZZER_PIN D4

SoftwareSerial mySerial(12, 14); // TX, RX
           
unsigned long sendDataPrevMillis = 0;
int count = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();

  mySerial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.println(String(temperature) + "," + String(humidity));
  mySerial.println(String(temperature) + "," + String(humidity));

  // Kiểm tra điều kiện và điều khiển còi báo
  if (temperature > 20.0) {
    tone(BUZZER_PIN, 5000); // 5000 Hz là một ví dụ, bạn có thể điều chỉnh theo nhu cầu
    delay(1000);
    noTone(BUZZER_PIN);
    delay(1000);
  }

  delay(500);
}
