#ifndef _EEPROM_INTERNAL_H
#define _EEPROM_INTERNAL_H

#include "callback_handler.h"

/** internal flash base address , factory default base address */
#define FLASH_ADDRESS_BASE                      0x08080000
#define FACTORY_DEFAULT_FLASH_ADDRESS_BASE      0x08080100

/** param internal address offset, at least 4 offset even if 1 byte */
#define DEVEUI_FLASH_ADDRESS_OFFSET             0                       //0 
#define APPEUI_FLASH_ADDRESS_OFFSET             0x00000008              //8 Bytes       
#define APPKEY_FLASH_ADDRESS_OFFSET             0x00000010              //8 Bytes
#define APPSKEY_FLASH_ADDRESS_OFFSET            0x00000020              //16 Bytes
#define NWKSKEY_FLASH_ADDRESS_OFFSET            0x00000030              //16 Bytes
#define DEVADDR_FLASH_ADDRESS_OFFSET            0x00000040              //4 Bytes
#define VERSION_FLASH_ADDRESS_OFFSET            0x00000044              //3 Bytes
#define ADRSTATUS_FLASH_ADDRESS_OFFSET          0x00000048              //1 Bytes
#define APPPORT_FLASH_ADDRESS_OFFSET            0x0000004c              //1 Bytes
#define DATARATE_FLASH_ADDRESS_OFFSET           0x00000050              //1 Bytes

/** param internal flash address */
#define DEVEUI_FLASH_ADDRESS                    FLASH_ADDRESS_BASE + DEVEUI_FLASH_ADDRESS_OFFSET
#define APPEUI_FLASH_ADDRESS                    FLASH_ADDRESS_BASE + APPEUI_FLASH_ADDRESS_OFFSET
#define APPKEY_FLASH_ADDRESS                    FLASH_ADDRESS_BASE + APPKEY_FLASH_ADDRESS_OFFSET
#define APPSKEY_FLASH_ADDRESS                   FLASH_ADDRESS_BASE + APPSKEY_FLASH_ADDRESS_OFFSET
#define NWKSKEY_FLASH_ADDRESS                   FLASH_ADDRESS_BASE + NWKSKEY_FLASH_ADDRESS_OFFSET
#define DEVADDR_FLASH_ADDRESS                   FLASH_ADDRESS_BASE + DEVADDR_FLASH_ADDRESS_OFFSET
#define VERSION_FLASH_ADDRESS                   FLASH_ADDRESS_BASE + VERSION_FLASH_ADDRESS_OFFSET
#define ADRSTATUS_FLASH_ADDRESS                 FLASH_ADDRESS_BASE + ADRSTATUS_FLASH_ADDRESS_OFFSET
#define APPPORT_FLASH_ADDRESS                   FLASH_ADDRESS_BASE + APPPORT_FLASH_ADDRESS_OFFSET
#define DATARATE_FLASH_ADDRESS                  FLASH_ADDRESS_BASE + DATARATE_FLASH_ADDRESS_OFFSET

/** param internal factory default flash address */
#define DEVEUI_FACTORY_FLASH_ADDRESS            FACTORY_DEFAULT_FLASH_ADDRESS_BASE + DEVEUI_FLASH_ADDRESS_OFFSET
#define APPEUI_FACTORY_FLASH_ADDRESS            FACTORY_DEFAULT_FLASH_ADDRESS_BASE + APPEUI_FLASH_ADDRESS_OFFSET
#define APPKEY_FACTORY_FLASH_ADDRESS            FACTORY_DEFAULT_FLASH_ADDRESS_BASE + APPKEY_FLASH_ADDRESS_OFFSET
#define APPSKEY_FACTORY_FLASH_ADDRESS           FACTORY_DEFAULT_FLASH_ADDRESS_BASE + APPSKEY_FLASH_ADDRESS_OFFSET
#define NWKSKEY_FACTORY_FLASH_ADDRESS           FACTORY_DEFAULT_FLASH_ADDRESS_BASE + NWKSKEY_FLASH_ADDRESS_OFFSET
#define DEVADDR_FACTORY_FLASH_ADDRESS           FACTORY_DEFAULT_FLASH_ADDRESS_BASE + DEVADDR_FLASH_ADDRESS_OFFSET
#define VERSION_FACTORY_FLASH_ADDRESS           FACTORY_DEFAULT_FLASH_ADDRESS_BASE + VERSION_FLASH_ADDRESS_OFFSET
#define ADRSTATUS_FACTORY_FLASH_ADDRESS         FACTORY_DEFAULT_FLASH_ADDRESS_BASE + ADRSTATUS_FLASH_ADDRESS_OFFSET
#define APPPORT_FACTORY_FLASH_ADDRESS           FACTORY_DEFAULT_FLASH_ADDRESS_BASE + APPPORT_FLASH_ADDRESS_OFFSET
#define DATARATE_FACTORY_FLASH_ADDRESS          FACTORY_DEFAULT_FLASH_ADDRESS_BASE + DATARATE_FLASH_ADDRESS_OFFSET

/* 
 *  @brief write buffer to eeprom user space
 *  @param[in] T_STORAGE_buff *ptr
 */
bool eepromWriteBuffer( T_STORAGE_buff *ptr );

/* 
 *  @brief read buffer from eeprom 
 *  @param[in] T_STORAGE_buff *ptr
 */
bool eepromReadBuffer( T_STORAGE_buff *ptr );

/* 
 *  @brief write buffer to eeprom using factory default address
 *  @param[in] T_STORAGE_buff *ptr
 */
bool eepromWriteFactoryDefault( T_STORAGE_buff *ptr );

/* 
 *  @brief read buffer from eeprom from factory default address
 *  @param[in] T_STORAGE_buff *ptr
 */
bool eepromReadFactoryDefaultBuffer( T_STORAGE_buff *ptr );



#endif