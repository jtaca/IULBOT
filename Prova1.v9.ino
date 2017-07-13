#include <Arduino.h>
#include <Servo.h>
#include <SPI.h>
#include <Pixy.h>

boolean TESTEACTIVE = true; //******************************
//********************$$$$$**********************************

Servo ME; //motor esquerdo
Servo MD; //motor direito
Servo FL; //forklift

Pixy pixy;

#define pinME 4
#define pinMD 8
#define pinFL 6

#define piezzo 12
#define LDRE A0
#define LDREC A1
#define LDRC A2
#define LDRDC A3
#define LDRD A4

// Para a pixy
int ftevaldir = 170;
int ftevalesq = 0;

String color = "Black";
int signature = 0;

int lightEsq = 0;
int lightCenEsq = 0;
int lightCen = 0;
int lightCenDir = 0;
int lightDir = 0;

//////////////////////////////////////////////////////ALTERAR NO CAMPO!!!!!!
int limiarEsq = 200;
int limiarCenEsq = 200;
int limiarCen = 250;        //menor -> braco maior -> preto
int limiarCenDir = 250;
int limiarDir = 200;
/////////////////////////////////////////////////////////////////////////////

boolean dir = false;
boolean cenDir = false;
boolean cen = false;
boolean cenEsq = false;
boolean esq = false;

boolean aRecolher = true;
int entregues = 0;
boolean pacote = false;

void Apito(int freq) {
  tone(piezzo, freq);
  delay(100);
  noTone(piezzo);
}

void Desloca (int esq, int dir, int tempo) {
  AttachM();
  MD.write(dir);
  ME.write(esq);
  delay(tempo);
}

void Frente() {
  Desloca(0, 180, 700);
}
void Para() {
  Desloca(93, 93, 100);
  //DetachM();
}

void Tras(int i) {
  Desloca(180, 0, i);
}

void Dir90() {
  Desloca(50, 50, 1500);
  while (!LinhaCen()) {
    LerLinha();
    Desloca(50, 50, 10);
  }
  Para();
}

void Esq90() {

  Desloca(150, 150, 1500);
  LerLinha();

  while (!LinhaCen()) {
    LerLinha();
    Desloca(150, 150, 10);
  }
  Para();
}

void AttachM() { //Ativa as Rodas
  ME.attach(pinME);
  MD.attach(pinMD);
}

void DetachM() { //Desativa as Rodas
  ME.detach();
  MD.detach();
}

boolean Trans(int val, int sen) {
  boolean bo;
  if (val > sen) {
    if(TESTEACTIVE)
    Serial.print("-Branco-");
    bo = true;
  } else {
    if(TESTEACTIVE)
      Serial.print("-Preto-");
    bo = false;
  }
  return bo;

}

void LerLinha() {
  int limiar = 0;
  if(TESTEACTIVE){
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
  }  

  if (pacote) {
    limiar = 0; //75
  } else {
    limiar = 0;
  }

  esq = Trans(limiarEsq + limiar, lightEsq);
  cenEsq = Trans(limiarCenEsq + limiar, lightCenEsq);
  cen = Trans(limiarCen + limiar, lightCen);
  cenDir = Trans(limiarCenDir + limiar, lightCenDir);
  dir = Trans(limiarDir + limiar, lightDir);
  if(TESTEACTIVE){
  Serial.print(esq);
  Serial.print(cenEsq);
  Serial.print(cen);
  Serial.print(cenDir);
  Serial.print(dir);

  Serial.println();
  }
}

