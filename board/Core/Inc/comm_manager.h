/**
 * @file   comm_manager.h
 * @brief  Communication manager thread: the sender board reads the encoder and sends its
 *         angle, the receiver board receives the requested angle.
 *         The role is selected by IS_SENDER_BOARD in main.h.
 */

#ifndef COMM_MANAGER_H
#define COMM_MANAGER_H

#include "tx_api.h"

/**
 * @brief  Entry function of the communication manager thread.
 *
 * Initializes the UDP link (and the encoder on the sender board), then runs
 * forever. Create it from App_ThreadX_Init() with tx_thread_create().
 *
 * @param[in] input  Not used.
 */
VOID comm_manager_thread_entry(ULONG input);

#endif
