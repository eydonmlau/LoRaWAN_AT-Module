#include "eeprom_internal.h"
#include "stm32l1xx.h"

bool eepromWriteBuffer( T_STORAGE_buff *ptr ) {
    uint8_t i;
    uint32_t data;
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR );
    for( i = 0; i < ptr->maxSize;  ) {
        data = *( ptr->buffBase + i );
        data = data << 8;
        data |= *( ptr->buffBase + i + 1 );
        data = data << 8;
        data |= *( ptr->buffBase + i + 2 );
        data = data << 8;
        data |= *( ptr->buffBase + i + 3 );
        if( HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, ptr->flashAddress + i, data ) == HAL_OK )
            i += 4;
    }
    HAL_FLASH_Lock();
    return true;
}

bool eepromReadBuffer( T_STORAGE_buff *ptr ) {
    uint8_t i;
    uint32_t data;
    HAL_FLASH_Unlock( );
    for( i = 0; i < ptr->maxSize; i += 4 ) {
        data = *(__IO uint32_t *)( ptr->flashAddress + i );
        *( ptr->buffBase + i + 3 ) = (uint8_t)data;
        data = data >> 8;
        *( ptr->buffBase + i + 2 ) = (uint8_t)data;
        data = data >> 8;
        *( ptr->buffBase + i + 1 ) = (uint8_t)data;
        data = data >> 8;
        *( ptr->buffBase + i ) = (uint8_t)data;      
    }
    HAL_FLASH_Lock( );
    
    return true;
}

bool eepromWriteFactoryDefault( T_STORAGE_buff *ptr ) {
    uint8_t i;
    uint32_t data;
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR );
    for( i = 0; i < ptr->maxSize;  ) {
        data = *( ptr->buffBase + i );
        data = data << 8;
        data |= *( ptr->buffBase + i + 1 );
        data = data << 8;
        data |= *( ptr->buffBase + i + 2 );
        data = data << 8;
        data |= *( ptr->buffBase + i + 3 );
        if( HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, ptr->factoryDefaultFlashAddress + i, data ) == HAL_OK )
            i += 4;
    }
    HAL_FLASH_Lock();
    return true;
}

bool eepromReadFactoryDefaultBuffer( T_STORAGE_buff *ptr ) {
    uint8_t i;
    uint32_t data;
    HAL_FLASH_Unlock( );
    for( i = 0; i < ptr->maxSize; i += 4 ) {
        data = *(__IO uint32_t *)( ptr->factoryDefaultFlashAddress + i );
        *( ptr->buffBase + i + 3 ) = (uint8_t)data;
        data = data >> 8;
        *( ptr->buffBase + i + 2 ) = (uint8_t)data;
        data = data >> 8;
        *( ptr->buffBase + i + 1 ) = (uint8_t)data;
        data = data >> 8;
        *( ptr->buffBase + i ) = (uint8_t)data;      
    }
    HAL_FLASH_Lock( );
    
    return true;
}
