#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>

#define PORT 8148

int main(int argc, char **argv)
{
	/* Creating TCP socket */
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Failed to create socket");
		exit(-1);
	}

	/* Creating sockaddr_in for server address */
	struct sockaddr_in servaddr = {0};
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);


	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
		perror("setsockopt failed...\n");
		exit(-1);
	}

	/* Binding socket to the port */
	if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) {
		perror("Failed to bind socket");
		exit(-1);
	}

	/* Listening for TCP connections */
	if (listen(sockfd, 10) != 0) {
		perror("Failed to listen for connections");
		exit(-1);
	}

	printf("Server listening on port %d...\n", ntohs(servaddr.sin_port));

	/* Receiving & handling the message */
	struct pollfd fds[1];
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;

	while (true) {
		int ret = poll(fds, 1, -1);
		if (ret < 0) {
			perror("poll() error");
			exit(1);
		}

		if (fds[0].revents & POLLIN) {
			struct sockaddr_in cliaddr;
			socklen_t clilen = sizeof(cliaddr);
			int connfd = accept(sockfd, (struct sockaddr *) &cliaddr, &clilen);

			char *response;

			char buf[1024];
			memset(buf, 0, sizeof(buf));
			read(connfd, buf, sizeof(buf));
			if (strstr(buf, "GET /wrong")) {
				response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nLength: 39 [text/plain]\n\n"
					   "<!DOCTYPE html>\n"
					   "<html>\n"
					   "  <body>\n"
					   "    Not found\n"
					   "  </body>\n"
					   "</html>\n";
			} else {
				response = "HTTP/1.1 200 OK\nContent-Type: text/html\nLength: 59 [text/plain]\nSaving to: ‘index.html’\n\n"
					   "<!DOCTYPE html>\n"
					   "<html>\n"
					   "  <body>\n"
					   "    Hello CS 221\n"
					   "  </body>\n"
					   "</html>\n";
			}

			write(connfd, response, strlen(response));
			close(connfd);
		}
	}

	close(sockfd);

	return 0;
}
