/**********************************************************
 *(c) by Knobloch GmbH
 *       Selitstrasse 10
 *       D-55234 Erbes-Büdesheim                                    
 *
 *  Headerfile:     KeLibTxtDl.h    KeLib TXT Download Definitions
 *
 *  Start:          21.02.2015
 *  Last Change:    19.08.2017
 *                  
 *                  
 *  Revisionen:
 *      00.01:      21.02.2015
 *                - Start
 *
 *
 *  Compiler:   gcc / g++
 **********************************************************/
#ifndef KELIB_TXT_DEFINES_DL
    #define KELIB_TXT_DEFINES_DL
    
    

    typedef char                CCHAR8;
    typedef signed char         CHAR8;
    typedef unsigned char       UCHAR8;

    typedef signed char         INT8;
    typedef signed short        INT16;
    typedef signed int          INT32;

    typedef unsigned char       UINT8;
    typedef unsigned short      UINT16;
    typedef unsigned int        UINT32;

    typedef unsigned char       BOOL8;
    typedef unsigned short      BOOL16;
    typedef unsigned long       BOOL32;
    
    // Sorry, different developers
    // KE loves _t:
    typedef signed char         int8_t;
    typedef signed short int    int16_t;
    typedef signed int          int32_t;
    //typedef signed long int     int64_t;

    typedef unsigned char       uint8_t;
    typedef unsigned short int  uint16_t;
    typedef unsigned int        uint32_t;
  
    
    
    
    #ifndef __FT_SHMEM_H__
        #include "FtShmem.h"        // Ftshmem: Definitionen und Deklarationen
    #endif


    // I2C
    #define I2C_SPEED_100_KHZ       0
    #define I2C_SPEED_400_KHZ       1



// -------------------------------------------------------------------
// Prototypes



    // Start the IO thread
    uint32_t StartTxtDownloadProg(void);

    // Stop the IO thread
    uint32_t StopTxtDownloadProg(void);

    // Check if the motor IO thread is running
    bool MotorIOLib_ThreadIsRunning(void);
    
    // Set Callback function
    void SetTransferAreaCompleteCallback(bool (*callback)(FISH_X1_TRANSFER *transarea, int nareas));
    
    

