/**
 * @file   control.h
 * @brief  Control thread: the sender board reads the encoder and sends its
 *         angle, the receiver board receives the requested angle.
 *         The role is selected by IS_SENDER_BOARD in main.h.
 */

#ifndef CONTROL_H
#define CONTROL_H

#include "tx_api.h"

/**
 * @brief  Entry function of the control thread.
 *
 * Initializes the UDP link (and the encoder on the sender board), then runs
 * forever. Create it from App_ThreadX_Init() with tx_thread_create().
 *
 * @param[in] input  Not used.
 */
VOID control_thread_entry(ULONG input);

#endif
