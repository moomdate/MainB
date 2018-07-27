/* line:1037
Setting UTF-8 in keil ide for display thai charactor
1. open keil
2. go to menu file select Configuration
3. in tab Editor > Encoding > Endcode in UTF-8 without signature
*/

//#include "stm32f10x.h"
#include "systick.h"
#include <ctype.h> //use toupper
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h> //strtol
#include "stm32f10x_spi.h"

#define RCC_APB2Periph_GPIO_SPI_FLASH_CS RCC_APB2Periph_GPIOD
#define SPI_DISPLAY_CS_PORT GPIOD
#define SPI_DISPLAY_CS_PIN GPIO_Pin_4

//-------------------------------------define mode-------------------------------
#define mode_on_meno 0
#define mode_notepad 1
#define mode_read 2
#define mode_bluetooth 3
//--------------------------------spi display----------------------------------
#define SPI_DISPLAY_CS_LOW() GPIO_ResetBits(GPIOD, GPIO_Pin_4) //SPI_FLASH_CS_LOW
#define SPI_DISPLAY_CS_HIGH() GPIO_SetBits(GPIOD, GPIO_Pin_4)  //SPI_FLASH_CS_HIGH

//--------------------------------spi flash------------------------------------
#define SPI_FLASH_CS_LOW() GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_FLASH_CS_HIGH() GPIO_SetBits(GPIOA, GPIO_Pin_4)

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (opti
  on LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address ((u32)0x4001244C)

#define delayCur 250000 // delay time for cursor blink
#define sector 4096     //amount char in one sector
#define MaxInLine 42    //char for read in one line
#define CRLF 2
void writeFlash(int address, int size);
void writeFlash2(int address, int size);
int addressWriteFlashTemp = 0x0;
int strLength = 0;
int countSector4096 = 0;
int countSector = 0;
int addressSector = 0x00;
int endReadFile = 0;
char buffer22Char[42];
int pointer22char = 0;
int pointerSector = 0; //current sector
int pointerSectorStatus = 0;
int jumpCRLF = 0;
int AmountSector = 0;
int AmountSectorT = 0;
int NextLenghtLess20 = 0;
int NextPoint = 0;
int maxLengthLoopL22 = 22;
int countLFTwoStep = 0;
int TempPointer22char = 0;
int varForLoop = 0;
int readPreviousSector = 0;
int countLengthInLine = 0;
int seeCur = 0;
int breakOpenDir = 0;

uint16_t *str;
char buffer[6];
unsigned int BLEConBuffer[30];
int keyCodeRecieve[7];
int countKey = 0;
int seeHead = 0;
int maxData = 7;
int i = 0, j = 0, k = 0;
int i1 = 0, j1 = 0, count1 = 0; //uart2
int uart2Buffer = 0;
int maxSize = 5;
int count = 0;
int count2 = 0;
int Hex;
int isHex = 0; // if equal 1 is hex, or equal 2 is Switch
int itFirst = 0;
int BLEConn = 0;
int pushToDisplay = 0;
int countConn = 0;

int bufferKey3digit[3] = {0};

int hexbuffer = 0;
int hexPrep = 0;
int SeeHead = 0;
int dataLength = 0;
//----------------*------ch376 command-------------------------------
int data0[] = {0x57, 0xab, 0x27};
int enumgo[] = {0x57, 0xab, 0x33};
int FileOpen[] = {0x00, 0x57, 0xab, 0x32};
int ResetAll[] = {0x00, 0x57, 0xab, 0x05};
int ReadData[] = {0x00, 0x57, 0xAB, 0x27};
int FileCreate[] = {0x00, 0x57, 0xab, 0x34};
int FileUpdate[] = {0x00, 0x57, 0xab, 0x3d};
int USBDiskMount[] = {0x00, 0x57, 0xab, 0x31};
int continueRead[] = {0x00, 0x57, 0xab, 0x3b};
int FileClose[] = {0x00, 0x57, 0xab, 0x36, 0x01};
int setSDCard[] = {0x00, 0x57, 0xab, 0x15, 0x03}; //+
int setFileName[15] = {0x00, 0x57, 0xab, 0x2f, 0x2f};
int checkConnection[] = {0x00, 0x57, 0xab, 0x06, 0x57};
int SetByteRead[] = {0x00, 0x57, 0xab, 0x3a, 0x80, 0x00};      //15 = 21 character
int setFilenameForFunction[] = {0x00, 0x57, 0xab, 0x2f};       // use vie function set file name
int setAllName[] = {0x00, 0x57, 0xab, 0x2f, 0x2f, 0x2a, 0x00}; //*
int SetByteRead6char[] = {0x00, 0x57, 0xab, 0x3a, 0x06, 0x00};
int changeBaudRateByte[] = {0x00, 0x57, 0xab, 0x02, 0x03, 0x98};
int FilePointer[] = {0x00, 0x57, 0xab, 0x39, 0x00, 0x00, 0x00, 0x00};
int FilePointerend[] = {0x00, 0x57, 0xab, 0x39, 0xff, 0xff, 0xff, 0xff};
/////////////////////////////////////////////////////////////////////
//---------------------- menu variable ------------------------------
int count_menu = 1;
int maxMenu = 3;
int menu[6] = {0x01, 0x02, 0x04, 0x80, 0x04, 0x02};

int countMenuInReadMode = 0;
int st_bluetooth[] = {0x43, 0x07, 0x25, 0x11, 0x1e, 0x15, 0x15, 0x1e, 0x13}; //bluetooth
int st_0[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
int st_notepad[] = {0x1d, 0x15, 0x1e, 0x11, 0x0f, 0x01, 0x19}; //notepad
int st_notepad_mode[] = {0x1d, 0x15, 0x1e, 0x11, 0x0f, 0x01, 0x19, 0x00, 0x0d, 0x15, 0x19, 0x11};
int st_filemanage[] = {0x0b, 0x0a, 0x07, 0x11, 0x0d, 0x01, 0x1d, 0x01, 0x1b, 0x11};
int st_read[] = {0x17, 0x11, 0x01, 0x19};
int st_usbconnect[] = {0x25, 0x0e, 0x03, 0x00, 0x09, 0x15, 0x1d, 0x1d, 0x11, 0x09, 0x1e};
int st_clock[] = {0x09, 0x07, 0x15, 0x09, 0x05};
int st_calcu[] = {0x09, 0x01, 0x07, 0x09, 0x25, 0x07, 0x01, 0x1e, 0x15, 0x17};
int st_tools[] = {0x1e, 0x15, 0x15, 0x07, 0x0e};
int fullScreen[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
int read_mode[] = {0x57, 0x51, 0x41, 0x59, 0x00, 0x4d, 0x55, 0x59, 0x51};
int bigL[] = {0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47};
int mode_1[4] = {0x00, 0x01, 0x02};
int mode = 0; //1,2,3,4,5,6,7,8

////////////////////////////////////////////////////////////////
int checkKeyError = 0;
int keyCode = 0;
int previousMode = 0;

//----------------------------------------- BLE data --------------------------------------------------//
////////////////////////////////////////////
//54 53 53 4d 20 56 31 2e 30  --TSSM V1.0 //
//53 4c 52 49 20 56 31 2e 30  --SLRI V1.0 //
////////////////////////////////////////////
int connectData[] = {0xff, 0xff, 0xa2, 0x0b, 0x16, 0x14, 0x10, 0x53, 0x4c, 0x52, 0x49, 0x20, 0x56, 0x31, 0x2e, 0x30};
int navRight[] = {0xff, 0xff, 0xa6, 0x03, 0x00, 0x08, 0x00};
int navLeft[] = {0xff, 0xff, 0xa6, 0x03, 0x00, 0x04, 0x00};

//-----------------------------------------End BLE data-----------------------------------------------//
//-----------------------------------------ch370s data------------------------------------------------//

int countSector512 = 0;
int countRoundSector512 = 0;
int countCounitueRead = 0; //512 ,round =24
char bufferFromRead[25] = "";
char bufferPrevoisRead[1000][25];
int CountbufferFromRead = 0;
int startRead = 0;
int countPageWhenRead = 0;
int continueReadFile = 0;
int openFileStatus = 0;
int readstatus = 0;
int becon = 0;

int setFileNameLength = 5;
int DiskconnectStatus[] = {0x00, 0x57, 0xab, 0x03}; //+
int openDirStatus = 0;
//--readmode
char dataTemp512[512];
int countdataTemp512 = 0;
int waitEnd = 0; // 10
int lastAscii = 0;

//int sendNull[] = {0};

int searchStep = 0;
int stepCount = 0;
int r_count = 0;
int nextAgain = 0;
int countFileLegth = 0;
char prepareNameToOpen[] = "";
int FileWrite[] = {0x00, 0x57, 0xab, 0x2d};

int command_ = 0;
int DirName[] = {0x00, 0x57, 0xab, 0x2f, 0x2f, 0x54, 0x45, 0x53, 0x54, 0x00};

int SwitchEndFileName = 0;
int endFileName = 0;

int d_Time = 0;
int toggleCur = 0;
int tempCur = 0;

int unicodeTable[] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    // !"#$%&'()*+,-./
    0x00, 0x2e, 0x10, 0x3c, 0x28, 0x3f, 0x2f, 0x20, 0x26, 0x34, 0x14, 0x16, 0x02, 0x24, 0x04, 0x32,
    /*0-9*/
    0x2c, 0x21, 0x23, 0x29, 0x39, 0x31, 0x2b, 0x3b, 0x33, 0x2a,
    /*:  ;  */
    0x12, 0x06, 0x30, 0x36, 0x18, 0x22, 0x5c,
    /*A-Z*/
    0x41, 0x43, 0x49, 0x59, 0x51, 0x4b, 0x5b, 0x53, 0x4a, 0x5a, 0x45, 0x47, 0x4d, 0x5d, 0x55, 0x4f,
    0x5f, 0x57, 0x4e, 0x5e, 0x65, 0x67, 0x7a, 0x6d, 0x7d, 0x75 /*Z*/,
    0x77, 0x4c, 0x7e, 0x6e, 0x78, 0x1c, /*symbol*/ //checked
    0x01, 0x03, 0x09, 0x19, 0x11, 0x0b, 0x1b, 0x13, 0x0a, 0x1a, 0x05, 0x07, 0x0d, 0x1d, 0x15, 0x0f,
    0x1f, 0x17, 0x0e, 0x1e, 0x25, 0x27, 0x3a, 0x2d, 0x3d, 0x35, /*z*/

    0x37, 0x33, 0x3e, 0x2e, 0xff /*{|}*/
};
//------------------------------------------ ch370t data ------------------------------------//'
char filelist[10][15];
int seaching = 1; //for seaching file

/* Private function prototypes -----------------------------------------------*/

///------------------------------------spi---------------------------------//
void keyboardMode(void);
void SendCommandToBLE2(int);
void Init_SPI(void);
void delay_SPI(void);
void sentdata_cell(int data);
void cell_sentdata(int cell);
uint8_t Flash_ReadWriteByte2(uint8_t data);
/*------------------------------------------------------------------------*/
void GPIO_Configuration(void);
void USART1_Configuration(void);
void USART2_Configuration(void);
void USART3_Configuration(void);
void USART3_Configuration2(void);
void UART4_Configuration(void);
void ADC_Configuration(void);
void sendUart(int);
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);
void ConnectBLE(void);
void BluetoothMode(void);
void SendCommandToBLE(int data[], int sizeOfData);

void printDot(int data[], int size);
void stringToUnicodeAndSendToDisplay(char *str);
void stringToUnicodeAndSendToDisplayC(char *string, int po); //cur position

void clearDot(void);
void menu_s(void);
void menu_CH376(void); //for ch376s
void caseMenu(int);
void copy_string(char *target, char *source);
int fileWrite(int k, char *filename, char *string);
void writeFile4096(char *fname, char *strSource);
const char *fileName(void);

char DataForWrite[32] = "";
char DataForWrite_aftersort[32] = "";
const char *FileName_buffer = DataForWrite;

//--- sd card---//
void ReadFile(void);
int createFile(char *name);
int CreateFile__ = 1;
void appendFile(void);
void SendCH370(int data[], int);
void changeBuatRate(void);
void setFilename(char *name);

//--------------------------------new prototype  ------------------------------
void searchFile2(void);
void NextFile(void);
void keyRead(void);
void exitOncePath(void);
void cdWithPath(char *path);
int checkFileType(char *file);
int openDir(char *dirPath);
int savePath(char *pathName);
int checkSlash(char *pathName);
int countPath(char *pathSource);

int maxFile = 0;
int fileSelect = 0;

int readFileFromCH376sToFlashRom(char *filename);
void slidingFileFromRomToDisplay(void);

char Dirpath[30] = "";
char fileLists[30][15];
USART_InitTypeDef USART_InitStructure36;
//////////////////////////////////////////////////////////////////////////////

float AD_value;
vu16 ADC_ConvertedValue;

//------------select uart---------/

int sendUart1 = 0;
int sendUart2 = 0;
int sendUart3 = 0;
int sendUart4 = 0;

/*-----------------------------spi flash------------------------------------------*/
extern void SPI_Flash_Init(void);

extern unsigned char SST25_buffer[];
extern unsigned char SST25_buffer99[];

unsigned char SST25_buffer88[] = {0x0a, 0x0d};
/* Private define ------------------------------------------------------------*/
#define TxBufferSize1 (countof(TxBuffer1) - 1)
#define RxBufferSize1 (countof(TxBuffer1) - 1)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/

uint8_t TxBuffer1[] = "SPI SST25VF016B Example: This is SPI DEMO, 999888888888888888888888888888";
uint8_t NbrOfDataToTransfer1 = TxBufferSize1;

void Delay(uint32_t nCount); //--delay spi
//GPIO_InitTypeDef GPIO_InitStructure;
int16_t USART_FLAG;

extern void SST25_W_BLOCK(uint32_t addr, u8 *readbuff, uint16_t BlockSize);
extern void SST25_R_BLOCK(uint32_t addr, u8 *readbuff, uint16_t BlockSize);

// John global Value
char str1[sector];
char str2[22];

SPI_InitTypeDef SPI_InitStructure;
void configFlash(void)
{
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
}
void configDisplay(void)
{
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
}

/*******************************************************************************
  Function Name  : main
  Description    : Main program
  Input          : None
  Output         : None
  Return         : None
  Attention      : None
*******************************************************************************/
//ฮัลโหล
// -----------------------change buat rate-------------------
//
/////////////////////////////////////////////////////////////
void changeBuatRate()
{
  SendCH370(changeBaudRateByte, sizeof(changeBaudRateByte));
  delay_ms(50);
  /*if (USART_GetITStatus(USART3, USART_IT_RXNE) ) {
    i1 = USART_ReceiveData(USART3);
    printf("%c\r\n", i1);
    }*/
  printf("change 57600\r\n");
}
void prepareSD()
{
  int preStatus = 1;
  command_++;
  while (preStatus)
  {
    if (command_ == 1)
    {
      SendCH370(checkConnection, sizeof(checkConnection));
      printf(" check connecttion \r\n");
      command_++; //2
      delay_ms(45);
    }
    else if (command_ == 2)
    {
      SendCH370(setSDCard, sizeof(setSDCard));
      printf(" set sd card\r\n");
      command_++; //4
      delay_ms(45);
    }
    else if (command_ == 3)
    {
      SendCH370(USBDiskMount, sizeof(USBDiskMount));
      printf(" usb disk mount \r\n");
      command_++; //6
      delay_ms(45);
    }
    else if (command_ == 4)
    {
      SendCH370(setAllName, sizeof(setAllName));
      printf(" set all file \r\n");
      command_++; //10
      delay_ms(45);
    }
    else if (command_ == 5)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      printf(" file open \r\n");
      command_++; //10
      delay_ms(45);
    }
    else if (command_ == 6)
    {
      //SendCH370(FileClose, sizeof(FileClose));
      //printf(" File Close \r\n");
      command_++; //10
      delay_ms(45);
      preStatus = 0;
    }
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      printf("%x\r\n", i1);
    }
  }
  printf("End prepare\r\n");
}
void beepError()
{
  USART_SendData(USART2, 155);
  delay_ms(45);
}
void errorBreak()
{
  USART_SendData(USART2, 151);
  delay_ms(45);
}
int getBatterry()
{
  USART_SendData(USART2, 150);
  delay_ms(45);
  if (USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    uart2Buffer = USART_ReceiveData(USART2);
  }
  return uart2Buffer;
}

