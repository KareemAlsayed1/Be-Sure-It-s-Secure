#include <Servo.h>
#include "SevenSeg.h"
#include <DHT.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
/*---------7 Segment 2 digit display--------*/
//Defines the segments A-G: SevenSeg(A, B, C, D, E, F, G);
SevenSeg disp (A12,A11,A10,A9,A8,A13,A14);
DHT dht (2,DHT11);
const int numOfDigits =2;
//CC(or CA) pins of segment
int digitPins [numOfDigits]={3,4};
/**************Timers ****************/
unsigned long  Tconnection = millis ();
unsigned long  Tcamera = millis ();
unsigned long  Tdoors = millis ();
/****Variables for seven segment****/
int digitL ;
int digitR ;
int temp = 00; 
long unsigned screen = millis();
/**********Servo name**************/
Servo camv;
Servo camh;
Servo gateleft;
Servo gateright;
Servo airsuction;
/*********Output pins**************/
int isml =  19;
int osml =  20;
int wsml =  21;
int g1    = 22;
int g2    = 23;
int y1    = 24;
int y2    = 25;
int r1    = 26;
int fan1 = 43;
int fan2 = 45;
int fan3 = 47;
/*********Input pins**************/
int smi = A0;
int smo = A1;
int smw = A2;
int fli = A3;
int flo = A4;
int flw = A5;
/********communication data***********/
typedef struct{
  byte Mlamppost;
  byte Mroll;
  byte Mconnection ;
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
 byte smi;
 byte smo;
 byte smw;
 byte fanon1;
 byte fanon2;
 byte fanon3;
 byte fli;
 byte flo;
 byte flw;
 byte gatesstate;
 byte danger;
 byte connection = 1;
}
D_t;
A_t router;
D_t securty;
/***********************nRf24 stuff******************************************************/

