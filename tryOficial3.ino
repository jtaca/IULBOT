#include <Arduino.h>

boolean TESTEACTIVE  ;//= true; //******************************
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

void Desloca (int esq, int dir, int tempo) {
  AttachM();
  MD.write(dir);
  ME.write(esq);
  delay(tempo);
}

void Frente() {
  Desloca(0, 180, 400);
}
void Para() {
  Desloca(93, 93, 100);
  DetachM();
}

void Tras() {
  Desloca(180, 0, 500);
}

void Dir90() {
  Desloca(0, 0, 550);
  Para();
}

void Esq90() {
  Desloca(180, 180, 550);
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
  } else if (!esq && cenEsq && cen && !cenDir && !dir) { //01100
    Desloca(60, 180, 100);
  } else if (!esq && !cenEsq && cen && cenDir && !dir) { //00110
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

/*void Ajusta() {
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
  }
  }*/

void BaixaFL() {
  int pos;
  FL.attach(pinFL);
  for (pos = 90; pos <= 110; pos += 1) {
    FL.write(pos);
    delay(15);
  }
  Apito(2000);
  ParaFL();

  //pacote = true;
}
void ParaFL() {
  FL.write(93);
  delay(100);
  FL.write(92);
}

void LevantaFL() {
  int pos = 0;
  FL.attach(pinFL);
  for (pos = 90; pos >= 50; pos -= 1) {
    FL.write(pos);
    delay(15);
  }
  FL.write(90);
  //ParaFL();
  //FL.detach();
  pacote = false;
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
  delay(1000);
  noTone(piezzo);
}

void IgnoraCentrais() { //Inicio
  int linhaEsq = 0;
  while (linhaEsq <= 2) {
    SeguirLinha();

    if (LinhaEsq() || LinhaDir() || LinhaTudo()) { //ha linha na direita
      Desloca(0, 130, 300);
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

  //Desloca(0, 130, 1000);
  for(int i=0; i<10; i++){
    SeguirLinha();
  }
}

void RotinaS() {
  int count = 0;

  DesclocaAteCaixa();
  if (aRecolher) {
    LevantaFL();
  } else {
    BaixaFL();
  }
  Tras();
  Tras();
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
  Esq90();
  Frente();
  if (aRecolher) {
    while (count < entregues) {
      SeguirLinha();
      if (LinhaDir()) { //ha linha na direita
        Desloca(0, 130, 300);
        count++;
      }

    }
    Dir90();
    DesclocaAteCaixa();
    BaixaFL();

  } else {

    while (count < (5 - entregues)) {
      SeguirLinha();
      if (LinhaDir()) { //ha linha na direita
        Desloca(0, 130, 300);
        count++;
      }
    }
    Dir90();
  }

  aRecolher = !aRecolher;

}

void setup() {
  Para();
  Serial.begin(9600);
  Apito(1200);
  delay(1000);
  Apito(1200);
  Serial.print("Waiting for Mr.Led...\n");
  pixy.init();
  if (!TESTEACTIVE){
    IgnoraCentrais(); // Rotina de Inicio
  }

}

void loop() {
  if (TESTEACTIVE) {
    while (true) {
      Teste();
    }
  }
  Serial.print("Starting S!");
  RotinaS();
  /*LevantaFL();
  delay(2000);
  BaixaFL();
  delay(2000);*/


  //SeguirLinha();
  //LevantaFL();

}
