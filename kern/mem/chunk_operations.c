/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include "kheap.h"
#include "memory_manager.h"

/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va,
		uint32 num_of_pages) {
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] cut_paste_pages
	// Write your code here, remove the panic and write your code
	uint32 tmp = ROUNDDOWN(source_va, PAGE_SIZE);
	while (tmp < ROUNDUP(source_va + num_of_pages * PAGE_SIZE, PAGE_SIZE)) {

		uint32 *ptr1 = NULL;
		int ret = get_page_table(page_directory, tmp, &ptr1);
		if (ret == TABLE_NOT_EXIST) {
			create_page_table(page_directory, tmp);
		}

		tmp = tmp + PAGE_SIZE;

	}
	tmp = ROUNDDOWN(dest_va, PAGE_SIZE);
	while (tmp < ROUNDUP(dest_va + num_of_pages * PAGE_SIZE, PAGE_SIZE)) {

		uint32 *ptr1 = NULL;
		int ret = get_page_table(page_directory, tmp, &ptr1);
		// if it connects with frame
		struct FrameInfo *retFrame = get_frame_info(page_directory, tmp, &ptr1);

		if (retFrame != NULL)
			return -1;

		// map cur with ele henak
		tmp = tmp + PAGE_SIZE;

	}

	uint32 a = ROUNDDOWN(source_va, PAGE_SIZE), b = ROUNDUP(
			source_va + num_of_pages * PAGE_SIZE, PAGE_SIZE), dist = dest_va;
	while (a < b) {
		uint32 *ptr2 = NULL;
		int ret2 = get_page_table(page_directory, a, &ptr2);
		//struct FrameInfo *ptr_frame_info ;
		//How to get frame of source

		struct FrameInfo *retFram2 = get_frame_info(page_directory, a, &ptr2);
		int ret = map_frame(page_directory, retFram2, dist,
				pt_get_page_permissions(page_directory, a));

		//unmap_frame(uint32 *ptr_page_directory, uint32 virtual_address);
		unmap_frame(page_directory, a);

		dist = dist + PAGE_SIZE;
		a = a + PAGE_SIZE;
	}
	return 0;
	//panic("cut_paste_pages() is not implemented yet...!!");
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va,
		uint32 size) {
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] copy_paste_chunk
	// Write your code here, remove the panic and write your code
	uint32 tmp = source_va;
	while (tmp < source_va + size) {

		uint32 *ptr1 = NULL;
		int ret = get_page_table(page_directory, tmp, &ptr1);
		if (ret == TABLE_NOT_EXIST) {
			create_page_table(page_directory, tmp);
		}

		tmp = tmp + PAGE_SIZE;

	}
	tmp = dest_va;
	while (tmp < dest_va + size) {
		uint32 *ptr1 = NULL;
		int ret = get_page_table(page_directory, tmp, &ptr1);
		// if it connects with frame
		uint32 permition = pt_get_page_permissions(page_directory, tmp);
		if (ptr1 != NULL) {
			struct FrameInfo *retFrame = get_frame_info(page_directory, tmp,
					&ptr1);

			if (retFrame != NULL) {
				if ((permition & PERM_WRITEABLE) == 0) {
					return -1;
				}
			}
		}

		// map cur with ele henak
		tmp = tmp + PAGE_SIZE;

	}

	uint32 a = source_va, b = source_va + size, dist = dest_va, dist2 = dest_va
			+ size;
	unsigned char inside;

	while (a < b) {

		uint32 *ptr_soiurce = NULL;
		uint32 *ptr_dist = NULL;
		int ret = get_page_table(page_directory, a, &ptr_soiurce);
		int ret2 = get_page_table(page_directory, dist, &ptr_dist);
		struct FrameInfo *retFrame_dist = get_frame_info(page_directory, dist,
				&ptr_dist);

		if (retFrame_dist != NULL) {
			uint32 st = a;
			uint32 en = dist;
			for (int i = 0; i < PAGE_SIZE && en < dist2; i++) {

				unsigned char *byte = (unsigned char *) (st);
				inside = *byte;
				unsigned char *byte2 = (unsigned char *) (en);
				*byte2 = inside;
				st++;
				en++;

			}
		} else {
			uint32 st = a;
			uint32 en = dist;
			struct FrameInfo *ptr_frame_info = get_frame_info(page_directory,
					dist, &ptr_dist);

			allocate_frame(&ptr_frame_info);
			uint32 x = pt_get_page_permissions(page_directory,
					a) | PERM_WRITEABLE;
			map_frame(page_directory, ptr_frame_info, dist, x);
			for (int i = 0; i < PAGE_SIZE && en < dist2; i++) {

				unsigned char *byte = (unsigned char *) (st);
				inside = *byte;
				unsigned char *byte2 = (unsigned char *) (en);
				*byte2 = inside;
				st++;
				en++;
			}
		}

		a = a + PAGE_SIZE;
		dist = dist + PAGE_SIZE;
	}

	return 0;

	//panic("copy_paste_chunk() is not implemented yet...!!");
}