//------------------------notepad function and variable---------------------------
//ฟังค์ชั่นหลักของ notepad
void notepad_main(void);
//อ่านค่าคีย์บอร์ด
void notepad_readKey(void);
//สำหรับให้เคอเซอร์ชี้ตำแหน่งของสตริงตัวล่าสุดของบรรทัด
int notepad_getnullPostion(char *);
int notepad_countLinewithOutLNsign(char *str);
int notepad_checkEnterSignInLine(char *str);
void notepad_removeEnterSign(char *str);
int notepad_countEnterSign(char *str);
void notepad_fillEnterSign(char *str);
void notepad_append(char subject[], const char insert[], int pos);
void printStringLR(char *str, int p);
int notepad_countStr(char *data);
char numToASCII(int num);

char *subStringLanR(char *str, int p);

void removeChar(char *str, int idxToDel);
void printStringInLine(char *str);
//ตำแหน่งปัจจุบันของเคอร์เซอร์
int notepad_cursorPosition = 0;
//ตัวคูณเลื่อนตำแหน่งเคอร์เซอร์
int notepad_multiplyCursor = 0;
int notepad_multiplyDisplay = 0;
//บรรทัดปัจจุบัน
int notepad_currentLine = 0;

int display_f = 0;
char keybuff[2] = "\0";
int enterSign = 45; //escape
int unicode_to_ASCII(int);
int mapCursor(int, int, int);
int checkBit(int);
int keyMapping(int, int, int);
void clearKeyValue(void);

#define notepad_Line 97
#define notepad_MaxinLine 40

char notepad_buffer_string[notepad_Line][notepad_MaxinLine]; //97*40 charactor
////////////////////////end notepad function/////////////////////

