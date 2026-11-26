/*
 * encoder_driver.c
 *
 *  Created on: Nov 16, 2026
 *      Author: Jirka
 */

#include "encoder_driver.h"

#define ENCODER_STEPS 256

UINT encoder_driver_initialize()
{
	return TX_SUCCESS;
}

UINT encoder_driver_get_native(uint32_t *position)
{
	if (position == NULL)
	{
		return TX_PTR_ERROR;
	}

	*position = TIM1->CNT;
	return TX_SUCCESS;
}

UINT encoder_driver_get_degrees(uint32_t *position)
{
	UINT ret;
	uint32_t pos_native;

	if (position == NULL)
	{
		return TX_PTR_ERROR;
	}

	ret = encoder_driver_get_native(&pos_native);
	*position = pos_native * 360 / ENCODER_STEPS;
	return ret;
}

UINT encoder_driver_set_native(uint32_t position)
{
	TIM1->CNT = position;
	return TX_SUCCESS;
}

UINT encoder_driver_set_degrees(uint32_t position)
{
	UINT ret;
	uint32_t pos_native = position * ENCODER_STEPS / 360;
	ret = encoder_driver_set_native(pos_native);
	return ret;
}

UINT encoder_driver_home()
{
	TIM1->CNT = 0;
	return TX_SUCCESS;
}
