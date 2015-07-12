#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <notify.h>
#include "../src/notify.h"

int main(int argc, char **argv) {
    if (argc > 1) {
        //! [Send data from console]
        char title[__NOTIFY_MAX_TITLE_LEN];                        // Allocate buffer for `title` field
        char format[__NOTIFY_MAX_FORMAT_LEN];                      // Allocate buffer for `format` field
        char data[__NOTIFY_MAX_DATA_LEN];                          // Allocate buffer for `payload
        std::cout << "Format: ";
        std::cin >> format;                                        // Read `format` from stdin (console input)
        std::cout << "Title: ";
        std::cin >> title;                                         // Read `title` from stdin (console input)
        std::cout << "Data: ";
        std::cin >> data;                                          // Read payload from stdin (console input)
        int sender = socket(AF_INET, SOCK_DGRAM, 0);               // Create UDP socket
        ssize_t bytes = notify_text(sender, format, title, data);  // Send notification (as text)
        std::clog << "Sent " << bytes << " bytes" << std::endl;
        close(sender);                                             // Close socket
        //! [Send data from console]
    } else {
        //! [Receive data]
        char buffer[__NOTIFY_PACKET_LIMIT];                                                 // Allocate buffer for packet
        ssize_t pack_size;
        size_t tm_ms = 5000;                                                                // Set timeout (5 seconds)
        int listener = socket(AF_INET, SOCK_DGRAM, 0);                                      // Create UDP socket
        notify_setup_listener(listener);                                                    // Prepare socket
        while ((pack_size = notify_collect(listener, tm_ms, buffer, sizeof(buffer))) > 0) { // Read one packet
            std::cout << "Packet size: " << pack_size << std::endl;
            notify_packet_t pp = notify_parse_packet(buffer, pack_size);                    // Parse packet to structure
            std::cout << "Format: ";
            if (pp.format != NULL)                                                          // Check field `format`
                std::cout << pp.format << std::endl;
            else
                std::cout << "<no format field>" << std::endl;
            std::cout << "Title: ";
            if (pp.title != NULL)                                                           // Check field `title`
                std::cout << pp.title << std::endl;
            else
                std::cout << "<no title field>" << std::endl;
            std::cout << "Data (" << pp.data_size << " bytes): ";
            std::cout.write(pp.data, pp.data_size);
            std::cout << std::endl;
        }
        close(listener);                                                                    // Close socket
        //! [Receive data]
    }
    return 0;
}