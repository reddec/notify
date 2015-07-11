#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "notify.h"

int main(int argc, char **argv) {
    if (argc > 1) {
        char title[__NOTIFY_MAX_TITLE_LEN];
        char format[__NOTIFY_MAX_FORMAT_LEN];
        char data[__NOTIFY_MAX_DATA_LEN];
        std::cout << "Format: ";
        std::cin >> format;
        std::cout << "Title: ";
        std::cin >> title;
        std::cout << "Data: ";
        std::cin >> data;
        if (data[0] == '-')data[0] = '\0';
        int sender = socket(AF_INET, SOCK_DGRAM, 0);
        std::clog << "Sent: " << notify_text(sender, format, title, data) << " bytes";
        close(sender);

    } else {
        char buffer[__NOTIFY_PACKET_LIMIT];
        ssize_t pack_size;
        int listener = socket(AF_INET, SOCK_DGRAM, 0);
        notify_setup_listener(listener);
        while ((pack_size = notify_collect(listener, 5000, buffer, sizeof(buffer))) > 0) {
            std::cout << "Packet size: " << pack_size << std::endl;
            notify_packet_t pp = notify_parse_packet(buffer, pack_size);
            std::cout << "Format: " << (pp.format != NULL ? pp.format : "<no format>") << std::endl;
            std::cout << "Title: " << (pp.title != NULL ? pp.title : "<no title>") << std::endl;
            std::cout << "Data (" << pp.data_size << " bytes): ";
            std::cout.write(pp.data, pp.data_size);
            std::cout << std::endl;
        }
        std::cout << "Errno: " << errno << std::endl;
        close(listener);
    }
    return 0;
}