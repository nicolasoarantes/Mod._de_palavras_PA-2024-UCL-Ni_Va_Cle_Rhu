#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include <String.h>

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String frase;

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Digite uma frase:");
  Serial.begin(9600);
}

void loop() {
  while (!Serial.available()) {} // Espera até que algo seja digitado no terminal
  frase = Serial.readStringUntil('\n'); // Lê a frase digitada pelo usuário até o caractere de nova linha

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Frase digitada:");
  lcd.setCursor(0, 1);
  lcd.print(frase);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digite uma palavra:");
  lcd.setCursor(0, 1);
  lcd.print("da frase para substituir:");


  while (!Serial.available()) {} // Espera até que algo seja digitado no terminal
  String palavraEscolhida = Serial.readStringUntil('\n'); // Lê a palavra escolhida pelo usuário até o caractere de nova linha
  palavraEscolhida.trim(); // Remove espaços em branco do início e do fim da palavra


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Palavra escolhida:");
  lcd.setCursor(0, 1);
  lcd.print(palavraEscolhida);

  delay(2000); // Aguarda 2 segundos antes de continuar
  for (int i = 0; i < frase.length() - 16; i++) {
    lcd.scrollDisplayLeft();
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digite a palavra");
  lcd.setCursor(0, 1);
  lcd.print("para substituir:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < frase.length() - 16; i++) {
    lcd.scrollDisplayLeft();
    delay(500);
  }
  while (!Serial.available()) {} // Espera até que algo seja digitado no terminal
  String palavraInserida = Serial.readStringUntil('\n'); // Lê a palavra a ser inserida no lugar da palavra escolhida até o caractere de nova linha
  palavraInserida.trim(); // Remove espaços em branco do início e do fim da palavra

  // Substitui a palavra escolhida pela palavra inserida na frase
  frase.replace(palavraEscolhida, palavraInserida);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nova frase:");
  lcd.setCursor(0, 1);
  lcd.print(frase);

  // Rolagem da frase caso ela ultrapasse o limite das 2 linhas
  for (int i = 0; i < frase.length() - 16; i++) {
    lcd.scrollDisplayLeft();
    delay(500);
  }

  delay(5000); // Aguarda 5 segundos antes de reiniciar o loop
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digite uma frase:");
}