#include "bsp_spi.h"
//////////////////////////////////////////////////////////////////////////////////

// SPI ��������

//////////////////////////////////////////////////////////////////////////////////

// ������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ
// SPI�ڳ�ʼ��
// �������Ƕ�SPI1�ĳ�ʼ��
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;

	// ����SPI1��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// ����SPI1 SCK, MISO��MOSI����(PB3, PB4, PB5)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// ����F_CS����(PB0)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// ����SPI1���ŵ�AF5
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	// ����SPI1
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	 // ����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;						 // ����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;					 // ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;							 // ʱ�ӿ���״̬Ϊ�͵�ƽ
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;						 // ��һ�����������ݱ�����
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;							 // NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ16
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;					 // ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStruct.SPI_CRCPolynomial = 7;							 // CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStruct);								 // ����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

	// ����SPI1
	SPI_Cmd(SPI1, ENABLE);
}

// SPI1�ٶ����ú���
// SPI�ٶ�=fAPB2/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256
// fAPB2ʱ��һ��Ϊ84Mhz��
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); // �ж���Ч��
	SPI1->CR1 &= 0XFFC7;											// λ3-5���㣬�������ò�����
	SPI1->CR1 |= SPI_BaudRatePrescaler;								// ����SPI1�ٶ�
	SPI_Cmd(SPI1, ENABLE);											// ʹ��SPI1
}

uint8_t SPI1_ReadWriteByte(uint8_t data)
{
	// �ȴ�SPI1���ͻ�����Ϊ��
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		;
	// ��������
	SPI_I2S_SendData(SPI1, data);
	// �ȴ�SPI1���ջ������ǿ�
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		;
	// ���ؽ��յ�������
	return SPI_I2S_ReceiveData(SPI1);
}
