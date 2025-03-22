// C program to Implement Ping 

// 
// run as
//
// ./ping <remote_ip> 
// which will attempt to send ping packets to the target <remote_ip>
//
// or
//
// ./ping <remote_ip> <log_file_path> <log_message>
// which will do the same as above, but it will also open up a log
// file at <log_file_path>, write the <log_message> to it, then
// the output of the ping command will also be written to <log_file_path>
// in addition to stdout.


#include <stdio.h> 
#include <stdarg.h>
#include <stdbool.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netdb.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h> 
#include <netinet/ip_icmp.h> 
#include <signal.h> 
#include <time.h> 

// Define the Packet Constants 
// ping packet size 
#define PING_PKT_S 64 

// Automatic port number 
#define PORT_NO 0 

#define PING_SLEEP_RATE 1000000  

// Gives the timeout delay for receiving packets 
// in seconds 
#define RECV_TIMEOUT 1 

// Define the Ping Loop 
int pingloop=1; 

// Log file descriptor
FILE *g_log_fd = NULL;

// Max log message size
#define MAX_LOG_BUFF_LEN 200 

// ping packet structure 
struct ping_pkt 
{ 
	struct icmphdr hdr; 
	char msg[PING_PKT_S-sizeof(struct icmphdr)]; 
}; 

void log_printf(const char *fmt, ...);

// Calculating the Check Sum 
unsigned short checksum(void *b, int len) 
{ 
	unsigned short *buf = b; 
	unsigned int sum = 0; 
	unsigned short result; 

	for ( sum = 0; len > 1; len -= 2 )
	{	
		sum += *buf++; 
	}
	if ( len == 1 )
	{	
		sum += *(unsigned char*)buf; 
	}
	sum = (sum >> 16) + (sum & 0xFFFF); 
	sum += (sum >> 16); 
	result = ~sum; 
	return result; 
} 


// Interrupt handler 
void intHandler(int dummy) 
{ 
	pingloop=0; 
} 

// Performs a DNS lookup 
char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con) 
{ 
	log_printf("\nResolving DNS..\n"); 
	struct hostent *host_entity; 
	char *ip=(char*)malloc(NI_MAXHOST*sizeof(char)); 

	if ((host_entity = gethostbyname(addr_host)) == NULL) 
	{ 
		// No ip found for hostname 
		return NULL; 
	} 
	
	//filling up address structure 
	strcpy(ip, inet_ntoa(*(struct in_addr *)host_entity->h_addr)); 

	(*addr_con).sin_family = host_entity->h_addrtype; 
	(*addr_con).sin_port = htons (PORT_NO); 
	(*addr_con).sin_addr.s_addr = *(long*)host_entity->h_addr; 

	return ip; 
	
} 

// make ping requests
void send_ping(int ping_sockfd, struct sockaddr_in *ping_addr, char *ip_addr) 
{ 
	int ttl_val=64, msg_count=0, i, addr_len, keep_send_recving=1, msg_received_count=0; 
	struct ping_pkt pckt; 
	struct sockaddr_in r_addr; 
	struct timespec time_start, time_end, tfs, tfe; 
	long double rtt_msec=0, total_msec=0; 
	struct timeval tv_out; 
	tv_out.tv_sec = RECV_TIMEOUT; 
	tv_out.tv_usec = 0; 

	clock_gettime(CLOCK_MONOTONIC, &tfs); 

	
	// set socket options at ip to TTL and value to 64, 
	// change to what you want by setting ttl_val 
	if (setsockopt(ping_sockfd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0) 
	{ 
		log_printf("\nSetting socket options to TTL failed!\n"); 
		return; 
	} 

	else
	{ 
		log_printf("\nSocket set to TTL..\n"); 
	} 

	// setting timeout of recv setting 
	setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out); 

	// send icmp packet in an infinite loop 
	while(pingloop) 
	{ 
		keep_send_recving=1; 
	
		//filling packet 
		bzero(&pckt, sizeof(pckt)); 
		
		pckt.hdr.type = ICMP_ECHO; 
		pckt.hdr.un.echo.id = getpid(); 
		
		for ( i = 0; i < sizeof(pckt.msg)-1; i++ )
		{	
			pckt.msg[i] = i+'0'; 
		}

		pckt.msg[i] = 0; 
		pckt.hdr.un.echo.sequence = msg_count++; 
		pckt.hdr.checksum = checksum(&pckt, sizeof(pckt)); 


		usleep(PING_SLEEP_RATE); 

		//send packet 
		clock_gettime(CLOCK_MONOTONIC, &time_start); 
		if (sendto(ping_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr*) ping_addr, sizeof(*ping_addr)) <= 0) 
		{ 
			log_printf("\nPacket Sending Failed!\n"); 
			keep_send_recving=0; 
		} 

		//receive packet 
		addr_len=sizeof(r_addr); 

		if ( recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, (socklen_t*)&addr_len) <= 0 && msg_count>1) 
		{ 
			log_printf("\nPacket receive failed!\n"); 
		} 

		else
		{ 
			clock_gettime(CLOCK_MONOTONIC, &time_end); 
			
			double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec)) / 1000000.0;
			rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + timeElapsed; 
			
			// if packet was not sent, don't receive 
			if(keep_send_recving) 
			{ 
				if(!(pckt.hdr.type ==69 && pckt.hdr.code==0)) 
				{ 
					log_printf("Error..Packet received with ICMP type %d code %d\n", pckt.hdr.type, pckt.hdr.code); 
				} 
				else
				{ 
					log_printf("%d bytes from %s: msg_seq=%d ttl=%d rtt = %Lf ms.\n", PING_PKT_S, ip_addr, msg_count, ttl_val, rtt_msec); 

					msg_received_count++; 
				} 
			} 
		}	 
	} 
	clock_gettime(CLOCK_MONOTONIC, &tfe); 
	double timeElapsed = ((double)(tfe.tv_nsec - tfs.tv_nsec))/1000000.0; 
	
	total_msec = (tfe.tv_sec - tfs.tv_sec) * 1000.0 + timeElapsed; 
					
	log_printf("\n===%s ping statistics===\n", ip_addr); 
	log_printf("\n%d packets sent, %d packets received, %f percent packet loss. Total time: %Lf ms.\n\n", msg_count, msg_received_count, ((msg_count - msg_received_count)/msg_count) * 100.0, total_msec); 
} 

