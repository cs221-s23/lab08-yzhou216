#include "lab08.h"

extern struct pollfd fds[MAX_CLI];
extern int cur_fd;

void send_response(int sockfd, const char *status, const char *content_type, const char *body)
{
	char response[4096];
	snprintf(response, sizeof(response),
		"HTTP/1.1 %s\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %d\r\n"
		"\r\n"
		"%s",
		status, content_type, strlen(body), body);
	send(sockfd, response, strlen(response), 0);
}

void async_accept(int sockfd, int connfd, struct sockaddr_in *cli, int len)
{
	while (true) {
		int s = poll(fds, cur_fd, -1);
		if (s < 0) {
			perror("Polling error");
			break;
		}

		char buf[MSG_BUF + 1];
		memset(buf, 0, MSG_BUF + 1);
		for (int i = 0; i < cur_fd; i++) {
			if (fds[i].revents & POLLIN && fds[i].fd == sockfd) {
				connfd = accept(sockfd, (struct sockaddr *)cli, &len);
				if (connfd < 0) {
					printf("Server accept failed...\n");
					exit(-1);
				}
				fds[cur_fd].fd = connfd;
				fds[cur_fd].events = POLLIN;
				cur_fd++;
			} else {
				int n = recv(fds[i].fd, buf, sizeof(buf) - 1, 0);
				if (n <= 0) {
					close(fds[i].fd);
					fds[i] = fds[cur_fd - 1];
					cur_fd--;
					continue;
				}
			}

			char method[10];
			char uri[1024];
			char protocol[10];
			sscanf(buf, "%s %s %s", method, uri, protocol);
			if (!strcmp(method, "GET") && !strcmp(uri, "/")) {
				const char *body = "<!DOCTYPE html>\n<html>\n<body>\nHello CS 221\n</body>\n</html>\n";
				send_response(fds[i].fd, "200 OK", "text/html", body);
			}

			/*
			close(fds[i].fd);
			fds[i] = fds[cur_fd - 1];
			cur_fd--;
			*/
		}

		close(connfd);
	}
}
