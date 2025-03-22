import os
import re
import subprocess
import sys
import time

PROC_LIST_CMD = "ps -e -o pid,comm"
KILL_CMD = "kill -9"
APP = "gedit"


def parse_proclist_nix(proc_list):
    proc_list = proc_list.decode("utf8") if isinstance(proc_list, bytes) else proc_list

    proc_dict = {}
    proc_name = None
    proc_id = None
    regex = re.compile("(\d+)\s*(.*)")
    for line in proc_list.split("\n"):
        match = regex.search(line)
        if match is not None:
            proc_id, proc_name = match.group(1, 2)

            if proc_name in proc_dict:
                proc_dict[proc_name].append(proc_id)
            else:
                proc_dict[proc_name] = [proc_id]

            print(f"Name = {proc_name}\nPID = {proc_id}\n\n")
            proc_name, proc_id = None, None

    return proc_dict


# Use the Python subprocess module to start and kill an application
proc = subprocess.Popen(APP)
time.sleep(1)
proc.terminate()

# Use the Python subprocess module to start an instance of ps, pipe
# the output to a file, and display ONLY the PIDs and process names of all the
# running processes.
out_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), "out.txt")
subprocess.run(f"{PROC_LIST_CMD} > {out_file}", shell=True, check=True)
# Next line not needed, but shows how to parse the file
proclist = open(out_file).read()
_ = parse_proclist_nix(proclist)

# Do the above command without the intermediate file
proclist = subprocess.run(PROC_LIST_CMD, shell=True, check=True, capture_output=True)
_ = parse_proclist_nix(proclist.stdout)


# Write a function that takes as input a PID, and use the subprocess module and
# kill to kill the process with the given ID
def kill_pid(pid):
    subprocess.run(f"{KILL_CMD} {pid}", shell=True, check=True)


proc1 = subprocess.Popen(APP)
time.sleep(1)
proc2 = subprocess.Popen(APP)
time.sleep(2)
kill_pid(proc1.pid)
kill_pid(proc2.pid)


# Bonus: Write another function that takes as input a process name and uses
# subprocess and kill to kill ALL processes that have that name
def kill_proc(proc_name):
    proc_list = subprocess.run(
        f"{PROC_LIST_CMD}", shell=True, check=True, capture_output=True
    )
    proc_dict = parse_proclist_nix(proc_list.stdout)

    for pid in proc_dict[proc_name]:
        kill_pid(pid)


subprocess.Popen(APP)
time.sleep(1)
subprocess.Popen(APP)
time.sleep(2)
kill_proc(APP)
