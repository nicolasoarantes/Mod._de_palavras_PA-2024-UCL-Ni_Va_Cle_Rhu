#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

#define BTN_SELECT 4
#define BTN_LEFT 3
#define BTN_NONE 5

int opcaoSelecionada = 0;
int botaoAtual = BTN_NONE;

HANDLE configurarPortaSerial(const char* portaSerial) {
    HANDLE hSerial = CreateFile(portaSerial, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf("Porta serial não existe.\n");
        } else {
            printf("Erro ao abrir a porta serial.\n");
        }
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Erro ao obter o estado da porta serial.\n");
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Erro ao configurar a porta serial.\n");
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("Erro ao configurar os timeouts da porta serial.\n");
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    return hSerial;
}

int lerBotoesLCD() {
    if (_kbhit()) {
        char ch = _getch();
        if (ch == 's') return BTN_SELECT;
        if (ch == 'l') return BTN_LEFT;
    }
    return BTN_NONE;
}

void mostrarMenu() {
    printf("\nPA1 - 2024\nEscolha sua opcao de input no arduino!\n");
}

void executarOpcao(HANDLE hSerial) {
    if (opcaoSelecionada == 0) {
        printf("Digite sua mensagem: ");
        char mensagem[256];
        fgets(mensagem, sizeof(mensagem), stdin);
        DWORD bytesWritten;
        if (WriteFile(hSerial, mensagem, strlen(mensagem), &bytesWritten, NULL)) {
            printf("Mensagem enviada: %s\n", mensagem);
        } else {
            printf("Erro ao enviar a mensagem para o Arduino.\n");
        }
    } else {
        printf("Aguardando mensagem do Arduino...\n");
        char mensagem[256];
        DWORD bytesRead;
        if (ReadFile(hSerial, mensagem, sizeof(mensagem) - 1, &bytesRead, NULL)) {
            mensagem[bytesRead] = '\0';
            printf("Mensagem recebida: %s\n", mensagem);
        } else {
            printf("Erro ao ler a mensagem do Arduino.\n");
        }
    }
    mostrarMenu();
}

int main() {
    HANDLE hSerial = configurarPortaSerial("COM9");
    if (hSerial == INVALID_HANDLE_VALUE) {
        return -1;
    }

    printf("PA1 - 2024\n");
    Sleep(2000);
    system("cls");

    mostrarMenu();

    while (1) {
        botaoAtual = lerBotoesLCD();
        if (botaoAtual == BTN_SELECT) {
            executarOpcao(hSerial);
        } else if (botaoAtual == BTN_LEFT) {
            opcaoSelecionada = (opcaoSelecionada + 1) % 2;
            mostrarMenu();
        }

        // Checar por mensagens do Arduino constantemente
        if (opcaoSelecionada == 1) {
            char mensagem[256];
            DWORD bytesRead;
            if (ReadFile(hSerial, mensagem, sizeof(mensagem) - 1, &bytesRead, NULL)) {
                if (bytesRead > 0) {
                    mensagem[bytesRead] = '\0';
                    printf("Mensagem recebida do Arduino: %s\n", mensagem);
                }
            }
        }

        Sleep(100);
    }

    CloseHandle(hSerial);
    return 0;
}
