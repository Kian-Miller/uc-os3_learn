#include "w25qxx.h"
//////////////////////////////////////////////////////////////////////////////////

// W25QXX ��������

//////////////////////////////////////////////////////////////////////////////////

u16 W25QXX_TYPE = W25Q80; // Ĭ����W25Q16

// ��ʼ������
void W25QXX_Init(void)
{
	SPI1_Init();
	SPI1_SetSpeed(SPI_BaudRatePrescaler_2); // ����Ϊ42Mʱ��,����ģʽ
	W25QXX_Write_Enable();
	W25QXX_Write_Disable();
}

// ��ȡW25QXX��״̬�Ĵ���
// BIT7  6   5   4   3   2   1   0
// SPR   RV  TB BP2 BP1 BP0 WEL BUSY
// SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
// TB,BP2,BP1,BP0:FLASH����д��������
// WEL:дʹ������
// BUSY:æ���λ(1,æ;0,����)
// Ĭ��:0x00
// ��ȡ״̬�Ĵ���
uint8_t W25QXX_ReadSR(void)
{
	uint8_t byte = 0;
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_ReadStatusReg);
	byte = SPI1_ReadWriteByte(0xFF);
	W25Q16_CS_HIGH();
	return byte;
}

// дW25QXX״̬�Ĵ���
// ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(uint8_t sr)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_WriteStatusReg);
	SPI1_ReadWriteByte(sr);
	W25Q16_CS_HIGH();
}

// дʹ��
void W25QXX_Write_Enable(void)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_WriteEnable);
	W25Q16_CS_HIGH();
}

// д����
void W25QXX_Write_Disable(void)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_WriteDisable);
	W25Q16_CS_HIGH();
}

// ��ȡоƬID
// ����ֵ����:
// 0XEF13,��ʾоƬ�ͺ�ΪW25Q80
// 0XEF14,��ʾоƬ�ͺ�ΪW25Q16
// 0XEF15,��ʾоƬ�ͺ�ΪW25Q32
// 0XEF16,��ʾоƬ�ͺ�ΪW25Q64
// 0XEF17,��ʾоƬ�ͺ�ΪW25Q128
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(0x90);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	Temp |= SPI1_ReadWriteByte(0xFF) << 8;
	Temp |= SPI1_ReadWriteByte(0xFF);
	W25Q16_CS_HIGH();
	return Temp;
}
// SPI��һҳ(0~65535)��д������256���ֽڵ�����
// ��ָ����ַ��ʼд�����256�ֽڵ�����
// pBuffer:���ݴ洢��
// WriteAddr:��ʼд��ĵ�ַ(24bit)
// NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX_Write_Page(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u16 i;
	W25QXX_Write_Enable();						 // SET WEL
	W25Q16_CS_LOW();							 // ʹ������
	SPI1_ReadWriteByte(W25X_PageProgram);		 // ����дҳ����
	SPI1_ReadWriteByte((u8)((WriteAddr) >> 16)); // ����24bit��ַ
	SPI1_ReadWriteByte((u8)((WriteAddr) >> 8));
	SPI1_ReadWriteByte((u8)WriteAddr);
	for (i = 0; i < NumByteToWrite; i++)
		SPI1_ReadWriteByte(pBuffer[i]); // ѭ��д��
	W25Q16_CS_HIGH();					// ȡ��Ƭѡ
	W25QXX_Wait_Busy();					// �ȴ�д�����
}

// �޼���дSPI FLASH
// ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
// �����Զ���ҳ����
// ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
// pBuffer:���ݴ洢��
// WriteAddr:��ʼд��ĵ�ַ(24bit)
// NumByteToWrite:Ҫд����ֽ���(���65535)
// CHECK OK
void W25QXX_Write_NoCheck(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u16 pageremain;
	pageremain = W25Q16_PAGE_SIZE - WriteAddr % W25Q16_PAGE_SIZE; // ��ҳʣ����ֽ���
	if (NumByteToWrite < pageremain)
		pageremain = NumByteToWrite; // ������W25Q16_PAGE_SIZE���ֽ�
	while (1)
	{
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
			break; // д�������
		else	   // NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;

			NumByteToWrite -= pageremain; // ��ȥ�Ѿ�д���˵��ֽ���
			if (NumByteToWrite > W25Q16_PAGE_SIZE)
				pageremain = W25Q16_PAGE_SIZE; // һ�ο���д��W25Q16_PAGE_SIZE���ֽ�
			else
				pageremain = NumByteToWrite; // ����W25Q16_PAGE_SIZE���ֽ���
		}
	};
}

