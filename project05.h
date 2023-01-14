#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>


#define MAXPOLLS 50
#define TIMEOUT 1000
#define BUF_SIZE 128
/*struct polls{
	struct pollfd polls[MAX_POLLS];
	int num_polls;
};*/
struct user_t{
	char host[128];
	char name[128];
	char status[128];
	char port[128];
};
struct users{
	struct user_t arr[65];
	int count;
};

//void add_users(struct users *user, struct user_t u, int b);
int users_update(struct users *user, struct user_t u, int b);
int socket_presence();
int socket_tcp(char* port);
void error();
void errortwo();
int write_msg(int n, int m, struct users User);
void chat_write(char* name, struct users user, char* msg);
void chat_read(int fd, struct users *user, struct user_t *u);
void presence_read(int fd, struct user_t *u);
void presence_write(int sfd, char *status, char *name, char *port);
