#include <Arduino.h>
#include <Servo.h>
#include <SPI.h>
#include <Pixy.h>

boolean TESTEACTIVE ;//= true; //******************************
//********************$$$$$**********************************

Servo ME; //motor esquerdo
Servo MD; //motor direito
Servo FL; //forklift

Pixy pixy;

#define pinME 4
#define pinMD 8
#define pinFL 6

#define piezzo 10
#define LDRE A0
#define LDREC A1
#define LDRC A2
#define LDRDC A3
#define LDRD A4

// Para a pixy
String color = "Black";
int signature = 0;

int lightEsq = 0;
int lightCenEsq = 0;
int lightCen = 0;
int lightCenDir = 0;
int lightDir = 0;

//////////////////////////////////////////////////////ALTERAR NO CAMPO!!!!!!
int limiarEsq = 130;
int limiarCenEsq = 140;
int limiarCen = 240;        //menor -> braco maior -> preto
int limiarCenDir = 130;
int limiarDir = 130; 
/////////////////////////////////////////////////////////////////////////////

boolean dir = false;
boolean cenDir = false;
boolean cen = false;
boolean cenEsq = false;
boolean esq = false;

boolean aRecolher = true;
int entregues = 0;

void Desloca (int esq, int dir, int tempo) {
  AttachM();
  MD.write(dir);
  ME.write(esq);
  delay(tempo);
}

void Frente() {
  Desloca(0, 180, 650);
}
void Para() {
  Desloca(93, 93, 100);
  DetachM();
}

void Tras(int i) {
  Desloca(180, 0, i);
}

void Dir90() {
  Desloca(0, 0, 1200);
  Para();
}

void Esq90() {
  Desloca(180, 180, 1200);
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

void SeguirLinha() { //
  LerLinha();
  if ((!esq && !cenEsq && cen && !cenDir && !dir) || (!esq && cenEsq && cen && cenDir && !dir) ||
      (!esq && cenEsq && !cen && cenDir && !dir)) { //00100 ou 01110 ou 01010
    Desloca(0, 130, 100);
  } else if ((!esq && cenEsq && cen && !cenDir && !dir)||(!esq && cenEsq && !cen && !cenDir && !dir)) { //01100 01000
    Desloca(60, 180, 100);
  } else if ((!esq && !cenEsq && cen && cenDir && !dir)||(!esq && !cenEsq && !cen && cenDir && !dir)) { //00110 00010
    Desloca(0, 120, 100);
  } else if (!esq && cenEsq && !cen && !cenDir && !dir) { //01000
    Desloca(70, 180, 100);
  } else if (!esq && !cenEsq && !cen && cenDir && !dir) { //00010
    Desloca(0, 120, 100);
  } else if (!esq && !cenEsq && !cen && cenDir && dir) { //00011
    Desloca(0, 110, 100);
  } else if (esq && cenEsq && !cen && !cenDir && !dir) { //11000
    Desloca(80, 180, 100);
  } else {
    Para();
    //tone(piezzo, 1000);
    delay(100);
    //noTone(piezzo);
    Serial.println("Para");
  }
}


void levanta () {
  int pos;
  for (pos = 90; pos >= 70; pos -= 1) { 
    FL.write(pos);              
    delay(15);                      
  }
  FL.write(80);
}
void para() {
  FL.write(90);
}

void baixa () {
  int pos = 0;
  for (pos = 90; pos <= 140; pos += 1) { 
    // in steps of 1 degree
    FL.write(pos);              
    delay(15);                       
  }
  FL.write(120);
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
            case 'x':
              Para();
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
void Apito(int freq) {
  tone(piezzo, freq);
  delay(100);
  noTone(piezzo);
}

void IgnoraCentrais() { //Inicio
  int linhaEsq = 0;
  
  while (linhaEsq <= 2) {
    SeguirLinha();

    if (LinhaEsq() || LinhaDir() || LinhaTudo()) { 
      Apito(1800);
      Desloca(0, 130, 500);
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

void DesclocaAteCaixa() {
  for (int i = 0; i < 5; i++) {
    SeguirLinha();
  }
}

void RotinaS() {
  int count = 0;

  Serial.print("Starting S!");
  baixa();
  DesclocaAteCaixa();
  if (aRecolher) {
   levanta();
  }
  Tras(500);
  Tras(500);
  if (!aRecolher){
    levanta();
  }  
  Dir90();
  Dir90();
  Tras(500);
  Tras(500);
  while (count <= 1) { //Enquanto nao ha linha a equerda nem em todo o lado
    SeguirLinha();
    if (LinhaEsq() || LinhaTudo()) {
      count++;
    }
  }
  //Frente();
  count = 0;
  Para();
  Apito(1200);
  Frente();
  Esq90();
  while (!LinhaDir()) { // nao ha linha na direita
    SeguirLinha();
    if (LinhaEsq() || LinhaTudo()) { //ha linha na direita
      Desloca(0, 130, 300);
    }
  }
  Apito(2000);
  Frente();
  Dir90();
  IgnoraCentrais();
  Frente();
  Tras(500);
  Esq90(); 
  if (aRecolher) {
    while (count <= entregues) {
      SeguirLinha();
      if (LinhaDir()) { //ha linha na direita
        Desloca(0, 130, 300);
        count++; /////////////////////////////
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
  baixa();
  Frente();
  Frente();
  para();
  aRecolher = !aRecolher;
}

void setup() {
  Para();
  Serial.begin(9600);
  Apito(1000);
  FL.attach(pinFL);
  levanta();
  Serial.print("Waiting for Mr.Led...\n");
  pixy.init();
  if (TESTEACTIVE) {
    Teste();
  }
}

void loop() {
  
  IgnoraCentrais();
  for(int i=0; i<9;i++){
    RotinaS();
    Apito(600);
  }
 
}
