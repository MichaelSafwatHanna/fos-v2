#include <inc/lib.h>

#define HEAP_PAGES_COUNT (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE
int u_heap_pages[HEAP_PAGES_COUNT]; // 0 -> available, else(size of block) -> not available
uint32 u_heap_free_space = HEAP_PAGES_COUNT * PAGE_SIZE;

// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

uint32 First_fit_Strategy(int pages_needed)
{
	int start_index = -1;
	int end_index = -1;
	int is_free = -1;

	for (int i = 0; i < HEAP_PAGES_COUNT; i++)
	{
		if (u_heap_pages[i] == 0)
		{
			if (is_free <= 0)
				start_index = i;
			is_free = 1;
		}
		else
		{
			is_free = 0;
		}

		if (pages_needed == ((i - start_index) + 1) && is_free == 1)
		{
			end_index = i;
			break;
		}
	}

	if (pages_needed != (end_index - start_index) + 1 || is_free <= 0)
	{
		return -1; // No Suitable Free Memory Block
	}

	for (int i = start_index; i <= end_index; i++)
	{
		u_heap_pages[i] = pages_needed * PAGE_SIZE;
	}

	return (start_index * PAGE_SIZE) + USER_HEAP_START;
}

uint32 Best_fit_strategy(int pages_needed)
{
	int min_start_index = 0;
	int min_pages_count = HEAP_PAGES_COUNT;
	int start_index_pointer = 0;
	int pages_counter = 0;
	int is_free = -1;

	for (int i = 0; i < HEAP_PAGES_COUNT; i++)
	{
		if (u_heap_pages[i] == 0)
		{
			if (is_free <= 0)
				start_index_pointer = i;

			pages_counter++;
			is_free = 1;
		}
		else
		{
			if (pages_counter >= pages_needed && pages_counter < min_pages_count)
			{
				min_pages_count = pages_counter;
				min_start_index = start_index_pointer;
			}
			is_free = 0;
			pages_counter = 0;
		}
	}

	if (pages_counter >= pages_needed && pages_counter < min_pages_count)
	{
		min_pages_count = pages_counter;
		min_start_index = start_index_pointer;
	}

	if (is_free <= 0)
		return -1; // No Suitable Free Memory Block

	int end_index = min_start_index + pages_needed - 1;

	for (int i = min_start_index; i <= end_index; i++)
	{
		u_heap_pages[i] = pages_needed * PAGE_SIZE;
	}

	return (min_start_index * PAGE_SIZE) + USER_HEAP_START;
}

void *malloc(uint32 size)
{
	size = ROUNDUP(size, PAGE_SIZE);
	if (size >= HEAP_PAGES_COUNT * PAGE_SIZE || size >= u_heap_free_space)
		return NULL;

	int pages_needed = size / PAGE_SIZE;
	uint32 start_address = -1;
	if (sys_isUHeapPlacementStrategyFIRSTFIT() == 1)
	{
		start_address = First_fit_Strategy(pages_needed);
	}
	else if (sys_isUHeapPlacementStrategyBESTFIT() == 1)
	{
		start_address = Best_fit_strategy(pages_needed);
	}

	if (start_address == -1)
		return NULL;

	u_heap_free_space -= size;
	sys_allocateMem(start_address, size);

	return (void *)start_address;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void *virtual_address)
{
	int start_index = (ROUNDDOWN((uint32)virtual_address, PAGE_SIZE) - USER_HEAP_START) / PAGE_SIZE;
	int size = u_heap_pages[start_index];
	int end_index = start_index + (size / PAGE_SIZE) - 1;
	for (int i = start_index; i <= end_index; i++)
	{
		u_heap_pages[i] = 0;
	}
	u_heap_free_space += size;
	sys_freeMem((uint32)virtual_address, size);
}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//
//===============
// [1] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	panic("this function is not required...!!");
	return 0;
}

//==================================================================================//
//================================ OTHER FUNCTIONS =================================//
//==================================================================================//

void *smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("this function is not required...!!");
	return 0;
}

void *sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("this function is not required...!!");
	return 0;
}

void sfree(void *virtual_address)
{
	panic("this function is not required...!!");
}

void expand(uint32 newSize)
{
	panic("this function is not required...!!");
}
void shrink(uint32 newSize)
{
	panic("this function is not required...!!");
}

void freeHeap(void *virtual_address)
{
	panic("this function is not required...!!");
}
