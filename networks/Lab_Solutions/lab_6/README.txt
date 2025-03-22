For Windows: use python secure_socket.py <args>
For Linux: use python3.8 secure_socket.py <args> (has_dualstack_ipv6 is version 3.8+)

Using the client to connect to pyctf:
View pyctf certificate in a browser. It seems to have issues in Firefox so you can use Chrome. 
According to the cert, the common name is
pyctf.class.net, 
so use that as the hostname (the solution file sets this as the default)

run with args "--ip pyctf.class.net 443" to connect to pyctf on port 443.
Notice that you are get an error because the certificate is self signed. You can fix this by
passing in the certificate pyctf-class-net.pem as a trusted cert.
Add the cert when you run "--ip pyctf.class.net --server-crt pyctf-class-net.pem 443" and you 
should get tho pyctf website html text back.

Running client and server:
First, you need to use the openssl command in cmds.txt to generate a certificate key file and cert to use. 
Follow the prompts on the command line, but you need to enter "pyctf.class.net" for the Common Name 
or you will have to specify a host name when you run it.
Windows Users can find OpenSSL at C:\Program Files\Git\usr\bin\openssl.exe

You also want to have the server.crt and server.key files in the same directory as the python file 
if you use default settings.

Run the server with args "--server <port>", choosing a port to listen on (ip defaults to '' or all 
interfaces)
Run the client with args "<port>" to use the default ip 127.0.0.1, or use "--ip <addr> <port>" to 
choose another ip address. You can use "localhost" or "::1" to use an ipv6 address.

Notice that using ipv6 or ipv4 addresses both work if a dualstack server was created.
Ipv6 addresses work like normal, and ipv4 addresses are converted to ipv6, ex. 127.0.0.1 becomes 
'::ffff:127.0.0.01'

