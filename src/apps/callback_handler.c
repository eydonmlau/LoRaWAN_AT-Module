#include "callback_handler.h"
#include "Commissioning.h"
#include "eeprom_internal.h"
#include "command_parse.h"
#include "board.h"
#include "utilities.h"
#include "LoRaMac.h"


static uint8_t DevEui[]         =       LORAWAN_DEVICE_EUI;
static uint8_t AppEui[]         =       LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[]         =       LORAWAN_APPLICATION_KEY;
static uint8_t AppSKey[]        =       LORAWAN_APPSKEY;
static uint8_t NWKSKey[]        =       LORAWAN_NWKSKEY;
static uint32_t Addr            =       LORAWAN_DEVICE_ADDRESS;
static uint8_t DevAddr[]        =       {0};
static uint8_t Version[]        =       {1,0,0};                        //major, minor, patch
uint8_t InitalizeFactory        =       1;
static uint8_t AdrStatus        =       1;
MibRequestConfirm_t mibReq;

T_STORAGE_buff storageBuff[] = {
    {   DevEui,         DEVEUI_FLASH_ADDRESS,        DEVEUI_FACTORY_FLASH_ADDRESS,        TYPE_DEVEUI,          DEVEUI_MAX_SIZE,        false   },
    {   AppEui,         APPEUI_FLASH_ADDRESS,        APPEUI_FACTORY_FLASH_ADDRESS,        TYPE_APPEUI,          APPEUI_MAX_SIZE,        false   },
    {   AppKey,         APPKEY_FLASH_ADDRESS,        APPKEY_FACTORY_FLASH_ADDRESS,        TYPE_APPKEY,          APPKEY_MAX_SIZE,        false   },
    {   AppSKey,        APPSKEY_FLASH_ADDRESS,       APPSKEY_FACTORY_FLASH_ADDRESS,       TYPE_APPSKEY,         APPSKEY_MAX_SIZE,       false   },
    {   NWKSKey,        NWKSKEY_FLASH_ADDRESS,       NWKSKEY_FACTORY_FLASH_ADDRESS,       TYPE_NWKSKEY,         NWKSKEY_MAX_SIZE,       false   },
    {   DevAddr,        DEVADDR_FLASH_ADDRESS,       DEVADDR_FACTORY_FLASH_ADDRESS,       TYPE_DEVADDR,         DEVADDR_MAX_SIZE,       false   },
    {   Version,        VERSION_FLASH_ADDRESS,       VERSION_FACTORY_FLASH_ADDRESS,       TYPE_VERSION,         VERSION_MAX_SIZE,       false   },
    {   &AdrStatus,      ADRSTATUS_FLASH_ADDRESS,     ADRSTATUS_FACTORY_FLASH_ADDRESS,     TYPE_ADRSTATUS,       ADRSTATUS_MAX_SIZE,     false   },
    
};

/** Variables Init , Initialize gloable variables from eeprom when program start */
void VariablesInit( ) {
        
    if( InitalizeFactory == 0 ) {
        T_STORAGE_buff *ptr;
        
        BoardGetUniqueId( DevEui );
        ptr = &storageBuff[TYPE_DEVEUI];
        eepromWriteFactoryDefault(ptr);        
        
        ptr = &storageBuff[TYPE_APPEUI];
        eepromWriteFactoryDefault(ptr);
        
        ptr = &storageBuff[TYPE_APPKEY];
        eepromWriteFactoryDefault(ptr);
        
        ptr = &storageBuff[TYPE_APPSKEY];
        eepromWriteFactoryDefault(ptr);
        
        ptr = &storageBuff[TYPE_NWKSKEY];
        eepromWriteFactoryDefault(ptr);
        
       srand1( BoardGetRandomSeed( ) );

        // Choose a random device address
        Addr = randr( 0, 0x01FFFFFF );
        DevAddr[0] = Addr >> 24;
        DevAddr[1] = Addr >> 16;
        DevAddr[2] = Addr >> 8;
        DevAddr[3] = Addr;
        ptr = &storageBuff[TYPE_DEVADDR];
        eepromWriteFactoryDefault(ptr);
        
        ptr = &storageBuff[TYPE_VERSION];
        eepromWriteFactoryDefault(ptr);
        
        ptr = &storageBuff[TYPE_ADRSTATUS];
        eepromWriteFactoryDefault(ptr);
        
        factoryDefaultSetting( );
        
    } else {
        eepromReadBuffer( &storageBuff[TYPE_DEVEUI] ); 
    
        eepromReadBuffer( &storageBuff[TYPE_APPEUI] );
    
        eepromReadBuffer( &storageBuff[TYPE_APPKEY] );
    
        eepromReadBuffer( &storageBuff[TYPE_APPSKEY] );
    
        eepromReadBuffer( &storageBuff[TYPE_NWKSKEY] );
    
        eepromReadBuffer( &storageBuff[TYPE_DEVADDR] );
    
        eepromReadBuffer( &storageBuff[TYPE_VERSION] );   
        
        eepromReadBuffer( &storageBuff[TYPE_ADRSTATUS] ); 
    }
}

