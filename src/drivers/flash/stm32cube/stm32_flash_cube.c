/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.10.2014
 */

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <util/math.h>

#include <drivers/flash/flash.h>
#include <hal/reg.h>
#include <hal/mem.h>
#include <kernel/printk.h>
#include <util/log.h>

#include <drivers/block_dev/flash/stm32flash.h>

#define STM32_FIRST_BLOCK_OFFSET   0x1
#define STM32_ERR_MASK             0x1F3

extern char _flash_start, _flash_end;

#define STM32_FLASH_START ((uint32_t) &_flash_start)
#define STM32_FLASH_END   ((uint32_t) &_flash_end)

static inline int stm32_flash_check_range(struct flash_dev *dev, unsigned long base, size_t len) {
	return STM32_FLASH_START + base + len <= STM32_FLASH_END;
}

static inline int stm32_flash_check_align(unsigned long base, size_t len) {
	return ((uintptr_t) base & STM32_FLASH_WORD_ALIGN ) == 0 &&
			((uintptr_t) len  & STM32_FLASH_WORD_ALIGN) == 0;
}

static inline int stm32f3_flash_check_block(struct flash_dev *dev, uint32_t block) {
	unsigned int n_block, i;

	n_block = 0;
	for (i = 0; i < dev->num_block_infos; i ++) {
		n_block += dev->block_info[i].blocks;
	}

	return block < n_block;
}


static inline void stm32_flash_set_program_size(void) {
	/* setting word 32-bit access */
	REG_STORE(&FLASH->CR, (REG_LOAD(&FLASH->CR) & CR_PSIZE_MASK) | FLASH_PSIZE_WORD);
}

static inline void stm32_flash_wait(void) {
	while (REG_LOAD(&FLASH->SR) & FLASH_FLAG_BSY) {
		/* nop */
	}
}

static int stm32_flash_erase_block(struct flash_dev *dev, uint32_t block) {
	int err;
	uint32_t page_err;
	FLASH_EraseInitTypeDef erase_struct;

	if (!stm32f3_flash_check_block(dev, block)) {
		return -EINVAL;
	}

	HAL_FLASH_Unlock();
	stm32_flash_wait();

	erase_struct = (FLASH_EraseInitTypeDef) {
		.TypeErase = FLASH_TYPEERASE_SECTORS,
		.Sector = block,
		.NbSectors = 1,
	};

	err = HAL_FLASHEx_Erase(&erase_struct, &page_err);
	/* TODO check errcode */
	/* Lock to prevent unwanted operations with flash memory */
	HAL_FLASH_Lock();

	return err;
}


static int stm32_flash_read(struct flash_dev *dev, uint32_t base, void* data, size_t len) {
	size_t rlen = min(len, STM32_FLASH_END - STM32_FLASH_START + base);

	/* read can be unaligned */

	memcpy(data, (void *) STM32_FLASH_START + base, rlen);

	return rlen;
}

static int stm32_flash_program(struct flash_dev *dev, uint32_t base, const void* data, size_t len) {
	unsigned long sr;
	int err;

	if (!stm32_flash_check_align(base, len)
			|| ((uintptr_t) data & 0x3) != 0) {
		return -EINVAL;
	}

	if (!stm32_flash_check_range(dev, base, len)) {
		return -EFBIG;
	}

	HAL_FLASH_Unlock();

	stm32_flash_wait();

	/* clear all errors */
	REG_STORE(&FLASH->SR, STM32_ERR_MASK);

	stm32_flash_set_program_size();

	REG_ORIN(&FLASH->CR, FLASH_CR_PG);

	sr = REG_LOAD(&FLASH->SR);
	if (!(sr & STM32_ERR_MASK)) {
		regcpy32((void *) STM32_FLASH_START + base, data, len >> 2);
		stm32_flash_wait();
		err = 0;
	} else {
		printk("%s: EIO sr=%08lx\n", __func__, sr);
		err = -EIO;
	}

	HAL_FLASH_Lock();

	return err;
}

static int stm32_flash_copy(struct flash_dev *dev, uint32_t base_dst,
				uint32_t base_src, size_t len) {
	return stm32_flash_program(dev, base_dst, (void *) STM32_FLASH_START + base_src, len);
}

static const struct flash_dev_drv stm32_flash_drv = {
	.flash_read = stm32_flash_read,
	.flash_erase_block = stm32_flash_erase_block,
	.flash_program = stm32_flash_program,
	.flash_copy = stm32_flash_copy,
};

static int stm32_f4_flash_init(void *arg) {
	struct flash_dev *flash;

	flash = flash_create("stm32flash", STM32_FLASH_FLASH_SIZE);

	if (flash == NULL) {
		log_error("Failed to create flash device!");
		return -1;
	}

	flash->drv = &stm32_flash_drv;
	flash->size = STM32_FLASH_END - STM32_FLASH_START;
	flash->num_block_infos = 1;
	flash->block_info[0] = (flash_block_info_t) {
		.block_size = STM32_FLASH_PAGE_SIZE,
		.blocks = STM32_FLASH_FLASH_SIZE / STM32_FLASH_PAGE_SIZE
	};

	return 0;
}

FLASH_DEV_DEF("stm32flash", &stm32_flash_drv, stm32_f4_flash_init);
