/*
 * Copyright: 
 * All Rights Reserved.
 *
 * File:	flash.h
 * Purpose:	
 * History:
 */

#ifndef _FLASH_H_
#define _FLASH_H_

#include "types.h"
#include <xtensa/config/core.h>


/**************************************************
*                 flash map
*
*                0 ----------
*                    |     |
*       bootloader   |     |
*                    |     |
*           0x20000 ----------
*				     |     |
*           0x30000 ----------
*                    |     |
*           image0   |     |
*                    |     |
*                    |     |
*          0x90000 ----------
*                    |     |
*           image1   |     |
*                    |     |
*                    |     |
*          0xF0000 ----------
*
***************************************************************/


/*Flash address*/
/*Flash address*/
#define FLASH_BASE_ADDR		XCHAL_RESET_VECTOR_VADDR
#define FLASH_SIZE			0x200000
#define FLASH_END_ADDR		(FLASH_BASE_ADDR + FLASH_SIZE - 1)

/*flash 的返回值*/
#define FLASH_PARA_ERR		-2
#define FLASH_ERASE_ERR		-3
#define MALLOC_ERR			-4
#define FLASH_OK			0
#define FLASH_ERROR			-1

#define FLASH_ERASED		0x1234

#define FLASH_TYPE			INT32U
#define UNIT_LEN			4

#ifdef FPGA_VERSION			//64K

#define BOOTLOADER_START	FLASH_BASE_ADDR
#define BOOTLOADER_LEN		(128 * 1024)

#define BOOTINFO_ADDR		(FLASH_BASE_ADDR + 0x20000)//0x20000 = 128K
#define BOOTINFO_LEN		(60 * 1024)
#define BOOTINFO_END_ADDR	(BOOTINFO_ADDR + BOOTINFO_LEN - 1)
#define EXCINFO_ADDR		(BOOTINFO_ADDR + BOOTINFO_LEN)
#define EXCINFO_LEN			0x400					//1K
#define EXC_NUM_MAX			4

#define BOARD_CFG_ADDR		(FLASH_BASE_ADDR + 0x30000)  //192K
#define BOARD_CFG_LEN		(64 * 1024)

#define IMAGE_CFG_ADDR		(FLASH_BASE_ADDR + 0x40000)//256K	/*保存image0配置信息*/
#define IMAGE_CFG_LEN		(16 * 1024)

#define SYS_LOG_ADDR		(FLASH_BASE_ADDR + 0x5c000)

#define IMAGE0_ADDR			(FLASH_BASE_ADDR + 0x60000)//384K
#define IMAGE_LEN			(300 * 1024)
#define IMAGE1_ADDR			(IMAGE0_ADDR + IMAGE_LEN)	/*0x50130000*/
#define IMAGE_LEN_SINGLE	(2*IMAGE_LEN)

#define IMAGE_INFO_LEN		0x20 /*image 文件标志区长度*/
#define IMAGE0_START		(IMAGE0_ADDR + IMAGE_INFO_LEN)
#define IMAGE1_START		(IMAGE1_ADDR + IMAGE_INFO_LEN)

#define IMAGE0_CRC_ADDR		IMAGE0_ADDR/*保存image0的crc的地址*/
#define IMAGE1_CRC_ADDR		IMAGE1_ADDR/*保存image1的crc的地址*/
#define IMAGE0_LEN_ADDR		(IMAGE0_ADDR + 4)/*保存image0长度的地址*/
#define IMAGE1_LEN_ADDR		(IMAGE1_ADDR + 4)/*保存image1长度的地址*/

#define FILE_LEN_ADDR_OST	0x14
#define IMAGE_VER_OSF		0x1dc
#define IMAGE0_VER_ADR		(IMAGE0_START + IMAGE_VER_OSF)
#define IMAGE1_VER_ADR		(IMAGE1_START + IMAGE_VER_OSF)

#else//4K

#define BOOTLOADER_START	FLASH_BASE_ADDR
#define BOOTLOADER_LEN		(128 * 1024)

#define BOOTINFO_ADDR		(FLASH_BASE_ADDR + 0x20000)//0x20000 = 128K
#define BOOTINFO_LEN		(4 * 1024)	//4K
#define EXCINFO_ADDR		(BOOTINFO_ADDR + BOOTINFO_LEN)
#define EXCINFO_LEN			0x400		//1K
#define EXC_NUM_MAX			4

#define BOARD_CFG_ADDR		(FLASH_BASE_ADDR + 0x22000)  //136K
#define BOARD_CFG_LEN		(4 * 1024)	//4K

#define IMAGE_CFG_ADDR		(FLASH_BASE_ADDR + 0x23000)//140K	/*保存image0配置信息*/
#define IMAGE_CFG_LEN		(4 * 1024)	//4K

#define SYS_LOG_ADDR		(FLASH_BASE_ADDR + 0x24000) //144K

#define IMAGE0_ADDR			(FLASH_BASE_ADDR + 0x30000)//192K
#define IMAGE_LEN			(384 * 1024)
#define IMAGE1_ADDR			(IMAGE0_ADDR + IMAGE_LEN)	/*0x50130000*/
#define IMAGE_LEN_SINGLE	(2*IMAGE_LEN)

