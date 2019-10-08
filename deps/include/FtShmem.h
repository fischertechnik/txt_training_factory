//=============================================================================
//              |
// Headerfile   |   FtShmem.h
//              |   Common header file for Library [ftMscLib] and ROBO-TX
//              |   Controller firmware
//              |
// Created      |   09.01.2009, by H.-Peter Classen
//              |
// Last Change  |   
//-----------------------------------------------------------------------------

#ifndef __FT_SHMEM_H__
// Protect against multiple file inclusion
#define __FT_SHMEM_H__


#define FIRMWARE_VER        0x011E      // firmware version is 1.30


#define IZ_COUNTER          4           // number of counter
#define IZ_PWM_CHAN         8           // number of pwm channels
#define IZ_MOTOR            4           // number of motor
#define IZ_UNI_INPUT        8           // number of universal ios


// 5 kOhm Range
#define R_MIN               10          // [Ohm]
#define R_MAX               4999        // [Ohm]
#define R_OVR               5000        // [Ohm]

// 15 kOhm Range
#define R2_MIN              30          // [Ohm]
#define R2_MAX              14999       // [Ohm]
#define R2_OVR              15000       // [Ohm]

// 10V Range
#define U_MAX               9999        // [mV]
#define U_OVR               10000       // [mV]

// Ultrasonic Sensor Range
#define ULTRASONIC_MIN      2           // [cm]
#define ULTRASONIC_MAX      1023        // [cm]
#define ULTRASONIC_OVR      1024        // [cm]
#define NO_ULTRASONIC       4096        // not present



// Fish.X1 Bus Address Identifier
// [fish_x1_bus_adr_e]
enum IFBusAdr
{
    BUS_ADR_INVALID = 0,
    BUS_ADR_WILDCARD,
    BUS_ADR_MASTER,
    BUS_ADR_SLAVE1,
    BUS_ADR_SLAVE2,
    BUS_ADR_SLAVE3,
    BUS_ADR_SLAVE4,
    BUS_ADR_SLAVE5,
    BUS_ADR_SLAVE6,
    BUS_ADR_SLAVE7,
    BUS_ADR_SLAVE8
};


//  mult-interface-mode status (slave)
#define SLAVE_OFFLINE       0
#define SLAVE_ONLINE        1

//  length of strings
#define HOSTNAME_LEN        16          // "ROBO-IF-xxxxxxxx"
#define BLUETOOTH_ADR_LEN   17          // "xx:xx:xx:xx:xx:xx"
#define DISPL_MSG_LEN_MAX   98
#define VERSION_STRLEN      15

#define INVALID_VALUE       -32768



#ifndef __ASSEMBLY__


// define IF role (Application - I/O), (Local or Slave)
// [shm_if_id_e]
enum ShmIfId
{
    LOCAL_IO = 0,           // Local I/O
    REMOTE_IO_1,            // Remote I/O Slave #1
    REMOTE_IO_2,            // Remote I/O Slave #2
    REMOTE_IO_3,            // Remote I/O Slave #3
    REMOTE_IO_4,            // Remote I/O Slave #4
    REMOTE_IO_5,            // Remote I/O Slave #5
    REMOTE_IO_6,            // Remote I/O Slave #6
    REMOTE_IO_7,            // Remote I/O Slave #7
    REMOTE_IO_8,            // Remote I/O Slave #8
    SHM_IF_CNT
};

// member of IF slaves
#define SLAVE_CNT_MAX       (SHM_IF_CNT-1)  // only Slaves = 8
#define IF08_MAX            SHM_IF_CNT      // Master + Slaves = 9


// [inp_mode_e]
enum InputMode
{
    MODE_U = 0,
    MODE_R,
    MODE_R2,
    MODE_ULTRASONIC,
    MODE_INVALID
};


// [pgm_state_e]
enum PgmState
{
    PGM_STATE_INVALID = 0,
    PGM_STATE_RUN,
    PGM_STATE_STOP
};


// Timer units for GetSystemTime hook function
enum TimerUnit
{
    TIMER_UNIT_INVALID        = 0,
    TIMER_UNIT_SECONDS        = 2,
    TIMER_UNIT_MILLISECONDS   = 3,
    TIMER_UNIT_MICROSECONDS   = 4
};


// [pgm_info_s], 8 bytes
typedef struct pgm_info_s
{
    char          * name;
    UINT8           state;        // enum PgmState    state;
    char            dummy[3];
} PGM_INFO;


