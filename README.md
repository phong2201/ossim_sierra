# Operating Systems Assignment - Simple Operating System

**Course**: Operating Systems
**University**: HCMC University of Technology
**Faculty**: Computer Science & Engineering
**Date**: March 9, 2025

---

## \:dart: Objective

Simulate core components of a simple operating system:

* Scheduler (Multi-Level Queue)
* Synchronization
* Virtual-to-Physical Memory Allocation
* System Call Interface

---

## \:open\_file\_folder: Source Code Structure

Extracted source includes:

### Header Files

* `timer.h`, `cpu.h`, `queue.h`, `sched.h`, `mem.h`, `common.h`, `bitopts.h`, `os-mm.h`, `mm.h`, `os-cfg.h`

### Source Files

* `timer.c`, `cpu.c`, `queue.c`, `os.c`, `loader.c`, `sched.c`, `mm.c`, `mm-vm.c`, `mm-memphy.c`

### Others

* `Makefile`
* `input/`: input configuration files
* `output/`: expected simulation results

---

## \:busts\_in\_silhouette: Process Model

Each process is represented as a `pcb_t` structure containing:

* `pid`, `priority`, `code`, `regs[10]`, `pc`, `bp`
* `page_table` (obsolete), `mm` (for memory mapping)

### Supported Instructions:

* `CALC`
* `ALLOC <size> <reg>`
* `FREE <reg>`
* `READ <src_reg> <offset> <dst_reg>`
* `WRITE <value> <dst_reg> <offset>`

### Process File Format (in `input/proc/`)

```
<priority> <instruction_count>
<instruction_0>
...
<instruction_n-1>
```

---

## \:hammer: Run the Simulation

Create a configuration file in `input/`:

```
<time_slice> <num_cpu> <num_proc>
<launch_time_0> <path_0> <priority_0>
...
<launch_time_m> <path_m> <priority_m>
```

### Custom Memory Setting (if not using `MM_FIXED_MEMSZ`):

```
<ram_size> <swap0> <swap1> <swap2> <swap3>
```

### Commands:

```sh
make all
./os <config_file>
```

---

## \:triangular\_flag\_on\_post: Scheduler (MLQ)

Implements Multi-Level Queue (MLQ) scheduling:

* Each priority level has its own queue
* Round-robin within each queue
* Queue traversal uses a fixed slot formula: `slot = MAX_PRIO - prio`

Key functions to implement:

* `enqueue()` / `dequeue()` in `queue.c`
* `get_proc()` in `sched.c`

---

## \:floppy\_disk: Memory Management (Paging)

* Virtual memory per process with `mm_struct`
* Each process maintains:

  * `vm_area_struct` list
  * `vm_rg_struct` for regions
  * `symrgtbl[]` symbol table (fixed size)

### Address Translation

* Single-level paging (Page Table Entries)
* Address split into page number + offset
* Page table bits: present, swapped, dirty, etc.

### Memory Operations:

* `ALLOC`: find free region or grow segment via `SYSCALL MEMINC`
* `FREE`: adds back to `vm_freerg_list`
* `READ`/`WRITE`: may invoke swapping via `SYSCALL MEMSWP`, `SYSCALL MEMIO`

### Devices:

* `RAM` (primary, fast, direct CPU access)
* `SWAP` (secondary, indirect access)

---

## \:telephone\_receiver: System Call Interface

System calls are triggered via `SYSCALL <index>`:

### Implemented Calls:

* `listsyscall`: List all available system calls
* `memmap`: Manipulate memory mappings
* `killall`: Terminate all processes by region ID (you implement)

### Add New System Call

1. Create `src/sys_xxxhandler.c`
2. Register in `Makefile` and `syscall.tbl`
3. Recompile and run

---

## \:toolbox: Putting It All Together

* Combine scheduler, memory, and system call modules
* Use locking mechanisms to prevent race conditions in shared structures

---

## \:memo: Report Requirements

Prepare a detailed report with:

* Gantt chart (Scheduling)
* Heap/Data memory status (Memory)
* Call graph (System Calls)
* Discussion on correctness and interpretation

Include report in the project directory and submit as:

```
assignment_<STUDENTID>.zip
```

---

## \:scroll: Code of Ethics

Code is for educational use only under course CO2018. Do not reuse or redistribute.

---

## \:calendar: Revision History

| Version | Date    | Description                    |
| ------- | ------- | ------------------------------ |
| 1.0     | 01.2019 | Initial CPU, scheduler, memory |
| 1.1     | 09.2022 | Add MLQ scheduling             |
| 2.0     | 03.2023 | Add MM paging framework        |
| 2.1     | 10.2023 | Add page replacement           |
| 2.2     | 03.2024 | Add TLB                        |
| 2.3     | 10.2024 | Add heap segment               |
| 3.0     | 03.2025 | Add system call                |

---
