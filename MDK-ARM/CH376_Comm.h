//----------------------//
#ifndef _CH376___
#define _CH376___

#define CMD_RET_SUCCESS 0x51
#define CND_RET_ABORT   0x5f

typedef uint8_t byte;



byte GET_IV_VER             =           0x01;
byte SET_BAURDRATE          =           0x02;
byte ENTER_SLEEP            =           0X03;
byte RESET_ALL              =           0x05;
byte CHECK_EXIST            =           0x06;
byte SET_SD0_INT            =           0x0b;
byte GET_FILE_SIZE          =           0x06;
byte SET_USB_MODE           =           0x15;
byte GET_STATUS             =           0x22;
byte RD_USB_DATA0           =           0x27;
byte WR_USB_DATA0           =           0x2c;
byte WR_REQ_DATA            =           0x2d;
byte WR_OFS_DATA            =           0x2e;
byte SET_FILE_NAME          =           0x2f;
byte DISK_CONNECT           =           0x30;
byte DISK_MOUNT             =           0x31;
byte FILE_OPEN              =           0x32;
byte FILE_ENUM_GO           =           0x33;
byte FILE_CREATE            =           0x34;
byte FILE_ERASE             =           0x35;
byte FILE_CLOSE             =           0x36;
byte DIR_INFO_READ          =           0x37;
byte DIR_INFO_SAVE          =           0x38;
byte BYTE_LOCATE            =           0x39;
byte BYTE_READ              =           0x3a;
byte BYTE_RD_GO             =           0x3B;
byte BYTE_WRITE             =           0x3c;
byte BYTE_WR_GO             =           0x3d;
byte DISK_CAPACITY          =           0x3e;
byte DISK_QUERY             =           0x3f;
byte DIR_CREATE             =           0x40;
byte SEG_LOCATE             =           0x4a;
byte SEG_READ               =           0x4b;
byte SEG_WRITE              =           0x4c;
byte DISK_BOC_CMD           =           0x50;
byte DISK_READ              =           0x54;
byte DISK_RD_GO             =           0x55;
byte DISK_WRITE             =           0x56;
byte DISK_WR_GO             =           0x57;

byte SET_USB                =           0x05;
byte SET_SD                 =           0x03;
byte USB_INT_SUCCESS        =           0x14;
#endif
