# Operating Systems Assignment - Simple Operating System

**Course**: Operating Systems
**University**: HCMC University of Technology
**Faculty**: Computer Science & Engineering
**Date**: March 9, 2025

---

## 🎯 Objective

Simulate core components of a simple operating system:

* Scheduler (Multi-Level Queue)
* Synchronization
* Virtual-to-Physical Memory Allocation
* System Call Interface

---

## 📦 Source Code Structure

Extracted source includes:

### Header Files

* `timer.h`: define the timer
* `cpu.h`: define virtual CPU functions
* `queue.h`: queue operations
* `sched.h`: scheduler logic
* `mem.h`, `os-mm.h`, `mm.h`: memory management
* `common.h`: common structs/functions
* `bitopts.h`: bit manipulation
* `os-cfg.h`: optional configurations

### Source Files

* `timer.c`, `cpu.c`, `queue.c`, `sched.c`, `os.c`, `loader.c`, `mm.c`, `mm-vm.c`, `mm-memphy.c`

### Other Files

* `Makefile`
* `input/`: input programs and environment configurations
* `output/`: sample outputs

---

## 🧠 Process Model

Each process is represented by a `pcb_t` struct:

```c
struct pcb_t {
  uint32_t pid;
  uint32_t priority;
  char path[100];
  uint32_t code_seg_t * code;
  addr_t regs[10];
  uint32_t pc;
  #ifdef MLQ_SCHED
  uint32_t prio;
  #endif
  struct page_table_t * page_table;
  uint32_t bp;
};
```

Fields include:

* `pid`: process ID
* `priority`: static priority (lower value = higher priority)
* `code`: pointer to code segment
* `regs[10]`: general purpose registers
* `pc`: program counter
* `page_table`: obsolete
* `bp`: break pointer (heap management)
* `prio`: runtime priority (if MLQ)

### Supported Instructions

* `CALC`: simulate computation
* `ALLOC <size> <reg>`: allocate memory
* `FREE <reg>`: free allocated memory
* `READ <src> <offset> <dst>`: read from memory
* `WRITE <val> <dst> <offset>`: write to memory

---

## 🛠️ Creating a Process

A process program is defined in a file:

```
<priority> <number of instructions>
<instruction 0>
...
<instruction N-1>
```

The priority here can be overridden when launching via environment config file.

---

## 🧪 Running the Simulation

Configuration file format:

```
<time_slice> <num_cpus> <num_processes>
<time_0> <path_0> <prio_0>
...
<time_M-1> <path_M-1> <prio_M-1>
```

### With custom memory size (if `MM_FIXED_MEMSZ` is undefined):

```
<RAM_SZ> <SWP0_SZ> <SWP1_SZ> <SWP2_SZ> <SWP3_SZ>
```

Run commands:

```sh
make all
./os <config_file>
```

---

## ⏳ Scheduler - Multi-Level Queue (MLQ)

Each priority has its own ready queue.

* New processes are enqueued based on priority.
* Round-robin within each queue.
* CPU switches between queues based on fixed slots: `slot = MAX_PRIO - prio`

### Functions to Implement

* `enqueue()` and `dequeue()` in `queue.c`
* `get_proc()` in `sched.c`

> Linux-like MLQ used, but without feedback mechanism.

---

## 💾 Memory Management - Paging

### Virtual Memory Mapping

* Each process has a list of `vm_area_struct`
* Inside each area are regions tracked by `vm_rg_struct`
* Allocated regions stored in `symrgtbl[]` (limited number)

#### Structures:

```c
struct vm_rg_struct {
  unsigned long rg_start, rg_end;
  struct vm_rg_struct *rg_next;
};

struct vm_area_struct {
  unsigned long vm_id, vm_start, vm_end, sbrk;
  struct mm_struct *vm_mm;
  struct vm_rg_struct *vm_freerg_list;
  struct vm_area_struct *vm_next;
};

struct mm_struct {
  uint32_t *pgd;
  struct vm_area_struct *mmap;
  struct vm_rg_struct symrgtbl[PAGING_MAX_SYMTBL_SZ];
  struct pgn_t *fifo_pgn;
};
```

### Physical Memory - `memphy_struct`

```c
struct framephy_struct {
  int fpn;
  struct framephy_struct *fp_next;
};

struct memphy_struct {
  BYTE *storage;
  int maxsz, rdmflg, cursor;
  struct framephy_struct *free_fp_list, *used_fp_list;
};
```

---

## 🧭 Paging Translation

* Use one-level page table
* CPU address split into page number + offset
* Page Table Entry (PTE) format includes:

  * `present`, `swapped`, `dirty` bits
  * physical frame number, swap info

### Basic Memory Operations

* **ALLOC**: find free region or call `SYSCALL MEMINC`
* **FREE**: add region to `vm_freerg_list`
* **READ/WRITE**: handle page fault, swap in/out as needed

---

## 🔁 Memory System Flow

```
ALLOC → get region → no region → SYSCALL MEMINC → got new region
FREE → mark region as free
READ/WRITE → locate page → if not present → SYSCALL MEMSWP → SYSCALL MEMIO
```

### Modules Involved

* `libmem`: user-facing API
* `mm-vm.c`: virtual memory logic
* `mm.c`: address mapping
* `mm-memphy.c`: memory backend

---

## 📞 System Calls

### Available System Calls

| Name          | Index | Description                          |
| ------------- | ----- | ------------------------------------ |
| `listsyscall` | 0     | Show available syscalls              |
| `memmap`      | 17    | Memory operations: alloc, free, swap |
| `killall`     | 101   | Kill processes matching REGIONID     |

### Adding a Custom Syscall

1. Add `sys_xxxhandler.c`
2. Register in `Makefile` and `syscall.tbl`
3. Use `SYSCALL <index>` in your program

---

## 🧩 Integration & Synchronization

Final phase integrates scheduler + memory + syscall. You must:

* Identify shared resources
* Protect access with locking (critical sections)

Example shared structures: ready queue, memory table.

---

## 📝 Report Guidelines

* Draw Gantt chart (Scheduler)
* Show heap/data segment state (Memory)
* Explain syscall interactions
* Interpret simulation outputs


## 📜 Code of Ethics

Use of provided code is limited to educational purposes within CO2018 course.

---

## 📅 Revision History

| Version | Date     | Description                |
| ------- | -------- | -------------------------- |
| 1.0     | Jan 2019 | Init CPU/Scheduling/Memory |
| 1.1     | Sep 2022 | Add MLQ Scheduler          |
| 2.0     | Mar 2023 | Paging Framework           |
| 2.1     | Oct 2023 | Page Replacement           |
| 2.2     | Mar 2024 | Add TLB                    |
| 2.3     | Oct 2024 | Add heap segment           |
| 3.0     | Mar 2025 | Add system calls           |

---
