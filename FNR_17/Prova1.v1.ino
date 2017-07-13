#include <Arduino.h>
#include <Servo.h>
#include <SPI.h>
#include <Pixy.h>

boolean TESTEACTIVE  = true; //******************************
//********************$$$$$**********************************
int t = 500;

int velP = 150;

Servo FL; //forklift

Pixy pixy;

#define PinFL 4

#define piezzo 10
#define OE 11
#define OD 12

#define LDRE A4
#define LDRC A3
#define LDRD A2
#define LDRDC A1
#define LDREC A0

int VD = 5;     //MD Speed Control 
int VE = 6;     //ME Speed Control
int MD = 4;    //MD Direction Control
int ME = 7;    //ME Direction Control

int pos = 0;    // variable to store the servo position

// Para a pixy
String color = "Black";
int signature = 0;

int lightEsq = 0;
int lightCenEsq = 0;
int lightCen = 0;
int lightCenDir = 0;
int lightDir = 0;

//////////////////////////////////////////////////////ALTERAR NO CAMPO!!!!!!
int limiarDir = 160; //menor -> braco maior -> preto
int limiarCenDir = 160;
int limiarCen = 260;
int limiarCenEsq = 140;
int limiarEsq = 160;
/////////////////////////////////////////////////////////////////////////////

boolean dir = false;
boolean cenDir = false;
boolean cen = false;
boolean cenEsq = false;
boolean esq = false;

boolean pacote = false;
boolean aRecolher = true;
int entregues = 0;


void stop(void)                    //Stop
{
  digitalWrite(VD,LOW);   
  digitalWrite(VE,LOW);      
}   
void advance(char a,char b)          //Move forward
{
  analogWrite (VD,a); //direita      //PWM Speed Control
  digitalWrite(MD,HIGH);    
  analogWrite (VE,b);  //esquerda  
  digitalWrite(ME,HIGH);
}  

void DeslocaT(char b,char a, int c){
  advance(b,a);
  delay(c);
  stop();
}  

void back_off (char a,char b)          //Move backward
{
  analogWrite (VD,a);
  digitalWrite(MD,LOW);   
  analogWrite (VE,b);    
  digitalWrite(ME,LOW);
}

void back_offT (int a)          //Move backward
{
   back_off (150,150);
   delay(a);
   stop();
}  
void turn_D (char a,char b)             //Turn Left
{
  analogWrite (VD,a);
  digitalWrite(MD,LOW);    
  analogWrite (VE,b);    
  digitalWrite(ME,HIGH);
}
void Dir90(){ // Fazer com sensores chao
  while(!LinhaCentro()){
  turn_D(150,150);
  delay(100);
  }
  stop();
}

void Esq90(){ // Fazer com sensores chao
  while(!LinhaCentro()){
  turn_E(150,150);
  delay(100);
  }
  stop();
}

