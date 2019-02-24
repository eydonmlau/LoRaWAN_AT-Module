#include "at_parser.h"

/*
 *  Handler Storage
 *          counter / size / pointer
 */ 
static uint16_t                 handlerIdx;
static uint16_t                 handlerSize;
static T_AT_storage*            handlerStorage;


/*
 *  AT Command Params
 *           size / pointer  
 */ 
static uint16_t                 paramSize;
static char*                    paramStorage;


/*
 * Handler vars
 */ 
static T_AT_handler             fpHandler;
static uint8_t                  flags;
static uint8_t                  count;

/*
 * Look up table for MARK START string
 */
#define MS_LUT_S        6
char MS_LUT[7][3] = 
{
    "",                     // Default -- skiped by search
    "+",                    // AT+...
    "#",                    // AT#...
    "$",                    // AT$...
    "%",                    // AT%...
    "\\",                   // AT\...
    "&"                     // AT&...
};

/*
 * Look up table for MARK END string
 * 
 * MARK END represents the command type
 */ 

#define ME_LUT_S            6
char ME_LUT[6][3] = 
{
    "",                     // Default -- skipped by search
    "=?",                   // Get
    "?",                    // Get
    "=",                    // Set
    ":",                    // Set
    "\r"                   // Exec
};

/*
 * Get number of params
 * 
 * @return nb of params
 */ 
static uint8_t getNbParams( char *pParam ) {
    uint8_t count = 0;
    while( *pParam != '\0' ) {
        if( *pParam == ',' )
            count++;
        pParam++;
    }
    return count + 1;
}

/*
 * Simple Hash code generation
 *
 * Hash code is used to save the command to the storage in aim to have fixed
 * storage space for all functions.
 */
static uint32_t makeHash( char *pCmd ) {
    uint16_t    ch = 0;
    uint32_t    hash = 0x05;

    while((ch = *pCmd++))
        hash += (ch << 1);
    return hash;
}

/*
 * Search handler storage for provided command
 *
 * Function search the storage based on sting length and hash code.
 * If function returns zero command does not exists in storage area.
 */
static uint16_t findHandler( char* pCmd ) {
    uint8_t     len = 0;
    uint16_t    idx = 0;
    uint32_t    hash = 0;

    idx = 0;
    len = strlen( pCmd );
    hash = makeHash( pCmd );
    for( idx = 1; idx < handlerIdx; idx++ ) {
        if( handlerStorage[idx].len == len ) {
            if( handlerStorage[idx].hash == hash ) {
                return idx;
            }
        }
    }
    return _AT_UNKNOWN;                
}

/*
 * Search input for strings from LUT table.
 * LUT table must be 2 dimensional char array.
 *
 * Depend of flag returned value is :
 * - index of found string at LUT
 * - found string offset inside input
 * - (-1) no match
 */
/* Flags */
#define LUT_IDX             0
#define IN_OFF              1
static int searchLut( char* pInput, char (*pLut)[ 3 ], int lutSize, int flag ) {
    uint8_t     inLen = 0;
    uint8_t     inOff = 0;
    uint8_t     lutLen = 0;
    uint8_t     lutIdx = 0;

    if( ( inLen = strlen( pInput ) ) > _AT_CMD_MAXSIZE )
        inLen = _AT_CMD_MAXSIZE;
    for( lutIdx = 1; lutIdx < lutSize; lutIdx++ ) {
        lutLen = strlen( pLut[lutIdx] );
        for( inOff = 0; inOff < inLen; inOff++ ) {
            if( !strncasecmp( pLut[lutIdx], pInput + inOff, lutLen ) ) {
                if( flag == LUT_IDX )
                    return lutIdx;
                else if( flag == IN_OFF )
                    return inOff;
            }
        }
    }
    return -1;
}



/*
 * Parsing
 *
 * @param[in] char* input - AT Command
 * @param[out] at_cmd_cb* cb - handler pointer for the particular command
 * @param[out] uint8_t* cnt -  number of params 
 *
 * Function parses provided raw command string and returns previously saved
 * handler for the particular command. If command is not found return 
 * the default handler.
 */