RF24 radio(10,11); 
const uint64_t pipes[2] = { 
  0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

/************************************Data***************************************************************/
int automatic = 1;
int Mconnection = 0;
int danger = 0;
int door = 15;
int n = 1;
/*************************************************************************************************/
void setup() {
  // put your setup code here, to run once:
  
  securty.danger = danger;
  camv.attach (8);
  camh.attach (7);
  gateleft.attach (5);
  gateright.attach (6);
  airsuction.attach (9);
  //Defines the number of digits to be "numOfDigits" and the digit pins to be the elements of the array "digitPins"
  disp.setDigitPins ( numOfDigits , digitPins );
  //Only for common cathode 7segments
  disp.setCommonCathode();
  //Control brightness (values 0-100);
  disp.setDutyCycle(100);
  pinMode (isml , OUTPUT);
  pinMode (osml , OUTPUT);
  pinMode (wsml , OUTPUT);
  pinMode (g1   , OUTPUT);
  pinMode (g2   , OUTPUT);
  pinMode (y1   , OUTPUT);
  pinMode (y2   , OUTPUT);
  pinMode (r1   , OUTPUT);
  pinMode (fan1 , OUTPUT);
  pinMode (fan2 , OUTPUT);
  pinMode (fan3 , OUTPUT);
  disp.setTimer (2);
  disp.startTimer ();
  radio.begin();
    Serial.begin (9600);
radio.openWritingPipe(pipes[1]);
radio.openReadingPipe(1,pipes[0]);
radio.startListening();

}

 void loop() {
if (door != router.Cgates){
  if ( n == 1 ){
    Tdoors = millis ();
    n--;
  }
  if (millis()-Tdoors > 200)     //Has one second passed?
{
  door = router.Cgates ;
  n =1;
  }
}else {
  n = 1;
}
/******************Connections *********************************************************/
if(millis()-Tconnection > 3000)     //Has one second passed?
{ if ( router.Mconnection == 0){
  Mconnection = 0;
}else {
  Mconnection = 1;
}
  router.Mconnection = 0;
  Tconnection = millis ();}
      if (Mconnection == 0){
        router.Cconnection = 0;
        router.Rconnection = 0;
      }else {
        router.Cconnection = 1;
        router.Rconnection = 1;
      }

 /*****************Temprature Monitor************************************************************************************************************/
  temp = dht.readTemperature();
   disp.write (temp); 
  if (temp < 25 && temp != 0){
    digitalWrite (g1 , HIGH);
    digitalWrite (g2 , LOW );
    digitalWrite (y1 , LOW );
    digitalWrite (y2 , LOW );
    digitalWrite (r1 , LOW );
  }else if (temp >= 25 && temp < 35){
    digitalWrite (g1 , HIGH);
    digitalWrite (g2 , HIGH );
    digitalWrite (y1 , LOW );
    digitalWrite (y2 , LOW );
    digitalWrite (r1 , LOW );
  }else if (temp >= 35 && temp < 45){
    digitalWrite (g1 , HIGH);
    digitalWrite (g2 , HIGH );
    digitalWrite (y1 , HIGH );
    digitalWrite (y2 , LOW );
    digitalWrite (r1 , LOW );
  }else if (temp >= 45 && temp < 55){
    digitalWrite (g1 , HIGH);
    digitalWrite (g2 , HIGH );
    digitalWrite (y1 , HIGH );
    digitalWrite (y2 , HIGH );
    digitalWrite (r1 , LOW );
  }else if (temp >= 55 ){
    digitalWrite (g1 , HIGH);
    digitalWrite (g2 , HIGH );
    digitalWrite (y1 , HIGH );
    digitalWrite (y2 , HIGH );
    digitalWrite (r1 , HIGH );
    }else {
    digitalWrite (g1 , LOW);
    digitalWrite (g2 , LOW);
    digitalWrite (y1 , LOW);
    digitalWrite (y2 , LOW);
    digitalWrite (r1 , LOW);
    }
 /****************Fans and air suction********************************************************************************/
 if (router.Cconnection == 0 || securty.smi == 1 || securty.smw == 1 || securty.smo == 1){
  automatic = 1;
 }else {
  automatic = 0;
 }
 if (automatic == 1){
    if (securty.smi == 1 || securty.smw == 1){
      digitalWrite (fan3 , HIGH);
      securty.fanon3 = 1;
    }else{
      digitalWrite (fan3 , LOW);
      securty.fanon3 = 0;
    }
    if (securty.smo == 1 || securty.smw == 1){
      digitalWrite (fan1 , HIGH);
      securty.fanon1 = 1;    
    }else {
      digitalWrite (fan1 , LOW);
      securty.fanon1 = 0;   
    }
   if (securty.smo == 1 || securty.smi == 1 || securty.smw == 1){
     airsuctionmode ();
   }else {
      fanmode ();
   }
  }else {
    if (router.Cfan1 == 1){
      digitalWrite (fan1 , HIGH);
      securty.fanon1 = 1;    
    }else {
      digitalWrite (fan1 , LOW);
      securty.fanon1 = 0;
    }    if (router.Cfan2 == 1){
      digitalWrite (fan2 , HIGH);
      securty.fanon2 = 1;    
    }else{
      digitalWrite (fan2 , LOW);
      securty.fanon2 = 0;
    } if (router.Cfan3 == 1){
      digitalWrite (fan3 , HIGH);
      securty.fanon3 = 1;   
    }else{
      digitalWrite (fan3 , LOW);
      securty.fanon3 = 0;
    }
    if (router.Cairsuction == 1){
      airsuctionmode ();
    }else {
      fanmode ();
    }
   }
 /******************Read data smoke *****************************************************************************************/
 if (digitalRead (smi) == 1){
  securty.smi = 0;
  digitalWrite (isml, LOW);
 }else {
  securty.smi = 1;
  digitalWrite (isml, HIGH);
 }
 if (digitalRead (smo) == 1){
  securty.smo = 0;
  digitalWrite (osml, LOW);
 }else {
  securty.smo = 1;
  digitalWrite (osml, HIGH);
 }
  if (digitalRead (smw) == 1){
  securty.smw = 0;
  digitalWrite (wsml, LOW);
 }else {
  securty.smw = 1;
  digitalWrite (wsml, HIGH);
 }
/******************** Read data flame sensor *************************************************************************/
 if (digitalRead (fli) == 1){
  securty.fli = 0;
 }else {
  securty.fli = 1;
 }
 if (digitalRead (fli) == 1){
  securty.flo = 0;
 }else {
  securty.flo = 1;
 }
  if (digitalRead (smw) == 1){
  securty.flw = 0;
  digitalWrite (wsml, LOW);
 }else {
  securty.flw = 1;
  digitalWrite (wsml, HIGH);
 }
/***************************Gates***********************************************************/
 if (door == 3){
    opendoors ();
  }else if (door == 1){
    closedoors ();
  }else if (door == 2) {
    gatesautomatic ();
  }
/**************************danger *********************************************************/
if (securty.smi == 1 || securty.smo == 1 || securty.smw == 1 || securty.fli == 1 || securty.flo == 1 || securty.flw == 1){
   danger = 1;
}else {
   danger = 0;
}
 /***********************Camera contoller*************************************************/
 int p = 1 ;
 if (router.Cconnection == 1){
  camv.write (router.Ccamv);
  camh.write (router.Ccamh);
 }else {
  if(millis()- Tcamera > 20000)     //Has one second passed?
{  if (p == 4){
  p = 1;
}if (p == 1){
  moveware ();
}else if (p == 2){
  moveinput ();
}else if (p == 3){
  moveoutput ();
}
p++;
  Tcamera = millis ();    
}
 }
/**************Communication data***************************************/
 if ( radio.available() )
  {
    bool done = false;
    while (!done)
    {
      done = radio.read( &router, sizeof(router) );
    }
    if (router.Mroll == 3){
    radio.stopListening();
    radio.write( &securty, sizeof(securty) );
    radio.startListening();
    }
 
  Serial.print("conrol.camh = ");
  Serial.println(router.Ccamh);
  Serial.print("conrol.fan1 = ");
  Serial.println(router.Cfan1);
  Serial.print("conrol.fan2 = ");
  Serial.println(router.Cfan2);
  Serial.print("conrol.fan3 = ");
  Serial.println(router.Cfan3);
  Serial.print("conrol.gates = ");
  Serial.println(router.Cgates);
  Serial.print("conrol.airsuction = ");
  Serial.println(router.Cairsuction);
 /******************End loop *******************************************/
}
 }

/**************Functions ********************************************************************************************************/
void closedoors (){
  gateleft.write (180);
  gateright.write (105);
  securty.gatesstate = 0;
}
void opendoors (){
  gateleft.write (90);
  gateright.write (180);
  securty.gatesstate = 1;
}
void moveinput (){
  camh.write (103);
  camv.write (30);
}
void moveoutput(){
camh.write (33);
camv.write (12);
}
void moveware (){
camh.write (180);
camv.write (25);
}
void fanmode (){
  airsuction.write (8);
}
void airsuctionmode (){
  airsuction.write (169);
}
void gatesautomatic (){
  if (danger == 1){
    opendoors ();
  }else {
    closedoors();
  }
}
ISR( TIMER2_COMPA_vect ){
 disp.interruptAction ();
 }

