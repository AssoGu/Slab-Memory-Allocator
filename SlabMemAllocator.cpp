/*
 * SlabMemAllocator.cpp
 *
 *  Created on: 2 Sep 2019
 *      Author: g
 */
#include "SlabMemAllocator.h"


/* Memory Allocation/Free  Methods */

void memAllocInit(uint32_t memStart, uint32_t memSize)
{
	g_memStart     = memStart;
	g_slabList 	   = nullptr;
	g_slabPool 	   = allocSlabPool(g_memStart);

	g_memStart += 0x1000;

}


void* memAlloc(uint32_t size)
{

	uint32_t newAlloc;
	Slab* current = g_slabList;
	for(; current != nullptr; current = current->nextSlab)
	{
		if(current->Alloc(size, newAlloc))
			return (void*)newAlloc;
	}

	uint32_t newSlabAlloc;
	bool didAlloc = g_slabPool->Alloc(sizeof(Slab), newSlabAlloc);
	if(didAlloc == false)
	{
		//create new slab pool
		g_slabPool  = allocSlabPool(g_memStart);
		g_memStart += 0x1000;
		g_slabPool->Alloc(sizeof(Slab), newSlabAlloc);
	}

	Slab* newSlab = (Slab*)newSlabAlloc;
	newSlab->Init(g_memStart, size);
	g_memStart += 0x1000;

	newSlab->nextSlab = g_slabList;
	g_slabList 		  = newSlab;

	newSlab->Alloc(size, newAlloc);
	return (void*)newAlloc;

}

void memFree(void* ptr)
{
	if(!ptr)
		return;

	Slab* slab       = g_slabList;
	uint32_t loc     = (uint32_t)ptr;
	for(; slab; slab = slab->nextSlab)
	{
		if(slab->Free(loc))
			return;
	}

}


static Slab* allocSlabPool(uint32_t slabStart)
{
	Slab slabObj;
	slabObj.Init(slabStart, sizeof(Slab));

	uint32_t allocSlab;
	slabObj.Alloc(sizeof(Slab), allocSlab);

	Slab* slabPoolHead = (Slab*)allocSlab;
	*slabPoolHead      = slabObj;
	return slabPoolHead;

}



/* Struct Methods */

void Slab::Init(uint32_t memStart, uint32_t blockSize)
{
	nextSlab  		= nullptr;
	slabStart 		= memStart;
	size 	  		= blockSize;
	freeBlockList	= (BlockNode*)slabStart;

	std::memset((void*)slabStart, 0, 0x1000);

	uint32_t   numOfBlocks = (0x1000 / blockSize) - 1;
	BlockNode* head 	 = freeBlockList;
	for(uint32_t i = 1; i<numOfBlocks; i++ )
	{
		head->next = (BlockNode*)(slabStart + (blockSize * i));
		head 	   = head->next;
	}

}

bool Slab::Alloc(uint32_t blockSize, uint32_t& newAlloc)
{
	if(blockSize != size || freeBlockList == nullptr )
		return false;

	newAlloc 	  = (uint32_t)freeBlockList;
	freeBlockList = freeBlockList->next;

	return true;
}

bool Slab::Free(uint32_t blockLocation)
{

	if(blockLocation < slabStart || blockLocation > slabStart + 0x1000)
		return false;

	BlockNode* newBlockNode = (BlockNode*)blockLocation;
	newBlockNode->next 		= freeBlockList;
	freeBlockList		    = newBlockNode;

	return true;

}



