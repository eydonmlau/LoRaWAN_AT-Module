#ifndef _COMMAND_PARSE_H
#define _COMMAND_PARSE_H

#include "at_parser.h"
#include "callback_handler.h"

/*
 * @name        AT Respone Error code
 * 
 * Error code represented string  
 */ 
#define _AT_OK                                          (0)
#define _AT_NB_PARAMS_INVALID                           (-1)
#define _AT_CONTENT_PARAMS_INVALID                      (-2)
#define _AT_PARAMS_INVALID                              (-3)
#define _AT_FLASH_EEPROM_ERROR                          (-4)
#define _AT_COMMAND_DISABLE_CURRENTLY                   (-5)
#define _AT_UNKNOWN_ERROR                               (-6)
#define _AT_HEAP_NOT_ENOUGH                             (-7)
#define _AT_BUSY                                        (-8)
#define _AT_UNSUPPORT_PARAMS                            (-9)
#define _AT_COMMAND_UNKNOWN                             (-10)    
#define _AT_COMMAND_WRONG_FORMAT                        (-11)
#define _AT_COMMAND_UNAVAILABLE_CURRENTLY_MODE          (-12)
#define _AT_TOO_MANY_PARAMS                             (-20)
#define _AT_LENGTH_TOO_LONG                             (-21)
#define _AT_COMMAND_NEDD_CRLF                           (-22)
#define _AT_INVALIDE_CHARACTER                          (-23)
     
typedef struct 
{
    char* name;
    T_AT_handler func;
}T_CMD_FUNC_TABLE;

/*
 * Test if contain "(double Quotation), if so delete it 
 * return:      0,  correct format , and delete it or none exist
 *              1,  wrong format
 */
uint8_t AT_strQuotation( char * pStr );

/* 
 *  @brief AT_getParam ,use comma as separator
 *  @param[in],  pIn , original string
 *  @param[out], pOut, param
 *  @param[out], index, address offset
 *  @return:     0,    next param not exist
 *               else, next param address offset           
 */
uint8_t AT_getParam( char *pIn, char *pOut, uint8_t index);

/*
 *  AT respone string function
 */ 
void AT_respString( char* respID, char* resp );

/*
 *  AT respone string with size
 *  
 *  @param[in] resp - error string to respone
 */ 
void AT_respNString( char* respID, char* resp, uint8_t size);

/*
 *  AT respone string without respID function
 *  
 *  @param[in] resp - error string to respone
 */ 
void AT_respStringWithoutID( char* resp );

/*
 *  AT respone errno code function
 */ 
void AT_respErrcode( char* respID, int errcode );

/*
 * AT Command Init
 */ 
void AT_cmdInit(void);

uint8_t CHECK_AT_BUSY();

void  SET_AT_BUSY();

void SET_AT_IDLE();

/*
 *  @brief Uart Buffer Parse
 *  
 *  @param[in] buffer - buffer receive from Uart 
 */ 
void commandParse(char* buffer);

#endif