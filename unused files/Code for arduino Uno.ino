#include "Arduino.h"
#include <SoftwareSerial.h>
#include <dht11.h>

#define Pin3  7
#define Pin7  6
#define Pin6  5
#define Pin5  4
#define Pin4  3   
#define DHT11PIN 13                                  

const byte rxPin = 9;
const byte txPin = 8;
SoftwareSerial BTSerial(rxPin, txPin); // RX TX

dht11 DHT11;

void setup() {
// define pin modes for tx, rx:
pinMode(rxPin, INPUT);
pinMode(txPin, OUTPUT);
pinMode(Pin6, OUTPUT); 
pinMode(Pin7, OUTPUT); 
pinMode(Pin3, OUTPUT); 
pinMode(Pin5, OUTPUT); 
pinMode(Pin4, OUTPUT);
BTSerial.begin(9600);
Serial.begin(9600);
}

String messageBuffer = "";
String message = "";

void loop() {
  while (BTSerial.available() > 0) {
    char data = (char) BTSerial.read();
    messageBuffer += data;
    
    /*if (data == ';'){
      message = messageBuffer;
      messageBuffer = "";
      Serial.print(message); // send to serial monitor
      message = "You sent " + message;
      BTSerial.print(message); // send back to bluetooth terminal
    }*/
  /*  //DHT Sensor
    int chk = DHT11.read(DHT11PIN);
    
    Serial.print("Humidity (%): ");
    Serial.println((float)DHT11.humidity, 2);
    Serial.print("Temperature (°C): ");
    Serial.println((float)DHT11.temperature, 2);

    /*BTSerial.print("Humidity (%): ");
    BTSerial.println((float)DHT11.humidity, 2);
    BTSerial.print("Temperature (°C): ");
    BTSerial.println((float)DHT11.temperature, 2);*/

    if(data == 'a'){ digitalWrite(Pin3, HIGH);}
    if(data == 'b'){ digitalWrite(Pin3, LOW);} 
    if(data == 'c'){ digitalWrite(Pin7, HIGH);}
    if(data == 'd'){ digitalWrite(Pin7, LOW);}
    if(data == 'e'){ digitalWrite(Pin6, HIGH);}
    if(data == 'f'){ digitalWrite(Pin6, LOW);}
    if(data == 'g'){ digitalWrite(Pin5, HIGH);}
    if(data == 'h'){ digitalWrite(Pin5, LOW);}
    if(data == 'i'){ digitalWrite(Pin4, HIGH);}
    if(data == 'j'){ digitalWrite(Pin4, LOW);}
  }

}