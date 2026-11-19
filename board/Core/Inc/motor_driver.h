/**
* @file   motor_driver.h
*
* @brief  Driver of motor controlled through PWM on TIM2.
*/

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "tx_api.h"

#define MOTOR_PWM_MAX 500


/**
* @brief Prepares motor driver for operation.
*
* @retval TX_SUCCESS Driver was initialized.
*/
UINT motor_driver_initialize(VOID);

/**
* @brief  Sets PWM duty and direction of motor.
*
* Value is clamped by +-MOTOR_PWM_MAX.
*
* @param[in] pwm  Required PWM value limited by +-MOTOR_PWM_MAX.
*
* @retval TX_SUCCESS        Value has been set.
* @retval TX_NOT_AVAILABLE  Driver was not initialized.
*/
UINT motor_driver_output(INT pwm);

/**
* @brief  Obtain current PWM value of motor and pass it into *pwm.
*
* @param[out] pwm  Current pwm value of motor is put in.
*
* @retval TX_NOT_AVAILABLE  Driver was not initialized.
* @retval TX_PTR_ERROR      pwm is NULL.
* @retval TX_SUCCESS        Current pwm was successfully returned.
*/
UINT motor_driver_status(INT *pwm);

/**
* @brief Stop motor and terminates driver.
*
* @retval TX_SUCCESS Driver was terminated.
*/
UINT motor_driver_terminate(VOID);

/**
* @brief  Moves the motor in both directions to test everything is working.
*
* Runs for 3 seconds.
* Can be called only from thread.
*/
VOID motor_driver_self_test(VOID);

#endif
