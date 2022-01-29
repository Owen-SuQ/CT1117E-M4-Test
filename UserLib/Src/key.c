//
// Created by Owen on 2022/1/15.
//

#include "key.h"

#define HARD_KEY_NUM        3                    /* 实体按键个数 */
static KEY_T s_tBtn[HARD_KEY_NUM] = {0};
static KEY_FIFO_T s_tKey;                        /* 按键FIFO变量,结构体 */

static void KEY_FIFO_Init(void);

//static void KEY_GPIO_Config(void);

static uint8_t IsKey1Down(void);

static uint8_t IsKey2Down(void);

static uint8_t IsKey3Down(void);

static void KEY_Detect(uint8_t i);

/**
 * @brief 初始化按键. 该函数在主函数的开头被调用。
 */
void KEY_Init(void)
{
    KEY_FIFO_Init();        /* 初始化按键变量 */
    //KEY_GPIO_Config();        /* 初始化按键硬件 */
}

/**
 * @brief 初始化按键变量
 */
static void KEY_FIFO_Init(void)
{
    uint8_t i;

    /* 对按键FIFO读写指针清零 */
    s_tKey.Read = 0;
    s_tKey.Write = 0;

    /* 给每个按键结构体成员变量赋一组缺省值 */
    for (i = 0; i < HARD_KEY_NUM; i++)
    {
        s_tBtn[i].LongTime = KEY_LONG_TIME;            /* 长按时间 0 表示不检测长按键事件 */
        s_tBtn[i].Count = KEY_FILTER_TIME / 2;        /* 计数器设置为滤波时间的一半 */
        s_tBtn[i].State = 0;                            /* 按键缺省状态，0为未按下 */
        s_tBtn[i].RepeatSpeed = 0;                        /* 按键连发的速度，0表示不支持连发 */
        s_tBtn[i].RepeatCount = 0;                        /* 连发计数器 */
    }
    /* 判断按键按下的函数 */
    s_tBtn[0].IsKeyDownFunc = IsKey1Down;
    s_tBtn[1].IsKeyDownFunc = IsKey2Down;
    s_tBtn[2].IsKeyDownFunc = IsKey3Down;
}

/**
 * @brief 配置按键对应的GPIO
 */
//static void KEY_GPIO_Config(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//
//    /* 第1步：打开GPIO时钟 */
//    __HAL_RCC_GPIOE_CLK_ENABLE();
//
//    /* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
//    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;            /* 设置输入 */
//    GPIO_InitStructure.Pull = GPIO_NOPULL;                 /* 上下拉电阻不使能 */
//    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  /* GPIO速度等级 */
//
//    GPIO_InitStructure.Pin = GPIO_PIN_4;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//    GPIO_InitStructure.Pin = GPIO_PIN_3;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//    GPIO_InitStructure.Pin = GPIO_PIN_2;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
//}

/**
 * @brief 判断按键是否按下 单键和组合键区分 单键事件不允许有其他键按下
 * @return 返回值1 表示按下(导通），0表示未按下（释放）
 */