//------------------------- main function------------------------
/////////////////////////////////////////////////////////////////
int main(void)
{
  /* Configure the GPIO ports */
  GPIO_Configuration(); //if config this can't use printf
  //----------------------------
  UART4_Configuration();  //9600
  USART2_Configuration(); //115200
  USART1_Configuration(); //115200
  USART3_Configuration(); //9600

  Init_SPI();
  delay_init();
  sendUart(1);

  configDisplay();
  printDot(st_0, sizeof(st_0));
  delay_ms(1200);
  //printStringLR("Hello test test aaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbb",0);

  //delay_ms(3000);

  //****************** check dot****************
  //USART3_Configuration2();

  //changeBuatRate();
  /*SendCH370(changeBaudRateByte, sizeof(changeBaudRateByte));
    delay_ms(45);
    SendCH370(ResetAll, sizeof(ResetAll));
    delay_ms(100);


    USART_InitStructure36.USART_BaudRate = 57600;
    USART_InitStructure36.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure36.USART_StopBits = USART_StopBits_1;
    USART_InitStructure36.USART_Parity = USART_Parity_No;
    USART_InitStructure36.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure36.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure36);
    //SendCH370(ResetAll, sizeof(ResetAll));
    delay_ms(100);
  */

  prepareSD();
  //command_ = 1;

  //configFlash();
  /*while (1) {
    menu_s();
    while (mode == 1) {
      notepad();
    }
    while (mode == 2 && openFileStatus == 0) {
      searchFile();
    }
    while (mode == 2 && openFileStatus == 1) {
      ReadFile();
    }
    while (mode == 3) {
      GPIO_SetBits(GPIOC , GPIO_Pin_0);
      BluetoothMode();
      // keyboardMode();
      if (becon == 0) {
        menu_s();
      }
      else {
        keyboardMode();
      }
    }
    }*/
  //-notepad test
  //printf("batterry %d ",getBatterry());
  while (1)
  {
    notepad_main();
  }
  // command_ = 1;
  /*
  while (1) // explorror and read mode
  {

   
    keyRead();
    searchFile2();
    for (i = 0; i < maxFile; i++)
    {
      printf("%s\r\n", fileLists[i]);
    }
  }
  */
  //new
}
void notepad_main()
{
  int doing = 1; //status for do something in notepad mode

  while (doing)
  {
    notepad_readKey();       // key recieve
    if (countKey >= maxData) // do events
    {                        //Recieve & checking key
      seeHead = 0;
      printf("See key %x,%x,%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
      if (checkKeyError == 0xff)
      { //clear error key
        countKey = 0;
        SeeHead = 0;
      }
      if ((bufferKey3digit[1] > 3 || bufferKey3digit[2] != 0) && seeCur != 1) // key control
      {
        keyCode = keyMapping(bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
        //--- เลื่อนบรรทัด ----
        if (keyCode == 40)
        { // next line
          if (notepad_currentLine < notepad_Line)
            notepad_currentLine++;

          notepad_cursorPosition = notepad_getnullPostion(notepad_buffer_string[notepad_currentLine]);
          // บังคับห้ามมีค่าเกิน max
          if (notepad_cursorPosition >= notepad_MaxinLine)
            notepad_cursorPosition = notepad_MaxinLine;
          k = notepad_countLinewithOutLNsign(notepad_buffer_string[notepad_currentLine]); //ไม่นับ enter
          if (notepad_cursorPosition > k)
            notepad_cursorPosition = k;
        }
        else if (keyCode == 38)
        { // previous line
          if (notepad_currentLine > 0)
            notepad_currentLine--;

          notepad_cursorPosition = notepad_getnullPostion(notepad_buffer_string[notepad_currentLine]);
          if (notepad_cursorPosition >= notepad_MaxinLine)
            notepad_cursorPosition = notepad_MaxinLine;
          k = notepad_countLinewithOutLNsign(notepad_buffer_string[notepad_currentLine]); //ไม่นับ enter
          if (notepad_cursorPosition > k)
            notepad_cursorPosition = k;
          //printf("null position in line:%d \r\n", notepad_getnullPostion(notepad_buffer_string[notepad_currentLine]));
        }
        else if (keyCode == 1)
        { //left
          display_f = 0;
          k = 20;// ไม่นับถึง enter
          if (notepad_cursorPosition > k)                                                 //กดได้ไม่เกิน enter
            notepad_cursorPosition = k;
          else if (notepad_cursorPosition > strlen(notepad_buffer_string[notepad_currentLine]))
            notepad_cursorPosition = strlen(notepad_buffer_string[notepad_currentLine]);
            notepad_multiplyCursor = 0;
        }
        else if (keyCode == 2)
        { //right
          display_f = 1;
        }
      }
      else if (bufferKey3digit[0] == 0x40 && seeCur != 1) //enter
      {
        printf("New line \r\n");
        if (notepad_cursorPosition < notepad_MaxinLine)
        {
          while (notepad_cursorPosition < notepad_MaxinLine) //20 ตัวอักษร
          {
            notepad_append(notepad_buffer_string[notepad_currentLine], "-", notepad_cursorPosition);
            if (notepad_cursorPosition >= notepad_MaxinLine) //defualt 40 charactor
            {
              //new line
              notepad_currentLine++;
              notepad_cursorPosition = 1;
            }
            else
            {
              notepad_cursorPosition++;
              if (notepad_cursorPosition == notepad_MaxinLine) // max size in line
              {
                notepad_cursorPosition = 0;
                notepad_currentLine++;
                break;
              }
            }
          }
          //notepad_currentLine++;
        }
      }
      else if (bufferKey3digit[0] == 0x80 && seeCur != 1)
      { //remove str at index

        //notepad_cursorPosition = notepad_getnullPostion(notepad_buffer_string[notepad_currentLine]);
        printf("remove char %d -%c\r\n", notepad_cursorPosition, notepad_buffer_string[notepad_currentLine][notepad_cursorPosition]);

        if (notepad_cursorPosition > 0)
        {
          notepad_cursorPosition--;
        }
        else if (notepad_currentLine > 0)
        {
          notepad_cursorPosition = notepad_MaxinLine - 1;
          notepad_currentLine--;
        }
        //------------ถ้าเจอ enter ลบจนกว่าจะหมดไปใน line----------------------
        if (notepad_buffer_string[notepad_currentLine][notepad_cursorPosition] == '-') //ภ้าเป็น enter
        {
          while (notepad_buffer_string[notepad_currentLine][notepad_cursorPosition] == '-')
          {
            removeChar(notepad_buffer_string[notepad_currentLine], notepad_cursorPosition);
            notepad_cursorPosition--;
          }
        }
        removeChar(notepad_buffer_string[notepad_currentLine], notepad_cursorPosition);
      }
      else if (seeCur == 1)
      {
        notepad_cursorPosition = mapCursor(bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
        k = notepad_countLinewithOutLNsign(notepad_buffer_string[notepad_currentLine]); // ไม่นับถึง enter
        if (notepad_cursorPosition > k)                                                 //กดได้ไม่เกิน enter
          notepad_cursorPosition = k;
        else if (notepad_cursorPosition > strlen(notepad_buffer_string[notepad_currentLine]))
          notepad_cursorPosition = strlen(notepad_buffer_string[notepad_currentLine]);

        if (display_f == 1)
          notepad_multiplyCursor = 20;
        else
          notepad_multiplyCursor = 0;
        printf("cursor set at:%d\r\n", notepad_cursorPosition + notepad_multiplyCursor);
      }
      else if (1 && seeCur != 1) //type message in notepad mode
      {
        // printf("else -------------------------\r\n");
        //read key and convert to ascii
        keyCode = unicode_to_ASCII(bufferKey3digit[0]);
        if (keyCode == 0) //space bar edit in unicode table
          keyCode = 32;
        // printf("keycode/*/////%d\r\n",keyCode);
        //store data to #notepad_buffer_string

        //notepad_buffer_string[notepad_currentLine][notepad_cursorPosition] = keyCode;
        //increase charactor postion

        //manage charactor position // line feed

        //--- check enter in line ------

        if (notepad_checkEnterSignInLine(notepad_buffer_string[notepad_currentLine]) == 1)
        {
          notepad_removeEnterSign(notepad_buffer_string[notepad_currentLine]);
        }
        keybuff[0] = (char)keyCode;
        //printf("current po at %d \r\n", notepad_cursorPosition);
        // enter ตัวสุดท้ายจะถูกเบียดลง จะเกิน max ผมจะไม่ให้มันเบียด
        printf("/////////////////// %d %d//////////////////", notepad_cursorPosition, notepad_multiplyCursor);
        notepad_append(notepad_buffer_string[notepad_currentLine], keybuff, notepad_cursorPosition + notepad_multiplyCursor);

        if (notepad_cursorPosition >= notepad_MaxinLine) //defualt 40 charactor
        {
          //new line
          notepad_currentLine++;
          notepad_cursorPosition = 1;
        }
        else
        {
          notepad_cursorPosition++;
          if (notepad_cursorPosition == notepad_MaxinLine / 2)
          {
            display_f = 1;
          }
        }
      }

      if (notepad_checkEnterSignInLine(notepad_buffer_string[notepad_currentLine]) == 1)
      {
        notepad_fillEnterSign(notepad_buffer_string[notepad_currentLine]); //--เติม enter ถ้าไม่เต็มบรรทัด
      }
      printf("\r\n=====================sub=======================\r\n");

      // เช็คความยาว ตัดมาแสดง ไม่เกิน 20 ตัว

      //
      subStringLanR(notepad_buffer_string[notepad_currentLine], display_f);
      printf("\r\n=====================normal=======================\r\n");
      // printf("current in dex is: %d\r\n", notepad_cursorPosition);
      printStringInLine(notepad_buffer_string[notepad_currentLine]);
      // printf("typing// index at :%s\r\n", notepad_buffer_string[notepad_currentLine]);
      //  printf("current line is : %d\r\n", notepad_currentLine);
      printf("\r\n============================================\r\n");
      clearKeyValue(); // clear key buffer
    }
  }
}
char *subStringLanR(char *str, int p)
{
  char buff[20];
  int begin = 0;
  int end = 0;
  int l = 0;
  memset(buff, 0, strlen(buff));
  if (strlen(str) > 20)
  {
    //clear buff
    if (!p)
    { // split left
      begin = 0;
      end = 20;
    }
    else
    { // split right
      begin = 20;
      end = 40;
    }
    strncpy(buff, str + begin, end - begin);
    l = 1;
    printf("%s\r\n", buff);
  }
  else
  {
    printf("%s\r\n", str);
  }
}
void printStringLR(char *str, int s)
{
  char buff[20];
  int begin = 0;
  int end = 0;

  if (strlen(str) > 20)
  {
    //clear buff
    if (!s)
    { // split left
      begin = 0;
      end = 20;
    }
    else
    { // split right
      begin = 20;
      end = 40;
    }
    memset(buff, 0, strlen(buff));
    strncpy(buff, str + begin, end - begin);
    stringToUnicodeAndSendToDisplay(buff);
  }
  else
  {
    stringToUnicodeAndSendToDisplay(str);
  }
}

void notepad_fillEnterSign(char *str)
{
  int a, b;
  if (notepad_checkEnterSignInLine(str) == 1)
  {
    a = notepad_countLinewithOutLNsign(str);
    b = notepad_countEnterSign(str);
    printf("/////////////////////////////////////a = %d,b = %d\r\n", a, b);
    if (a + b != notepad_MaxinLine)
    {
      while (b < notepad_MaxinLine - 1)
      {
        //str[b] = (char)enterSign; //fille enter sign
        keybuff[0] = (char)enterSign;
        if (str[b] != '\0')
          notepad_append(str, keybuff, b);
        else
          str[b] = (char)enterSign;
        b++;
      }
    }
  }
}
int notepad_countEnterSign(char *str)
{
  int cc = 0;
  int ccE = 0;
  while (str[cc] != 0x00 && cc < notepad_MaxinLine)
  {
    if (str[cc] == enterSign)
    {
      ccE++;
    }
    cc++;
  }
  return ccE;
}
void notepad_removeEnterSign(char *str)
{
  int cc = 0;
  int indexSign = notepad_countLinewithOutLNsign(str) + 1;

  removeChar(str, indexSign);
}
//--check in line "-"
int notepad_checkEnterSignInLine(char *str)
{
  int cc = 0;
  int st = 0;
  while (str[cc] != 0x00 && cc < notepad_MaxinLine)
  {
    if (str[cc] == enterSign)
    { // '-
      st = 1;
      break;
    }
    cc++;
  }
  return st;
}
//--count line don't count enter sign--
int notepad_countLinewithOutLNsign(char *str)
{
  int cc = 0;
  while (str[cc] != enterSign)
  {
    if (str[cc] == enterSign)
      break;
    cc++;
  }
  return cc;
}
char numToASCII(int num)
{
  return (char)num;
}
int notepad_countStr(char *data)
{
  int cc__ = 0;
  int cc = 0;
  if (strlen(data) > 0)
  {
    while (data[cc__] != '\0' || cc__ < 42)
    {
      cc++;
    }
  }
  // ทดสอบคอมเม้น
  //

  return cc;
}
void notepad_append(char subject[], const char insert[], int pos)
{
  char buf[100] = {0}; // 100 so that it's big enough. fill with zeros
  int len;
  strncpy(buf, subject, pos); // copy at most first pos characters
  len = strlen(buf);
  strcpy(buf + len, insert); // copy all of insert[] at the end
  len += strlen(insert);     // increase the length by length of insert[]
  strcpy(buf + len, subject + pos);
  strcpy(subject, buf);
}

void removeChar(char *str___, int idxToDel)
{
  memmove(&str___[idxToDel], &str___[idxToDel + 1], strlen(str___) - idxToDel);
}
void printStringInLine(char *str)
{
  int aaa = 0;
  while (aaa < notepad_MaxinLine)
  {
    if (str[aaa] != 0)
      printf("%c", str[aaa]);
    aaa++;
  }
  printf("\r\n");
}
int notepad_getnullPostion(char *str)
{
  int cc_m = 0;
  while (str[cc_m] != 0)
  { // null byte
    cc_m++;
  }
  return cc_m;
}

void notepad_readKey()
{
  if (USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    //----------------------------- uart to key--------------------------------
    uart2Buffer = USART_ReceiveData(USART2); //-
    if (uart2Buffer == 0xff && SeeHead == 0)
    {
      SeeHead = 1;
      countKey = 0;
    }
    if (countKey == 2 && uart2Buffer == 0xa4)
    {
      // key is cursor #seeCur = 1;
      seeCur = 1;
    }
    if (countKey >= 4 && countKey <= 6)
    {                                              //-
      bufferKey3digit[countKey - 4] = uart2Buffer; //-
    }
    if (countKey == 2) //error checking
    {
      checkKeyError = uart2Buffer;
    }
    countKey++;
    // ---------------------------- serial interrupt----------------------------
  }
}
void clearKeyValue()
{
  countKey = 0;
  keyCode = 0;
  seeCur = 0;
}
//------------ mapping key to keyCode  ----------------------
//
//
//////////////////////////////////////////////////////////////
int keyMapping(int a, int b, int c)
{
  int keyCode__ = 0;
  if (a == 0x00 && b == 0x00 && c == 0x04)
  {
    keyCode__ = 37; // ArrowLeft
  }
  else if (a == 0x00 && b == 0x00 && c == 0x10)
  {
    keyCode__ = 38; // ArrowUp
  }
  else if (a == 0x00 && b == 0x00 && c == 0x08)
  {
    keyCode__ = 39; // ArrowRight
  }
  else if (a == 0x00 && b == 0x00 && c == 0x20)
  {
    keyCode__ = 40; // ArrowDown
  }
  else if (b == 64 || c == 8)
  {
    keyCode__ = 13; // enter
  }
  else if (a == 0x00 && (b == 0x01 || b == 0x02 || b == 0x03) && c == 0x00) //space bar
  {
    keyCode__ = 32; // space
  }

  else if (a == 0x40 && b == 0x00 && c == 0x00) // enter button 7
  {
    keyCode__ = 13;
  }
  //-----------------on read sector----------------
  //
  else if (a == 0x00 && b == 0x04 && c == 0x00)
  { // left button (readMode)
    keyCode__ = 1;
  }
  else if (a == 0x00 && b == 0x08 && c == 0x00)
  { // right button (readMode)
    keyCode__ = 2;
  }
  return keyCode__;
}
//โหลๆ
//----------------manage key ----------------------
//
//
//
///////////////////////////////////////////////////
void keyRead()
{ //used
  if (USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    //----------------------------- uart to key--------------------------------
    uart2Buffer = USART_ReceiveData(USART2); //-
    if (uart2Buffer == 0xff && SeeHead == 0)
    {               //-
      SeeHead = 1;  //-
      countKey = 0; //-
    }               //-
    if (countKey == 2 && uart2Buffer == 0xa4)
    {
      seeCur = 1;
    }
    if (countKey >= 4 && countKey <= 6)
    {                                              //-
      bufferKey3digit[countKey - 4] = uart2Buffer; //-
    }
    if (countKey == 2) //checkKeyError
    {
      checkKeyError = uart2Buffer;
    }
    countKey++;
    // ---------------------------- end uart to key ----------------------------
    // printf("[%x]\r\n",uart2Buffer);
  }
  if (countKey >= maxData)
  { //Recieve & checking key
    seeHead = 0;
    printf("See key %x,%x,%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
    if (checkKeyError == 0xff)
    { //clear error key
      countKey = 0;
      SeeHead = 0;
    }

    // key mapping //
    keyCode = keyMapping(bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
    // end keymapping //

    /*if(1){ //key in mode 0
       if (count_menu >= 1 && count_menu <= maxMenu && mode == 0) { //select menu //modemenu
        if (keyCode == 40) { //up
          if (count_menu != maxMenu) {
            count_menu++;
          }
        }
        else if (keyC
        ode == 38) { //down
          if (count_menu != 1) {
            count_menu -= 1;
          }
        }
      }
      if (keyCode == 39) {
        if (mode == 0) { //
          previousMode = mode;
          mode = count_menu;
          printf("mode:%d",mode);
        }
      }
      caseMenu(count_menu);
      }*/
    //--------
    if (endReadFile == 1) //node 2
    {                     // on mode read
      //printf("testttttttttttttttttttttttttt");
      slidingFileFromRomToDisplay();
    }
    if (2 && endReadFile != 1)
    { //key in mode 2
      if (keyCode == 38)
      {
        if (fileSelect > 0)
        {
          fileSelect--;
        }
        else
        {
          beepError();
        }
      }
      else if (keyCode == 40)
      {
        if (fileSelect < maxFile - 1)
        {
          fileSelect++;
        }
        else
        {
          beepError();
        }
      }
      printf("%s\r\n", fileLists[fileSelect]);
      printf("%s\r\n", Dirpath);
      if (keyCode == 39)
      { //enter // right joy
        //command_ = 16;
        printf("Opening %s\r\n", fileLists[fileSelect]);
        printf("type:%d\r\n", checkFileType(fileLists[fileSelect]));
        if (checkFileType(fileLists[fileSelect]) == 0)
        { // is folder
          maxFile = 0;
          delay_ms(45);
          if (openDir(fileLists[fileSelect]))
          {
            if (strstr(fileLists[fileSelect], "..") != NULL)
            {
              if (countPath(Dirpath) > 0)
              {                      //back and clear path
                exitOncePath();      // remove current path
                cdWithPath(Dirpath); //back one path
              }
            }
            else
            { //save path
              savePath(fileLists[fileSelect]);
            }
            fileSelect = 0;
            //printf("seach all file sucess \r\n");
            command_ = 4;
          }
        } //readFileFromCH376sToFlashRom
        else if (checkFileType(fileLists[fileSelect]) == 1 || checkFileType(fileLists[fileSelect]) == 2)
        {
          //printf("is tbt ");
          readFileFromCH376sToFlashRom(fileLists[fileSelect]);
        }
        //command_ = 16;
      }
      if (keyCode == 37)
      { // left joy
        printf("exit\r\n");
        maxFile = 0;
        exitOncePath();
        //openDir("..");
        cdWithPath(Dirpath);
        command_ = 4;
      }
    }
    ///------
    countKey = 0;
    keyCode = 0;
    seeCur = 0;
  }
}

//-----------------------------------------------------------------------------
//---------------------------------readFileFromCH376sToFlashRom-----------------
// อ่านข้อมูลจาก CH376 ลง Flash rom
////////////////////////////////////////////////////////////////////////////////
int readFileFromCH376sToFlashRom(char *fileName___)
{
  int readFileStatus___ = 1;
  //stringToUnicodeAndSendToDisplay("Reading....");
  r_count = 0;
  //SendCH370(ResetAll, sizeof(ResetAll)); //reset chip
  //printf("reading............ all\r\n");
  delay_ms(10);
  command_ = 4;
  while (readFileStatus___ == 1)
  {
    if (command_ == 4)
    {
      setFilename(fileName___);
      command_++; //5
      stringToUnicodeAndSendToDisplay("Reading....");
      delay_ms(45);
    }
    else if (command_ == 5)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      command_++; //6
      delay_ms(45);
    }
    else if (command_ == 6)
    {

      SendCH370(SetByteRead, sizeof(SetByteRead));
      //result 1D can read
      command_ = 99;
      delay_ms(30);
    }
    else if (command_ == 95)
    {
      //left (prevois line)
      SendCH370(continueRead, sizeof(continueRead));
      command_++; //96
    }
    else if (command_ == 98)
    {
      //right (next line)
      command_ = 6;
    }
    else if (command_ == 99)
    {
      SendCH370(ReadData, sizeof(ReadData));
      command_++;
    }
    //menu_s();
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      if (command_ == 96 && i1 == 0x14)
      {
        command_ = 6;
      }
      if (i1 == 0x80 || i1 == 0x14)
      {
        // printf("funk\r\n");
      }
      else if (command_ == 100 && countSector512 < 4 && countdataTemp512 < 512)
      {
        if (i1 == '\0')
        {
          // printf("End of File\r\n");-
        }
        else if (countdataTemp512 < ((128) * (countSector512 + 1)) - 1)
        {
          //------------------------------128 byte----------------------------
          // เก็บcharactor 128 byte
          ////////////////////////////////////////////////////////////////////
          dataTemp512[countdataTemp512] = i1;
          countdataTemp512++;
          strLength++;
          waitEnd = 0;
          lastAscii = i1;
        }
        else
        {
          //-----------------------------------------------------------------
          // ข้อมูลที่่อ่านครบ 128*4 = 512
          // ทำการเพิ่มคัวแปร countSector512
          ///////////////////////////////////////////////////////////////////
          countSector512++;
          dataTemp512[countdataTemp512] = i1;
          countdataTemp512++;
          if (countSector512 >= 4)
          {
            //---------------------------512 byte-----------------------------
            //  เอาข้อมูล 512 เก็บลง buffer ยาว [4096] จนครบ
            //
            //////////////////////////////////////////////////////////////////
            command_ = 95;
            for (i = addressWriteFlashTemp; i < addressWriteFlashTemp + 512; i++)
            {
              SST25_buffer[i - (sector * countSector)] = dataTemp512[i - addressWriteFlashTemp];
            }
            //  ทำการเพิ่มจำนวนตัวแปรที่นับ sector:countSector4096
            countSector4096++;
            //writeFlash(addressWriteFlashTemp);
            addressWriteFlashTemp += countdataTemp512;
            if (countSector4096 >= 8)
            {
              //---------------------------4096 byte---------------------------
              // ข้อมูลครบ 512*8 = 4096 ----> 1 sector
              //push (string 4096 charactor) to flash rom.
              //
              //////////////////////////////////////////////////////////////////
              //use variable:SST25_W_BLOCK
              // stringToUnicodeAndSendToDisplay("Reading....");
              writeFlash(addressSector, sector);
              //Delay(0xff);
              stringToUnicodeAndSendToDisplay("Reading....");
              addressSector += sector;
              countSector4096 = 0;
              //--------------------------------------------------------------------------------------------
              //check this value `addressWriteFlashTemp`--
              //--------------------------------------------------------------------------------------------
              countSector++;
            }
            //--- reset temp sector---//
            countdataTemp512 = 0;
            countSector512 = 0;
          }
          else
          {
            command_ = 6;
          }
        }
      }
    }
    else
    {
      if (lastAscii == i1)
      {
        waitEnd++;
        if (waitEnd == 100 * 100) // end of file
        {
          beepError();
          // printf("end----------------------------------------------------------------------------------------------------------\r\n");
          //---------------------------- last sector ----------------------------------
          //
          /////////////////////////////////////////////////////////////////////////////
          for (i = addressWriteFlashTemp; i < addressWriteFlashTemp + 512; i++)
          {
            SST25_buffer[i - (sector * countSector)] = dataTemp512[i - addressWriteFlashTemp];
          }
          configFlash();
          writeFlash(addressSector, sector);
          addressWriteFlashTemp += countdataTemp512;
          waitEnd++;
          //----------------------store last sector to flash rom-----------------------
          //
          /////////////////////////////////////////////////////////////////////////////
          SPI_FLASH_CS_LOW();
          SST25_R_BLOCK(0, SST25_buffer99, sector);
          SPI_FLASH_CS_HIGH();
          Delay(0xffff);
          endReadFile = 1;
          //---------------------------------------------------------------------------
          //
          //NextPoint; -> ทำแหน่งที่จะอ่าน String ชุดถีดไป
          //pointer22char -> buffer ทำแหน่ง string ล่าสุด รวม 0x0d,0x0a
          /////////////////////////////////////////////////////////////////////////////
          //- first line index at 0 to 0x0a->0x0d;
          // \r,\n
          for (NextPoint = pointer22char; SST25_buffer99[NextPoint] != 0x0d; NextPoint++)
          {
            buffer22Char[NextPoint - pointer22char] = SST25_buffer99[NextPoint];
          }
          printf("End\r\n%s:\r\n", buffer22Char);
          //stringToUnicodeAndSendToDisplay(buffer22Char);
          pointer22char += NextPoint + 2;
          NextPoint = pointer22char;
          AmountSector = addressWriteFlashTemp / sector;  //---- จำนวน sector ----
          AmountSectorT = addressWriteFlashTemp % sector; //---- เศษ ที่เหลือของ sector ---
          //-----------------------------------display string 20 charactor -----------------------------------
          //
          ////////////////////////////////////////////////////////////////////////////////////////////////////
          while (endReadFile == 1)
          { // query string-
            // menu_s();
            keyRead();
          }
        } // ทดสอบ
      }
    }
  }
  return 0;
}

//------------------slidingFileFromRomToDisplay--------------------------
//.
//อ่านข้อมูลจากROMมาเก็บในตัวแปลครั้งละ 4096 ตัวอักษรและดึงออกมาครั้งละไม่เกิน 20 หรือ 40
//ตัวอักษร  โดยการกด key 38,40,1,2
/////////////////////////////////////////////////////////////////////////
void slidingFileFromRomToDisplay()
{
  // check #endReadFile status for do in this void.
  //int MaxInLine = 42;
  if ((keyCode == 38 || keyCode == 40) && endReadFile == 1)
  {
    if (keyCode == 40) //next line
    {
      // clear previous value
      pointer22char += NextPoint - pointer22char;
      //------------------------------------------------------------------
      /*
       ถ้าตำแหน่งปัจจุบัน + จำนวนข้้อควา่มที่จะอ่าน > จำนวน Sector(4096) 
       และ ถ้าตำแหน่งปัจจุบัน + จำนวนข้้อควา่มที่จะอ่าน ยังน้อยกว่าความยาวของไฟล์ 
      */
      //-------------------------------------------------------------------
      if (pointer22char + MaxInLine > sector && pointer22char + MaxInLine < addressWriteFlashTemp)
      {
        // sector more than (one)
        pointerSectorStatus = 1;
        printf("change sector\r\n");
      }
      //------------------------------------------------------------------
      //
      //------------------------------------------------------------------
      if (pointer22char + MaxInLine < addressWriteFlashTemp) // can read.
      {
        // ให้ maxLengthLoopL22 ยาวเท่า MaxInLine
        maxLengthLoopL22 = MaxInLine;
      }
      else
      {
        // maxLengthLoopL22 เก็บข้อมูลที่เหลือ ที่สามารถอ่านได้
        maxLengthLoopL22 = addressWriteFlashTemp - pointer22char; //last value.
        beepError();                                              // beep error sound
      }
    } //--end keyCode 40--
    else if (keyCode == 38)
    {
      //prev line pointer22char   bug***************************
      //-
      //
      //
      ///////////////////////////////////////////////////////////
      printf("\r\n \r\n pointer22char %d \r\n \r\n", pointer22char);
      //NextPoint
      TempPointer22char = pointer22char;
      for (varForLoop = TempPointer22char; varForLoop > 0; varForLoop--)
      { // edit condition > 0 ->   ??? > 1*pointerSector
        countLengthInLine++;
        if (SST25_buffer99[varForLoop] == 0x0d) //see head
        {
          //
          // ถ้าเจอ CL ให้เพื่มค่าของ #countLFTwoStep
          countLFTwoStep++;
          /*if (countLengthInLine == 5 && countLFTwoStep == 2)
          {
            pointer22char = varForLoop;
            countLFTwoStep = 0;
            printf("Length:--%d-- a\r\n", countLengthInLine);
            break;
          }
          if (countLFTwoStep == 2)
          { //check 0x0d 0x0a two event
            pointer22char = varForLoop + 2;
            countLFTwoStep = 0;
            printf("Length:--%d--\r\n", countLengthInLine);
            break;
          }*/
          /*
          0x0d 0x0a 
          0x0d 0x0a 
          0x0d [0x0a] <-- stop here!!!!
          0x0d 0x0a |0xFF 0xFF 0xFF 0xFF 0xFF
          0x0d 0x0a|
          example :  current line is : (0xFF,0xFF,0xFF,0xFF)
          */
          if (countLFTwoStep == 2)
          {                                 //ย้อนกลับปกติ แบบไม่มีบรรทัดว่าง
                                            /* current = 0x0d
            0x0d 0x0a
            0x0d 0x0a
            */
                                            // ให้หยุดที่ var - 1
            pointer22char = varForLoop + 2; //0x0a

            countLFTwoStep = 0;
            break;
          }
        }
        else if (varForLoop == 1)
        { //begin of file
          pointer22char = 0;
          // -- find max length --

          //-- end find mx length --
          if (pointerSector != 0)
          {                         //if sector != 1
            readPreviousSector = 1; //status for read previous sector
            printf("reading prevois sector -----------------------------\r\n");
          }
        }

        if (pointer22char + MaxInLine < addressWriteFlashTemp)
        {
          maxLengthLoopL22 = MaxInLine;
        }
        else
        {
          maxLengthLoopL22 = addressWriteFlashTemp - pointer22char; //last value
        }
        /// printf("%c=",SST25_buffer99[j]);
      }                      // end for loop:
      countLengthInLine = 0; //clear
    }                        //--end keyCode 38--

    //--------------end query line for display string in line------------
    //
    //-------------------------------------------------------------------

    //********************Prepare string [20-40] for display on braille dot********************
    // NextPoint เท่ากับตำแหน่งปัจจุบัน ,ถ้า NextPoint น้อยกว่า (ตำแหน่งปัจจุบัน+ความยาวที่จะสามารถอ่านได้)
    //-----------------------------------------------------------------------------------------
    // ตำแหน่งปัจจุบัน; if()
    for (NextPoint = pointer22char; NextPoint < (pointer22char + maxLengthLoopL22); NextPoint++)
    {
      //-------------------------query line [20,40]-----------------------------
      //------------------------------------------------------------------------
      if (NextPoint + (pointerSector * sector) < addressWriteFlashTemp)
      {
        if (SST25_buffer99[NextPoint] == 0x0d) //next value-> 0x0a
        {
          // CRLF = 2
          // MaxInLine = 40
          /*
          ยกเว้น ox0d,0x0a โดดข้ามไปเลย
          */
          jumpCRLF = pointer22char + (MaxInLine - CRLF) - NextPoint; //store index value whene amount string less than 20
          NextPoint += CRLF;
          break;
        }

        //NextLenghtLess20
        //-------------------Store String value in #buffer22Char-----------------
        buffer22Char[NextPoint - pointer22char] = SST25_buffer99[NextPoint];
        //printf("%c/", SST25_buffer99[NextPoint]);
      }
      else // ถ้าเกืนความยาวของไฟล์
      {
        break;
      }
    } // end for loop;
    for (i = 40 - jumpCRLF; i < 40; i++)
    {
      //เคลียตัวอักษรที่นอกเหนือจากความยาวของ String ที่จะแสดงผล[hex is 0x00]
      buffer22Char[i] = 0;
    }

    //*****************read data from flash rom***********************
    //previous line
    //ถ้ามีการบอกว่าต้องอ่านข้ิอมูลใน sector ก่อนหน้า
    //////////////////////////////////////////////////////////////////
    if (readPreviousSector == 1)
    { //read previous sector when keycode == 38 && sector != 0
      readPreviousSector = 0;
      pointerSector--;
      configFlash();
      SPI_FLASH_CS_LOW();
      SST25_R_BLOCK(pointerSector * sector, SST25_buffer99, sector);
      SPI_FLASH_CS_HIGH();
      Delay(0xffff);
      pointer22char = sector;
    }
    //check string buffer before push to display when < less than 20 charactor
    //printf("\r\nString40ch:%s\r\n",buffer22Char);

    printStringLR(buffer22Char, 0);
    //stringToUnicodeAndSendToDisplay(buffer22Char);
    printf("//sector: %d //send: %d-- %s -\r\n", pointerSector, pointer22char, buffer22Char);
    //-------------read data from flash rom--------------------------
    //Next line
    //อ่านข้อมูลจาก ROM ใน Sector ถัดไป
    /////////////////////////////////////////////////////////////////
    if (pointerSectorStatus == 1)
    {
      //read next sector [length more than 4096]
      pointerSectorStatus = 0;
      pointerSector++; // current sector
      pointer22char = 0;
      NextPoint = 0;
      //printf("seeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\r\n");
      // print last string again
      configFlash();
      SPI_FLASH_CS_LOW();
      SST25_R_BLOCK(pointerSector * sector, SST25_buffer99, sector);
      SPI_FLASH_CS_HIGH();
      Delay(0xffff);
      stringToUnicodeAndSendToDisplay(buffer22Char); // print againt
      //delay_ms(1000);
    }
  }
  else if (keyCode == 1 || keyCode == 2) //[left,right] button
  {
    if (keyCode == 2) //right button
      printStringLR(buffer22Char, 1);
    else // left button
      printStringLR(buffer22Char, 0);
  }
}
//----------------------- check file type ------------------------
// *.TBT file return 1
// *.BRF file return 2
// *.TXT file return 3
/////////////////////////////////////////////////////////////////
int checkFileType(char *file)
{ //used
  int type = 0;
  if (strstr(file, ".TBT") != NULL)
  {
    type = 1;
  }
  else if (strstr(file, ".BRF") != NULL)
  {
    type = 2;
  }
  else if (strstr(file, ".TXT") != NULL)
  {
    type = 3;
  }
  return type;
}
//------------- function for reset [module ch376] and mount-------------
/////////////////////////////////////////////////////////////////////////
void mountStatus()
{ //used
  int mountStatus = 1;
  command_ = 1;
  printf("Reset CH376 and mount\r\n");
  while (mountStatus)
  {
    if (command_ == 1)
    {
      SendCH370(ResetAll, sizeof(ResetAll));
      delay_ms(100);
    }
    else if (command_ == 2)
    {
      SendCH370(setSDCard, sizeof(setSDCard));
      delay_ms(45);
      command_++;
    }
    else if (command_ == 3)
    {
      SendCH370(USBDiskMount, sizeof(USBDiskMount));
      delay_ms(45);
      command_++;
      mountStatus = 0;
    }
  }
}
///----------- [search file] first version. current version not use..----------------
//
//
///////////////////////////////////////////////////////////////////////////////////
void searchFile2()
{
  int readStatus = 1;
  //printf("Seaching............................\r\n");
  //SendCH370(ResetAll, sizeof(ResetAll)); //reset chip
  //printf("reset all\r\n");
  int time_check = 0;
  //maxFile = 0;
  while (readStatus)
  {
    if (command_ == 1)
    {
      SendCH370(checkConnection, sizeof(checkConnection));
      //printf("Check Connection str %s\r\n", FileName_buffer);
      command_++; //2
      delay_ms(45);
    }
    else if (command_ == 2)
    {
      SendCH370(setSDCard, sizeof(setSDCard));
      //printf("Set SD Card Mode\r\n");
      command_++; //4
      delay_ms(45);
    }
    else if (command_ == 3)
    {
      SendCH370(USBDiskMount, sizeof(USBDiskMount));
      //printf(" USB Disk Mount\r\n");
      command_++; //6
      delay_ms(45);
    }
    else if (command_ == 4)
    {
      //exitOncePath();
      delay_ms(45);
      if (openDir("*"))
      {
        //printf("seach all main file sucess \r\n");
        delay_ms(46);
        command_ = 16;
      }
    }
    else if (command_ == 16)
    {
      nextAgain = 0;
      SendCH370(enumgo, sizeof(enumgo));
      SendCH370(data0, sizeof(data0));
      printf("");
      // clear buffer
      time_check = 0;
      command_++;
      countFileLegth = 0;
      //delay_ms(50);
    }
    else if (command_ == 22)
    {
      setFilename("/*");
      printf("Set * back to root Name\r\n");
      command_++; //8
      delay_ms(45);
    }
    else if (command_ == 23)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      printf("File Open\r\n");
      command_++; //7
      delay_ms(45);
    }
    keyRead();
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      // printf("%x %d",i1,command_);
      if (i1 == 0x1d && countFileLegth == 0)
      { // if see file
        nextAgain = 1;
        DataForWrite[0] = 42;
      }
      else if (i1 == 0x42 && countFileLegth == 0)
      {
        readStatus = 0;
      }
      if (command_ == 2 && i1 == 0x82)
      {
        readStatus = 0;
        //break;
        errorBreak();
        printf("Not found SD-Card\r\n");
      }
      if (command_ > 16 && i1 != 0x1d)
      {
        if ((int)i1 >= 32 && (int)i1 <= 126)
        {
          DataForWrite[countFileLegth] = i1;
          countFileLegth++;
          time_check = 0;
        }
        else
        {
          DataForWrite[countFileLegth] = 42;
          countFileLegth++;
          time_check = 0;
        }
      }
    }
    if (countFileLegth >= 33)
    {
      //printf("length:%d\r\n",countFileLegth);
      //for(j = 0; j < 32; j++)
      //printf("%s max %d:\r\n",fileName(),maxFile);
      strcpy(fileLists[maxFile], fileName());
      maxFile++;
      countFileLegth = 0;
      //printf("\r\n%s",fileName());
      if (nextAgain == 1)
      {
        //printf("nextttttttttttttt\r\n");
        delay_ms(45);
        command_ = 16;
        //next agian
      }
    }
    time_check++;
    if (time_check > 10005)
    {
      time_check = 0;
      if (nextAgain == 1)
      {
        //printf("nextttttttttttttt\r\n");
        delay_ms(45);
        command_ = 16;
        //next agian
      }
    }
  }
}
//----------------------------- open directory ------------------------------
// [openDir] use for open directory with ch376 module
//
//////////////////////////////////////////////////////////////////////////////
int openDir(char *dirPath__)
{
  int Open_stats = 1, output = 0;
  int time_check = 0;
  char buff[30] = "/";
  char buff2[30] = "/";
  command_ = 1;
  //memset(buff,0,30);
  if (strstr(dirPath__, "..") != NULL)
  { // if see ".." -> /..
    strcat(buff, dirPath__);
  }
  while (Open_stats)
  {
    if (command_ == 1)
    {
      if (checkSlash(Dirpath) == 0)
      {
        strcat(buff2, dirPath__);
        setFilename(buff2); // open in root
        // printf("print file name:%s\r\n",buff2);
      }
      else if (strlen(buff) > 1)
      {
        setFilename(buff);
      }
      else
      {
        setFilename(dirPath__);
      }
      command_++;
      delay_ms(45);
    }
    else if (command_ == 2)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      command_++;
      delay_ms(45);
    }
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      time_check = 0;
      //printf("%x \r\n", i1);
      if (command_ == 3 && i1 == 0x41)
      {
        output = 1;
        //  printf(" status 1 \r\n ");
      }
      else if (command_ == 3 && i1 == 0x42)
      { // error not found path
        output = 2;
        // printf(" status 2 \r\n ");
      }
      else
      {
        output = i1;
      }
      if (command_ == 3)
      {
        // printf("break di\r\n");
        Open_stats = 0;
        //break;
      }
      //printf("\r\n  -- %c ,%x-- \r\n", i1,i1);
    }
    time_check++;
    if (time_check > 12005)
    {
      //printf("time out.... \r\n");
      Open_stats = 2;

      time_check = 0;
      Open_stats = 0;
    }
  }
  command_ = 99;
  return output;
}
//----------------------------------Save path Dir ---------------------------
// save directory path for remember path or view currect path
// store path in global variable name:Dirpath
//
//////////////////////////////////////////////////////////////////////////////
int savePath(char *pathName)
{ //used
  char prepareStr[30] = "";
  if (checkSlash(pathName))
  {
    //printf("has slash");
  }
  strcpy(prepareStr, "/");
  strcat(prepareStr, pathName);
  if (strlen(Dirpath) == 0)
    strcpy(Dirpath, prepareStr);
  else
    strcat(Dirpath, prepareStr);
  //printf("%s", prepareStr);
  return 1;
}
//--------------------------------exit one path -----------------------------
// remove cerrent dir at variable name:Dirpath
//ex: path = "ab/cd/" --> path "ab/"
//////////////////////////////////////////////////////////////////////////////
void exitOncePath()
{ //used
  int start = countPath(Dirpath);
  int longL = strlen(Dirpath);
  if (start > 0)
    while (start == countPath(Dirpath))
    {
      Dirpath[longL] = 0x0; //remove path
      if (longL == 0)
        break;
      longL--;
    }
}
//-------------------------------open dir with path --------------------------
// ex. path = "abc/def"
// cdWithPath(path);
//////////////////////////////////////////////////////////////////////////////
void cdWithPath(char *path)
{ //used
  int L = 0;
  char buffer[15] = "";
  int bbuf = 0;
  int root = 1;
  char rootD[15] = "/";
  //printf("********************************************\r\n");
  openDir("..");
  while (L < strlen(path))
  {
    if (L > 0)
    {
      if (path[L] == '/')
      {
        if (root == 1)
        {
          root = 0;
          strcat(rootD, buffer);
          //printf("\r\nbuffer1: %s\r\n", rootD);
          openDir(rootD);
        }
        else
        {
          //printf("\r\nbuffer2: %s\r\n", buffer);
          openDir(buffer);
        }
        memset(buffer, 0, 15);
        bbuf = 0, L++;
      }
      buffer[bbuf] = path[L];
      bbuf++;
    }
    L++;
    if (L >= 15)
      break;
  }
  openDir(buffer);
  //printf("\r\nbuffer3: %s\r\n", buffer);
}
//---------------------- count path ------------------------------------------
//
//Output: number of path
//////////////////////////////////////////////////////////////////////////////
int countPath(char *pathSource)
{ //used
  int countPath = 0;
  while (*pathSource)
  {
    if (*pathSource == '/')
    {
      countPath++;
    }
    pathSource++;
  }
  return countPath;
}
int checkSlash(char *pathName)
{ //used
  int seeS = 0;
  while (*pathName)
  {
    if (*pathName == '/')
    {
      seeS = 1;
    }
    pathName++;
  }
  return seeS;
}
//--------------------------- Create and write file()------------------------
//
//
// test function --
//////////////////////////////////////////////////////////////////////////////
void createFileAndWrite(char *fname)
{ //use
  command_ = 0;
  command_++;
  while (1)
  {
    if (createFile(fname) == 1)
    {
      // printf("Create File TA99 success\r\n");
      break;
    }
  }
  SendCH370(ResetAll, sizeof(ResetAll));
  delay_ms(100);
  command_ = 1;
  //  strcpy(str33,"The Afghan Taliban have rejected calls to extend a three-day ceasefire declared for the Muslim Eid festival. A spokesman said the truce would end on Sunday night and operations against the security forces would resume. Government officials urged the militants not to return to fighting, as dozens of unarmed Taliban exchanged Eid greetings with soldiers and civilians. Meanwhile, at least 18 people were killed in a suicide attack in the city of Jalalabad, officials said. The blast happened outside the office of the governor of Nangarhar province while officials were meeting Taliban insurgents as part of the ceasefire. Dozens were The ceasefire ends tonight and our operations will begin, God willing. We have no intention to extend the ceasefire, said spokesman Zabihullah Mujahid.");
  strcat(str1, str2);
  writeFile4096(fname, str1);
}
//-----------------------write file less than 4096 or equal------------------
//
//////////////////////////////////////////////////////////////////////////////
void writeFile4096(char *fname, char *strSource)
{ //use
  int maxSize = 0, sizeWrite = 128;
  int loop255 = 0;
  int Sloop255 = 0;
  int iL = 0, pp = 0;
  char buffForWrite[128], buffForWrite2[128];
  int WF = 0;
  memset(buffForWrite2, 0, 256);
  maxSize = strlen(strSource);
  loop255 = maxSize / sizeWrite;
  Sloop255 = maxSize % sizeWrite;
  while (iL < loop255)
  { //
    buffForWrite[pp - iL * sizeWrite] = strSource[pp];
    pp++;
    if (pp % sizeWrite == 0)
    {
      command_ = 1;
      while (1)
      { //save 255
        if (fileWrite(WF, fname, buffForWrite) == 1)
        {
          iL++;
          if (iL == 0)
            WF = 0;
          else
            WF = 1;
          break;
        }
      }
    }
  }
  while ((pp - iL * sizeWrite) < Sloop255)
  {
    buffForWrite2[pp - iL * sizeWrite] = strSource[pp];
    pp++;
  }
  command_ = 1;
  while (1)
  { //save 255
    if (fileWrite(WF, fname, buffForWrite2) == 1)
    {
      break;
    }
  }
}
//------------------------------from john-------------------------------------
//
//////////////////////////////////////////////////////////////////////////////
int mapCursor(int P1, int P2, int P3)
{
  if (P1 != 0)
  {
    return checkBit(P1);
  }
  else if (P2 != 0)
  {
    return checkBit(P2) + 8;
  }
  else if (P3 != 0)
  {
    return checkBit(P3) + 16;
  }
  else
  {
    return 0;
  }
}
//-----------------------------check bit of cursor---------------------------
//  input: cursor input
//  Output: bit position
//////////////////////////////////////////////////////////////////////////////
int checkBit(int input)
{
  int i;
  for (i = 0; i <= 8; i++)
  {
    if (input == 1)
      break;
    input = input >> 1;
  }
  return i;
}
//-----------------------------------save file name---------------------------
//
//
//////////////////////////////////////////////////////////////////////////////
void saveName()
{
  int saving = 1;
  char nameBuff[15];
  char bufferDisplay[20]; // Name:/
  int cc = 1;

  memset(bufferDisplay, 0, 20);
  strcpy(bufferDisplay, "Name:/");
  memset(nameBuff, 0, 15);
  nameBuff[0] = '/';
  countKey = 0;
  keyCode = 0;
  seeCur = 0;
  clearDot();
  while (saving == 1)
  {
    if (USART_GetITStatus(USART2, USART_IT_RXNE))
    {
      //----------------------------- uart to key--------------------------------
      uart2Buffer = USART_ReceiveData(USART2); //-
      if (uart2Buffer == 0xff && SeeHead == 0)
      {               //-
        SeeHead = 1;  //-
        countKey = 0; //-
      }
      if (countKey == 2 && uart2Buffer == 0xa4)
      { // if cursor
        seeCur = 1;
      }
      if (countKey >= 4 && countKey <= 6)
      {                                              //-
        bufferKey3digit[countKey - 4] = uart2Buffer; //-
      }

      if (countKey == 2)
      {
        checkKeyError = uart2Buffer;
      }
      countKey++;

      if (countKey >= maxData)
      {
        seeHead = 0;
        if (checkKeyError == 0xff)
        { //check error key
          //printf("Key Error");
          countKey = 0;
          SeeHead = 0;
        }
        if ((bufferKey3digit[0] == 0x00 && bufferKey3digit[1] == 0x20 && bufferKey3digit[2] == 0x00) || (bufferKey3digit[0] == 0x00 && bufferKey3digit[1] == 0x00 && bufferKey3digit[2] == 0x04))
        { //back to notepad
          saving = 0;
        }
        if (bufferKey3digit[0] == 0x80 && bufferKey3digit[1] == 0 && bufferKey3digit[2] == 0)
        {
          keyCode = 8;
        }
        if (bufferKey3digit[2] == 2 || bufferKey3digit[1] == 0x10)
        { // save file
          printf("\r\n enter \r\n");
          i = 0;
          while (nameBuff[i] != '\0')
          {
            if (nameBuff[i] >= 97)
            {
              nameBuff[i] = nameBuff[i] - 32;
            }
            i++;
          }
          strcat(nameBuff, ".TBT");
          //printf("\r\nSaving file :%s\r\n", nameBuff);
          createFileAndWrite(nameBuff);
          stringToUnicodeAndSendToDisplay("Success......");
          delay_ms(600);
          saving = 0;
        }
        if (keyCode == 8)
        { // delete file name
          if (cc > 1)
          { // steel save root /
            cc--;
            nameBuff[cc] = '\0';
            //printf("Delete:%s \r\n", nameBuff);
          }
          else if (cc == 1)
          { // last cha
            nameBuff[cc] = '\0';
            //printf("delete 22 \r\n");
          }
        }
        else // enter file name
        {
          // printf("See key %x,%x,%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
          for (i = 0; i < 255; i++)
          {
            if (bufferKey3digit[0] == unicodeTable[(char)i])
            {
              break;
            }
          }
          if (i >= 33 && i < 126 && cc <= 10)
          { // name length less than 10
            nameBuff[cc] = i;
            //printf("Filename:%s\r\n", nameBuff);

            cc++;
          }
        }
        memset(bufferDisplay, 0, 20);
        strcpy(bufferDisplay, "Name:");
        strcat(bufferDisplay, nameBuff);
        //stringToUnicodeAndSendToDisplay(bufferDisplay);

        /*-----------------------
          print file name here
          -----------------------*/
        countKey = 0;
        keyCode = 0;
        seeCur = 0;
      }
    }
    d_Time++;
    if (d_Time >= delayCur)
    { //blink cu
      if (toggleCur == 0)
        toggleCur = 1;
      else
        toggleCur = 0;
      if (!toggleCur)
        stringToUnicodeAndSendToDisplay(bufferDisplay);
      else
        stringToUnicodeAndSendToDisplayC(bufferDisplay, cc + 5);
      d_Time = 0;
    }
  }
}

