#include "pq.h"
#include <stdio.h>
#include <stdlib.h>

struct PriorityQueue {
  uint32_t tail;     /* Index that indicates the queue's tail in items. */
  uint32_t capacity; /* Total amount of nodes the queue can hold. */
  Node **items;      /* The array containing Node objects. */
};

/* Constructs the PriorityQueue object with capacity in mind. */
PriorityQueue *pq_create(uint32_t capacity) {
  PriorityQueue *q = (PriorityQueue *)malloc(sizeof(PriorityQueue));
  q->tail = 0;
  q->capacity = capacity;
  q->items = (Node **)calloc(q->capacity, sizeof(Node *));
  return q;
}

void pq_delete(PriorityQueue **q) {
  /* If the queue's node array is not NULL, go through and free each node.
     Then, free and set the node array to NULL. */
  if ((*q)->items != NULL) {
    for (uint32_t i = 0; i < (*q)->capacity; i++) {
      if ((*q)->items[i] != NULL) {
        node_delete(&((*q)->items[i]));
      } else {
        free((*q)->items[i]);
        (*q)->items[i] = NULL;
      }
    }

    free((*q)->items);
    (*q)->items = NULL;
  }

  /* If the queue's pointer is not NULL, free memory from it and set it
     to NULL. */
  if (*q != NULL) {
    free(*q);
    *q = NULL;
  }
}

/* Returns true if the queue is empty and false otherwise. */
bool pq_empty(PriorityQueue *q) {
  if (q->tail == 0) {
    return true;
  }
  return false;
}

/* Returns true if the queue is full and false otherwise. */
bool pq_full(PriorityQueue *q) {
  if (q->tail == q->capacity) {
    return true;
  }
  return false;
}

/* Returns the amount of nodes currently in the queue. */
uint32_t pq_size(PriorityQueue *q) { return q->tail; }

/* Pushes a node into the queue assuming that the queue is not full. Node
   n is the node that is pushed into the queue. Keep in mind that nodes
   with lower frequencies have higher priorities to be pushed out first
   than nodes with higher frequencies.  Nodes with same frequencies are
   ordered in a first-in-first-out fashion. */
bool enqueue(PriorityQueue *q, Node *n) {
  if (pq_full(q) == true) {
    return false;
  }

  uint32_t temp_tail = q->tail;

  while (temp_tail > 0) {
    if ((q->items[temp_tail - 1]->frequency) > (n->frequency)) {
      temp_tail--;
    } else {
      break;
    }
  }

  for (uint32_t i = q->tail; i > temp_tail; i--) {
    q->items[i] = q->items[i - 1];
  }

  q->items[temp_tail] = n;
  q->tail += 1;

  return true;
}

/* Pops a node from the queue assuming that the queue is not empty. Node
   n is the node that was popped from the queue. */
bool dequeue(PriorityQueue *q, Node **n) {
  if (pq_empty(q) == true) {
    return false;
  }

  *n = q->items[0];
  q->tail -= 1;

  for (uint32_t i = 0; i < q->tail; i++) {
    q->items[i] = q->items[i + 1];
  }

  q->items[q->tail] = NULL;

  return true;
}

/* Prints each node in the queue. */
void pq_print(PriorityQueue *q) {
  for (uint32_t i = 0; i < q->capacity; i++) {
    if (q->items[i] != NULL) {
      node_print(q->items[i]);
    }
  }
}
