#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

struct Stack {
  uint32_t top;      /* Index that indicates the stack's head in items. */
  uint32_t capacity; /* Total amount of nodes the stack can hold. */
  Node **items;      /* The array containing Node objects. */
};

/* Constructs the Stack object with capacity in mind. */
Stack *stack_create(uint32_t capacity) {
  Stack *s = (Stack *)malloc(sizeof(Stack));
  s->top = capacity;
  s->capacity = capacity;
  s->items = (Node **)calloc(s->capacity, sizeof(Node *));
  return s;
}

void stack_delete(Stack **s) {
  /* If the stack's node array is not NULL, go through and free each node.
     Then, free and set the node array to NULL. */
  if ((*s)->items != NULL) {
    for (uint32_t i = 0; i < (*s)->capacity; i++) {
      if ((*s)->items[i] != NULL) {
        node_delete(&((*s)->items[i]));
      } else {
        free((*s)->items[i]);
        (*s)->items[i] = NULL;
      }
    }

    free((*s)->items);
    (*s)->items = NULL;
  }

  /* If the stack's pointer is not NULL, free memory from it and set it
     to NULL. */
  if (*s != NULL) {
    free(*s);
    *s = NULL;
  }
}

/* Returns true if the stack is empty and false otherwise. */
bool stack_empty(Stack *s) {
  if (s->top == s->capacity) {
    return true;
  }
  return false;
}

/* Returns true if the stack is full and false otherwise. */
bool stack_full(Stack *s) {
  if (s->top == 0) {
    return true;
  }
  return false;
}

/* Returns the amount of nodes currently in the stack. */
uint32_t stack_size(Stack *s) { return s->capacity - s->top; }

/* Pushes a node into the stack assuming that the stack is not full. Node
   n is the node that is pushed into the stack. */
bool stack_push(Stack *s, Node *n) {
  if (stack_full(s) == true) {
    return false;
  }
  s->top -= 1;
  s->items[s->top] = n;
  return true;
}

/* Pops a node from the stack assuming that the stack is not empty. Node
   n is the node that was popped from the stack. */
bool stack_pop(Stack *s, Node **n) {
  if (stack_empty(s) == true) {
    return false;
  }
  *n = s->items[s->top];
  s->items[s->top] = NULL;
  s->top += 1;
  return true;
}

/* Prints each node in the stack. */
void stack_print(Stack *s) {
  for (uint32_t i = 0; i < s->capacity; i++) {
    if (s->items[i] != NULL) {
      node_print(s->items[i]);
    }
  }
}
