#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <ctype.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hi:o:v"

struct Stack {
  uint32_t top;
  uint32_t capacity;
  Node **items;
};

struct PriorityQueue {
  uint32_t tail;
  uint32_t capacity;
  Node **items;
};

int main(int argc, char **argv) {

  int opt = 0;
  bool input_file_exists = false;
  bool output_file_exists = false;
  bool print_stats = false;
  char *input_file = NULL;
  char *output_file = NULL;

  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
    case 'h': /* Help Message */
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman encoder.\n");
      fprintf(stderr,
              "  Compresses a file using the Huffman coding algorithm.\n\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  %s [-h] [-i infile] [-o outfile]\n\n", argv[0]);
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to compress.\n");
      fprintf(stderr, "  -o outfile     Output of compressed data.\n");
      return 0;
    case 'i': /* Input File */
      if (access(optarg, F_OK) != 0) {
        fprintf(stderr, "Input file doesn't exist\n");
        return 1;
      }
      input_file_exists = true;
      input_file = optarg;
      break;
    case 'o': /* Output FIle */
      output_file_exists = true;
      output_file = optarg;
      break;
    case 'v': /* Enabling Stats */
      print_stats = true;
      break;
    default: /* Bad Option */
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman encoder.\n");
      fprintf(stderr,
              "  Compresses a file using the Huffman coding algorithm.\n\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  %s [-h] [-i infile] [-o outfile]\n\n", argv[0]);
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to compress.\n");
      fprintf(stderr, "  -o outfile     Output of compressed data.\n");
      return 1;
    }
  }

  /* Setting the input file descriptor with the input file or a temp file
     in cases where the message read was from stdin (standard output). */
  int input;
  if (input_file_exists == true) {
    input = open(input_file, O_RDWR | O_CREAT, 0777);
  } else {
    input = open("message_from_stdin.txt", O_CREAT | O_WRONLY | O_TRUNC, 0600);
  }

  /* Initializes the histogram, table, and header. */
  uint64_t histogram[ALPHABET] = {0};
  Code *table = (Code *)calloc(ALPHABET, sizeof(Code));
  Header header = {
      .magic = 0, .permissions = 0, .tree_size = 0, .file_size = 0};

  /* Count the frequencies of characters from the input file or stdin
     and put the frequencies in the histogram. */
  uint8_t character[1];
  if (input_file_exists == true) {
    while (read_bytes(input, character, 1) > 0) {
      histogram[(uint8_t)character[0]] += 1;
    }
  } else {
    while (read_bytes(0, character, 1) > 0) {
      histogram[(uint8_t)character[0]] += 1;
      write_bytes(input, character, 1);
    }
  }

  if (histogram[0] == 0) {
    histogram[0] = 1;
  }

  if (histogram[1] == 0) {
    histogram[1] = 1;
  }

  /* Counts the amount of unique symbols present in the histogram */
  for (uint64_t i = 0; i < 256; i++) {
    if (histogram[i] != 0) {
      header.tree_size++;
    }
  }

  /* Builds the Huffman Tree*/
  Node *tree = build_tree(histogram);

  /* Builds the Code Table */
  build_codes(tree, table);

  /* Gets relevant stats from the input file descriptor */
  struct stat SMeta;
  fstat(input, &SMeta);
  mode_t sMode = SMeta.st_mode;
  off_t infile_size = SMeta.st_size;

  /* Sets the header's attributes */
  header.magic = MAGIC;
  header.permissions = sMode;
  header.tree_size = (3 * header.tree_size) - 1;
  header.file_size = infile_size;

  /* Writes the header and dumps tree to the output file or stdout
     (standard output). */
  int output = 0;
  if (output_file_exists == true) {
    output = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    fchmod(output, sMode);
    bytes_written += write_bytes(output, (uint8_t *)&header, sizeof(header));
    dump_tree(output, tree);
  } else {
    bytes_written += write_bytes(1, (uint8_t *)&header, sizeof(header));
    dump_tree(1, tree);
  }

  /* Resets the input file descriptor so that the message from the input
     file or stdin can be read again. */
  close(input);
  if (input_file_exists == true) {
    input = open(input_file, O_RDWR | O_CREAT, 0777);
  } else {
    input = open("message_from_stdin.txt", O_RDWR | O_CREAT, 0777);
  }

  /* Write the corresponding code for each symbol in stdin or the input
     file to stdout or the output file. Also flushes any remaining
     buffered codes with flush_codes(). */
  if (output_file_exists == true) {
    while (read_bytes(input, character, 1) > 0) {
      write_code(output, &table[character[0]]);
    }
    flush_codes(output);
  } else {
    while (read_bytes(input, character, 1) > 0) {
      write_code(1, &table[character[0]]);
    }
    flush_codes(1);
  }

  /* If stats are enabled, prints out compression statistics to standard
     error (stderr). */
  if (print_stats == true) {
    fprintf(stderr, "Uncompressed file size: %lu bytes\n", infile_size);
    fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);

    long double space_saving =
        100 * (1 - ((long double)bytes_written / (long double)infile_size));
    fprintf(stderr, "Space saving: %.2Lf%%", space_saving);
    fprintf(stderr, "\n");
  }

  /* Removes the temporary file from the directory if it was ever used. */
  if (input_file_exists == false) {
    remove("message_from_stdin.txt");
  }

  close(input);
  close(output);
  delete_tree(&tree);
  free(table);

  return 0;
}
