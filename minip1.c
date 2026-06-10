#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_EXPR_LEN 256
#define MAX_STACK 100

typedef struct {
    double items[MAX_STACK];
    int top;
} Stack;

void stack_init(Stack *s) {
    s->top = -1;
}

int stack_is_empty(Stack *s) {
    return s->top == -1;
}

int stack_is_full(Stack *s) {
    return s->top == MAX_STACK - 1;
}

void push(Stack *s, double value) {
    if (stack_is_full(s)) {
        printf("Error: Stack overflow!\n");
        return;
    }
    s->items[++s->top] = value;
}

double pop(Stack *s) {
    if (stack_is_empty(s)) {
        printf("Error: Stack underflow!\n");
        return 0;
    }
    return s->items[s->top--];
}

double peek(Stack *s) {
    if (stack_is_empty(s)) {
        printf("Error: Stack is empty!\n");
        return 0;
    }
    return s->items[s->top];
}

int is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

double perform_operation(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) {
                printf("Error: Division by zero!\n");
                return 0;
            }
            return a / b;
        case '^': return pow(a, b);
        default: return 0;
    }
}

double evaluate_postfix(char *expr) {
    Stack s;
    stack_init(&s);
    
    int i = 0;
    while (i < strlen(expr)) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }
        
        if (isdigit(expr[i]) || (expr[i] == '.' && i + 1 < strlen(expr) && isdigit(expr[i + 1]))) {
            double num = 0;
            int decimal_places = 0;
            int after_decimal = 0;
            
            while (i < strlen(expr) && (isdigit(expr[i]) || expr[i] == '.')) {
                if (expr[i] == '.') {
                    after_decimal = 1;
                    i++;
                    continue;
                }
                
                if (after_decimal) {
                    decimal_places++;
                    num = num * 10 + (expr[i] - '0');
                } else {
                    num = num * 10 + (expr[i] - '0');
                }
                i++;
            }
            
            if (after_decimal) {
                num /= pow(10, decimal_places);
            }
            
            push(&s, num);
        }
        else if (is_operator(expr[i])) {
            if (stack_is_empty(&s)) {
                printf("Error: Invalid expression (insufficient operands)!\n");
                return 0;
            }
            
            double b = pop(&s);
            
            if (stack_is_empty(&s)) {
                printf("Error: Invalid expression (insufficient operands)!\n");
                return 0;
            }
            
            double a = pop(&s);
            double result = perform_operation(a, b, expr[i]);
            push(&s, result);
            i++;
        }
        else {
            printf("Error: Invalid character '%c' in expression!\n", expr[i]);
            return 0;
        }
    }
    
    if (s.top != 0) {
        printf("Error: Invalid expression (too many operands)!\n");
        return 0;
    }
    
    return pop(&s);
}

void infix_to_postfix(char *infix, char *postfix) {
    Stack s;
    stack_init(&s);
    int j = 0;
    
    for (int i = 0; i < strlen(infix); i++) {
        char c = infix[i];
        
        if (isspace(c)) continue;
        
        if (isdigit(c) || c == '.') {
            postfix[j++] = c;
        }
        else if (c == '(') {
            push(&s, c);
        }
        else if (c == ')') {
            while (!stack_is_empty(&s) && peek(&s) != '(') {
                postfix[j++] = ' ';
                postfix[j++] = pop(&s);
            }
            if (!stack_is_empty(&s)) {
                pop(&s);
            }
        }
        else if (is_operator(c)) {
            postfix[j++] = ' ';
            while (!stack_is_empty(&s) && is_operator(peek(&s)) && 
                   precedence(peek(&s)) >= precedence(c)) {
                postfix[j++] = pop(&s);
                postfix[j++] = ' ';
            }
            push(&s, c);
        }
    }
    
    postfix[j++] = ' ';
    while (!stack_is_empty(&s)) {
        postfix[j++] = pop(&s);
        postfix[j++] = ' ';
    }
    postfix[j] = '\0';
}

void display_menu() {
    printf("\n========================================\n");
    printf("  CALCULATOR - Infix Expression Evaluator\n");
    printf("========================================\n");
    printf("1. Evaluate an expression\n");
    printf("2. View supported operations\n");
    printf("3. Clear history\n");
    printf("4. Exit\n");
    printf("----------------------------------------\n");
}

void show_operations() {
    printf("\nSupported Operations:\n");
    printf("  +  : Addition\n");
    printf("  -  : Subtraction\n");
    printf("  *  : Multiplication\n");
    printf("  /  : Division\n");
    printf("  ^  : Exponentiation\n");
    printf("  () : Parentheses for grouping\n");
    printf("\nExample: 5 * 3 + 2 = 17\n");
    printf("Example: (10 + 5) * 2 = 30\n");
    printf("Example: 2 ^ 3 = 8\n");
}

int main() {
    int choice;
    char infix[MAX_EXPR_LEN];
    char postfix[MAX_EXPR_LEN * 2];
    double result;
    
    printf("\nWelcome to the Expression Calculator!\n");
    
    do {
        display_menu();
        printf("Enter your choice (1-4): ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input! Please enter a number.\n");
            continue;
        }
        
        while (getchar() != '\n');
        
        switch (choice) {
            case 1:
                printf("\nEnter an expression (e.g., 5 + 3 * 2): ");
                if (fgets(infix, MAX_EXPR_LEN, stdin) != NULL) {
                    infix[strcspn(infix, "\n")] = 0;
                    
                    infix_to_postfix(infix, postfix);
                    printf("Postfix notation: %s\n", postfix);
                    
                    result = evaluate_postfix(postfix);
                    printf("Result: %.6g\n", result);
                }
                break;
            
            case 2:
                show_operations();
                break;
            
            case 3:
                printf("\nHistory cleared.\n");
                break;
            
            case 4:
                printf("\nThank you for using the calculator. Goodbye!\n");
                break;
            
            default:
                printf("Invalid choice! Please select 1-4.\n");
        }
        
    } while (choice != 4);
    
    return 0;
}
