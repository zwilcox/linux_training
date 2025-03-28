#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_DATA 260

#define MSG_ECHO 0
#define MSG_HEXI 1
#define MSG_ADDR 2
#define MSG_DELR 3
#define MSG_CONS 4
#define MSG_MAX  5

typedef struct msg {
    short type;
    short len;
    //unsigned char* data;
} msg_t;

typedef struct record {
    struct record *next;
    struct record *prev;
    int rid;
    int price;
} rec_t;

typedef int (*msghandler_t)(int, unsigned int, unsigned char*);
// Forward decs for msg handler functions
int doEchoMsg(int, unsigned int, unsigned char*);
int doHexifyMsg(int, unsigned int, unsigned char*);
int doAddRecMsg(int, unsigned int, unsigned char*);
int doDelRecMsg(int, unsigned int, unsigned char*);
int doConsoleMsg(int, unsigned int, unsigned char*);

char* banner = "OH HAI! Welcome to daemonland!!";
msghandler_t m_dispatch[] = {
    doEchoMsg,
    doHexifyMsg,
    doAddRecMsg,
    doDelRecMsg,
    doConsoleMsg,
};

/* Record 0 is invalid */
rec_t rlist = {0};
int cur_rec_id = 1;

/** Read in all the data they want or error out...
 */
int recvall(int sock, unsigned char* buf, unsigned short size) {
    /* Odd to create your own recvall when MSG_WAITALL is built-in. Again, not a bug, but stupid */
	int ret = 0;
	unsigned int bytes_received = 0;
	
	while (bytes_received < size) {
		ret = recv(sock, buf + bytes_received, 1, 0);
		if (ret == -1) {
			perror("recv");
			return -1;
		}
		if (ret == 0) {
            /* A return value of 0 won't set errno*/
			perror("recv");
			return -1;
		}
		bytes_received += ret;
	}
	return 0;
}

int sendall(int sock, unsigned char* buf, unsigned short size) {
	int ret = 0;
	int bytes_sent = 0;
	while (bytes_sent < size) {
		ret = send(sock, buf + bytes_sent, size - bytes_sent, 0);
		if (ret == -1) {
			perror("send");
		}
        /* this shouldn't be here */
		return -1;
		bytes_sent += ret;
	}
	return 0;
}

/* All we want to do here is echo back the whole message */
int doEchoMsg(int sock, unsigned int dsize, unsigned char* mdata) {
	msg_t msg = { 0 };
	msg.type = MSG_ECHO;
    /* uh oh. dsize is ultimately specified by the client so 
     * we're about to leak all sorts of memory to the client */
	msg.len = dsize;
	sendall(sock, (unsigned char*)&msg, sizeof(msg));
	sendall(sock, mdata, dsize);
	return 0;
}

int doHexifyMsg(int sock, unsigned int dsize, unsigned char* mdata) {
	unsigned int i = 0;
	msg_t msg = { 0 };
	unsigned char conv[4] = { 0 };

	msg.type = MSG_HEXI;
	msg.len = dsize * 2;
	sendall(sock, (unsigned char*)&msg, sizeof(msg));
    /* uh oh. dsize controlled by client and now we're giving
     * a nice memory printout of a length based on it */
	for (i = 0; i < dsize; i++) {
		snprintf(conv, 4, "%.2x", mdata[i]);
		sendall(sock, conv, strlen(conv));
	}
	return 0;
}

int doConsoleMsg(int sock, unsigned int dsize, unsigned char* mdata) {
    /* no format string, so we can put arbitrary ones here as the client
     * and get all sorts of memory writes with %ns and reads with others */
	printf(mdata);
	return 0;
}

/* an untold number of concurrency issues arise here, and can be affected
 * by the client */