extern "C" 
{
    // Get Adress of Transfer Area
    FISH_X1_TRANSFER * GetKeLibTransferAreaMainAddress(void);


    // I2C
    uint32_t InitI2C(void);
    uint32_t KeLibI2cTransfer(uint8_t u8DevAddr, uint16_t u16NumWrite, uint8_t *pWriteData, uint16_t u16NumRead, uint8_t *pReadData, uint16_t u16Clock400kHz);
}






    // Errors
    #define KELIB_ERROR_NONE                        0x00000000L     
    #define KELIB_ERROR_UNDEFINED                   0x00000001L     
    
    #define KELIB_ERROR_LIB_NOT_INIT                0x00000100L     
    
    
    // Errors
    #define KELIB_ERROR_WRONG_PARAMETER_1_VALUE     0x00001001L     
    #define KELIB_ERROR_WRONG_PARAMETER_2_VALUE     0x00001002L     
    #define KELIB_ERROR_WRONG_PARAMETER_3_VALUE     0x00001003L     
    #define KELIB_ERROR_WRONG_PARAMETER_4_VALUE     0x00001004L     
    #define KELIB_ERROR_WRONG_PARAMETER_5_VALUE     0x00001005L     
    #define KELIB_ERROR_WRONG_MODE                  0x00001010L     
    
    #define KELIB_ERROR_SOFTWARE_1                  0x00001020L     

    #define KELIB_ERROR_ZERO_POINTER_VALUE          0x00001050L     


    // Error Fonts
    #define KELIB_ERROR_MENUE_LOAD_FONT_001         0x00001101L   
    #define KELIB_ERROR_MENUE_LOAD_FONT_002         0x00001102L   
    #define KELIB_ERROR_MENUE_LOAD_FONT_003         0x00001103L   
    #define KELIB_ERROR_MENUE_LOAD_FONT_004         0x00001104L   
    #define KELIB_ERROR_MENUE_LOAD_FONT_005         0x00001105L   


    // Error Files
    #define KELIB_ERROR_FILE_NOT_FOUND              0x00001201L   
    #define KELIB_ERROR_FILE_EMPTY                  0x00001202L   
    #define KELIB_ERROR_FILE_AT_CLOSE               0x00001203L   
    #define KELIB_ERROR_FILE_NOT_OPEN               0x00001204L   
    #define KELIB_ERROR_FILE_POS                    0x00001205L   
    #define KELIB_ERROR_FILE_NOT_SUPPORTED_TYPE     0x00001206L   
    #define KELIB_ERROR_FILE_WRONG_TYPE_001         0x00001207L   
    #define KELIB_ERROR_FILE_READ_DIRECTORY_001     0x00001208L   
    #define KELIB_ERROR_FILE_INDEX_TO_HIGH_001      0x00001209L   
    #define KELIB_ERROR_FILE_EXTENSION_WRONG        0x0000120AL   
    #define KELIB_ERROR_FILE_WRITE_WRONG_SIZE       0x0000120BL   
    #define KELIB_ERROR_FILE_NOT_DELETED            0x0000120CL   
    #define KELIB_ERROR_FILE_WRONG_DATA_FORMAT      0x0000120DL   
    #define KELIB_ERROR_FILE_STATUS_READ            0x0000120EL   
    #define KELIB_ERROR_FILE_ID_NOT_NULL            0x0000120FL   
    #define KELIB_ERROR_READ_WRONG_SIZE             0x00001210L   

    #define KELIB_ERROR_SETUPFILE_NOT_READ          0x00001301L   

    // Error Sound
    #define KELIB_ERROR_SOUND_IS_PLAYING            0x00001401L   
    #define KELIB_ERROR_SOUND_UNKNOWN_ACTION        0x00001402L   
    #define KELIB_ERROR_SOUND_WRONG_DATA_POINTER    0x00001403L   
    #define KELIB_ERROR_SOUND_FILE_TO_BIG           0x00001404L   
    #define KELIB_ERROR_SOUND_FILE_TO_SMALL         0x00001405L   
    #define KELIB_ERROR_SOUND_THREAD_NOT_STOPPABLE  0x00001406L   
    #define KELIB_ERROR_SOUND_FILE_1                0x00001407L   
    #define KELIB_ERROR_SOUND_FILE_2                0x00001408L   
    #define KELIB_ERROR_SOUND_FILE_3                0x00001409L   
    
  
    // Errors SPI 
    #define KELIB_ERROR_SPI_OPEN_DEVICE             0x00001501L   
    #define KELIB_ERROR_SPI_SET_MODE                0x00001502L   
    #define KELIB_ERROR_SPI_SET_BITS_1              0x00001503L   
    #define KELIB_ERROR_SPI_SET_BITS_2              0x00001504L   
    #define KELIB_ERROR_SPI_SETUP_SPEED             0x00001505L   
    #define KELIB_ERROR_SPI_AT_CLOSE                0x00001506L   
    
    
    // Errors with Malloc
    #define KELIB_ERROR_NO_FREE_MEMORY              0x00001601L   
    
    
    // Errors with I2C
    #define KELIB_ERROR_I2C_OPEN_ERROR              0x00001701L    
    #define KELIB_ERROR_I2C_ADDR_ERROR              0x00001702L    
    #define KELIB_ERROR_I2C_READ_ERROR              0x00001703L    
    #define KELIB_ERROR_I2C_WRITE_ERROR             0x00001704L    
    #define KELIB_ERROR_I2C_WRITE_READ_ERROR        0x00001705L    
    #define KELIB_ERROR_I2C_SET_REG_ADR_ERROR       0x00001706L    
    #define KELIB_ERROR_I2C_WRONG_REG_SIZE_VAL      0x00001707L    
    #define KELIB_ERROR_I2C_WRONG_DATA_WIDTH        0x00001708L    
    #define KELIB_ERROR_I2C_SET_STOP_ERROR          0x00001709L    
    #define KELIB_ERROR_I2C_SET_SPEED_ERROR         0x0000170AL    
    #define KELIB_ERROR_I2C_WRONG_ERROR_MASK        0x0000170BL    
    #define KELIB_ERROR_I2C_SET_ADR_SIZE_10         0x0000170CL    
    #define KELIB_ERROR_I2C_SET_ADR_SIZE_7          0x0000170DL    
    #define KELIB_ERROR_I2C_NO_WRITE_NO_READ        0x0000170EL    
    #define KELIB_ERROR_I2C_COMMUNICATION           0x0000170FL    
    
    
    // Errors IP / WLAN
    #define KELIB_ERROR_IPADR_WRONG_1               0x00001800L    
    #define KELIB_ERROR_IPADR_WRONG_2               0x00001801L    
    #define KELIB_ERROR_GET_IP_1                    0x00001802L    
    #define KELIB_ERROR_GET_IP_2                    0x00001803L    
    #define KELIB_ERROR_NET_DEV_IS_NOT_LINKED       0x00001804L    
    #define KELIB_ERROR_DEV_BUSY_NO_SCAN            0x00001805L    




    // Error with COM-Port
    #define KELIB_ERROR_COM_OPEN_DEVICE             0x00001901L    
    #define KELIB_ERROR_COM_CLOSE_DEVICE            0x00001902L    
    #define KELIB_ERROR_SET_SERIAL_PORT             0x00001903L    

    #define KELIB_ERROR_WRITE_TO_SUB_CONTRL         0x00001920L    
    #define KELIB_ERROR_READ_FROM_SUB_CONTRL_1      0x00001921L    
    #define KELIB_ERROR_READ_FROM_SUB_CONTRL_2      0x00001922L    
    #define KELIB_ERROR_READ_FROM_SUB_CONTRL_3      0x00001923L    


    // Error with Communication / Net
    #define KELIB_ERROR_NO_NET_FOUND                0x00001A01L    
    #define KELIB_ERROR_NET_SOCKET_NO_INIT          0x00001A02L    
    #define KELIB_ERROR_NET_CREATE_SOCKET_1         0x00001A03L    
    #define KELIB_ERROR_NET_CREATE_SOCKET_2         0x00001A04L    
    #define KELIB_ERROR_NET_SET_SOCKET_OPT          0x00001A05L    
    #define KELIB_ERROR_NET_BIND_SOCKET             0x00001A06L    
    #define KELIB_ERROR_NET_BC_SEND_1               0x00001A07L    
    

    // Error with Messages
    #define KELIB_ERROR_MSG_NOT_INITIALISED         0x00001B01L    
    #define KELIB_ERROR_MSG_THREAD_IS_RUNNING       0x00001B02L    
    #define KELIB_ERROR_MSG_THREAD_NO_RUN           0x00001B03L    
    #define KELIB_ERROR_MSG_NO_START                0x00001B04L    
    #define KELIB_ERROR_MSG_UNKNOWN_HW_ID           0x00001B05L    
    #define KELIB_ERROR_MSG_UNKNOWN_OPTION          0x00001B06L    
    #define KELIB_ERROR_MSG_BUFFER_FULL             0x00001B07L    
    #define KELIB_ERROR_MSG_BUFFER_FULL_TIMEOUT     0x00001B08L    
    #define KELIB_ERROR_MSG_IS_BUSY_NO_START        0x00001B09L    
    #define KELIB_ERROR_MSG_BUFFER_EMPTY            0x00001B0AL    
    
    
    // RetCodes / Error with Download-C-Programm
    #define KELIB_IS_DLC_PROG                       0x00001C01L    
    #define KELIB_IS_RPP_PROG                       0x00001C02L    
    #define KELIB_ERROR_DLC_TXT_BUSY                0x00001C03L  
    #define KELIB_THREAD_NOT_STARTABLE              0x00001C04L
    




    // Error with MotorIOLib
    #define KELIB_ERROR_WRONG_CYLE_COUNT            0x00002802L  
    #define KELIB_ERROR_WRONG_TRANSFER_CODE         0x00002803L
    #define KELIB_ERROR_WRONG_SIZE_OF_RX_BYTES      0x00002804L
    #define KELIB_ERROR_SEND_TRANSFER_DATA          0x00002805L
    #define KELIB_ERROR_RECEIVE_TRANSFER_DATA       0x00002806L
    

    
    // Error IO Motorplatine
    #define KELIB_ERROR_WRONG_SETUP_UNI_INPUTS      0x00003001L     




