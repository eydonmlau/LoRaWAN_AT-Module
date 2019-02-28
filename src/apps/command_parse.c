#include "command_parse.h"
#include "uart.h"
#include "delay.h"

#define MAX_PARAMS_SIZE         256
#define MAX_COMMAND_TYPE_SIZE   30

extern Uart_t Uart1;

/** AT module is running or not  */
uint8_t ATRunning = 0;

/** storage command type struct array */
T_AT_storage storage[MAX_COMMAND_TYPE_SIZE];


/**  storage params bufffer */
char buffer[MAX_PARAMS_SIZE];

/*
 *  @brief AT uart send buffer
 * 
 *  @param[in] buffer - buffer to send to uart
 */  
static void AT_uartSend(char* buffer) {
    UartPutBuffer( &Uart1 , (uint8_t*)buffer, strlen(buffer) );
}

/*
 *  @brief AT uart send buffer, totally size bytes  
 * 
 *  @param[in] buffer - buffer to send to uart
 */  
static void AT_uartNSend(char* buffer, uint8_t size) {
    UartPutBuffer( &Uart1 , (uint8_t*)buffer, size );
}

/*
 *  AT respone string function
 *  
 *  @param[in] resp - error string to respone
 */ 
void AT_respString( char* respID, char* resp ) {
    if( !strchr( respID, '+' ) )
        AT_uartSend( "+" );
    AT_uartSend( respID );
    AT_uartSend( ": " );
    AT_uartSend(resp);
    AT_uartSend( "\r\n" );
}

/*
 *  AT respone string with size
 *  
 *  @param[in] resp - error string to respone
 */ 
void AT_respNString( char* respID, char* resp, uint8_t size) {
    if( !strchr( respID, '+' ) )
        AT_uartSend( "+" );
    AT_uartSend( respID );
    AT_uartSend( ": " );
    AT_uartNSend(resp, size);
    AT_uartSend( "\r\n" );
}

/*
 *  AT respone string without respID function
 *  
 *  @param[in] resp - error string to respone
 */ 
void AT_respStringWithoutID( char* resp ) {
    AT_uartSend( resp );
    AT_uartSend( "\r\n" );
}

/*
 *  AT respone errno code function
 * 
 *  @param[in] errcode - error code 
 */ 
void AT_respErrcode( char* respID, int errcode ) {
    char tmp[20] = {0};
    if( !strchr( respID, '+' ) )
        AT_uartSend("+");
    AT_uartSend( respID );
    AT_uartSend( ": " );
    snprintf( tmp, 20, "ERROR(%i)", errcode );
    AT_uartSend( tmp );
    AT_uartSend( "\r\n" );
}

/** command and function table list */
static T_CMD_FUNC_TABLE cmdFuncTable[] = 
{
    { "AT",               atHandler                     },
    { "+HELP",            helpHandler                   },
    { "+FDEFAULT",        factoryDefaultHandler         },
    { "+RESET",           resetHandler                  },
    { "+LOWPOWER",        lowPowerHandler               },
    { "+VER",             versionHandler                },
    { "+MSG",             MessageHandler                },
    { "+MSGHEX",          messageHexHandler             },
    { "+CMSG",            confirmMessageHandler         },
    { "+CMSGHEX",         confirmMsgHexHandler          },
    { "+CH",              channelHandler                },       
    { "+DR",              dataRateHandler               },
    { "+ADR",             adjustDataRateHandler         },
    { "+REPT",            repetitionHandler             },
    { "+POWER",           txPowerHandler                },
    { "+RXWIN2",          rxWindow2Handler              },
    { "+RXWIN1",          rxWindow1Handler              },
    { "+PORT",            portHandler                   },
    { "+ID",              idHandler                     },    
    { "+KEY",             keyHandler                    },
    { "+CLASS",           classHandler                  },
    { "+JOIN",            joinHandler                   },
    { "+DELAY",           delayHandler                  },  
};

/*
 * AT Command Init
 */ 