static uint8_t IsKey1Down(void)
{
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static uint8_t IsKey2Down(void)
{
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static uint8_t IsKey3Down(void)
{
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief 将1个键值压入按键FIFO缓冲区 可用于模拟一个按键
 * @param _KeyCode 按键代码
 */
void KEY_FIFO_Put(uint8_t _KeyCode)
{
    s_tKey.Buf[s_tKey.Write] = _KeyCode;

    if (++s_tKey.Write >= KEY_FIFO_SIZE)
    {
        s_tKey.Write = 0;
    }
}

/**
 * @brief 从按键FIFO缓冲区读取一个键值
 * @return 按键代码
 */
uint8_t KEY_FIFO_Get(void)
{
    uint8_t ret;

    if (s_tKey.Read == s_tKey.Write)
    {
        return KEY_NONE;
    }
    else
    {
        ret = s_tKey.Buf[s_tKey.Read];

        if (++s_tKey.Read >= KEY_FIFO_SIZE)
        {
            s_tKey.Read = 0;
        }
        return ret;
    }
}

/**
 * @brief 读取按键的状态
 * @param _ucKeyID 按键ID，从0开始
 * @return 1 表示按下， 0 表示未按下
 */
uint8_t KEY_GetState(KEY_ID_E _ucKeyID)
{
    return s_tBtn[_ucKeyID].State;
}

/**
 * @brief 设置按键参数
 * @param _ucKeyID 按键ID，从0开始
 * @param _LongTime 长按事件时间
 * @param _RepeatSpeed 连发速度
 */
void KEY_SetParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t _RepeatSpeed)
{
    s_tBtn[_ucKeyID].LongTime = _LongTime;            /* 长按时间 0 表示不检测长按键事件 */
    s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;            /* 按键连发的速度，0表示不支持连发 */
    s_tBtn[_ucKeyID].RepeatCount = 0;                        /* 连发计数器 */
}

/**
 * @brief 清空按键FIFO缓冲区
 */
void KEY_FIFO_Clear(void)
{
    s_tKey.Read = s_tKey.Write;
}

/**
 * @brief 扫描所有按键。非阻塞，被systick中断周期性的调用，10ms一次
 */
void KEY_Scan(void)
{
    uint8_t i;

    for (i = 0; i < HARD_KEY_NUM; i++)
    {
        KEY_Detect(i);
    }
}

/**
 * @brief 检测一个按键。非阻塞状态，必须被周期性的调用。
 * @param i IO的id， 从0开始编码
 */
static void KEY_Detect(uint8_t i)
{
    KEY_T *pBtn;

    pBtn = &s_tBtn[i];/*读取相应按键的结构体地址，程序里面每个按键都有自己的结构体。*/
    if (pBtn->IsKeyDownFunc())
    {/*这个里面执行的是按键按下的处理*/
        if (pBtn->Count < KEY_FILTER_TIME)/*用于按键滤波前给 Count 设置一个初值*/
        {
            pBtn->Count = KEY_FILTER_TIME;
        }
        else if (pBtn->Count < 2 * KEY_FILTER_TIME)/*这里实现 KEY_FILTER_TIME 时间长度的延迟*/
        {
            pBtn->Count++;
        }
        else
        {
            if (pBtn->State == 0)
            {
                pBtn->State = 1;/*如果按键按下了，这里就将其设置为 1*/

                /* 发送按钮按下的消息 */
                KEY_FIFO_Put((uint8_t) (3 * i + 1));
            }

            if (pBtn->LongTime > 0) /*LongTime初始值是100。单位10ms，持续1秒，认为长按事件*/
            {
                if (pBtn->LongCount < pBtn->LongTime) /*LongCount长按计数器。单位10ms，持续1秒，认为长按事件*/
                {
                    /* 发送按钮持续按下的消息 */
                    if (++pBtn->LongCount == pBtn->LongTime)/*LongCount等于LongTime(100),10ms进来一次，进来了100次也就是说按下时间为于1s*/
                    {
                        /* 键值放入按键FIFO */
                        KEY_FIFO_Put((uint8_t) (3 * i + 3));
                    }
                }
                else/*LongCount大于LongTime(100),也就是说按下时间大于1s*/
                {
                    if (pBtn->RepeatSpeed > 0)/* RepeatSpeed连续按键周期 */
                    {
                        if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
                        {
                            pBtn->RepeatCount = 0;
                            /* 长按键后，每隔10ms发送1个按键。因为长按也是要发送键值得，10ms发送一次。*/
                            KEY_FIFO_Put((uint8_t) (3 * i + 1));
                        }
                    }
                }
            }
        }
    }
    else
    {/*这个里面执行的是按键松手的处理或者按键没有按下的处理*/
        if (pBtn->Count > KEY_FILTER_TIME)
        {
            pBtn->Count = KEY_FILTER_TIME;
        }
        else if (pBtn->Count != 0)
        {
            pBtn->Count--;
        }
        else
        {
            if (pBtn->State == 1)
            {
                pBtn->State = 0;

                /* 发送按钮弹起的消息 */
                KEY_FIFO_Put((uint8_t) (3 * i + 2));
            }
        }

        pBtn->LongCount = 0;
        pBtn->RepeatCount = 0;
    }
}
