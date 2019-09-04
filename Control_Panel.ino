/*********Libraries****************/
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
/*************Timers*****************/
unsigned long  Tbuzzer = millis();
unsigned long  Twarningi = millis ();
unsigned long  Twarningo = millis ();
unsigned long  Twarningw = millis ();
unsigned long  Tconnection = millis ();
unsigned long  Ts = millis ();
unsigned long  Tc = millis ();
unsigned long  lcdtime = millis ();

/*************nRF24lo****************/

RF24 radio(10,11); 
const uint64_t pipes[2] = { 
  0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

/***********LCD intialize***********/
LiquidCrystal lcd(A8, A9, A10, A11, A12, A13); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7) 
/********Keypad intialize***********/
String password="1234";
String tempPassword;
boolean locked = true; // State of the alarm
boolean enteredPassword; // State of the entered password to stop the alarm
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keypressed;
//define the cymbols on the buttons of the keypads
char keyMap[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2,3,4,5}; //Row pinouts of the keypad
byte colPins[COLS] = {6,7,8,9}; //Column pinouts of the keypad
Keypad myKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, ROWS, COLS);
/*******Output varibles*****************/
// i = input
// o = output
// w = warehouse
// s = safe
// sm = smoke 
// l = led
// l = location 
// f = flame 
// w = warning
////////////////
// c = camera
// a = autmatic
///////////////
// g = gate
// o = open
// c = close
///////////////
// r = robot
// o = obstcal
// r = running
// t = turned off
///////////////
// f = fan
// a = unit a
// b = unit b
// c = unit c
///////////////
// c = connection 
// o = on 
// of = off
 int isl  = 14;// input safe mode led
 int iwl  = 15;// input warning mode led
 int ifl  = 16;// input flame led
 int isml = 17;// input smoke detection led
 int ill  = 18;// input robot location
 int osl   =19;// output safe mode led
 int owl  = 20;// output wrning mode led
 int ofl  = 21;// output flame led
 int osml = 22;// output smoke  detection led
 int oll  = 23;// output robot location
 int wsl  = 24;// warehouse safe mode led
 int wwl  = 25;// warehouse warning mode led
 int wfl  = 26;// warehouse flame led
 int wsml = 27;// warehouse smoke 
 int wll  = 28;// warehouse robot location 
 int cal  = 29;// camera automatic mode
 int gol  = 30;// gate open led indicator
 int gcl  = 31;// gate close led indicator
 int rol  = 32;// robot obstacle indicator 
 int rrl  = 33;// robot system is runing
 int rtl  = 34;// robot system is turned off
 int fal  = 35;// cooling fan unit A indicator led
 int fbl  = 36;// cooling fan unit B indicator led
 int fcl  = 37;// cooling fan unit C indicator led
 int col  = 38;// control panel is connected
 int cofl = 39;// control panel is not connected
 #define buzzer 12
/****** Input pins******************/
 int camv = A0;        // camera verical
 int camh = A1;        // camera horizontal
 int fan1 = 40;        // fan A turn on 
 int fan2 = 42;        // fan B turn on 
 int fan3 = 41;        // fan C turn on
 int airsuction = 43;  //air suction mode 
 int turnoff = 44;     // robot turn off
 int gclose = A2;      //gate close mode
 int gopen = A3;       // gate open mode
 int panel = 45;       // control panel state (on/off) 
 int automaticloc = 46;// Automatic camera location 
 int camerainput = A4; // Rotate camera input location
 int cameraoutput = A5;// Rotate camera output location
 int lcdrobot = 48;    //switch lcd mode to robot mode
 int lcdlogistics = 47;//sitch lcd mode to logistics mode
 /*******Varibles data***************/
 int camerah = 0;
 int camerav = 0;
 int Mconnection = 0;
 int Sconnection = 0;
 int Rconnection = 0;
 int idanger = 0;
 int odanger = 0;
 int wdanger = 0;
 int off = 0 ;
