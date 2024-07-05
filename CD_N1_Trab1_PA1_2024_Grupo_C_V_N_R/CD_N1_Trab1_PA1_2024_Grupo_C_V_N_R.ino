#include <LiquidCrystal.h>

// Definindo os pinos do LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Definição dos valores dos botões
#define btnDIREITA  0
#define btnCIMA     1
#define btnBAIXO    2
#define btnESQUERDA 3
#define btnSELECIONAR 4
#define btnNENHUM   5

int botaoAtual = btnNENHUM; // Armazena o estado atual do botão
int opcaoSelecionada = 0; // Armazena a opção selecionada pelo usuário

unsigned long lastDebounceTime = 0;  // Tempo da última mudança de estado
unsigned long debounceDelay = 50;    // Tempo de debounce

// Função para ler os botões do LCD com debounce
int lerBotoesLCD() {
    int adc_valor = analogRead(0);  // Lê o valor analógico do pino A0
    int botaoLido = btnNENHUM;

    if (adc_valor < 50) {
        botaoLido = btnDIREITA;
    } else if (adc_valor < 195) {
        botaoLido = btnCIMA;
    } else if (adc_valor < 380) {
        botaoLido = btnBAIXO;
    } else if (adc_valor < 555) {
        botaoLido = btnESQUERDA;
    } else if (adc_valor < 790) {
        botaoLido = btnSELECIONAR;
    }

    if (botaoLido != botaoAtual) {
        lastDebounceTime = millis();
        botaoAtual = botaoLido;
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        return botaoAtual;
    }

    return btnNENHUM;
}

void setup() {
    lcd.begin(16, 2);
    Serial.begin(9600);
    lcd.print("PA 2024");
    delay(2000);
    lcd.clear();
    lcd.print("Escolha sua opcao");
    delay(1000);
    mostrarMenu();
}

void loop() {
    int botao = lerBotoesLCD();
    if (botao != btnNENHUM) {
        if (botao == btnSELECIONAR) {
            executarOpcao();
            mostrarMenu();
        } else if (botao == btnESQUERDA) {
            opcaoSelecionada = (opcaoSelecionada + 1) % 2;
            mostrarMenu();
        }
    }
}

void mostrarMenu() {
    lcd.clear();
    lcd.print(opcaoSelecionada == 0 ? "> PC -> Arduino" : "  PC -> Arduino");
    lcd.setCursor(0, 1);
    lcd.print(opcaoSelecionada == 1 ? "> Arduino -> PC" : "  Arduino -> PC");
}

void executarOpcao() {
    lcd.clear();
    if (opcaoSelecionada == 0) {
        receberDoPC();
    } else {
        enviarParaPC();
    }
}

void receberDoPC() {
    lcd.print("Aguardando...");
    while (!Serial.available());
    String mensagem = Serial.readString();
    lcd.clear();
    lcd.print(mensagem);
    delay(5000);
    lcd.clear();
    mostrarMenu();
}

void enviarParaPC() {
    lcd.print("Escrevendo:");
    String mensagem = "";
    char letras[] = {'a', 'e', 'i', 'o', 'u', ' '}; // Espaço adicionado
    int indiceLetra = 0;

    while (mensagem.length() < 8) {
        int botao = lerBotoesLCD();
        if (botao == btnESQUERDA) {
            indiceLetra = (indiceLetra + 1) % 6;
            lcd.setCursor(mensagem.length(), 1);
            lcd.print(letras[indiceLetra]);
            delay(200);  // Debounce
        } else if (botao == btnSELECIONAR) {
            mensagem += letras[indiceLetra];
            lcd.setCursor(mensagem.length(), 1);
            lcd.print(letras[indiceLetra]);
            delay(200);  // Debounce
        }
    }

    // Enviar a mensagem completa para o PC
    Serial.println(mensagem);
    lcd.clear();
    lcd.print("Msg enviada");
    delay(5000);
    lcd.clear();
    mostrarMenu();
}