/** test handler */
void defaultHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    char respID[_AT_CMD_MAXSIZE + 1] = {0};
    AT_getRespID( resp, respID );
    AT_respErrcode( respID, _AT_COMMAND_UNKNOWN );
    SET_AT_IDLE();
}

/** at handler, Test Command */
void atHandler(char * resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "AT", "OK" );
    SET_AT_IDLE();
}

/** help handler, Show Command List */
void helpHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "HELP", "OK" );
    AT_respStringWithoutID("    AT        --- AT Ping");
    AT_respStringWithoutID("    HELP      --- Print Command List");
    AT_respStringWithoutID("    FDEFAULT  --- Factory Data Reset");
    AT_respStringWithoutID("    RESET     --- Software Reset");
    AT_respStringWithoutID("    LOWPOWER  --- Enter Sleep Mode");
    AT_respStringWithoutID("    VER       --- Version");
    AT_respStringWithoutID("    MSG       --- Unconfirmed Message");
    AT_respStringWithoutID("    MSGHEX    --- Unconfirmed Message In Hex");
    AT_respStringWithoutID("    CMSG      --- Comfirmed Message");
    AT_respStringWithoutID("    CMSGHEX   --- Confirmed Message In Hex");
    AT_respStringWithoutID("    CH        --- Channel Frequency");
    AT_respStringWithoutID("    ADR       --- ADR ON/OFF");
    AT_respStringWithoutID("    DR        --- Datarate");
    AT_respStringWithoutID("    REPT      --- MSG/MSGHEX Repetition");
    AT_respStringWithoutID("    POWER     --- TX Power");
    AT_respStringWithoutID("    RXWIN1    --- RX window1");
    AT_respStringWithoutID("    RXWIN2    --- RX window2");
    AT_respStringWithoutID("    PORT      --- TX Port");
    AT_respStringWithoutID("    ID        --- DevADDR/DevEUI/AppEUI");
    AT_respStringWithoutID("    KEY       --- NWKSKey/AppSKey/AppKey");
    AT_respStringWithoutID("    CLASS     --- ClassA/C(B not supported yet)");
    AT_respStringWithoutID("    JOIN      --- OTAA Join request");
    AT_respStringWithoutID("    Delay     --- Rx Window Delay");    
    SET_AT_IDLE();
}

/* factoryDefaultSetting , Reset to factory default configuration
 * first step , read buffer from factory default flash address to different kind array of buffBase address
 * second step, storage this arrays to flash address.
 */
static void factoryDefaultSetting() {
        T_STORAGE_buff *ptr;
        
        eepromReadFactoryDefaultBuffer( &storageBuff[TYPE_DEVEUI] ); 
    
        eepromReadFactoryDefaultBuffer( &storageBuff[TYPE_APPEUI] );
    
        eepromReadFactoryDefaultBuffer( &storageBuff[TYPE_APPKEY] );
    
        eepromReadFactoryDefaultBuffer( &storageBuff[TYPE_APPSKEY] );
    
        eepromReadFactoryDefaultBuffer( &storageBuff[TYPE_NWKSKEY] );
    
        eepromReadFactoryDefaultBuffer( &storageBuff[TYPE_DEVADDR] );
    
        eepromReadFactoryDefaultBuffer( &storageBuff[TYPE_VERSION] ); 
        
        eepromReadFactoryDefaultBuffer( &storageBuff[TYPE_ADRSTATUS] );
        
        ptr = &storageBuff[TYPE_DEVEUI];
        eepromWriteBuffer(ptr);
        
        ptr = &storageBuff[TYPE_APPEUI];
        eepromWriteBuffer(ptr);
        
        ptr = &storageBuff[TYPE_APPKEY];
        eepromWriteBuffer(ptr);
        
        ptr = &storageBuff[TYPE_APPSKEY];
        eepromWriteBuffer(ptr);
        
        ptr = &storageBuff[TYPE_NWKSKEY];
        eepromWriteBuffer(ptr);
        
        ptr = &storageBuff[TYPE_DEVADDR];
        eepromWriteBuffer(ptr);
        
        ptr = &storageBuff[TYPE_VERSION];
        eepromWriteBuffer(ptr);
        
        ptr = &storageBuff[TYPE_ADRSTATUS];
        eepromWriteBuffer(ptr);
}

