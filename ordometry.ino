#include<Servo.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#define SOFTWAREREG         0x0D                           //Byte to read the software version. Values of 0 sent using 'write' have to be cast as a byte not to be misinterperted as NULL
#define MD25ADDRESS         0x58                           //Address of the MD25
#define CMD                 0x10                           //Byte to 'write' to the CMD
#define SPEEDR              0x00                           //Byte to send speed to first motor
#define SPEEDL              0x01                           //Byte to send speed to second motor
#define ENCODER1            0x02                           //Highest byte of motor encoder 1
#define ENCODER2            0x06                           //Highest byte of motor encoder 2
#define VOLTREAD            0x0A                           //Byte to read battery volts
#define RESETENCODERS       0x20                           //Byte to reset encode registers to 0
#define ACCELERATION        0xE


int pos3 = 0;                                              //initialising position of servo                                       //array of 2 leds
int counter = 0;                                           //counter initialiser
long poss2 = Wire.read();                                  //reads the poss2 from the i2c
Servo myservo;  
int ledPin12 = 12;//giving the servo a variable name

void setup(){
  myservo.attach(10);                                      //attaching servo to pin 10
  myservo.write(pos3);                                     //sets up the servo to pos3 (0);
  Wire.begin();                                            //begin wire library
  delay(200);                           
  byte softVer = getSoft();                                //byte to software version 
  encodeReset();
  Serial.begin(9600);//resetsEncoder at the start
  ledBlink();
  pinMode(ledPin12, OUTPUT);
}

void encodeReset(){                                        //Function to set the encoder values to 0
  Wire.beginTransmission(MD25ADDRESS);                     //MD25 address
  Wire.write(CMD);                                         //'Write' to the CMD
  Wire.write(RESETENCODERS);                               //Reset the encode registers to 0
  Wire.endTransmission(); 
}

void accelerate() {
    Wire.beginTransmission(MD25ADDRESS);
    Wire.write(ACCELERATION);
    Wire.write(10);
    Wire.endTransmission();
}
//  
//void decelerate(int d) {
//    long poss2 = encoder2();
//    if(abs(poss2) > 0.5*abs(d)) {
//    
//    Wire.beginTransmission(MD25ADDRESS);
//    Wire.write(SPEEDR);
//    Wire.write(148);
//    Wire.endTransmission();
//
//    Wire.beginTransmission(MD25ADDRESS);
//    Wire.write(SPEEDL);
//    Wire.write(148);
//    Wire.endTransmission();
//    delay(300);
//    }
//}

byte getSoft(){                                            //Function to get the software version
Wire.beginTransmission(MD25ADDRESS);                       //MD25 address
Wire.write(SOFTWAREREG);                                   //Send byte to read software version as a single byte
Wire.endTransmission();
Wire.requestFrom(MD25ADDRESS, 1);                          //Request one byte from MD25
while(Wire.available() < 1);                               //Wait for it to arrive
byte software = Wire.read();                               //Read it in
return(software);
}

long encoder1(){                                            
  Wire.beginTransmission(MD25ADDRESS);                      //address of MD25
  Wire.write(ENCODER1);                                     //Send byte to read the position of encoder 1
  Wire.endTransmission();
  Wire.requestFrom(MD25ADDRESS, 4);                         //Requesting 4 bytes from MD25
  while(Wire.available() < 4);                              //Wait for 4 bytes to arrive
  long poss1 = Wire.read();                                 //First byte for encoder 1, HH.
  poss1 <<= 8;
  poss1 += Wire.read();                                     //Second byte for encoder 1, HL
  poss1 <<= 8;
  poss1 += Wire.read();                                     //Third byte for encoder 1, LH
  poss1 <<= 8;
  poss1  +=Wire.read();  //Final byte for encoder 1, LL//Wait for everything to make sure everything is sent;
  return(poss1);//Return poss1 as a long
}

void ledBlink() {                                           //led blink function over ten leds
      digitalWrite(ledPin12, HIGH);                       //led pin HIGH
      delay(100);
      digitalWrite(ledPin12, LOW);                      //led pin LOW
      delay(100);
  }

long stopMotor(){    
  
  
  //Function to stop motors
  Wire.beginTransmission(MD25ADDRESS);                      //MD25 address
  Wire.write(SPEEDR);                                       //'Write' to motor 1
  Wire.write(128);                                          //Send a value of 128 to motor 1 to stop it
  Wire.endTransmission();
  Wire.beginTransmission(MD25ADDRESS);                      //MD25 address
  Wire.write(SPEEDL);                                       //'Write' to motor 2
  Wire.write(128);                                          //Send a value of 128 to motor 2 to stop it
  Wire.endTransmission();
}

