#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <meta charset_UTF-8>
#include <LiquidCrystal.h> //Inclui a biblioteca

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;//Pinos para ligar o display
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);//Define os pinos que serão usados para ligar o display

void setup() {

  lcd.begin(16, 2);//Define o número de colunas e linhas do display
  lcd.setCursor(0, 0);//Define o cursor para a coluna 0, linha 0
  lcd.print("UCL - Trabalho 1 PA");
  lcd.setCursor(0, 1);//Define o cursor para a coluna 0, linha 1
  lcd.print(" SHIELD DISPLAY ");
  Serial.begin(9600);
}

//Função para botão select
void select() {
  lcd.clear();
  lcd.setCursor(0, 0);//Define o cursor para a coluna 0, linha 0
  lcd.print("APERTOU O BOTAO");
  lcd.setCursor(0, 1);//Define o cursor para a coluna 0, linha 1
  lcd.print("     SELECT     ");
}

//Função para botão left
void left() {
  lcd.clear();
  lcd.setCursor(0, 0);//Define o cursor para a coluna 0, linha 0
  lcd.print("APERTOU O BOTAO");
  lcd.setCursor(0, 1);//Define o cursor para a coluna 0, linha 1
  lcd.print("      LEFT      ");
}

//Função para botão up
void up() {
  lcd.clear();
  lcd.setCursor(0, 0);//Define o cursor para a coluna 0, linha 0
  lcd.print("APERTOU O BOTAO");
  lcd.setCursor(0, 1);//Define o cursor para a coluna 0, linha 1
  lcd.print("       UP       ");
}

//Função para botão down
void down() {
  lcd.clear();
  lcd.setCursor(0, 0);//Define o cursor para a coluna 0, linha 0
  lcd.print("APERTOU O BOTAO");
  lcd.setCursor(0, 1);//Define o cursor para a coluna 0, linha 1
  lcd.print("      DOWN      ");
}

//Função para botão right
void right() {
  lcd.clear();
  lcd.setCursor(0, 0);//Define o cursor para a coluna 0, linha 0
  lcd.print("APERTOU O BOTAO");
  lcd.setCursor(0, 1);//Define o cursor para a coluna 0, linha 1
  lcd.print("     RIGHT      ");
}

void loop() {
  Serial.println(analogRead(0));//Exibe a leitura do pino analógico A0 no monitor serial
  delay(100);

  if ((analogRead(0)) < 80) { //Se a leitura for menor que 80 chama a função right
    right();
  }
  else if ((analogRead(0)) < 200) {//Se a leitura for menor que 200 chama a função up
    up();
  }
  else if ((analogRead(0)) < 400) {//Se a leitura for menor que 400 chama a função down
    down();
  }
  else if ((analogRead(0)) < 600) {//Se a leitura for menor que 600 chama a função left
    left();
  }
  else if ((analogRead(0)) < 800) {//Se a leitura for menor que 800 chama a função left
    select();
  }
}