/** factory Default Handler */
void factoryDefaultHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    if( *flag  == _AT_GET_EQUAL_QUESTION | *flag  == _AT_GET_QUESTION | *flag  == _AT_GET_EXEC ) {
        factoryDefaultSetting();
        AT_respString( "FDEFAULT", "OK" );
    } else {
        AT_respErrcode( "EDEFAULT", _AT_UNSUPPORT_PARAMS);
    }

    SET_AT_IDLE();
}

/** Soft reset Handler */
void resetHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "RESET", "OK" );
    BoardResetMcu();
    SET_AT_IDLE();
}

/** low Power Handler , Enter sleep mode */
void lowPowerHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "LOWPOWER", "OK" );
    SET_AT_IDLE();
}

/** Version Handler */
void versionHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    if( *flag  == _AT_GET_EQUAL_QUESTION | *flag  == _AT_GET_QUESTION | *flag  == _AT_GET_EXEC ) {
        AT_respString( "VER", "$MAJOR.$MINOR.$PATCH" );
        char tmp[10] = {0};
        snprintf(tmp, 10, "%d.%d.%d", Version[0], Version[1], Version[2]);
        AT_respString( "VER", tmp );
        
    } else {
        AT_respErrcode( "VER", _AT_UNSUPPORT_PARAMS);
    }
    SET_AT_IDLE();
}

/** Message Handler, LoRaWAN unconfirmed data */
void MessageHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "MSG", "OK" );
    SET_AT_IDLE();
}

/** Message Hex Handler , LoRaWAN unconfirmed data in hex */
void messageHexHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "MSGHEX", "OK" );
    SET_AT_IDLE();
}

/** confirm Message Handler, LoRaWAN confirmed data */
void confirmMessageHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "CMSG", "OK" );
    SET_AT_IDLE();
}

/** confirm Msg Hex Handler, LoRaWAN confirmed data in hex */
void confirmMsgHexHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "CMSGHEX", "OK" );
    SET_AT_IDLE();
}

/** channel Handler, LoRaWAN channel frequency */
void channelHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "CH", "OK" );
    SET_AT_IDLE();
}

/** dataRate Handler, LoRaWAN datarate */
void dataRateHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "DR", "OK" );
    SET_AT_IDLE();
}

bool AT_setADR(uint8_t buff) {
    bool flag;
    if( buff == 0 )
        flag = false;
    else if( buff == 1 ) 
        flag = true;
    else 
        return false;
    
    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = flag;
    LoRaMacMibSetRequestConfirm( &mibReq );
    return true;   
}

