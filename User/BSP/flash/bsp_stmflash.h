#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "stm32f4xx.h"
#include "bsp_stmflash.h"
//////////////////////////////////////////////////////////////////////////////////

// STM32内部FLASH读写驱动代码

//////////////////////////////////////////////////////////////////////////////////

// STM32 FLASH起始地址
#define STM32_FLASH_BASE 0x08000000
#define INSIDE_FLASH_SAVE_ADDR 0X0800C004 // 设置FLASH 保存地址(必须为偶数，且所在扇区,要大于本代码所占用到的扇区.
                                          // 否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机.

// FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0 ((u32)0x08000000)  // 扇区0起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_1 ((u32)0x08004000)  // 扇区1起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_2 ((u32)0x08008000)  // 扇区2起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_3 ((u32)0x0800C000)  // 扇区3起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_4 ((u32)0x08010000)  // 扇区4起始地址, 64 Kbytes
#define ADDR_FLASH_SECTOR_5 ((u32)0x08020000)  // 扇区5起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_6 ((u32)0x08040000)  // 扇区6起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_7 ((u32)0x08060000)  // 扇区7起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_8 ((u32)0x08080000)  // 扇区8起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_9 ((u32)0x080A0000)  // 扇区9起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_10 ((u32)0x080C0000) // 扇区10起始地址,128 Kbytes
#define ADDR_FLASH_SECTOR_11 ((u32)0x080E0000) // 扇区11起始地址,128 Kbytes

u32 STMFLASH_ReadWord(u32 faddr);                                 // 读出字
void STMFLASH_Write(u32 WriteAddr, u32 *pBuffer, u32 NumToWrite); // 从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr, u32 *pBuffer, u32 NumToRead);    // 从指定地址开始读出指定长度的数据

#endif