// [display_msg_s], 100 bytes
typedef struct display_msg_s
{
    UINT8           id;
    char            text[DISPL_MSG_LEN_MAX + 1];
} FTX1_MSG;


// structure for received message, 36 bytes
typedef struct shm_if_msgrec_s
{
    FTX1_MSG        display_msg;
} FTX1_MSGREC;


// [display_frame_s], 8 bytes
typedef struct display_frame_s
{
    UCHAR8        * frame;
    UINT16          id;
    BOOL16          is_pgm_master_of_display;
} FTX1_FRAME;


// define version, [version_u], 4 bytes
typedef union version_u
{
    UINT32          abcd;
    struct
    {
        UCHAR8      a;
        UCHAR8      b;
        UCHAR8      c;
        UCHAR8      d;
    } part;
} FT_VERSION;


// fish.X1 version structure
// [shm_if_version_s], 16 bytes
typedef struct shm_if_version_s
{
    FT_VERSION      hardware;
    FT_VERSION      firmware;
    FT_VERSION      shm_if;
    FT_VERSION      fish_x1;
} FTX1_SHMVER;


//  ===========================================================================
//  Bluetooth API


// Number of Bluetooth channels
#define BT_CNT_MAX              8

// Allowed values for channel index are 1...8
#define BT_CHAN_IDX_MIN         1
#define BT_CHAN_IDX_MAX         8

#define BT_ADDR_LEN             6           // Bluetooth address length
#define BT_MSG_LEN              16          // max. Bluetooth message length


// Bluetooth connection states
enum BtConnState
{
    BT_STATE_IDLE = 0,              // BT channel is disconnected
    BT_STATE_CONN_ONGOING,          // BT channel is being connected
    BT_STATE_CONNECTED,             // BT channel is connected
    BT_STATE_DISC_ONGOING           // BT channel is being disconnected
};


// Status of Bluetooth inquiry scan
enum BtInquiryScanStatus
{
    BT_INQUIRY_SCAN_NOT_POSSIBLE = 0,
    BT_INQUIRY_SCAN_START,
    BT_INQUIRY_SCAN_RESULT,
    BT_INQUIRY_SCAN_BUSY,
    BT_INQUIRY_SCAN_TIMEOUT,
    BT_INQUIRY_SCAN_END
};


// Status codes for status field in Bluetooth callback functions
enum CB_BtStatus
{
  /*  0 */  BT_SUCCESS = 0,         // Successful end of command
  /*  1 */  BT_CON_EXIST,           // Already connected
  /*  2 */  BT_CON_SETUP,           // Establishing of connection is ongoing
  /*  3 */  BT_SWITCHED_OFF,        // Cannot connect/listen, Bluetooth is set to off
  /*  4 */  BT_ALL_CHAN_BUSY,       // Cannot connect, no more free Bluetooth channels
  /*  5 */  BT_NOT_ROBOTX,          // Cannot connect/listen, device is not a ROBO TX Controller
  /*  6 */  BT_CON_TIMEOUT,         // Cannot connect, timeout, no device with such a BT address
  /*  7 */  BT_CON_INVALID,         // Connection does not exist
  /*  8 */  BT_CON_RELEASE,         // Disconnecting is ongoing
  /*  9 */  BT_LISTEN_ACTIVE,       // Listen is already active
  /* 10 */  BT_RECEIVE_ACTIVE,      // Receive is already active
  /* 11 */  BT_CON_INDICATION,      // Passive connection establishment (incoming connection)
  /* 12 */  BT_DISCON_INDICATION,   // Passive disconnection (initiated by remote end)
  /* 13 */  BT_MSG_INDICATION,      // Received data (incoming message)
  /* 14 */  BT_CHANNEL_BUSY,        // No connect command is allowed when listen is active or
                                    // no listen command is allowed when connected
  /* 15 */  BT_BTADDR_BUSY,         // BT address is already used by another channel
  /* 16 */  BT_NO_LISTEN_ACTIVE     // Cannot connect, no active listen on remote end
};


// Bluetooth inquiry scan status
typedef struct bt_scan_status_s
{
    UINT16          status;         // status code, see enum BtInquiryScanStatus

    // Bluetooth device info, valid only when status == BT_INQUIRY_SCAN_RESULT
    UCHAR8          bt_addr[BT_ADDR_LEN];
    char            dummy_1[2];
    char            bt_name[HOSTNAME_LEN + 1];
    char            dummy_2[3];
} BT_SCAN_STATUS;