/** adjust DataRate Handler, LoRaWAN ADR control */
void adjustDataRateHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    T_STORAGE_buff *ptr;
    char tmp[256] = {0};
    uint8_t type = 0;
    uint8_t index = 0;
    char* status[2] = {"OFF", "ON"}; 
    if( *flag  == _AT_GET_EQUAL_QUESTION | *flag  == _AT_GET_QUESTION | *flag  == _AT_GET_EXEC ) {
        if( AdrStatus == 0 )
            strncat(tmp, "OFF", 2);
        else
            strncat(tmp, "ON", 3);
        AT_respString( "ADR", tmp );
    } else if( *flag  == _AT_SET_EQUAL ) {
        index = AT_getParam( resp, tmp, index );
        if( index != 0 ) {
            AT_respErrcode( "ADR", _AT_NB_PARAMS_INVALID );
            SET_AT_IDLE();
            return;
        }
        if( strncasecmp( status[0], (const char*)tmp , strlen( tmp ) ) == 0 ) {
            type = 0;
        } else if( strncasecmp( status[1], (const char*)tmp , strlen( tmp ) ) == 0 ) {
            type = 1;
        } else {
            AT_respErrcode( "ADR", _AT_CONTENT_PARAMS_INVALID );
            SET_AT_IDLE();
            return;
        }
        AT_setADR(type);
        AdrStatus = type;
        ptr = &storageBuff[TYPE_DEVADDR];
        eepromWriteBuffer(ptr);
        AT_respString( "ADR", status[type] );       
    } else {
        AT_respErrcode("ADR", _AT_UNKNOWN_ERROR);
    }
    
    SET_AT_IDLE();
}

/** repetition Handler, Unconfirmed message repetition */
void repetitionHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "REPT", "OK" );
    SET_AT_IDLE();
}

/** txPower Handler, LoRaWAN TX power */
void txPowerHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "POWER", "OK" );
    SET_AT_IDLE();
}

/** rx Window2 Handler, LoRaWAN RX window2 */
void rxWindow2Handler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "RXWIN2", "OK" );
    SET_AT_IDLE();
}

/** rx Window1 Handler, LoRaWAN RX window1 */
void rxWindow1Handler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "RXWIN1", "OK" );
    SET_AT_IDLE();
}

/** port Handler, LoRaWAN communication port */
void portHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "PORT", "OK" );
    SET_AT_IDLE();
}

