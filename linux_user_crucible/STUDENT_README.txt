Your goals are to:

1. Create an IRC client:
	- Server is running on 172.16.1.100, port 6667 @0830.
    - Don't import/use any IRC libraries
    - You can interact via the terminal or via a script/program
    - Use RFC 2812 and RFC 2813 to understand the protocol
	- Make sure you can respond to IRC pings so you don't get timed out
	- Make sure you support at least JOIN and PRIVMSG

2. Talk to the Bots:
	- The server supports the BOT command - see its MOTD for usage
	- Implement the BOT command so you can send it with your client
	- Make sure you can get bot# to join a channel of your choosing

3. Exploit the Bots:
    Bots 1 - 5 are yours to exploit:
	- Bot 1 - Gives you a flag when you send it a PRIVMSG
	- Bot 2 - Outputs a payload for you to RE, which will get you a flag
	- Bot 3 - Wants you to tell the channel where to grab a BINARY you're hosting so it can virus scan it
			- How would you host a file?
			Note to LDRNet students: you will likely need to host at the external IP of your NoMachine (not the ifconfig IP)
				- to find IP: see the main NoMachine window -> right click on your VM -> Connection Info
			- The bot will not allow you to execute any syscalls AFTER a required main().
				- Can you get past that?
			- You will not receive an printed flag...BUT if you can get a reverse shell...you can find a flag
				- Will say "Looks good to me" if successful (scan passed).
				- Will say "Looks like a virus to me" if unsucessful (scan failed). 
	- Bot 4 - Might be vulnerable to some blind attacks - You will get a flag 

	- Bot 5 - Grand challenge. Binary and source provided. - Goal is to get bot to run a reverse shell to get a flag. ->currently ask one of us if you get there 