//---------------------unicode from keyboardto ascii -------------------------
//
//////////////////////////////////////////////////////////////////////////////
int unicode_to_ASCII(int key)
{
  for (j = 0; j < 255; j++)
  {
    if (key == unicodeTable[(char)j])
    {
      break;
    }
  }
  return j;
}
//-----------new----------

void stringToUnicodeAndSendToDisplay(char *string)
{
  int strleng = 0;
  configDisplay();
  //clearDot();
  for (j = 0; j < 20 && string[j] != '\0' && string[j] != 0x0a; j++)
  {
    strleng++;
  }
  printf("| %s \r\n", string);
  SPI_DISPLAY_CS_LOW();

  for (j = 20; j >= 0; j--)
  {
    if (j < strleng)
    {
      Delay(0x55F);
      cell_sentdata(~unicodeTable[((int)*(string + j))]); //send to display
      printf(" %c unicode:%x ascii:%d lum: %d\r\n", *(string + j), unicodeTable[((int)*(string + j))], (int)*(string + j), j);
    }
    else
    {
      cell_sentdata(0xff); // null dot send to display
    }
  }
  SPI_DISPLAY_CS_HIGH();
  Delay(0x55F);
}
void stringToUnicodeAndSendToDisplayC(char *string, int po)
{ //cur position
  int strleng = 0;
  configDisplay();
  //clearDot();
  for (j = 0; j < 20 && string[j] != '\0' && string[j] != 0x0a; j++)
  {
    strleng++;
  }
  printf("| %s \r\n", string);
  SPI_DISPLAY_CS_LOW();

  for (j = 20; j >= 0; j--)
  {
    if (j == po)
    {
      cell_sentdata((~unicodeTable[((int)*(string + j))] & (~0xc0))); //curPostion
    }
    else if (j < strleng)
    {
      Delay(0x55F);
      cell_sentdata(~unicodeTable[((int)*(string + j))]); //send to display
      printf(" %c unicode:%x ascii:%d lum: %d\r\n", *(string + j), unicodeTable[((int)*(string + j))], (int)*(string + j), j);
    }
    else if (strleng == 0 && j == 0)
    {
      cell_sentdata((~0xc0)); //first position
    }
    else
    {
      cell_sentdata(0xff); // null dot send to display
    }
  }
  SPI_DISPLAY_CS_HIGH();
  Delay(0x55F);
}
//-------------------------- appenf file example----------------------
//
//
//
//
//////////////////////////////////////////////////////////////////////
void appendFile()
{

  if (command_ == 1)
  {
    SendCH370(checkConnection, sizeof(checkConnection));
    command_++; //2
    printf("WrilteFile \r\n Check Connection\r\n");
  }
  else if (command_ == 3)
  {
    SendCH370(setSDCard, sizeof(setSDCard));
    printf("Set SD Card Mode\r\n");
    command_++; //4
  }
  else if (command_ == 5)
  {
    SendCH370(USBDiskMount, sizeof(USBDiskMount));
    printf(" DiskMount R:%d\r\n", command_);
    command_++; //6
  }
  else if (command_ == 7)
  {
    SendCH370(setFileName, sizeof(setFileName));
    printf("Set File Name\r\n");
    command_++; //8
  }
  else if (command_ == 9)
  {
    SendCH370(FileOpen, sizeof(FileOpen));
    printf("File Open\r\n");
    command_++; //10
  }
  else if (command_ == 11)
  {
    SendCH370(FilePointer, sizeof(FilePointer));
    printf("Pointer\n");
    command_++; //12
  }
  else if (command_ == 13)
  {
    //    SendCH370(FileLength, sizeof(FileLength));
    printf("File Length\r\n");
    command_++; //14
  }
  else if (command_ == 15)
  {
    SendCH370(FileWrite, sizeof(FileWrite));
    printf("File Write\r\n");
    command_++; //16
  }
  else if (command_ == 17)
  {
    SendCH370(FileUpdate, sizeof(FileUpdate));
    printf("File Update\r\n");
    command_++; //18
  }
  else if (command_ == 19)
  {
    SendCH370(FileClose, sizeof(FileClose));
    printf("File Closed\r\n");
    command_++; //20
  }
  //  SendCH370(checkConnection,sizeof(checkConnection));
  if (USART_GetITStatus(USART3, USART_IT_RXNE))
  {
    i1 = USART_ReceiveData(USART3);
    printf("%x\r\n", i1);
  }
}
//------------------------next file // not use------------------
//
//
//
////////////////////////////////////////////////////////////////
void NextFile()
{
  SendCH370(enumgo, sizeof(enumgo));
  delay_ms(45);
  SendCH370(data0, sizeof(data0));
  delay_ms(45);
  // printf("\r\nNext File\r\n");
}
//---------------set size of string and ch376s----------------------
//
////////////////////////////////////////////////////////////////////
void setFileLength(char *str___)
{
  int jjr = 0;
  int FileLength222[] = {0x00, 0x57, 0xab, 0x3c, 0x00, 0x00};
  while (str___[jjr] != '\0')
  {
    jjr++;
  }
  if (jjr > 128)
  {
    jjr = 128;
  }
  FileLength222[4] = jjr;
  printf("\r\n str length2 in set file length:%d \r\n", FileLength222[4]);
  SendCH370(FileLength222, sizeof(FileLength222));
}
//------------------------write string to ch376s--------------------
//
//
////////////////////////////////////////////////////////////////////
void setFileWrite(char *str)
{
  SendCH370(FileWrite, sizeof(FileWrite));
  sendUart(3);
  printf("%s", str);
  sendUart(1);
}
int fileWrite(int k, char *filename, char *string__)
{

  int status = 0;
  if (command_ == 1)
  {
    SendCH370(checkConnection, sizeof(checkConnection));
    command_++; //2
    // printf("Check Connection\r\n");
    delay_ms(50);
  }
  else if (command_ == 2)
  {
    SendCH370(setSDCard, sizeof(setSDCard));
    // printf("Set SD Card Mode\r\n");
    command_++; //4
    delay_ms(50);
  }
  else if (command_ == 3)
  {
    SendCH370(USBDiskMount, sizeof(USBDiskMount));
    //printf(" DiskMount R:%d\r\n", command_);
    command_++; //6
    delay_ms(50);
  }
  else if (command_ == 4)
  {
    setFilename(filename);
    // printf("Set File Name\r\n");
    command_++; //8
    delay_ms(50);
  }
  else if (command_ == 5)
  {
    SendCH370(FileOpen, sizeof(FileOpen));
    // printf("File Open\r\n");
    command_++; //10
    delay_ms(50);
  }
  else if (command_ == 6)
  {
    if (k == 0) // head
      SendCH370(FilePointer, sizeof(FilePointer));
    else
      SendCH370(FilePointerend, sizeof(FilePointerend));
    // printf("Pointer\n");
    command_++; //12
    delay_ms(50);
  }
  else if (command_ == 7)
  { //FileLength

    setFileLength(string__);
    //  printf("File Length--\r\n");
    command_++; //14
    delay_ms(50);
  }
  else if (command_ == 8)
  {
    setFileWrite(string__);
    // printf("File Write\r\n");
    command_++; //16
    delay_ms(50);
  }
  else if (command_ == 9)
  {
    SendCH370(FileUpdate, sizeof(FileUpdate));
    // printf("File Update\r\n");
    command_++; //18
    delay_ms(50);
  }
  else if (command_ == 10)
  {
    SendCH370(FileClose, sizeof(FileClose));
    //  printf("File Closed\r\n");
    command_++; //20
    delay_ms(50);
  }
  if (USART_GetITStatus(USART3, USART_IT_RXNE))
  {
    i1 = USART_ReceiveData(USART3);
    //  printf("%x\r\n", i1);
    if (i1 == 0x14 && command_ == 11)
    {
      status = 1;
    }
  }

  return status;
}
//--------------------search file first version---------------------
//
//
////////////////////////////////////////////////////////////////////

