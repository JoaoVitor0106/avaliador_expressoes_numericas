// expressao.c
#include "expressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define PI 3.14159265358979323846

// Estruturas e Funções para Pilha de Números (usada na avaliação)

typedef struct {
    float *items;
    int top;
    int capacity;
} Stack;

Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (stack == NULL) {
        fprintf(stderr, "Erro de alocação de memória para a pilha.\n");
        exit(EXIT_FAILURE);
    }
    stack->capacity = capacity;
    stack->top = -1;
    stack->items = (float*)malloc(stack->capacity * sizeof(float));
    if (stack->items == NULL) {
        fprintf(stderr, "Erro de alocação de memória para os itens da pilha.\n");
        free(stack);
        exit(EXIT_FAILURE);
    }
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

// Funções Auxiliares Comuns

int isNumber(const char *str) {
    char *endptr;
    if (str == NULL || *str == '\0') return 0;
    strtod(str, &endptr);
    return endptr != str && *endptr == '\0' && !isspace((unsigned char)*str);
}

void replaceCommasWithDots(char *str) {
    if (str == NULL) return;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ',') {
            str[i] = '.';
        }
    }
}

// Implementação de getValorPosFixa (Avaliação de Expressões Pós-fixadas)

float getValorPosFixa(char *StrPosFixa) {
    char *StrCopia = strdup(StrPosFixa);
    if (StrCopia == NULL) {
        fprintf(stderr, "Erro de alocação de memória para cópia da string de entrada.\n");
        exit(EXIT_FAILURE);
    }

    Stack *stack = createStack(512);
    char *token = strtok(StrCopia, " ");
    
    while (token != NULL) {
        if (isNumber(token)) {
            push(stack, atof(token));
        } else {
            if (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 || 
                strcmp(token, "*") == 0 || strcmp(token, "/") == 0 || 
                strcmp(token, "%") == 0 || strcmp(token, "^") == 0) {
                
                if (stack->top < 1) {
                    fprintf(stderr, "Erro: Operação binária '%s' precisa de 2 operandos. Expressão mal formada.\n", token);
                    free(StrCopia); free(stack->items); free(stack);
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
                        free(StrCopia); free(stack->items); free(stack);
                        exit(EXIT_FAILURE);
                    }
                    result = a / b;
                }
                else if (strcmp(token, "%") == 0) result = fmod(a, b);
                else if (strcmp(token, "^") == 0) result = pow(a, b);
                
                push(stack, result);
            }
            else if (strcmp(token, "raiz") == 0 || strcmp(token, "sen") == 0 || 
                     strcmp(token, "cos") == 0 || strcmp(token, "tg") == 0 || 
                     strcmp(token, "log") == 0) {
                
                if (isStackEmpty(stack)) {
                    fprintf(stderr, "Erro: Operação unária '%s' precisa de 1 operando. Expressão mal formada.\n", token);
                    free(StrCopia); free(stack->items); free(stack);
                    exit(EXIT_FAILURE);
                }
                
                float a = pop(stack);
                float result;
                
                if (strcmp(token, "raiz") == 0) {
                    if (a < 0) {
                        fprintf(stderr, "Erro: Raiz quadrada de número negativo.\n");
                        free(StrCopia); free(stack->items); free(stack);
                        exit(EXIT_FAILURE);
                    }
                    result = sqrt(a);
                }
                else if (strcmp(token, "sen") == 0) result = sin(a * PI / 180.0);
                else if (strcmp(token, "cos") == 0) result = cos(a * PI / 180.0);
                else if (strcmp(token, "tg") == 0) {
                    double angle_rad = a * PI / 180.0;
                    if (fabs(fmod(angle_rad, PI) - PI/2) < 1e-9 || fabs(fmod(angle_rad, PI) + PI/2) < 1e-9) {
                         fprintf(stderr, "Erro: Tangente de ângulo inválido (90, 270 graus, etc. onde é indefinida).\n");
                         free(StrCopia); free(stack->items); free(stack);
                         exit(EXIT_FAILURE);
                    }
                    result = tan(angle_rad);
                }
                else if (strcmp(token, "log") == 0) {
                    if (a <= 0) {
                        fprintf(stderr, "Erro: Logaritmo de número não positivo.\n");
                        free(StrCopia); free(stack->items); free(stack);
                        exit(EXIT_FAILURE);
                    }
                    result = log10(a);
                }
                
                push(stack, result);
            } else {
                fprintf(stderr, "Erro: Operador ou token desconhecido '%s'. Expressão mal formada.\n", token);
                free(StrCopia); free(stack->items); free(stack);
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " ");
    }
    
    if (stack->top != 0) {
        fprintf(stderr, "Erro: Expressão mal formada - operandos sobrando ou faltando na pilha.\n");
        free(StrCopia); free(stack->items); free(stack);
        exit(EXIT_FAILURE);
    }
    
    float finalResult = pop(stack);
    free(StrCopia); free(stack->items); free(stack);
    return finalResult; 
}