//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should share the given size from dest_va with the source_va
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int share_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va,
		uint32 size, uint32 perms) {
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] share_chunk
	// Write your code here, remove the panic and write your code
	uint32 tmp = ROUNDDOWN(source_va, PAGE_SIZE);
	while (tmp < ROUNDUP(source_va + size, PAGE_SIZE)) {

		uint32 *ptr1 = NULL;
		int ret = get_page_table(page_directory, tmp, &ptr1);
		if (ret == TABLE_NOT_EXIST) {
			create_page_table(page_directory, tmp);
		}

		tmp = tmp + PAGE_SIZE;

	}
	tmp = ROUNDDOWN(dest_va, PAGE_SIZE);
	while (tmp < ROUNDUP(dest_va + size, PAGE_SIZE)) {

		uint32 *ptr1 = NULL;
		int ret = get_page_table(page_directory, tmp, &ptr1);
		// if it connects with frame
		struct FrameInfo *retFrame = get_frame_info(page_directory, tmp, &ptr1);

		if (ptr1 != NULL && retFrame != NULL)
			return -1;

		tmp = tmp + PAGE_SIZE;

	}
	uint32 a = ROUNDDOWN(source_va, PAGE_SIZE), b = ROUNDUP(source_va + size,
			PAGE_SIZE), dist = dest_va;
	while (a < b) {
		uint32 *ptr2 = NULL;
		int ret2 = get_page_table(page_directory, a, &ptr2);
		struct FrameInfo *ptr_frame_info;
		//How to get frame of source

		struct FrameInfo *retFram2 = get_frame_info(page_directory, a, &ptr2);
		int ret = map_frame(page_directory, retFram2, dist, perms);

		dist = dist + PAGE_SIZE;
		a = a + PAGE_SIZE;
	}
	return 0;

	//panic("share_chunk() is not implemented yet...!!");
}

//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate in RAM the given range [va, va+size)
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms) {
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] allocate_chunk
	// Write your code here, remove the panic and write your code
	uint32 tmp = ROUNDDOWN(va, PAGE_SIZE);
	while (tmp < ROUNDUP(va + size, PAGE_SIZE)) {

		uint32 *ptr1 = NULL;
		int ret = get_page_table(page_directory, tmp, &ptr1);
		if (ret == TABLE_NOT_EXIST) {
			create_page_table(page_directory, tmp);
		}

		tmp = tmp + PAGE_SIZE;

	}
	tmp = ROUNDDOWN(va, PAGE_SIZE);
	while (tmp < ROUNDUP(va + size, PAGE_SIZE)) {

		uint32 *ptr1 = NULL;
		int ret = get_page_table(page_directory, tmp, &ptr1);
		// if it connects with frame
		struct FrameInfo *retFrame = get_frame_info(page_directory, tmp, &ptr1);

		if (retFrame != NULL)
			return -1;

		tmp = tmp + PAGE_SIZE;

	}
	uint32 a = ROUNDDOWN(va, PAGE_SIZE), b = ROUNDUP(va + size, PAGE_SIZE);
	while (a < b) {
		uint32 *ptr2 = NULL;
		int ret2 = get_page_table(page_directory, a, &ptr2);

		//How to get frame of source

		struct FrameInfo *ptr_frame_info = get_frame_info(page_directory, a,
				&ptr2);
		int ret = allocate_frame(&ptr_frame_info);
		if (ret != E_NO_MEM) {
			uint32 physical_address = to_physical_address(ptr_frame_info);
		} else
			return -1;
		int ret3 = map_frame(page_directory, ptr_frame_info, a, perms);
		ptr_frame_info->va = a;
		a = a + PAGE_SIZE;
	}
	return 0;
	//panic("allocate_chunk() is not implemented yet...!!");
}

/*BONUS*/
//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva,
		uint32 *num_tables, uint32 *num_pages) {
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_allocated_space
	// Write your code here, remove the panic and write your code
	/*
	 1- check if the frame is taken then pages++
	 2- tables ? we need to know the index of the table if it same with pre
	 then shift pre to cur  else shfit and ans++
	 * */
	uint32 pages = 0, tables = 0, prev = ROUNDDOWN(sva, PAGE_SIZE), st =
			ROUNDDOWN(sva, PAGE_SIZE), ed = ROUNDUP(eva, PAGE_SIZE);
	for (uint32 i = st; i < ed; i = i + PAGE_SIZE)
	{
		uint32 *ptr = NULL, *tmp = NULL;
		struct FrameInfo *ptr_frame_info = get_frame_info(page_directory, i,
				&ptr);
		struct FrameInfo *ptr_frame_inf = get_frame_info(page_directory, prev,
				&tmp);
		if (ptr_frame_info != NULL) {
			pages++;
		}
		/*
		 1 - PDX(CUR) = PDX(PRE)
		 2 -
		 */
		if (ptr != NULL) {

			if (i == st) {
				tables++;
			} else {
				if (tmp == ptr) {
					prev = i;

				} else {
					tables++;
					prev = i;
				}
			}

		}

	}
	*num_tables = tables;
	*num_pages = pages;
	//panic("calculate_allocated_space() is not implemented yet...!!");
}