//-------------copy string *char to char* same strcmp---------------
//
//
////////////////////////////////////////////////////////////////////
void copy_string(char *target, char *source)
{
  while (*source)
  {
    *target = *source;
    source++;
    target++;
  }
  *target = '\0';
}
//----------------------write string to flash rom-------------------
//
//
////////////////////////////////////////////////////////////////////
void writeFlash(int address, int size)
{
  //-- งง --//
  SPI_DISPLAY_CS_LOW();
  //-- -- //
  configFlash();
  SPI_FLASH_CS_LOW();
  SST25_W_BLOCK(address, SST25_buffer, sector);
  SPI_FLASH_CS_HIGH();
}
void writeFlash2(int address, int size)
{
  configFlash();
  SPI_FLASH_CS_LOW();
  SST25_W_BLOCK(address, SST25_buffer99, sector);
  //printf("---test-----%s",SST25_buffer);
  SPI_FLASH_CS_HIGH();
}
//------------------------read file ยังไม่ได้ Optimize-----------------
//
//
////////////////////////////////////////////////////////////////////

void OpenDir()
{ //readf
  //printf("OpenDir....\r\n");
  command_ = 0;
  //SendCH370(ResetAll, sizeof(ResetAll)); //reset chip
  // printf("reset all\r\n");
  delay_ms(45);
  command_++;
  while (openDirStatus == 1)
  {
    if (command_ == 1)
    {
      SendCH370(checkConnection, sizeof(checkConnection));
      command_++; //2
      delay_ms(45);
      // printf("Check Connection str %d\r\n",myFunction());
    }
    else if (command_ == 2)
    {
      SendCH370(setSDCard, sizeof(setSDCard));
      printf("Set SD Card Mode\r\n");
      command_++; //3
      delay_ms(45);
    }
    else if (command_ == 3)
    {
      SendCH370(USBDiskMount, sizeof(USBDiskMount));
      printf("USB Disk Mount\r\n");
      command_++; //4
      delay_ms(50);
    }
    else if (command_ == 4)
    {
      SendCH370(DirName, sizeof(DirName));
      printf("Set file Name\r\n");
      command_++; //5
      delay_ms(50);
    }
    else if (command_ == 5)
    { ///FileOpen
      SendCH370(FileOpen, sizeof(FileOpen));
      printf("File Open\r\n");
      command_++; //6
      delay_ms(50);
    }
    // menu_s();
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      printf("%x-%d\r\n", i1, i1);
    }
    //OpenDir
    if (command_ == 6 && i1 == 0x41)
    {
      openDirStatus = 0;
    }
  }
}
//