// Open log file at path, and write buff to it
FILE* init_log(char *path, char *buff, size_t buff_len)
{

	FILE *fd = fopen(path, "w");
	if(fd == NULL)
	{
		perror("fopen");
		return 0;
	}

	size_t bytes_written = fwrite(buff, sizeof(buff[0]), buff_len, fd);
	if(bytes_written == 0)
	{
		perror("fwrite");
		return 0;
	}	

	return fd;
}

// Helper function to print to both stdout and the log file if there is one
void log_printf(const char *fmt, ...)
{
	va_list valist;
	va_start(valist, fmt);

	// print message
	int bytes_written = vprintf(fmt, valist);
	if(bytes_written == 0)
	{
		fprintf(stderr, "vprintf failed");
		return;
	}
	va_end(valist);
	va_start(valist, fmt);

	// attempt to log the message as well
	if(g_log_fd)
	{
		bool logged = true;
		char log_buff[MAX_LOG_BUFF_LEN] = {0};
		bytes_written = vsnprintf(log_buff, MAX_LOG_BUFF_LEN, fmt, valist);
		if(bytes_written != 0)
		{
			bytes_written = fwrite(log_buff, sizeof(char), bytes_written, g_log_fd);
			if(bytes_written == 0)
			{
				logged = false;	
			}
		}
		else { logged = false; }

		if(!logged)
		{
			fprintf(stderr, "failed to log message\n");
		}
	}

	va_end(valist);
	return;
}

// Main function to set up socket fd, log fd, and start sending pings
int main(int argc, char *argv[]) 
{ 
	int sockfd; 
	char *ip_addr; 
	struct sockaddr_in addr_con; 

	if(argc<2) 
	{ 
		printf("\nFormat %s <address>\n", argv[0]); 
		return 0; 
	} 

	if(argc==4)
	{
		g_log_fd = init_log(argv[2], argv[3], strlen(argv[3]));
		if(g_log_fd == NULL)
		{
			printf("Failed to open log file for writing\n");
			return 0;
		}
		log_printf("\n\nLog file %s created and will be used for this session\n", argv[2]);
	}

	// populates addr_con structure
	ip_addr = dns_lookup(argv[1], &addr_con);
	if(ip_addr == NULL)
	{
		log_printf("\nDNS lookup failed! Could not resolve hostname!\n");
		return 0;
	}

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); 
	if(sockfd<0) 
	{ 
		log_printf("\nSocket file descriptor not received!!\n"); 
		return 0; 
	} 
	else
	{
		log_printf("\nSocket file descriptor %d received\n", sockfd); 
	}

	signal(SIGINT, intHandler);//catching interrupt 

	//send pings continuously 
	send_ping(sockfd, &addr_con, ip_addr); 

	if(g_log_fd != NULL)
	{
		fclose(g_log_fd);
	}	
	return 0; 
} 

