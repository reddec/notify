//
// Created by reddec on 11.07.15.
//

#include "notify.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

ssize_t notify_packet(int socket_fd, notify_packet_t pack) {
    static struct sockaddr_in destination = {
            .sin_addr.s_addr = 0,
            .sin_port=0,
            .sin_family=AF_INET
    };
    if (destination.sin_addr.s_addr == 0) {
        destination.sin_port = htons(__NOTIFY_MCAST_PORT);
        destination.sin_addr.s_addr = htonl(__NOTIFY_MCAST_ADDR);
    }
    if (socket_fd < 0)
        return notify_error_bad_socket;
    size_t f_sz = strnlen(pack.format, notify_max_format_len);
    size_t t_sz = strnlen(pack.title, notify_max_title_len);
    size_t data_size = pack.data_size > notify_max_data_len ? notify_max_data_len : pack.data_size;
    size_t packet_size = f_sz + 1 + t_sz + 1 + data_size;
    char *packet = (char *) malloc(packet_size);
    if (packet == NULL)
        return notify_error_allocation;
    memcpy(packet, pack.format, f_sz);
    packet[f_sz] = notify_field_delimiter;
    memcpy(&packet[f_sz + 1], pack.title, t_sz);
    packet[f_sz + 1 + t_sz] = notify_field_delimiter;
    memcpy(&packet[f_sz + 1 + t_sz + 1], pack.data, data_size);
    ssize_t response = sendto(socket_fd, packet, packet_size, 0, (const struct sockaddr *) &destination,
                              sizeof(destination));

    free(packet);
    return response;

}

ssize_t notify_data(int socket_fd, const char *format, const char *title, const void *data, size_t data_size) {
    notify_packet_t pack = {
            .format=format,
            .title=title,
            .data=data,
            .data_size=data_size
    };
    return notify_packet(socket_fd, pack);
}

ssize_t notify_text(int socket_fd, const char *format, const char *title, const char *text) {
    return notify_data(socket_fd, format, title, text, strnlen(text, notify_max_data_len));
}

ssize_t notify_local_tcp_service(int socket_fd, const char *service_name, uint16_t port) {
    char buf[7];
    sprintf(buf, "%u", port);
    return notify_text(socket_fd, notify_format_tcp_service, service_name, buf);
}

ssize_t notify_local_udp_service(int socket_fd, const char *service_name, uint16_t port) {
    char buf[7];
    sprintf(buf, "%u", port);
    return notify_text(socket_fd, notify_format_udp_service, service_name, buf);
}

ssize_t notify_setup_listener(int sock) {
    if (sock < 0)
        return notify_error_bad_socket;
    int set_option_on = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &set_option_on, sizeof(set_option_on));

    struct sockaddr_in addr;

    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(__NOTIFY_MCAST_PORT);
    addr.sin_family = AF_INET;
    if (bind(sock, (const struct sockaddr *) &addr, sizeof(addr)) == -1) {
        close(sock);
        return notify_error_bind;
    }
    //Join to multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = htonl(__NOTIFY_MCAST_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        close(sock);
        return notify_error_join;
    }
    return 0;
}


ssize_t notify_collect(int socket_fd, size_t interval_ms, char *buffer, size_t buf_size) {
    if (socket_fd < 0) return notify_error_bad_socket;
    struct timeval tv;
    tv.tv_sec = interval_ms / 1000;
    tv.tv_usec = (interval_ms % 1000) * 1000;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        return notify_error_common;
    }
    ssize_t sz = recv(socket_fd, buffer, buf_size, 0);
    if (sz == EAGAIN) {
        return notify_error_timeout;
    }
    return sz;
}

notify_packet_t notify_parse_packet(const char *buffer, size_t size) {
    notify_packet_t result = {
            .format=NULL,
            .title=NULL,
            .data=NULL,
            .data_size=0
    };
    size_t offset = strnlen(buffer, size) + 1;
    if (offset < size) {
        result.format = buffer;
        size_t title_off = offset;
        offset = offset + strnlen(&buffer[offset], size - offset) + 1;
        if (offset < size) {
            result.title = &buffer[title_off];
            result.data = &buffer[offset];
            result.data_size = size - offset;
        } else if (buffer[size - 1] == '\0') {
            result.title = &buffer[title_off];
        }
    }
    return result;
}
