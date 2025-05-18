/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

// #ifdef MM_PAGING
/*
 * System Library
 * Memory Module Library libmem.c 
 */

#include "string.h"
#include "mm.h"
#include "syscall.h"
#include "libmem.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

static pthread_mutex_t mmvm_lock = PTHREAD_MUTEX_INITIALIZER;

/*enlist_vm_freerg_list - add new rg to freerg_list
 *@mm: memory region
 *@rg_elmt: new region
 *
 */
int enlist_vm_freerg_list(struct mm_struct *mm, struct vm_rg_struct *rg_elmt)
{
  struct vm_rg_struct *rg_node = mm->mmap->vm_freerg_list;

  if (rg_elmt->rg_start >= rg_elmt->rg_end)
    return -1;
  if (rg_node != NULL)
    rg_elmt->rg_next = rg_node;

  /* Enlist the new region */
  mm->mmap->vm_freerg_list = rg_elmt;

  return 0;
}

/*get_symrg_byid - get mem region by region ID
 *@mm: memory region
 *@rgid: region ID act as symbol index of variable
 *
 */
struct vm_rg_struct *get_symrg_byid(struct mm_struct *mm, int rgid)
{
  if (rgid < 0 || rgid > PAGING_MAX_SYMTBL_SZ)
    return NULL;

  return &mm->symrgtbl[rgid];
}

/*__alloc - allocate a region memory
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@rgid: memory region ID (used to identify variable in symbole table)
 *@size: allocated size
 *@alloc_addr: address of allocated memory region
 *
 */
int __alloc(struct pcb_t *caller, int vmaid, int rgid, int size, int *alloc_addr)
{
  /*Allocate at the toproof */
  struct vm_rg_struct rgnode;

  /* TODO: commit the vmaid */
  // rgnode.vmaid

  if (get_free_vmrg_area(caller, vmaid, size, &rgnode) == 0)
  {
    caller->mm->symrgtbl[rgid].rg_start = rgnode.rg_start;
    caller->mm->symrgtbl[rgid].rg_end = rgnode.rg_end;
 
    *alloc_addr = rgnode.rg_start;

    return 0;
  }

  /* TODO get_free_vmrg_area FAILED handle the region management (Fig.6)*/

  /* TODO retrive current vma if needed, current comment out due to compiler redundant warning*/
  /*Attempt to increate limit to get space */
  //struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
  struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
  int inc_sz = PAGING_PAGE_ALIGNSZ(size);
  int old_sbrk = cur_vma->sbrk;
  caller->mm->symrgtbl[rgid].rg_start = old_sbrk;
  caller->mm->symrgtbl[rgid].rg_end = old_sbrk + size;
  //int inc_sz = PAGING_PAGE_ALIGNSZ(size);
  //int inc_limit_ret;

  /* TODO retrive old_sbrk if needed, current comment out due to compiler redundant warning*/
  //int old_sbrk = cur_vma->sbrk;

  /* TODO INCREASE THE LIMIT as inovking systemcall 
   * sys_memap with SYSMEM_INC_OP 
   */
  //struct sc_regs regs;
  //regs.a1 = ...
  //regs.a2 = ...
  //regs.a3 = ...
  if (cur_vma->vm_end - old_sbrk + 1 < inc_sz) {
    struct sc_regs regs;
    regs.a1 = SYSMEM_INC_OP;
    regs.a2 = vmaid;           
    regs.a3 = inc_sz;          

    if (syscall(caller, 17, &regs) < 0) return -1; 
  }
  /* SYSCALL 17 sys_memmap */

  /* TODO: commit the limit increment */

  /* TODO: commit the allocation address 
  // *alloc_addr = ...
  */
 if (old_sbrk + size < cur_vma->sbrk){
  struct vm_rg_struct* left_rg = malloc(sizeof(struct vm_rg_struct));
  left_rg->rg_start = old_sbrk + size;
  left_rg->rg_end = cur_vma->sbrk;
  enlist_vm_freerg_list(caller->mm, left_rg);
  }
*alloc_addr = old_sbrk;
return 0;
}

/*__free - remove a region memory
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@rgid: memory region ID (used to identify variable in symbole table)
 *@size: allocated size
 *
 */
