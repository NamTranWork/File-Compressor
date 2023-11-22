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
      fprintf(stderr, "  A Huffman decoder.\n");
      fprintf(stderr,
              "  Decompresses a file using the Huffman coding algorithm.\n\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  %s [-h] [-i infile] [-o outfile]\n\n", argv[0]);
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to decompress.\n");
      fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
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
      fprintf(stderr, "  A Huffman decoder.\n");
      fprintf(stderr,
              "  Decompresses a file using the Huffman coding algorithm.\n\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  %s [-h] [-i infile] [-o outfile]\n\n", argv[0]);
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to decompress.\n");
      fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
      return 1;
    }
  }

  /* Sets the input file descrptor with the input file if it exists.
     For standard input, 0 should suffice. */
  int input = 0;
  if (input_file_exists == true) {
    input = open(input_file, O_RDWR | O_CREAT, 0777);
  }

  /* Sets the output file descrptor with the output file if it exists.
     For standard output, 1 should suffice. */
  int output = 1;
  if (output_file_exists == true) {
    output = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0600);
  }

  /* Initializes the header object*/
  Header header = {
      .magic = 0, .permissions = 0, .tree_size = 0, .file_size = 0};

  /* Reads the header from the input file descriptor. */
  bytes_read = read_bytes(input, (uint8_t *)&header, sizeof(header));

  /* The program will exit and print out an error message if the magic
     number doesn't match with the MAGIC macro defined in defines.h */
  if (header.magic != MAGIC) {
    fprintf(stderr, "Invalid magic number\n");
    return 1;
  }

  /* If the output file exists, sets its permission bits with the bits
     provided from the header's permission bits. */
  if (output_file_exists == true) {
    fchmod(output, header.permissions);
  }

  /* Reads the dumped tree from infile into an array that is tree_size
     bytes long */
  uint8_t tree[header.tree_size];
  bytes_read += read_bytes(input, tree, header.tree_size);

  /* Reconstructs the Huffman Tree*/
  Node *h_tree = rebuild_tree(header.tree_size, tree);

  Node *temp_node = h_tree;
  uint8_t temp_bit = 0;
  bytes_written = 0;

  /* Traverses down the Huffman tree a bit at a time from input.  If
     a leaf node was found, it's written to outfile or standard output.
     Else, the trees continues to traverse down (0 = left and 1 = right). */
  while (read_bit(input, &temp_bit) == true &&
         bytes_written < header.file_size) {
    if (temp_node->left == NULL && temp_node->right == NULL) {
      uint8_t character[1];
      character[0] = temp_node->symbol;
      bytes_written += write_bytes(output, character, 1);
      temp_node = h_tree;
    }

    if (temp_bit == 0) {
      temp_node = temp_node->left;
    } else {
      temp_node = temp_node->right;
    }
  }

  /* I noticed that the loop doesn't write the possible last byte to
     outfile or stdout.  So this portion code does that task. */
  if (temp_node->left == NULL && temp_node->right == NULL &&
      bytes_written < header.file_size) {
    uint8_t character[1];
    character[0] = temp_node->symbol;
    bytes_written += write_bytes(output, character, 1);
    temp_node = h_tree;
  }

  /* If stats are enabled, prints out decompression statistics to standard
     error (stderr). */
  if (print_stats == true) {
    fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_read);
    fprintf(stderr, "Decompressed file size: %lu bytes\n", bytes_written);

    long double space_saving =
        100 * (1 - ((long double)bytes_read / (long double)bytes_written));
    fprintf(stderr, "Space saving: %.2Lf%%", space_saving);
    fprintf(stderr, "\n");
  }

  close(input);
  close(output);
  delete_tree(&h_tree);

  return 0;
}