// Estruturas e Funções para Pilha de Strings (usada na conversão de notações)

typedef struct {
    char items[512][32];
    int top;
} StackStr;

void pushStr(StackStr* s, const char* item) {
    if (s->top == 511) {
        fprintf(stderr, "Erro: Pilha de strings cheia. Aumente a capacidade ou reduza a expressão.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(s->items[++s->top], item);
}

char* popStr(StackStr* s) {
    if (s->top == -1) {
        fprintf(stderr, "Erro: Pilha de strings vazia ao tentar popStr.\n");
        exit(EXIT_FAILURE);
    }
    return s->items[s->top--];
}

char* peekStr(StackStr* s) {
    if (s->top == -1) {
        fprintf(stderr, "Erro: Pilha de strings vazia ao tentar peekStr.\n");
        exit(EXIT_FAILURE);
    }
    return s->items[s->top];
}

int isEmptyStr(StackStr* s) {
    return s->top == -1;
}

// Funções Auxiliares para Conversão Infixa para Pós-fixa

int prioridade(const char* op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    if (ehFuncao(op)) return 4;
    return 0;
}

int ehOperador(const char* token) {
    return strcmp(token, "+") == 0 || strcmp(token, "-") == 0 ||
           strcmp(token, "*") == 0 || strcmp(token, "/") == 0 ||
           strcmp(token, "%") == 0 || strcmp(token, "^") == 0;
}

int ehFuncao(const char* token) {
    return strcmp(token, "sen") == 0 || strcmp(token, "cos") == 0 ||
           strcmp(token, "tg") == 0 || strcmp(token, "log") == 0 ||
           strcmp(token, "raiz") == 0;
}

char* getNextToken(char **expr_ptr, char *token_buffer) {
    char *expr = *expr_ptr;
    int i = 0;

    while (isspace((unsigned char)*expr)) {
        expr++;
    }

    if (*expr == '\0') {
        *expr_ptr = expr;
        return NULL;
    }

    if (strncmp(expr, "raiz", 4) == 0 && !isalpha((unsigned char)expr[4]) && !isdigit((unsigned char)expr[4])) {
        strcpy(token_buffer, "raiz");
        *expr_ptr = expr + 4;
        return token_buffer;
    }
    else if (strncmp(expr, "sen", 3) == 0 && !isalpha((unsigned char)expr[3]) && !isdigit((unsigned char)expr[3])) {
        strcpy(token_buffer, "sen");
        *expr_ptr = expr + 3;
        return token_buffer;
    }
    else if (strncmp(expr, "cos", 3) == 0 && !isalpha((unsigned char)expr[3]) && !isdigit((unsigned char)expr[3])) {
        strcpy(token_buffer, "cos");
        *expr_ptr = expr + 3;
        return token_buffer;
    }
    else if (strncmp(expr, "tg", 2) == 0 && !isalpha((unsigned char)expr[2]) && !isdigit((unsigned char)expr[2])) {
        strcpy(token_buffer, "tg");
        *expr_ptr = expr + 2;
        return token_buffer;
    }
    else if (strncmp(expr, "log", 3) == 0 && !isalpha((unsigned char)expr[3]) && !isdigit((unsigned char)expr[3])) {
        strcpy(token_buffer, "log");
        *expr_ptr = expr + 3;
        return token_buffer;
    }
    else if (strchr("+-*/%^()", *expr) != NULL) {
        token_buffer[0] = *expr;
        token_buffer[1] = '\0';
        *expr_ptr = expr + 1;
        return token_buffer;
    }
    else if (isdigit((unsigned char)*expr) || 
             (*expr == '.' && isdigit((unsigned char)*(expr + 1))) ||
             (*expr == '-' && (isdigit((unsigned char)*(expr + 1)) || (*(expr + 1) == '.' && isdigit((unsigned char)*(expr + 2))))) ) {
        
        if (*expr == '-') {
            token_buffer[i++] = *expr;
            expr++;
        }

        while (isdigit((unsigned char)*expr)) {
            token_buffer[i++] = *expr;
            expr++;
        }
        if (*expr == '.') {
            token_buffer[i++] = *expr;
            expr++;
            while (isdigit((unsigned char)*expr)) {
                token_buffer[i++] = *expr;
                expr++;
            }
        }
        token_buffer[i] = '\0';
        *expr_ptr = expr;
        return token_buffer;
    }
    else {
        token_buffer[0] = *expr;
        token_buffer[1] = '\0';
        *expr_ptr = expr + 1;
        return token_buffer;
    }
}

// Implementação de getFormaPosFixa (Conversão Infixa para Pós-fixa)

char* getFormaPosFixa(char *StrInfixa) {
    static char saida[512];
    saida[0] = '\0';

    StackStr pilha;
    pilha.top = -1;

    char *expressao_copia = strdup(StrInfixa);
    if (expressao_copia == NULL) {
        fprintf(stderr, "Erro de alocação de memória para cópia da expressão.\n");
        saida[0] = '\0';
        return saida;
    }
    replaceCommasWithDots(expressao_copia);

    char *expressao_ptr = expressao_copia;
    char token_buffer[32];

    char* token;
    while ((token = getNextToken(&expressao_ptr, token_buffer)) != NULL) {
        if (strcmp(token, "") == 0) continue;
        
        if (isNumber(token)) {
            strcat(saida, token);
            strcat(saida, " ");
        }
        else if (ehFuncao(token)) {
            pushStr(&pilha, token);
        }
        else if (strcmp(token, "(") == 0) {
            pushStr(&pilha, token);
        }
        else if (strcmp(token, ")") == 0) {
            while (!isEmptyStr(&pilha) && strcmp(peekStr(&pilha), "(") != 0) {
                strcat(saida, popStr(&pilha));
                strcat(saida, " ");
            }
            if (!isEmptyStr(&pilha) && strcmp(peekStr(&pilha), "(") == 0) {
                popStr(&pilha);
            } else {
                fprintf(stderr, "Erro: Parênteses desbalanceados na expressão infixa.\n");
                saida[0] = '\0';
                free(expressao_copia);
                return saida;
            }
            if (!isEmptyStr(&pilha) && ehFuncao(peekStr(&pilha))) {
                strcat(saida, popStr(&pilha));
                strcat(saida, " ");
            }
        }
        else if (ehOperador(token)) {
            while (!isEmptyStr(&pilha) &&
                   (ehFuncao(peekStr(&pilha)) || ehOperador(peekStr(&pilha))) &&
                   (prioridade(peekStr(&pilha)) > prioridade(token) ||
                    (prioridade(peekStr(&pilha)) == prioridade(token) && strcmp(token, "^") != 0))) {
                
                strcat(saida, popStr(&pilha));
                strcat(saida, " ");
            }
            pushStr(&pilha, token);
        }
        else {
            fprintf(stderr, "Erro: Token desconhecido na expressão infixa: '%s'\n", token);
            saida[0] = '\0';
            free(expressao_copia);
            return saida;
        }
    }

    while (!isEmptyStr(&pilha)) {
        if (strcmp(peekStr(&pilha), "(") == 0) {
            fprintf(stderr, "Erro: Parênteses desbalanceados na expressão infixa.\n");
            saida[0] = '\0';
            free(expressao_copia);
            return saida;
        }
        strcat(saida, popStr(&pilha));
        strcat(saida, " ");
    }

    if (strlen(saida) > 0 && saida[strlen(saida) - 1] == ' ') {
        saida[strlen(saida) - 1] = '\0';
    }

    free(expressao_copia);
    return saida;
}

// Estrutura para os elementos da pilha na conversão pós-fixa para infixa
typedef struct {
    char str[256];
    int precedence;
    int is_operand_or_simple_group;
} InfixElement;

// Pilha para elementos infix
typedef struct {
    InfixElement items[512];
    int top;
} StackInfix;

void pushInfix(StackInfix* s, const char* str, int precedence, int is_operand_or_simple_group) {
    if (s->top == 511) {
        fprintf(stderr, "Erro: Pilha de elementos infix cheia.\n");
        exit(EXIT_FAILURE);
    }
    s->top++;
    strncpy(s->items[s->top].str, str, sizeof(s->items[s->top].str) - 1);
    s->items[s->top].str[sizeof(s->items[s->top].str) - 1] = '\0';
    s->items[s->top].precedence = precedence;
    s->items[s->top].is_operand_or_simple_group = is_operand_or_simple_group;
}

InfixElement popInfix(StackInfix* s) {
    if (s->top == -1) {
        fprintf(stderr, "Erro: Pilha de elementos infix vazia.\n");
        exit(EXIT_FAILURE);
    }
    return s->items[s->top--];
}

int isEmptyInfix(StackInfix* s) {
    return s->top == -1;
}

// Retorna a prioridade de um operador para a conversão pós-fixa para infixa
int getInfixPrecedence(const char* op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    if (ehFuncao(op)) return 4;
    return 0;
}

// Implementação de getFormaInFixa (Conversão Pós-fixa para Infixa)
char* getFormaInFixa(char* StrPosFixa) {
    static char final_infix_str[512];
    final_infix_str[0] = '\0';

    char *StrCopia = strdup(StrPosFixa);
    if (StrCopia == NULL) {
        fprintf(stderr, "Erro de alocação de memória para cópia da string.\n");
        final_infix_str[0] = '\0';
        return final_infix_str;
    }

    StackInfix s_infix;
    s_infix.top = -1;

    char *token = strtok(StrCopia, " ");
    
    while (token != NULL) {
        if (isNumber(token)) {
            pushInfix(&s_infix, token, 0, 1);
        } else if (ehOperador(token)) {
            if (s_infix.top < 1) {
                fprintf(stderr, "Erro: Expressão pós-fixa mal formada para operador '%s' (operandos insuficientes).\n", token);
                free(StrCopia);
                final_infix_str[0] = '\0';
                return final_infix_str;
            }
            InfixElement op2 = popInfix(&s_infix);
            InfixElement op1 = popInfix(&s_infix);

            char temp_expr[512];
            int op_precedence = getInfixPrecedence(token);

            int need_paren_op1 = (op_precedence > op1.precedence) || 
                                 (op_precedence == op1.precedence && strcmp(token, "^") == 0);

            int need_paren_op2 = (op_precedence > op2.precedence) || 
                                 (op_precedence == op2.precedence && strcmp(token, "^") != 0) ||
                                 (op_precedence == op2.precedence && op2.is_operand_or_simple_group == 0);
            
            if ((strcmp(token, "-") == 0 || strcmp(token, "/") == 0 || strcmp(token, "%") == 0) &&
                op_precedence == op2.precedence) {
                need_paren_op2 = 1;
            }
            
            snprintf(temp_expr, sizeof(temp_expr), "%s%s%s %s %s%s%s",
                     need_paren_op1 ? "(" : "", op1.str, need_paren_op1 ? ")" : "",
                     token,
                     need_paren_op2 ? "(" : "", op2.str, need_paren_op2 ? ")" : "");

            pushInfix(&s_infix, temp_expr, op_precedence, 0);
        } else if (ehFuncao(token)) {
            if (isEmptyInfix(&s_infix)) {
                 fprintf(stderr, "Erro: Expressão pós-fixa mal formada para função '%s' (operando insuficiente).\n", token);
                free(StrCopia);
                final_infix_str[0] = '\0';
                return final_infix_str;
            }
            InfixElement op1 = popInfix(&s_infix);
            char temp_expr[256];
            snprintf(temp_expr, sizeof(temp_expr), "%s(%s)", token, op1.str);
            pushInfix(&s_infix, temp_expr, getInfixPrecedence(token), 0);
        } else {
            fprintf(stderr, "Erro: Token desconhecido ou expressão mal formada em pós-fixa: '%s'\n", token);
            free(StrCopia);
            final_infix_str[0] = '\0';
            return final_infix_str;
        }
        token = strtok(NULL, " ");
    }

    if (s_infix.top != 0) {
        fprintf(stderr, "Erro: Expressão pós-fixa mal formada - elementos sobrando na pilha.\n");
        free(StrCopia);
        final_infix_str[0] = '\0';
        return final_infix_str;
    }

    strcpy(final_infix_str, s_infix.items[s_infix.top].str);
    free(StrCopia);
    return final_infix_str;
}

// Implementação de getValorInFixa (Avaliação de Expressões Infixas)

float getValorInFixa(char *StrInFixa) {
    char copiaInfixa[512];
    strncpy(copiaInfixa, StrInFixa, sizeof(copiaInfixa) - 1);
    copiaInfixa[sizeof(copiaInfixa) - 1] = '\0';

    char* posFixaStr = getFormaPosFixa(copiaInfixa); 
    
    if (posFixaStr != NULL && strlen(posFixaStr) > 0) {
        char copiaPosFixa[512];
        if (strlen(posFixaStr) < sizeof(copiaPosFixa)) {
            strcpy(copiaPosFixa, posFixaStr);
            return getValorPosFixa(copiaPosFixa);
        } else {
            fprintf(stderr, "Erro: String pós-fixada muito longa para cópia interna.\n");
            return NAN;
        }
    } else {
        fprintf(stderr, "Erro na conversão de infixa para pós-fixa, não foi possível avaliar.\n");
        return NAN;
    }
}
