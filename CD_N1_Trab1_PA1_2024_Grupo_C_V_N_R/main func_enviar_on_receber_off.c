#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define SERIAL_PORT "COM9"  // Ajuste conforme necessário
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
char lerBotaoArduino(HANDLE hSerial);

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

char lerBotaoArduino(HANDLE hSerial) {
    char buffer[1];
    DWORD bytesRead;
    if (ReadFile(hSerial, buffer, 1, &bytesRead, NULL) && bytesRead == 1) {
        return buffer[0];
    }
    return '\0';
}

int main() {
    Sistema sistema;
    sistema.hSerial = configurarSerial(SERIAL_PORT);
    if (sistema.hSerial == INVALID_HANDLE_VALUE) return 1;

    sistema.estado = AGUARDANDO;
    char buffer[16];

    printf("Inicializando sistema...\n");
    Sleep(2000);

    while (1) {
        printf("Escolha sua opção no Arduino...\n");
        char comando = lerBotaoArduino(sistema.hSerial);

        if (comando == 'S') {
            if (sistema.estado == AGUARDANDO) {
                sistema.estado = PC_TO_ARDUINO;
                printf("PC -> Arduino selecionado.\nDigite sua mensagem (max 16 caracteres):\n");
                fgets(buffer, sizeof(buffer), stdin);
                enviarSerial(sistema.hSerial, buffer);
                printf("Mensagem enviada ao Arduino.\n");
            } else if (sistema.estado == PC_TO_ARDUINO) {
                sistema.estado = ARDUINO_TO_PC;
                printf("Arduino -> PC selecionado.\nAguardando mensagem do Arduino...\n");
                lerSerial(sistema.hSerial, buffer, sizeof(buffer));
                printf("Mensagem recebida: %s\n", buffer);
            }
            printf("Voltando ao menu principal...\n");
            Sleep(5000);
            sistema.estado = AGUARDANDO;
        } else if (comando == 'L') {
            // Navegação ainda não é necessária no PC, apenas no Arduino
            continue;
        }
        Sleep(100);
    }

    CloseHandle(sistema.hSerial);
    return 0;
}
