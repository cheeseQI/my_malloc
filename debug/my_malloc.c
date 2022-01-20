#include "my_malloc.h"
#include "assert.h"

Node * dummyHead = NULL; //current plan : dll
Node * dummyTail = NULL;

void * growHeap(size_t size) {
        //sbrk return start of new size, not the end!
        Node * block = sbrk(INFO_SIZE +  2 * size); //temporary, maybe should allocate more space
        block->sz = 2 * size + INFO_SIZE; //record real size! 
        if (dummyHead == NULL && dummyTail == NULL) {
            dummyHead = block;
            dummyTail = block;
            block->prev = NULL;
            block->next = NULL;
        }
        else { //addLast
            dummyTail->next = block;
            block->prev = dummyTail;
            block->next = NULL;
            dummyTail = block;
        }
    if (block->sz - 2 * INFO_SIZE <= size) {
            //although not enough to split will waste part of space, it is better than not allocate and find next!
            block->allocated = 1;
            return (void *) block + INFO_SIZE;
    }
    else return split(block, size); //block + 1, 1 is info_size
}

void * ff_malloc(size_t size) {
    Node * curr = dummyHead;
    // traverse all Nodes until find suitable space;
    while (curr != NULL) {
        if (curr->allocated == 0 && curr->sz - INFO_SIZE >= size && curr->sz - 2 * INFO_SIZE <= size) {
            //although not enough to split will waste part of space, it is better than not allocate and find next!
            curr->allocated = 1;
            return (void *) curr + INFO_SIZE;
        }
        if (curr->allocated == 0 && curr->sz - 2 * INFO_SIZE > size) { //need consider the space for info for new split
            return split(curr, size);
        }
        curr = curr->next;
    }
    //no suitable space
    return growHeap(size);
}

void * bf_malloc(size_t size) {
    Node * blockBF = NULL;
    size_t sz = INT_MAX; //what is the max space that malloc can allocate
    Node * curr = dummyHead;
    while (curr != NULL) {
        if (curr->allocated == 0 && curr->sz == size) {
            blockBF = curr;
            break;
        }
        else if (curr->allocated == 0 && curr->sz > size) {
            if (curr->sz < sz) {
                sz = curr->sz;
                blockBF = curr;
            }
        }
        curr = curr->next;
    }

    if (blockBF == NULL) return growHeap(size);    
    if (blockBF->sz >= size && blockBF->sz < size + INFO_SIZE) {
            blockBF->allocated = 1;
            return (void *) blockBF + INFO_SIZE;
    }
    return split(blockBF, size);
}

/**
 * @brief 
 * 
 * @param chunck 
 * @param needed 
 * @return addr of the position that is malloc
 */
void * split(Node * chunck, size_t neededSz) {    
    Node * remainBlock = (void *) chunck + neededSz + INFO_SIZE;
    remainBlock->sz = chunck->sz - neededSz - INFO_SIZE;
    /*
    if (chunck == dummyTail) {
        assert(sbrk(0) == (void *) chunck +  chunck->sz);
        assert(sbrk(0) == (void *)remainBlock + remainBlock->sz);       
    }
    else {
        assert(chunck->next == (void *) chunck +  chunck->sz);
        assert((void *) chunck +  chunck->sz == (void *)remainBlock + remainBlock->sz);  
    }
    */
    remainBlock->allocated = 0;
    remainBlock->prev = NULL;
    remainBlock->next = NULL;
    chunck->sz = neededSz + INFO_SIZE;
    chunck->allocated = 1;
    if (chunck == dummyTail) {
        dummyTail = remainBlock;
    } 
    remainBlock->next = chunck->next;
    remainBlock->prev = chunck;
    chunck->next = remainBlock;
    return (void *) chunck + INFO_SIZE;
}

void ff_free(void * ptr) {
    common_free(ptr);
}

void bf_free(void * ptr) {
    common_free(ptr);
}

void common_free(void * ptr) {
    if (ptr == NULL) return;
    Node * block = ptr - INFO_SIZE; // get meta block, need minus
    block->allocated = 0;
    if (block != dummyTail) {
        if (block->next->allocated == 0) merge(block, block->next);
    }
    if (block != dummyHead) {
        if (block->prev->allocated == 0) merge(block->prev, block);
    }
}

void merge(Node * first, Node * second) {
    if (second == dummyTail) dummyTail = first;
    first->sz = first->sz + second->sz - INFO_SIZE; //addidtional info neglected
    first->next = second->next;
    if (first->next != NULL) first->next->prev = first; //no memory leak?? no one point to second this time...
}

unsigned long get_largest_free_data_segment_size() {
    unsigned long largest = 0;
    Node * curr = dummyHead;
    while (curr != NULL) {
        if (curr->allocated == 0 && curr->sz > largest) largest = curr->sz; 
        curr = curr->next;
    }
    return largest - INFO_SIZE;
}

unsigned long get_total_free_size() {
    unsigned long sum = 0;
    Node * curr = dummyHead;
    while (curr != NULL) {
        if (curr->allocated == 0) sum += (curr->sz - INFO_SIZE); 
        curr = curr->next;
    }
    return sum;
}
//firstly all warning about : warning: pointer of type ‘void *’ used in arithmetic
//this can be avoided using other gcc extension maybe