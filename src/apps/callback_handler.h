#ifndef _CALLBACK_HANDLER_H
#define _CALLBACK_HANDLER_H

#include "at_parser.h"

typedef struct 
{
    uint8_t     *buffBase;                                      /** buffer base address */
    uint32_t    flashAddress;                                   /** internal flash(eeprom) address  */
    uint32_t    factoryDefaultFlashAddress;                     /** factory default params internal flash(eeprom) address */
    uint8_t     typeIndex;                                      /** type index  */
    uint8_t     maxSize;                                        /** buffer max size */
    bool        isReadOnly;                                     /** readonly flag  */
}T_STORAGE_buff;

/** Type Index Define */
#define TYPE_DEVEUI                             0
#define TYPE_APPEUI                             1
#define TYPE_APPKEY                             2
#define TYPE_APPSKEY                            3
#define TYPE_NWKSKEY                            4
#define TYPE_DEVADDR                            5
#define TYPE_VERSION                            6
#define TYPE_ADRSTATUS                          7
#define TYPE_APPPORT                            8
#define TYPE_DATARATE                           9

/** Buffer Max Size Define */
#define DEVEUI_MAX_SIZE                         8
#define APPEUI_MAX_SIZE                         8
#define APPKEY_MAX_SIZE                         16
#define APPSKEY_MAX_SIZE                        16
#define NWKSKEY_MAX_SIZE                        16
#define DEVADDR_MAX_SIZE                        4
#define VERSION_MAX_SIZE                        3
#define ADRSTATUS_MAX_SIZE                      1
#define APPPORT_MAX_SIZE                        1
#define DATARATE_MAX_SIZE                       1

/** Variables Init , Initialize gloable variables from eeprom when program start */
void VariablesInit( );

/** test handler */
void defaultHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** at handler, Test command */
void atHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** help handler */
void helpHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/* factoryDefaultSetting , Reset to factory default configuration
 * first step , read buffer from factory default flash address to different kind array of buffBase address
 * second step, storage this arrays to flash address.
 */
static void factoryDefaultSetting();

/** factory Default Handler */
void factoryDefaultHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** Soft reset Handler */
void resetHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** low Power Handler , Enter sleep mode */
void lowPowerHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** Version Handler */
void versionHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** Message Handler, LoRaWAN unconfirmed data */
void MessageHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** Message Hex Handler , LoRaWAN unconfirmed data in hex */
void messageHexHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** confirm Message Handler, LoRaWAN confirmed data */
void confirmMessageHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** confirm Msg Hex Handler, LoRaWAN confirmed data in hex */
void confirmMsgHexHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** channel Handler, LoRaWAN channel frequency */
void channelHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** dataRate Handler, LoRaWAN datarate */
void dataRateHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** adjust DataRate Handler, LoRaWAN ADR control */
void adjustDataRateHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** repetition Handler, Unconfirmed message repetition */
void repetitionHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** txPower Handler, LoRaWAN TX power */
void txPowerHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** rx Window2 Handler, LoRaWAN RX window2 */
void rxWindow2Handler( char *resp, uint8_t *flag, uint8_t *cnt );

/** rx Window1 Handler, LoRaWAN RX window1 */
void rxWindow1Handler( char *resp, uint8_t *flag, uint8_t *cnt );

/** port Handler, LoRaWAN communication port */
void portHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** id Handler, LoRaWAN DevAddr / DevEUI / appEUI */
void idHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** keyHandler, LoRaWAN NWKSKEY / APPSKEY / APPKEY */
void keyHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** class Handler, choose LoRaWAN work in class(A/B/C) */
void classHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** join Handler, LoRaWAN OTAA Join */
void joinHandler( char *resp, uint8_t *flag, uint8_t *cnt );

/** delay Handler, RX window delay */
void delayHandler( char *resp, uint8_t *flag, uint8_t *cnt );



#endif

/** EOF */