/**
 * @file   udp_link.h
 * @brief  UDP link between the sender and the receiver board.
 *
 * The sender board sends its heading, the receiver board receives it as the
 * requested heading. Both boards use UDP_LINK_PORT; the role, IP and MAC
 * address are selected by IS_SENDER_BOARD in main.h.
 *
 * Message format: the heading as ASCII text in whole degrees, signed and not
 * clamped to 0..360.
 *
 * Only one thread may use the link, the functions are not protected by a lock.
 */

#ifndef UDP_LINK_H
#define UDP_LINK_H

#include "main.h"
#include "nx_api.h"

#ifndef IS_SENDER_BOARD
#error "IS_SENDER_BOARD is not defined, set it in main.h (1 = sender, 0 = receiver)"
#elif (IS_SENDER_BOARD != 0) && (IS_SENDER_BOARD != 1)
#error "IS_SENDER_BOARD must be 0 or 1"
#endif

#if IS_SENDER_BOARD
#define UDP_LINK_SOURCE_MAC           0x00
#define UDP_LINK_SOURCE_IP_ADDRESS    IP_ADDRESS(192, 168, 1, 1)
#define UDP_LINK_TARGET_IP_ADDRESS    IP_ADDRESS(192, 168, 1, 2)
#else
#define UDP_LINK_SOURCE_MAC           0x01
#define UDP_LINK_SOURCE_IP_ADDRESS    IP_ADDRESS(192, 168, 1, 2)
#define UDP_LINK_TARGET_IP_ADDRESS    IP_ADDRESS(192, 168, 1, 1)
#endif

#define UDP_LINK_PORT 5000

/**
 * @brief  Creates the UDP socket and binds it to UDP_LINK_PORT.
 *
 * Must be called from a thread, after NetX Duo is initialized
 * (MX_NetXDuo_Init). Calling it again when the link is ready does nothing.
 *
 * @retval NX_SUCCESS           The link is ready.
 * @retval NX_PORT_UNAVAILABLE  UDP_LINK_PORT is already bound by another socket.
 * @retval other                Error code from nx_udp_socket_create or
 *                              nx_udp_socket_bind; the link stays uninitialized
 *                              and the function may be called again.
 */
UINT udp_link_initialize(VOID);

/**
 * @brief  Sends the heading to the other board (UDP_LINK_TARGET_IP_ADDRESS).
 *
 * Does not block: when no packet is free in the pool, it fails at once.
 * Delivery is not guaranteed (UDP), the caller is never told about a lost message.
 *
 * @param[in] heading  Heading in whole degrees, signed, not clamped.
 *
 * @retval NX_SUCCESS      The message was handed to NetX Duo for sending.
 * @retval NX_NOT_CREATED  The link is not initialized.
 * @retval NX_NO_PACKET    No free packet in the pool.
 * @retval other           Error code from NetX Duo.
 */
UINT udp_link_send_heading(INT heading);

/**
 * @brief  Takes the latest received heading, if there is one.
 *
 * Does not block. The socket keeps only the newest message, older ones are
 * dropped. On any return value other than NX_SUCCESS, *heading is left unchanged,
 * so the caller keeps the last valid heading.
 *
 * @param[out] heading  Received heading in whole degrees, signed, not clamped.
 *
 * @retval NX_SUCCESS         A new heading was written to *heading.
 * @retval NX_NO_PACKET       Nothing new has arrived. This is the normal case,
 *                            not an error.
 * @retval NX_INVALID_PACKET  The message was not a valid number (wrong format,
 *                            too long or out of the INT range); it was dropped.
 * @retval NX_PTR_ERROR       heading is NULL.
 * @retval NX_NOT_CREATED     The link is not initialized.
 * @retval other              Error code from NetX Duo.
 */
UINT udp_link_receive_heading(INT *heading);

#endif /* UDP_LINK_H */
