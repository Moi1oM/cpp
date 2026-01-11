#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 5

int main() {
    int epoll_fd, nfds;
    struct epoll_event ev, events[MAX_EVENTS];

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN; 
    ev.data.fd = STDIN_FILENO;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        perror("epoll_ctl: stdin");
        exit(EXIT_FAILURE);
    }

    printf("터미널에 아무 글자나 입력하고 Enter를 누르세요 (종료: Ctrl+C)\n");

    while (1) {
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == STDIN_FILENO) {
                char buf[256];
                ssize_t bytes = read(STDIN_FILENO, buf, sizeof(buf) - 1);
                buf[bytes] = '\0';
                printf(">> epoll 감지! 입력된 내용: %s", buf);
            }
        }
    }

    close(epoll_fd);
    return 0;
}