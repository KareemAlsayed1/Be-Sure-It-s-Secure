#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <NewPing.h>
#include <LiquidCrystal.h>
#include "HX711.h"
#include <Servo.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define LS 2      // left sensor line tracking
#define RS 3      // right sensor line tracking
/*-------definning Outputs------*/
#define LM1 20       // left motor
#define LM2 19       // left motor
#define RM1 18      // right motor
#define RM2 17      // right motor
#define Speed 7
/*-------Color sensor----------*/
#define S0 39
#define S1 41
#define S2 42
#define S3 43
#define sensorOut 44
/*-------Weight sensor--------*/
#define DOUT 5
#define CLK  6

/*-------Buzzer-------------*/
int Buzzer = 34;

/*-------LED---------------*/
int Red   = 36;
int Green = 38;

/*---------LCD-----------------*/
LiquidCrystal lcd (22,24,26,28,30,32);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
RF24 radio(40,53);  // make sure this corresponds to the pins you are using
Servo myservo;
HX711 scale(DOUT, CLK);


unsigned long lcdtime = millis();
unsigned long buzzertime = millis();
unsigned long Btime = millis();
unsigned long Tconnection = millis ();
int Mconnection = 0;
int Cconnection = 0;
int Sconnection = 0;
int distance;
int off = 0;
int frequency = 0;
int R,G,B;
int obsi , mass , effency , fire, state , button , C;
char color, loc;
String location;
const uint64_t pipes[2] = { 
  0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

float calibration_factor = 220; //-7050 worked for my 440lb max scale setup
float units;
float ounces;
/************************Communication data****************************/
typedef struct{
  byte Mlamppost;
  byte Mroll;
  byte Mconnection;
  byte Ccamv;
  byte Ccamh;
  byte Cfan1;
  byte Cfan2;
  byte Cfan3;
  byte Cgates;
  byte Cairsuction;
  byte Cturnoff;  
  byte Cconnection;
  int Rmass;
  byte Robsetcal;
  byte Rrunning;
  byte Rturnedoff;
  byte Rconnection;
  byte Ssmi;
  byte Ssmo;
  byte Ssmw;
  byte Sfanon1;
  byte Sfanon2;
  byte Sfanon3;
  byte Sfli;
  byte Sflo;
  byte Sflw;
  byte Sgatesstate;
  byte Sdanger;
  byte Sconnection;
  char Rlocation;
}
A_t;
typedef struct{
  char location;
  int mass;
  byte fork;
  byte obsetcal;
  byte connection = 1;
  byte working;
  byte turnedoff;
}
B_t;

A_t router;  
B_t robot; 

void setup()
{
  radio.begin();
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();
  lcd.begin (16,2);
  pinMode(LS, INPUT);
  pinMode(RS, INPUT);
  pinMode(LM1, OUTPUT);
  pinMode(LM2, OUTPUT);
  pinMode(RM1, OUTPUT);
  pinMode(RM2, OUTPUT);
  pinMode(Speed, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);
  pinMode(Buzzer, OUTPUT); 
  Serial.begin (57600);
  lcd.clear ();
  lcd.setCursor (0,0);
  lcd.print ("Location:");
  lcd.setCursor (0,1);
  lcd.print ("distance:");
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  digitalWrite(Green,HIGH);
  digitalWrite(Red,HIGH);
  digitalWrite(Buzzer , HIGH);
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average();
  myservo.attach (9);
  myservo.write (7);
  robot.obsetcal = 0;
  
}
void loop()
{
  /*
      if(millis()-Tconnection > 2000)    
{  
  if ( router.Mconnection == 0){
  Mconnection = 0;
}else {
  Mconnection = 1;
}
  router.Mconnection = 0;
  Tconnection = millis ();}
    if (Mconnection == 0){
Sconnection = 0;
router.Cturnoff = 0;
Cconnection = 0;}else {
Sconnection = router.Sconnection;
Cconnection = router.Cconnection;
}*/
    
  distance = sonar.ping_cm ();
  scale.set_scale(calibration_factor);
  
  Serial.print("Reading: ");
  units = scale.get_units(), 10;
  if (units < 0) {
    units = 0.00;
  }
  robot.mass = units ;
  ounces = units * 0.035274;
  Serial.print(units);
  Serial.print(" grams"); 
      // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  R = frequency;
  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  G= frequency;
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  // Printing the value on the serial monitor
  B = frequency;
  if (55 < R && R < 160 && 40 < G && G < 142 && 65 < B  && B< 142){
    color = 'G'; 
    loc = 'I';
    location = "Input";
  }else if (130 < R && R < 185 && 150 < G && G < 220 && 90 < B  && B< 150){
    color = 'B';
  }else if (20 < R && R < 100 && 100 < G && G < 180 && 80 < B  && B< 180){
    color = 'R';
    loc = 'O';
    location = "Output";
  }else if (170 < R && R < 240 && 190 < G && G < 300 && 160 < B  && B< 240){
    color = 'S';
    loc = 'R';
    location = "Service";
  }else if (10 < R && R < 55 && 15 < G && G < 60 && 10 < B  && B< 60){
    color = 'W' ;
    loc = 'S';
    location = "Store";
    }
  else {
    Serial.println ("N");
  }

  switch (loc){
    case 'O': 
    myservo.write(85);
    state = 1;
    break;
    case 'R': 
    myservo.write (85);
    state = 1;
    break;
    case 'I':
    myservo.write (7);
    state = 0;
    break;
    case 'S':
    myservo.write (85);
    state = 1;
    break;
    default:
    myservo.write (7);
    state = 0;
    break;
  }


  
  Serial.println (distance);
 if (distance > 9 && router.Cturnoff == 0 && router.Sdanger == 0 || distance == 0 && router.Cturnoff == 0 && router.Sdanger == 0){
  digitalWrite(Green,HIGH);
  digitalWrite(Red,LOW);
  digitalWrite(Buzzer , LOW); 
  Serial.print ("State :");
   Serial.println ("On");
  obsi = 0 ;
  if(millis()-lcdtime > 500)     //Has one second passed?
  {
  lcd.clear ();
  lcd.setCursor (0,0);
  lcd.print ("Location:");
  lcd.setCursor (0,1);
  lcd.print ("distance:"); 
  lcd.setCursor (9,1);
  lcd.print ("      ");
  lcd.setCursor (9,1);
  lcd.print (distance);
  lcd.print ("cm");
  lcd.setCursor (9,0);
  lcd.print ("       ");
  lcd.setCursor (9,0);
  lcd.print (location);
  lcdtime = millis ();
  }
 if(digitalRead(LS) && digitalRead(RS))     // Move Forward
  {
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, LOW);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, LOW);

  }
  
  if(!(digitalRead(LS)) && digitalRead(RS))     // Turn right
  {
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, LOW);
  }
  
  if(digitalRead(LS) && !(digitalRead(RS)))     // turn left
  {
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, LOW);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, HIGH);
  }
  
  if(!(digitalRead(LS)) && !(digitalRead(RS)))     // stop
  {
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, HIGH);
    
  }
  robot.turnedoff = 0;
  robot.working   = 1;
  robot.obsetcal = 0;
  }else if (router.Sdanger == 1 && router.Sconnection == 1){
stopall ();
robot.turnedoff = 1;
robot.working = 0;
robot.obsetcal = 0;
 if(millis()-buzzertime > 50)     //Has one second passed?
  {
  if (off == 0){
    off = 1;
  }else {
    off = 0;
  }
  digitalWrite (Buzzer , off);
  digitalWrite (Red, off);
  digitalWrite (Green , LOW);
  buzzertime = millis ();
  }

    
     if(millis()-lcdtime > 500)     //Has one second passed?
  { lcd.clear ();
    lcd.print ("*****Warning****");
    lcd.setCursor (0,1);
    lcd.print ("     Fire!!");
  lcdtime = millis ();
  }
  }
  else if (router.Cturnoff == 1) {
    stopall ();
    robot.turnedoff = 1;
    robot.working = 0;
    robot.obsetcal = 0;
    digitalWrite(Green,LOW);
    digitalWrite(Red,HIGH);
    digitalWrite (Buzzer , LOW);
      Serial.print ("State :");
   Serial.println ("OFF");
    
 if(millis()-lcdtime > 500)     //Has one second passed?
  { lcd.clear ();
    lcd.print ("   Turned off!  ");
  lcdtime = millis ();
  }
  }
 else if(distance < 9 && distance != 0 && router.Cturnoff == 0) {  
  robot.turnedoff = 0;
    robot.working = 0;
    robot.obsetcal = 1;
  stopall ();
  obsi = 1;
  if(millis()-lcdtime > 500)     //Has one second passed?
  {
  lcd.clear();
  lcd.print ("**Please Move***");
  lcd.setCursor (0,1);
  lcd.print ("******Away******");
  lcdtime = millis ();
  }
  if(millis()-buzzertime > 200)     //Has one second passed?
  {
  if (off == 0){
    off = 1;
  }else {
    off = 0;
  }
  digitalWrite (Buzzer , off);
  digitalWrite (Red, off);
  digitalWrite (Green , LOW);
  buzzertime = millis ();
  }
 }

/*---------------------comunication-------------------------*/
  robot.location= loc ; 
  robot.mass= mass;
 // duino2.X = state;
 // duino2.Y = obsi;
 // duino2.Z = C;
  // end of analog reading
   // radio stuff
/********************Data communication***********************/
 if ( radio.available() )
  {
    bool done = false;
    while (!done)
    {
      done = radio.read( &router, sizeof(router) );
    }
    if (router.Mroll == 1){
    radio.stopListening();
    radio.write( &robot, sizeof(robot) );
    radio.startListening();
    }
  }
  /*************************************************/
Serial.print ("Turnoff :");
Serial.println (router.Cturnoff);
}

void stopall (){
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, LOW);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, LOW);
}

