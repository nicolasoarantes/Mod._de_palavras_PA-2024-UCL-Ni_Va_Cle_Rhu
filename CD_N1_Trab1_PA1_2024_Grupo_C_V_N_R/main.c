#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define SERIAL_PORT "COM9"
#define BAUD_RATE CBR_9600

typedef enum {
    AGUARDANDO, PC_TO_ARDUINO, ARDUINO_TO_PC
} EstadoMenu;

typedef struct {
    HANDLE hSerial;
    EstadoMenu estado;
} Sistema;

HANDLE configurarSerial(const char *porta);
void enviarSerial(HANDLE hSerial, const char *mensagem);
void lerSerial(HANDLE hSerial, char *buffer, int tamanho);
void receberDados(HANDLE hSerial, char *buffer, int length);

HANDLE configurarSerial(const char *porta) {
    HANDLE hSerial = CreateFile(porta, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Erro ao abrir porta serial\n");
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        fprintf(stderr, "Erro ao obter estado da porta serial\n");
        return INVALID_HANDLE_VALUE;
    }

    dcbSerialParams.BaudRate = BAUD_RATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        fprintf(stderr, "Erro ao definir estado da porta serial\n");
        return INVALID_HANDLE_VALUE;
    }

    return hSerial;
}

void enviarSerial(HANDLE hSerial, const char *mensagem) {
    DWORD bytesWritten;
    WriteFile(hSerial, mensagem, strlen(mensagem), &bytesWritten, NULL);
}

void lerSerial(HANDLE hSerial, char *buffer, int tamanho) {
    DWORD bytesRead;
    ReadFile(hSerial, buffer, tamanho, &bytesRead, NULL);
    buffer[bytesRead] = '\0';
}

void receberDados(HANDLE hSerial, char *buffer, int length) {
    DWORD bytes_read;
    if (!ReadFile(hSerial, buffer, length, &bytes_read, NULL)) {
        printf("Erro ao ler a partir da porta serial\n");
    }
    buffer[bytes_read] = '\0';
}

void limparBufferSerial(HANDLE hSerial) {
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

int main() {
    Sistema sistema;
    sistema.hSerial = configurarSerial(SERIAL_PORT);
    if (sistema.hSerial == INVALID_HANDLE_VALUE) return 1;

    sistema.estado = AGUARDANDO;
    char buffer[256];

    while (1) {
        printf("Escolha sua opcao:\n");
        printf("1 - PC -> Arduino\n");
        printf("2 - Arduino -> PC\n");

        int escolha;
        scanf("%d", &escolha);
        getchar(); // Consumir o newline deixado pelo scanf

        if (escolha == 1) {
            sistema.estado = PC_TO_ARDUINO;
        } else if (escolha == 2) {
            sistema.estado = ARDUINO_TO_PC;
        } else {
            printf("Opcao invalida!\n");
            continue;
        }

        if (sistema.estado == PC_TO_ARDUINO) {
            printf("Digite sua mensagem (max 16 caracteres):\n");
            fgets(buffer, sizeof(buffer), stdin);
            // Remove newline character
            buffer[strcspn(buffer, "\n")] = 0;
            enviarSerial(sistema.hSerial, buffer);
            printf("Mensagem enviada ao Arduino.\n");
        } else if (sistema.estado == ARDUINO_TO_PC) {
            printf("Aguardando mensagem do Arduino...\n");
            limparBufferSerial(sistema.hSerial);
            receberDados(sistema.hSerial, buffer, sizeof(buffer) - 1);
            printf("Mensagem recebida: %s\n", buffer);
        }
    }
//Tratar trazendo apenas o prox. 
    CloseHandle(sistema.hSerial);
    return 0;
}