/*BONUS*/
//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
// calculate_required_frames:
// calculates the new allocation size required for given address+size,
// we are not interested in knowing if pages or tables actually exist in memory or the page file,
// we are interested in knowing whether they are allocated or not.
uint32 calculate_required_frames(uint32* page_directory, uint32 sva,
		uint32 size) {
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_required_frames
	// Write your code here, remove the panic and write your code
	//panic("calculate_required_frames() is not implemented yet...!!");
	uint32 t = 0, p = 0, st = ROUNDDOWN(sva, PAGE_SIZE), ed = ROUNDUP(
			sva + size, PAGE_SIZE), prev = ROUNDDOWN(sva, PAGE_SIZE);
	for (uint32 i = st; i < ed; i += PAGE_SIZE)
	{
		/*
		 1- TABLES ? IF FOUND CONTINUE ELSE CTRtables++
		 2- PAGES ?
		 */
		uint32 *ptr = NULL, *tmp = NULL;
		struct FrameInfo *ptr_frame_info = get_frame_info(page_directory, i,
				&ptr);
		struct FrameInfo *ptr_frame_inf = get_frame_info(page_directory, prev,
				&tmp);

		if (ptr == NULL) {
			p++;
			if (i == st)
				t++;
			else {
				//  start virtual address of the table = sva / page size * 1024
				if (i / (PAGE_SIZE * 1024) == prev / (PAGE_SIZE * 1024)) {

					prev = i;

				} else {
					t++;
					prev = i;
				}
			}
		} else {
			p += (ptr_frame_info == NULL);
		}
	}
	return t + p;
}
//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================
//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size) {
	// Write your code here, remove the panic and write your code
	panic("allocate_user_mem() is not implemented yet...!!");
}

//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size) {
	//TODO: [PROJECT MS3] [USER HEAP - KERNEL SIDE] free_user_mem
	// Write your code here, remove the panic and write your code
	//panic("free_user_mem() is not implemented yet...!!");
	// pf_remove_env_page(e, virtual_address);
	//TODO: [PROJECT MS3] [USER HEAP - KERNEL SIDE] free_user_mem
	// Write your code here, remove the panic and write your code

	//	env_page_ws_clear_entry(struct Env* e, uint32 entry_index)

	uint32 st = ROUNDDOWN(virtual_address, PAGE_SIZE);
	uint32 en = ROUNDUP(virtual_address + size, PAGE_SIZE);

	for (int i = 0; i < e->page_WS_max_size; i++) {
		uint32 * ptr = NULL;
		uint32 va = env_page_ws_get_virtual_address(e, i);
		struct FrameInfo *retFrame = get_frame_info(e->env_page_directory, va,
				&ptr);
		if (e->ptr_pageWorkingSet[i].empty == 0 && va >= st && va < en) {
			unmap_frame(e->env_page_directory, va);
			env_page_ws_clear_entry(e, i);
		}
	}
	for (uint32 i = st; i < en; i += PAGE_SIZE) {
			pf_remove_env_page(e, i);
		}
	for (uint32 i = st; i < en; i += PAGE_SIZE) {
		int free = 1;
		uint32 * ptr = NULL;
		uint32 ret = get_page_table(e->env_page_directory, i, &ptr);
		uint32 va = i;
		struct FrameInfo *retFrame = get_frame_info(e->env_page_directory, va,
				&ptr);
		if (ret == TABLE_IN_MEMORY) {
			for (int j = 0; j < 1024; j++) {
				if (ptr[j] != 0) {
					free = 0;
				}
			}
			if (free) {
				kfree(ptr);
				pd_clear_page_dir_entry(e->env_page_directory, va);
			}
		}
	}

	//This function should:
	//1. Free ALL pages of the given range from the Page File *done
	//2. Free ONLY pages that are resident in the working set from the memory *done
	//3. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address,
		uint32 size) {
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Free any BUFFERED pages in the given range
	//4. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address,
		uint32 dst_virtual_address, uint32 size) {
	//TODO: [PROJECT MS3 - BONUS] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory

	/**/
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//