void SeguirLinha() { //
  int avanco = 10;
  LerLinha();
  
  if ((!esq && !cenEsq && cen && !cenDir && !dir) || 
      (!esq && cenEsq && cen && cenDir && !dir) ||
      (!esq && cenEsq && !cen && cenDir && !dir)) { //00100 ou 01110 ou 01010
      
         Desloca(ftevalesq, ftevaldir, avanco);
    
  } else if ((!esq && cenEsq && cen && !cenDir && !dir) ||
            (!esq && cenEsq && !cen && !cenDir && !dir)) { //01100 01000
    
         Desloca(ftevalesq+20, ftevaldir, avanco);
    
  } else if ((!esq && !cenEsq && cen && cenDir && !dir)
             || (!esq && !cenEsq && !cen && cenDir && !dir)) { //00110 00010
    Desloca(ftevalesq, ftevaldir-40, avanco);
    
  } else if (!esq && cenEsq && !cen && !cenDir && !dir) { //01000
    Desloca(ftevalesq+40, ftevaldir, avanco);
    
  } else if (!esq && !cenEsq && !cen && cenDir && !dir) { //00010
    Desloca(ftevalesq, ftevaldir - 60, avanco);
    
  } else if (!esq && !cenEsq && !cen && cenDir && dir) { //00011
    Desloca(ftevalesq, ftevaldir - 80, avanco);
    
  } else if (esq && cenEsq && !cen && !cenDir && !dir) { //11000
    Desloca(ftevalesq+60, ftevaldir, avanco);
    
  } else {
    Para();
    delay(10);
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
  if ((esq && cenEsq && cen && cenDir && dir) || (esq && cenEsq && !cen && cenDir && dir)) { //11111 e 11011
    Serial.println("LinhaTudo!");
    return true;
  } else {
    return false;
  }
}

boolean LinhaCen() { //ha linha oa centro?
  if ((!esq && cenEsq && cen && cenDir && !dir) || //01110
      (!esq && !cenEsq && cen && cenDir && !dir) || //00110
      (!esq && cenEsq && cen && !cenDir && !dir) || //01100
      (!esq && !cenEsq && cen && !cenDir && !dir)) { //00100
    Serial.println("LinhaCen!");
    return true;
  } else {
    return false;
  }
}

void levanta () {
  int pos;
  pacote = true;
  for (pos = 50; pos >= 0; pos -= 1) {
    FL.write(pos);
    delay(15);
  }
}
void parafl() {
  FL.write(40);
}


void baixa () {
  int pos;
  pacote = false;
  for (pos = 0; pos <= 50; pos += 1) {
    FL.write(pos);
    delay(15);
  }
  Tras(400);
  Para();
  FL.write(50);
}

void Teste() { //Ler todos os sensores

  if (TESTEACTIVE) {
    while (true) {
      if (Serial.available()) {
        char val = Serial.read();
        if (val != -1)
        {
          switch (val)
          {
            case 'w'://Move Forward
              Frente();
              break;
            case 's'://Move Backward
              Tras(500);
              break;
            case 'a'://Turn Left
              Esq90();
              break;
            case 'd'://Turn Right
              Dir90();
              break;
            case 'q':
              levanta();
              break;
            case 'e':
              baixa();
              break;
            case 'z':
              LerLinha();
              break;
            case 'p':
              printPixy();
              break;  
            case 'x':
              Desloca(90, 90, 100);
              break;
            case 'c':
              IgnoraCentrais();
              break;

          }
        }
        else Para();
      }
    }
  }
}

/************** PIXY *************/
void printPixy() { // atualiza o valor da cor da caixa
  static int i = 0;
  uint16_t blocks;
  boolean c = false;

  while (!c) {
    blocks = pixy.getBlocks();
    if (blocks) {
      i++;
      for (int j = 0; j < blocks; j++) {
        signature = pixy.blocks[j].signature;
        Serial.println(signature);
      }
    }
    if ((signature >= 1) && (signature < 4)) {
      c = true;
    }
    /*color = colorOf(signature);
      if (color != "")
      Serial.println(color);
    */
  }
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

void IgnoraCentrais() { //Inicio
  int linhaEsq = 0;
  Serial.print("R-IC");
  LerLinha();
  while (linhaEsq <= 2) {
    SeguirLinha();
    if (/*LinhaEsq() ||*/ LinhaDir() || LinhaTudo()) {
      Apito(1800);
      Desloca(0, 150, 500);
      linhaEsq++;
    }
  }
  Para();
}

void DesclocaAteCaixa() {
  Serial.print("R-DAC");
  for (int i = 0; i < 100; i++) {
    SeguirLinha();
  }
  Para();
}

void DesclocaAteMaquina() {
  for (int i = 0; i < 5; i++) {
    SeguirLinha();
  }
  Para();
}

void RotinaS() {
  int count = 0;

  Serial.print("Starting S!");
  DesclocaAteCaixa();
  if (aRecolher) {
    //printPixy();
    levanta();
  }
  if (!aRecolher) {
    baixa();
  }
  /*while (count <= 1) { //Enquanto nao ha linha a equerda nem em todo o lado
    SeguirLinha();
    Tras(100);
    if (LinhaDir() || LinhaTudo()) {
      count++;
    }
  }*/
  count = 0;
  Tras(1300);
  
  Dir90();
  Apito(1200);
  Frente();
  Tras(400);
  //Frente();
  //Esq90();
  while (!LinhaDir()) { // nao ha linha na direita
    SeguirLinha();
    if (LinhaEsq() || LinhaTudo()) { //ha linha na direita
      Desloca(0, 140, 400);
    }
  }
  Apito(2000);
  Frente();
  Tras(700);
  Dir90();
  /*switch (signature) {
    case 1:
      Esq90();
      while (count <= 1) { //Enquanto nao ha linha a equerda nem em todo o lado
        SeguirLinha();
        if (LinhaTudo()) {
          count++;
        }
      }
      SeguirLinha();
      Para();
      break;*
    case 2:
      while (!LinhaTudo()) {
        SeguirLinha();
      }
      Frente();
      Esq90();
      DesclocaAteMaquina();
      baixa();
      Tras(500);
      delay(10000);
      DesclocaAteCaixa();
      levanta();
      Tras(500);
      Dir90();
      Dir90();
      while (!LinhaTudo()) {
        SeguirLinha();
      }
      Frente();
      Esq90();
      while (!LinhaTudo()) {
        SeguirLinha();
      }
      Frente();
      while (!LinhaTudo()) {
        SeguirLinha();
      }
      break;
    case 3:*/
      IgnoraCentrais();
      /*break;
  }*/
  Frente();
  Tras(500);
  Esq90();

  if (aRecolher) {
    while (count <= entregues) {
      SeguirLinha();
      if (LinhaDir()) { //ha linha na direita
        Desloca(0, 130, 300);
        count++; 
      }

    }
    entregues++;
  } else {
    while (count <= (5 - entregues)) {
      SeguirLinha();
      if (LinhaDir()) { //ha linha na direita
        Desloca(0, 130, 300);
        count++;
      }
    }
  }
  Frente();
  Tras(250);
  Dir90();
  Frente();
  /*Frente();
    Frente();
    baixa();
    Frente();*/
  Para();
  aRecolher = !aRecolher;
  signature = 0;
}

void setup() {
  Para();
  Serial.begin(19200);
  Apito(1000);
  FL.attach(pinFL);
  parafl();
  Serial.println("Good Luck!-");
  pixy.init();
  if (TESTEACTIVE) {
    Teste();
    Serial.println("Teste)");
  }
}

void loop() {
  IgnoraCentrais();
  for (int i = 0; i < 9; i++) {
    RotinaS();
    Apito(600);
  }
  //SeguirLinha();

}