void setFilename(char *filename)
{ //setFilenameForFunction //0x00 0x57 0xab 0x2f
  int loovar = 0;
  int tempLL[14];
  SendCH370(setFilenameForFunction, sizeof(setFilenameForFunction));
  while (filename[loovar] != '\0')
  {
    tempLL[loovar] = filename[loovar];
    loovar++;
  }
  tempLL[loovar] = '\0';
  SendCH370(tempLL, sizeof(tempLL));
}
//-------------------------------create file in sd card --------------------------
//return status:1
//
//////////////////////////////////////////////////////////////////////////////////
int createFile(char *name)
{
  int status_create = 0;
  if (command_ == 1)
  {
    SendCH370(checkConnection, sizeof(checkConnection));
    command_++; //2
    delay_ms(50);
  }
  else if (command_ == 2)
  {
    SendCH370(setSDCard, sizeof(setSDCard));
    command_++; //4
    delay_ms(50);
  }
  else if (command_ == 3)
  {
    SendCH370(USBDiskMount, sizeof(USBDiskMount));
    command_++; //6
    delay_ms(50);
  }
  else if (command_ == 4)
  {
    setFilename(name);
    command_++; //8
    delay_ms(50);
  }
  else if (command_ == 5)
  {
    printf("\r\nfile create \r\n");
    SendCH370(FileCreate, sizeof(FileCreate));
    delay_ms(80);
    command_++; //10
  }
  else if (command_ == 6)
  {
    SendCH370(FileClose, sizeof(FileClose));
    command_++; //18
    delay_ms(50);
  }
  if (USART_GetITStatus(USART3, USART_IT_RXNE))
  {
    i1 = USART_ReceiveData(USART3);
    printf("%x\r\n", i1);
  }
  if (command_ == 7 && i1 == 0x14)
    status_create = 1;

  return status_create;
}
//---------------------------- fillter file name for display------------------
//
//
//////////////////////////////////////////////////////////////////////////////
const char *fileName()
{
  int loop = 0;
  int count_char = 0;
  int seeT = 0, seeBr = 0, seeTx = 0;
  for (loop = 0; loop < 32; loop++)
  {
    if (((int)DataForWrite[loop] >= 33 && (int)DataForWrite[loop] <= 126) && DataForWrite[loop] != 42 && loop < 12)
    { //end of 12
      DataForWrite_aftersort[count_char] = DataForWrite[loop];
      count_char++;
    }
  }
  while (count_char < 32)
  {
    DataForWrite_aftersort[count_char] = 0;
    count_char++;
  }
  count_char = 0;
  while (DataForWrite_aftersort[count_char] != '\0')
  {
    if (DataForWrite_aftersort[count_char] == 'T' && seeT == 0)
    {
      if (DataForWrite_aftersort[count_char + 1] == 'B' && DataForWrite_aftersort[count_char + 2] == 'T')
      {
        seeT = 1;
        DataForWrite_aftersort[count_char] = '.';
        DataForWrite_aftersort[count_char + 1] = 'T';
        DataForWrite_aftersort[count_char + 2] = 'B';
        DataForWrite_aftersort[count_char + 3] = 'T';
        count_char += 3;
      }
    }
    else if (seeT || DataForWrite_aftersort[count_char] == 42)
    {
      DataForWrite_aftersort[count_char] = 0;
    }
    count_char++;
  }
  count_char = 0;
  while (DataForWrite_aftersort[count_char] != '\0')
  {
    if (DataForWrite_aftersort[count_char] == 'B' && seeBr == 0)
    {
      if (DataForWrite_aftersort[count_char + 1] == 'R' && DataForWrite_aftersort[count_char + 2] == 'F')
      {
        seeBr = 1;
        DataForWrite_aftersort[count_char] = '.';
        DataForWrite_aftersort[count_char + 1] = 'B';
        DataForWrite_aftersort[count_char + 2] = 'R';
        DataForWrite_aftersort[count_char + 3] = 'F';
        count_char += 3;
      }
    }
    else if (seeBr || DataForWrite_aftersort[count_char] == 42)
    {
      DataForWrite_aftersort[count_char] = 0;
    }
    count_char++;
  }
  count_char = 0;
  while (DataForWrite_aftersort[count_char] != '\0')
  {
    if (DataForWrite_aftersort[count_char] == 'T' && seeTx == 0)
    {
      if (DataForWrite_aftersort[count_char + 1] == 'X' && DataForWrite_aftersort[count_char + 2] == 'T')
      {
        seeTx = 1;
        DataForWrite_aftersort[count_char] = '.';
        DataForWrite_aftersort[count_char + 1] = 'T';
        DataForWrite_aftersort[count_char + 2] = 'X';
        DataForWrite_aftersort[count_char + 3] = 'T';
        count_char += 3;
      }
    }
    else if (seeTx || DataForWrite_aftersort[count_char] == 42)
    {
      DataForWrite_aftersort[count_char] = 0;
    }
    count_char++;
  }
  /*if (seeT == 1) {
    printf("this's tbt file\r\n");
    } else {
    printf("this's Dir\r\n");
    }*/
  return DataForWrite_aftersort;
}
void menu_CH376()
{
  if (USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    //----------------------------- uart to key--------------------------------
    uart2Buffer = USART_ReceiveData(USART2); //-
    if (uart2Buffer == 0xff && SeeHead == 0)
    {               //-
      SeeHead = 1;  //-
      countKey = 0; //-
    }               //-
    if (countKey == 2 && uart2Buffer == 0xa4)
    {
      seeCur = 1;
    }
    if (countKey >= 4 && countKey <= 6)
    {                                              //-
      bufferKey3digit[countKey - 4] = uart2Buffer; //-
    }
    if (countKey == 2) //checkKeyError
    {
      checkKeyError = uart2Buffer;
    }
    countKey++;
    if (countKey >= maxData)
    { //Recieve & checking key
      seeHead = 0;

      if (seeCur == 1)
      {
        printf("see cur \r\n");
      }
      else
      {
        printf("See key %x,%x,%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
      }
      if (checkKeyError == 0xff)
      { //check error key
        countKey = 0;
        SeeHead = 0;
      }

      printf("current mode:%d\r\n", mode);
      //printf("%d\r\n",sizeof(mode_1)/sizeof(int));
      //////////////////////////////////menu selector ///////////////////////////////////
      if (bufferKey3digit[1] != 0 || bufferKey3digit[2] != 0)
      { //joy menu
        // ---------------------------- to key code -----------------------------
        if (bufferKey3digit[2] == 1 || bufferKey3digit[2] == 0x20)
        {               // joy is down
          keyCode = 40; // arrow down
          // command_++;
        }
        else if (bufferKey3digit[1] == 1 || bufferKey3digit[1] == 3 || bufferKey3digit[1] == 2)
        {               // joy is up
          keyCode = 55; // arrow up
        }
        else if (bufferKey3digit[1] == 128 || bufferKey3digit[2] == 0x10)
        {               // joy is up
          keyCode = 38; //
          //  command_=8;
        }
        else if (bufferKey3digit[1] == 64 || bufferKey3digit[2] == 8)
        {
          keyCode = 13; // enter
        }
        else if (bufferKey3digit[1] == 32 || bufferKey3digit[2] == 4)
        {
          keyCode = 8; // backspace
          ///command_ = 99;
        }
        else if (bufferKey3digit[1] == 4)
        {
          keyCode = 38; // left
          //  command_ = 97;
          //command_ = 95; //before delete
        }
        else if (bufferKey3digit[1] == 8)
        {
          keyCode = 40; // right
          // command_ = 98;
        }
        if (bufferKey3digit[0] == 0x9f)
        {
          printf("new Folder\r\n");
        }
      }
    } // end check.
    countKey = 0;
    keyCode = 0;
    seeCur = 0;
  }
}

void caseMenu(int count_menu)
{
  switch (count_menu)
  {
  case 1:
    printDot(st_notepad, sizeof(st_notepad));
    break;
  case 2:
    printDot(st_read, sizeof(st_read));
    break;
  case 3:
    printDot(st_bluetooth, sizeof(st_bluetooth));
    break;
    /*case 1:
        printDot(st_notepad, sizeof(st_notepad));
        break;
        case 2:
        printDot(st_filemanage, sizeof(st_filemanage));
        break;
        case 3:
        printDot(st_read, sizeof(st_read));
        break;
        case 4:
        printDot(st_usbconnect, sizeof(st_usbconnect));
        break;
        case 5:
        printDot(st_bluetooth, sizeof(st_bluetooth));
        break;
        case 6:
        printDot(st_clock, sizeof(st_clock));
        break;
        case 7:
        printDot(st_calcu, sizeof(st_calcu));
        break;
        case 8:
        printDot(st_tools, sizeof(st_tools));
        break;*/
  }
}

void printDot(int data[], int size)
{
  int tsize = size / sizeof(int);
  //  printf("%d", sizeof(testarray[0]) / sizeof(int));
  clearDot();
  SPI_DISPLAY_CS_LOW();
  Delay(0x55F);
  for (j = 20; j >= 0; j--)
  {
    if (j < tsize)
      cell_sentdata(~data[j]);
    else
      cell_sentdata(0xff); // null dot
  }
  SPI_DISPLAY_CS_HIGH();
  Delay(0x55F);
}
void clearDot()
{
  SPI_DISPLAY_CS_LOW();
  Delay(0x55F);
  for (j = 20; j >= 0; j--)
  {
    cell_sentdata(0xff);
  }
  SPI_DISPLAY_CS_HIGH();
  Delay(0x55F);
}
void Delay(uint32_t nCount)
{
  for (; nCount != 0; nCount--)
    ;
}
uint8_t Flash_ReadWriteByte2(uint8_t data)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    ;

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, data);

  /* Wait to receive a byte */
  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}
