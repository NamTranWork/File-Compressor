#include "io.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;
static char buffer[BLOCK];
static int index = -1;
static uint64_t max_bytes_per_read = 0;
static int left_over_bits = 0;

/* Basically reads nbytes from infile and setting the read characters into
   the buffer pointer buf.  But, there are cases where the read() function
   might fail for some reasons, so loop calls are sometimes needed. */
int read_bytes(int infile, uint8_t *buf, int nbytes) {
  int ret = 0;
  int total_bytes = 0;

  ret = read(infile, buf, nbytes); /* Attempt to read nbytes from infile */
  while (ret != 0) {
    total_bytes += ret;

    /* This is for cases where less than nbytes were read. */
    if ((nbytes - total_bytes) < nbytes) {
      ret = read(infile, buf + total_bytes, nbytes - total_bytes);
    } else {
      ret = read(infile, buf, nbytes);
    }
  }

  return total_bytes;
}

/* Write nbytes characters from the buffer to outfile. */
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
  int ret = write(outfile, buf, nbytes);
  int total_bytes = ret;

  /* Loop call to make sure that all nbytes were written from the
     buffer to the outfile. */
  while (total_bytes < nbytes) {
    if ((nbytes - total_bytes) < nbytes) {
      ret = write(outfile, buf + total_bytes, (nbytes - total_bytes));
    } else {
      ret = write(outfile, buf + total_bytes, nbytes);
    }
    total_bytes += ret;
  }
  return total_bytes;
}

/* Reads each byte from the static buffer from LSB to MSB one at a time.
   Returns true if a bit was read and sets the resulting bit value to the
   integer pointer bit.  Returns false if there are no more bits to be read
   from the static buffer. */
bool read_bit(int infile, uint8_t *bit) {
  /* Reads BLOCK bytes from buffer at a time until buffer is all read. */
  if (index == -1) {
    max_bytes_per_read = read_bytes(infile, (uint8_t *)buffer, BLOCK);
    bytes_read += max_bytes_per_read;
    index = (8 * max_bytes_per_read) - 1;
    max_bytes_per_read--;
  }

  if (index <= -1) /* Returns false if buffer can't be read anymore. */
  {
    return false;
  } else /* Reads a bit from the static buffer */
  {
    if ((buffer[max_bytes_per_read - (index / 8)] & (1 << (7 - (index % 8)))) >
        0) {
      *bit = 1;
    } else {
      *bit = 0;
    }
    index--;
    return true;
  }
}

static int count = 0;
static int count1 = 0;
static uint64_t bit2 = 0;

/* Writes all bits from the Code object to the buffer.  If the buffer is
   full, the buffer's characters will be written to outfile so that the
   buffer is usable again. */
void write_code(int outfile, Code *c) {
  for (uint32_t bit = 0; bit < code_size(c); bit++) {

    count++;
    if (bit2 % 8 == 0) {
      /* This is to make sure that each byte in the static buffer gets
         reset to 0 before bits are added into it from the Code object. */
      buffer[(bit2 / 8) % BLOCK] = 0;
    }

    if (count1 % 8 == 0) {
      /* Keeps track of the bit position in the buffer. */
      count1 = 0;
    }

    /* Bits from the Code object are written from LSB to MSB in the
       buffer's current byte. */
    if (code_get_bit(c, bit) == true) {
      buffer[(bit2 / 8) % BLOCK] |= (uint8_t)1 << count1;
    } else {
      buffer[(bit2 / 8) % BLOCK] |= (uint8_t)0 << count1;
    }

    count1++;

    /* Writes the contents of the buffer to outfile if the buffer
       is full. */
    if ((count / 8) >= BLOCK) {
      bytes_written += write_bytes(outfile, (uint8_t *)buffer, BLOCK);
      count = 0;
    }
    bit2++;
  }
  left_over_bits = count;
}

/* If there are still remaining bits in the buffer, simply write them
   to outfile. */
void flush_codes(int outfile) {
  if (left_over_bits > 0) {
    bytes_written +=
        write_bytes(outfile, (uint8_t *)buffer, 1 + ((left_over_bits - 1) / 8));
  }
}