void servo() {                                              //runs the servo function and will be called within the loop
    counter++;                                              //will count plus one every time servo is called
    if(counter % 1 == 0) {                                  //if counter / 1 then add 34 degrees to servo
      pos3 = pos3 + 34;     
      myservo.write(pos3);                                  //new value of pos3. Servo modified
      delay(200);     
    }
}
  
long encoder2(){                                            //Function to read and display value of encoder 2 as a long
  Wire.beginTransmission(MD25ADDRESS);                      //MD25 address
  Wire.write(ENCODER2);                                     //Send byte to read the position of encoder 2
  Wire.endTransmission();
  Wire.requestFrom(MD25ADDRESS, 4);                         //Request 4 bytes from MD25
  while(Wire.available() < 4);                              //Wait for 4 bytes to become available
  long poss2 = Wire.read();                                 //First byte for encoder 2, HH
  poss2 <<= 8;
  poss2 += Wire.read();                                     //Second byte for encoder 2, HL
  poss2 <<= 8;
  poss2 += Wire.read();                                     //Third byte for encoder 2, LH
  poss2 <<= 8;
  poss2  +=Wire.read();//Final byte for encoder 2, LL
  Serial.println(poss2);
  return(poss2);
  
}
void Rwheelforward(int R, int L, double d){//Function to drive motors forward, setting motor 1 speed to R, motor 2 speed to L and turning the wheels by d degrees
    encodeReset();
    do {
      
    Wire.beginTransmission(MD25ADDRESS);                    //Start connection to MD25 for motor 1 (right wheel)
    Wire.write(SPEEDR);                                     //'Write' to motor 1
    Wire.write(R + 128);                                    //Set right wheel to speed R + 128 (giving base as zero)
    Wire.endTransmission();
    
    Wire.beginTransmission(MD25ADDRESS);                    //Start connection to MD25 for motor 2 (left wheel)
    Wire.write(SPEEDL);                                     //'Write' to motor 2
    Wire.write(L + 128);                                    //Set left wheel to speed L + 128 (giving base as zero)
    Wire.endTransmission();
    delay(50);

     }while(encoder2() < d);                                //while encoder if less than d perform above do statement otherwise go to next if statement below
      long poss4 = abs(encoder2())-abs(d);                    //correction code, if the enocder goes too far then it will call RwheelturnCorrect.
      if(poss4 > 0 && R < 0) {
        
      RwheelturnCorrect((R+(-1.5*R)), (L-((1.5)*L)), -d);
      stopMotor();
      delay(50);
      }
      stopMotor();                                          //stops motor at the end of while loop.
    delay(50);
}

void RwheelturnCorrect(int R, int L, double d) {            //uses the difference in values to correct itself.
    
    if(abs(encoder2()) - abs(d) > 0) {
    Wire.beginTransmission(MD25ADDRESS);                    //Start connection to MD25 for motor 1 (right wheel)
    Wire.write(SPEEDR);                                     //'Write' to motor 1
    Wire.write(R + 128);                                    //Set right wheel to speed R + 128 (giving base as zero)
    Wire.endTransmission();
    
    Wire.beginTransmission(MD25ADDRESS);                    //Start connection to MD25 for motor 2 (left wheel)
    Wire.write(SPEEDL);                                     //'Write' to motor 2
    Wire.write(L + 128);                                    //Set left wheel to speed L + 128 (giving base as zero)
    Wire.endTransmission();
    RwheelturnCorrect(R,L,d);                               //call function until statement is no longer true
    }else {
      stopMotor();
      delay(200);
    }
}

void Rwheelbackward(int R, int L, double d){                  //Function to drive motors backwards, setting motor 1 speed to R, motor 2 speed to L and turning the wheels by d degrees
    encodeReset();
 do{
    //acceleration();
   // decelerate(d);
    Wire.beginTransmission(MD25ADDRESS);
    Wire.write(SPEEDR);                                     //'Write' to motor 1
    Wire.write(R + 128);                                    //Set right wheel to speed R + 128 (giving base as zero)
    Wire.endTransmission();
    
    Wire.beginTransmission(MD25ADDRESS);                    //Start connection to MD25 for motor 2 (left wheel)
    Wire.write(SPEEDL);                                     //'Write' to motor 2
    Wire.write(L + 128);                                    //Set left wheel to speed L + 128 (giving base as zero)
    Wire.endTransmission();
    delay(50);
    
  }
   while(encoder2() > d);                                   //same error correction as above but for turning the other way
   
   long poss3 = abs(encoder2())-abs(d);
   
   if(poss3 > 0 && R > 0 && L < 0) {
    
    RwheelturnCorrect(-(0.5*R), -(0.5*L), -d);
    stopMotor();
    delay(200);
    
    }
    stopMotor();
      delay(200);
}

