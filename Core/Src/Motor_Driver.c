/*
 * Motor_Driver.c
 *
 *  Created on: 2022/02/15
 *      Author: leopi
 */

#include "Motor_Driver.h"
//A1,12

////
void Motor_PWM_Start(){ // モータPWMの開始とCCR値の
#if 1
  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK)
  {
	  Error_Handler();
  }
  if (HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1) != HAL_OK)
  {
	  Error_Handler();
  }
  if (HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1) != HAL_OK)
  {
	  Error_Handler();
  }
#endif
}

void Motor_PWM_Stop(){ // モータPWMの開始とCCR値の設
#if 1
  if (HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1) != HAL_OK)
  {
	  Error_Handler();
  }
  if (HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_1) != HAL_OK)
  {
	  Error_Handler();
  }
  if (HAL_TIM_PWM_Stop(&htim10, TIM_CHANNEL_1) != HAL_OK)
  {
	  Error_Handler();
  }
#endif
}
inline void Motor_Switch(int left, int right){
	if (left > 0 ){
		//to -
		HAL_GPIO_WritePin(GPIO_LEFT, GPIO_L_PIN_NUM, GPIO_PIN_SET); //A2が左SET:1で正転

	}
	else  if (left < 0){
		//to +
		HAL_GPIO_WritePin(GPIO_LEFT, GPIO_L_PIN_NUM, GPIO_PIN_RESET); //A2が左,RESET:0で転
		left = -left;
	}
	if (right > 0){
		//to -
		HAL_GPIO_WritePin(GPIO_RIGHT, GPIO_R_PIN_NUM, GPIO_PIN_SET); //A0が右,RESET:0で転

	}

	else if (right < 0){
	  	//to +
	  	HAL_GPIO_WritePin(GPIO_RIGHT, GPIO_R_PIN_NUM, GPIO_PIN_RESET); //A0が右,SET:1で正転
	  	right = -right;
	}

	//上限はマクロで設定
	if(left > 2000*0.8) left = 2000*0.8;
	if(right > 2000*0.8) right = 2000*0.8;

	  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, left); //tim2ch1が左
	  __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, right); //tim5ch1が右
}