/********Communication data**********/
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
  byte Sconnection;
  char Rlocation;
}
A_t;
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
A_t router;  
C_t control;

int n = 1;
int n1 = 1;
/************************************/
int number = 1;
void setup() {
pinMode(buzzer, OUTPUT);
pinMode (isl, OUTPUT);
pinMode (iwl, OUTPUT);
pinMode (ifl, OUTPUT);
pinMode (isml,OUTPUT);
pinMode (ill, OUTPUT);
pinMode (osl, OUTPUT);
pinMode (owl, OUTPUT);
pinMode (ofl, OUTPUT);
pinMode (osml,OUTPUT);
pinMode (oll, OUTPUT);
pinMode (wsl, OUTPUT);
pinMode (wwl, OUTPUT);
pinMode (wfl, OUTPUT);
pinMode (wsml,OUTPUT);
pinMode (wll, OUTPUT);
pinMode (cal, OUTPUT);
pinMode (gol, OUTPUT);
pinMode (gcl, OUTPUT);
pinMode (rol, OUTPUT);
pinMode (rrl, OUTPUT);
pinMode (rtl, OUTPUT);
pinMode (fal, OUTPUT); 
pinMode (fbl, OUTPUT);
pinMode (fcl, OUTPUT);
pinMode (col, OUTPUT);
pinMode (cofl,OUTPUT);
pinMode (camv,INPUT);
pinMode (camh,INPUT);
pinMode (fan1,INPUT);
pinMode (fan2,INPUT);
pinMode (fan3,INPUT);
pinMode (airsuction,INPUT);
pinMode (turnoff,INPUT);
pinMode (gclose,INPUT);
pinMode (gopen,INPUT);
pinMode (panel,INPUT);
pinMode (automaticloc,INPUT);
pinMode (camerainput,INPUT);
pinMode (cameraoutput,INPUT);
pinMode (lcdrobot,INPUT);
pinMode (lcdlogistics,INPUT);
lcd.begin(20,4);
radio.begin();
radio.openWritingPipe(pipes[1]);
radio.openReadingPipe(1,pipes[0]);
radio.startListening();
Serial.begin (9600);
Serial.println ("Test");
}
void loop() {
  if (number == 1 ){
      enterPassword();
      number--;
  }
  
  if (digitalRead (panel) != LOW){
    digitalWrite (ill , LOW);

 /********----------------------------
 if (Rconnection != router.Rconnection){
  if ( n == 1 ){
    Ts = millis ();
    n--;
  }
  if (millis()-Ts > 1500)     //Has one second passed?
{
  Rconnection = router.Rconnection;
  n =1;
  }
}else {
  n = 1;
}
 if (Sconnection != router.Sconnection){
  if ( n1 == 1 ){
    Tc = millis ();
    n1--;
  }
  if (millis()-Tc > 1500)     //Has one second passed?
{
  Sconnection = router.Sconnection;
  n1 =1;
  }
}else {
  n1 = 1;
}
 if(millis()-lcdtime > 3000)     //Has one second passed?
  {router.Sconnection = 0;
  router.Cconnection = 0;
  lcdtime = millis ();
  }
       */
  Sconnection = 1;
 Rconnection = 1;     
      if(millis()-Tconnection > 3000)     //Has one second passed?
{ if ( router.Mconnection == 0){
  Mconnection = 0;
}else {
  Mconnection = 1;
}
  router.Mconnection = 0;
  Tconnection = millis ();
  }

  /***************LCD scbreen ***********************************************/
  if (digitalRead (lcdrobot) == HIGH && digitalRead (lcdlogistics) == LOW  ){
  if (Rconnection == 1){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Robot Current State ");
  lcd.setCursor(0,1);
  lcd.print("Location:           "); 
  lcd.setCursor(0,2);
  lcd.print("Loaded mass:        ");
  lcd.setCursor(0,3);
  lcd.print("Obstacel:            ");
  }else {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sorry,,, Robot is   ");
  lcd.setCursor(0,1);
  lcd.print("  now disconnected  "); 
  lcd.setCursor(0,2);
  lcd.print("   Please check the ");
  lcd.setCursor(0,3);
  lcd.print("     Connection     ");
  }}else if(digitalRead (lcdrobot) == LOW && digitalRead (lcdlogistics) == HIGH ){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Factory logestics  ");
  lcd.setCursor(0,1);
  lcd.print("Lamposts:           "); 
  lcd.setCursor(0,2);
  lcd.print("Gates:              ");
  lcd.setCursor(0,3);
  lcd.print("Light:              ");
  }else {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("    Default Mode    ");
  lcd.setCursor(0,1);
  lcd.print("_____Connections____"); 
  lcd.setCursor(0,2);
  lcd.print("Robot:           ");
  if (Rconnection == 1 ){
  lcd.setCursor(7,2); 
  lcd.print ("Connected");
  }else {
  lcd.setCursor(7,2); 
  lcd.print ("disconnected");
  }
  lcd.setCursor(0,3);
  lcd.print("Sec. :           ");
  if (Sconnection == 1 ){
  lcd.setCursor(7,3); 
  lcd.print ("Connected");
  }else {
  lcd.setCursor(7,3); 
  lcd.print ("disconnected");
  }
  }
/**************************Panel connection*********************************************/
if (Mconnection == 1){
  digitalWrite (cofl , LOW);
  digitalWrite (col , HIGH);
}else {
  digitalWrite (cofl , HIGH);
  digitalWrite (col , LOW);
}
/*********************************Camera section***************************************/
if (digitalRead (automaticloc) == HIGH){
  digitalWrite (cal , HIGH);
}else {
  digitalWrite (cal , LOW);
}
if (digitalRead (automaticloc) == LOW){
 camerah = map(analogRead (camh), 0, 1023, 0, 179); // scale it to use it with the servo (value between 0 and 180)  
 camerav = map(analogRead (camv), 0, 1023, 0, 83); // scale it to use it with the servo (value between 0 and 180)  
}else if (digitalRead (camerainput)== LOW && digitalRead (cameraoutput) == HIGH && digitalRead (automaticloc) == HIGH){
 camerah = 33;
 camerav = 12;  
}else if (digitalRead (camerainput)== HIGH && digitalRead (cameraoutput) == LOW && digitalRead (automaticloc) == HIGH){
 camerah = 103;
 camerav = 30;  
}else if (digitalRead (camerainput)== LOW && digitalRead (cameraoutput) == LOW && digitalRead (automaticloc) == HIGH){
 camerah = 180;
 camerav = 25;  
}
control.camv = camerav;
control.camh = camerah;
/**********************Gates********************************************************/
if (digitalRead (gopen) == HIGH && digitalRead (gclose) == LOW){
  control.gates = 1;
}else if (digitalRead (gopen) == LOW && digitalRead (gclose) == HIGH){
  control.gates = 3;
}else if (digitalRead (gopen) == LOW && digitalRead (gclose) == LOW){
  control.gates = 2;
}
  if(control.gates == 3) {
    digitalWrite (gol , HIGH);
    digitalWrite (gcl , LOW);
  }else if (control.gates == 1){
    digitalWrite (gol , LOW);
    digitalWrite (gcl , HIGH);
  }else{
   digitalWrite (gol , LOW);
   digitalWrite (gcl , LOW);
  }

/**********************Robot Section**************************************************/
control.turnoff = digitalRead (turnoff);
 if (router.Rrunning == 1 && Rconnection == 1){
  digitalWrite (rol, LOW);
  digitalWrite (rrl, HIGH);
  digitalWrite (rtl, LOW);
  
 }
else if (router.Robsetcal == 1 && Rconnection == 1){
  digitalWrite (rol, HIGH);
  digitalWrite (rrl, LOW);
  digitalWrite (rtl, LOW);
}else if (router.Rturnedoff == 1 && Rconnection == 1){
  digitalWrite (rol, LOW);
  digitalWrite (rrl, LOW);
  digitalWrite (rtl, HIGH);
}else {
  digitalWrite (rol, LOW);
  digitalWrite (rrl, LOW);
  digitalWrite (rtl, LOW);
}
/*******************Fans mode*******************************************************/
control.airsuction = digitalRead (airsuction);
control.fan1 = digitalRead (fan1);
control.fan2 = digitalRead (fan2);
control.fan3 = digitalRead (fan3);  
if (Sconnection == 1){
  if (router.Sfanon1){
    digitalWrite (fal,HIGH);
  }else{
    digitalWrite (fal,LOW);
  }
    if (router.Sfanon2){
    digitalWrite (fbl,HIGH);
  }else{
    digitalWrite (fbl,LOW);
  }
    if (router.Sfanon3){
    digitalWrite (fcl,HIGH);
  }else{
    digitalWrite (fcl,LOW);
  }
}else{
  digitalWrite (fal, LOW);
  digitalWrite (fbl, LOW);
  digitalWrite (fcl, LOW);
}
/*********************Input Zone******************************/
int bi = 0;
if (router.Rlocation == 'I' && Rconnection == 1){
  digitalWrite (ill, HIGH);
}else {
  digitalWrite (ill, LOW);
}
if (Sconnection == 1){
  if (router.Ssmi == 1 && router.Sfli == 0){ 
    digitalWrite (isml, HIGH);
    digitalWrite (ifl , LOW);
    digitalWrite (isl , LOW);
    idanger = 1;
  }else if (router.Ssmi == 0 && router.Sfli == 1){
    digitalWrite (isml, LOW);
    digitalWrite (ifl , HIGH);
    digitalWrite (isl , LOW);
    idanger = 1;
  }else if (router.Ssmi == 1 && router.Sfli == 1){
    digitalWrite (isml, HIGH);
    digitalWrite (ifl , HIGH);
    digitalWrite (isl , LOW);
    idanger = 1;
  }else {
    digitalWrite (isml, LOW);
    digitalWrite (ifl , LOW);
    digitalWrite (isl , HIGH);
    idanger = 0;
  }
  if (idanger == 1){
  if(millis()- Twarningi > 50)     //Has one second passed?
{  if (bi == 0){
    bi = 1;
  }else {
    bi = 0;
  }
  digitalWrite(iwl ,bi);
  Twarningi = millis ();    
} }else {
  digitalWrite(iwl ,0);
}
}else {
    digitalWrite (isml, LOW);
    digitalWrite (ifl , LOW);
    digitalWrite (isl , LOW);
    idanger = 0;
    digitalWrite(iwl ,0);
}
/******************Output zone********************************/
if (router.Rlocation == 'O' && Rconnection == 1){
  digitalWrite(oll, HIGH);
}else {
  digitalWrite (oll, LOW);
}
if (Sconnection == 1){
  if (router.Ssmo == 1 && router.Sflo == 0){ 
    digitalWrite (osml, HIGH);
    digitalWrite (ofl , LOW);
    digitalWrite (osl , LOW);
    odanger = 1;
  }else if (router.Ssmo == 0 && router.Sflo == 1){
    digitalWrite (osml, LOW);
    digitalWrite (ofl , HIGH);
    digitalWrite (osl , LOW);
    odanger = 1;
  }else if (router.Ssmo == 1 && router.Sflo == 1){
    digitalWrite (osml, HIGH);
    digitalWrite (ofl , HIGH);
    digitalWrite (osl , LOW);
    odanger = 1;
  }else {
    digitalWrite (osml, LOW);
    digitalWrite (ofl , LOW);
    digitalWrite (osl , HIGH);
    odanger = 0;
  }
  if (odanger == 1){
    digitalWrite (owl , HIGH);
  }else {
    digitalWrite (owl , LOW);
  }
}else {
    digitalWrite (osml, LOW);
    digitalWrite (ofl , LOW);
    digitalWrite (osl , LOW);
    odanger = 0;
}
/******************Warehouse zone********************************/
if (router.Rlocation == 'W' && Rconnection == 1){
  digitalWrite(wll, HIGH);
}else {
  digitalWrite (wll, LOW);
}
if (Sconnection == 1){
  if (router.Ssmw == 1 && router.Sflw == 0){ 
    digitalWrite (wsml, HIGH);
    digitalWrite (wfl , LOW);
    digitalWrite (wsl , LOW);
    wdanger = 1;
  }else if (router.Ssmw == 0 && router.Sflw == 1){
    digitalWrite (wsml, LOW);
    digitalWrite (wfl , HIGH);
    digitalWrite (wsl , LOW);
    wdanger = 1;
  }else if (router.Ssmw == 1 && router.Sflw == 1){
    digitalWrite (wsml, HIGH);
    digitalWrite (wfl , HIGH);
    digitalWrite (wsl , LOW);
    wdanger = 1;
  }else {
    digitalWrite (wsml, LOW);
    digitalWrite (wfl , LOW);
    digitalWrite (wsl , HIGH);
    wdanger = 0;
    
  }
    if (odanger == 1){
    digitalWrite (owl , HIGH);
  }else {
    digitalWrite (owl , LOW);
  }
}else {
    digitalWrite (wsml, LOW);
    digitalWrite (wfl , LOW);
    digitalWrite (wsl , LOW);
    wdanger = 0;
}
/******************Buzzers ***********************************/
if (wdanger == 1 || idanger == 1 || odanger == 1){
  if(millis()-Tbuzzer > 50)     //Has one second passed?
{  if (off == 0){
    off = 1;
    tone(buzzer, 1000, 200);
  }else {
    off = 0;
    noTone (buzzer);
  } 
  Tbuzzer = millis ();
}    
}else {
  noTone (buzzer);
}

/********************Data communication***********************/
 if ( radio.available() )
  {
    bool done = false;
    while (!done)
    {
      done = radio.read( &router, sizeof(router) );
    }
    if (router.Mroll == 2){
    radio.stopListening();
    radio.write( &control, sizeof(control) );
    radio.startListening();
    }
  }
  /*************************************************/
}else {
  turnoffleds ();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  Factory Control   ");
  lcd.setCursor(0,1);
  lcd.print("       Panel        "); 
  lcd.setCursor(0,2);
  lcd.print("     Turned OFF!!   ");
  lcd.setCursor(0,3);
  lcd.print("____________________");
}
  Serial.print("conrol.camv = ");
  Serial.println(control.camv);
  Serial.print("conrol.camh = ");
  Serial.println(control.camh);
  Serial.print("conrol.fan1 = ");
  Serial.println(control.fan1);
  Serial.print("conrol.fan2 = ");
  Serial.println(control.fan2);
  Serial.print("conrol.fan3 = ");
  Serial.println(control.fan3);
  Serial.print("conrol.gates = ");
  Serial.println(control.gates);
  Serial.print("conrol.airsuction = ");
  Serial.println(control.airsuction);
  Serial.print("conrol.turnoff = ");
  Serial.println(control.turnoff);
  Serial.print("C = ");
  Serial.println(router.Ssmi);
}
void enterPassword() {
  int k = 9;
  int t = 0;
  tempPassword = "";
  locked = true;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("  Factory Control   ");
          lcd.setCursor(0,1);
          lcd.print("       Panel        "); 
          lcd.setCursor(0,2);
          lcd.print("    Please enter    ");
          lcd.setCursor(0,3);
          lcd.print("Password:           ");
      while(locked) {
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY){
        if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
            keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
            keypressed == '8' || keypressed == '9' ) {
          tempPassword += keypressed;
          lcd.setCursor(k,3);
          lcd.print("*");
          tone(buzzer, 2000, 100);
          k++;
          t++;
        }
      }   if (keypressed == 'B' ) {
          tempPassword.remove (t-1);
          if (k > 9){
          k--;}
           tone(buzzer, 2000, 100);
          lcd.setCursor(k,3);
          lcd.print(" ");
          t--;
          Serial.println ("B");
        }
      if (k > 13 || keypressed == '#') {
        tempPassword = "";
        k=9;
        t=0;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("  Factory Control   ");
          lcd.setCursor(0,1);
          lcd.print("       Panel        "); 
          lcd.setCursor(0,2);
          lcd.print("    Please enter    ");
          lcd.setCursor(0,3);
          lcd.print("Password:           ");
      }
      if ( keypressed == 'A') {
        if ( tempPassword == password ) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("   Unlocked !!  ");
          lcd.setCursor(0,1);
          lcd.print("    Welcome <3  "); 
          lcd.setCursor(0,2);
          lcd.print("Loading....     ");
          tone(buzzer,3000, 100);
          locked = false;
          turnonleds ();
          delay (100);
          turnoffleds ();
          delay (100);
          turnonleds ();
          delay (100);
          turnoffleds ();
          delay (100);
          turnonleds ();
          delay (100);
          turnoffleds ();
          delay (100);
          turnonleds ();
          delay (100);
          turnoffleds ();
          delay (100);
          delay (2000);
        }
        else if (tempPassword != password) {
          lcd.clear ();
          lcd.setCursor(0,0);
          lcd.print("Wrong Password!!");
          lcd.setCursor(0,1);
          lcd.print("Please try again");
          tone(buzzer, 1000, 200);
          delay(2000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" Factory Control");
          lcd.setCursor(0,1);
          lcd.print("     Panel      "); 
          lcd.setCursor(0,2);
          lcd.print("  Please enter  ");
          lcd.setCursor(0,3);
          lcd.print("Password:       ");
          t = 0;
          k=9;
          tempPassword = "";
        }
      }    
    }
}
void turnoffleds (){
  noTone (buzzer);
  digitalWrite (isl, LOW);
  digitalWrite (iwl, LOW);
  digitalWrite (ifl, LOW);
  digitalWrite (isml,LOW);
  digitalWrite (ill, LOW);
  digitalWrite (osl, LOW);
  digitalWrite (owl, LOW);
  digitalWrite (ofl, LOW);
  digitalWrite (osml,LOW);
  digitalWrite (oll, LOW);
  digitalWrite (wsl, LOW);
  digitalWrite (wwl, LOW);
  digitalWrite (wfl, LOW);
  digitalWrite (wsml,LOW);
  digitalWrite (wll, LOW);
  digitalWrite (cal, LOW);
  digitalWrite (gol, LOW);
  digitalWrite (gcl, LOW);
  digitalWrite (rol, LOW);
  digitalWrite (rrl, LOW);
  digitalWrite (rtl, LOW);
  digitalWrite (fal, LOW);
  digitalWrite (fbl, LOW);
  digitalWrite (fcl, LOW);
  digitalWrite (col, LOW);
  digitalWrite (cofl,LOW);
}
void turnonleds (){
  digitalWrite (isl, HIGH);
  digitalWrite (iwl, HIGH);
  digitalWrite (ifl, HIGH);
  digitalWrite (isml,HIGH);
  digitalWrite (ill, HIGH);
  digitalWrite (osl, HIGH);
  digitalWrite (owl, HIGH);
  digitalWrite (ofl, HIGH);
  digitalWrite (osml,HIGH);
  digitalWrite (oll, HIGH);
  digitalWrite (wsl, HIGH);
  digitalWrite (wwl, HIGH);
  digitalWrite (wfl, HIGH);
  digitalWrite (wsml,HIGH);
  digitalWrite (wll, HIGH);
  digitalWrite (cal, HIGH);
  digitalWrite (gol, HIGH);
  digitalWrite (gcl, HIGH);
  digitalWrite (rol, HIGH);
  digitalWrite (rrl, HIGH);
  digitalWrite (rtl, HIGH);
  digitalWrite (fal, HIGH);
  digitalWrite (fbl, HIGH);
  digitalWrite (fcl, HIGH);
  digitalWrite (col, HIGH);
  digitalWrite (cofl,HIGH);
}