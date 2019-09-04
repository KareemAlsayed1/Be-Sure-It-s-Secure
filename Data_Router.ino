// http://www.bajdi.com
// Sending a struct with the nRF24L01 module
// Data to be sent is the reading of 2 analog pins
// Data received is the analog reading of 2 pins on the other Arduino 
#include <LiquidCrystal.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>
LiquidCrystal lcd (2,3,4,5,6,7);

/**************Timer *****************************/
unsigned long  TSconnection = millis ();
unsigned long  TCconnection = millis ();
unsigned long  TRconnection = millis ();
/*************************************************/




#define LDR A1

#define Lamppost 13

#define Redalert 8
 
#define buzzer 9


byte customChar[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b01110,
  0b10001,
  0b00000,
  0b00000
};
Servo camv;
Servo camh;
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum
int off = 0;
int obsi, mass, effency, fire, state,button, C;
int green = 14;
int blue = 15;
int yellow = 16;
int red = 17;
int white = 18;
int light = 1;
int on = 1;
char loc;

String Loc;
unsigned long lcdtime = millis();
unsigned long connection = millis();
unsigned long  Blink = millis();
unsigned long  Tcamera = millis();

/*---------------------Communication data-------------------------------*/
typedef struct{
  byte Mlamppost;
  byte Mroll;
  byte Mconnection = 1;
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
  byte connection;
  byte working;
  byte turnedoff;
}
B_t;
typedef struct{
  byte camv; // camera verical 
  byte camh; // camera horizontal
  byte fan1; // fan A turn on 
  byte fan2; // fan B turn on 
  byte fan3; // fan C turn on
  byte gates; // gates open / close
  byte airsuction; //air suction mode 
  byte turnoff; // robot turn off
  byte connection ;
}
C_t;
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
 byte connection ;
}
D_t;

