#include "w25qxx.h"
//////////////////////////////////////////////////////////////////////////////////

// W25QXX 驱动代码

//////////////////////////////////////////////////////////////////////////////////

u16 W25QXX_TYPE = W25Q80; // 默认是W25Q16

// 初始化函数
void W25QXX_Init(void)
{
	SPI1_Init();
	SPI1_SetSpeed(SPI_BaudRatePrescaler_2); // 设置为42M时钟,高速模式
	W25QXX_Write_Enable();
	W25QXX_Write_Disable();
}

// 读取W25QXX的状态寄存器
// BIT7  6   5   4   3   2   1   0
// SPR   RV  TB BP2 BP1 BP0 WEL BUSY
// SPR:默认0,状态寄存器保护位,配合WP使用
// TB,BP2,BP1,BP0:FLASH区域写保护设置
// WEL:写使能锁定
// BUSY:忙标记位(1,忙;0,空闲)
// 默认:0x00
// 读取状态寄存器
uint8_t W25QXX_ReadSR(void)
{
	uint8_t byte = 0;
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_ReadStatusReg);
	byte = SPI1_ReadWriteByte(0xFF);
	W25Q16_CS_HIGH();
	return byte;
}

// 写W25QXX状态寄存器
// 只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void W25QXX_Write_SR(uint8_t sr)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_WriteStatusReg);
	SPI1_ReadWriteByte(sr);
	W25Q16_CS_HIGH();
}

// 写使能
void W25QXX_Write_Enable(void)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_WriteEnable);
	W25Q16_CS_HIGH();
}

// 写保护
void W25QXX_Write_Disable(void)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_WriteDisable);
	W25Q16_CS_HIGH();
}

// 读取芯片ID
// 返回值如下:
// 0XEF13,表示芯片型号为W25Q80
// 0XEF14,表示芯片型号为W25Q16
// 0XEF15,表示芯片型号为W25Q32
// 0XEF16,表示芯片型号为W25Q64
// 0XEF17,表示芯片型号为W25Q128
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
// SPI在一页(0~65535)内写入少于256个字节的数据
// 在指定地址开始写入最大256字节的数据
// pBuffer:数据存储区
// WriteAddr:开始写入的地址(24bit)
// NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void W25QXX_Write_Page(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u16 i;
	W25QXX_Write_Enable();						 // SET WEL
	W25Q16_CS_LOW();							 // 使能器件
	SPI1_ReadWriteByte(W25X_PageProgram);		 // 发送写页命令
	SPI1_ReadWriteByte((u8)((WriteAddr) >> 16)); // 发送24bit地址
	SPI1_ReadWriteByte((u8)((WriteAddr) >> 8));
	SPI1_ReadWriteByte((u8)WriteAddr);
	for (i = 0; i < NumByteToWrite; i++)
		SPI1_ReadWriteByte(pBuffer[i]); // 循环写数
	W25Q16_CS_HIGH();					// 取消片选
	W25QXX_Wait_Busy();					// 等待写入结束
}

// 无检验写SPI FLASH
// 必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
// 具有自动换页功能
// 在指定地址开始写入指定长度的数据,但是要确保地址不越界!
// pBuffer:数据存储区
// WriteAddr:开始写入的地址(24bit)
// NumByteToWrite:要写入的字节数(最大65535)
// CHECK OK
void W25QXX_Write_NoCheck(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u16 pageremain;
	pageremain = W25Q16_PAGE_SIZE - WriteAddr % W25Q16_PAGE_SIZE; // 单页剩余的字节数
	if (NumByteToWrite < pageremain)
		pageremain = NumByteToWrite; // 不大于W25Q16_PAGE_SIZE个字节
	while (1)
	{
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
			break; // 写入结束了
		else	   // NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;

			NumByteToWrite -= pageremain; // 减去已经写入了的字节数
			if (NumByteToWrite > W25Q16_PAGE_SIZE)
				pageremain = W25Q16_PAGE_SIZE; // 一次可以写入W25Q16_PAGE_SIZE个字节
			else
				pageremain = NumByteToWrite; // 不够W25Q16_PAGE_SIZE个字节了
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

// 写SPI FLASH
// 在指定地址开始写入指定长度的数据
// 该函数带擦除操作!
// pBuffer:数据存储区
// WriteAddr:开始写入的地址(24bit)
// NumByteToWrite:要写入的字节数(最大65535)
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
				break; // 需要擦除
		}
		if (i < secremain) // 需要擦除
		{
			for (i = 0; i < secremain; i++)
			{
				W25QXX_BUF[i + secoff] = pBuffer[i];
			}
			W25QXX_Erase_Sector(secpos);
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * W25Q16_SECTOR_SIZE, W25Q16_SECTOR_SIZE);
		}
		else
			W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); // 写已经擦除了的,直接写入扇区剩余区间.
		if (NumByteToWrite == secremain)
			break; // 写入结束了
		else	   // 写入未结束
		{
			secpos++;	// 扇区地址增1
			secoff = 0; // 偏移位置为0

			pBuffer += secremain;		 // 指针偏移
			WriteAddr += secremain;		 // 写地址偏移
			NumByteToWrite -= secremain; // 字节数递减
			if (NumByteToWrite > W25Q16_SECTOR_SIZE)
				secremain = W25Q16_SECTOR_SIZE; // 下一个扇区还是写不完
			else
				secremain = NumByteToWrite; // 下一个扇区可以写完了
		}
	}
}

// 整片擦除
void W25QXX_Erase_Chip(void)
{
	W25QXX_Write_Enable();
	W25QXX_Wait_Busy();
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_ChipErase);
	W25Q16_CS_HIGH();
	W25QXX_Wait_Busy();
}

// 扇区擦除
// Dst_Addr:扇区地址, 函数内会计算实际地址
// 擦除一个扇区的最少时间:150ms
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

// 等待空闲
void W25QXX_Wait_Busy(void)
{
	while ((W25QXX_ReadSR() & 0x01) == 0x01)
		;
}

// 进入掉电模式
void W25QXX_PowerDown(void)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_PowerDown);
	W25Q16_CS_HIGH();
	delay_ms(3); // 延时3ms确保进入掉电模式
}

// 唤醒
void W25QXX_WAKEUP(void)
{
	W25Q16_CS_LOW();
	SPI1_ReadWriteByte(W25X_ReleasePowerDown);
	W25Q16_CS_HIGH();
	delay_ms(3); // 延时3ms确保唤醒
}

// 延时函数
void delay_ms(uint32_t ms)
{
	// 假设一个简单的延时函数
	for (uint32_t i = 0; i < ms * 8000; i++)
		;
}