// Structure for Bluetooth callback functions (other than receive)
typedef struct bt_cb_s
{
    UINT16          chan_idx;       // channel index
    UINT16          status;         // status code, see enum CB_BtStatus
} BT_CB;


// Structure for Bluetooth receive callback function
typedef struct bt_receive_cb_s
{
    UINT16          chan_idx;       // channel index
    UINT16          status;         // status code, see enum CB_BtStatus
    UINT16          msg_len;        // message length
    UCHAR8          msg[BT_MSG_LEN];// Bluetooth message
} BT_RECV_CB;


// Bluetooth connection status structure, 8 bytes
typedef struct btstatus_s
{
    UINT16          conn_state;     // see enum BtConnState
    BOOL16          is_listen;      // if TRUE - BT channel is waiting for incoming connection (listening)
    BOOL16          is_receive;     // if TRUE - BT channel is ready to receive incoming messages
    UINT16          link_quality;   // 0...31, 0 - the worst, 31 - the best signal quality
} BT_STATUS;


struct shm_if_s;

// Pointer to the Bluetooth callback function (other than receive)
typedef void (*P_CB_FUNC)(struct shm_if_s *, BT_CB *);

// Pointer to the Bluetooth receive callback function
typedef void (*P_RECV_CB_FUNC)(struct shm_if_s *, BT_RECV_CB *);


//  ===========================================================================
//  I2C API

// Status codes for status field in I2C callback functions
enum CB_I2cStatus
{
  /*  0 */  I2C_SUCCESS = 0,        // Successful end of command
  /*  1 */  I2C_READ_ERROR,         // read error
  /*  2 */  I2C_WRITE_ERROR         // write error
};

// Structure for I2C callback functions
typedef struct i2c_cb_s
{
    UINT16          value;          // read/write value
    UINT16          status;         // status code, see enum CB_I2Status
} I2C_CB;

// Pointer to the I2C callback function
typedef void (*P_I2C_CB_FUNC)(struct shm_if_s *, I2C_CB *);


//  ===========================================================================
//  structures for Transfer Area


// fish.X1 shared memory interface info structure
// [ftX1info_s], 64 bytes
typedef struct ftX1info_s
{
    char            hostname[HOSTNAME_LEN + 1];
    char            btaddr[BLUETOOTH_ADR_LEN + 1];
    char            dummy;
    UINT32          SharedMemoryStart;
    UINT32          AppAreaStart;
    UINT32          AppAreaSize;
    FTX1_SHMVER     version;
} FTX1_SHMIFINFO;


// fish.X1 structure [ftX1state_s], 100 bytes
typedef struct ftX1state_s
{
    // used by local application
    BOOL8           init;
    BOOL8           config;
    char            dummy[2];
    UINT32          trace;

    // public state info
    BOOL8           io_mode;
    UINT8           id;
    UINT8           info_id;
    UINT8           config_id;
    BOOL8           io_slave_alive[SLAVE_CNT_MAX];
    BT_STATUS       btstatus[BT_CNT_MAX];
    PGM_INFO        master_pgm;
    PGM_INFO        local_pgm;
} FTX1_STATE;


// fish.X1 [struct uni_inp_config], 4 bytes
typedef struct uni_inp_config
{
    UINT8           mode;        // enum InputMode  mode
    BOOL8           digital;
    char            dummy[2];
} UNI_CONFIG;


// fish.X1 [struct cnt_inp_config], 4 bytes
typedef struct cnt_inp_config
{
    UINT8           mode;        // enum InputMode  mode;
    char            dummy[3];
} CNT_CONFIG;


// fish.X1 config structure
// [shm_if_config_s], 88 bytes
typedef struct ftX1config
{
    UINT8           pgm_state_req;        // enum PgmState    pgm_state_req;
    BOOL8           old_FtTransfer;
    char            dummy[2];
    BOOL8           motor[IZ_MOTOR];
    UNI_CONFIG      uni[IZ_UNI_INPUT];
    CNT_CONFIG      cnt[IZ_COUNTER];
    INT16           motor_config[IZ_MOTOR][4];
} FTX1_CONFIG;


// fish.X1 input structure
// [shm_if_input_s], 68 bytes
typedef struct ftX1input
{
    INT16           uni[IZ_UNI_INPUT];
    INT16           cnt_in[IZ_COUNTER];
    INT16           counter[IZ_COUNTER];
    INT16           display_button_left;
    INT16           display_button_right;
    // Set to 1 when last requested counter reset was fulfilled
    BOOL16          cnt_resetted[IZ_COUNTER];
    // Set to 1 by motor control if target position is reached
    BOOL16          motor_ex_reached[IZ_MOTOR];
    // Counter reset command id of the last fulfilled counter reset
    UINT16          cnt_reset_cmd_id[IZ_COUNTER];
    // Motor extended command id of the last fulfilled motor_ex command
    UINT16          motor_ex_cmd_id[IZ_MOTOR];
} FTX1_INPUT;


