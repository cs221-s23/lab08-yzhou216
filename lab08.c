#include "lab08.h"

struct pollfd fds[MAX_CLI];
int cur_fd = 0;

int main(int argc, char **argv)
{
	int sockfd;
	int connfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Socket creation failed...\n");
		exit(-1);
	}

	memset(fds, 0, sizeof(fds));
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;
	cur_fd = 1;

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
		printf("setsockopt failed...\n");
		exit(-1);
	}

	if ((bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))) {
		printf("Socket bind failed...\n");
		exit(-1);
	}

	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(-1);
	}

	async_accept(sockfd, connfd, &cli, sizeof(cli));

	close(sockfd);

	return 0;
}