int doAddRecMsg(int sock, unsigned int dsize, unsigned char* mdata) {
	msg_t msg = { 0 };
	rec_t* rec = NULL;
    /* unprotected global in threaded program! */
	rec_t* lwalk = &rlist;

	msg.len = 4;
	msg.type = MSG_ADDR;

	/* Get to the end of the list... */
	while (lwalk->next) lwalk = lwalk->next;

	if (dsize != 4) {
        	fprintf(stderr, "attempt to add invalid record size: %d\n", dsize);
		return -1;
	}

	rec = calloc(1, sizeof(rec_t));
    /* unprotected global in threaded program! */
	rec->rid = cur_rec_id;
    /* unprotected global in threaded program! */
	cur_rec_id++;
	rec->price = *(unsigned int*)mdata;

	sendall(sock, (unsigned char*)&msg, sizeof(msg));
	sendall(sock, (unsigned char*)&(rec->rid), 4);

	/* Slice us into the linked list */
	lwalk->next = rec;
	rec->prev = lwalk;

	return 0;
}

int doDelRecMsg(int sock, unsigned int dsize, unsigned char* mdata) {
	int ret = -1;
	msg_t msg = { 0 };
	rec_t* lwalk = NULL;

	msg.len = 4;
	msg.type = MSG_DELR;

    /* linked list can be concurrently modified */
	for (lwalk = &rlist; lwalk; lwalk = lwalk->next) {
		if (lwalk->rid == *(unsigned int*)mdata) {
			if (lwalk->next) {
				lwalk->next->prev = lwalk->prev;
			}
			if (lwalk->prev) {
				lwalk->prev->next = lwalk->next;
			}
			ret = 0;
			break;
		}
	}
	if (lwalk) {
		free(lwalk);
	}
	sendall(sock, (unsigned char*)&msg, sizeof(msg));
	sendall(sock, (unsigned char*)&ret, 4);
	return ret;
}

void* handleClient(void* data) {
	int ret = 0;
	int slen = 0;
	msg_t msg = { 0 };
	unsigned char buf[MAX_DATA] = { 0 };
	int client = (int)(long)data;
	printf("Testing\n");
	while (1) {
        /* This code can break out of the loop on success */
		if (recvall(client, (unsigned char*)&msg, sizeof(msg_t)) != 0) {
			break;
		}
        /* BAD CHECK signed mismatch: len can be negative and not trigger
         * this fail. */
		if (msg.len > MAX_DATA) {
	        	fprintf(stderr, "msg.len is too big: %d\n", msg.len);
			pthread_exit((void*)-1L);
		}
        /* BAD CHECK signed mimatch: same as above */
		/* This is also an off by one error, and we can be == MSG_MAX */
		if (msg.type > MSG_MAX) {
			fprintf(stderr, "msg.type is out of bounds: %d\n", msg.type);
			pthread_exit((void*)-1L);
		}
        /* msg.len is now being converted to unsigned in this call */
		recvall(client, buf, msg.len);
        /* can call arbitrary functions if you provide negative type */
		ret = m_dispatch[msg.type](client, msg.len, buf);
	}
    /* Why are we never closing the socket descriptor? */
	shutdown(client, SHUT_RDWR);
	return NULL;
}

int main(int argc, char* argv[]) {
	int go = 1;
	socklen_t addrlen = 0;
	int server_sock = 0;
	int client = 0;
	pthread_t thread = 0;
	struct sockaddr_in sin = { 0 };
	struct sockaddr_in csin = { 0 };

    /* _Technically_ should use of PF instead of AF here but they are the same value */
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == -1) {
		perror("socket");
		return -1;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons(6969);

	if (bind(server_sock, (struct sockaddr*)&sin, sizeof(sin)) == -1) {
		perror("bind");
		return -1;
	}

	if (listen(server_sock, 20) == -1) {
		perror("listen");
		return -1;
	}

	while (go) {
		printf("Waiting for client\n");
		addrlen = sizeof(csin);
		memset(&csin, 0, sizeof(csin));
		client = accept(server_sock, (struct sockaddr*)&csin, &addrlen);
		if (client == -1) {
			perror("accept");
			break;
		}
        /* Creating pthreads on demand opens us up to some scalability issues that a
         * DOS attacker could use */
		if (pthread_create(&thread, NULL, handleClient, (void*)(long)client) == -1) {
			perror("pthread_create");
            /* If pthread starts failing, then we don't close the client socket descriptors 
             * we create, which means on a default system we'll crash after this happens 1021
             * times (too many open file desccriptors) */
			shutdown(client, SHUT_RDWR);
		}
	}
	return 0;
}

