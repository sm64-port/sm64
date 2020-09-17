#pragma once

struct Stack;
typedef enum { NOP = 1,
               QUIT = 2,
               GENERATE = 3,
               PLAY = 4 } AudioTask;

struct Stack *createStack(unsigned capacity);

int stack_isFull(struct Stack *stack);
int stack_isEmpty(struct Stack *stack);
void stack_push(struct Stack *stack, AudioTask item);
AudioTask stack_pop(struct Stack *stack);
AudioTask stack_peek(struct Stack *stack);
void stack_clear(struct Stack *stack);
