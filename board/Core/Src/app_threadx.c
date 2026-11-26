/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "motor_driver.h"
#include "comm_manager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TRACEX_BUFFER_SIZE		64000
#define TEST_MOTOR_DRIVER_STACK_SIZE 4096
#define TEST_MOTOR_DRIVER_PRIORITY 10
#define COMM_MANAGER_THREAD_STACK_SIZE	4096
#define COMM_MANAGER_THREAD_PRIORITY		5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t tracex_buffer[TRACEX_BUFFER_SIZE];

TX_THREAD test_motor_driver_thread;
TX_THREAD comm_manager_thread;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void test_motor_driver(ULONG init);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */
  TX_BYTE_POOL *bytePool = (TX_BYTE_POOL *) memory_ptr;
  VOID *test_motor_pointer;
  VOID *comm_manager_pointer;

  ret = tx_byte_allocate(bytePool, &test_motor_pointer, TEST_MOTOR_DRIVER_STACK_SIZE, TX_NO_WAIT);

  if (ret != TX_SUCCESS)
  {
    return ret;
  }

  // Test motor thread
  ret = tx_thread_create(&test_motor_driver_thread, "Motor driver thread test", test_motor_driver, 0, 
    test_motor_pointer, TEST_MOTOR_DRIVER_STACK_SIZE, TEST_MOTOR_DRIVER_PRIORITY, TEST_MOTOR_DRIVER_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return ret;
  }

  // stack allocation for comm manager thread
  ret = tx_byte_allocate(bytePool, &comm_manager_pointer, COMM_MANAGER_THREAD_STACK_SIZE, TX_NO_WAIT);

  if (ret != TX_SUCCESS)
  {
    return ret;
  }

  // Comm manager thread create
  ret = tx_thread_create(&comm_manager_thread, "Comm manager thread", comm_manager_thread_entry, 0,
    comm_manager_pointer, COMM_MANAGER_THREAD_STACK_SIZE, COMM_MANAGER_THREAD_PRIORITY, COMM_MANAGER_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return ret;
  }

  /* USER CODE END App_ThreadX_MEM_POOL */

  /* USER CODE BEGIN App_ThreadX_Init */
  tx_trace_enable(&tracex_buffer, TRACEX_BUFFER_SIZE, 30);

  motor_driver_initialize();
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */

void test_motor_driver(ULONG init)
{
  motor_driver_self_test();
}

/* USER CODE END 1 */