void Rwheelcorrect(int d) {                                 //error correction for going forwards or backwards

  if(abs(encoder2())-abs(d) > 0) {

    Wire.beginTransmission(MD25ADDRESS);
    Wire.write(SPEEDR);
    Wire.write(120);
    Wire.endTransmission();
    
    Wire.beginTransmission(MD25ADDRESS);
    Wire.write(SPEEDL);
    Wire.write(120);
    Wire.endTransmission();
    Rwheelcorrect(d);
    stopMotor();
    }
}


void RwheelcorrectBack(int d) {
  long poss2 = encoder2();
  if(abs(poss2-d) > 0) {
    Wire.beginTransmission(MD25ADDRESS);
    Wire.write(SPEEDR);
    Wire.write(168);
    Wire.endTransmission();
    
    Wire.beginTransmission(MD25ADDRESS);
    Wire.write(SPEEDL);
    Wire.write(168);
    Wire.endTransmission();
    RwheelcorrectBack(d);
    stopMotor();
    }
}

void arccorrect(int R, int L, int d) {                    //arc turn error correction which takes 3 parameters and reverses the error on arcs
    if(abs(encoder2()) - abs(d) > 0) {                    // same logic as above. If difference is greater than 0 perform logic.  
    Wire.beginTransmission(MD25ADDRESS);
    Wire.write(SPEEDR);
    Wire.write(-(R)+128);
    Wire.endTransmission();
    
    Wire.beginTransmission(MD25ADDRESS);
    Wire.write(SPEEDL);
    Wire.write(-(L)+128);
    Wire.endTransmission();    
    arccorrect(R, L, d);
    }
    stopMotor();                                          //else stopMotor()
}
  
void loop(){                                                //loops through all our functions

delay(25);
Rwheelforward(60, 60, 390);                 //0 to 1
delay(25);
Rwheelcorrect(390);
delay(25);
ledBlink();
delay(25);
Rwheelbackward(30,-30, -245);               //turn at 1
delay(25);                                 //light led
Rwheelforward(60, 60, 298.2);                 //1 to 2
delay(25);
Rwheelcorrect(298.2);
delay(25);
Rwheelforward(-30, 30, 245);              //turn 2
delay(25);
servo();                                    //drop m&m
delay(25);
ledBlink();
delay(25);//light led 
Rwheelforward(60, 60, 565);                 //2 to 3
delay(25);
Rwheelcorrect(565);
delay(25); 
ledBlink();                                 //light led
delay(25);
Rwheelforward(-30, 30, 245);               //turn 3
delay(25);
Rwheelforward(32, 10, 202);                //arc 3 to 4
delay(25);
arccorrect(32, 10, 202);                                   //drop m&m
delay(25);
Rwheelforward(10,10,20);
delay(25);
Rwheelcorrect(20);
delay(25);
servo();
delay(50);
Rwheelbackward(30,-30,-245);                //turn 4 \
delay(50);
ledBlink();                                  // light led
delay(25);
Rwheelforward(60, 60, 730);                 //4 to 5
delay(25);
Rwheelcorrect(730);
delay(25);
ledBlink();                                 //light led
Rwheelforward(-30, 30, 245);                  // turn 5
delay(25);
Rwheelforward(50,50,458.8);
delay(25);                 
Rwheelcorrect(458.8);
delay(25);
servo();
ledBlink();                                 // led light up
Rwheelbackward(-30,-30, 60);
delay(25);//drop m&m
Rwheelforward(-30, 30, 245);                //turn at 6
delay(25);
Rwheelforward(60, 60, 458.8);
delay(25);
Rwheelcorrect(458.8);                       //at 7
delay(25);
Rwheelforward(-30, 30, 245);                //turn 7
delay(25);
Rwheelforward(60, 60, 458.8);                 //7 - 8
delay(25);
Rwheelcorrect(458.8);
delay(25); 
ledBlink();                                 //light led
delay(25);
servo();                                    //drop m&m
delay(25);
Rwheelforward(-30, 30, 115);                  //turn 8       
delay(25);
Rwheelforward(60, 60, 680);                   //8-9
delay(25);
Rwheelcorrect(680);
delay(25);
Rwheelbackward(30, -30, -80);                //turn 9
delay(25);
Rwheelbackward(-30, -30, -210);             //9-10
delay(25);
ledBlink();                                 //light blink
delay(25);
servo();                                    //drop m&m
delay(25);
Rwheelbackward(40,-40, -245);              //turn  at 10       
delay(25);    
Rwheelbackward(-10, -50, -1760);           // 10 - 11
delay(25);
Rwheelbackward(-10,-10,-10);                
delay(25);
Rwheelbackward(40,-40, -90.6);             //turn 11
delay(25);
Rwheelforward(60,60,383.3);                //11-12
delay(25);
Rwheelcorrect(383.3);
delay(25);
ledBlink();                               //12-13
Rwheelforward(60,60,453.2);               //13-14
delay(25);
Rwheelcorrect(453.2);
delay(25);
ledBlink();                               //light led

//delay(10000);
//stopMotor();

}


