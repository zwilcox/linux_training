# Handle reception of tcp messages
```bash
nc -l 8080 &
trace-cmd record -p function_graph -P <pid>
```

In another terminal
```
nc 127.0.0.1 8080
...send some data
```

Then end the trace-cmd and run `trace-cmd report`. You can search for stuff that starts with tcp_*
IP Layer:
- ip_rcv: Handles incoming IP packets.
- ip_input: Processes the IP header and forwards the packet to the appropriate protocol module (TCP, UDP, etc.).
TCP Layer:
- tcp_rcv: Receives TCP segments and performs various tasks, including:
	- Checking the TCP header for errors and sequence numbers.
	- Reassembling TCP segments into a complete data stream.
	- Performing flow control and congestion control.
	- Delivering the received data to the appropriate socket.
- tcp_v4_rcv: Specifically handles IPv4 TCP packets.
- tcp_v6_rcv: Specifically handles IPv6 TCP packets.
Socket Layer
- sock_rcvmsg: Receives data from the network stack and copies it into the user-space buffer provided by the application.
- sock_recv: A simplified version of sock_rcvmsg for common use cases.


# Open a file for reading
```
trace-cmd record -p function_graph -F stat /proc/self/stat
trace-cmd report
```
Search for open. There will be lots of do_execat_open and some other things. You are looking for vfs_open or ext4_file_open
System Call Entry:
- sys_open(): This is the initial entry point for the open() system call. It handles the system call arguments, performs error checking, and calls the vfs_open() function.

Virtual File System (VFS) Layer:
- vfs_open(): This function is responsible for:
	- Validating the file path and permissions.
	- Looking up the file in the file system hierarchy.
	- Creating a struct file object to represent the open file.
	- Calling the appropriate file system-specific open function.

File System-Specific Open Function:
The specific file system (e.g., ext4, XFS, NTFS) determines the exact function called to open the file. For example:
- ext4: ext4_open()
- XFS: xfs_open()
These functions handle file system-specific operations, such as:
	- Checking file permissions and access modes.
	- Acquiring necessary locks.
	- Opening the file on the underlying storage device.
	- Setting up the file's state and operations.

# Send a signal to a process
```
nc -l 8080 &
trace-cmd record  -p function_graph -F kill <pid>
```
Then `trace-cmd report` and search for signal
Core Signal Handling Functions:
- send_sig_info(): This is a core function responsible for sending a signal to a specific process. It takes a signal number, a siginfo_t structure (containing additional signal information), and a task_struct pointer as arguments.
- send_sig(): A simpler version of send_sig_info() that doesn't require a siginfo_t structure.
- force_sig(): This function is used to send a signal to a process immediately, regardless of its current state.
- force_sig_info(): Similar to force_sig(), but it also delivers a siginfo_t structure.

Other Relevant Functions:
- kill_pid(): This function sends a signal to a specific process by its PID. It's often used by user-space processes to send signals to other processes.
- kill_pg(): This function sends a signal to all processes in a process group.
- kill_proc(): This function sends a signal to a specific process, regardless of its process group.

```

# Load an ELF into memory for execution as a process
```
trace-cmd record -p function_graph ls
trace-cmd report


Look for elf. Should see a load_elf_binary() function. This is in the exec family of calls
Key Kernel Functions:
- do_execve(): This function is the main entry point for executing a new program. It's called by the execve() system call.
- load_elf_binary(): This function is responsible for parsing the ELF header, loading the ELF segments into memory, and setting up the process's memory space.
- do_mmap(): This function is used to map the ELF segments into the process's virtual memory space.
- start_thread(): This function creates a new thread of execution and initializes its stack and registers.

```