void AT_cmdInit( void ) {
    
    AT_initParser( defaultHandler, storage, MAX_COMMAND_TYPE_SIZE , buffer, MAX_PARAMS_SIZE);
    for( int i = 0; i < sizeof(cmdFuncTable)/sizeof(cmdFuncTable[0]); i++ ) {
        AT_saveHandler(cmdFuncTable[i].name, cmdFuncTable[i].func);
    }
}

/*
 * Delete target char
 */
uint8_t AT_strDeleteTargetChar( char* pStr , char target) {
    char* pTmp = pStr;
    char* pTmp_len = pStr;
    uint8_t len = strlen(pStr);
    while( *pStr != '\0' ) {
        if( *pStr != target )
            *pTmp++ = *pStr;
        ++pStr;
    }
    *pTmp = '\0';
    return len - strlen(pTmp_len);
}

/*
 * Test if contain "(double Quotation), if so delete it 
 * return:      0,  correct format , and delete it or none exist
 *              others,  wrong format
 */
uint8_t AT_strQuotation( char * pStr ) {  
    uint8_t len = strlen( pStr );
    if( *pStr == '"' ) {
        if( *( pStr + len - 1 ) == '"' ) {
            if( AT_strDeleteTargetChar( pStr, '"' ) == 2)
                return 0;
            else
                return 1;
        }
    }
    return AT_strDeleteTargetChar( pStr, '"' );
}

/*
 * Delete space
 */
static void AT_strTrim( char* pStr ) {
    AT_strDeleteTargetChar(pStr, ' ');
}

/* 
 *  @brief AT_getParam ,use comma as separator
 *  @param[in],  pIn , original string
 *  @param[out], pOut, param
 *  @param[out], index, address offset
 *  @return:     0,    next param not exist
 *               else, next param address offset           
 */
uint8_t AT_getParam( char *pIn, char *pOut, uint8_t index) {
    uint8_t ret = 0;
    while( *( pIn + index ) != ',' && *( pIn + index ) != '\0' ) {
        *pOut = *( pIn + index );
        pOut++;
        pIn++;
        ret ++;
    }
    if( *( pIn + index ) == ',' )
        ret++;
    else if( *( pIn + index ) == '\0' )
        ret = 0;
    return ret;
}

/*
 *  @brief check buffer is valid , contain CRLF  or not, if so delete LF
 * 
 */ 
static bool AT_checkCRLF( char* buffer ) {
    int len = strlen( buffer );
    if( buffer[len - 2] == '\r' && buffer[len - 1] == '\n' ) {
        buffer[len - 1] = '\0';
        return true;
    }
    return false;
}

/*
 *  @brief letter toupper
 *  @param[in] char byte
 *  @param[out] char byte
 */
char toupper(char c) {
    if( ( c >= 'a' ) && ( c <= 'z' ) )
        return c + ( 'A' - 'a' );
    return c;
}

uint8_t CHECK_AT_BUSY() {     
    return ATRunning;
}
void  SET_AT_BUSY() {
    ATRunning = 1;
}
void SET_AT_IDLE() {
    ATRunning = 0;
}

/*
 *  @brief Uart Buffer Parse
 *  
 *  @param[in] buffer - buffer receive from Uart 
 */ 
 void commandParse( char* buffer ) {
     char respID[_AT_CMD_MAXSIZE + 1] = {0};
     
     /** letter to upper */
    for( int i = 0; i < strlen( buffer ); i++ )
        buffer[i] = toupper(buffer[i]);
    
    AT_strTrim( buffer );
     
    if( AT_checkCRLF( buffer ) ) {
        if( CHECK_AT_BUSY() ) {
            AT_getRespID( buffer, respID );
            AT_respErrcode( respID, _AT_BUSY );
        }
        else {
            SET_AT_BUSY();
            AT_cmdParse( buffer );
        }
    } else {
        AT_getRespID( buffer, respID );
        AT_respErrcode( respID, _AT_COMMAND_NEDD_CRLF );
    }
}