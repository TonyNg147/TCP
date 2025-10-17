#include <arpa/inet.h>
#include <cstdlib>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#ifdef __linux__
#include <sys/epoll.h>
#elif __APPLE__
#include <sys/event.h>
#include <sys/time.h>
#endif
#include <sys/socket.h>
#include <unistd.h>
// TODO: split function to more readable and debug
int main() {
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd == -1) {
		fprintf(stderr, "Error when instantiating socket client %s\n",
				strerror(errno));
		std::exit(EXIT_FAILURE);
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(9000);

	if (connect(client_fd, (sockaddr *)&addr, sizeof(sockaddr)) == -1) {
		fprintf(stderr, "Error when connecting to server\n");
		std::exit(EXIT_FAILURE);
	}

	int assigned_id = -1;
	read(client_fd, (void *)&assigned_id, sizeof(int));

	fprintf(stdout, "[Client-%d]: Connect to server successfully\n",
			assigned_id);

#ifdef __linux__
	int epoll_instance_fd = epoll_create1(0);

	if (epoll_instance_fd = epoll_create1(0); epoll_instance_fd == -1) {
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}

	epoll_event interested_fd, ready_fds[2];
	interested_fd.events = EPOLLIN;
	interested_fd.data.fd = client_fd;
	if (epoll_ctl(epoll_instance_fd, EPOLL_CTL_ADD, client_fd,
				  &interested_fd) == -1) {
		perror("epoll_ctl: listen_sock");
		exit(EXIT_FAILURE);
	}

	interested_fd.events = EPOLLIN;
	interested_fd.data.fd = STDIN_FILENO;
	if (epoll_ctl(epoll_instance_fd, EPOLL_CTL_ADD, STDIN_FILENO,
				  &interested_fd) == -1) {
		perror("EPOLL ADD STDIN FAILED");
		exit(EXIT_FAILURE);
	}

	while (true) {
		int number_of_ready = epoll_wait(epoll_instance_fd, ready_fds, 2, -1);
		if (number_of_ready > 0) {
			for (int i = 0; i < number_of_ready; ++i) {
				if (ready_fds[i].data.fd == STDIN_FILENO) {
					char buf[4096];
					int n = read(STDIN_FILENO, buf, 4096);
					buf[n] = '\0';
					if (buf[n - 1] == '\n')
						buf[n - 1] = '\0';
					write(client_fd, buf, n);

					printf("\x1b[1A");	 // Move cursor up 1 line
					printf("\x1b[2K\r"); // Clear the entire line
					fflush(stdout);
					fprintf(stdout, "[Client-%d]: %s\n", assigned_id, buf);
					fflush(stdout);

				} else {
					char buf[4096];
					int n = read(ready_fds[i].data.fd, buf, 4096);
					if (n > 0) {
						buf[n] = '\0';
						fprintf(stdout, "%s\n", buf);
						fflush(stdout);
					}
				}
			}
		}
	}

	close(client_fd);
#elif __APPLE__
	int kq = kqueue();
	if (kq == -1) {
		perror("kqueue");
		exit(EXIT_FAILURE);
	}

	struct kevent changeList[2];
	EV_SET(&changeList[0], client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
		   NULL);
	EV_SET(&changeList[1], STDIN_FILENO, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
		   NULL);
	kevent(kq, changeList, 2, NULL, 0, NULL);

	struct kevent eventList[2];
	while (true) {
		int nev = kevent(kq, NULL, 0, eventList, 2, NULL);
		if (nev == -1) {
			perror("kevent wait");
			break;
		}

		for (int i = 0; i < nev; ++i) {
			int fd = (int)eventList[i].ident;

			if (fd == STDIN_FILENO) {
				char buf[4096];
				int n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
				if (n <= 0)
					continue;
				buf[n] = '\0';
				if (buf[n - 1] == '\n')
					buf[n - 1] = '\0';

				write(client_fd, buf, n);

				printf("\x1b[1A");
				printf("\x1b[2K\r");
				fflush(stdout);
				fprintf(stdout, "[Client-%d]: %s\n", assigned_id, buf);
				fflush(stdout);
			} else if (fd == client_fd) {
				char buf[4096];
				int n = read(client_fd, buf, sizeof(buf) - 1);
				if (n > 0) {
					buf[n] = '\0';
					fprintf(stdout, "%s\n", buf);
					fflush(stdout);
				} else {
					fprintf(stderr, "Server closed connection.\n");
					close(client_fd);
					close(kq);
					return 0;
				}
			}
		}
	}
	close(kq);
#endif
	close(client_fd);
	return 0;
}