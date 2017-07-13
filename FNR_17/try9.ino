boolean TESTEACTIVE ;//= true; //******************************
//********************$$$$$**********************************
int t = 500;
#include <Servo.h>

#include <SPI.h>
#include <Pixy.h>

Servo ME; //motor esquerdo
Servo MD; //motor direito
Servo FL; //forklift

Pixy pixy;

#define pinME 4
#define pinMD 8
#define pinFL 6

#define piezzo 3
#define OE 11
#define OD 12

#define BS A3
#define LDRE A4
#define LDRC A2
#define LDRD A0
#define LDRDC A1
#define LDREC A3

// Para a pixy
String color = "Black";
int signature = 0;

int lightEsq = 0;
int lightCenEsq = 0;
int lightCen = 0;
int lightCenDir = 0;
int lightDir = 0;

//////////////////////////////////////////////////////ALTERAR NO CAMPO!!!!!!
int limiarDir = 130; //menor -> braco maior -> preto
int limiarCenDir = 120;
int limiarCen = 200;
int limiarCenEsq = 130;
int limiarEsq = 110;

boolean dir = false;
boolean cenDir = false;
boolean cen = false;
boolean cenEsq = false;
boolean esq = false;

void LevantaFL() {
  int pos;
  FL.attach(pinFL);
  for (pos = 100; pos >= 90; pos -= 1) { 
    FL.write(pos);             
    delay(15);                       
  }
}

void BaixaFL() {
  int pos = 0;
  FL.attach(pinFL);
  for (pos = 0; pos <= 90; pos += 1) { 
    FL.write(pos);              
    delay(15);                       
  }
  FL.detach();
}

void Desloca (int esq, int dir, int tempo) {
  AttachM();
  MD.write(dir);
  ME.write(esq);
  delay(tempo);
}

void Frente() {
  Desloca(0, 180, 10);
}
void Para() {
  Desloca(93, 93, 100);
  DetachM();
}

void Tras() {
  Desloca(180, 0, 100);
}

void Esq45() {
  Desloca(0, 0, 1000);
}

void EsqAjuste() {
  Desloca(0, 0, 10);
}

void Dir45() {
  Desloca(180, 180, 1000);
}

void DirAjuste() {
  Desloca(180, 180, 10);
}

void AttachM() { //Ativa as lagartas
  ME.attach(pinME);
  MD.attach(pinMD);
}

void DetachM() { //Desativa as lagartas
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
  } else if (!esq && cenEsq && cen && !cenDir && !dir) { //01100
    Desloca(60, 180, 100);
  } else if (!esq && !cenEsq && cen && cenDir && !dir) { //00110
    Desloca(0, 120, 10);
  } else if (!esq && cenEsq && !cen && !cenDir && !dir) { //01000
    Desloca(60, 180, 10);
  } else if (!esq && !cenEsq && !cen && cenDir && !dir) { //00010
    Desloca(0, 120, 10);
  /*} else if (!esq && !cenEsq && !cen && cenDir && dir) { //00011
    Desloca(70, 180, 10);
  } else if (esq && cenEsq && !cen && !cenDir && !dir) { //11000
    Desloca(90, 0, 10);
  } else if (esq && !cenEsq && !cen && !cenDir && !dir) { //10000
    Desloca(90, 0, 100);
    Desloca(0, 90, 10);
  } else if (!esq && !cenEsq && !cen && !cenDir && dir) { //00001
    Desloca(180, 90, 100);
    Desloca(180, 0, 10);*/
  } else {
    Para();
    //tone(piezzo, 1000);
    delay(100);
    //noTone(piezzo);
    Serial.println("Para");
  }
}

void Ajusta() {
  LerLinha();
  while (!cenEsq || !cen) {
    LerLinha();
    if (!cenEsq) {
      DirAjuste();
      Para();
      Serial.println("Ajusta Dir");
    }
    LerLinha();
    if (!cenDir) {
      EsqAjuste();
      Para();
      Serial.println("Ajusta Esq");
    }
    delay(400); //////////////////////////////TIRAR///////////////////
  }
}


void Teste() { //Ler todos os sensores
  LerLinha();
  delay(400);
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
        //Serial.print(buf);
        pixy.blocks[j].print();
        signature = pixy.blocks[j].signature;
      }
    }
  }
  color = colorOf(signature);
  if(color !="")
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

void setup() {
  Para();
  Serial.begin(9600);
  tone(piezzo, 1200);
  delay(1000);
  noTone(piezzo);

  //LightsUp(); // apenas um olho :(
  //delay(500);
  //AttachM();
  //BaixaFL();
  LevantaFL();
  //FL.attach(pinFL);
  //FL.write(93);     
  Serial.print("Waiting for Mr.Led...\n");
  pixy.init();
}

void loop() {

  if (TESTEACTIVE) {
    while (true) {
      Teste();
    }
  }
  //Para();
  SeguirLinha();
  //delay(400);
  //LevantaFL();
  //delay(300);
  //BaixaFL();
  //delay(300);
  //Esq45();
  //Frente();
  //printPixy();
  //Serial.println(color);

  //delay(400);
  //BaixaFL();
  //delay(400);
  //LevantaFL();

}
