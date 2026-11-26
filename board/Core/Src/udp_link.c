#include "udp_link.h"
#include <stdio.h>

/* Angle is sent as the raw bytes of an INT (4 bytes). */
#define UDP_LINK_MESSAGE_SIZE  sizeof(INT)

extern NX_IP           NetXDuoEthIpInstance;
extern NX_PACKET_POOL  NxAppPool;

static NX_UDP_SOCKET  udp_link_socket;
static UINT           udp_link_initialized = TX_FALSE;

UINT udp_link_initialize(VOID)
{
    UINT ret;

    if (udp_link_initialized == TX_TRUE)
    {
        return NX_SUCCESS;
    }

    /* Queue depth 1: when a new datagram arrives, the older one is dropped,
       so the socket always holds only the latest angle. */
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

UINT udp_link_send_angle(INT angle)
{
    UINT       ret;
    NX_PACKET *packet;

    if (udp_link_initialized != TX_TRUE)
    {
        return NX_NOT_CREATED;
    }

    ret = nx_packet_allocate(&NxAppPool, &packet, NX_UDP_PACKET, NX_NO_WAIT);
    if (ret != NX_SUCCESS)
    {
        return ret;
    }

    /* Copy the bytes of the variable as they lie in memory. */
    ret = nx_packet_data_append(packet, &angle, UDP_LINK_MESSAGE_SIZE, &NxAppPool, NX_NO_WAIT);
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

    printf("sending angle %d\n", (int) angle);

    return NX_SUCCESS;
}

UINT udp_link_receive_angle(INT *angle)
{
    UINT       ret;
    NX_PACKET *packet;
    ULONG      length;
    INT        value;

    if (angle == NX_NULL)
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

    if (packet->nx_packet_length != UDP_LINK_MESSAGE_SIZE)
    {
        nx_packet_release(packet);
        return NX_INVALID_PACKET;
    }

    ret = nx_packet_data_extract_offset(packet, 0, &value, sizeof(value), &length);

    nx_packet_release(packet);

    if (ret != NX_SUCCESS)
    {
        return ret;
    }

    if (length != UDP_LINK_MESSAGE_SIZE)
    {
        return NX_INVALID_PACKET;
    }

    *angle = value;

    printf("received angle %d\n", (int) value);

    return NX_SUCCESS;
}
