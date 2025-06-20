//expressao.c
#include "expressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h> // Para isspace, isdigit, isalpha

#define PI 3.14159265358979323846

// Estrutura de pilha para números
typedef struct {
    float *items;
    int top;
    int capacity;
} Stack;

// Funções auxiliares para a pilha
Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    // Capacidade inicial da pilha, ajustável
    stack->items = (float*)malloc(stack->capacity * sizeof(float));
    return stack;
}

int isStackEmpty(Stack *stack) {
    return stack->top == -1;
}

void push(Stack *stack, float item) {
    if (stack->top == stack->capacity - 1) {
        fprintf(stderr, "Erro: Pilha de números cheia. Aumente a capacidade.\n");
        exit(EXIT_FAILURE);
    }
    stack->items[++stack->top] = item;
}

float pop(Stack *stack) {
    if (isStackEmpty(stack)) {
        fprintf(stderr, "Erro: Pilha de números vazia ao tentar pop.\n");
        exit(EXIT_FAILURE);
    }
    return stack->items[stack->top--];
}

float peek(Stack *stack) {
    if (isStackEmpty(stack)) {
        fprintf(stderr, "Erro: Pilha de números vazia ao tentar peek.\n");
        exit(EXIT_FAILURE);
    }
    return stack->items[stack->top];
}

// Função para verificar se uma string é um número
int isNumber(const char *str) {
    char *endptr;
    if (str == NULL || *str == '\0') return 0;

    // Tenta converter a string para float
    strtod(str, &endptr);

    // Verifica se a conversão consumiu a string inteira e se não é apenas um sinal +/-
    // e que a string não é vazia ou composta apenas por espaços
    return endptr != str && *endptr == '\0' && !isspace((unsigned char)*str);
}

