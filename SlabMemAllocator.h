/*
 * SlabMemAllocator.h
 *
 *  Created on: 2 Sep 2019
 *      Author: kalihome
 */

#ifndef SLABMEMALLOCATOR_H_
#define SLABMEMALLOCATOR_H_

#include <iostream>
#include <cstring>

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;


/* Struct Methods */


typedef struct BlockNode
{
	BlockNode* next;
};

typedef struct Slab
{
	Slab* nextSlab;
	BlockNode* freeBlockList;
	uint32_t slabStart;
	uint32_t size;

	void Init(uint32_t memStart, uint32_t blockSize);
	bool Alloc(uint32_t blockSize, uint32_t& newAlloc);
	bool Free(uint32_t blockLocation);

};

/* Memory Allocation/Free  Methods */

void  memAllocInit(uint32_t memStart, uint32_t memSize);
void* memAlloc(uint32_t size);

	//create a pool of slabs object on the first 4kb in memory
static Slab* allocSlabPool(uint32_t slabStart);



/* Global Variables */
extern uint32_t g_memStart;
extern Slab* g_slabList;
extern Slab* g_slabPool;

#endif /* SLABMEMALLOCATOR_H_ */
