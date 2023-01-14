#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "project05.h"
//hello
int main(int argc, char **argv){
	struct pollfd my_fds[MAXPOLLS];
	bool eof = false;
	int num_polls = 0;
	int num_readable = 0;
	char* username = argv[1];
	char* portNum = argv[2];
	struct user_t user;
	struct user_t ru;
	struct user_t rv;
	strcpy(user.name, username);
	strcpy(user.port, portNum);
	int i = 0;
	int n;
	int count = 0;
	my_fds[0].fd = STDIN_FILENO;
	my_fds[0].events = POLLIN;
	my_fds[0].revents = 0;
	num_polls++;
	my_fds[1].fd = socket_presence();
	my_fds[1].events = POLLIN;
	my_fds[1].revents = 0;
	num_polls++;
	my_fds[2].fd = socket_tcp(user.port);
	my_fds[2].events = POLLIN;
	my_fds[2].revents = 0;
	num_polls++;
	struct users User;
	 
	int count2 = 0;
	int b = 3;
	int m = 0;
	while(!eof){
		num_readable = poll(my_fds, num_polls, TIMEOUT);

		if(num_readable == -1){
			perror("poll failed");
			exit(-1);
		} else if(num_readable > 0){
			if(my_fds[0].revents & POLLIN){
				int z = write_msg(n, m, User);
				if(z == -1){
					eof = true;
				}
				}
			}
			if(my_fds[1].revents & POLLIN){
				presence_read(my_fds[1].fd, &ru);
				count2 = users_update(&User, ru, count2);
			}
			if(my_fds[2].revents & POLLIN){
				my_fds[b].fd = accept(my_fds[2].fd, NULL, NULL);
				if(my_fds[b].fd == -1){
					error();
				}
				my_fds[b].events = POLLIN;
				my_fds[b].revents = 0;
				num_polls++;
				chat_read(my_fds[b].fd, &User, &rv);
				b++;
			}
		i++;
		count++;
		if(count >= 60){
			presence_write(my_fds[1].fd, "online", user.name, user.port);
			count = 0;
		}
	}
	presence_write(my_fds[1].fd, "offline", user.name, user.port);
	for(int i =  1; i < num_polls; i++){
		int fd = my_fds[i].fd;
		if(fd != -1){
			close(fd);
		}
		//close(fd);
	}
	
}
