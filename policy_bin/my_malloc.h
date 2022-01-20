#include <stdio.h>
#include <limits.h>
//#include <sys/types.h>
#include <unistd.h>

#define INFO_SIZE sizeof(Node)

typedef struct _Node{
    size_t sz;
    int allocated;
    struct _Node * prev;
    struct _Node * next;
} Node;

void * ff_malloc(size_t size);
void * bf_malloc(size_t size);
void * growHeap(size_t size);
void * split(Node * chunck, size_t neededSz);


void ff_free(void * ptr);
void bf_free(void * ptr);
void common_free(void * ptr);
void merge(Node * first, Node * second);

unsigned long get_largest_free_data_segment_size();
unsigned long get_total_free_size();