//
// Created by Owen on 2022/1/14.
//

#include "led.h"

#define Shift_Bit 1

/**
 * @brief 控制LED的进程
 * @param ledODR
 * @param direction
 */

void LED_Process(uint8_t *ledODR, uint8_t direction)
{
    LED_Displacement(ledODR, direction);
    //LED_Toggle(8);
    LED_Unlock();
    //LED_Display(ledODR);
}

/**
 * @brief 控制左位移右位移，direction为0时左移，为1时右移
 * @param ledODR
 * @param direction
 */
void LED_Displacement(uint8_t *ledODR, uint8_t direction)
{
    //HAL_Delay(500);

    if (direction == 1)
    {
        *ledODR = (*ledODR >> Shift_Bit) | (*ledODR << (8 - Shift_Bit));      //右平移
    }
    else
    {
        *ledODR = (*ledODR << Shift_Bit) | (*ledODR >> (8 - Shift_Bit));    //左平移
    }
    GPIOC->ODR = ~*ledODR << 8;         //写GPIOC ODR寄存器 输出GPIOC 8~15
}

/**
 * @brief 写GPIO C ODR寄存器 输出GPIO C 8~15
 * @param ledODR
 */
void LED_Display(uint8_t *ledODR)
{
    GPIOC->ODR = ~*ledODR << 8;         //写GPIOC ODR寄存器 输出GPIOC 8~15
    LED_Unlock();
}

void LED_Unlock(void)
{
    HAL_GPIO_WritePin(LDLE_GPIO_Port, LDLE_Pin, GPIO_PIN_SET);        //锁存器关
    HAL_GPIO_WritePin(LDLE_GPIO_Port, LDLE_Pin, GPIO_PIN_RESET);       //锁存器开
}

/**
 * @brief 来自HAL_GPIO_TogglePin()
 * @param led
 */
void LED_Toggle(uint8_t led)
{

    if (led > 8)
    {
        led = led % 8;
    }
    else if (led >= 1)
    {
        uint16_t ledODR = 1;
        uint32_t odr;
        odr = GPIOC->ODR;

        for (int i = 0; i < led - 1; ++i)
        {
            ledODR = ledODR << 1;
        }
        ledODR = ledODR << 8;

        GPIOC->BSRR = ((odr & ledODR) << 16) | (~odr & ledODR);
    }
}
