boolean TESTEACTIVE ;//= true; //******************************
//********************$$$$$**********************************
int t = 500;
#include <Servo.h>
Servo BE; //braço esquerdo
Servo BD; //braço direito
Servo ME; //motor esquerdo
Servo MD; //motor direito
Servo FL; //forklift
#define pinBE 5
#define pinBD 3
#define pinME 4
#define pinMD 8
#define pinFL 6

#define piezzo 9
#define OE 11
#define OD 12

#define BS A3
#define LDRE A2
#define LDRC A1
#define LDRD A0

int lightEsq = 0;
int lightCen = 0;
int lightDir = 0;

int limiarDir = 250; //menor -> braco maior -> preto
int limiarCen = 250;
int limiarEsq = 200;

boolean dir = false;
boolean cen = false;
boolean esq = false;


boolean wasPressed = false;

void Baixa() {
  AttachB();
  BE.write(0);
  BD.write(180);
  delay(100);
  BE.write(93);
  BD.write(87);
  DetachB();
}

void Levanta() {
  AttachB();
  BE.write(180);
  BD.write(0);
  delay(300);
  BE.write(93);
  BD.write(87);
  DetachB();
}

void LevantaFL() {
  int pos;
  FL.attach(pinFL);
  FL.write(0);
  delay(300);
  FL.write(0);
  FL.detach();
}

void BaixaFL() {
  FL.attach(pinFL);
  FL.write(130);
  delay(50);
  FL.write(90);
  FL.detach();
}

void Desloca (int esq, int dir, int tempo) {
  MD.write(dir);
  delay(100);
  ME.write(esq);
  delay(tempo);
}

void DeslocaFrente () {
  AttachM();
  MD.write(180);
  ME.write(0);
  //delay(1000);
  DetachM();
}

void Frente() {
  Desloca(180, 0, 100);
}
void Para() {
  Desloca(90, 90, 100);
}

void Tras() {
  Desloca(0, 180, 1000);
}

void Esq45() {
  Desloca(0, 0, 1000);
}

void Dir45() {
  Desloca(180, 180, 1000);
}

void Blink(int times) {
  for (int i = 0; i < times; i++) {
    LightsUp();
    delay(200);
    LightsDown();
    delay(200);
  }
}

void LightsUp() {
  digitalWrite(12, HIGH);
  digitalWrite(11, HIGH);
}

void LightsDown() {
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
}

void AttachM() { //Ativa as lagartas
  ME.attach(pinME);
  MD.attach(pinMD);
}

void AttachB() { //Ativar os braços
  BD.attach(pinBD);
  BE.attach(pinBE);
}

void DetachM() { //Desativa as lagartas
  ME.detach();
  MD.detach();
}

void DetachB() { //Desativa os braços
  BD.detach();
  BE.detach();
}

boolean Press() { //Verifica se o botão é primido
  Serial.print("Botão Press: ");
  Serial.println(analogRead(BS));
  if (wasPressed) {
    wasPressed = false;
  } else {
    wasPressed = true;
  }
  //delay(500);
  return analogRead(BS) >= 1020;
}

void LerLinha() {
  lightEsq = analogRead(LDRE);
  Serial.print("SensorEsq: ");
  Serial.println(lightEsq);
  lightCen = analogRead(LDRC);
  Serial.print("SensorCen: ");
  Serial.println(lightCen);
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
  if (limiarCen < lightCen) {
    Serial.print("Preto");
    cen = true;
  } else {
    Serial.print("Branco");
    cen = false;
  }
  if (limiarDir < lightDir) {
    Serial.println("-Preto");
    dir = true;
  } else {
    Serial.println("-Branco");
    dir = false;
  }
  Serial.print(esq); Serial.print(cen); Serial.print(dir);
  Serial.println();
}

void SeguirLinha() { //
  LerLinha();
  if (!esq && cen && !dir) {
    Frente();
    Serial.println("Frente");
  } else if(!esq && cen && dir) {
    Para();
    tone(piezzo,1000);
    delay(100);
    noTone(piezzo);
    Dir45();
    Frente();
    //Tras();
  }else{ 
    Para();
    Serial.println("Para");
  }
}


void Teste() { //Ler todos os sensores
  LerLinha();
  delay(400);
  Press();
}

void setup() {

  Serial.begin(9600);
  tone(piezzo, 1200);
  delay(1000);
  noTone(piezzo);

  AttachB();
  BD.write(0);
  delay(150);
  BD.write(87);
  BE.write(0);
  delay(50);
  BE.write(93);
  DetachB();

  LightsUp();
  delay(500);
  AttachM();
  Para();
}

void loop() {

  if (TESTEACTIVE) {
    LightsDown();
    while (true) {
      Teste();
    }
  }
  SeguirLinha();
  //Esq45();
 
}