void Init_SPI(void)
{
  // SPI_InitTypeDef SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SPI_FLASH_CS, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //dot display 10MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //dot display
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //spi flash
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  SPI_FLASH_CS_HIGH();
  SPI_DISPLAY_CS_HIGH();
  /* SPI1 Config -------------------------------------------------------------*/

  /* Enable SPI1 */
  SPI_Cmd(SPI1, ENABLE);
  SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_TXE);
  //SPI_DISPLAY_CS_LOW();
}

void sentdata_cell(int data)
{
  //for (i = 0; i < 1; i++) {

  Flash_ReadWriteByte2(data);
  Delay(0x25F);
  // }
}
//----------------------------Cell send---------------------------------------
//
//
//////////////////////////////////////////////////////////////////////////////
void cell_sentdata(int cell)
{
  //SPI_DISPLAY_CS_LOW();
  // Delay(0x55F);
  sentdata_cell(cell);
  //    Delay(0x005e0);
  // SPI_DISPLAY_CS_HIGH();
  //Delay(0x55F);
}
void delay_SPI()
{
  Delay(0xfffff);
}
//Uart2ReceiveKeyCode

//--------------------------------- keyboard mode for BLE---------------------
//
//
//////////////////////////////////////////////////////////////////////////////
void keyboardMode()
{
  sendUart(1);
  if (USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    i1 = USART_ReceiveData(USART2);
    if (i1 == 0xff && seeHead == 0)
    {
      seeHead = 1;
      countKey = 0;
    }
    keyCodeRecieve[countKey++] = i1;
    //printf("%x",i1);
    //SendCommandToBLE2(i1);
  }
  if (countKey >= maxData)
  {
    SendCommandToBLE(keyCodeRecieve, sizeof(keyCodeRecieve));
    seeHead = 0;
    //int navLeft[] = {0xff, 0xff, 0xa6, 0x03, 0x00, 0x04, 0x00};
    for (j = 0; j < 7; j++)
    {
      printf("%c", keyCodeRecieve[j]);
    }
    countKey = 0;
  }
}
//--------------------------------------bluetooth mode------------------------
//
//
//////////////////////////////////////////////////////////////////////////////
void BluetoothMode()
{
  if (USART_GetITStatus(UART4, USART_IT_RXNE))
  { //if serial available
    hexbuffer = USART_ReceiveData(UART4);
    if (hexbuffer == 0xff && SeeHead == 0)
    {
      SeeHead = 1;
      count = 0;
      dataLength = 0;
    }
    if (count >= 23 && SeeHead == 1)
    {
      SeeHead = 0;
      pushToDisplay = 1;
    }
    BLEConBuffer[count] = hexbuffer;
    count++;
    if (count > 1 && BLEConn == 0)
    { //
      if (BLEConBuffer[0] == 0xff && BLEConBuffer[1] == 0xff && BLEConBuffer[2] == 0xa1 && BLEConn == 0)
      { //connect
        ConnectBLE();
        countConn++;
        if (countConn == 2)
        {
          BLEConn = 1;
          becon = 1;
        }
        count = 0;
      }
    }
  }
  else
  {
    sendUart(1);
    if (pushToDisplay == 1)
    {
      SPI_DISPLAY_CS_LOW();
      Delay(0x55F);
      for (j = 20; j >= 0; j--)
      {
        cell_sentdata(0xff);
      }
      SPI_DISPLAY_CS_HIGH();
      Delay(0x55F);
      for (j = 23; j >= 0; j--)
      {
        if (USART_GetITStatus(UART4, USART_IT_RXNE))
        {
          if (BLEConn != 0)
          {
            count = 0;
          }
          break;
        }
        else
        { //------------------ send to braille display ----------------------
          hexPrep = BLEConBuffer[j];
          if (hexPrep != 0xff && hexPrep != 0xa3 && hexPrep != 0x14)
          { //ff ff 3a
            cell_sentdata(~hexPrep);
          }
        }
      }
      pushToDisplay = 0;
      count = 0;
    }
  }
}
//----------------------------connect bluetooth-------------------------------
//
//
//////////////////////////////////////////////////////////////////////////////
void ConnectBLE()
{
  int x = 0;
  while (x < sizeof(connectData) / sizeof(int))
  {
    USART_SendData(UART4, connectData[x++]);
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
    {
    }
  }
}
//--------------------------send command to bluetooth module------------------
//
//
//////////////////////////////////////////////////////////////////////////////
void SendCommandToBLE2(int data)
{
  USART_SendData(UART4, data);
  while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
  {
  }
}
//---------------------------send command to ch376s---------------------------
//
//
//////////////////////////////////////////////////////////////////////////////
void SendCH370(int data[], int sizeOfData)
{
  int x = 0;
  while (x < sizeOfData / sizeof(int))
  {
    USART_SendData(USART3, data[x++]);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
    {
    }
  }
}
//---------------------------send data to bluetooth module--------------------
//
//
//////////////////////////////////////////////////////////////////////////////
void SendCommandToBLE(int data[], int sizeOfData)
{
  int x = 0;
  while (x < sizeOfData / sizeof(int))
  {
    USART_SendData(UART4, data[x++]);
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
    {
    }
  }
}
//----------------------------SendUART ---------------------------------------
//
//
//////////////////////////////////////////////////////////////////////////////
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount) //UARTSend(menu,sizeof(menu));
{
  while (ulCount--)
  {
    USART_SendData(USART1, *pucBuffer++);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {
    }
  }
}
//--------------------------------------send uart 4------------------------------
//
//
/////////////////////////////////////////////////////////////////////////////////
void UART4Send(const unsigned char *pucBuffer, unsigned long ulCount) //UARTSend(menu,sizeof(menu));
{
  while (ulCount--)
  {
    USART_SendData(UART4, *pucBuffer++);
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
    {
    }
  }
}
/*******************************************************************************
  Function Name  : GPIO_Configuration
  Description    : Configures the different GPIO ports.
  Input          : None
  Output         : None
  Return         : None
  Attention    : None
*******************************************************************************/
void sendUart(int data)
{
  sendUart1 = 0;
  sendUart2 = 0;
  sendUart3 = 0;
  sendUart4 = 0;
  switch (data)
  {
  case 1:
    sendUart1 = 1;
    break;
  case 2:
    sendUart2 = 1;
    break;
  case 3:
    sendUart3 = 1;
    break;
  case 4:
    sendUart4 = 1;
    break;
  default:
    sendUart1 = 0;
    sendUart2 = 0;
    sendUart3 = 0;
    sendUart4 = 0;
    break;
  }
}