void W25QXX_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_ReadData);
	SPI1_ReadWriteByte((uint8_t)((ReadAddr) >> 16));
	SPI1_ReadWriteByte((uint8_t)((ReadAddr) >> 8));
	SPI1_ReadWriteByte((uint8_t)ReadAddr);
	for (uint16_t i = 0; i < NumByteToRead; i++)
	{
		pBuffer[i] = SPI1_ReadWriteByte(0xFF);
	}
	W25Q16_CS_HIGH();
}

// дSPI FLASH
// ��ָ����ַ��ʼд��ָ�����ȵ�����
// �ú�������������!
// pBuffer:���ݴ洢��
// WriteAddr:��ʼд��ĵ�ַ(24bit)
// NumByteToWrite:Ҫд����ֽ���(���65535)
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u32 secpos;
	u16 secoff;
	u16 secremain;
	u16 i;
	u8 *W25QXX_BUF;
	W25QXX_BUF = W25QXX_BUFFER;
	secpos = WriteAddr / W25Q16_SECTOR_SIZE;
	secoff = WriteAddr % W25Q16_SECTOR_SIZE;
	secremain = W25Q16_SECTOR_SIZE - secoff;
	if (NumByteToWrite < secremain)
	{
		secremain = NumByteToWrite;
	}
	while (1)
	{
		W25QXX_Read(W25QXX_BUF, secpos * W25Q16_SECTOR_SIZE, W25Q16_SECTOR_SIZE);
		for (i = 0; i < secremain; i++)
		{
			if (W25QXX_BUF[secoff + i] != 0XFF)
				break; // ��Ҫ����
		}
		if (i < secremain) // ��Ҫ����
		{
			for (i = 0; i < secremain; i++)
			{
				W25QXX_BUF[i + secoff] = pBuffer[i];
			}
			W25QXX_Erase_Sector(secpos);
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * W25Q16_SECTOR_SIZE, W25Q16_SECTOR_SIZE);
		}
		else
			W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); // д�Ѿ������˵�,ֱ��д������ʣ������.
		if (NumByteToWrite == secremain)
			break; // д�������
		else	   // д��δ����
		{
			secpos++;	// ������ַ��1
			secoff = 0; // ƫ��λ��Ϊ0

			pBuffer += secremain;		 // ָ��ƫ��
			WriteAddr += secremain;		 // д��ַƫ��
			NumByteToWrite -= secremain; // �ֽ����ݼ�
			if (NumByteToWrite > W25Q16_SECTOR_SIZE)
				secremain = W25Q16_SECTOR_SIZE; // ��һ����������д����
			else
				secremain = NumByteToWrite; // ��һ����������д����
		}
	}
}

// ��Ƭ����
void W25QXX_Erase_Chip(void)
{
	W25QXX_Write_Enable();
	W25QXX_Wait_Busy();
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_ChipErase);
	W25Q16_CS_HIGH();
	W25QXX_Wait_Busy();
}

// ��������
// Dst_Addr:������ַ, �����ڻ����ʵ�ʵ�ַ
// ����һ������������ʱ��:150ms
void W25QXX_Erase_Sector(uint32_t Dst_Addr)
{
	Dst_Addr *= W25Q16_SECTOR_SIZE;
	W25QXX_Write_Enable();
	W25QXX_Wait_Busy();
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_SectorErase);
	SPI1_ReadWriteByte((Dst_Addr) >> 16);
	SPI1_ReadWriteByte((Dst_Addr) >> 8);
	SPI1_ReadWriteByte(Dst_Addr);
	W25Q16_CS_HIGH();
	W25QXX_Wait_Busy();
}

// �ȴ�����
void W25QXX_Wait_Busy(void)
{
	while ((W25QXX_ReadSR() & 0x01) == 0x01)
		;
}

// �������ģʽ
void W25QXX_PowerDown(void)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_PowerDown);
	W25Q16_CS_HIGH();
	delay_ms(3); // ��ʱ3msȷ���������ģʽ
}

// ����
void W25QXX_WAKEUP(void)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_ReleasePowerDown);
	W25Q16_CS_HIGH();
	delay_ms(3); // ��ʱ3msȷ������
}

// ��ʱ����
void delay_ms(uint32_t ms)
{
	// ����һ���򵥵���ʱ����
	for (uint32_t i = 0; i < ms * 8000; i++)
		;
}