int __free(struct pcb_t *caller, int vmaid, int rgid)
{
    if (rgid < 0 || rgid >= PAGING_MAX_SYMTBL_SZ)
        return -1;

    struct vm_rg_struct *rg = get_symrg_byid(caller->mm, rgid);
    if (rg == NULL || rg->rg_start >= rg->rg_end)
        return -1;

    struct vm_rg_struct *newrg = (struct vm_rg_struct *)malloc(sizeof(struct vm_rg_struct));
    newrg->rg_start = rg->rg_start;
    newrg->rg_end = rg->rg_end;
    newrg->rg_next = NULL;

    struct vm_area_struct *vma = get_vma_by_num(caller->mm, vmaid);
    struct vm_rg_struct *cur = vma->vm_freerg_list;
    struct vm_rg_struct *prev = NULL;

    while (cur != NULL)
    {
        if (newrg->rg_end == cur->rg_start)
        {
            newrg->rg_end = cur->rg_end;
            if (prev == NULL)
                vma->vm_freerg_list = cur->rg_next;
            else
                prev->rg_next = cur->rg_next;

            free(cur);
            cur = (prev == NULL) ? vma->vm_freerg_list : prev->rg_next;
            continue;
        }
        else if (newrg->rg_start == cur->rg_end)
        {
            newrg->rg_start = cur->rg_start;
            if (prev == NULL)
                vma->vm_freerg_list = cur->rg_next;
            else
                prev->rg_next = cur->rg_next;

            free(cur);
            cur = (prev == NULL) ? vma->vm_freerg_list : prev->rg_next;
            continue;
        }
        prev = cur;
        cur = cur->rg_next;
    }

    newrg->rg_next = vma->vm_freerg_list;
    vma->vm_freerg_list = newrg;

    rg->rg_start = 0;
    rg->rg_end = 0;

    return 0;
}


/*liballoc - PAGING-based allocate a region memory
 *@proc:  Process executing the instruction
 *@size: allocated size
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int liballoc(struct pcb_t *proc, uint32_t size, uint32_t reg_index)
{
  /* TODO Implement allocation on vm area 0 */
  int addr;
  int val = __alloc(proc, 0, reg_index, size, &addr); 
  /* By default using vmaid = 0 */
  printf("================================================================\n");
  printf("Proc %d in allocation process, size: %d of region: %d\n", proc->pid,size,  reg_index);
  printf("Rg start: %ld - Rg end:  %ld\n", proc->mm->symrgtbl[reg_index].rg_start, proc->mm->symrgtbl[reg_index].rg_end);
  printf("===== PHYSICAL MEMORY AFTER ALLOCATION =====\n");
  printf("Pid: %d - Region: %d Address: %08x Size: %d BYTE\n", proc->pid,  reg_index, addr, size);
  print_pgtbl(proc, 0, -1);
  struct vm_area_struct *vma = get_vma_by_num(proc->mm, 0);
  printf("======= HEAP SEGMENT INFO =======\n");
  printf("Heap Start : %ld\n", vma->vm_start);
  printf("Heap End   : %ld\n", vma->vm_end);
  printf("Heap sbrk  : %ld\n", vma->sbrk);
  printf("======= FREE REGION LIST =======\n");
  struct vm_rg_struct *free_rg = vma->vm_freerg_list;
  while (free_rg != NULL) {
    if(free_rg->rg_start < free_rg->rg_end){
      printf("Free: %ld -> %ld\n", 
        free_rg->rg_start, free_rg->rg_end);
    }
    free_rg = free_rg->rg_next;
  }
  printf("================================================================\n");

  return val;
}

/*libfree - PAGING-based free a region memory
 *@proc: Process executing the instruction
 *@size: allocated size
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */

