#include "../../include/mem.h"

unsigned long malloc(unsigned long size){
    return dPartitionAlloc(pMemHandler,size);

}

unsigned long free(unsigned long start){
    return dPartitionFree(pMemHandler,start);
}

unsigned long kmalloc(unsigned long size){
    return dPartitionAlloc(pMemHandler_kernel,size);
}

unsigned long kfree(unsigned long start){
    return dPartitionFree(pMemHandler_kernel,start);
}