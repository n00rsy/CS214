#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "arraylist.h"

#define MAX_INT 2147483647

// This constant can be avoided by explicitly
// calculating height of Huffman Tree
#define MAX_TREE_HT 100
// A Huffman tree node
struct MinHeapNode {

    // One of the input characters
    char * data;

    // Frequency of the character
    unsigned freq;

    // Left and right child of this node
    struct MinHeapNode *left, *right;
};

// A Min Heap:  Collection of
// min-heap (or Huffman tree) nodes
struct MinHeap {

    // Current size of min heap
    unsigned size;

    // capacity of min heap
    unsigned capacity;

    // Array of minheap node pointers
    struct MinHeapNode** array;
};

// A utility function allocate a new
// min heap node with given character
// and frequency of the character
struct MinHeapNode* newNode(char * data, unsigned freq);

// A utility function to create
// a min heap of given capacity
struct MinHeap* createMinHeap(unsigned capacity);

// A utility function to
// swap two min heap nodes
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b);

// The standard minHeapify function.
void minHeapify(struct MinHeap* minHeap, int idx);

// A utility function to check
// if size of heap is 1 or not
int isSizeOne(struct MinHeap* minHeap);

// A standard function to extract
// minimum value node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap);

// A utility function to insert
// a new node to Min Heap
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode);


// A standard function to build min heap
void buildMinHeap(struct MinHeap* minHeap);

// A utility function to print an array of size n
void printArr(int arr[], int n, int fd);

// Utility function to check if this node is leaf
int isLeaf(struct MinHeapNode* root);

// Creates a min heap of capacity
// equal to size and inserts all character of
// data[] in min heap. Initially size of
// min heap is equal to capacity
struct MinHeap* createAndBuildMinHeap(char **data, int * freq, int size);

// The main function that builds Huffman tree
struct MinHeapNode* buildHuffmanTree(char **data, int * freq, int size);


// Prints huffman codes from the root of Huffman Tree.
// It uses arr[] to store codes
void printCodes(struct MinHeapNode* root, int arr[], int top, int fd);


// The main function that builds a
// Huffman Tree and print codes by traversing
// the built Huffman Tree
void HuffmanCodes(char ** data, int * freq, int size);


char *substring(char *string, int position, int length);


void readCodebookFromFile(ArrayList * array, char * fileName);


void buildHuffmanFromArrayList(ArrayList * array);


// Driver program to test above functions

/*
int main(){

    //test code for arraylist
    char arryee[6][15] = { "ass", "bitchesssss", "cock", "dick", "error", "faded" };
    int freq[] = { 5, 9, 12, 13, 16, 45 };
    ArrayList * array = init(5);
    //readCodebookFromFile(array);
    //print_array(array);
    /*
    int i;
    for(int i =0;i<6;i++){
        token *t = malloc(sizeof(token));
        t->str = arryee[i];
        t->num = freq[i];
        //printf("YERR: %d\n", t->num);
        add(array,t);
    }
    */
    //print_array(array);
    //char **test = arrayListStrings(array);
    //int * test2 = arrayListInts(array);
    //end of test code

    /*
    correct output:
    f: 0
    c: 100
    d: 101
    a: 1100
    b: 1101
    e: 111
    

    int size = array->current_size;

    //HuffmanCodes(test, test2, size);
    end(array);
    return 0;
}
*/
