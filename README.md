# Huffman Compression in C

## Project Overview

This project implements Huffman coding, a popular and efficient lossless compression algorithm, in C.  
The program reads an input file, generates Huffman codes based on character frequencies, and compresses the file into a compact binary format.  
It writes the compressed data along with a header including frequency information needed for decompression.

### Features:
- Reads any input file (text or binary).
- Calculates Huffman codes for optimal prefix-free encoding.
- Compresses the input data efficiently.
- Saves compressed data and frequency table to an output file.
- Clean, modular C code with detailed comments for learning.

---

## How Huffman Coding Works

Huffman coding assigns variable-length binary codes to characters, with shorter codes for more frequent characters, which reduces overall file size.  
The algorithm involves:
1. Computing frequency of each character in the file.
2. Building a binary tree where low-frequency nodes are deeper.
3. Generating codes from root to leaves (0 for left, 1 for right).
4. Encoding the file using these codes into a compressed bitstream.

---

## Compilation

Make sure you have a C compiler (e.g., gcc) installed.

Open a terminal and run:

```bash
gcc -o huffman_compressor huffman_compressor.c
