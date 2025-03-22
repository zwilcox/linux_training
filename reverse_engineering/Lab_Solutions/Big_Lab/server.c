#include "openssl/md5.h"
#include "openssl/rc4.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#define PORT		9876
#define ALIVE_RESP	"Alive\n"
#define PROMPT		"CMD > "
#define FILETOSEND	"./helpfile.txt"
#define SECRETFILE	"./secretfile.txt"
#define SLEEPTIME	3000
#define CERTFILE	"./cert.pem"
#define KEYFILE		"./key.pem"
#define GROOMSZ		0x4548

typedef enum _commands {
	QUIT = 0,
	ENABLE,		// enable tls and getting files
	SET_PASSCODE,	// set the passcode
	GET_FILE,	// get the file
	STARTTLS,	// upgrade this socket to a TLS socket, and expect to get an admin password (heartbleed here)
	ALIVE,		// send back "Alive"
	UNKNOWN,
} command_t;

typedef struct _client {
	int cl;				// socket fd
	struct sockaddr_in6 src;	// source addr
} client_t;

// Global
char* secrets = NULL;
size_t secret_sz = 0;

SSL_CTX* create_ctx() {
	const SSL_METHOD* method;
	SSL_CTX* ctx = NULL;
	
	method = SSLv23_server_method();
	ctx = SSL_CTX_new(method);
	if (ctx == NULL) {
		fprintf(stderr, "Unable to create SSL context\n");
		ERR_print_errors_fp(stderr);
		goto ERR_END;
	}

	SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

	if (SSL_CTX_use_certificate_file(ctx, CERTFILE, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		goto ERR_FREE_END;
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, KEYFILE, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		goto ERR_FREE_END;
	}

	return ctx;

ERR_FREE_END:
	SSL_CTX_free(ctx);
ERR_END:
	return NULL;
}

void memfill(char* src, size_t ssz, char* dst, size_t dsz) {
	char* c = dst;
	while ((c+ssz) < (dst + dsz)) {
		memcpy(c, src, ssz);
		c += ssz;
	}
}

int init_secret() {
	int fd;
	int err = -1;

	fd = open(SECRETFILE, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Unable to open file containing the secrets\n");
		goto END;
	}

	secret_sz = lseek(fd, 0, SEEK_END);
	if (secret_sz == (size_t)-1) {
		perror("lseek");
		goto END;
	}

	if (lseek(fd, 0, SEEK_SET) == -1) {
		perror("lseek");
		goto END;
	}

	secrets = malloc(secret_sz);
	if (read(fd, secrets, secret_sz) == -1) {
		perror("read");
		goto END;
	}

	err = 0;
END:
	return err;
}

int do_tls(int cl) {
	// a stack string to hide it a bit more
	char secret[] = {'g', 'u', 'r', ' ', 'f', 'h', 'e', 'r', 'f', 'g', ' ', 'j', 'n', 'l', ' ', 'g', 'b', ' ', 'x', 'r', 'r', 'c', ' ', 'n', ' ', 'f', 'r', 'p', 'e', 'r', 'g', ' ', 'v', 'f', ' ', 'g', 'b', ' ', 'z', 'n', 'x', 'r', ' ', 'f', 'b', 'z', 'r', 'b', 'a', 'r', ' ', 'g', 'u', 'v', 'a', 'x', ' ', 'g', 'u', 'r', 'l', ' ', 'n', 'y', 'e', 'r', 'n', 'q', 'l', ' ', 'x', 'a', 'b', 'j', ' ', 'g', 'u', 'r', ' ', 'n', 'a', 'f', 'j', 'r', 'e'};
	SSL_CTX* ctx;
	SSL* ssl;
	int err = -1;
	int amt;
	int i;
	char* t0;
	char* t1;
	
	// create the context
	ctx = create_ctx();
	if (ctx == NULL) {
		goto END;
	}

	// Do start TLS
	ssl = SSL_new(ctx);
	if (ssl == NULL) {
		fprintf(stderr, "Unable to create new SSL\n");
		ERR_print_errors_fp(stderr);
		goto END_CTX;
	}

	if (SSL_set_fd(ssl, cl) != 1) {
		fprintf(stderr, "Unable to set fd for STARTTLS\n");
		ERR_print_errors_fp(stderr);
		goto END_SSL;
	}

	// A little bit of grooming to allow heartbleed to show us something
	//TODO I would like a better way to get our secrets leaked
	// ideally without modifying OpenSSL
	// I tried doing some grooming in another thread, but it was very spotty
	// I am okay with some non-determinism though, that is good for them to see
	t0 = malloc(GROOMSZ);
	t1 = malloc(GROOMSZ);
	memfill(secrets, secret_sz, t1, GROOMSZ);
	free(t0);

	if (SSL_accept(ssl) <= 0) {
		fprintf(stderr, "Problem during SSL_accept\n");
		ERR_print_errors_fp(stderr);
		goto END_FREE;
	}
	
	// send secret rot13d message
	// Just an easteregg, quote from Dune, not actually important 
	
	// we send it with a bit of delay between the letters, because calling SSL_write
	// will yeild to SSL so it can handle heartbeat messages
	
	for (i = 0; i < (int)sizeof(secret); i++) {
		usleep(SLEEPTIME);
		amt = SSL_write(ssl, &secret[i], 1);
		if (amt < 0) {
			fprintf(stderr, "unable to SSL_write\n");
			ERR_print_errors_fp(stderr);
			goto END_FREE;
		}
		if (amt == 0) {
			fprintf(stderr, "Client exited early\n");
			goto END_FREE;
		}
	}

	err = 0; // success

END_FREE:
	free(t1);
END_SSL:
	SSL_free(ssl);
END_CTX:
	SSL_CTX_free(ctx);
END:
	return err;
}

int send_all(int cl, const char* buf, size_t sz) {
	int amt = 0;
	size_t sent = 0;
	while (sent < sz) {
		amt = send(cl, buf+sent, sz-sent, MSG_NOSIGNAL);
		if (amt == -1) {
			perror("send");
			return -1;
		}
		sent += amt;
	}
	return sz;
}

int recv_line(int cl, char* buf, size_t sz) {
	size_t amt = 0;
	int err = 0;
	int ended = 0;
	while (amt < sz) {
		err = recv(cl, buf+amt, 1, 0);
		if (err == -1) {
			perror("recv");
			return -1;
		}
		if (err == 0) {
			errno = ENOTCONN;
			return -1;
		}

		if (*(buf+amt) == '\n') {
			*(buf+amt) = '\0';
			ended = 1;
			break;
		}

		amt += err;
	}

	if (!ended) {
		// buf too small
		errno = EMSGSIZE;
		return -1;
	}

	return amt;
}

int encrypt_file(unsigned char* fbuf, size_t sz, int passcode, unsigned int* seedp, unsigned char** encbuf) {
	char rnd = 0;
	MD5_CTX md5ctx;
	unsigned char digest[MD5_DIGEST_LENGTH] = {0};
	RC4_KEY key;

	// get key via md5 of a random number and a passcode (they will have to brute force it, or see that we use a fixed seed)
	// (if this has been called before, they have to keep calling rand_r() with the same seed to find a key)
	
	rnd = (char)rand_r(seedp);
	if (MD5_Init(&md5ctx) == 0) {
		// error, can't init md5
		fprintf(stderr, "MD5_Init: Unable to init\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}

	if (MD5_Update(&md5ctx, &passcode, sizeof(passcode)) == 0) {
		// error, can't update dm5
		fprintf(stderr, "MD5_Update: Unable to update\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}

	if (MD5_Update(&md5ctx, &rnd, sizeof(rnd)) == 0) {
		fprintf(stderr, "MD5_Update: Unable to update\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}

	if (MD5_Final(digest, &md5ctx) == 0) {
		fprintf(stderr, "MD5_Final: Unable to finish digest\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}

	// encrypt it
	RC4_set_key(&key, sizeof(digest), digest);

	// first allocate the output buffer
	(*encbuf) = malloc(sz);
	if ((*encbuf) == NULL) {
		fprintf(stderr, "Unable to allocate buffer for RC4 encryption\n");
		perror("malloc");
		return -1;
	}
	
	RC4(&key, sz, fbuf, *encbuf);

	return 0;
}

int send_file(int cl, int passcode, unsigned int* seedp) {
	int fd = -1;
	size_t sz = 0;
	ssize_t amt = 0;
	unsigned char* fbuf = NULL;
	int err = -1;
	unsigned char* encbuf = NULL;
	char lenbuf[0x20] = {0};

	fd = open(FILETOSEND, O_RDONLY);
	if (fd == -1) {
		perror("open");
		goto END;
	}

	sz = (size_t)lseek(fd, 0, SEEK_END);
	if (sz == (size_t)-1) {
		perror("lseek");
		goto END;
	}
	if (lseek(fd, 0, SEEK_SET) == -1) {
		perror("lseek");
		goto END;
	}

	if (sz == 0) {
		errno = EINVAL;
		goto END;
	}

	// allocate buffer for file
	fbuf = malloc(sz);
	if (fbuf == NULL) {
		perror("malloc");
		goto END;
	}

	// read it in
	
	amt = read(fd, fbuf, sz);
	if (amt == -1) {
		perror("read");
		goto END_FREE;
	}

	if (encrypt_file(fbuf, sz, passcode, seedp, &encbuf) == -1) {
		goto END_FREE_BOTH;
	}

	// send out length of file
	// TO make this easier, you can also output the random number used with the key here
	// but it shouldn't be needed, because we have a static seed
	amt = snprintf(lenbuf, sizeof(lenbuf)-1, "sz:%lu\n", sz);
	if (amt < 0) {
		perror("snprintf");
		goto END_FREE_BOTH;
	}

	if (send_all(cl, lenbuf, strlen(lenbuf)) == -1) {
		goto END_FREE_BOTH;
	}

	// then send out the file

	if (send_all(cl, (char*)encbuf, sz) == -1) {
		goto END_FREE_BOTH;
	}

	err = 0; // success

END_FREE_BOTH:
	free(encbuf);
END_FREE:
	free(fbuf);
END:
	return err;
}

command_t get_command(int cl, int* opt) {
	char buf[0x100] = {0};
	int amt = 0;
	int cmd = 0;
	// send prompt
	if (send_all(cl, PROMPT, sizeof(PROMPT)-1) == -1) {
		return QUIT;
	}

	// recv command
	amt = recv_line(cl, buf, sizeof(buf)-1);
	if (amt == -1) {
		perror("recv_line");
		return QUIT;
	}

	// parse command
	amt = sscanf(buf, "%d %d", &cmd, opt);
	if (amt < 1) {
		return UNKNOWN;
	}

	return (command_t)cmd;
}

void* handle_client(void* arg) {
	client_t* cli = (client_t*) arg;
	int enabled = 0;
	int passcode = -1;
	command_t cmd;
	int opt;
	unsigned int seed = 0x44454553;
	char saddr[INET6_ADDRSTRLEN+1] = {0};

	fprintf(
		stderr,
		"Got connection from: %s %d\n",
		inet_ntop(AF_INET6, &(cli->src.sin6_addr), saddr, sizeof(saddr)),
		cli->src.sin6_port
	);
	
	// get commands in a loop
	while (1) {
		opt = 0;
		cmd = get_command(cli->cl, &opt);

		switch (cmd) {
		case ALIVE:
			if (send_all(cli->cl, ALIVE_RESP, sizeof(ALIVE_RESP)-1) == -1) {
				perror("send");
				goto END;
			}	
			break;
		case SET_PASSCODE:
			if (opt != 0) {
				passcode = opt;
			}
			break;
		case ENABLE:
			if (opt == passcode) {
				enabled = 1;
			}
			break;
		case GET_FILE:
			if (enabled) {
				send_file(cli->cl, passcode, &seed);
			}
			break;
		case STARTTLS:
			do_tls(cli->cl);
			goto END;
		case QUIT:
			goto END;
		case UNKNOWN:
		default:
			break;	
		}
	}
	
END:
	fprintf(stderr, "Session finished with: %s %d\n", saddr, cli->src.sin6_port);

	close(cli->cl);
	free(cli);
	return NULL;
}

int do_init() {
	int err = -1;
	struct sigaction sa = { 0 };

	// initialize the secret
	if (init_secret() != 0) {
		fprintf(stderr, "Unable to init secret\n");
		goto END;
	}

	// ignore sigpipe
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &sa, NULL) == -1) {
		perror("sigaction");
		goto END;
	}

	// inintialize OpenSSL
	SSL_load_error_strings();
	ERR_load_crypto_strings();
	OpenSSL_add_all_ciphers();
	if (OpenSSL_add_ssl_algorithms() != 1) {
		fprintf(stderr, "Unable to init SSL\n");
		goto END;
	}

	err = 0;
END:
	return err;
}

int main() {
	int sk;
	int cl;
	int err = 0;
	pthread_t recent = 0;
	struct sockaddr_in6 addr = {0};
	client_t* cli = NULL;
	pthread_attr_t attr;
	socklen_t len;

	if (do_init() != 0) {
		fprintf(stderr, "Unable to initialize\n");
		goto END;
	}

	sk = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (sk == -1) {
		perror("socket");
		goto END;
	}
	
	addr.sin6_family = AF_INET6;
	addr.sin6_addr = in6addr_any;
	addr.sin6_port = htons(PORT);

	// bind to the port
	if (setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
		perror("setsockopt");
		goto END_CLOSE;
	}

	if (bind(sk, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind");
		goto END_CLOSE;
	}

	// listen for connections
	if (listen(sk, 3) == -1) {
		perror("listen");
		goto END_CLOSE;
	}

	// accept connections and pop off threads
	err = pthread_attr_init(&attr);
	if (err != 0) {
		errno = err;
		perror("pthread_attr_init");
		goto END_CLOSE;
	}

	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (err != 0) {
		errno = err;
		perror("pthread_attr_setdetachstate");
		goto END_CLOSE_CLEAN;
	}

	while (1) {
		cli = calloc(1, sizeof(client_t));
		if (cli == NULL) {
			perror("calloc");
			break;
		}

		len = sizeof(cli->src);
		cl = accept(sk, (struct sockaddr*) &(cli->src), &len);
		if (cl == -1) {
			perror("accept");
			break;
		}

		cli->cl = cl;

		err = pthread_create(&recent, &attr, handle_client, (void*)cli);
		if (err != 0) {
			free(cli);
			errno = err;
			perror("pthread_create");
			break;
		}
		cli = NULL; // thread is in charge of freeing that
	}
	
END_CLOSE_CLEAN:
	pthread_attr_destroy(&attr);

END_CLOSE:
	close(sk);
END:
	// cleanup openssl
	EVP_cleanup();

	printf("Done\n");
	return 0;
}