// fish.X1 output structure, only out values, 44 bytes
typedef struct ftX1output
{
    // Counter reset requests (increment each time by one)
    UINT16          cnt_reset_cmd_id[IZ_COUNTER];
    // If not 0, synchronize this channel with the given channel (1:channel 0, ..)
    UINT8           master[IZ_MOTOR];
    // User program selected motor PWM values
    INT16           duty[IZ_PWM_CHAN];
    // Selected distane (counter value) at which motor shall stop
    UINT16          distance[IZ_MOTOR];
    // Increment by one each time motor_ex settings change
    UINT16          motor_ex_cmd_id[IZ_MOTOR];
} FTX1_OUTPUT;


// fish.X1 output structure, with display message, 108 bytes
typedef struct _ftX1display
{
    FTX1_MSG        display_msg;
    FTX1_FRAME      display_frame;
} FTX1_DISPLAY;


// status of transferarea (ftMscLib), 4 bytes
typedef struct _transfer_status
{
    UINT8           status;             //  status transfer area (X1)
    UINT8           iostatus;           //  status io communication
    UINT16          ComErr;             //  system error code by connection error
} TRANSFER_STATUS;


// change fields for UniIO, Counter, Timer, Update status, 8 bytes
typedef struct _change_state
{
    UINT16          UpdInterface;
    UINT8           ChangeStatus;
    UINT8           ChangeUni;
    UINT8           ChangeCntIn;
    UINT8           ChangeCounter;
    UINT8           ChangeTimer;
    UINT8           reserved;
} CHANGE_STATE;


// 16-bit timers used by RoboPro, 12 bytes
typedef struct _rp_timer
{
    UINT16          Timer1ms;
    UINT16          Timer10ms;
    UINT16          Timer100ms;
    UINT16          Timer1s;
    UINT16          Timer10s;
    UINT16          Timer1min;
} RP_TIMER;


// motor values and debug, 24 bytes
typedef struct _motor
{
    // Motor PWM values
    INT16           duty[IZ_PWM_CHAN];
    // Values used for debugging motor control
    INT16           debug[IZ_MOTOR];
} MOTOR;


// Button input simulation, 4 bytes
typedef struct _input_sim
{
    INT16           simButtons[2];
} INPUT_SIM;


