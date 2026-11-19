#include "motor_driver.h"
#include "main.h" /* to get TIM2 from HAL */

static INT motor_pwm;
static UINT motor_initialized;


UINT motor_driver_initialize(VOID)
{
    TIM2->CCR3 = 0;
    TIM2->CCR4 = 0;
    motor_pwm = 0;
    motor_initialized = TX_TRUE;

    return TX_SUCCESS;
}

UINT motor_driver_output(INT pwm)
{
    TX_INTERRUPT_SAVE_AREA

    if (motor_initialized != TX_TRUE)
    {
        return TX_NOT_AVAILABLE;
    }

    pwm = pwm > MOTOR_PWM_MAX ? MOTOR_PWM_MAX : pwm;
    pwm = pwm < -MOTOR_PWM_MAX ? -MOTOR_PWM_MAX : pwm;

    TX_DISABLE
    if (pwm >= 0)
    {
        TIM2->CCR3 = 0;
        TIM2->CCR4 = pwm;
    }
    else
    {
        TIM2->CCR3 = -pwm;
        TIM2->CCR4 = 0;
    }

    motor_pwm = pwm;
    TX_RESTORE

    return TX_SUCCESS;
}

UINT motor_driver_status(INT *pwm)
{
    if (motor_initialized != TX_TRUE)
    {
        return TX_NOT_AVAILABLE;
    }

    if (pwm == NULL)
    {
        return TX_PTR_ERROR;
    }

    *pwm = motor_pwm;

    return TX_SUCCESS;
}

UINT motor_driver_terminate(VOID)
{
    TIM2->CCR3 = 0;
    TIM2->CCR4 = 0;
    motor_pwm = 0;
    motor_initialized = TX_FALSE;

    return TX_SUCCESS;
}

VOID motor_driver_self_test(VOID)
{
    printf("Motor driver self test\n");
    printf("Rotating to positive side\n");
    motor_driver_output(100);
    tx_thread_sleep(150); // 1,5s

    printf("Rotating to negative side\n");
    motor_driver_output(-100);
    tx_thread_sleep(150); // 1,5s
    motor_driver_output(0);
    printf("Motor driver self test finished\n");
}

