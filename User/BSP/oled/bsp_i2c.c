#include "bsp_i2c.h"

void I2C2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef I2C_InitStruct;

    // Enable clocks for GPIOB and I2C2
    RCC_AHB1PeriphClockCmd(I2C2_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(I2C2_CLK, ENABLE);

    // Configure GPIOB Pin 10 and 11 as I2C2
    GPIO_InitStruct.GPIO_Pin = I2C2_SCL_PIN | I2C2_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(I2C2_GPIO_PORT, &GPIO_InitStruct);

    // Connect GPIO pins to I2C2 AF
    GPIO_PinAFConfig(I2C2_GPIO_PORT, I2C2_SCL_PinSource, I2C2_SCL_AF);
    GPIO_PinAFConfig(I2C2_GPIO_PORT, I2C2_SDA_PinSource, I2C2_SDA_AF);

    // Configure I2C2
    I2C_InitStruct.I2C_ClockSpeed = 400000; // 400kHz
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2CNUM, &I2C_InitStruct);

    // Enable I2C2
    I2C_Cmd(I2CNUM, ENABLE);
}

void I2C2_Write(u8 device, u8 address, u8 data)
{
    // Wait for I2C2 to be ready
    while (I2C_GetFlagStatus(I2CNUM, I2C_FLAG_BUSY))
        ;

    // Send I2C2 START condition
    I2C_GenerateSTART(I2CNUM, ENABLE);
    while (!I2C_CheckEvent(I2CNUM, I2C_EVENT_MASTER_MODE_SELECT))
        ;

    // Send device address
    I2C_Send7bitAddress(I2CNUM, device, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2CNUM, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;

    // Send data
    I2C_SendData(I2CNUM, address);
    while (!I2C_CheckEvent(I2CNUM, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_SendData(I2CNUM, data);
    while (!I2C_CheckEvent(I2CNUM, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;

    // Send I2C STOP Condition
    I2C_GenerateSTOP(I2CNUM, ENABLE);
}
