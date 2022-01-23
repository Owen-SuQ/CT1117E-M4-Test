//
// Created by Owen on 2022/1/19 22:13.
//

#include "lcd.h"
/*
void LCD_Proc(uint8_t *ucLcd)                    //LCD处理
{
//    if (usLcd < 500)
//    {                 //500ms未到
//        return;
//    }
//    usLcd = 0;

//  sprintf((char*)ucLcd, "         %03u         ", ucSec);
//  LCD_DisplayStringLine(Line4, ucLcd);
//  LCD_DisplayChar(Line5, Column9, ucLcd[9]);
//  LCD_SetTextColor(Red);
//  LCD_DisplayChar(Line5, Column10, ucLcd[10]);
//  LCD_SetTextColor(White);
//  LCD_DisplayChar(Line5, Column11, ucLcd[11]);
//  LCD_SetTextColor(Red);
//  LCD_DisplayStringLine(Line6, ucLcd);
//  LCD_SetTextColor(White);

    //sprintf((char *) ucLcd, " SEC:%03u   CNT:%03u ", ucSec, ucCnt);

//    sprintf((char *) ucLcd, " SEC:%03u   CNT:%03u ", ucSec, ucCnt);
//    LCD_DisplayStringLine(Line2, ucLcd);
//
//    sprintf((char *) ucLcd, " R37:%04u  B1: %03X", usAdc[2], ucMcp);
//    LCD_DisplayStringLine(Line4, ucLcd);
//    sprintf((char *) ucLcd, " R38:%04u  MCP:%03u", usAdc[0], usAdc[1]);
//    LCD_DisplayStringLine(Line5, ucLcd);
//
//    TIM_GetCapture(usCapt);
//    sprintf((char *) ucLcd, " FRE:%05u B2: %04u", 1000000 / usCapt[0], usComp);
//    LCD_DisplayStringLine(Line7, ucLcd);
//    sprintf((char *) ucLcd, " PER:%05u WID:%04u ", usCapt[0], usCapt[1]);
//    LCD_DisplayStringLine(Line8, ucLcd);
}
*/
/*
  程序说明: CT117E嵌入式竞赛板LCD驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT117E嵌入式竞赛板
  日    期: 2011-8-9
*/
#include "gpio.h"
#include "fonts.h"

uint16_t TextColor = 0x0000, BackColor = 0xFFFF;

void LCD_Write(uint8_t RS, uint16_t Value)                            /* LCD写 */
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);     /* RD#=1 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);   /* CS#=0 */
    GPIOC->ODR = Value;                                     /* 输出数据 */
    if (RS == 0)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); /* RS=0 */
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
    }   /* RS=1 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);   /* WR#=0 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);     /* WR#=1 */
}

/*******************************************************************************
* Function Name  : LCD_Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Delay(uint16_t n)
{
    uint16_t i, j;
    for (i = 0; i < n; ++i)
    {
        for (j = 0; j < 3000; ++j) {}
    }
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_Write(0, LCD_Reg);                                        /* 写索引 */
    LCD_Write(1, LCD_RegValue);                                /* 写数据 */
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetCursor(uint8_t Xpos, uint16_t Ypos)
{
    LCD_WriteReg(32, Xpos);            // 水平地址
    LCD_WriteReg(33, Ypos);            // 垂直地址
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM_Prepare
* Description    : Prepare to write to the LCD RAM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
    LCD_Write(0, 0x22);                                          /* 写索引 */
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM
* Description    : Writes to the LCD RAM.
* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM(uint16_t RGB_Code)
{
    LCD_Write(1, RGB_Code);                                    /* 写数据 */
}

/*******************************************************************************
* Function Name  : LCD_DrawChar
* Description    : Draws a character on LCD.
* Input          : - Xpos: the Line where to display the character shape.
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..9
*                  - Ypos: start column address.
*                  - c: pointer to the character data.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawChar(uint8_t Xpos, uint16_t Ypos, uint16_t *ch)
{
    uint8_t index = 0, i = 0;

    for (index = 0; index < 24; index++)    /* 24行 */
    {
        LCD_SetCursor(Xpos, Ypos);
        LCD_WriteRAM_Prepare();
        for (i = 0; i < 16; i++)                        /* 16列 */
        {
            if ((ch[index] & (1 << i)) == 0x00)
            {
                LCD_WriteRAM(BackColor);            /* 0-背景色 */
            }
            else
            {
                LCD_WriteRAM(TextColor);
            }            /* 1-前景色 */
        }
//  Ypos++;                                                    /* 水平模式：下一行 */
        Xpos++;                                                    /* 垂直模式：下一行 */
    }
}

