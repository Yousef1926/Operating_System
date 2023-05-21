#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//==================================================================//
//==================================================================//
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)//
//==================================================================//
//==================================================================//

void initialize_dyn_block_system()
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] initialize_dyn_block_system
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("initialize_dyn_block_system() is not implemented yet...!!");

	//[1] Initialize two lists (AllocMemBlocksList & FreeMemBlocksList) [Hint: use LIST_INIT()]
	LIST_INIT(&AllocMemBlocksList);
	LIST_INIT(&FreeMemBlocksList);


#if STATIC_MEMBLOCK_ALLOC
	//DO NOTHING
#else
	/*[2] Dynamically allocate the array of MemBlockNodes
	 * 	remember to:
	 * 		1. set MAX_MEM_BLOCK_CNT with the chosen size of the array
	 * 		2. allocation should be aligned on PAGE boundary
	 * 	HINT: can use alloc_chunk(...) function
	 */

	    MAX_MEM_BLOCK_CNT = (KERNEL_HEAP_MAX - KERNEL_HEAP_START)/PAGE_SIZE;
		allocate_chunk(ptr_page_directory,KERNEL_HEAP_START,MAX_MEM_BLOCK_CNT*sizeof(struct MemBlock), PERM_WRITEABLE);
		MemBlockNodes =  (struct MemBlock*) KERNEL_HEAP_START;
		int max_calculated_size =ROUNDUP(sizeof(struct MemBlock)*MAX_MEM_BLOCK_CNT,PAGE_SIZE);


#endif
	//[3] Initialize AvailableMemBlocksList by filling it with the MemBlockNodes
	 initialize_MemBlocksList(MAX_MEM_BLOCK_CNT);
	 struct MemBlock*cur=LIST_FIRST(&AvailableMemBlocksList);
	 //struct MemBlock *Blocktoadd;

	 cur->sva =KERNEL_HEAP_START+max_calculated_size;
	 cur->size=(KERNEL_HEAP_MAX - KERNEL_HEAP_START)-max_calculated_size;

	 LIST_REMOVE(&AvailableMemBlocksList, cur);


	//[4] Insert a new MemBlock with the remaining heap size into the FreeMemBlocksList
	 LIST_INSERT_HEAD(&FreeMemBlocksList,cur);
}

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kmalloc
	// your code is here, remove the panic and write your code
	struct MemBlock *cur;
		            size = ROUNDUP(size,PAGE_SIZE);
		            int firstfit = isKHeapPlacementStrategyFIRSTFIT();
		            int nextfit = isKHeapPlacementStrategyNEXTFIT();
		                if(nextfit == 1) {
		                    cur = alloc_block_NF(size);
		                }
		                else if(firstfit == 1)
		                    {
		                        cur = alloc_block_FF(size);
		                        //insert_sorted_allocList(cur);
		                    }
		                else
		                    {
		                        cur = alloc_block_BF(size);
		                        //insert_sorted_allocList(cur);
		                    }

		                    if(cur != NULL)
		                    {
		                       int x=allocate_chunk(ptr_page_directory, cur->sva , cur->size , PERM_WRITEABLE);
		                       LIST_INSERT_TAIL(&AllocMemBlocksList,cur);

		                       if(x==0)
		                       {
		                    	   return (void *)cur->sva;
		                       }
		                       else
		                    	   return NULL;

		                    }
		                    else
		                        return NULL;

		        return NULL;

	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy

	//change this "return" according to your answer
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kfree
	// Write your code here, remove the panic and write your code
	//&AllocMemBlocksList
	    struct MemBlock * cur=NULL ;

		cur = find_block(&AllocMemBlocksList, (uint32 )virtual_address);
		if(cur==NULL)
		{
			return;
		}
		    uint32 st= ROUNDDOWN((uint32 )virtual_address,PAGE_SIZE);
			uint32 en=ROUNDUP((uint32 )virtual_address+cur->size,PAGE_SIZE);
		    LIST_REMOVE(&AllocMemBlocksList,cur);

		while(st<en)
		{

			unmap_frame(ptr_page_directory,st);

			st=st+PAGE_SIZE;
		}

		insert_sorted_with_merge_freeList(cur);

	//panic("kfree() is not implemented yet...!!");

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

struct FrameInfo *ret=NULL;
ret=to_frame_info(physical_address);
uint32 x = ret->va;
return x;

    //return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details
	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	 uint32 *ptr = NULL;
			  int ret = get_page_table( ptr_page_directory,virtual_address,&ptr);
	          // pa = frame number * page size
		       uint32 ans = ptr[PTX(virtual_address - KERNEL_HEAP_START)] >> 12;

		       ans = ans * PAGE_SIZE;

		       return ans;


	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
}


void kfreeall()
{
	panic("Not implemented!");

}

void kshrink(uint32 newSize)
{
	panic("Not implemented!");
}

void kexpand(uint32 newSize)
{
	panic("Not implemented!");
}




//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT MS2 - BONUS] [KERNEL HEAP] krealloc
	// Write your code here, remove the panic and write your code

	struct MemBlock *cur;
		if((uint32*)virtual_address==NULL)
		{
			struct MemBlock *cur;
			uint32 size = ROUNDUP(new_size,PAGE_SIZE);
			int firstfit = isKHeapPlacementStrategyFIRSTFIT();
			int nextfit = isKHeapPlacementStrategyNEXTFIT();
			if(nextfit == 1) {
				cur = alloc_block_NF(size);
					                    //insert_sorted_allocList(cur);
					                }
					                else if(firstfit == 1)
					                    {
					                        cur = alloc_block_FF(size);
					                        //insert_sorted_allocList(cur);
					                    }
					                    else
					                    {
					                        cur = alloc_block_BF(size);
					                        //insert_sorted_allocList(cur);
					                    }

					                    if(cur != NULL)
					                    {
					                       int x=allocate_chunk(ptr_page_directory, cur->sva , cur->size , PERM_WRITEABLE);
					                       LIST_INSERT_TAIL(&AllocMemBlocksList,cur);
					                       return (void *)cur->sva;

					                    }
					                    else
					                        return NULL;

					        return NULL;
		}
		else if(new_size==0)
		{
			kfree(virtual_address);
			return (uint32*)virtual_address;

		}
	return NULL;






	//panic("krealloc() is not implemented yet...!!");
}