/** id Handler, LoRaWAN DevAddr / DevEUI / appEUI */
void idHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    T_STORAGE_buff *ptr;
    uint8_t index = 0;
    char tmp[256] = {0};
    char ret[30] = {0};
    char *id[3] = {"DEVADDR","DEVEUI", "APPEUI"};
    uint8_t type = 0;
    int buff[8] = {0};
   
    if( *flag  == _AT_GET_EQUAL_QUESTION | *flag  == _AT_GET_QUESTION | *flag  == _AT_GET_EXEC ) {
        snprintf( ret, 256, "DevAddr, %02x:%02x:%02x:%02x",              \
                         DevAddr[0], DevAddr[1], DevAddr[2], DevAddr[3] );
        
        AT_respString( "ID", ret );
        snprintf( ret, 256, "DevEui, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",              \
                         DevEui[0], DevEui[1], DevEui[2], DevEui[3], DevEui[4], DevEui[5], DevEui[6], DevEui[7] );
        
        AT_respString( "ID", ret );
        memset(ret, 0, sizeof(ret));
        snprintf( ret, 256, "AppEui, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",              \
                         AppEui[0], AppEui[1], AppEui[2], AppEui[3], AppEui[4], AppEui[5], AppEui[6], AppEui[7] );
        AT_respString( "ID", ret );
        memset(ret, 0, sizeof(ret));
    } else if( *flag  == _AT_SET_EQUAL ) {
        index = AT_getParam( resp, tmp, index );
        if( index == 0 ) {
            if( strncasecmp( id[0], (const char*)tmp , strlen( tmp ) ) == 0 ) {
                snprintf( ret, 256, "DevAddr, %02x:%02x:%02x:%02x",              \
                         DevAddr[0], DevAddr[1], DevAddr[2], DevAddr[3] );
                AT_respString( "ID", ret );
            }  else if( strncasecmp( id[1], (const char*)tmp , strlen( tmp ) ) == 0 ) {
                snprintf( ret, 256, "DevEui, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",              \
                         DevEui[0], DevEui[1], DevEui[2], DevEui[3], DevEui[4], DevEui[5], DevEui[6], DevEui[7] );
                AT_respString( "ID", ret );
            }  else if( strncasecmp( id[2], tmp , strlen( tmp ) ) == 0 ) {
                snprintf( ret, 256, "AppEui, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",              \
                         AppEui[0], AppEui[1], AppEui[2], AppEui[3], AppEui[4], AppEui[5], AppEui[6], AppEui[7] );
                AT_respString( "ID", ret );
            } else {
                AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                SET_AT_IDLE();
                return;
            }
        } else {
            if( strncasecmp( id[0], (const char*)tmp , strlen( tmp ) ) == 0 ) {
                type = 0;
            }  else if( strncasecmp( id[1], (const char*)tmp , strlen( tmp ) ) == 0 ) {
                type  = 1;
            }  else if( strncasecmp( id[2], tmp , strlen( tmp ) ) == 0 ) {
                type = 2;
            } else {
                AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                SET_AT_IDLE( );
                return;
            }
            memset( tmp, 0, 256 );
            index = AT_getParam( resp, tmp, index );
            if( index != 0 ) {
                AT_respErrcode( "ID", _AT_NB_PARAMS_INVALID );
                SET_AT_IDLE();
                return;
            } 
            if( AT_strQuotation( tmp ) ) {                      //check double quotation
                AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                SET_AT_IDLE();
                return;
            }
            if( type == 0 ) { 
                uint8_t count = 0;
                uint8_t len = strlen( tmp );
                if( len > 8 ) {
                    AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( (count = sscanf( tmp, "%02x %02x %02x %02x", &buff[0],                              \
                                    &buff[1], &buff[2], &buff[3] )) == 0 ) {
                    AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( count == 4 ) {
                    for(int i = 0; i < 4; i++)
                        DevAddr[i] = buff[i];
                    ptr = &storageBuff[TYPE_DEVADDR];
                    eepromWriteBuffer(ptr);
                    snprintf( ret, 256, "DevAddr, %02x:%02x:%02x:%02x",                                 \
                         DevAddr[0], DevAddr[1], DevAddr[2], DevAddr[3] );
                    AT_respString( "ID", ret );
                }
                else {
                    AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
            } else if( type == 1 ) { 
                uint8_t count = 0;
                if( strlen( tmp ) > 16 ) {
                    AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( (count = sscanf( tmp, "%02x %02x %02x %02x %02x %02x %02x %02x",                    \
                                    &buff[0], &buff[1], &buff[2], &buff[3], &buff[4], &buff[5], &buff[6], &buff[7] )) == 0 ) {
                    AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( count == 8 ) {
                    for(int i = 0; i < 8; i++)
                        DevEui[i] = buff[i];
                    ptr = &storageBuff[TYPE_DEVEUI];
                    eepromWriteBuffer(ptr);
                    snprintf( ret, 256, "DevEui, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",              \
                            DevEui[0], DevEui[1], DevEui[2], DevEui[3], DevEui[4], DevEui[5], DevEui[6], DevEui[7] );
                    AT_respString( "ID", ret );
                }
                else {
                    AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
            }  else if( type == 2 ) { 
                uint8_t count = 0;
                if( strlen( tmp ) > 16 ) {
                    AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( (count = sscanf( tmp, "%02x %02x %02x %02x %02x %02x %02x %02x",                    \
                                    &buff[0], &buff[1], &buff[2], &buff[3], &buff[4], &buff[5], &buff[6], &buff[7] )) == 0 ) {
                    AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( count == 8 ) {
                    for(int i = 0; i < 8; i++)
                        AppEui[i] = buff[i];
                    ptr = &storageBuff[TYPE_APPEUI];
                    eepromWriteBuffer(ptr);
                    snprintf( ret, 256, "AppEui, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",              \
                            AppEui[0], AppEui[1], AppEui[2], AppEui[3], AppEui[4], AppEui[5], AppEui[6], AppEui[7] );
                    AT_respString( "ID", ret );
                } else {
                    AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
            }           
        } 
    } else {
        AT_respErrcode("ID", _AT_UNKNOWN_ERROR);
    }
    SET_AT_IDLE();
}

/** keyHandler, LoRaWAN NWKSKEY / APPSKEY / APPKEY */
void keyHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    T_STORAGE_buff *ptr;
    uint8_t index = 0;
    char tmp[256] = {0};
    char ret[60] = {0};
    char *id[3] = {"APPKEY","NWKSKEY","APPSKEY"};
    uint8_t type = 0;
    int buff[16] = {0};
   
    if( *flag  == _AT_GET_EQUAL_QUESTION | *flag  == _AT_GET_QUESTION | *flag  == _AT_GET_EXEC ) {
        snprintf( ret, 60, "AppKey, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",                   \
                         AppKey[0], AppKey[1], AppKey[2], AppKey[3],AppKey[4], AppKey[5], AppKey[6], AppKey[7],                         \
                         AppKey[8], AppKey[9], AppKey[10], AppKey[11],AppKey[12], AppKey[13], AppKey[14], AppKey[15]    );       
        AT_respString( "KEY", ret );
        
        memset(ret, 0, sizeof(ret));
        snprintf( ret, 60, "NWKSKey, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",                  \
                         NWKSKey[0], NWKSKey[1], NWKSKey[2], NWKSKey[3],NWKSKey[4], NWKSKey[5], NWKSKey[6], NWKSKey[7],                 \
                         NWKSKey[8], NWKSKey[9], NWKSKey[10], NWKSKey[11],NWKSKey[12], NWKSKey[13], NWKSKey[14], NWKSKey[15] );
        
        AT_respString( "KEY", ret );

        snprintf( ret, 60, "AppSKey, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",                  \
                         AppSKey[0], AppSKey[1], AppSKey[2], AppSKey[3],AppSKey[4], AppSKey[5], AppSKey[6], AppSKey[7],                 \
                         AppSKey[8], AppSKey[9], AppSKey[10], AppSKey[11],AppSKey[12], AppSKey[13], AppSKey[14], AppSKey[15] );
        AT_respString( "KEY", ret );
    } else if( *flag  == _AT_SET_EQUAL ) {
        index = AT_getParam( resp, tmp, index );
        if( index == 0 ) {
            if( strncasecmp( id[0], (const char*)tmp , strlen( tmp ) ) == 0 ) {
                snprintf( ret, 60, "AppKey, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",           \
                         AppKey[0], AppKey[1], AppKey[2], AppKey[3],AppKey[4], AppKey[5], AppKey[6], AppKey[7],                         \
                         AppKey[8], AppKey[9], AppKey[10], AppKey[11],AppKey[12], AppKey[13], AppKey[14], AppKey[15] );       
                AT_respString( "ID", ret );
            }  else if( strncasecmp( id[1], (const char*)tmp , strlen( tmp ) ) == 0 ) {
                snprintf( ret, 60, "NWKSKey, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",          \
                         NWKSKey[0], NWKSKey[1], NWKSKey[2], NWKSKey[3],NWKSKey[4], NWKSKey[5], NWKSKey[6], NWKSKey[7],                 \
                         NWKSKey[8], NWKSKey[9], NWKSKey[10], NWKSKey[11],NWKSKey[12], NWKSKey[13], NWKSKey[14], NWKSKey[15] );
                AT_respString( "ID", ret );
            }  else if( strncasecmp( id[2], tmp , strlen( tmp ) ) == 0 ) {
                snprintf( ret, 60, "AppSKey, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",          \
                         AppSKey[0], AppSKey[1], AppSKey[2], AppSKey[3],AppSKey[4], AppSKey[5], AppSKey[6], AppSKey[7],                 \
                         AppSKey[8], AppSKey[9], AppSKey[10], AppSKey[11],AppSKey[12], AppSKey[13], AppSKey[14], AppSKey[15] );
                AT_respString( "KEY", ret );
            } else {
                AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                SET_AT_IDLE();
                return;
            }
        } else {
            if( strncasecmp( id[0], (const char*)tmp , strlen( tmp ) ) == 0 ) {
                type = 0;
            }  else if( strncasecmp( id[1], (const char*)tmp , strlen( tmp ) ) == 0 ) {
                type  = 1;
            }  else if( strncasecmp( id[2], tmp , strlen( tmp ) ) == 0 ) {
                type = 2;
            } else {
                AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                SET_AT_IDLE( );
                return;
            }
            memset( tmp, 0, 256 );
            index = AT_getParam( resp, tmp, index );
            if( index != 0 ) {
                AT_respErrcode( "KEY", _AT_NB_PARAMS_INVALID );
                SET_AT_IDLE();
                return;
            } 
            if( AT_strQuotation( tmp ) ) {                      //check double quotation
                AT_respErrcode( "ID", _AT_CONTENT_PARAMS_INVALID );
                SET_AT_IDLE();
                return;
            }
            if( type == 0 ) { 
                uint8_t count = 0;
                uint8_t len = strlen( tmp );
                if( len > 32 ) {
                    AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( (count = sscanf( tmp, "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x  ",          \
                                    &buff[0],&buff[1], &buff[2], &buff[3], &buff[4],&buff[5], &buff[6], &buff[7],                       \
                                    &buff[8],&buff[9], &buff[10], &buff[11], &buff[12],&buff[13], &buff[14], &buff[15])) == 0 ) {
                    AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( count == 16 ) {
                    for(int i = 0; i < 16; i++)
                        AppKey[i] = buff[i];
                    ptr = &storageBuff[TYPE_APPKEY];
                    eepromWriteBuffer(ptr);
                    snprintf( ret, 60, "AppKey, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",       \
                         AppKey[0], AppKey[1], AppKey[2], AppKey[3],AppKey[4], AppKey[5], AppKey[6], AppKey[7],                         \
                         AppKey[8], AppKey[9], AppKey[10], AppKey[11],AppKey[12], AppKey[13], AppKey[14], AppKey[15]    ); 
                    AT_respString( "KEY", ret );
                }
                else {
                    AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
            } else if( type == 1 ) { 
                uint8_t count = 0;
                if( strlen( tmp ) > 32 ) {
                    AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( (count = sscanf( tmp, "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",          \
                                    &buff[0],&buff[1], &buff[2], &buff[3], &buff[4],&buff[5], &buff[6], &buff[7],                      \
                                    &buff[8],&buff[9], &buff[10], &buff[11], &buff[12],&buff[13], &buff[14], &buff[15])) == 0 ) {
                    AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( count == 16 ) {
                    for(int i = 0; i < 16; i++)
                        NWKSKey[i] = buff[i];
                    ptr = &storageBuff[TYPE_NWKSKEY];
                    eepromWriteBuffer(ptr);
                    snprintf( ret, 60, "NWKSKey, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",      \
                         NWKSKey[0], NWKSKey[1], NWKSKey[2], NWKSKey[3],NWKSKey[4], NWKSKey[5], NWKSKey[6], NWKSKey[7],                 \
                         NWKSKey[8], NWKSKey[9], NWKSKey[10], NWKSKey[11],NWKSKey[12], NWKSKey[13], NWKSKey[14], NWKSKey[15]    );
                    AT_respString( "KEY", ret );
                }
                else {
                    AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
            }  else if( type == 2 ) { 
                uint8_t count = 0;
                if( strlen( tmp ) > 32 ) {
                    AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( (count = sscanf( tmp, "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x  ",          \
                                    &buff[0],&buff[1], &buff[2], &buff[3], &buff[4],&buff[5], &buff[6], &buff[7],                       \
                                    &buff[8],&buff[9], &buff[10], &buff[11], &buff[12],&buff[13], &buff[14], &buff[15])) == 0 ) {
                    AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
                if( count == 16 ) {
                    for(int i = 0; i < 16; i++)
                        AppSKey[i] = buff[i];
                    ptr = &storageBuff[TYPE_APPSKEY];
                    eepromWriteBuffer(ptr);
                    snprintf( ret, 60, "AppSKey, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",      \
                         AppSKey[0], AppSKey[1], AppSKey[2], AppSKey[3],AppSKey[4], AppSKey[5], AppSKey[6], AppSKey[7],                 \
                         AppSKey[8], AppSKey[9], AppSKey[10], AppSKey[11],AppSKey[12], AppSKey[13], AppSKey[14], AppSKey[15]    );
                    AT_respString( "KEY", ret );
                } else {
                    AT_respErrcode( "KEY", _AT_CONTENT_PARAMS_INVALID );
                    SET_AT_IDLE( );
                    return;
                }
            }           
        } 
    } else {
        AT_respErrcode("KEY", _AT_UNKNOWN_ERROR);
    }
    SET_AT_IDLE();
}

/** class Handler, choose LoRaWAN work in class(A/B/C) */
void classHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "CLASS", "OK" );
    SET_AT_IDLE();
}

/** join Handler, LoRaWAN OTAA Join */
void joinHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "JOIN", "OK" );
    SET_AT_IDLE();
}

/** delay Handler, RX window delay */
void delayHandler( char *resp, uint8_t *flag, uint8_t *cnt ) {
    AT_respString( "DELAY", "OK" );
    SET_AT_IDLE();
}



/** EOF */