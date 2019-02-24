#ifndef __AT_PARSER_H
#define __AT_PARSER_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/** Max Command size excluding command args*/
#define _AT_CMD_MAXSIZE             15

/*
 * @name        AT Command Types
 * 
 * Command types provided as second argument during the handler call.
 */ 
#define _AT_UNKNOWN                     (0)
#define _AT_GET_EQUAL_QUESTION          (1)
#define _AT_GET_QUESTION                (2)
#define _AT_SET_EQUAL                   (3)
#define _AT_SET_COLON                   (4)
#define _AT_GET_EXEC                    (5)

/*
 * @name        AT Parser Types
 * 
 * @typedef     Handler prototype
 * 
 */
typedef void ( *T_AT_handler )( char *buffer, uint8_t *type , uint8_t *cnt); 

/*
 * @brief       Parser Structure
 * 
 * Struct is used for stroing the command with callbacks.
 * Command strings are converted to the hash code for easiest comparision.
 */ 
typedef struct
{
    /** Command Length */
    uint16_t            len;
    /** Command Hash Value */
    uint32_t            hash;
    /** Get Callback */
    T_AT_handler        handler;
}T_AT_storage;



/**
 * @brief Save AT Command to Storage
 *
 * @param[in] pCmd          command string
 * @param[in] pHandler      handler for provided command
 *
 * Saves handlers for the particular command.
 */
int AT_saveHandler( char *pCmd, T_AT_handler pHandler );


/**
 * @brief AT Parser Initialization
 *
 * @param[in] pHandler          default handler
 * @param[in] pStorage          handler storage
 * @param[in] storageSize       handler storage size
 * @param[in] pBuffer           params array
 * @param[in] bufferSize        params array size
 * #param[in] pRespId           respone command ID
 * 
 * Initialization should be executed before any other function. User can
 * execute this function later inside the application to reset AT Engine to
 * the default state.
 */
void AT_initParser( T_AT_handler pHandler, T_AT_storage* pStorage, uint16_t storageSize,  char* pBuffer, uint16_t bufferSize );


/*
 * @brief AT Command Parse
 * 
 * @param[in] buffer            buffer to parsed
 */ 
void AT_cmdParse( char* buffer );

/*
 * @brief Get respone ID
 */
void AT_getRespID( char *pInput, char *pOutput );

#endif