// Hook table with pointers to the functions,
// that can be called by RoboPro, 132 bytes
typedef struct _hook_table
{
    BOOL32  (*IsRunAllowed)             (void);
    UINT32  (*GetSystemTime)            (enum TimerUnit unit);
    void    (*DisplayMsg)               (struct shm_if_s * p_shm, char * p_msg);
    BOOL32  (*IsDisplayBeingRefreshed)  (struct shm_if_s * p_shm);
    void    (*BtConnect)                (UINT32 channel, UCHAR8 * btaddr, P_CB_FUNC p_cb_func);
    void    (*BtDisconnect)             (UINT32 channel, P_CB_FUNC p_cb_func);
    void    (*BtSend)                   (UINT32 channel, UINT32 len, UCHAR8 * p_msg, P_CB_FUNC p_cb_func);
    void    (*BtStartReceive)           (UINT32 channel, P_RECV_CB_FUNC p_cb_func);
    void    (*BtStopReceive)            (UINT32 channel, P_RECV_CB_FUNC p_cb_func);
    void    (*BtStartListen)            (UINT32 channel, UCHAR8 * btaddr, P_CB_FUNC p_cb_func);
    void    (*BtStopListen)             (UINT32 channel, P_CB_FUNC p_cb_func);
    char   *(*BtAddrToStr)              (UCHAR8 * btaddr, char * str);
    void    (*I2cRead)                  (UCHAR8 devaddr, UINT32 offset, UCHAR8 flags, P_I2C_CB_FUNC p_cb_func);
    void    (*I2cWrite)                 (UCHAR8 devaddr, UINT32 offset, UINT16 data, UCHAR8 flags, P_I2C_CB_FUNC p_cb_func);
    INT32   (*sprintf)                  (char * s, const char * format, ...);
    INT32   (*memcmp)                   (const void * s1, const void * s2, UINT32 n);
    void   *(*memcpy)                   (void * s1, const void * s2, UINT32 n);
    void   *(*memmove)                  (void * s1, const void * s2, UINT32 n);
    void   *(*memset)                   (void * s, INT32 c, UINT32 n);
    char   *(*strcat)                   (char * s1, const char * s2);
    char   *(*strncat)                  (char * s1, const char * s2, UINT32 n);
    char   *(*strchr)                   (const char * s, INT32 c);
    char   *(*strrchr)                  (const char * s, INT32 c);
    INT32   (*strcmp)                   (const char * s1, const char * s2);
    INT32   (*strncmp)                  (const char * s1, const char * s2, UINT32 n);
    INT32   (*stricmp)                  (const char * s1, const char * s2);
    INT32   (*strnicmp)                 (const char * s1, const char * s2, UINT32 n);
    char   *(*strcpy)                   (char * s1, const char * s2);
    char   *(*strncpy)                  (char * s1, const char * s2, UINT32 n);
    UINT32  (*strlen)                   (const char * s);
    char   *(*strstr)                   (const char * s1, const char * s2);
    char   *(*strtok)                   (char * s1, const char * s2);
    char   *(*strupr)                   (char * s);
    char   *(*strlwr)                   (char * s);
    INT32   (*atoi)                     (const char * nptr);
} HOOK_TABLE;




    #define     NUM_OF_IR_RECEIVER              4
    #define     IR_RECEIVER_IDX_ALL_DATA        0       // Index for IR-Array: with all Data 
    #define     IR_RECEIVER_IDX_OFF_OFF         1       // Index for IR-Array: with Data on Frequency 1 and Receiver 1 (Switches: Off | Off )
    #define     IR_RECEIVER_IDX_ON_OFF          2       // Index for IR-Array: with Data on Frequency 1 and Receiver 2 (Switches: Off | On )
    #define     IR_RECEIVER_IDX_OFF_ON          3       // Index for IR-Array: with Data on Frequency 2 and Receiver 1 (Switches: On  | Off )
    #define     IR_RECEIVER_IDX_ON_ON           4       // Index for IR-Array: with Data on Frequency 2 and Receiver 2 (Switches: On  | On )
    
    #define     NUM_OF_TXT_MODULES              2   


// --------------------------------------------
// New TXT Data
// --------------------------------------------
typedef struct  _IR_DATA
{
    INT16   i16JoyLeftX;                // Value of left Joystick X-Axis  (0=middle -15..0..+15)
    UINT16  u16JoyLeftXtoLeft;          // unsigned-Int Value of left Joystick X-Axis from middle to left maximum
    UINT16  u16JoyLeftXtoRight;         // unsigned-Int Value of left Joystick X-Axis from middle to right maximum

    INT16   i16JoyLeftY;                // Value of left Joystick Y-Axis  (0=middle -15..0..+15)
    UINT16  u16JoyLeftYtoForward;       // unsigned-Int Value of left Joystick Y-Axis from middle to forward maximum
    UINT16  u16JoyLeftYtoBackwards;     // unsigned-Int Value of left Joystick Y-Axis from middle to backwards maximum

    INT16   i16JoyRightX;               // Value of right Joystick X-Axis  (0=middle -15..0..+15)
    UINT16  u16JoyRightXtoLeft;         // unsigned-Int Value of right Joystick X-Axis from middle to left maximum
    UINT16  u16JoyRightXtoRight;        // unsigned-Int Value of right Joystick X-Axis from middle to right maximum

    INT16   i16JoyRightY;               // Value of right Joystick Y-Axis  (0=middle -15..0..+15)
    UINT16  u16JoyRightYtoForward;      // unsigned-Int Value of right Joystick Y-Axis from middle to forward maximum
    UINT16  u16JoyRightYtoBackwards;    // unsigned-Int Value of right Joystick Y-Axis from middle to backwards maximum

    UINT16  u16ButtonOn;                // ON-Switch: 1=pressed
    UINT16  u16ButtonOff;               // OFF-Switch: 1=pressed

    UINT16  u16DipSwitch1;              // 1: Switch ON, 0: Switch OFF
    UINT16  u16DipSwitch2;              // 1: Switch ON, 0: Switch OFF
} KE_IR_INPUT_V01;