#define IMAGES_CFG          (0xFE0F1000)
#define IMAGES_ADDR			(0xFE110000)
#define IMAGES_LEN			(256 * 1024)
#define IMAGEX_ADDR			(IMAGES_ADDR+IMAGES_LEN)
#define IMAGEX_LEN			IMAGES_LEN

#define IMAGE_INFO_LEN		0x20 /*image 文件标志区长度*/
#define IMAGE0_START		(IMAGE0_ADDR + IMAGE_INFO_LEN)
#define IMAGE1_START		(IMAGE1_ADDR + IMAGE_INFO_LEN)

#define IMAGE0_CRC_ADDR		IMAGE0_ADDR/*保存image0的crc的地址*/
#define IMAGE1_CRC_ADDR		IMAGE1_ADDR/*保存image1的crc的地址*/
#define IMAGE0_LEN_ADDR		(IMAGE0_ADDR + 4)/*保存image0长度的地址*/
#define IMAGE1_LEN_ADDR		(IMAGE1_ADDR + 4)/*保存image1长度的地址*/

#define FILE_LEN_ADDR_OST	0x14
#define IMAGE_VER_OSF		0x1dc
#define IMAGE0_VER_ADR		(IMAGE0_START + IMAGE_VER_OSF)
#define IMAGE1_VER_ADR		(IMAGE1_START + IMAGE_VER_OSF)

#define DATA_ADDR			(FLASH_BASE_ADDR + 0xF0000) /* 960K */
#define DATA_LEN			0x10000						/* 64K */
#define MAC_ADDR			(DATA_ADDR) /* 960K */
#define MAC_LEN				0x6							/* 6Byte */

#define USER_ADDR           (IMAGEX_ADDR + IMAGEX_LEN)
#define USER_LEN            (FLASH_BASE_ADDR+FLASH_SIZE-USER_ADDR)

#define FLASH_ADAPTOR_ADDR  USER_ADDR
#define FLASH_ADAPTOR_LEN   0x1000

#define IMAGES_MCU_ADDR (FLASH_ADAPTOR_ADDR + FLASH_ADAPTOR_LEN)

#endif

#define FLASH_PAGE_SHIFT	8UL
#define FLASH_PAGE_SIZE		256UL			/* 256 in every page */
#define FLASH_PAGE_MASK		(FLASH_PAGE_SIZE - 1)	/* 255 */
#define FLASH_SECTOR_SHIFT	16UL
#define FLASH_SECTOR_SIZE	(4 * 1024UL)		/* 4K in every sector */
#define FLASH_SECTOR_MASK	(FLASH_SECTOR_SIZE - 1)	/* 0x3ff */
#define FLASH_BLOCK_SIZE	(64 * 1024UL)		/* 64K in every block */
#define FLASH_BLOCK_MASK	(FLASH_BLOCK_SIZE - 1)	/* 0xffff */
#define FLASH_MAX_SECTOR_NUM	31			/* M25P16 has 32 sectors: 0 ~ 31 */

/* Flash opcodes. */
#define OPCODE_WREN     0x06    /* Write enable */
#define OPCODE_WRDI     0x04    /* Write disable */
#define OPCODE_RDSR     0x05    /* Read status register */
#define OPCODE_WRSR     0x01    /* Write status register */
#define OPCODE_READ     0x03    /* Read data bytes */
#define OPCODE_PP       0x02    /* Page program */
#define OPCODE_SE       0xd8    /* Sector erase 64k*/
#define OPCODE_BR       0xc7    /* Buck erase */
#define OPCODE_RES      0xab    /* Read Electronic Signature */
#define OPCODE_RDID     0x9f    /* Read JEDEC ID */
#define OPCODE_SE_4K    0x20    /* erase only one Sector 4k*/

/* status Register */
#define SR_WIP			0x01 /*Write In Progress Bit*/

/* AC Characteristics */
#define FSCK_CLK 		40000000 /* 40Mhz */

/* spi control */
#define CS_HIGH			0x1
#define CS_LOW			0x0

#define BNUM_1B			0x1
#define BNUM_2B			0x2
#define BNUM_3B			0x3
#define BNUM_4B			0x0

#define spi_speed_cfg()				\
({						\
	uint8_t sck;				\
 	sck = ((CPU_CLK >> 4) / FSCK_CLK);	\
 	SET_SPI_SPI_SCK_HIGH_PERIOD(sck);	\
 	SET_SPI_SPI_SCK_LOW_PERIOD(sck);	\
})

#define flash_addr2sector(addr)		\
	(((addr) > FLASH_END_ADDR || (addr) < FLASH_BASE_ADDR) ? FLASH_PARA_ERR : ((addr) >> FLASH_SECTOR_SHIFT))
#define flash_sector2addr(sector)	\
	(FLASH_BASE_ADDR + ((sector) << FLASH_SECTOR_SHIFT))

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
extern int32_t flash_image_bin_erase(uint32_t addr, uint32_t len);

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
extern int32_t flash_user_info_erase(uint32_t addr, uint32_t len);

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
extern int32_t flash_image_bin_write(uint32_t dst, uint8_t *psrc, uint32_t len);

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
extern int32_t flash_user_info_write(uint32_t dst, uint8_t *psrc, uint32_t len);

#endif	/* end of _FLASH_H_ */
