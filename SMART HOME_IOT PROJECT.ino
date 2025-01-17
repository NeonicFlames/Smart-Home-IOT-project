  #include "Arduino.h"
  #include <SoftwareSerial.h>
  #include <LiquidCrystal_I2C.h> //LCD Library
  #include <SPI.h> //RFID card reader library
  #include <RFID.h> //RFID card reader library
  #include <Wire.h> // LCD library 4 wires library
  #include <dht.h>

  #include <ESP8266_Lib.h>
  #include <BlynkSimpleShieldEsp8266.h>

  #define SDA_DIO 9
  #define RESET_DIO 8
  #define DHT11PIN A2
  #define ventilateurPIN 10 // Pin -> for Fan


  #define BLYNK_PRINT Serial
  #define ESP8266_BAUD 115200



  // LED EVERY ROOM
  #define studyroomLED 22
  #define bedroomLED 24
  #define bathroomLED 26
  #define livingroomLED 28
  #define kitchenLED 30 

  RFID rfid(SDA_DIO, RESET_DIO);
  String rfidCard;

  dht DHT;

  const int ledPin = 11;   //the number of the LED pin
  const int ldrPin = A4;  //the number of the LDR pin

  const int ledpin=13; // ledpin,flamepin and buzpin are not changed throughout the process
  const int flamepin=A3;
  const int buzpin=12;
  const int threshold=100;// sets threshold value for flame sensor
  int flamesensvalue=0; // initialize flamesensor reading 
  bool unlockDoor = false;

  char auth[] = "XXXX"; // Line 49 - 51: Login WI-FI
  char ssid[] = "XXXX";
  char pass[] = "XXXX";

  LiquidCrystal_I2C lcd(0x27, 16, 2); // SDA A4, SCL A5, GDN GND, VCC 5V
  ESP8266 wifi(&Serial1); // Declare pin 18,19 arduino mega

  void setup() {

    Serial.begin(115200);
    Serial1.begin(115200); //begin  ESP01s Baud Rate


    SPI.begin();
    rfid.init();
  
    //rfid
    pinMode(7, OUTPUT);//buzzer
    pinMode(A0, OUTPUT);//led True
    pinMode(A1, OUTPUT);//led False

    //Dht
    pinMode(ventilateurPIN,OUTPUT);//relay Fan
    
    //Photoresistor
    pinMode(ledPin, OUTPUT);  //initialize the LED pin as an output
    pinMode(ldrPin, INPUT);   //initialize the LDR pin as an input

    //flame
    pinMode(ledpin,OUTPUT); 
    pinMode(flamepin,INPUT); 
    pinMode(buzpin,OUTPUT); 


    pinMode(studyroomLED, OUTPUT);
    pinMode(bedroomLED, OUTPUT);
    pinMode(bathroomLED, OUTPUT);
    pinMode(livingroomLED, OUTPUT);
    pinMode(kitchenLED, OUTPUT);
    



    lcd.begin(16,2);
    lcd.backlight();
    lcd.clear();
    
    lcd.setCursor(0,0);
    lcd.print("Connecting To");
    lcd.setCursor(0, 1);
    lcd.print("WI-FI");
    Blynk.begin(auth, wifi, ssid, pass);


    if (Blynk.connected())
    {
      tone(7,1000);
      delay(500);
      noTone(7);
    }
    
    lcd.clear();
    
    lcd.setCursor(0,0);
    lcd.print("Scan your card");
    lcd.setCursor(0,1);
    lcd.print("on the Reader...");



    digitalWrite(kitchenLED, HIGH); 

  /* DEBUG LED
    digitalWrite(studyroomLED, HIGH);5
    digitalWrite(bedroomLED, HIGH); 
    digitalWrite(bathroomLED, HIGH); 
    digitalWrite(livingroomLED, HIGH); 
    digitalWrite(kitchenLED, HIGH); 
  */                                                               
  } 


  void loop() {

    Blynk.run();
    RFIDFunc();
    DHTFunc();
    photoResistor();
    flameDetection();
  }
    



  void RFIDFunc()
  {
    if (rfid.isCard() || unlockDoor == 1) {
        if (rfid.readCardSerial() || unlockDoor == 1) {
          rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
          Serial.println(rfidCard);
          if (rfidCard == "85 205 177 42" || unlockDoor == 1) {

            Serial.println("********************");
            Serial.println("Door Unlocked");
            Serial.println("********************");
            delay (200);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Door Unlocked");
            delay (1000);
            Blynk.logEvent("valid_rfid","Someone has entered valid RFID to the house");
            digitalWrite(A0, HIGH);
            tone(7,1000);
            delay (500);
            digitalWrite(A0, LOW);
            noTone(7);
            delay(5000);

            digitalWrite(A1, HIGH);
            tone(7,1000);
            delay (500);
            digitalWrite(A1, LOW);
            noTone(7);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Door Locked");
            delay (1000);


            unlockDoor = false;

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Scan your card");
            lcd.setCursor(0,1);
            lcd.print("on the Reader...");
          }
          else {
            Serial.println("****************");
            Serial.println("Access Denied");
            Serial.println("****************");
            //for LCD
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Access Denied");
            lcd.setCursor(0,1);
            lcd.print("Try Again");
            delay (1000);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Scan your card");
            lcd.setCursor(0,1);
            lcd.print("on the Reader...");
            //Buzzer & RED LED(FALSE)
            digitalWrite(A1, HIGH);
            tone(7,1000);
            delay (500);
            digitalWrite(A1, LOW);
            noTone(7);

            unlockDoor = false;
            Blynk.logEvent("invalid_rfid","An invalid RFID have been applied to the door!");
            return;
          }
        }
        rfid.halt();
      }
  }

    void DHTFunc() // Need to check the pins back.
    {
    int chk = DHT.read11(DHT11PIN);
    Serial.print("Temperature = ");
    Serial.println(DHT.temperature);
    Serial.print("Humidity = ");
    Serial.println(DHT.humidity);
    Blynk.virtualWrite(V4, DHT.temperature);

    /*  if ((float)DHT11.temperature >23) {
        digitalWrite(ventilateurPIN,HIGH); // the fan starts to turn
      }
      else {
        digitalWrite(ventilateurPIN,LOW); // the fan stops
      }
    */
    }


    bool photoresistorswitch = false;
    void photoResistor()
    {
      int ldrStatus = analogRead(ldrPin);   //read the status of the LDR value

      //check if the LDR status is <= 25
      //if it is, the LED is HIGH
        if (ldrStatus <= 540) {
          digitalWrite(ledPin, HIGH);               //turn LED on
          Serial.println("LDR is DARK, LED is ON");
          Serial.println(ldrStatus);
          delay(1000);
        }
        else if (photoresistorswitch == true) {
          digitalWrite(ledPin, HIGH);
          Serial.println("LDR is DARK, LED is ON");
          Serial.println(ldrStatus);
          delay(1000);
        }
        else if (ldrStatus >= 540)
        {
          digitalWrite(ledPin, LOW);          //turn LED off
          Serial.println("LDR is BRIGHT, LED is OFF");
          Serial.println(ldrStatus);
          photoresistorswitch = false;
          delay(1000);
        }
        else if (photoresistorswitch == false)
        {
          digitalWrite(ledPin, LOW);          //turn LED off
          Serial.println("LDR is BRIGHT, LED is OFF");
          Serial.println(ldrStatus);
          photoresistorswitch = false;
          delay(1000);
        }

        //Serial.println(photoresistorswitch); debug photoresistor switch
    }

    void flameDetection()
    {
      flamesensvalue=analogRead(flamepin); // reads analog data from flame sensor
      if (flamesensvalue<=threshold) // compares reading from flame sensor with the threshold value 
      { 
          digitalWrite(ledpin,HIGH); //turns on led and buzzer
          Blynk.logEvent("fire_detection","Fire has been detected!!!");
          tone(buzpin,1000); 
          delay(100); //stops program for 1 second 
      } 
      else
      { 
          digitalWrite(ledpin,LOW); //turns led off led and buzzer 
          noTone(buzpin); 
      } 

    }



  BLYNK_WRITE(V1) { //Fan
    int pinValue = param.asInt();

    if (pinValue == 1) {
      digitalWrite(ventilateurPIN,HIGH); // fan = on
    } else {
      digitalWrite(ventilateurPIN,LOW); // fan = off
    }
  }

  BLYNK_WRITE(V2) { //Unlock Door
    int pinValue = param.asInt();

    if (pinValue == 1)
      unlockDoor = true;
    else
      unlockDoor = false;
  }

  BLYNK_WRITE(V3) { //PhotoResistor
    int pinValue = param.asInt();

    if (pinValue == 1)
      photoresistorswitch = true;
    else
      photoresistorswitch = false;
  }



  BLYNK_WRITE(V5) { //Bedroom LED
    int pinValue = param.asInt();

    if (pinValue == 1)
    digitalWrite(bedroomLED, HIGH);
    else
    digitalWrite(bedroomLED, LOW);
  }

  BLYNK_WRITE(V6) { //Bathroom LED
    int pinValue = param.asInt();

    if (pinValue == 1)
    digitalWrite(bathroomLED, HIGH);
    else
    digitalWrite(bathroomLED, LOW);
  }

  BLYNK_WRITE(V7) { //Living Room LED
    int pinValue = param.asInt();

    if (pinValue == 1)
    digitalWrite(livingroomLED, HIGH);
    else
    digitalWrite(livingroomLED, LOW);
  }

  BLYNK_WRITE(V8) { //Kitchen LED
    int pinValue = param.asInt();

    if (pinValue == 1)
    digitalWrite(kitchenLED, HIGH);
    else
    digitalWrite(kitchenLED, LOW);
  }

  BLYNK_WRITE(V9) { //Study Room LED
    int pinValue = param.asInt();

    if (pinValue == 1)
    digitalWrite(studyroomLED, HIGH);
    else
    digitalWrite(studyroomLED, LOW);
  }