int libfree(struct pcb_t *proc, uint32_t reg_index)
{
  /* TODO Implement free region */
  /* By default using vmaid = 0 */
    printf("================================================================\n");
    printf("Proc %d in deallocation process -  region: %d\n", proc->pid,  reg_index);
    int val_free = __free(proc, 0, reg_index);
    printf("===== PHYSICAL MEMORY AFTER DEALLOCATION =====\n");
    printf("Pid: %d - Region: %d\n", proc->pid,  reg_index);
    print_pgtbl(proc, 0, -1);
    struct vm_area_struct *vma = get_vma_by_num(proc->mm, 0);
    printf("======= HEAP SEGMENT INFO =======\n");
    printf("Heap Start : %ld\n", vma->vm_start);
    printf("Heap End   : %ld\n", vma->vm_end);
    printf("Heap sbrk  : %ld\n", vma->sbrk);
    printf("======= FREE REGION LIST =======\n");
    struct vm_rg_struct *free_rg = vma->vm_freerg_list;
    while (free_rg != NULL) {
      if(free_rg->rg_start < free_rg->rg_end){
        printf("Free: %ld -> %ld\n", 
          free_rg->rg_start, free_rg->rg_end);
      }
      free_rg = free_rg->rg_next;
    }
    printf("================================================================\n");
  return val_free;
}

/*pg_getpage - get the page in ram
 *@mm: memory region
 *@pagenum: PGN
 *@framenum: return FPN
 *@caller: caller
 *
 */
int pg_getpage(struct mm_struct *mm, int pgn, int *fpn, struct pcb_t *caller)
{
  uint32_t pte = mm->pgd[pgn];

  if (!PAGING_PAGE_PRESENT(pte))
  { /* Page is not online, make it actively living */
    int vicpgn, swpfpn; 
    //int vicfpn;
    //uint32_t vicpte;

    //int tgtfpn = PAGING_PTE_SWP(pte);//the target frame storing our variable

    /* TODO: Play with your paging theory here */
    /* Find victim page */
    find_victim_page(caller->mm, &vicpgn);

    /* Get free frame in MEMSWP */
    MEMPHY_get_freefp(caller->active_mswp, &swpfpn);
    int vicfpn = PAGING_FPN(mm->pgd[vicpgn]);

    /* TODO: Implement swap frame from MEMRAM to MEMSWP and vice versa*/
    /* TODO copy victim frame to swap 
     * SWP(vicfpn <--> swpfpn)
     * SYSCALL 17 sys_memmap 
     * with operation SYSMEM_SWP_OP
     */
    //struct sc_regs regs;
    //regs.a1 =...
    //regs.a2 =...
    //regs.a3 =..
    struct sc_regs regs;
    regs.a1 = SYSMEM_SWP_OP;
    regs.a2 = vicfpn;    // frame RAM đang chứa victim
    regs.a3 = swpfpn;    // vị trí trong SWAP để lưu victim
    syscall(caller, 17, &regs);
    pte_set_swap(&mm->pgd[vicpgn], 0, swpfpn); 
    /* SYSCALL 17 sys_memmap */

    /* TODO copy target frame form swap to mem 
     * SWP(tgtfpn <--> vicfpn)
     * SYSCALL 17 sys_memmap
     * with operation SYSMEM_SWP_OP
     */
    /* TODO copy target frame form swap to mem 
    
    //regs.a1 =...
    //regs.a2 =...
    //regs.a3 =..
    */
   int tgtfpn = vicfpn;
   int tgt_swpoff = PAGING_PTE_SWP(pte);
   regs.a1 = SYSMEM_SWP_OP;
   regs.a2 = tgt_swpoff; // từ SWAP
   regs.a3 = tgtfpn;     // vào RAM
   syscall(caller, 17, &regs);
    /* SYSCALL 17 sys_memmap */
    pte_set_fpn(&mm->pgd[pgn], tgtfpn);
    /* Update page table */
    //pte_set_swap() 
    //mm->pgd;

    /* Update its online status of the target page */
    //pte_set_fpn() &
    //mm->pgd[pgn];
    //pte_set_fpn();

    enlist_pgn_node(&caller->mm->fifo_pgn,pgn);
  }

  *fpn = PAGING_FPN(mm->pgd[pgn]);

  return 0;
}

/*pg_getval - read value at given offset
 *@mm: memory region
 *@addr: virtual address to acess
 *@value: value
 *
 */