/*
  
  
    #ifndef KE_C_DEFINITIONEN_ARM
        #include "ke-c.h"
    #endif


    

    //  ===========================================================================
    //  Extensions
    
    #define TXT_EXTENSION_NAME_LEN      25
    #define TXT_MAX_EXTENSIONS          8
    #define TXT_NUM_OF_MODULES          (TXT_MAX_EXTENSIONS + 1)

    struct TXT_EXTENSION_DATA
    {
        uint16_t    u16Nr;          // Number of Extension für RoboPro
        uint16_t    u16Reserved0;   // For later use
        uint32_t    u32Reserved0;   // For later use
        uint32_t    u32Reserved1;   // For later use
        
        uint16_t    u16Typ;         // Extension Typ
        char        caName[TXT_EXTENSION_NAME_LEN];
    };

    
    struct TXT_EXTENSION_BUS_DATA
    {
        uint16_t    u16AnzExtensions;   // Number of Connected Extensions
        
        struct TXT_EXTENSION_DATA  sExtData[TXT_MAX_EXTENSIONS];
    };


    typedef enum 
    {
        TXT_IF_MODE_MASTER = 0,     // 0 = Master TXT
        TXT_IF_MODE_EXTENSION_1     // 1 = Extension
    } eTXTMODE;



    //  ===========================================================================
    //  I2C API

    // #define I2C_API_STOP_KEEP_OPEN      0x10    // Bit 4:  1 = send no STOP
    // #define I2C_API_SPEED_400_KHZ       0x80    // Bit 7:  1 = 400 kHz


    // Status codes for status field in I2C callback functions
    enum I2cStatus
    {
        KELIB_I2C_SUCCESS = 0,          // 0: Successful end of command
        KELIB_I2C_READ_ERROR,           // 1: read error
        KELIB_I2C_WRITE_ERROR           // 2: write error
    };


    // Structure for I2C callback functions
    typedef struct s_I2C_CallBack
    {
        uint16_t    u16Value;           // read/write value
        uint16_t    u16Status;          // status code, see enum CB_I2Status
    } s_I2C_CB;


    // Pointer to the I2C callback function
    typedef void (*P_I2C_CallBack_Func)(struct s_I2C_CallBack *);



    //  ===========================================================================
    //  WLAN Setup
    #define TXT_WLAN_AP_MODE            0x01
    #define TXT_WLAN_CLIENT_MODE        0x02
    
    #define TXT_WLAN_SECURITY           0x04
    #define TXT_WLAN_SECURITY_NONE      0x00
    #define TXT_WLAN_SECURITY_WPA2      0x04
    
    #define TXT_WLAN_IP_                0x08
    #define TXT_WLAN_IP_DHCP            0x00
    #define TXT_WLAN_IP_STATIC          0x08
    
    #define TXT_WLAN0_DEVICE            1
    #define TXT_WLAN1_DEVICE            2
    
    
    #define TXT_NET_NAME_LEN            32
    #define TXT_MAX_NET_NAME_LEN        (32 + 2)
    #define TXT_MAX_NET_IPV4_LEN        20
    
    #define TXT_MSG_WITH_WLAN0          0               // Messages over WLAN 0
    #define TXT_MSG_WITH_WLAN1          1               // Messages over WLAN 1
    
    
    struct STRUCT_IP_DATA
    {
        char        caDevName[TXT_MAX_NET_NAME_LEN];    // Name of the Net Device
        
        char        caIpAdrStrg[TXT_MAX_NET_IPV4_LEN];  // Result: IP-Address in xxx.xxx.xxx.xxx Notation
        uint32_t    u32IpAddrHost;                      // Result: IP-Address Host-Byte-Order
        uint32_t    u32IpAddrNet;                       // Result: IP-Address Network-Byte-Order
        uint32_t    u32IpBcAddrNet;
        uint32_t    u32Flags;
        uint16_t    u16FlagIsRunning;                   // Result: 1=Link is Running
        uint16_t    u16DataOk;                          // false = Data not found
    };
    


    // Messages
    #define TXT_MSG_MODE_MASTER                 1       // TXT works as MMT (Message-Master-TXT)
    #define TXT_MSG_MODE_SLAVE                  0       // TXT works as MST (Message-Slave-TXT)
    
    #define FT_ROBO_TXT_IF_DEVICE               130     // From KeLib.h
    
    #define NUM_MESSAGE_DEVICES                 24      // Number of Message Devices
    
    #define DEVICE_NAME_LEN                     26      // Number of Characters (String with 25 CHAR + '\0')
    
                                                        // Message-System
    #define MSG_HWID_SELF                       0       // Message for himself
    #define MSG_HWID_SER                        1       // Message for the Robo-Interface serial port
    #define MSG_HWID_RF                         2       // Message for other Interfaces (over RF)
    #define MSG_HWID_RF_SELF                    3       // Message for other Interfaces (over RF) and for himself (back over RF)
    
    
    #define MSG_SEND_NORMAL                     0       // Message send "normal"
    #define MSG_SEND_OTHER_THAN_LAST            1       // Message send "if not the same as the last message"
    #define MSG_SEND_IF_NOT_PRESENT             2       // Message send "if not present in the send buffer"
    
    
    struct  STRUCT_MESSAGE_DEVICE
    {
        uint32_t        u32DevHdl;                      // Device Handle
        uint32_t        u32AddrHost;                    // IP-Address from Device (Host Byte Order)
        uint16_t        u16DevTyp;                      // Devicetyp
        uint16_t        u16FNr;                         // Funkrufnummer
        uint32_t        u32DevNr;                       // Devicenumber
        uint8_t         u8aName[DEVICE_NAME_LEN];       // Devicename ( e.g."TXT-9999")
    };

    
    struct  STRUCT_MESSAGE_DEVICE_LIST
    {   // Mesage-Master: Table with Message-Entries
        uint16_t    u16NumDevices;
        struct      STRUCT_MESSAGE_DEVICE   sMsgDev[NUM_MESSAGE_DEVICES];
    };


    
    #define MESSAGE_TYP_SLAVE       0   // Device works as Message Slave
    #define MESSAGE_TYP_MASTER      1   // Device works as Message Master
    
    #define MESSAGE_STOP            0   // Stop Messaging
    #define MESSAGE_START           1   // Start Messaging

    
    typedef union 
    {
        uint8_t     u8Data[6];
        
        struct 
        {
            uint8_t     u8HwId;                 // Hardware Id
            uint8_t     u8SubId;                // Sub-Id
            uint8_t     D0;
            uint8_t     D1;
            uint8_t     D2;
            uint8_t     D3;
        } B;
                
        struct
        {
            uint8_t     u8HwId;                 // Hardware Id
            uint8_t     u8SubId;                // Sub-Id
            uint16_t    u16MsgId;
            uint16_t    u16Msg;
        } W;
        
    } SMESSAGE;
    
    typedef void (*MSG_UPDATE_CALLBACK)(SMESSAGE *);
        


    typedef struct _NOTIFICATION_EVENTS 
    {
        // Callback-Procedure for Messaging
        MSG_UPDATE_CALLBACK     CallbackMessage;    // Pointer to the Callback-Procedure for Receiving Messages
    } 
    NOTIFICATION_EVENTS;




    // ********************************************************
    // KeLibTxt - Prototypes
    // ********************************************************
    
    // Init KeLib
    uint32_t InitKeLibTxt(void);
 
    // Close KeLib
    uint32_t CloseKeLibTxt(void);
    
    // Is KeLib initialised?
    // Return:
    #define KELIB_IS_NOT_INIT           0
    #define KELIB_IS_INIT               1
    uint32_t IsKeLibInit(void);                
   

    // Initialize some Variables for the Transfer Thread
    void InitSerToMotTransfer(void);
    
    // Transfers Data to the IO Module and gets the Inputs
    // u8CycleCount is a 8 Bit Counter only for validating the Transfer, 
    // normaly increment the Value each calling
    uint32_t SerIoTransfer(uint8_t u8CycleCount, uint8_t u8KzDownload); 
    
    // Flush serial buffers, in case something remained from some previous requests    
    void FlushSerPort(void);
       

    void ClearTransArea(void);
       
    // Switches all Motors off
    int SwitchAllOutputsOff(void);
   
   
    // Get Adress of Transfer Area
    FISH_X1_TRANSFER * GetKeLibTransferAreaMainAddress(void);
   
   
    // TXT Mode / Role
    uint32_t KeLibSetTxtModus(eTXTMODE eMode);
    
    // Get Extension Data
    uint32_t KeLibGetExtensionData(struct TXT_EXTENSION_BUS_DATA *);
    
    
    // Sound
    uint32_t KeLibPlaySound (uint16_t u16Action, uint16_t u16SoundNr, uint16_t u16Repeat, uint16_t u16Cmd);
    void KeLibResetSoundEngine (void);

   
    // I2C
    #define I2C_SPEED_100_KHZ       0
    #define I2C_SPEED_400_KHZ       1
    uint32_t KeLibI2cTransfer(uint8_t u8DevAddr, uint16_t u16NumWrite, uint8_t *pWriteData, uint16_t u16NumRead, uint8_t *pReadData, uint16_t u16Clock400kHz);
    
    
    // Net
    uint32_t GetIpAddressOfInterface(struct STRUCT_IP_DATA *psIpData);
    uint32_t CheckIfInterfaceIsLinked(char *pName);  
    uint32_t InitTxtKeLibIpPorts(uint32_t, uint32_t);  
    
    
    // Message
    uint32_t InitTxtMessage(uint32_t u32Network, uint32_t u32DevTyp, uint32_t u32DevId, char *pDeviceName);
    uint32_t NetSearchTxt(struct STRUCT_MESSAGE_DEVICE_LIST *);
    uint32_t StartMessage(NOTIFICATION_EVENTS *pEvents);
    uint32_t StopMessage(void);
    uint32_t SendFtMessage(uint8_t u8HwId, uint8_t u8SubId, uint32_t u32Message, uint32_t u32WaitTime, uint32_t u32Option);
    
    
    // DLC
    uint32_t KeLibCheckDlFile(char *pFileName);
    
    
    void SetMenu1MutexLock(void);
    void SetMenu1MutexUnlock(void);
  


*/
  

#endif

