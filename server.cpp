#include <algorithm>
#include <cstdlib>
#include <vector>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

/*The handler to hanlde the detected FD with its assoicated data*/
typedef void (*FDHandler)(const void *const);

struct FDTracker {
	int fd;
	FDHandler handler;
};

struct ClientTracker : FDTracker {
	ClientTracker(int fd, int id) : FDTracker{fd}, id{id} {}
	int id;
};
#include <sys/socket.h>
int server_fd;
struct sigaction sa, old_sa;
int epoll_instance_fd;
int NUMBER_OF_CLIENT = 0;
std::vector<ClientTracker> client_fds = {20, ClientTracker{-1, 0}};

void handle_sigterm(int sig) {
	printf("Received SIG %d\n", sig);
	if (sig == SIGTERM || sig == SIGINT) {
		close(server_fd);
	}
	old_sa.sa_handler(sig);
}

void process_client_message(const void *const tracker);

void handle_new_connection(const void *const tracker) {
	const FDTracker *const fdTracker = (FDTracker *)tracker;
	sockaddr_in client_addr;

	socklen_t size_of_addr = sizeof(sockaddr_in);

	int new_client_fd = -1;

	if (new_client_fd =
			accept(server_fd, (sockaddr *)&client_addr, &size_of_addr);
		new_client_fd == -1) {
		fprintf(stderr, "Error when accept new client %s\n", strerror(errno));
		std::exit(EXIT_FAILURE);
	}

	if (size_of_addr != sizeof(sockaddr_in)) {
		fprintf(stderr, "Something went wrong\n");
	}

	int current_id = NUMBER_OF_CLIENT++;
	client_fds[current_id].id = current_id;
	client_fds[current_id].fd = new_client_fd;
	client_fds[current_id].handler = &process_client_message;

	epoll_event ev;

	ev.events = EPOLLIN;
	ev.data.ptr = (void *)&client_fds[current_id];
	if (epoll_ctl(epoll_instance_fd, EPOLL_CTL_ADD, new_client_fd, &ev) == -1) {
		perror("Add newly connected clients");
		exit(EXIT_FAILURE);
	}

	write(new_client_fd, (void *)&current_id, sizeof(int));

	fprintf(
		stdout,
		"New connection from address %s with port %d With FD %d, current ID "
		"%d.\n",
		inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),
		new_client_fd, current_id);
}

void process_client_message(const void *const tracker) {
	const ClientTracker *const client = (ClientTracker *)tracker;
	char buf[4096];

	int n = read(client->fd, buf, 4096);
	if (n == 0) {
		printf("Client disconnect\n");
		if (epoll_ctl(epoll_instance_fd, EPOLL_CTL_DEL, client->fd, NULL) ==
			-1) {
			perror("Remove the disconnected client");
			exit(EXIT_FAILURE);
		}
		auto it = std::remove_if(
			client_fds.begin(), client_fds.end(),
			[needed_fd = client->fd](const ClientTracker &client) {
				return client.fd == needed_fd;
			});
		client_fds.erase(it, client_fds.cend());
	} else {
		if (client != nullptr) {
			char newly_formed_buf[8192];
			snprintf(newly_formed_buf, 8192, "[Client-%d] %s", client->id, buf);
			std::for_each(client_fds.cbegin(), client_fds.cend(),
						  [needed_fd = client->fd,
						   &newly_formed_buf](const ClientTracker &client) {
							  if (client.fd != needed_fd && client.fd != -1) {
								  write(client.fd, newly_formed_buf,
										strlen(newly_formed_buf));
							  }
						  });
		}
	}
}

int main() {
	// Need to handle SIGINT or SIGTERM to gracefully terminate the socket
	// Otherwise, It won't be closed till the time kernel schedules.
	sa.sa_handler = handle_sigterm;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGTERM, &sa, &old_sa);
	sigaction(SIGINT, &sa, &old_sa);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		fprintf(stderr, "Error when instantiating socket server %s\n",
				strerror(errno));
		std::exit(EXIT_FAILURE);
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(9000);

	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		fprintf(stderr, "Error when assigning information to a socket %s\n",
				strerror(errno));
		std::exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 10) == -1) {
		fprintf(stderr, "Error when listening socket server %s\n",
				strerror(errno));
		std::exit(EXIT_FAILURE);
	}

	epoll_instance_fd = epoll_create1(0);

	if (epoll_instance_fd = epoll_create1(0); epoll_instance_fd == -1) {
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}

	epoll_event interested_fd;
	std::vector<epoll_event> ready_fds(20);

	FDTracker new_conection{server_fd, &handle_new_connection};

	interested_fd.events = EPOLLIN;
	interested_fd.data.ptr = &new_conection;
	if (epoll_ctl(epoll_instance_fd, EPOLL_CTL_ADD, server_fd,
				  &interested_fd) == -1) {
		perror("epoll_ctl: listen_sock");
		exit(EXIT_FAILURE);
	}

	while (true) {
		int number_of_ready =
			epoll_wait(epoll_instance_fd, ready_fds.data(), 20, -1);
		if (number_of_ready > 0) {
			for (int i = 0; i < number_of_ready; ++i) {
				((FDTracker *const)ready_fds[i].data.ptr)
					->handler(ready_fds[i].data.ptr);
			}
		}
	}
}