#include "code.h"
#include <stdio.h>

/* Creates the Code object while setting top to 0 and zeroing out the
   array in bits. */
Code code_init(void) {
  Code p = {0, {0}};
  return p;
}

/* Returns the amount of bits in the Code object. */
uint32_t code_size(Code *c) { return c->top; }

/* Returns true if there are no bits in the Code object.  Returns false
   otherwise.  */
bool code_empty(Code *c) {
  if (c->top == 0) {
    return true;
  }
  return false;
}

/* Returns true if the maximum amount of bits are already stored in the
   Code object.  Returns false otherwise. */
bool code_full(Code *c) {
  if (c->top == ALPHABET) {
    return true;
  }
  return false;
}

/* Set a specific bit in the Code object to 1 and return true.  Returns
   false if bit i is out of range. */
bool code_set_bit(Code *c, uint32_t i) {
  if (!(i >= 0 && i < ALPHABET)) {
    return false;
  }
  c->bits[i / 8] |= ((uint8_t)1 << (i % 8));
  return true;
}

/* Set a specific bit in the Code object to 0 and return true.  Returns
   false if bit i is out of range. */
bool code_clr_bit(Code *c, uint32_t i) {
  if (!(i >= 0 && i < ALPHABET)) {
    return false;
  }
  c->bits[i / 8] &= ~((uint8_t)1 << (i % 8));
  return true;
}

/* Gets the bit value in Code c and returns true if the bit value is 1
   and false if the bit value is 0.  Also returns false if bit i is out
   of range. */
bool code_get_bit(Code *c, uint32_t i) {
  if (!(i >= 0 && i < ALPHABET)) {
    return false;
  }

  if (((c->bits[i / 8] >> (i % 8)) & (uint8_t)0x1) != 0) {
    return true;
  } else {
    return false;
  }
}

/* Pushes the bit and its value to the code's bit array. */
bool code_push_bit(Code *c, uint8_t bit) {
  if (code_full(c) == true) {
    return false;
  }

  if (bit == 1) {
    code_set_bit(c, c->top);
  }

  c->top += 1;
  return true;
}

/* Removes the bit from the code's top index and setting the resulting bit
   value to the pointer bit. */
bool code_pop_bit(Code *c, uint8_t *bit) {
  if (code_empty(c) == true) {
    return false;
  }

  if (code_get_bit(c, c->top - 1) == true) {
    *bit = 1;
  } else {
    *bit = 0;
  }

  code_clr_bit(c, c->top);
  c->top -= 1;
  return true;
}

/* Prints out each bit from each uint_8t value in the code's array. */
void code_print(Code *c) {
  for (uint32_t i = 0; i < c->top; i++) {
    if (i != 0 && (i % 8 == 0)) {
      printf("\n");
    }

    if (code_get_bit(c, i) == true) {
      printf("1");
    } else {
      printf("0");
    }
  }
  printf("\n");
}
