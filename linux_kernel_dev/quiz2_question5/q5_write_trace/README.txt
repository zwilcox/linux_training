Do not hand out any files for this question.
The makefile will record the trace of __x64_sys_write for both programs, then output the report to a.txt and b.txt. 
Then it will filter out timestamps etc, leaving only the call graph and output to acalls.txt and bcalls.txt. 
They differ at __vfswrite()->new_sync_write(), with pipe_write() vs sock_write_iter()
The type of socket doesn't matter, as the proto_ops sendmsg call comes later in the graph.