void turn_E (char a,char b)             //Turn Right
{
  analogWrite (VD,a);
  digitalWrite(MD,HIGH);    
  analogWrite (VE,b);    
  digitalWrite(ME,LOW);
}
void levanta (){ 
for (pos = 90; pos <= 140; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    FL.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  FL.write(92);
}

void baixa (){ 
  for (pos = 90; pos >= 70; pos -= 1) { // goes from 180 degrees to 0 degrees
    FL.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  FL.write(86);
}

void LerLinha() {
  lightEsq = analogRead(LDRE);
  Serial.print("SensorEsq: ");
  Serial.println(lightEsq);

  lightCenEsq = analogRead(LDREC);
  Serial.print("SensorCenEsq: ");
  Serial.println(lightCenEsq);

  lightCen = analogRead(LDRC);
  Serial.print("SensorCen: ");
  Serial.println(lightCen);

  lightCenDir = analogRead(LDRDC);
  Serial.print("SensorCenDir: ");
  Serial.println(lightCenDir);

  lightDir = analogRead(LDRD);
  Serial.print("SensorDir: ");
  Serial.println(lightDir);

  Serial.println();

  if (limiarEsq < lightEsq) {
    Serial.print("Preto-");
    esq = true;
  } else {
    Serial.print("Branco-");
    esq = false;
  }

  if (limiarCenEsq < lightCenEsq) {
    Serial.print("Preto-");
    cenEsq = true;
  } else {
    Serial.print("Branco-");
    cenEsq = false;
  }

  if (limiarCen < lightCen) {
    Serial.print("Preto-");
    cen = true;
  } else {
    Serial.print("Branco-");
    cen = false;
  }

  if (limiarCenDir < lightCenDir) {
    Serial.print("Preto-");
    cenDir = true;
  } else {
    Serial.print("Branco-");
    cenDir = false;
  }

  if (limiarDir < lightDir) {
    Serial.println("Preto");
    dir = true;
  } else {
    Serial.println("Branco");
    dir = false;
  }

  Serial.print(esq);
  Serial.print(cenEsq);
  Serial.print(cen);
  Serial.print(cenDir);
  Serial.print(dir);
  Serial.println();
}

void Apito(int freq) {
  tone(piezzo, freq);
  delay(1000);
  noTone(piezzo);
}

void IgnoraCentrais() { //Inicio
  int linhaEsq = 0;
  while (linhaEsq <= 2) {
    SeguirLinha();

    if (LinhaEsq() || LinhaDir() || LinhaTudo()) { //ha linha na direita
      DeslocaT(0, 130, 300);
      linhaEsq++;
    }
  }
}

boolean LinhaDir() { //ha linha na direita?
  if ((!esq && cenEsq && cen && cenDir && dir) //01111
      || (!esq && !cenEsq && !cen && cenDir && dir) //00011
      || (!esq && !cenEsq && cen && cenDir && dir)) { //00111
    Serial.println("LinhaDir!");
    return true;
  } else {
    return false;
  }
}

boolean LinhaEsq() { //ha linha na Equerda?
  if ((esq && cenEsq && cen && cenDir && !dir) //11110
      || (esq && cenEsq && !cen && !cenDir && !dir) //11000
      || (esq && cenEsq && cen && !cenDir && !dir)) { //11100
    Serial.println("LinhaEsq!");
    return true;
  } else {
    return false;
  }
}
boolean LinhaTudo() { //ha linha por todo o lado?
  if (esq && cenEsq && cen && cenDir && dir) { //11111
    Serial.println("LinhaTudo!");
    return true;
  } else {
    return false;
  }
}

boolean LinhaCentro() { //ha linha por todo o lado?
  if ((!esq && !cenEsq && cen && !cenDir && !dir) || (!esq && cenEsq && cen && cenDir && !dir) ||
      (!esq && cenEsq && !cen && cenDir && !dir)) { //00100 ou 01110 ou 01010
    Serial.println("LinhaCentro!");
    return true;
  } else {
    return false;
  }
}

void DesclocaAteCaixa() {

  //Desloca(0, 130, 1000);
  for(int i=0; i<10; i++){
    SeguirLinha();
  }
}

void SeguirLinha() { //
  LerLinha();
  if (LinhaCentro()) { //00100 ou 01110 ou 01010
    advance(150, 150);
  } else if (!esq && !cenEsq && cen && cenDir && !dir) { //00110
    advance(150, 130);
  } else if (!esq && !cenEsq && !cen && cenDir && !dir) { //00010
    advance(150, 110);
  } else if (!esq && !cenEsq && !cen && cenDir && dir) { //00011
    advance(150, 90);    
  } else if (!esq && !cenEsq && !cen && !cenDir && dir) { //00001
    advance(150, 0); 
  } else if (!esq && cenEsq && cen && !cenDir && !dir) { //01100
    advance(130, 150);   
  } else if (!esq && cenEsq && !cen && !cenDir && !dir) { //01000
    advance(110, 150); 
  } else if (esq && cenEsq && !cen && !cenDir && !dir) { //11000
    advance(90, 150);  
  } else if (esq && !cenEsq && !cen && !cenDir && !dir) { //10000
    advance(0, 150);
  } else {
    stop();
    Serial.println("Para");
  }
}  

void Teste() { //Ler todos os sensores
  if (TESTEACTIVE) {
    while (true) {
      if(Serial.available()){
        char val = Serial.read();
        if(val != -1)
        {
          switch(val)
          {
          case 'w'://Move Forward
            advance (255,255);   //move forward in max speed
            break;
          case 's'://Move Backward
            back_off (255,255);   //move back in max speed
            break;
          case 'a'://Turn Left
            turn_E (100,100);
            break;       
          case 'd'://Turn Right
            turn_D (100,100);
            break;
          case 'q'://Turn Right
            levanta ();
            break;
          case 'e'://Turn Right
            baixa();
            break;    
          case 'z':
            Serial.println("Hello");
            break;
          case 'x':
            stop();
            break;
          }
        }
        else stop();  
      }
        //LerLinha();
    } 
  //delay(400);
  }
}

/************** PIXY *************/
void printPixy() { // atualiza o valor da cor da caixa
  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32];

  blocks = pixy.getBlocks();

  if (blocks) {
    i++;
    if (i % 50 == 0) { //50fps
      sprintf(buf, "Detected %d:\n", blocks);
      Serial.print(buf);
      for (j = 0; j < blocks; j++) {
        sprintf(buf, "  block %d: ", j);
        Serial.print(buf);
        pixy.blocks[j].print();
        signature = pixy.blocks[j].signature;
      }
    }
  }
  color = colorOf(signature);
  if (color != "")
    Serial.println(color);
}

String colorOf(int s) {
  switch (s) {
    case 1:
      return "Red";
      break;
    case 3:
      return "Green";
      break;
    case 2:
      return "Blue";
      break;
  }
}

void RotinaS() { // pensar nisto
  int count = 0;

  DesclocaAteCaixa();
  if (aRecolher) {
    levanta();
  } else {
    baixa();
  }
  back_offT(500);
  back_offT(500);
  Dir90();
  Dir90();
  while (count<1) { //Esquanto nao ha linha a equerda nem em todo o lado
    SeguirLinha();
    if(LinhaEsq() || LinhaTudo()){
      count++;
    }
  }
  count = 0;
  delay(400);
  Apito(1200);
  DeslocaT(velP,velP, 300);
  Esq90();
  while (!LinhaDir()) { // nao ha linha na direita
    SeguirLinha();
    if (LinhaEsq() || LinhaTudo()) { //ha linha na direita
      DeslocaT(0, 130, 300);
    }
  }
  Apito(2000);
  DeslocaT(velP,velP, 300);
  Dir90();
  IgnoraCentrais();
  DeslocaT(velP,velP, 300);
  Esq90();
  DeslocaT(velP,velP, 300);
  if (aRecolher) {
    while (count < entregues) {
      SeguirLinha();
      if (LinhaDir()) { //ha linha na direita
        DeslocaT(0, 130, 300);
        count++;
      }

    }
    Dir90();
    DesclocaAteCaixa();
    baixa();

  } else {

    while (count < (5 - entregues)) {
      SeguirLinha();
      if (LinhaDir()) { //ha linha na direita
        DeslocaT(0, 130, 300);
        count++;
      }
    }
    Dir90();
  }

  aRecolher = !aRecolher;

}

void setup(void) { 
  int i;
  for(i=4;i<=7;i++)
    pinMode(i, OUTPUT);  
  pinMode(PinFL, OUTPUT);
  
  FL.attach(PinFL);  
  Serial.begin(9600);      //Set Baud Rate
  Apito(1200);
  //pixy.init();
  if (!TESTEACTIVE){
    IgnoraCentrais(); // Rotina de Inicio
  }else{
    Serial.println("Run keyboard control");
    //Teste();
  }  
} 

void loop(void){
  advance(100,100);
  delay(1000);
  back_off(100,100);
  delay(1000);
  turn_E(100,100);
  delay(1000);
  turn_D(100,100);
  Serial.print("Starting S!");
  //RotinaS(); 
}

