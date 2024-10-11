#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <Wire.h>
#include <ESP32Servo.h>
#include <mcs_tracking_inferencing.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SDA 15
#define SCL 14

using eloq::camera;
using eloq::ei::fomo;

Servo servo1;
Servo servo2;

const int Led_Flash = 4;
const int trigPin = 12;
const int echoPin = 13;

const int servo1_pin = 14;
const int servo2_pin = 15;

boolean startTimer = false;
unsigned long time_now=0;
int time_capture=0;
long duration;
int distance;

// Replace with your network credentials
const char* ssid = "PANG";
const char* password = "panG21.07";

// Google Script ID
const char* googleScriptID = "AKfycbxKGnIK2lY-vseYAegzjgQOCekAXv_3PQaz98H0q7weUJ-Qe3b4dhB2qkpmtYAi3fXN";

// Updated 11.10.2024
const char* root_ca = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCu7qg4XqYYUtvA\nuc+2dvIABADaqXS7fCFWqF4J0BnqmEUHrarMBE8FzX7aNwnCNQTGchYjZnwHvofb\nRN6IxeZsevaoKsUGUcTi76xWhvpsV/tij2ki5MWhnT+5ji6I75Cluw/57r8QOR+w\nDAjGS9MGLyo4VeIXtPviqf3Y1mgioyvZ1WLj0Bgpao/n4oD278Dn9p8GfVRslVm0\n58wo3ZnEKXK8srmMisDE3tjq70ypfLJlxsSPW7CHDz7egImVfQvf+HxTrLzZehOz\nUL6tbfP1vpKg5Reu7bNpBXUlQs569He8fL7vKmnx3xxcUMFqcVxbn8OwpL3zKIfm\n99DX7r/RAgMBAAECggEAGLpDrTQz5lxQE/4wDmrh6n6mYEJz7cyuKcHDCUARacf8\neglii1RQl1FlDROanYlBE4ws9PQXtECKk8Q4cL0IxWzcUQk6sJn+atXWVqTd4VLO\nhjAffH86MI4549DlYxX6LHM03u+QsopgoFITK3Ssb8NbBHufahp7QJ8m8u/b6IlM\nT0huG21DFMddM93YPRIuip4GE70GHb2d2infFCWp5D11tBA9OP22UuqL2H/6O8cJ\nh8sM8HIMtjLQNoxV7KaP9FPTNG8xuudqQh0/qZKtz7Y03yQiw6AwPcxiEsJWuV0H\noQQPQ/ec/oUVSpqoZzzNBqg5KO7IDFXcE8I0En++wQKBgQDfXJBecxvvcYRvWmtr\n+16j20RCifFa0360qBcSFQKBGGZVcjdpzvHcgb73wiixWXWfXrRNt1rjUh7lEf+M\n8HAbgMLZirBBZU+8Y/lpPpOMgQ/an68W8sjZBXcKZDYIZBm5OMfEaGbWR/28eO8N\nzrnKip9KWwvpz9jn0pbC5K6bPwKBgQDIfncNLS8dm8Ff6SAWcxlqEhjrP+M/KK5r\nzH3Kfda6qcpbJg/V5YeqxDSxxBU7fDTpDnZB6UZL355gnUpCQTk/Nt7Emg2OL9Kn\nhDDRfH6tP4O0Ud+7i95yGV6rVJpwbrymsV3ecx6hpIRrZH0IAUQNhUAlvNazjezL\nlflggDAw7wKBgQDKCj+ov7SoLWWfGAoNPe6IhyFCkuZXwvFc1qMvvwNp5LlEVzFa\nq0/Jpy8ni2AltAcaZp7voYg8EZ9Sk6o2QZMWva0C/FmcfdjXTxNfKLRspp3BoynM\nt0YIUNIx+x2JD4rZKUxCFDHZaHtTTYznT3RtimC18I9hKppIcAvhNNvgHwKBgDho\n1IghiVsUQpEAn2WRlrnwQ5CU1hQkFwb/bvj09OBp+yQwDk6JGOWj9It7dtnYtW5J\nAl7DCmM4lG/scUM45Klzf7PjACNcMzGskKuwElgeQakPTBqRXNHPb4JW+GjfzzY7\nOwTmcNzb0+H/UOpaqNIBBq5XdjTonpIdLR/IkUaDAoGAOLI/tk0gQsqjLAP2FfI1\nrcqStS5tDDGdbDVxHgn1Ut1Sc4g41BGmnJtJaL2wwZM6uauzVitd6wqPNT8vcEHz\nq7DpSFLp+hlkzH4nehOMfJGlbUsF5oev2ATJkmPF/5ZDzo8AazKRGxmo1wiNm0HI\nJYI9daZD4pJiyiQC86Ua/I0=\n-----END PRIVATE KEY-----\n";


// Function to follow redirects until the final URL
String followRedirects(String url) {
  HTTPClient http;
  http.begin(url, root_ca);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpResponseCode = http.GET();
  String payload = http.getString();

  while (httpResponseCode == HTTP_CODE_FOUND || httpResponseCode == HTTP_CODE_MOVED_PERMANENTLY || httpResponseCode == HTTP_CODE_TEMPORARY_REDIRECT) {
    // Get the redirected URL
    url = http.getLocation();
    http.end();
    http.begin(url, root_ca);
    httpResponseCode = http.GET();
    payload = http.getString();
  }
  
  http.end();
  return url;
}

// Function to send data to Google Sheets
void sendDataToGoogleSheets(float value1, float value2) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Your Google Script URL
    String url = String("https://script.google.com/macros/s/") + googleScriptID + "/exec";
    url = followRedirects(url);

    // Prepare JSON payload
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["value1"] = value1;
    jsonDoc["value2"] = value2;

    String payload;
    serializeJson(jsonDoc, payload);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable detector
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT);
    pinMode(Led_Flash, OUTPUT);
    servo1.attach(servo1_pin);
    servo2.attach(servo2_pin);
    // camera settings
    // replace with your own model!
    camera.pinout.aithinker();
    camera.brownout.disable();
    // NON-PSRAM FOMO only works on 96x96 (yolo) RGB565 images
    camera.resolution.yolo();
    camera.pixformat.rgb565();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());
    
    Serial.println("Camera OK");
    Serial.println("Put object in front of camera");

}


void loop() {

  long duration, cm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  if( distance <= 7){ 
      //detection 
      // capture picture
      if (!camera.capture().isOk()) {
          Serial.println(camera.exception.toString());
          return;
      }

      // run FOMO
      if (!fomo.run().isOk()) {
        Serial.println(fomo.exception.toString());
        return;
      }

      // how many objects were found?
      Serial.printf(
        "Found %d object(s) in %dms\n", 
        fomo.count(),
        fomo.benchmark.millis()
      );

      // if no object is detected, return
      if (!fomo.foundAnyObject())
        return;

      // if you expect to find a single object, use fomo.first
      Serial.printf(
        "Found %s at (x = %d, y = %d) (size %d x %d). "
        "Proba is %.2f\n",
        fomo.first.label,
        fomo.first.x,
        fomo.first.y,
        fomo.first.width,
        fomo.first.height,
        fomo.first.proba
      );

      if(fomo.first.label == "plastic" || fomo.first.label == "glass"  || fomo.first.label == "can" ){  
        Serial.println("OK");
        servo1.write(90);
        servo2.write(90);
        sendDataToGoogleSheets(1.0,1.0);
        delay(500);
      }
      else{
        Serial.println("Not Found...  Try Again");
      }
    }
  delay(200);
  servo1.write(0);
  servo2.write(0);
}
