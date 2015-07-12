#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <notify.h>
#include "../src/notify.h"

int main(int argc, char **argv) {
    if (argc > 1) {
        //! [Send data from console]
        // Allocate buffer for `title` field
        char title[__NOTIFY_MAX_TITLE_LEN];
        // Allocate buffer for `format` field
        char format[__NOTIFY_MAX_FORMAT_LEN];
        // Allocate buffer for `payload
        char data[__NOTIFY_MAX_DATA_LEN];
        std::cout << "Format: ";
        // Read `format` from stdin (console input)
        std::cin >> format;
        std::cout << "Title: ";
        // Read `title` from stdin (console input)
        std::cin >> title;
        std::cout << "Data: ";
        // Read payload from stdin (console input)
        std::cin >> data;
        // Create UDP socket
        int sender = socket(AF_INET, SOCK_DGRAM, 0);
        // Send notification (as text)
        ssize_t bytes = notify_text(sender, format, title, data);
        std::clog << "Sent " << bytes << " bytes" << std::endl;
        // Close socket
        close(sender);
        //! [Send data from console]
    } else {
        //! [Receive data]
        // Allocate buffer for packet
        char buffer[__NOTIFY_PACKET_LIMIT];
        ssize_t pack_size;
        // Set timeout (5 seconds)
        size_t tm_ms = 5000;
        // Create UDP socket
        int listener = socket(AF_INET, SOCK_DGRAM, 0);
        // Prepare socket
        notify_setup_listener(listener);
        // Read one by one packet till timeout exception or internal socket error
        while ((pack_size = notify_collect(listener, tm_ms, buffer, sizeof(buffer))) > 0) {
            std::cout << "Packet size: " << pack_size << std::endl;
            // Parse packet to structure
            notify_packet_t pp = notify_parse_packet(buffer, pack_size);
            std::cout << "Format: ";
            // Check field `format`. If packet not contains this filed (or it has incorrect end symbol), it will be NULL
            if (pp.format != NULL)
                std::cout << pp.format << std::endl;
            else
                std::cout << "<no format field>" << std::endl;
            std::cout << "Title: ";
            // Check field `title`. If packet not contains this filed (or it has incorrect end symbol), it will be NULL
            if (pp.title != NULL)
                std::cout << pp.title << std::endl;
            else
                std::cout << "<no title field>" << std::endl;
            std::cout << "Data (" << pp.data_size << " bytes): ";
            std::cout.write(pp.data, pp.data_size);
            std::cout << std::endl;
        }
        // Close socket
        close(listener);
        //! [Receive data]
    }
    return 0;
}