/*******************************************************************************
* Function Name  : LCD_Init
* Description    : Initializes the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Init(void)
{
    LCD_WriteReg(0x00, 0x0001);        /* 8230上电 */
//LCD_WriteReg(0x03,0x1030);        /* BGR 水平模式*/
    LCD_WriteReg(0x03, 0x1008);        /* BGR 垂直模式*/
    LCD_WriteReg(0x07, 0x0173);        /* 图形显示 */
    LCD_WriteReg(0x10, 0x1090);        /* 电源控制1 */
    LCD_WriteReg(0x11, 0x0227);        /* 电源控制2 */
    LCD_WriteReg(0x12, 0x001d);        /* 电源控制3 */
    LCD_WriteReg(0x60, 0x2700);        /* 输出控制 */
    LCD_WriteReg(0x61, 0x0001);        /* 显示控制 */
    LCD_Delay(250);
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : Clears the hole LCD.
* Input          : Color: the color of the background.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Clear(uint16_t Color)
{
    uint32_t index = 0;
    LCD_SetCursor(0x00, 0x0000);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for (index = 0; index < 76800; index++)
    {
        LCD_WriteRAM(Color);
    }
}

/*******************************************************************************
* Function Name  : LCD_SetTextColor
* Description    : Sets the Text color.
* Input          : - Color: specifies the Text color code RGB(5-6-5).
* Output         : - TextColor: Text color global variable used by LCD_DrawChar
*                  and LCD_DrawPicture functions.
* Return         : None
*******************************************************************************/
void LCD_SetTextColor(uint16_t Color)
{
    TextColor = Color;
}

/*******************************************************************************
* Function Name  : LCD_SetBackColor
* Description    : Sets the Background color.
* Input          : - Color: specifies the Background color code RGB(5-6-5).
* Output         : - BackColor: Background color global variable used by 
*                  LCD_DrawChar and LCD_DrawPicture functions.
* Return         : None
*******************************************************************************/
void LCD_SetBackColor(uint16_t Color)
{
    BackColor = Color;
}

/*******************************************************************************
* Function Name  : LCD_DisplayChar
* Description    : Displays one character (16dots width, 24dots height).
* Input          : - Line: the Line where to display the character shape .
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..9
*                  - Column: start column address.
*                  - Ascii: character ascii code, must be between 0x20 and 0x7E.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii)
{
    Ascii -= 32;
    LCD_DrawChar(Line, Column, &ASCII_Table[Ascii * 24]);
}

/*******************************************************************************
* Function Name  : LCD_DisplayStringLine
* Description    : Displays a maximum of 20 char on the LCD.
* Input          : - Line: the Line where to display the character shape .
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..9
*                  - *ptr: pointer to string to display on LCD.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr)
{
    uint8_t i = 0, j;
    uint16_t k;

//j = 15; k = 0;                    /* 水平模式：字符数，起始位置 */
    j = 20;
    k = 319;                /* 垂直模式：字符数，起始位置 */

    while ((*ptr != 0) && (i < j))
    {
//  LCD_DisplayChar(k, Line, *ptr);
//  k += 16;                            /* 水平模式：下一个字符位置 */
        LCD_DisplayChar(Line, k, *ptr);
        k -= 16;                            /* 垂直模式：下一个字符位置 */
        ptr++;                                    /* 下一个字符 */
        i++;
    }
}
