#include "udp_link.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

/* Heading is sent as ASCII text, e.g. "-30" or "725".*/
#define UDP_LINK_MESSAGE_SIZE  12

extern NX_IP           NetXDuoEthIpInstance;
extern NX_PACKET_POOL  NxAppPool;

static NX_UDP_SOCKET  udp_link_socket;
static UINT           udp_link_initialized;

UINT udp_link_initialize(VOID)
{
    UINT ret;

    if (udp_link_initialized == TX_TRUE)
    {
        return NX_SUCCESS;
    }

    /* Queue depth 1: when a new datagram arrives, the older one is dropped,
       so the socket always holds only the latest heading. */
    ret = nx_udp_socket_create(&NetXDuoEthIpInstance, &udp_link_socket, "UDP link socket",
                               NX_IP_MIN_DELAY, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 1);
    if (ret != NX_SUCCESS)
    {
        return ret;
    }

    /* No wait: an occupied port is reported at once instead of blocking the caller forever. */
    ret = nx_udp_socket_bind(&udp_link_socket, UDP_LINK_PORT, NX_NO_WAIT);
    if (ret != NX_SUCCESS)
    {
        nx_udp_socket_delete(&udp_link_socket);
        return ret;
    }

    udp_link_initialized = TX_TRUE;

    return NX_SUCCESS;
}

UINT udp_link_send_heading(INT heading)
{
    UINT       ret;
    NX_PACKET *packet;
    CHAR       message[UDP_LINK_MESSAGE_SIZE];
    int        byte_length;

    if (udp_link_initialized != TX_TRUE)
    {
        return NX_NOT_CREATED;
    }

    byte_length = snprintf(message, sizeof(message), "%d", (int) heading);

    ret = nx_packet_allocate(&NxAppPool, &packet, NX_UDP_PACKET, NX_NO_WAIT);
    if (ret != NX_SUCCESS)
    {
        return ret;
    }

    ret = nx_packet_data_append(packet, message, (ULONG) byte_length, &NxAppPool, NX_NO_WAIT);
    if (ret != NX_SUCCESS)
    {
        /* The packet is still ours, give it back to the pool. */
        nx_packet_release(packet);
        return ret;
    }

    ret = nx_udp_socket_send(&udp_link_socket, packet, UDP_LINK_TARGET_IP_ADDRESS, UDP_LINK_PORT);
    if (ret != NX_SUCCESS)
    {
        /* Send failed, so NetX did not take the packet, release it ourselves. */
        nx_packet_release(packet);
        return ret;
    }

    /* Success: the packet now belongs to NetX, do not touch or release it. */

    printf("sending angle %d\n", (int) heading);

    return NX_SUCCESS;
}

UINT udp_link_receive_heading(INT *heading)
{
    UINT       ret;
    NX_PACKET *packet;
    CHAR       message[UDP_LINK_MESSAGE_SIZE];
    ULONG      length;
    CHAR      *end;
    long       value;

    if (heading == NX_NULL)
    {
        return NX_PTR_ERROR;
    }

    if (udp_link_initialized != TX_TRUE)
    {
        return NX_NOT_CREATED;
    }

    ret = nx_udp_socket_receive(&udp_link_socket, &packet, NX_NO_WAIT);
    if (ret != NX_SUCCESS)
    {
        return ret;
    }

    /* message that does not fit the buffer (with room for '\0') cannot be a valid heading. */
    if (packet->nx_packet_length >= sizeof(message))
    {
        nx_packet_release(packet);
        return NX_INVALID_PACKET;
    }

    /* Copies at most sizeof(message) - 1 bytes, so the buffer cannot overflow. */
    ret = nx_packet_data_extract_offset(packet, 0, message, sizeof(message) - 1, &length);

    nx_packet_release(packet);

    if (ret != NX_SUCCESS)
    {
        return ret;
    }

    message[length] = '\0';

    /* text to number */
    errno = 0;
    value = strtol(message, &end, 10);

    /* Reject: no digits at all (end did not move) or out of INT range. */
    if ((end == message) || (errno == ERANGE))
    {
        return NX_INVALID_PACKET;
    }

    /* Reject garbage after the number, e.g. "72x". */
    if (*end != '\0')
    {
        return NX_INVALID_PACKET;
    }

    *heading = (INT) value;

    printf("received angle %d\n", (int) value);

    return NX_SUCCESS;
}
