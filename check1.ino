#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define RX D2
#define TX D3

#define BLYNK_TEMPLATE_ID "TMPL3sV9co2sT"
#define BLYNK_TEMPLATE_NAME "real time fluid monitoring system"
#define BLYNK_AUTH_TOKEN "EtvTL_Y25k2OqCy71Ne8IFukMgSY9N2s"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "pavan";  // Replace with your WiFi SSID
char pass[] = "pavan021";  // Replace with your WiFi password

SoftwareSerial mySerial(RX, TX); // RX, TX

void setup() {
  Serial.begin(9600);      // Initialize Serial Monitor
  mySerial.begin(9600);    // Initialize SoftwareSerial to communicate with Arduino

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();  // Run Blynk
  if (mySerial.available()) {
    String data = mySerial.readStringUntil('\n');
    int separatorIndex = data.indexOf(',');
    if (separatorIndex != -1) {
      String intPart = data.substring(0, separatorIndex);
      String floatPart = data.substring(separatorIndex + 1);
      int intValue = intPart.toInt();
      float floatValue = floatPart.toFloat();
      Serial.print("BPM: ");
      Serial.println(intValue);
      Serial.print("weight: ");
      Serial.println(floatValue);

      // Send data to Blynk
      Blynk.virtualWrite(V0, intValue);   // Send BPM to Virtual Pin V0
      Blynk.virtualWrite(V1, floatValue); // Send weight to Virtual Pin V1
    }
  }
}