int pg_getval(struct mm_struct *mm, int addr, BYTE *data, struct pcb_t *caller)
{
  int pgn = PAGING_PGN(addr);
  //int off = PAGING_OFFST(addr);
  int fpn;

  /* Get the page to MEMRAM, swap from MEMSWAP if needed */
  if (pg_getpage(mm, pgn, &fpn, caller) != 0)
    return -1; /* invalid page access */

  /* TODO 
   *  MEMPHY_read(caller->mram, phyaddr, data);
   *  MEMPHY READ 
   *  SYSCALL 17 sys_memmap with SYSMEM_IO_READ
   */
  // int phyaddr
  int phyaddr = fpn * PAGING_PAGESZ + PAGING_OFFST(addr);
  //struct sc_regs regs;
  //regs.a1 = ...
  //regs.a2 = ...
  //regs.a3 = ...
  struct sc_regs regs;
  regs.a1 = SYSMEM_IO_READ;
  regs.a2 = phyaddr;
  regs.a3 = 0;
  
  syscall(caller, 17, &regs);
  /* SYSCALL 17 sys_memmap */

  // Update data
  // data = (BYTE)
  *data = (BYTE)(regs.a3);
  return 0;
}

/*pg_setval - write value to given offset
 *@mm: memory region
 *@addr: virtual address to acess
 *@value: value
 *
 */
int pg_setval(struct mm_struct *mm, int addr, BYTE value, struct pcb_t *caller)
{
  int pgn = PAGING_PGN(addr);
  //int off = PAGING_OFFST(addr);
  int off = PAGING_OFFST(addr);
  int fpn;

  /* Get the page to MEMRAM, swap from MEMSWAP if needed */
  if (pg_getpage(mm, pgn, &fpn, caller) != 0)
    return -1; /* invalid page access */

  /* TODO
   *  MEMPHY_write(caller->mram, phyaddr, value);
   *  MEMPHY WRITE
   *  SYSCALL 17 sys_memmap with SYSMEM_IO_WRITE
   */
  // int phyaddr
  int phyaddr = fpn * PAGING_PAGESZ + PAGING_OFFST(addr);
  //struct sc_regs regs;
  //regs.a1 = ...
  //regs.a2 = ...
  //regs.a3 = ...
   struct sc_regs regs;
   regs.a1 = SYSMEM_IO_WRITE;
   regs.a2 = phyaddr;
   regs.a3 = value;
 
  /* SYSCALL 17 sys_memmap */
  syscall(caller, 17, &regs);
  // Update data
  // data = (BYTE) 

  return 0;
}

/*__read - read value in region memory
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@offset: offset to acess in memory region
 *@rgid: memory region ID (used to identify variable in symbole table)
 *@size: allocated size
 *
 */
int __read(struct pcb_t *caller, int vmaid, int rgid, int offset, BYTE *data)
{
  struct vm_rg_struct *currg = get_symrg_byid(caller->mm, rgid);
  struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);

  if (currg == NULL || cur_vma == NULL) /* Invalid memory identify */
    return -1;

  pg_getval(caller->mm, currg->rg_start + offset, data, caller);

  return 0;
}

/*libread - PAGING-based read a region memory */
int libread(
    struct pcb_t *proc, // Process executing the instruction
    uint32_t source,    // Index of source register
    uint32_t offset,    // Source address = [source] + [offset]
    uint32_t* destination)
{
  BYTE data;
  int val = __read(proc, 0, source, offset, &data);

  if (val == 0) {
    *destination = data;
  }

  /* TODO update result of reading action*/
  //destination 
#ifdef IODUMP
  printf("================================================================\n");
  printf("Read region=%d offset=%d value=%d\n", source, offset, data);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
#endif
  printf("===== PHYSICAL MEMORY DUMP ===== \n");
  MEMPHY_dump(proc->mram);
  printf("===== PHYSICAL MEMORY END-DUMP ===== \n");
#endif
  printf("================================================================\n");
  return val;
}

/*__write - write a region memory
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@offset: offset to acess in memory region
 *@rgid: memory region ID (used to identify variable in symbole table)
 *@size: allocated size
 *
 */