static uint8_t parseInput( char *pInput, T_AT_handler *pHandler, uint8_t *cnt) {
    uint8_t     hIdx = 0;
    int         startOff = 0;
    int         endOff = 0;
    int         endIdx = 0;
    char        tmp[ _AT_CMD_MAXSIZE + 1 ];
    memset( tmp, 0, _AT_CMD_MAXSIZE + 1 );
    memset( paramStorage, 0, paramSize );
    int pSize = strlen( pInput );
    if( ( startOff = searchLut( pInput, MS_LUT, MS_LUT_S, IN_OFF ) ) == -1 )
        startOff = 0;
    if( ( endOff = searchLut( pInput, ME_LUT, ME_LUT_S, IN_OFF ) )  == -1 )
        endOff = _AT_CMD_MAXSIZE;
    if( ( endIdx = searchLut( pInput, ME_LUT, ME_LUT_S, LUT_IDX ) ) == -1 )
        endIdx = 0;
    strncpy( tmp, pInput + startOff, endOff - startOff );
    hIdx = findHandler( tmp );
    *pHandler = handlerStorage[hIdx].handler;

    if( startOff != 0 ) {
        if( strncasecmp( pInput, "AT", startOff ) ) {                     /** in case AB+...  */   
            *pHandler = handlerStorage[0].handler;
            hIdx = 0;
        }
    }
    
    if( hIdx == 0 )               /* Storage the complete Unknown Command and  pass it to user callback function to parse respID */
        strncat( paramStorage, pInput, pSize );
    else{
        if( endOff < pSize - 1 ) {                                          /** Storage params into paramStorage pointer*/
            if(*(pInput+pSize - 1) == '\r')
                *(pInput+pSize - 1) = '\0';
            strncat( paramStorage,  pInput + endOff + 1, pSize - endOff + 1 );
            *cnt = getNbParams( paramStorage );
        } else {
            *cnt = 0;
        }
    }
    return endIdx;
}

/**
 * @brief Save AT Command to Storage
 *
 * @param[in] pCmd          command string
 * @param[in] pHandler      handler for provided command
 *
 * Saves handlers for the particular command.
 */
int AT_saveHandler( char *pCmd, T_AT_handler pHandler  )
{
    T_AT_storage cmd;

    if(!pHandler)
        pHandler = handlerStorage[0].handler;

    cmd.len = strlen(pCmd);
    if(cmd.len >= _AT_CMD_MAXSIZE)
        return 0;
    if(handlerIdx == handlerSize)
        return 0;
    if(findHandler(pCmd))
        return 0;
    cmd.hash        = makeHash(pCmd);
    cmd.handler     = pHandler;
    handlerStorage[handlerIdx] = cmd;
    handlerIdx++;
    return (handlerSize - handlerIdx);
}

/**
 * @brief AT Parser Initialization
 *
 * @param[in] pHandler          default handler
 * @param[in] pStorage          handler storage
 * @param[in] storageSize       handler storage size
 * @param[in] pBuffer           params array
 * @param[in] bufferSize        params array size
 *
 * Initialization should be executed before any other function. User can
 * execute this function later inside the application to reset AT Engine to
 * the default state.
 */
void AT_initParser( T_AT_handler pHandler, T_AT_storage* pStorage, uint16_t storageSize,  char* pBuffer, uint16_t bufferSize )
{
    T_AT_storage cmd;

    cmd.handler     = pHandler;
    cmd.hash        = makeHash("");
    cmd.len         = 0;
    handlerIdx      = 0;
    handlerSize     = storageSize;
    handlerStorage  = pStorage;
    paramStorage    = pBuffer;
    paramSize       = bufferSize;
    memset( ( void* )paramStorage, 0, paramSize );
    memset( ( void* )handlerStorage, 0, handlerSize * sizeof( T_AT_storage ) );
    handlerStorage[handlerIdx] = cmd;
    handlerIdx++;
}

/*
 * @brief AT Command Parse
 * 
 * @param[in] buffer            buffer to parsed
 */ 
void AT_cmdParse( char* buffer ) {
    flags = parseInput( buffer , &fpHandler, &count);
    fpHandler( paramStorage, &flags, &count);
}

/*
 * @brief Get respone ID
 */
void  AT_getRespID( char *pInput, char *pOutput ) {
    int         startOff = 0;
    int         endOff = 0;

    memset( pOutput, 0, _AT_CMD_MAXSIZE + 1 );
    if( ( startOff = searchLut( pInput, MS_LUT, MS_LUT_S, IN_OFF ) ) == -1 )
        startOff = 0;
    if( ( endOff = searchLut( pInput, ME_LUT, ME_LUT_S, IN_OFF ) )  == -1 )
        endOff = _AT_CMD_MAXSIZE;
    strncpy( pOutput, pInput + startOff, endOff - startOff );
}


/** EOF */