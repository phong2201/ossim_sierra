echo "Running sched"
./os sched > m_output/sched.output
python3 ganttchart.py m_output sched.output
python3 ganttchart.py output sched.output
echo "Done"

echo "Running sched_0"
./os sched_0 > m_output/sched_0.output
python3 ganttchart.py m_output sched_0.output
python3 ganttchart.py output sched_0.output
echo "Done"

echo "Running sched_1"
./os sched_1 > m_output/sched_1.output
python3 ganttchart.py m_output sched_1.output
python3 ganttchart.py output sched_1.output
echo "Done"

echo "Running os_1_singleCPU_mlq"
./os os_1_singleCPU_mlq > m_output/os_1_singleCPU_mlq.output
python3 ganttchart.py m_output os_1_singleCPU_mlq.output
python3 ganttchart.py output os_1_singleCPU_mlq.output
echo "Done"

echo "Running os_1_singleCPU_mlq_paging"
./os os_1_singleCPU_mlq_paging > m_output/os_1_singleCPU_mlq_paging.output
python3 ganttchart.py m_output os_1_singleCPU_mlq_paging.output
python3 ganttchart.py output os_1_singleCPU_mlq_paging.output
echo "Done"

echo "Running os_0_mlq_paging"
./os os_0_mlq_paging > m_output/os_0_mlq_paging.output
python3 ganttchart.py m_output os_0_mlq_paging.output
python3 ganttchart.py output os_0_mlq_paging.output
echo "Done"

echo "Running os_1_mlq_paging"
./os os_1_mlq_paging > m_output/os_1_mlq_paging.output
python3 ganttchart.py m_output os_1_mlq_paging.output
python3 ganttchart.py output os_1_mlq_paging.output
echo "Done"

echo "Running os_1_mlq_paging_small_1K"
./os os_1_mlq_paging_small_1K > m_output/os_1_mlq_paging_small_1K.output
python3 ganttchart.py m_output os_1_mlq_paging_small_1K.output
python3 ganttchart.py output os_1_mlq_paging_small_1K.output
echo "Done"

echo "Running os_1_mlq_paging_small_4K"
./os os_1_mlq_paging_small_4K > m_output/os_1_mlq_paging_small_4K.output
python3 ganttchart.py m_output os_1_mlq_paging_small_4K.output
python3 ganttchart.py output os_1_mlq_paging_small_4K.output
echo "Done"

echo "Running os_sc"
./os os_sc > m_output/os_sc.output
python3 ganttchart.py m_output os_sc.output
python3 ganttchart.py output os_sc.output
echo "Done"

echo "Running os_syscall"
./os os_syscall > m_output/os_syscall.output
python3 ganttchart.py m_output os_syscall.output
python3 ganttchart.py output os_syscall.output
echo "Done"

echo "Running os_syscall_list"
./os os_syscall_list > m_output/os_syscall_list.output
python3 ganttchart.py m_output os_syscall_list.output
python3 ganttchart.py output os_syscall_list.output
echo "Done"