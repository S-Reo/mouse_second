/*
 * Motor_Driver.h
 *
 *  Created on: Jun 20, 2022
 *      Author: leopi
 */

#ifndef INC_MOTOR_DRIVER_H_
#define INC_MOTOR_DRIVER_H_

/*
 * Motor_Driver.h
 *
 *  Created on: 2022/02/15
 *      Author: leopi
 */

#include <main.h>
#include <stdint.h>
//モータを制御するのに必要となるハードウェアは、モタドラ、モタドラへの出力を行うピンの設定。
//GPIO設定
//正転逆転は実機で確認して適宜入れ替える
#define GPIO_LEFT				GPIOA
#define GPIO_L_PIN_NUM		GPIO_PIN_12

#define GPIO_RIGHT				GPIOA
#define GPIO_R_PIN_NUM		GPIO_PIN_1

//PWMタイマ設定
extern TIM_HandleTypeDef htim2;//左
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim10;


void Motor_PWM_Start();
void Motor_PWM_Stop();

//出力反転処理してそのまま出力
void Motor_Switch(int left, int right);

#endif /* INC_MOTOR_DRIVER_H_ */