//-------------------------------------GPIO------------------------------------
//
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  /* Configure PB.02 (ADC Channel8) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
  Function Name  : USART_Configuration
  Description    : Configure USART1
  Input          : None
  Output         : None
  Return         : None
  Attention    : None
*******************************************************************************/
void UART4_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  /* RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,DISABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);*/

  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
  /*
     USART1_TX -> PA9 , USART1_RX ->  PA10
  */
  /* tx */ //gpiot
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* rx */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(UART4, &USART_InitStructure);
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
  USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
  USART_ClearFlag(UART4, USART_FLAG_TC);
  USART_Cmd(UART4, ENABLE);
}
//---------------------------config uart1-------------------------------------
// 115200
//
//////////////////////////////////////////////////////////////////////////////
void USART1_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
  /*
     USART1_TX -> PA9 , USART1_RX ->  PA10
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_ClearFlag(USART1, USART_FLAG_TC);
  USART_Cmd(USART1, ENABLE);
}
//---------------------------config uart3-------------------------------------
// 115200
// ch376s
//////////////////////////////////////////////////////////////////////////////
void USART3_Configuration(void)
{

  GPIO_InitTypeDef GPIO_InitStructure31;
  USART_InitTypeDef USART_InitStructure31;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  /*
     USART1_TX -> PA9 , USART1_RX ->  PA10
  */
  GPIO_InitStructure31.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure31.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure31.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure31);

  GPIO_InitStructure31.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure31.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure31.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure31);

  USART_InitStructure31.USART_BaudRate = 115200;
  USART_InitStructure31.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure31.USART_StopBits = USART_StopBits_1;
  USART_InitStructure31.USART_Parity = USART_Parity_No;
  USART_InitStructure31.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure31.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART3, &USART_InitStructure31);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  USART_ClearFlag(USART3, USART_FLAG_TC);
  USART_Cmd(USART3, ENABLE);
}
//---------------------------config uart3-------------------------------------
//
//
//////////////////////////////////////////////////////////////////////////////
void USART3_Configuration2(void)
{

  GPIO_InitTypeDef GPIO_InitStructure31;
  USART_InitTypeDef USART_InitStructure31;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  /*
     USART1_TX -> PA9 , USART1_RX ->  PA10
  */
  GPIO_InitStructure31.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure31.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure31.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure31);

  GPIO_InitStructure31.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure31.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure31.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure31);

  USART_InitStructure31.USART_BaudRate = 57600;
  USART_InitStructure31.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure31.USART_StopBits = USART_StopBits_1;
  USART_InitStructure31.USART_Parity = USART_Parity_No;
  USART_InitStructure31.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure31.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART3, &USART_InitStructure31);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  USART_ClearFlag(USART3, USART_FLAG_TC);
  USART_Cmd(USART3, ENABLE);
}
//---------------------------config uart2-------------------------------------
// 115200
// keyboard
//////////////////////////////////////////////////////////////////////////////
void USART2_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  /*
     USART1_TX -> PA9 , USART1_RX ->  PA10
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
  USART_ClearFlag(USART2, USART_FLAG_TC);
  USART_Cmd(USART2, ENABLE);
}
/*******************************************************************************
  Function Name  : ADC_Configuration
  Description    : Configure the ADC.
  Input          : None
  Output         : None
  Return         : None
  Attention    : None
*******************************************************************************/
//----------------------------adc not use--------------------------------
//
//
/////////////////////////////////////////////////////////////////////////
void ADC_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);
  /* DMA channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel8 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while (ADC_GetResetCalibrationStatus(ADC1))
    ;

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while (ADC_GetCalibrationStatus(ADC1))
    ;

  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
    @brief  Retargets the C library printf function to the USART.
    @param  None
    @retval None
*/
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  //USART_SendData(UART4, (uint8_t) ch);
  if (sendUart1 == 1)
  {
    USART_SendData(USART1, (uint8_t)ch);
  }
  if (sendUart2 == 1)
  {
    USART_SendData(USART2, (uint8_t)ch);
  }
  if (sendUart3 == 1)
  {
    USART_SendData(USART3, (uint8_t)ch);
  }
  //USART_SendData(UART4, (uint8_t) ch);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
  {
  }
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {
  }
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {
  }
  return ch;
}

#ifdef USE_FULL_ASSERT

/**
    @brief  Reports the name of the source file and the source line number
      where the assert_param error has occurred.
    @param  file: pointer to the source file name
    @param  line: assert_param error line source number
    @retval None
*/
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
/*

*/