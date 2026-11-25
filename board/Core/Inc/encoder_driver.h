/*
 * encoder_driver.h
 *
 *  Created on: Nov 16, 2026
 *      Author: Jirka
 */

#ifndef INC_ENCODER_DRIVER_H_
#define INC_ENCODER_DRIVER_H_

#include "main.h"
#include "tx_api.h"

/**
* @brief Prepares encoder driver for operation.
*
* @retval TX_SUCCESS Driver was initialised.
*/
UINT encoder_driver_initialize();

/**
* @brief  Returns encoder position in native steps.
*
* @param[out] position		Current encoder position relative to initial position, in steps.
*
* @retval TX_SUCCESS        Value retrieved successfully.
* @retval TX_PTR_ERROR  	Passed pointer was NULL.
*/
UINT encoder_driver_get_native(uint32_t *position);

/**
* @brief  Returns encoder position in degrees.
*
* @param[out] position		Current encoder position relative to initial position, in degrees.
*
* @retval TX_SUCCESS        Value retrieved successfully.
* @retval TX_PTR_ERROR  	Passed pointer was NULL.
*/
UINT encoder_driver_get_degrees(uint32_t *position);

/**
* @brief  Sets encoder position in native steps.
*
* @param[in] position		Required encoder position, in native steps.
*
* @retval TX_SUCCESS        Value set successfully.
*/
UINT encoder_driver_set_native(uint32_t position);

/**
* @brief  Sets encoder position in degrees.
*
* @param[in] position		Required encoder position, in degrees.
*
* @retval TX_SUCCESS        Value set successfully.
*/
UINT encoder_driver_set_degrees(uint32_t position);

/**
* @brief  Resets encoder position to 0.
*
* @retval TX_SUCCESS        Value set successfully.
*/
UINT encoder_driver_home();


#endif /* INC_ENCODER_DRIVER_H_ */