int __write(struct pcb_t *caller, int vmaid, int rgid, int offset, BYTE value)
{
  struct vm_rg_struct *currg = get_symrg_byid(caller->mm, rgid);
  struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);

  if (currg == NULL || cur_vma == NULL) /* Invalid memory identify */
    return -1;

  pg_setval(caller->mm, currg->rg_start + offset, value, caller);
  
  return 0;
}

/*libwrite - PAGING-based write a region memory */
int libwrite(
    struct pcb_t *proc,   // Process executing the instruction
    BYTE data,            // Data to be wrttien into memory
    uint32_t destination, // Index of destination register
    uint32_t offset)
{
  struct vm_rg_struct *region = get_symrg_byid(proc->mm, destination);
  uint32_t vaddr = region->rg_start + offset;
  uint32_t pgn = PAGING_PGN(vaddr);
  uint32_t page_offset = PAGING_OFFST(vaddr);
  uint32_t fpn = PAGING_FPN(proc->mm->pgd[pgn]);
  uint32_t phyaddr = fpn * PAGING_PAGESZ + page_offset;
#ifdef IODUMP
  printf("================================================================\n");
  printf("Write region=%d offset=%d value=%d\n", destination, offset, data);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
#endif
  printf("=====  WRITE ===== \n");
  printf("===== PHYSICAL MEMORY DUMP BEFORE WRITING ===== \n");
  MEMPHY_dump(proc->mram);
  printf("===== PHYSICAL MEMORY END-DUMP ===== \n");
  printf(">>> Value %d will be written to physical address %08x\n", data, phyaddr);
printf("================== \n");
#endif

  printf("================================================================\n");

  return __write(proc, 0, destination, offset, data);
}

/*free_pcb_memphy - collect all memphy of pcb
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@incpgnum: number of page
 */
int free_pcb_memph(struct pcb_t *caller)
{
  int pagenum, fpn;
  uint32_t pte;


  for(pagenum = 0; pagenum < PAGING_MAX_PGN; pagenum++)
  {
    pte= caller->mm->pgd[pagenum];

    if (!PAGING_PAGE_PRESENT(pte))
    {
      fpn = PAGING_PTE_FPN(pte);
      MEMPHY_put_freefp(caller->mram, fpn);
    } else {
      fpn = PAGING_PTE_SWP(pte);
      MEMPHY_put_freefp(caller->active_mswp, fpn);    
    }
  }

  return 0;
}


/*find_victim_page - find victim page
 *@caller: caller
 *@pgn: return page number
 *
 */
int find_victim_page(struct mm_struct *mm, int *retpgn)
{
  struct pgn_t *pg = mm->fifo_pgn;

  /* TODO: Implement the theorical mechanism to find the victim page */
  if (pg == NULL) return -1; 
  struct pgn_t *prev = NULL;
  while(pg->pg_next != NULL)
  {
    prev = pg;
    pg = pg->pg_next;
  }
  *retpgn = pg->pgn;    
  if(prev != NULL)
  {
    prev->pg_next = NULL;
  }
  else 
  {
    mm->fifo_pgn = NULL;
  }
  free(pg);

  return 0;
}

/*get_free_vmrg_area - get a free vm region
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@size: allocated size
 *
 */
int get_free_vmrg_area(struct pcb_t *caller, int vmaid, int size, struct vm_rg_struct *newrg)
{
    struct vm_area_struct *cur_vma= get_vma_by_num(caller->mm, vmaid);
    struct vm_rg_struct *rgit = cur_vma->vm_freerg_list;
    if (rgit == NULL)  return -1;
    newrg->rg_start = newrg->rg_end = -1;
    
    struct vm_rg_struct *best_fit = NULL;
    
    while (rgit != NULL)
    {
        if (rgit->rg_end - rgit->rg_start + 1 >= size)
        {
            if (best_fit == NULL || rgit->rg_start < best_fit->rg_start)
            {
                best_fit = rgit;
            }
        }
        rgit = rgit->rg_next;
    }

    // Nếu không có vùng nào phù hợp thì sai
    if (best_fit == NULL)
        return -1;

    newrg->rg_start = best_fit->rg_start;
    newrg->rg_end   = best_fit->rg_start + size;
    best_fit->rg_start += size;
    return 0;
}