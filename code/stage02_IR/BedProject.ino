/*
 * This Project is used in project Bed ver 0.1 - stage 02 include IR
 * the project contains access to bed buttons using relays
 * also contain IR reciever for using the cabels remote control 
 * 
 * 
 * IRLib from https://github.com/z3t0/Arduino-IRremote
 * IRLib – an Arduino library for infrared encoding and decoding
 * Version 1.51 March 2015 
 * Copyright 2013-2015 by Chris Young http://tech.cyborg5.com/irlib/
 */
#include <IRLib.h>

// pin 11 defined for IR signal
int RECV_PIN = 11;

IRrecv My_Receiver(RECV_PIN);

IRdecode My_Decoder;
unsigned int Buffer[RAWBUF];

// the following pin connect to relays
int HeadUp =2; //the lower pin 
int HeadDown=3;
int LegsUp=4;
int LegsDown=5; //the higer pin

unsigned long startTime;

//each move is 400ms unless interrupt (stop bumpings)
int duration = 400;

// currentState hold the active pin number or 99 on idel (for motions & interrupt)
int currentState =99;

void setup()
{ 
  //init pins
  int inMin = HeadUp; // Lowest output pin
  int inMax = LegsDown; // Highest output pin
  for(int i=inMin; i<=inMax; i++)
  {
    pinMode(i, OUTPUT);
  }
  
  //perform relays checkup
  for(int i=inMin; i<=inMax; i++)
  {
    digitalWrite(i, LOW);
    delay(150);
    digitalWrite(i,HIGH);
    delay(150);
  }

  //open serial port for debug
  Serial.begin(9600);
  delay(2000);while(!Serial);//wait from serial init

  //init IR
  My_Receiver.enableIRIn(); // Start the receiver
  My_Decoder.UseExtnBuf(Buffer);

  Serial.print("Ready...");
}

void loop() {

  // if pin is active wait the default duration
  if ( currentState != 99)
  {
    if (millis() - startTime  >= duration) {
      currentState=99;
      StopMovments();
    }
  }

  // IR got signal
  if (My_Receiver.GetResults(&My_Decoder)) {
    //Restart the receiver so it can be capturing another code
    //while we are working on decoding this one.
    
    My_Decoder.decode();

    //debug: print the IR value 
    Serial.println(My_Decoder.value);

    int result = My_Decoder.value;
    //check if IR value is relevant to our system
    switch (My_Decoder.value){
      case 4448: //CH+
        Movments(HeadUp);
        break;
      case 4449: //CH-
        Movments(HeadDown);
        break;
      case 4432: //Vol+
        Movments(LegsUp);
        break;
      case 4433: //Vol-
        Movments(LegsDown);
        break;
      case 6480: //Vol+
        Movments(LegsUp);
        break;
      case 6481: //Vol-
        Movments(LegsDown);
        break;
      case 6496: //CH+
        Movments(HeadUp);
        break;
      case 6497: //CH-
        Movments(HeadDown);
        break;
      default:
      Serial.print("nothing else ...");
        StopMovments();
        break;
    }
  
   
    My_Receiver.resume(); 
  }
  }

  /* 
   *  This Sub stops all moves by changing pinModes to HIGH
   */
  void StopMovments()
  {
    int inMin = 2; // Lowest input pin
    int inMax = 9; // Highest input pin
    for(int i=inMin; i<=inMax; i++)
    {
      digitalWrite(i, HIGH);
    }
  }
  /*
   * This sub change status of certain pin with minial delay to LOW
   */
void Movments(int PIN)
{
  if (PIN != currentState)
  {
    StopMovments();
    currentState= PIN;
  }
    digitalWrite(PIN,LOW);
    delay(500);
    startTime = millis();

}
 
