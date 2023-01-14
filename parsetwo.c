
#include "project05.h"

int write_msg(int n, int m, struct users User){
	char ch;
	char buf[64];
	int b = 0;
	while((n = read(STDIN_FILENO, &ch, 1)) >= 0){
		if(n == 0){
			m = -1;
			break;
		}
		buf[b++] = ch;
		if(ch == '\n'){
			break;
		}
	}
	if(m == -1){
		return m;
	}
	buf[b] = '\0';
	char name[64];
	char msg[64];
	int track = 0;
	int index = 0;
	int index2 = 0;
	b = 0;
	int msgstart = 1000;
	while(buf[b] != '\n'){
		if(track == 0){
			b++;
			track++;
			continue;
		}
		if(buf[b] == ':'){
			name[index++] = '\0';
			msgstart = b+2;
			
		}
		if(b >= msgstart){
			msg[index2++] = buf[b];
		}
		name[index++] = buf[b];
		track++;
		b++;
	}
	msg[index2] = '\0';
	chat_write(name, User, msg);
	return m;
}
int socket_presence(){
	struct addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *results;
	int e = getaddrinfo(NULL, "8221", &hints, &results);
	if(e != 0){
		printf("getaddrinfo: %s\n", gai_strerror(e));
		exit(-1);
	}

	struct addrinfo *r;
	int fd;
	for(r = results; r != NULL; r = r->ai_next){
		fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if(fd != -1){
			break;
		}
	}
	int enable = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable)) != 0){
		error();
	}
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) != 0){
		error();
	}
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) != 0){
		error();
	}
	if(bind(fd, r->ai_addr, r->ai_addrlen) < 0){
		error();
	}
	freeaddrinfo(results);
	return fd;	
}

int socket_tcp(char* port){
	struct addrinfo hints;
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;

	struct addrinfo *r;
	int e = getaddrinfo(NULL, port , &hints, &r);
	if(e != 0){
		printf("getaddrinfo: %s\n", gai_strerror(e));
		exit(-1);
	}

	
	int fd  = socket(r->ai_family, r->ai_socktype, r->ai_protocol); 
	if(fd == -1){
		error();
	}
	int enable = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) != 0){
		error();
	}
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) != 0){
		error();
	}
	if(ioctl(fd, FIONBIO, (char*) &enable, SOCK_NONBLOCK) < 0){
		error();
	}
	if(bind(fd, r->ai_addr, r->ai_addrlen) < 0){
		error();
	}
	if(listen(fd, MAXPOLLS) < 0){
		error();
	}
	freeaddrinfo(r);
	return fd;	
}
void error(){
	perror("socket failed");
	exit(-1);
}
void errortwo(){
	perror("read failed");
	exit(-1);
}
void presence_read(int fd, struct user_t *u){
	struct sockaddr_storage stg;
	char buf[64];
	socklen_t stg_len = sizeof(stg);
	if(recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&stg, &stg_len) == -1){
		errortwo();
	}
	char service[64];
	int rc = getnameinfo((struct sockaddr *)&stg, stg_len, u->host, sizeof(u->host), service, NI_MAXSERV, NI_NUMERICSERV);
	if(rc != 0){
		errortwo();
	}
	sscanf(buf, "%s %s %s", u->name, u->status, u->port);
	
}
void chat_read(int fd, struct users *user, struct user_t *u){
	struct sockaddr_storage peer;
	char buf[64];
	socklen_t peer_len = sizeof(peer);
	int m = recv(fd, buf, sizeof(buf),0);
	if(m  == -1){
		errortwo();
	}else if(m == 0){
		printf("Connection close");
		exit(-1);
	}
		
	char service[NI_MAXSERV];
	int rc = getpeername(fd, (struct sockaddr *)&peer, &peer_len);
	if(rc != 0){
		errortwo();
	}
	int rc2 = getnameinfo((struct sockaddr *)&peer, peer_len, u->host, sizeof(u->host), service, NI_MAXSERV, NI_NUMERICSERV);
	if(rc2 != 0){
		errortwo();
	}
	struct user_t t;
	for(int i = 0; i < user->count; i++){
		if(strcmp(u->host, user->arr[i].host) == 0){
			t = user->arr[i];
			break;
		}
	}
	printf("%s says: %s\n", t.name, buf);
}

int users_update(struct users *user, struct user_t u, int b){
	char* name = u.name;
	char* status = u.status;
	int found = 0;
	for(int i = 0; i < b; i++){
		struct user_t t = user->arr[i];
		char* name2 = t.name;
		if(!strcmp(name, name2)){
			found = 1;
			char* status2 = user->arr[i].status;
			if(strcmp(status, status2) != 0){
				user->arr[i] = u;
				printf("%s %s %s\n", u.name, u.status, u.port);
				break;
			}
		}	
	}
	if(found == 0){
		user->arr[b++] = u;
		user->count = b;
		printf("%s %s %s\n", u.name, u.status, u.port);
	}
	return b;
	
} 

void presence_write(int fd, char* status, char* name, char* port){
	char buf[64];
	snprintf(buf, 64, "%s %s %s", name, status, port);
	struct sockaddr_in addr;
	inet_pton(AF_INET, "10.10.13.255", &addr.sin_addr);
	addr.sin_port = htons(8221);
	addr.sin_family = AF_INET;
	socklen_t n = sizeof(addr);
	int m = sizeof(buf);
	int z = sendto(fd, buf, m, 0, (struct sockaddr *)&addr, n);	
}
void chat_write(char* name, struct users user, char* msg){
	struct addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	int found = 0;
	struct user_t u;
	for(int i = 0; i < user.count; i++){
		if(!strcmp(name, user.arr[i].name)){
			found = 1;
			u = user.arr[i];
			break;
		}
	}
	if(found == 0){
		printf("unkown user\n");
	}else{
		struct addrinfo *results;
		int e = getaddrinfo(u.host, u.port, &hints, &results);
		if(e != 0){
			printf("getaddrinfo: %s\n", gai_strerror(e));
			exit(-1);
		}
		struct addrinfo *r;
		int fd;
		for(r = results; r != NULL; r = r->ai_next){
			fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
			if(fd == -1){
				error();
			}
			if(connect(fd, r->ai_addr, r->ai_addrlen) != -1){
				break;
			}
		}
		send(fd, msg, strlen(msg), 0);
	}
	
}