typedef struct _TXT_SPECIAL_INPUTS
{
    // Supply voltage
    UINT16  u16TxtPower;
    // Processor temperature
    UINT16  u16TxtTemp;
    // Reference voltage
    UINT16  u16TxtRef;
    // ??
    UINT16  u16TxtVB;
    
    // Data from the IR-Inputs
    // [IR_RECEIVER_IDX_ALL_DATA]:IR Data for all receivers
    // [IR_RECEIVER_IDX_OFF_OFF]: IR Data for receivers with SW1=0  SW2=0
    // [IR_RECEIVER_IDX_OFF_ON]:  IR Data for receivers with SW1=0  SW2=1
    // [IR_RECEIVER_IDX_ON_OFF]:  IR Data for receivers with SW1=1  SW2=0
    // [IR_RECEIVER_IDX_ON_ON]:   IR Data for receivers with SW1=1  SW2=1 
    KE_IR_INPUT_V01     sIrInput[NUM_OF_IR_RECEIVER+1]; 

    // Id of sound command - set to sTxtInputs.u16SoundCmdId if firmware finished processing the command
    UINT16  u16SoundCmdId;
    
    // Date & Time
    UINT16  u16Sec;
    UINT16  u16Min;
    UINT16  u16Hour24;
    UINT16  u16Hour12;
    UINT16  u16PmFlag;          // 1 = PM Time
    UINT16  u16MDay;            // Day of Month (Range 1..31)
    UINT16  u16Month;
    UINT16  u16Year;
    UINT16  u16WDay;            // Week Day (Range 0..6 or 1..7 ???)
    
} TXT_SPECIAL_INPUTS;

typedef struct _TXT_SPECIAL_INPUTS_2
{
    // Microphone
    INT16  u16MicLin;
    INT16  u16MicLog;
} TXT_SPECIAL_INPUTS_2;

typedef struct _TXT_SPECIAL_OUTPUTS
{
    // Id of sound command - incremented whenever a new command is sent
    UINT16  u16SoundCmdId;
    // Index of the sound to play - 0 means stop sound
    UINT16  u16SoundIndex;
    // Repeat count for playing sound
    UINT16  u16SoundRepeat;
    
    // LED color (start button)
    UINT8 u8LEDColorR;
    UINT8 u8LEDColorG;
    UINT8 u8LEDColorB;
} TXT_SPECIAL_OUTPUTS;

// ============================================================================
//  transferarea of ROBO TX Controller
//-----------------------------------------------------------------------------
#define RESERVE_SIZE \
    (1024 - ( \
    sizeof(FTX1_SHMIFINFO)  + \
    sizeof(FTX1_STATE)      + \
    sizeof(FTX1_CONFIG)     + \
    sizeof(FTX1_INPUT)      + \
    sizeof(FTX1_OUTPUT)     + \
    sizeof(FTX1_DISPLAY)    + \
    sizeof(TRANSFER_STATUS) + \
    sizeof(CHANGE_STATE)    + \
    sizeof(RP_TIMER)        + \
    sizeof(MOTOR)           + \
    sizeof(INPUT_SIM)       + \
    sizeof(HOOK_TABLE)      + \
    sizeof(TXT_SPECIAL_INPUTS) + \
    sizeof(TXT_SPECIAL_OUTPUTS) + \
    sizeof(TXT_SPECIAL_INPUTS_2) \
    ))


typedef struct shm_if_s
{
    FTX1_SHMIFINFO      ftX1info;       // info structure
    FTX1_STATE          ftX1state;      // state structure
    FTX1_CONFIG         ftX1config;     // config structure   
    FTX1_INPUT          ftX1in;         // input structure
    FTX1_OUTPUT         ftX1out;        // output structure
    FTX1_DISPLAY        ftX1display;    // display structure

    TRANSFER_STATUS     IFStatus;
    CHANGE_STATE        IFChange;       // change state of Input, Counter, Timer
    RP_TIMER            IFTimer;        // 16-Bit timer variables
    MOTOR               IFMotor;        // motors control
    INPUT_SIM           IFInputSim;     // input simulation
    HOOK_TABLE          IFHookTable;    // hook table with functions pointers
    
    TXT_SPECIAL_INPUTS  sTxtInputs;     // TXT Special Inputs (Power, Temp, IR...)  
    TXT_SPECIAL_OUTPUTS sTxtOutputs;    // TXT Special Outputs (sound, LED)  
    TXT_SPECIAL_INPUTS_2 sTxtInputs2;     // TXT Special Inputs (Power, Temp, IR...)

    char                reserved[RESERVE_SIZE];
} FISH_X1_TRANSFER;



#endif // __ASSEMBLY__

#endif