A_t router;  
B_t robot;
C_t control;
D_t securty;
/*----------------------------------------------------------------------------*/
RF24 radio(40,53);   // make sure this corresponds to the pins you are using
const uint64_t pipes[2] = { 
  0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
void setup()
{ camv.attach (11);
  camh.attach (10);
  Serial.begin(57600);
  lcd.begin (20,4);
  pinMode (green , OUTPUT);
  pinMode (Redalert , OUTPUT);
  pinMode (Lamppost , OUTPUT);
  pinMode (blue , OUTPUT);
  pinMode (buzzer , OUTPUT);
  pinMode (red , OUTPUT);
  pinMode (yellow , OUTPUT);
  pinMode (white , OUTPUT);
  lcd.clear ();
  lcd.createChar(0, customChar);
  lcd.setCursor (0,1);
  lcd.print ("Loading.....");
  radio.begin();
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  obsi = 0;
  digitalWrite (green, HIGH);
  digitalWrite(blue, LOW);
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(white, LOW);
  digitalWrite(Redalert, LOW);
  digitalWrite(Lamppost, HIGH);
  digitalWrite(buzzer, HIGH);
  C = 0;
  button = 1 ;
  fire = 0;
  router.Mroll = 1;
 }

void loop()
{ 
/**************Connections Refresh **********************************
 if(millis()-TRconnection > 2000)     //Has one second passed?
{ if ( robot.connection == 0){
  router.Rconnection = 0;
}else {
  router.Rconnection = 1;
}
  robot.connection = 0;
  TRconnection = millis ();}
  /*----------------------------------------------------------------------------
 if(millis()-TCconnection > 5000)     //Has one second passed?
{ if ( control.connection == 0){
  router.Cconnection = 0;
}else {
  router.Cconnection = 1;
}
  control.connection = 0;
  TCconnection = millis ();}
  /*---------------------------
   if(millis()-TSconnection > 5000)     //Has one second passed?
{ if ( securty.connection == 0){
  router.Sconnection = 0;
  router.Sdanger = 0;
}else {
  router.Sconnection = 1;
}
  securty.connection = 0;
  TSconnection = millis ();}

/************************Data switching*****************************/
  if (router.Mroll == 1){
    router.Mroll = 2;
  }else if (router.Mroll == 2) {
    router.Mroll = 3;
  }else {
    router.Mroll = 1;
  }
/****************************Data transfer**************************/

 
 
 
  /***********************************************************************/
  if (securty.danger == 1 ){
    fire = 1;
  }else {
    fire = 0;
  }
  if (analogRead (LDR) < 160){
    router.Mlamppost = 1;
  }else {
    router.Mlamppost = 0;
  }
  if (fire == 1){
    digitalWrite (Lamppost , LOW);
     if(millis()-Blink > 50)     //Has one second passed?
{
  if (on == 0){
    on = 1;
  }else {
    on = 0;
  }
  digitalWrite(buzzer, on);   
  digitalWrite(Redalert, on);
   Blink = millis ();
}
  }else {
  digitalWrite (Lamppost , router.Mlamppost);
  digitalWrite(buzzer, LOW);   
  digitalWrite(Redalert, LOW);
  }
  Serial.println ("**" );
  Serial.println (analogRead(LDR) );
  
/*---------------------Data sending------------------------------------------*/
  radio.stopListening();
  bool ok = radio.write( &router, sizeof(router) );
  radio.startListening();

  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 500 )
      timeout = true;

  if ( timeout )
  {
    Serial.println("Failed, response timed out.");
  }
  else
  {
    if (router.Mroll == 1) {
      Serial.println("1");
      radio.read( &robot, sizeof(robot) );
 router.Rlocation = robot.location;
  router.Rmass = robot.mass;
  router.Robsetcal = robot.obsetcal;
   router.Rrunning = robot.working;
  router.Rturnedoff = robot.turnedoff;
    }else if (router.Mroll == 2){
      Serial.println("2");
      radio.read( &control, sizeof(control) );
       router.Ccamv = control.camv;
  router.Ccamh = control.camh;
  router.Cfan1 = control.fan1;
  router.Cfan2 = control.fan2;
  router.Cfan3 = control.fan3;
  router.Cgates = control.gates;
  router.Cairsuction = control.airsuction;
  router.Cturnoff = control.turnoff;
  router.Cconnection = control.connection;
    }else {
     Serial.println("3");
     radio.read( &securty, sizeof(securty) );
      router.Ssmi = securty.smi;
  router.Ssmo = securty.smo;
  router.Ssmw = securty.smw;
  router.Sfanon1 = securty.fanon1;
  router.Sfanon2 = securty.fanon2;
  router.Sfanon3 =securty.fanon3;
  router.Sfli = securty.fli;
  router.Sflo = securty.flo;
  router.Sflw = securty.flw;
  router.Sdanger = securty.danger;
  router.Sgatesstate = securty.gatesstate;
  }}
/*--------------------------------------------------------------------------------*/
  loc = robot.location;
  mass = robot.mass;
  obsi = robot.obsetcal;
  C = router.Rconnection;

  switch (robot.location){
    case 'S':
    Loc = "Warehouse";
    break;

    case 'O':
    Loc = "Output";
    break;

    case 'I':
    Loc = "Input";
    break;

    case 'R':
    Loc = "Service";
    break;

    default:
    Loc = "Disconnect";
    break;
  } 

   /***********************Camera contoller*************************************************/
 int p = 1 ;
 int t = 1 ;
 if (t == 1){
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
  // end of serial printing
if (fire == HIGH){
  digitalWrite (green, LOW);
  digitalWrite(blue, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(white, LOW);
  if(millis()-Blink > 50)     //Has one second passed?
{  if (off == 0){
    off = 1;
  }else {
    off = 0;
  }
  digitalWrite(red, off);
  digitalWrite(buzzer, off);   
  digitalWrite(Redalert, off);
 
  Blink = millis ();    
}    
      if(millis()-lcdtime > 250)     //Has one second passed?
{
lcd.clear ();
lcd.print ("*****Warming******");
lcd.setCursor (0,1);
lcd.print ("       Fire!!");
lcd.setCursor (0,2);
lcd.print ("     Please take");
lcd.setCursor (0,3);
lcd.print ("   the first Exit!");
lcdtime = millis ();   }
}else if (router.Rconnection == 0){
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite (green, LOW);
  digitalWrite(blue, LOW); 
  if(millis()-Blink > 250)     //Has one second passed?
{  if (off == 0){
    off = 1;
  }else {
    off = 0;
  }
  digitalWrite(white, off);  
   
   Blink = millis ();
} 
      
      
      if(millis()-lcdtime > 10)     //Has one second passed?
  {  lcd.clear ();
     lcd.setCursor (3,1);
     lcd.print ("Disconnected!");
     lcd.setCursor (2, 2);
     lcd.print ("Please check the");
     lcd.setCursor (5, 3);
     lcd.print ("connection!");
  lcdtime = millis ();   
  }
}else if (button == 0 ){
  digitalWrite(red, LOW);
  digitalWrite (green, LOW);
  digitalWrite(blue, LOW);
  digitalWrite(white, LOW);
  if(millis()-Blink > 250)     //Has one second passed?
{  if (off == 0){
    off = 1;
  }else {
    off = 0;
  }
   digitalWrite(yellow, off);   
   Blink = millis ();
} 
   if(millis()-lcdtime > 250)     //Has one second passed?
  {
    lcd.clear();
    lcd.setCursor (0,0);
  lcd.print ("  Robot is turned");
  lcd.setCursor (0,1);
  lcd.print ("       off ");
  lcd.write((uint8_t)0);
  lcdtime = millis ();}
}else if (obsi == 1){
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite (green, LOW);
  digitalWrite(white, LOW);
  if(millis()-Blink > 250)     //Has one second passed?
{  if (off == 0){
    off = 1;
  }else {
    off = 0;
  }
  digitalWrite(blue, off);    
   Blink = millis ();
} 
  if(millis()-lcdtime > 250)     //Has one second passed?
  {lcd.clear ();
  lcd.print ("* Please check the *");
  lcd.setCursor (0,1);
  lcd.print ("*       Robot      *");
  lcd.setCursor (0,2);
  lcd.print ("*There is obsticle *");
  lcd.setCursor (0,3);
  lcd.print ("*    in the Path   *");
  lcdtime = millis ();   
  }}else {
   if(millis()-lcdtime > 250)     //Has one second passed?
  {
  
  lcd.clear ();
  lcd.setCursor (0,0);
  lcd.print ("#*#<ROBOT status>#*#");
  lcd.setCursor (0,1);
  lcd.print ("Location :");
  lcd.setCursor (0,2);
  lcd.print ("Loaded mass :");
  lcd.setCursor (0,3);
  lcd.print ("Fork state :");
  lcd.setCursor (10,1);
  lcd.print ("          ");
  lcd.setCursor (10,1);
  lcd.print(Loc);
  lcd.setCursor (13 , 2);
  lcd.print ("       ");
  lcd.setCursor (13 , 2);
  lcd.print (mass);
  lcd.print ("g");
  lcd.setCursor (12 , 3);
  lcd.print ("        ");
  lcd.setCursor (12 , 3);
  if (state == 1){
    lcd.print ("UP");
  }else {
    lcd.print ("DOWN");
  }
  lcdtime = millis ();  }
  digitalWrite (green, HIGH);
  digitalWrite(blue, LOW);
  digitalWrite(red, LOW);
  digitalWrite(white, LOW);
  digitalWrite(yellow, LOW);
  }

  Serial.print("conrol.camv = ");
  Serial.println(router.Ccamv);
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
  Serial.print("conrol.turnoff = ");
  Serial.println(router.Ssmi);
  Serial.print("robot.location = ");
  Serial.println(robot.location);
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
  Serial.print("conrol.turnoff = ");
  Serial.println(router.Cturnoff);
 Serial.println(router.Rconnection);

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

