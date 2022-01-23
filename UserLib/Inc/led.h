//
// Created by Owen on 2022/1/15.
//
#pragma once
#ifndef LED_H
#define LED_H

#include "main.h"
#include <math.h>

void LED_Process(uint8_t *ledODR, uint8_t direction);

void LED_Displacement(uint8_t *ledODR, uint8_t direction);

void LED_Unlock(void);

void LED_Display(uint8_t *ledODR);

void LED_Toggle(uint8_t led);

#endif //LED_H