// Função principal para avaliar expressão pós-fixada
float getValorPosFixa(char *StrPosFixa) {
    // Fazer uma cópia de StrPosFixa porque strtok modifica a string.
    char *StrCopia = strdup(StrPosFixa);
    if (StrCopia == NULL) {
        fprintf(stderr, "Erro de alocação de memória para cópia da string.\n");
        exit(EXIT_FAILURE);
    }

    Stack *stack = createStack(512); // Capacidade da pilha
    char *token = strtok(StrCopia, " "); // Aqui ainda usamos strtok, mas a entrada já deve estar bem tokenizada
    
    while (token != NULL) {
        if (isNumber(token)) {
            push(stack, atof(token));
        } else {
            // Operações binárias
            if (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 || 
                strcmp(token, "*") == 0 || strcmp(token, "/") == 0 || 
                strcmp(token, "%") == 0 || strcmp(token, "^") == 0) {
                
                if (stack->top < 1) { // Precisa de pelo menos 2 operandos
                    fprintf(stderr, "Erro: Operação binária '%s' precisa de 2 operandos.\n", token);
                    free(StrCopia);
                    free(stack->items);
                    free(stack);
                    exit(EXIT_FAILURE);
                }
                
                float b = pop(stack);
                float a = pop(stack);
                float result;
                
                if (strcmp(token, "+") == 0) result = a + b;
                else if (strcmp(token, "-") == 0) result = a - b;
                else if (strcmp(token, "*") == 0) result = a * b;
                else if (strcmp(token, "/") == 0) {
                    if (b == 0) {
                        fprintf(stderr, "Erro: Divisão por zero.\n");
                        free(StrCopia);
                        free(stack->items);
                        free(stack);
                        exit(EXIT_FAILURE);
                    }
                    result = a / b;
                }
                else if (strcmp(token, "%") == 0) {
                    result = fmod(a, b);
                }
                else if (strcmp(token, "^") == 0) result = pow(a, b);
                
                push(stack, result);
            }
            // Operações unárias
            else if (strcmp(token, "raiz") == 0 || strcmp(token, "sen") == 0 || 
                    strcmp(token, "cos") == 0 || strcmp(token, "tg") == 0 || 
                    strcmp(token, "log") == 0) {
                
                if (isStackEmpty(stack)) { // Precisa de pelo menos 1 operando
                    fprintf(stderr, "Erro: Operação unária '%s' precisa de 1 operando.\n", token);
                    free(StrCopia);
                    free(stack->items);
                    free(stack);
                    exit(EXIT_FAILURE);
                }
                
                float a = pop(stack);
                float result;
                
                if (strcmp(token, "raiz") == 0) {
                    if (a < 0) {
                        fprintf(stderr, "Erro: Raiz quadrada de número negativo.\n");
                        free(StrCopia);
                        free(stack->items);
                        free(stack);
                        exit(EXIT_FAILURE);
                    }
                    result = sqrt(a);
                }
                else if (strcmp(token, "sen") == 0) result = sin(a * PI / 180.0); // converter para radianos
                else if (strcmp(token, "cos") == 0) result = cos(a * PI / 180.0); // converter para radianos
                else if (strcmp(token, "tg") == 0) {
                    // Prevenir tangentes de ângulos como 90, 270 graus, etc.
                    double angle_mod_180 = fmod(fabs(a), 180.0);
                    if (fabs(angle_mod_180 - 90.0) < 0.001 || fabs(angle_mod_180 - 270.0) < 0.001) {
                         fprintf(stderr, "Erro: Tangente de ângulo invalido (90, 270 graus, etc.).\n");
                         free(StrCopia);
                         free(stack->items);
                         free(stack);
                         exit(EXIT_FAILURE);
                    }
                    result = tan(a * PI / 180.0); // converter para radianos
                }
                else if (strcmp(token, "log") == 0) {
                    if (a <= 0) {
                        fprintf(stderr, "Erro: Logaritmo de número não positivo.\n");
                        free(StrCopia);
                        free(stack->items);
                        free(stack);
                        exit(EXIT_FAILURE);
                    }
                    result = log10(a);
                }
                
                push(stack, result);
            } else {
                fprintf(stderr, "Erro: Operador desconhecido ou expressão mal formada '%s'\n", token);
                free(StrCopia);
                free(stack->items);
                free(stack);
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " ");
    }
    
    if (stack->top != 0) {
        fprintf(stderr, "Erro: Expressão mal formada - operandos sobrando na pilha.\n");
        free(StrCopia);
        free(stack->items);
        free(stack);
        exit(EXIT_FAILURE);
    }
    
    float finalResult = pop(stack);
    free(StrCopia);
    free(stack->items);
    free(stack);
    return finalResult; 
}

// ===== Conversão de infixa para posfixa =====

typedef struct {
    char items[512][32]; // Ajustado para suportar tokens maiores
    int top;
} StackStr;

void pushStr(StackStr* s, const char* item) {
    if (s->top == 511) { // Capacidade máxima de 512, top vai até 511
        fprintf(stderr, "Erro: Pilha de strings cheia. Aumente a capacidade.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(s->items[++s->top], item);
}

char* popStr(StackStr* s) {
    if (s->top == -1) {
        fprintf(stderr, "Erro: Pilha de strings vazia ao tentar pop.\n");
        exit(EXIT_FAILURE);
    }
    return s->items[s->top--];
}

char* peekStr(StackStr* s) {
    if (s->top == -1) {
        fprintf(stderr, "Erro: Pilha de strings vazia ao tentar peek.\n");
        exit(EXIT_FAILURE);
    }
    return s->items[s->top];
}

int isEmptyStr(StackStr* s) {
    return s->top == -1;
}

// Prioridade dos operadores
int prioridade(const char* op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    if (strcmp(op, "raiz") == 0 || strcmp(op, "sen") == 0 || strcmp(op, "cos") == 0 ||
        strcmp(op, "tg") == 0 || strcmp(op, "log") == 0) return 4;
    return 0; // Para parênteses ou tokens desconhecidos, prioridade 0
}

// Verifica se é um operador binário
int ehOperador(const char* token) {
    return strcmp(token, "+") == 0 || strcmp(token, "-") == 0 ||
           strcmp(token, "*") == 0 || strcmp(token, "/") == 0 ||
           strcmp(token, "%") == 0 || strcmp(token, "^") == 0;
}

// Verifica se é uma função (operador unário)
int ehFuncao(const char* token) {
    return strcmp(token, "sen") == 0 || strcmp(token, "cos") == 0 ||
           strcmp(token, "tg") == 0 || strcmp(token, "log") == 0 ||
           strcmp(token, "raiz") == 0;
}

// Função para extrair o próximo token de uma string de expressão
// Lida com múltiplos caracteres, operadores de um caractere, funções e números
// sem exigir espaços entre eles.
char* getNextToken(char **expr_ptr, char *token_buffer) {
    char *expr = *expr_ptr;
    int i = 0;

    // Pular espaços em branco no início
    while (isspace((unsigned char)*expr)) {
        expr++;
    }

    if (*expr == '\0') {
        *expr_ptr = expr; // Sinaliza que a string terminou
        return NULL;
    }

    // Tentar identificar funções primeiro (multi-caracteres)
    // Assegura que a função seja a mais longa possível para evitar "log" em "logaritmo"
    if (strncmp(expr, "raiz", 4) == 0) {
        strcpy(token_buffer, "raiz");
        *expr_ptr = expr + 4;
        return token_buffer;
    }
    else if (strncmp(expr, "sen", 3) == 0) {
        strcpy(token_buffer, "sen");
        *expr_ptr = expr + 3;
        return token_buffer;
    }
    else if (strncmp(expr, "cos", 3) == 0) {
        strcpy(token_buffer, "cos");
        *expr_ptr = expr + 3;
        return token_buffer;
    }
    else if (strncmp(expr, "tg", 2) == 0) {
        strcpy(token_buffer, "tg");
        *expr_ptr = expr + 2;
        return token_buffer;
    }
    else if (strncmp(expr, "log", 3) == 0) {
        strcpy(token_buffer, "log");
        *expr_ptr = expr + 3;
        return token_buffer;
    }
    // Identificar operadores e parênteses de um caractere
    else if (strchr("+-*/%^()", *expr) != NULL) {
        token_buffer[0] = *expr;
        token_buffer[1] = '\0';
        *expr_ptr = expr + 1;
        return token_buffer;
    }
    // Identificar números (pode incluir ponto decimal e sinal negativo)
    // Permite número iniciar com '.' (ex: .5)
    // Permite número iniciar com '-' (ex: -5)
    else if (isdigit((unsigned char)*expr) || (*expr == '.' && isdigit((unsigned char)*(expr + 1))) ||
             (*expr == '-' && (isdigit((unsigned char)*(expr + 1)) || (*(expr + 1) == '.' && isdigit((unsigned char)*(expr + 2))))) ) {
        
        // Se começar com '-', inclua-o no número
        if (*expr == '-') {
            token_buffer[i++] = *expr;
            expr++;
        }

        // Copia a parte inteira e decimal do número
        while (isdigit((unsigned char)*expr) || *expr == '.') {
            token_buffer[i++] = *expr;
            expr++;
        }
        token_buffer[i] = '\0';
        *expr_ptr = expr; // Atualiza o ponteiro para a string para a próxima chamada
        return token_buffer;
    }
    else {
        // Captura o próximo caractere como um token desconhecido
        token_buffer[0] = *expr;
        token_buffer[1] = '\0';
        *expr_ptr = expr + 1;
        return token_buffer; // Retorna o caractere desconhecido para tratamento de erro
    }
}

// Função: Pre-processa a string para substituir vírgulas por pontos
void replaceCommasWithDots(char *str) {
    if (str == NULL) return;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ',') {
            str[i] = '.';
        }
    }
}

// Função para aplicar um operador aos operandos e empilhar o resultado
void applyOperator(Stack *operandStack, StackStr *operatorStack) {
    char *op = popStr(operatorStack);

    if (ehOperador(op)) { // Operadores binários
        if (isStackEmpty(operandStack) || operandStack->top < 1) {
            fprintf(stderr, "Erro: Poucos operandos para operador binário '%s'.\n", op);
            exit(EXIT_FAILURE);
        }
        float b = pop(operandStack);
        float a = pop(operandStack);
        float result;

        if (strcmp(op, "+") == 0) result = a + b;
        else if (strcmp(op, "-") == 0) result = a - b;
        else if (strcmp(op, "*") == 0) result = a * b;
        else if (strcmp(op, "/") == 0) {
            if (b == 0) {
                fprintf(stderr, "Erro: Divisão por zero.\n");
                exit(EXIT_FAILURE);
            }
            result = a / b;
        }
        else if (strcmp(op, "%") == 0) result = fmod(a, b);
        else if (strcmp(op, "^") == 0) result = pow(a, b);
        push(operandStack, result);
    }
    else if (ehFuncao(op)) { // Funções (operadores unários)
        if (isStackEmpty(operandStack)) {
            fprintf(stderr, "Erro: Poucos operandos para função '%s'.\n", op);
            exit(EXIT_FAILURE);
        }
        float a = pop(operandStack);
        float result;

        if (strcmp(op, "raiz") == 0) {
            if (a < 0) {
                fprintf(stderr, "Erro: Raiz quadrada de número negativo.\n");
                exit(EXIT_FAILURE);
            }
            result = sqrt(a);
        }
        else if (strcmp(op, "sen") == 0) result = sin(a * PI / 180.0);
        else if (strcmp(op, "cos") == 0) result = cos(a * PI / 180.0);
        else if (strcmp(op, "tg") == 0) {
            double angle_mod_180 = fmod(fabs(a), 180.0);
            if (fabs(angle_mod_180 - 90.0) < 0.001 || fabs(angle_mod_180 - 270.0) < 0.001) {
                 fprintf(stderr, "Erro: Tangente de ângulo invalido (90, 270 graus, etc.).\n");
                 exit(EXIT_FAILURE);
            }
            result = tan(a * PI / 180.0);
        }
        else if (strcmp(op, "log") == 0) {
            if (a <= 0) {
                fprintf(stderr, "Erro: Logaritmo de número não positivo.\n");
                exit(EXIT_FAILURE);
            }
            result = log10(a);
        }
        push(operandStack, result);
    }
    // Não precisamos de um else, pois o erro de token desconhecido já foi tratado no getNextToken
}


// Implementação da função getFormaPosFixa
char* getFormaPosFixa(char *Str) {
    static char saida[512]; // Buffer estático para a string de saída
    saida[0] = '\0'; // Limpa a string de saída

    StackStr pilha;
    pilha.top = -1;

    // Criar uma cópia da string de entrada e pré-processá-la
    char *expressao_copia = strdup(Str);
    if (expressao_copia == NULL) {
        fprintf(stderr, "Erro de alocação de memória para cópia da expressão.\n");
        saida[0] = '\0';
        return saida;
    }
    replaceCommasWithDots(expressao_copia); // Chama a função para substituir ',' por '.'

    char *expressao_ptr = expressao_copia; // Ponteiro que getNextToken irá avançar
    char token_buffer[32]; // Buffer para armazenar o token atual

    char* token;
    // O loop agora usa getNextToken para extrair os tokens diretamente da string original
    while ((token = getNextToken(&expressao_ptr, token_buffer)) != NULL) {
        if (strcmp(token, "") == 0) continue; // Pular tokens vazios se houver
        
        if (isNumber(token)) { // Usar a função isNumber para identificar operandos
            strcat(saida, token);
            strcat(saida, " ");
        }
        else if (ehFuncao(token)) { // É uma função
            pushStr(&pilha, token);
        }
        else if (strcmp(token, "(") == 0) { // É um parêntese de abertura
            pushStr(&pilha, token);
        }
        else if (strcmp(token, ")") == 0) { // É um parêntese de fechamento
            while (!isEmptyStr(&pilha) && strcmp(peekStr(&pilha), "(") != 0) {
                strcat(saida, popStr(&pilha));
                strcat(saida, " ");
            }
            if (!isEmptyStr(&pilha) && strcmp(peekStr(&pilha), "(") == 0) {
                popStr(&pilha); // Desempilha o '('
            } else {
                fprintf(stderr, "Erro: Parênteses desbalanceados na expressão infixa.\n");
                saida[0] = '\0'; // Limpa a saída em caso de erro
                free(expressao_copia);
                return saida;
            }
            // Se o topo da pilha é uma função, desempilha para a saída
            if (!isEmptyStr(&pilha) && ehFuncao(peekStr(&pilha))) {
                strcat(saida, popStr(&pilha));
                strcat(saida, " ");
            }
        }
        else if (ehOperador(token)) { // É um operador
            while (!isEmptyStr(&pilha) &&
                   (ehFuncao(peekStr(&pilha)) || ehOperador(peekStr(&pilha))) && // Compara com funções ou operadores na pilha
                   (prioridade(peekStr(&pilha)) > prioridade(token) ||
                    (prioridade(peekStr(&pilha)) == prioridade(token) && strcmp(token, "^") != 0))) { // Associatividade à esquerda para a maioria
                // Associatividade da direita para a esquerda para o operador de potência '^'
                // Se o operador no topo da pilha é '^' e o token atual também é '^', não desempilha (associa à direita)
                // Se não for '^', desempilha se prioridade igual ou maior (associa à esquerda)
                strcat(saida, popStr(&pilha));
                strcat(saida, " ");
            }
            pushStr(&pilha, token);
        }
        else {
            // Token desconhecido
            fprintf(stderr, "Erro: Token desconhecido na expressão infixa: '%s'\n", token);
            saida[0] = '\0'; // Limpa a saída em caso de erro
            free(expressao_copia);
            return saida;
        }
    }

    // Desempilhar quaisquer operadores restantes
    while (!isEmptyStr(&pilha)) {
        if (strcmp(peekStr(&pilha), "(") == 0) {
            fprintf(stderr, "Erro: Parênteses desbalanceados na expressão infixa ( '(' sem ')').\n");
            saida[0] = '\0'; // Limpa a saída em caso de erro
            free(expressao_copia);
            return saida;
        }
        strcat(saida, popStr(&pilha));
        strcat(saida, " ");
    }

    // Remover o último espaço se houver
    if (strlen(saida) > 0 && saida[strlen(saida) - 1] == ' ') {
        saida[strlen(saida) - 1] = '\0';
    }

    free(expressao_copia); // Libera a cópia da string
    return saida;
}

// Versão provisória para getFormaInFixa - precisa de implementação real
char* getFormaInFixa(char* Str) {
    // Esta é uma implementação dummy. Você precisa implementar a lógica real
    // para converter uma expressão pós-fixa para infixa.
    // Isso geralmente envolve o uso de uma pilha para construir a expressão infixa
    // adicionando parênteses conforme necessário para manter a precedência.
    static char dummy[512];
    snprintf(dummy, sizeof(dummy), "Ainda nao implementado para '%s'", Str);
    return dummy;
}

// Calcula o valor de Str (na forma infixa)
float getValorInFixa(char *StrInFixa) {
    // Fazer uma cópia da string de entrada e pré-processá-la (vírgulas para pontos)
    char *expressao_copia = strdup(StrInFixa);
    if (expressao_copia == NULL) {
        fprintf(stderr, "Erro de alocação de memória para cópia da expressão.\n");
        return 0.0;
    }
    replaceCommasWithDots(expressao_copia);

    Stack *operandStack = createStack(512); // Pilha para números
    StackStr operatorStack; // Pilha para operadores (char arrays)
    operatorStack.top = -1;

    char *expr_ptr = expressao_copia;
    char token_buffer[32];
    char *token;

    while ((token = getNextToken(&expr_ptr, token_buffer)) != NULL) {
        if (strcmp(token, "") == 0) continue; // Pular tokens vazios

        if (isNumber(token)) {
            push(operandStack, atof(token));
        } else if (ehFuncao(token)) {
            pushStr(&operatorStack, token);
        } else if (strcmp(token, "(") == 0) {
            pushStr(&operatorStack, token);
        } else if (strcmp(token, ")") == 0) {
            while (!isEmptyStr(&operatorStack) && strcmp(peekStr(&operatorStack), "(") != 0) {
                applyOperator(operandStack, &operatorStack);
            }
            if (!isEmptyStr(&operatorStack) && strcmp(peekStr(&operatorStack), "(") == 0) {
                popStr(&operatorStack); // Desempilha o '('
            } else {
                fprintf(stderr, "Erro: Parênteses desbalanceados na expressão infixa.\n");
                free(expressao_copia);
                free(operandStack->items);
                free(operandStack);
                exit(EXIT_FAILURE);
            }
            // Após desempilhar '(', se houver uma função no topo, aplica-a
            if (!isEmptyStr(&operatorStack) && ehFuncao(peekStr(&operatorStack))) {
                applyOperator(operandStack, &operatorStack);
            }
        } else if (ehOperador(token)) {
            while (!isEmptyStr(&operatorStack) &&
                   (ehFuncao(peekStr(&operatorStack)) || ehOperador(peekStr(&operatorStack))) &&
                   (prioridade(peekStr(&operatorStack)) > prioridade(token) ||
                    (prioridade(peekStr(&operatorStack)) == prioridade(token) && strcmp(token, "^") != 0))) { // Associatividade para ^
                applyOperator(operandStack, &operatorStack);
            }
            pushStr(&operatorStack, token);
        } else {
            fprintf(stderr, "Erro: Token desconhecido na expressão infixa: '%s'.\n", token);
            free(expressao_copia);
            free(operandStack->items);
            free(operandStack);
            exit(EXIT_FAILURE);
        }
    }

    // Processar quaisquer operadores restantes na pilha de operadores
    while (!isEmptyStr(&operatorStack)) {
        if (strcmp(peekStr(&operatorStack), "(") == 0) {
            fprintf(stderr, "Erro: Parênteses desbalanceados na expressão infixa ( '(' sem ')').\n");
            free(expressao_copia);
            free(operandStack->items);
            free(operandStack);
            exit(EXIT_FAILURE);
        }
        applyOperator(operandStack, &operatorStack);
    }

    if (operandStack->top != 0) {
        fprintf(stderr, "Erro: Expressão mal formada - mais de um operando restante na pilha.\n");
        free(expressao_copia);
        free(operandStack->items);
        free(operandStack);
        exit(EXIT_FAILURE);
    }

    float finalResult = pop(operandStack);
    free(expressao_copia);
    free(operandStack->items);
    free(operandStack);
    return finalResult;
}