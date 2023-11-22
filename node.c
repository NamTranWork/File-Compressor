#include "node.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/* Constructs a Node object */
Node *node_create(uint8_t symbol, uint64_t frequency) {
  Node *n = (Node *)malloc(sizeof(Node));
  n->left = NULL;
  n->right = NULL;
  n->symbol = symbol;
  n->frequency = frequency;
  return n;
}

/* Frees memory from the node and setting the pointer to NULL. */
void node_delete(Node **n) {
  if (*n) {
    free(*n);
    *n = NULL;
  }
}

Node *node_join(Node *left, Node *right) {
  /* Assuming that the left and right children nodes exists, the function
     creates a parent node combining both left's and right's frequencies
     and setting the symbol to '$'. */

  if (left != NULL && right != NULL) {
    Node *parent = (Node *)malloc(sizeof(Node));
    parent->left = left;
    parent->right = right;
    parent->symbol = '$';
    parent->frequency = left->frequency + right->frequency;
    return parent;
  }
  return NULL;
}

void node_print(Node *n) {
  /* The first option are for node symbols that are printable characters
     and non-control characters.  The second obtion are for node symbols
     that don't fit the requirements object. */
  if (isprint(n->symbol) != 0 && iscntrl(n->symbol) == 0) {
    printf("Frequency: %lu || Symbol: %c\n", n->frequency, n->symbol);
  } else {
    printf("Frequency: %lu || ", n->frequency);
    printf("Symbol: 0x%02" PRIx8 "\n", n->symbol);
  }
}

/* Returns true is Node n's frequency is bigger than Node m's frequency. */
bool node_cmp(Node *n, Node *m) {
  if (n->frequency > m->frequency) {
    return true;
  }
  return false;
}

/* Does the same thing as node_print() except that it only prints the
   node's symbol. */
void node_print_sym(Node *n) {
  if (isprint(n->symbol) != 0 && iscntrl(n->symbol) == 0) {
    printf("Symbol: %c\n", n->symbol);
  } else {
    printf("Symbol: 0x%02" PRIx8 "\n", n->symbol);
  }
}
