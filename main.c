//main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calculadora.h"

// Função auxiliar para exibir o menu
void mostrarMenu() {
    printf("===== Avaliador de Expressoes Numericas =====\n");
    printf("1. Converter Infixa para Posfixa\n");
    printf("2. Converter Posfixa para Infixa\n");
    printf("3. Calcular valor da expressao Infixa\n");
    printf("4. Calcular valor da expressao Posfixa\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
}

int main() {
    int opcao;
    char entrada[512];
    Expressao expr;

    do {
        mostrarMenu();
        scanf("%d", &opcao);
        getchar();

        switch(opcao) {
            case 1:
                printf("Digite a expressao infixa:\n");
                fgets(entrada, sizeof(entrada), stdin);
                entrada[strcspn(entrada, "\n")] = '\0';

                strcpy(expr.inFixa, entrada);
                strcpy(expr.posFixa, getFormaPosFixa(entrada));
                printf("Forma posfixa: %s\n\n", expr.posFixa);
                break;

            case 2:
                printf("Digite a expressao posfixa:\n");
                fgets(entrada, sizeof(entrada), stdin);
                entrada[strcspn(entrada, "\n")] = '\0';

                strcpy(expr.posFixa, entrada);
                strcpy(expr.inFixa, getFormaInFixa(entrada));
                printf("Forma infixa: %s\n\n", expr.inFixa);
                break;

            case 3:
                printf("Digite a expressao infixa:\n");
                fgets(entrada, sizeof(entrada), stdin);
                entrada[strcspn(entrada, "\n")] = '\0';

                strcpy(expr.inFixa, entrada);
                expr.Valor = getValorInFixa(entrada);
                printf("Valor da expressao: %.2f\n\n", expr.Valor);
                break;

            case 4:
                printf("Digite a expressao posfixa:\n");
                fgets(entrada, sizeof(entrada), stdin);
                entrada[strcspn(entrada, "\n")] = '\0';

                strcpy(expr.posFixa, entrada);
                expr.Valor = getValorPosFixa(entrada);
                printf("Valor da expressao: %.2f\n\n", expr.Valor);
                break;

            case 0:
                printf("Encerrando o programa...\n");
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n\n");
        }

    } while(opcao != 0);

    return 0;
}
