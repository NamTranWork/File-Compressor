# File-Compressor Documentation

## Directions
1) Open up the command line in Ubuntu 22.04 (Linux) and make sure that the clang complier and git have been installed in your local device.
2) Make sure that the repository folder gets cloned to a designated folder in your local device.
3) Make sure that right files have been loaded, especially the header files, program files, and the Makefile.
4) Go to the designated folder and open up the terminal.
5) Once you are in the designated directory, enter the command: $ make.
6) The commands in the Makefile will make compling the header and program files in the directory easier.
7) There is two main executables called encode and decode.  Encode basically compresses a message from a text file or standard input to an output file or standard output.  Decode decompresses and regains the message from a text file or standard input to an output file or standard output.
8) To run encode, do: $ stdin | ./encode <-options-> or ./encode -i infile <-options->
8) To run decode, do: $ stdin | ./decode <-options-> or ./decode -i infile <-options->


## Command-line options for encode.c
- -h: Prints out help message which states the purpose of the program and the acceptable command-line options.  Exits the program afterwards.
- -i <-infile-> : Specifies the input file to encode with Huffman coding.  Default: stdin (standard input)
- -o <-outfile-> : Specifies the output file to write the compressed input with.  Default: stdout (standard output)
- -v: Prints compression statistics to stderr (standard error)


## Command-line options for decode.c
- -h: Prints out help message which states the purpose of the program and the acceptable command-line options.  Exits the program afterwards.
- -i <-infile-> : Specifies the input file to decode with Huffman coding.  Default: stdin (standard input)
- -o <-outfile-> : Specifies the output file to write the decompressed input with.  Default: stdout (standard output)
- -v: Prints decompression statistics to stderr (standard error)

## Deliverables 
- encode.c (My implemention of the Huffman encoder and compressor)
- decode.c (My implemention of the Huffman decoder and decompressor)
- defines.c (Macros definitions used throughout the files)
- header.h (Contains a struct definition of a file header)
- node.h (Contains the node ADT interface)
- node.c (My implementation of the node ADT)
- pq.h (Contains the priority queue ADT interface)
- pq.c (My implementation of the priority queue ADT.  I also defined my own struct definition of a priority queue in this file.)
- code.h (Contains the code ADT interface)
- code.c (My implementation of the code ADT)
- io.h (Contains the I/O module interface)
- io.c (My implementation of the I/O module)
- stack.h (Contains the stack ADT interface)
- stack.c (My implementation of the stack ADT)
- huffman.h (Contains the Huffman coding module interface)
- huffman.c (My implementation of the Huffman coding module interface)
- Makefile (A compile program that I created to automate creating,removing, and formatting executables and object files.)


|Name|Email|
|----|-----|
|Nam Tran|natrtran@ucsc.edu|
