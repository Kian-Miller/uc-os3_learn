#include "bsp_spi.h"
//////////////////////////////////////////////////////////////////////////////////

// SPI 驱动代码

//////////////////////////////////////////////////////////////////////////////////

// 以下是SPI模块的初始化代码，配置成主机模式
// SPI口初始化
// 这里针是对SPI1的初始化
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;

	// 启用SPI1和GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// 配置SPI1 SCK, MISO和MOSI引脚(PB3, PB4, PB5)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// 配置F_CS引脚(PB0)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// 连接SPI1引脚到AF5
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	// 配置SPI1
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	 // 设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;						 // 设置SPI工作模式:设置为主SPI
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;					 // 设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;							 // 时钟空闲状态为低电平
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;						 // 第一个跳变沿数据被采样
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;							 // NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // 定义波特率预分频的值:波特率预分频值为16
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;					 // 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStruct.SPI_CRCPolynomial = 7;							 // CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStruct);								 // 根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

	// 启用SPI1
	SPI_Cmd(SPI1, ENABLE);
}

// SPI1速度设置函数
// SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256
// fAPB2时钟一般为84Mhz：
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); // 判断有效性
	SPI1->CR1 &= 0XFFC7;											// 位3-5清零，用来设置波特率
	SPI1->CR1 |= SPI_BaudRatePrescaler;								// 设置SPI1速度
	SPI_Cmd(SPI1, ENABLE);											// 使能SPI1
}

uint8_t SPI1_ReadWriteByte(uint8_t data)
{
	// 等待SPI1发送缓冲区为空
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		;
	// 发送数据
	SPI_I2S_SendData(SPI1, data);
	// 等待SPI1接收缓冲区非空
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		;
	// 返回接收到的数据
	return SPI_I2S_ReceiveData(SPI1);
}
