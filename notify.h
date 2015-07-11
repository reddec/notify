/*!
\file notify.h
\brief Notification on multicast UDP communication
\author RedDec <net.dev@mail.ru>
*/

#ifndef NOTTINGHAM_NOTIFY_H
#define NOTTINGHAM_NOTIFY_H

#ifdef __cplusplus
    extern "C" {
#endif
#define __NOTIFY_MAX_FORMAT_LEN 255
#define __NOTIFY_MAX_TITLE_LEN 255
#define __NOTIFY_PACKET_LIMIT 65500
#define __NOTIFY_MAX_DATA_LEN __NOTIFY_PACKET_LIMIT - __NOTIFY_MAX_FORMAT_LEN - 1 - __NOTIFY_MAX_TITLE_LEN - 1
#define __NOTIFY_MCAST_ADDR 3774347255 ///< IP: 224.247.247.247
#define __NOTIFY_MCAST_PORT 24724
#define __NOTIFY_FIELD_DELIMITER '\0'

#include <stdint.h>
#include <stdlib.h>

/// Return codes for each functions
enum notify_code {
    notify_error_common = -1,      ///< Common error. May be changed to more detailed code in future releases
    notify_error_allocation = -2,  ///< Can't allocate required memory
    notify_error_bind = -3,        ///< Can't bind UDP socket
    notify_error_join = -4,        ///< Can't join to multicast group
    notify_error_bad_socket = -5,  ///< Can't use socket. Usually it was closed or created incorrectly
    notify_error_timeout = -6      ///< Nothing received till specified time interval
};

static const size_t notify_max_format_len = __NOTIFY_MAX_FORMAT_LEN;     ///< Maximum size in bytes of `format` field
static const size_t notify_max_title_len = __NOTIFY_MAX_TITLE_LEN;       ///< Maximum size in bytes of `title` field
static const size_t notify_max_data_len = __NOTIFY_MAX_DATA_LEN;         ///< Maximum size in bytes of payload
static const size_t notify_max_packet_size = __NOTIFY_PACKET_LIMIT;      ///< Maximum size in bytes of notification packet
static const uint32_t notify_mcast_address = __NOTIFY_MCAST_ADDR;        ///< Multicast address for communication
static const uint16_t notify_mcast_port = __NOTIFY_MCAST_PORT;           ///< Port of multicast address for communication
static const uint8_t notify_field_delimiter = __NOTIFY_FIELD_DELIMITER;  ///< Delimiter between fields in packet
static const char notify_format_tcp_service[] = "service.tcp\0";         ///< Predefined `format` for TCP service
static const char notify_format_udp_service[] = "service.udp\0";         ///< Predefined `format` for UDP service

/// Container of packet. Contains only pointers to parts in external buffer
typedef struct notify_packet_t {
    const char *format;  ///< First element of `format` field. May be NULL
    const char *title;   ///< First element of `title` field. May be NULL
    const char *data;    ///< First element of payload. May be NULL
    size_t data_size;    ///< Payload size. May be 0. Can't be more then `notify_max_data_len`
} notify_packet_t;

/**
\brief Send notification packet
\param socket_fd  UDP socket descriptor
\param packet     notification packet
\return #notify_error_bad_socket, #notify_error_allocation, -1 (on socket error) or sent bytes
 */
ssize_t notify_packet(int socket_fd, notify_packet_t packet);

/**
\brief Send notification with raw data. Wraps it to ::notify_packet_t and sends
\param socket_fd  UDP socket descriptor
\param format     Packet format type. Can be used as small description of payload format
\param title      Head of packet content
\param data`      Payload content
\param data_size  Payload content size. Can't be more then #notify_max_data_len
\return  same as ::notify_packet
 **/
ssize_t notify_data(int socket_fd,
                    const char *format,
                    const char *title,
                    const void *data,
                    size_t data_size);

/**
\brief Send notification with textual data.

 Wraps it to ::notify_packet_t and sends.
 Content size determinated by strnlen() and can't be more then #notify_max_data_len

\param socket_fd  UDP socket descriptor
\param format     Packet format type. Can be used as small description of payload format
\param title      Head of packet content
\param text       Payload text content
\return  same as ::notify_packet
 */
ssize_t notify_text(int socket_fd,
                    const char *format,
                    const char *title,
                    const char *text);

/**
\brief Send notification of TCP service.

Wraps it to ::notify_packet_t with format #notify_format_tcp_service and sends.

\param socket_fd      UDP socket descriptor
\param service_name   TCP logical service name
\param port           TCP listening service port
\return same as ::notify_packet
*/
ssize_t notify_local_tcp_service(int socket_fd, const char *service_name, uint16_t port);

/**
\brief Send notification of UDP service.

Wraps it to `notify_packet_t` with format #notify_format_udp_service and sends.

\param socket_fd     UDP socket descriptor
\param service_name  UDP logical service name
\param port          UDP listening service port
\return - same as ::notify_packet
*/
ssize_t notify_local_udp_service(int socket_fd, const char *service_name, uint16_t port);

/**
\brief Setup UDP socket as multicast listener: bind, sets REUSE_ADDR and joins to multicast group.
\param socket_fd    UDP socket descriptor
\return  #notify_error_bad_socket, #notify_error_bind, #notify_error_join otherwise 0
 */
ssize_t notify_setup_listener(int socket_fd);

/**
\brief Collect first notification message till specified interval
\param socket_fd     UDP socket descriptor
\param interval_ms   Time interval in milliseconds. If nothing received `notify_error_timeout` will be returned
\param buffer        Buffer for incoming packet. More then `notify_max_packet_size` is not required
\param buf_size      Maximum buffer size for incoming packet
\return - #notify_error_bad_socket, #notify_error_common, #notify_error_timeout, otherwise - size of received packet
*/
ssize_t notify_collect(int socket_fd, size_t interval_ms, char *buffer, size_t buf_size);

/**
\brief Parse notification message to ::notify_packet_t structure.
\param buffer  packet raw data
\param size    size of packet
\return notification structure. If some fields can't be parsed, they will be filled by NULL
*/
notify_packet_t notify_parse_packet(const char *buffer, size_t size);


#ifdef __cplusplus
    }
#endif
#endif //NOTTINGHAM_NOTIFY_H
