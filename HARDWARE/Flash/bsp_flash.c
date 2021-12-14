#include "stm32f10x.h"
#include "bsp_flash.h"


uint8_t write_count(int32_t count){
		FLASH_Unlock();
    /* 清空所有标志位 */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
    FLASH_Status FLASHStatus = FLASH_COMPLETE;
		FLASHStatus = FLASH_ErasePage(COUNT_ADDR);
		FLASHStatus = FLASH_ProgramWord(COUNT_ADDR, count);
		
		FLASH_Unlock();
		return (*(__IO int32_t *) COUNT_ADDR) != count;
}

int32_t read_count(){
		return *(__IO int32_t *) COUNT_ADDR;
}
