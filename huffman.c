#include "huffman.h"
#include "code.h"
#include "defines.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <stdint.h>
#include <stdio.h>

/* Explained in pq.c */
struct PriorityQueue {
  uint32_t tail;
  uint32_t capacity;
  Node **items;
};

/* Explained in stack.c */
struct Stack {
  uint32_t top;
  uint32_t capacity;
  Node **items;
};

/* Static function that constructs a Huffman from the priority queue and
   returns the root of that tree. */
static Node *construct(PriorityQueue *q) {
  Node *root = NULL;
  Node *left = NULL;
  Node *right = NULL;
  Node *parent = NULL;
  while (pq_size(q) > 1) {

    dequeue(q, &left);
    dequeue(q, &right);

    parent = node_join(left, right);
    enqueue(q, parent);
  }
  dequeue(q, &root);

  return root;
}

/* Creates frequency nodes from the histogram and uses the static
   function construct() to create a huffman tree.  Returns the resulting
   root node from construct(). */
Node *build_tree(uint64_t hist[static ALPHABET]) {
  PriorityQueue *q = pq_create(ALPHABET);

  /* Creates frequency nodes from the histogram and enqueues them
  to the priority queue. */
  for (uint64_t i = 0; i < 256; i++) {
    if (hist[i] > 0) {

      Node *n = node_create(i, hist[i]);
      enqueue(q, n);
    }
  }

  Node *n = construct(q);
  pq_delete(&q);
  return n;
}

static Code c0;
static Code *c;

/* Initializes the static Code object and pointer */
static void set_int(void) {
  c0 = code_init();
  c = &c0;
}

/* This is a static function for the purpose of creating codes from
   the huffman tree and putting them in table. */
static void build_codes2(Node *root, Code table[static ALPHABET]) {
  if (root != NULL) {
    if (root->left == NULL && root->right == NULL) {
      table[root->symbol] = *c;
    } else {
      uint8_t temp_bit = 0;

      code_push_bit(c, 0);
      build_codes2(root->left, table);
      code_pop_bit(c, &temp_bit);

      code_push_bit(c, 1);
      build_codes2(root->right, table);
      code_pop_bit(c, &temp_bit);
    }
  }
}

/* The original function that first initailizes the static Code object
   and uses build_codes2() to actually build the codes from table. */
void build_codes(Node *root, Code table[static ALPHABET]) {
  set_int();
  build_codes2(root, table);
}

/* Conducts a post-order traversal of the Huffman Tree given by root and
   writing its contents to outfile. */
void dump_tree(int outfile, Node *root) {
  if (root != NULL) {
    dump_tree(outfile, root->left);
    dump_tree(outfile, root->right);

    uint8_t buffer[1];

    if (root->left == NULL && root->right == NULL) {
      /* If root is a leaf node, write L and the root's symbol to outfile */
      buffer[0] = 'L';
      bytes_written += write_bytes(outfile, buffer, 1);
      buffer[0] = (uint8_t)root->symbol;
      bytes_written += write_bytes(outfile, buffer, 1);
    } else {
      /* If root is an interior node, write L to outfile */
      buffer[0] = 'I';
      bytes_written += write_bytes(outfile, buffer, 1);
    }
  }
}

/* Performs a post-order traversal to build a Huffman tree from the
   array tree.  Returns the root node of the tree. */
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
  Node *root = NULL;
  Stack *sk = stack_create(nbytes);
  uint16_t index = 0;

  while (index < nbytes) {
    if (tree[index] == 'L') {
      /* If the next character from tree is L, go to the next chracter
         after L to get the leaf's symbol and push it into the stack.*/
      index++;
      Node *n = node_create(tree[index], 0);
      stack_push(sk, n);
      index++;
    } else /* If the next character is I for interior */
    {
      Node *right = NULL;
      Node *left = NULL;
      Node *parent = NULL;

      stack_pop(sk, &right);
      stack_pop(sk, &left);

      parent = node_join(left, right);
      stack_push(sk, parent);
      index++;
    }
  }
  stack_pop(sk, &root);
  stack_delete(&sk);
  return root;
}

/* Performs a post-order traversal to delete the nodes from the tree. */
void delete_tree(Node **root) {
  if ((*root)->left == NULL && (*root)->right == NULL) {
    node_delete(root);
    *root = NULL;
  } else {
    delete_tree(&(*root)->left);
    delete_tree(&(*root)->right);
    delete_tree(root);
  }
}
