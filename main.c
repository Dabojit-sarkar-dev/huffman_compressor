#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_NODES 512

typedef struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
} MinHeapNode;

typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    MinHeapNode **array;
} MinHeap;

// Function prototypes
MinHeapNode* createNode(char data, unsigned freq);
MinHeap* createMinHeap(unsigned capacity);
void insertMinHeap(MinHeap* minHeap, MinHeapNode* node);
MinHeapNode* extractMin(MinHeap* minHeap);
void minHeapify(MinHeap* minHeap, int idx);
void buildMinHeap(MinHeap* minHeap);
MinHeapNode* buildHuffmanTree(char data[], int freq[], int size);
void generateCodes(MinHeapNode* root, char *code, int top, char codes[256][MAX_TREE_NODES]);
void freeTree(MinHeapNode* root);
void writeBits(FILE* out, unsigned char *bitBuffer, int *bitCount, const char *code);
void flushBits(FILE* out, unsigned char *bitBuffer, int *bitCount);

// Frequency calculation
void calculateFrequency(const char *input, int freq[]) {
    for (int i = 0; i < 256; i++)
        freq[i] = 0;
    for (int i = 0; input[i] != '\0'; i++)
        freq[(unsigned char)input[i]]++;
}

// Create a new node
MinHeapNode* createNode(char data, unsigned freq) {
    MinHeapNode* node = (MinHeapNode*) malloc(sizeof(MinHeapNode));
    node->data = data;
    node->freq = freq;
    node->left = node->right = NULL;
    return node;
}

// Create a min heap with given capacity
MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**) malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2*idx + 1;
    int right = 2*idx + 2;

    if (left < (int)minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < (int)minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}

MinHeapNode* extractMin(MinHeap* minHeap) {
    MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1)/2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

void buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n-1)/2; i >= 0; i--)
        minHeapify(minHeap, i);
}

// Build Huffman Tree from frequencies
MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; i++)
        insertMinHeap(minHeap, createNode(data[i], freq[i]));

    buildMinHeap(minHeap);

    while (!isSizeOne(minHeap)) {
        MinHeapNode* left = extractMin(minHeap);
        MinHeapNode* right = extractMin(minHeap);
        MinHeapNode* top = createNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    MinHeapNode* root = extractMin(minHeap);
    free(minHeap->array);
    free(minHeap);
    return root;
}

// Generate codes from Huffman tree
void generateCodes(MinHeapNode* root, char *code, int top, char codes[256][MAX_TREE_NODES]) {
    if (root->left) {
        code[top] = '0';
        generateCodes(root->left, code, top + 1, codes);
    }
    if (root->right) {
        code[top] = '1';
        generateCodes(root->right, code, top + 1, codes);
    }
    if (!root->left && !root->right) {
        code[top] = '\0';
        strcpy(codes[(unsigned char)root->data], code);
    }
}

// Free allocated Huffman tree
void freeTree(MinHeapNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// Write bits to output file buffer (one bit at a time)
void writeBits(FILE *out, unsigned char *bitBuffer, int *bitCount, const char *code) {
    for (int i = 0; code[i] != '\0'; ++i) {
        // Shift bits to left, add new bit
        *bitBuffer = (*bitBuffer << 1) | (code[i] - '0');
        (*bitCount)++;

        // If we have a full byte, write it to file
        if (*bitCount == 8) {
            fputc(*bitBuffer, out);
            *bitBuffer = 0;
            *bitCount = 0;
        }
    }
}
// Flush remaining bits if less than full byte
void flushBits(FILE *out, unsigned char *bitBuffer, int *bitCount) {
    if (*bitCount > 0) {
        *bitBuffer = *bitBuffer << (8 - *bitCount);
        fputc(*bitBuffer, out);
        *bitBuffer = 0;
        *bitCount = 0;
    }
}

int main() {
    char inputFileName[256];
    char outputFileName[256];

    printf("Huffman Compressor\n");
    printf("Enter input file path (text to compress): ");
    fgets(inputFileName, sizeof(inputFileName), stdin);
    inputFileName[strcspn(inputFileName, "\n")] = 0;

    printf("Enter output (compressed) file path: ");
    fgets(outputFileName, sizeof(outputFileName), stdin);
    outputFileName[strcspn(outputFileName, "\n")] = 0;

    // Read input file into memory
    FILE *inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        fprintf(stderr, "Error: cannot open input file %s\n", inputFileName);
        return 1;
    }
    fseek(inputFile, 0, SEEK_END);
    long fileSize = ftell(inputFile);
    rewind(inputFile);

    if (fileSize == 0) {
        fprintf(stderr, "Error: input file is empty.\n");
        fclose(inputFile);
        return 1;
    }

    unsigned char *buffer = malloc(fileSize + 1);
    if (!buffer) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        fclose(inputFile);
        return 1;
    }

    fread(buffer, 1, fileSize, inputFile);
    buffer[fileSize] = '\0';
    fclose(inputFile);

    // Frequency calculation
    int freq[256];
    calculateFrequency((char*)buffer, freq);

    // Count unique chars for Huffman tree
    int uniqueCount = 0;
    char data[256];
    int dataFreq[256];
    for (int i = 0; i < 256; i++) {
        if (freq[i]) {
            data[uniqueCount] = (char)i;
            dataFreq[uniqueCount] = freq[i];
            uniqueCount++;
        }
    }

    if (uniqueCount == 0) {
        fprintf(stderr, "Error: no data to compress.\n");
        free(buffer);
        return 1;
    }

    // Build Huffman Tree
    MinHeapNode* root = buildHuffmanTree(data, dataFreq, uniqueCount);

    // Generate Huffman codes
    char codes[256][MAX_TREE_NODES];
    for(int i=0; i<256; i++)
        codes[i][0] = '\0';

    char code[MAX_TREE_NODES];
    generateCodes(root, code, 0, codes);

    // Open output file for writing compressed data
    FILE *outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        fprintf(stderr, "Error: cannot open output file %s\n", outputFileName);
        free(buffer);
        freeTree(root);
        return 1;
    }

    // Write header: number of unique chars (1 byte max 256)
    fputc(uniqueCount, outputFile);

    // Write the character-frequency table (each char + 4 bytes freq)
    for (int i = 0; i < uniqueCount; i++) {
        fputc(data[i], outputFile);
        // Write frequency as 4 byte unsigned int (big endian)
        unsigned int f = dataFreq[i];
        fputc((f >> 24) & 0xFF, outputFile);
        fputc((f >> 16) & 0xFF, outputFile);
        fputc((f >> 8) & 0xFF, outputFile);
        fputc(f & 0xFF, outputFile);
    }

    // Write compressed data as bits
    unsigned char bitBuffer = 0;
    int bitCount = 0;
    for (long i = 0; i < fileSize; i++) {
        writeBits(outputFile, &bitBuffer, &bitCount, codes[buffer[i]]);
    }
    flushBits(outputFile, &bitBuffer, &bitCount);

    fclose(outputFile);
    free(buffer);
    freeTree(root);

    printf("Compression complete!\n");
    printf("Input file: %s\n", inputFileName);
    printf("Output compressed file: %s\n", outputFileName);

    return 0;
}
