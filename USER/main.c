#include "systick.h"
#include <ctype.h> //use toupper
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h> //strtol

#include "stm32f10x_spi.h"
#include "CH376_Comm.h"
#include "keyCode.h"

#define markSector 999
//------------------------ typedef boolean------------------------
//================================================================
typedef uint8_t bool;
#define true 1
#define false 0
//================================================================

#define RCC_APB2Periph_GPIO_SPI_FLASH_CS RCC_APB2Periph_GPIOD
#define SPI_DISPLAY_CS_PORT GPIOD
#define SPI_DISPLAY_CS_PIN GPIO_Pin_4

//-------------------------------------define mode-------------------------------
#define MODE_ON_MENU 0
#define MODE_NOTEPAD 1
#define MODE_READ 2
#define MODE_BLUETOOTH 3
#define MODE_TOOLS 4
//-----------------------------------------------------------------------------
#define delayCur 250000 // delay time for cursor blink
#define sector 4096     //amount char in one sector
#define MaxInLine 42    //char for read in one line
#define CRLF 2
//--------------------------------spi display----------------------------------
#define SPI_DISPLAY_CS_LOW() GPIO_ResetBits(GPIOD, GPIO_Pin_4) //SPI_FLASH_CS_LOW
#define SPI_DISPLAY_CS_HIGH() GPIO_SetBits(GPIOD, GPIO_Pin_4)  //SPI_FLASH_CS_HIGH

//--------------------------------spi flash------------b------------------------
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

void writeFlash(int address);
void clearSector(int address);

int countSector4096 = 0;
int countSector = 0;
int addressSector = 0x00;

//
int AmountSector = 0;  // จำนวน Secter แต่ละ Sector ไม่เกิน 4096 Byte
int AmountSectorT = 0; // จำนวนเศษที่เหลือจาก Secter
int seeCur = 0;

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

int BLEConn = 0;
int pushToDisplay = 0;
int countConn = 0;

int bufferKey3digit[3] = {0};
int hexbuffer = 0;
int hexPrep = 0;
int SeeHead = 0;
int marker99 = 88;
//----------------------ch376 command-------------------------------
uint8_t data0[] = {0x57, 0xab, 0x27};
uint8_t enumgo[] = {0x57, 0xab, 0x33};
uint8_t FileOpen[] = {0x57, 0xab, 0x32};
uint8_t ResetAll[] = {0x57, 0xab, 0x05};
uint8_t ReadData[] = {0x57, 0xAB, 0x27};
uint8_t FileCreate[] = {0x57, 0xab, 0x34};
uint8_t FileUpdate[] = {0x57, 0xab, 0x3d};
uint8_t USBDiskMount[] = {0x57, 0xab, 0x31};
uint8_t continueRead[] = {0x57, 0xab, 0x3b};
uint8_t FileClose[] = {0x57, 0xab, 0x36, 0x01};
uint8_t FileClose0[] = {0x57, 0xab, 0x36, 0x00};
uint8_t setSDCard[] = {0x57, 0xab, 0x15, 0x03}; //+
uint8_t setFileName[15] = {0x57, 0xab, 0x2f, 0x2f};
uint8_t checkConnection[] = {0x57, 0xab, 0x06, 0x57};
uint8_t SetByteRead[] = {0x57, 0xab, 0x3a, 0x80, 0x00};      //15 = 21 character
uint8_t setFilenameForFunction[] = {0x57, 0xab, 0x2f};       // use vie function set file name
uint8_t setAllName[] = {0x57, 0xab, 0x2f, 0x2f, 0x2a, 0x00}; //*
uint8_t changeBaudRateByte[] = {0x57, 0xab, 0x02, 0x03, 0x98};
uint8_t FilePointer[] = {0x57, 0xab, 0x39, 0x00, 0x00, 0x00, 0x00};
uint8_t FilePointerend[] = {0x57, 0xab, 0x39, 0xff, 0xff, 0xff, 0xff};
uint8_t FileSize[] = {0x57, 0xab, 0x0c, 0x68};
uint8_t DiskConnect[] = {0x57, 0xab, 0x30};

uint8_t setname1[] = {0x57, 0xab, 0x2f, 0x2f, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x7e, 0x31, 0x2e, 0x54, 0x58, 0x54, 0x00};
uint8_t setname2[] = {0x57, 0xab, 0x2f, 0x2f, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x7e, 0x31, 0x2e, 0x54, 0x58, 0x54, 0x00};
// 0x00, 0x57, 0xab, 0x35
uint8_t Folder_DELETE[] = {0x57, 0xab, 0x35};

char buffascii[10];
uint8_t FileWrite[] = {0x57, 0xab, 0x2d};
uint8_t DirName[] = {0x57, 0xab, 0x2f, 0x2f, 0x54, 0x45, 0x53, 0x54, 0x00};
/////////////////////////////////////////////////////////////////////
//---------------------- menu variable ------------------------------
uint8_t count_menu = 1;
uint8_t menu[6] = {0x01, 0x02, 0x04, 0x80, 0x04, 0x02};
uint8_t st_bluetooth[] = {0x43, 0x07, 0x25, 0x11, 0x1e, 0x15, 0x15, 0x1e, 0x13}; //bluetooth
uint8_t st_0[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
uint8_t st_notepad[] = {0x1d, 0x15, 0x1e, 0x11, 0x0f, 0x01, 0x19}; //notepad
uint8_t st_read[] = {0x17, 0x11, 0x01, 0x19};
uint8_t mode = 0; //1,2,3,4,5,6,7,8
////////////////////////////////////////////////////////////////
int checkKeyError = 0;
int keyCode = 0;
int previousMode = 0;
uint8_t connectData[] = {0xff, 0xff, 0xa2, 0x0b, 0x16, 0x14, 0x10, 0x53, 0x4c, 0x52, 0x49, 0x20, 0x56, 0x31, 0x2e, 0x30};

int countSector512 = 0;

int becon = 0;

int ex_openDirStatus = 0;
//--readmode

// 10

int nextAgain = 0;
int readFileStatus___ = 0;
int countFileLegth = 0;

int command_ = 0;
int command2_ = 0;
int d_Time = 0;
int toggleCur = 0;

//------------------------------------------ ch370t data ------------------------------------//'
char filelist[10][15];
int seaching = 1; //for seaching file
int readStatus = 1;
int ABCCCCC = 1; // ใช้แทน readStatus
/* Private function prototypes -----------------------------------------------*/

///------------------------------------spi---------------------------------//
void keyboardMode(void);

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
void sendUart(uint8_t);
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);
void ConnectBLE(void);
void BluetoothMode(void);
void SendCommandToBLE(int data[], int sizeOfData);

void printDot(uint8_t data[], uint8_t size);
void stringToUnicodeAndSendToDisplay(char *str);
void stringToUnicodeAndSendToDisplayC(char *string, int po); //cur position

void clearDot(void);
void menu_s(void);
void menu_CH376(void); //for ch376s
void caseMenu(int);
//void copy_string(char *target, char *source);
int fileWrite(int k, char *filename, char *string);
void writeFile4096(char *fname, char *strSource);

const char *fileName(void);
char DataForWrite[32] = "";

//--- sd card---//
int createFile(char *name);
int CreateFile__ = 1;
void appendFile(void);
void SendCH370(uint8_t data[], uint8_t);

void setFilename(char *name);

//--------------------------------new prototype  ------------------------------
void searchFile2(void);
void NextFile(void);
void keyRead(void);
/*--------------------------------explorrer file---------------------------------
 ฟังก์ชั่นสำหรับจัดการพาร์ทของ Directory
 มีการจัดการกำการเข้าถึงโฟลเดอร์ และใช้คำสั่งกับ CH376 เพื่อเข้าโฟลเดอร์ หรืออ่านไฟล์ต่างๆ
 ex. /testfile/text.tbt*/
void ex_exitOncePath(void);
void ex_cdWithPath(char *path);
int ex_checkFileType(char *file);
int ex_openDir(char *dirPath);
int ex_savePath(char *pathName);
int ex_checkSlash(char *pathName);
int ex_countPath(char *pathSource);

void saveName(void);
int maxFile = 0;
int fileSelect = 0;
// read text to rom
int readFileFromCH376sToFlashRom(char *filename);

char Dirpath[30] = "";
#define maxfileListBuffer 30
#define fileListBufferSize 15
char fileLists[maxfileListBuffer][fileListBufferSize];
USART_InitTypeDef USART_InitStructure36;
//////////////////////////////////////////////////////////////////////////////

float AD_value;
vu16 ADC_ConvertedValue;
bool sendUart1 = false;
bool sendUart2 = false;
bool sendUart3 = false;
bool sendUart4 = false;

/*-----------------------------spi flash------------------------------------------*/
extern void SPI_Flash_Init(void);
extern unsigned char SST25_buffer[];
extern unsigned char SST25_bufferCC[];
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

SPI_InitTypeDef SPI_InitStructure; //global config.
void beepError(void);
void beep2(void);
void beep3(void);
void beep4(void);
void beep5(void);
void errorBreak(void);
uint8_t getBatterry(void);
int getMuteStatus(void);

void prepareSD_Card(void);

//================================================================================
//------------------------notepad function and variable---------------------------
/*
 _      ____  _____  _____ ____  ____  ____
/ \  /|/  _ \/__ __\/  __//  __\/  _ \/  _ \
| |\ ||| / \|  / \  |  \  |  \/|| / \|| | \|
| | \||| \_/|  | |  |  /_ |  __/| |-||| |_/|
\_/  \|\____/  \_/  \____\\_/   \_/ \|\____/

*/
#define bufferMaxSize 4096
#define notepad_Line 102 // 102 * 40 = 4080
#define scrapSize (notepad_Line * notepad_MaxinLine) - bufferMaxSize
#define notepad_MaxinLine 40
#define maxMenu 4
int enterSign = 255; //escape //'-'
char buffAs[2];

void notepad_main(void);
void notepad_readKey(void);
void notepad_removeEnterSign(char *str);
void notepad_fillEnterSign(char *str);
void notepad_append(char subject[], const char insert[], int pos);
void notepad_checkenterAndpush(char *str);
void notepad_checkMaxLine(void);
void printStringLR(char *str, int p);
void removeChar(char *str, int idxToDel);
void printStringInLine(char *str);
void clearKeyValue(void);

int notepad_getnullPostion(char *);
int notepad_countLinewithOutLNsign(char *str);
int notepad_checkEnterSignInLine(char *str);
int notepad_countEnterSign(char *str);
int notepad_countStr(char *data);
int notepad_lineIsEnter(char *str);
int unicode_to_ASCII(int);
int mapCursor(int, int, int);
int checkBit(int);
int keyMapping(int, int, int);

//=======================================================

///------------------------config------------------------
void configFlash(void);
void configDisplay(void);
void sendCommandtoAtmega(int data);
//-------------------------------------------------------
//char Notepad.buffer_string[notepad_Line][notepad_MaxinLine]; //97*40 charactor
char *subStringLanR(char *str, int p, int cur_);
char *substring(char *string, int position, int length);
//int Notepad.multiplyCursor = 0; //defualt is notepad_multiplyCursor
//int Notepad.displayFirst = 0; //display_f

//int Notepad.currentLine = 0; //notepad_currentLine

int maxLineN = 0; // จำนวนบรรทัดมากสุดที่พิมพ์ไปในโหมด notepad
uint8_t debug = 1;
char keybuff[2] = "\0";

//-------------------------------------------------------------
//get name bluetooth

char *getBluetoothName(void);
int atName[] = {0x41, 0x54, 0x2B, 0x4E, 0x41, 0x4D, 0x45, 0x3F};
int sendStatus = 1;
/////////////////////////////////////////////////////////////////
//int enterSign = (int)'-';
//char *regexStrEnter(char *str);
int str_cut(char *str, int begin, int len);

uint8_t unicodeTable[] =
    {
        0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //16
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //32
        // !"#$%&'()*+,-./
        0x00, 0x2e, 0x10, 0x3c, 0x2b, 0x29, 0x2f, 0x04, 0x37, 0x3e, 0x21, 0x2c, 0x20, 0x24, 0x28, 0x0c, //checked 8/6/2018
        /*0-9*/
        0x34, 0x02, 0x06, 0x12, 0x32, 0x22, 0x16, 0x36, 0x26, 0x14, //checked 8/6/2018
        /*:  ;  */
        0x31, 0x30, 0x23, 0x3f, 0x1c, 0x39, 0x48, //checked 8/6/2018 -- one check
        /*A-Z*/
        0x41, 0x43, 0x49, 0x59, 0x51, 0x4b, 0x5b, 0x53, 0x4a, 0x5a, 0x45, 0x47, 0x4d, 0x5d, 0x55, 0x4f,
        0x5f, 0x57, 0x4e, 0x5e, 0x65, 0x67, 0x7a, 0x6d, 0x7d, 0x75 /*Z*/, //checked 8/6/2018
        0x2a, 0x33, 0x3b, 0x6e, 0x38, 0x08, /*symbol*/                    //0x6e 0x1c

        0x01, 0x03, 0x09, 0x19, 0x11, 0x0b, 0x1b, 0x13, 0x0a, 0x1a, 0x05, 0x07, 0x0d, 0x1d, 0x15, 0x0f,
        0x1f, 0x17, 0x0e, 0x1e, 0x25, 0x27, 0x3a, 0x2d, 0x3d, 0x35, /*z*/ //checked 8/6/2018

        0x2a, 0x33, 0x3b, 0x18, 0xf1 /*{|}*/ //checked 8/6/2018
};
/** notepad  ยังไม่เสร็จ**/
#define readmode_maxsizeInLine 41
#define readmode_MaxLineBuffer 202 //102
int read_mode_currentIndex_after = 0;
int read_mode_contTextReaded = 0;
int ccMain = 0;

char readmode_bufferStr[readmode_MaxLineBuffer][readmode_maxsizeInLine];
char str_ready[40];
char str_be[10][11];
void PrepareText(void);
//char str_test[4000];

//-----------------------------------------------------------------------------
//sliding to display v 2
void convert_text2_buffer(char *str);
void slidText2Displayv2(void);

///////////////////////////////////////////////////////////////////////////////
void readSecter(int sector_);

//// add new ///
//----mode 4----
void mode4(void);
void mainMenuDisplayMode4(int);
#define maxMenu4 3
uint8_t countMenuInMode4 = 0;
bool selectMode4 = false;
//----mode 4----e

//-- read miode new ---
void queryLine(int line);
char bufferQueryLine[50];

//-------------------------
char buffer_afterRemove[42];

void MemSize()
{
  /*
   ===============================
    int       : 4 Byte
    char      : 1 Byte
    uint8_t   : 1 byte
    uint16_t  : 2 byte
    uint32_t  : 4 byte
    long      : 4 byte
    ===============================
    */
  printf("int      size :%d Byte\r\n", sizeof(int));
  printf("char     size :%d Byte\r\n", sizeof(char));
  //printf("uint4_t size :%d Byte\r\n",sizeof(uint4_t));
  printf("uint8_t  size :%d Byte\r\n", sizeof(uint8_t));
  printf("uint16_t size :%d Byte\r\n", sizeof(uint16_t));
  printf("uint32_t size :%d Byte\r\n", sizeof(uint32_t));
  printf("long     size :%d Byte\r\n", sizeof(long));
}
char bluetoothNambuffer[20];
void displayPrepare(void);
void testCell(void);  //test cell in mode
void StoreLine(void); //store line in  struct [lineInsector]

void StoreLineClear(void);        //clear store  in  struct [lineInsector]
void initSlidingMode(void);       //befor use struce
void clearReadlineInsector(void); //clear  struct [lineInsector]

#define lineInSectorBufferSize 100 //100*2 byte

typedef struct lineSlider
{
  int currentLine;
  int MainCurrentLine;
  //int currentSector;
  int TotalSector; // จำนวน secter
  //int sesString;
  uint16_t currentSector;
  uint16_t lineInsector[lineInSectorBufferSize];
  uint16_t totalLine;
  char strTemp[42];
  char strTemp2[42];
  bool DisplayLine;

} lineSlider;
typedef struct readROM
{
  uint16_t countdata_Temp512;
  uint32_t waitEnd;
  uint8_t lastAscii;
  int addressWriteFlashTemp;
  bool endReadFile;
  char dataTemp512[512];
} readROM;
typedef struct notepad
{
  uint8_t cursorPosition;
  uint8_t multiplyCursor;
  bool displayFirst;
  int currentLine;
  char buffer_string[notepad_Line][notepad_MaxinLine];
} notepad;

typedef struct pages
{
  uint16_t index;
  uint16_t page[150]; // 2*150 300 bytes
  uint16_t totalPage;
  int currentPage;

} page;
// typedef
page pages;
readROM ROMR;    //[prepare]read file to rom
lineSlider read; // read rom to display
notepad Notepad; //notepad mode

/*******************************************************************************
  Function Name  : main
  Description    : Main program
  Input          : None
  Output         : None
  Return         : None
  Attention      : None
*******************************************************************************/

//-------------------------------------------------------------------------------
//function go to line
unsigned int gotoLine_EnterLine(int maxLine);
int gotoLine_getLine(int Line, int Sector);
int gotoLine_getSectorInline(int line);
int gotoLine_get_int_len(int value);

uint16_t gt_Sector = 0;
int16_t gt_Line = 0;

// manage sd card
int DeleteFolder(char *str);
int DeleteFile(char *str);
void setFoldername(char *filename);

void upperASCII(char *str);
int confirm_delete(void);

//--------------------------- for mark variable ---------------------------------
#define mark_EndSign ':'
#define mark_MarkMaxBufferPage 20
int mark_searchName(char *fileName); // search file
int mark_CreateMarkFileConfig(char *filename);
int mark_hasFileConfig(char *filename);
int mark_getEndSign(int startAt);            //get position of end string
int mark_MarkerPage[mark_MarkMaxBufferPage]; // buffer
int mark_currentPostion = 0;                 // current mark position
int mark_maxmarkMaxMax = 0;                  // maximum page of mark
int mark_findIndexOfArray(int target);       // find index of line
int mark_RemoveArrayAtIndex(int position);
void mark_sortPageAndStore();
int mark_findIndexForInsert(int value);
int mark_insertMark(int);
int mark_sizeOfList();           // หาขนาด List
void mark_readFormROM();         //อ่านค่าจาก ROM
void mark_init();                //กำหนดค่าเริ่มต้น
int mark_findListTotalInArray(); //ค่า สูงสุด ใน อาเรย์

void mark_printfPageOfMarker(char *); // print lst
void mark_insertLine(char *, int);
void mark_filterPageToArray(int pS, int pE); // data string to num array
char *mark_str_replace(char *orig, char *rep, char *with);
//test.brf5/10/15/20/:test2.brf1/4/5/12/55/:test99.brf2/254/525/1000/:gg.txt/:

char data_temp[30];
char data2[400];
//----------------------------------------------------------------
/*
  Sector = gotoLine_getSectorInline(Line); //เนเธ”เน Sector
  gotoLine_getLine(Line, Sector);     // เธ•เนเธญเธเนเธ”เน Line เนเธ Sector
  printf("found Line in Sector (%d)\r\n", Sector);
  printf("In line %d\r\n", gotoLine_getLine(Line, Sector));
*/
int sumLineInSector(int index)
{
  int sumV = 0;
  /*for(j = 0 ; j< index;j++){
    printf("================================\r\n");
    printf("at index = %d line in sec = %d\r\n",j ,read.lineInsector[j]);
  }*/
  i = 0;
  while (i < index)
  {
    sumV += read.lineInsector[i];
    i++;
  }
  return sumV;
}
int readmode_countLineInOneSector(char *str, int index)
{
  int count = 0;
  int sum = 0;
  while (str[count] != '\0' && count < sector)
  {
    if (str[count] == 0x0d)
    {
      if (count != sector - 1)
      {
        // --- นับทรรทัด ----
        if (str[count + 1] == 0x0a) // นับบรรทัด
        {
          sum++;
        }
      }
    }
    else if (str[count] == 0x0c) // ถ้าเจอหน้า
    {
      pages.page[pages.index] = index > 0 ? sumLineInSector(index) + sum : sum;
      pages.index++;
    }
    count++;
  }
  return sum;
}
/*
// นับจำนวนหน้าจาก ROM
*/
int readmode_countPageInOneSector(char *str)
{
  int count = 0;
  int sum = 0;
  while (str[count] != '\0' && count < sector)
  {
    if (count != sector - 1)
    {
      // --- นับหน้า----
      if (str[count] == 0x0c) // นับบรรทัด
      {
        sum++;
      }
    }
    count++;
  }
  return sum;
}
/*******************************
*  ทดสอบเก็บข้อมูลลงรอม และอ่าน
********************************/
void StoreDataTOSector()
{
  printf("PROCESSING..\r\n");
  memset(SST25_buffer, '1', 4096); // เก็บค่าลงบัฟเฟอร์
  writeFlash(0);                   // เอาบัฟเฟอร์เขียนลง ROM
  memset(SST25_buffer, '2', 4096); // เก็บค่าลงบัฟเฟอร์
  writeFlash(4096);                // เอาบัฟเฟอร์เขียนลง ROM
}
int findTotalSectorUSED()
{
  int secAd = 0;
  while (1)
  {
    memset(SST25_buffer, 0, 4096);
    readSecter(secAd * 4096);
    secAd++;
    if (strlen(SST25_buffer) != 4096)
      break;
    else
      secAd++;
  }
  return secAd;
}
void appendToSECTOR()
{
  int sector__ = 0;
  char shiftChar[2];
  StoreDataTOSector();

  memset(SST25_buffer, 0, 4096); // เคลียร์ ค่าใน บัฟเฟแร์

  readSecter(sector__);      // อ่านค่าใน ROM
  SST25_buffer[4095] = 0x00; //clear last char

  notepad_append(SST25_buffer, "a", 35);

  shiftChar[0] = SST25_buffer[4095];
  readSecter(4096);          // อ่านค่าใน ROM
  SST25_buffer[4095] = 0x00; //clear last char
  notepad_append(SST25_buffer, shiftChar, 0);

  writeFlash(4096);

  readSecter(4096);
  // printf("on use:%d", findTotalSectorUSED());
}
void MainPrograme();
int doing = 0;

int sumLineInPreviousSec();
typedef struct marker
{
  char strMark;       // FILE NAME
  int pageinFile[20]; // SIZE OF INT X 20 = 80 BYTES
} marker;
int marker_findLastPosition()
{
  int pp = 0;
  while (SST25_buffer[pp] != '\0')
  { //find last postion
    pp++;
  }
  return pp;
}
void markger_set(char *filename_1)
{
  int lastPo = marker_findLastPosition();
  int cc = 0;
  //printf("last po %d", lastPo);
  // cc = lastPo;
  while (cc < strlen(filename_1))
  {
    SST25_buffer[lastPo + cc] = filename_1[cc];

    //printf("%c", filename_1[cc]);
    cc++;
  }
  SST25_buffer[lastPo + cc] = '\0';
}
void getMarker(char *fileName_1, int line)
{

  /*else
  {
    printf(" can't create the file\r\n");
  }

  mark_insertLine("testt.brf", 1);
  mark_insertLine("testt.brf", 231);

  mark_printfPageOfMarker("testt.brf");

  if (mark_CreateMarkFileConfig("testt3.brf"))
  {
    mark_insertLine("testt3.brf", 2);
  }
  mark_printfPageOfMarker("testt3.brf");*/
}
void SearchMarkerWithFileNameInSector()
{
}
void marker_get()
{
}
// return ; position at last character

void setConfigStr()
{
}
void readConfigFileFormSD()
{
  char *fileName_ = "data.dat";
  printf("readfile\r\n");
  command_ = 1;
  // getMarker("test");
  /*while (1)
  {
    if (command_ == 1)
    {   
      setFilename(fileName_);                     // ใช้คำสั่ง set ชื่อไฟล์
      printf("opening file (%s)\r\n", fileName_); // กำลังเปิดไฟล์ ชื่อไฟล์
      command_++;                                   // 5
      stringToUnicodeAndSendToDisplay("Reading....");
      delay_ms(45);
    }
    else if (command_ == 2)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      command_++; //6
      printf("open  \r\n");
      delay_ms(45);
    }
    else if (command_ == 3)
    {
      SendCH370(SetByteRead, sizeof(SetByteRead));
      printf("reading 3\r\n");
      //result 1D can read
      //ID สามารถอ่านไฟล์ได้
      command_ = 99;
      delay_ms(30);
    }
    else if (command_ == 95)
    {
      //left (prevois line)
      printf("reading 4\r\n");
      SendCH370(continueRead, sizeof(continueRead)); // 0x3b // enum go
      command_++; //96
    }
    else if (command_ == 98)
    {
      printf("reading 5\r\n");
      //right (next line)
      command_ = 6;
    }
    else if (command_ == 99)
    {
       printf("reading 6\r\n");
      SendCH370(ReadData, sizeof(ReadData)); // 0x27
      command_++;
    }
    //menu_s();
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      // printf("a");
      i1 = USART_ReceiveData(USART3);
      printf("0x%x\r\n", i1);
      if (command_ == 96 && i1 == 0x14)
      {

        
      }
    }
  }*/
}
uint8_t CH376Init();
uint8_t CH376_SetReadForm_SD();
uint8_t CH376_Disk_Connect();
uint8_t CH376_UsbDiskMount();

bool isSpaceKey(int);
char tempp__[20];
int filesize__[4];
int getSizeState = 1;
int CH376_GetFileSize();
void getFileSize(char *);
int waitRespon(int);
int timeOut_resp = 1000;
int ch376_status;
int preStatus = 1;

uint8_t loovar = 0;
uint8_t tempLL[14];

char rootBuffer[20];
char fileByte[20];

//--------22/7/2562--------
void readTextFileToMemmory(char *fileName);
//void prerpare_edit_notepad();
int raedingProcess = 0; // edit ch376
bool Editing = false;
void editFile(void);
void prepareEditor(void);
bool useFEdit = 0;
//void createFileAndWrite_hisPath(char *);
int createFile2(char *name);
void writeFile40962(char *fname, char *strSource);
int fileWrite2(int k, char *filename, char *string__);

void prepareForOpenDir()
{
  while (1)
  {
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
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      printf("0x%x \r\n");
    }
    if (command_ == 4 && i1 == 0x14)
      break;
  }
}
void prepareForOpenDir2(char *fileName)
{
  command_ = 5;
  printf("----prepareForOpenDir2----------\r\n");
  while (1)
  {
    if (command_ == 5)
    {
      setFilename(fileName); // ใช้คำสั่ง set ชื่อไฟล์
      //printf("opening file (%s)\r\n", fileName____); // กำลังเปิดไฟล์ ชื่อไฟล์
      command_++; // 5
      delay_ms(45);
    }
    else if (command_ == 6)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      command_++; //6
      //printf("openfile %s status  \r\n", fileName____);
      delay_ms(45);
    }
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      printf("0x%x \r\n");
    }
    if (command_ == 7 && i1 == 0x14)
      break;
  }
}
void createFileAndWrite_hisPath(char *fname)
{
  /*int count__a = 0;

  //ex_exitOncePath();

  //ex_exitOncePath();
  //printf("dir path is %s -----------\r\n", Dirpath);
  command_ = 1;
  prepareForOpenDir();
  ex_cdWithPath(Dirpath);
  command_ = 0;
  while (1)
  {
    if (createFile(fname) == 1) // สร้างไฟล์
      break;
  }

  SendCH370(ResetAll, sizeof(ResetAll)); // reset all
  delay_ms(200);
  printf("file created \r\n");
  prepareForOpenDir2(fname);
  ex_cdWithPath(Dirpath);
  command_ = 1;
  prepareForOpenDir();    // open path
  ex_cdWithPath(Dirpath); // open path
  printf("\r\ncd path \r\n");
  //prepareForOpenDir2(fname); // open file
  delay_ms(100);
  //command_ = 1;
  PrepareText(); // เตรียมข้อความ ลบ Enter Sign add '\r\n'
  printf("end prepare\r\n");
  writeFile4096(fname, SST25_buffer); //เขียนไฟล์*/
  //use
  command_ = 0;
  command_++;
  while (1)
  {
    if (createFile(fname) == 1) // สร้างไฟล์
      break;
  }
  SendCH370(ResetAll, sizeof(ResetAll)); // reset all
  delay_ms(100);
  command_ = 1;
  PrepareText(); // เตรียมข้อความ ลบ Enter Sign add '\r\n'
  //printf("all text :%s\r\n====================================================== ", str_test);
  //fileWrite(0,fname,"test head \r\n my name is surasak");
  writeFile4096(fname, SST25_buffer);                    //เขียนไฟล์
  stringToUnicodeAndSendToDisplay("Saved successfully"); //Saved successfully

  beep4();
  delay_ms(1300);
}
void beep(void);
uint8_t test_step1[] = {0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09}; //14
uint8_t test_step2[] = {0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12}; //25
uint8_t test_step3[] = {0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24}; //36
uint8_t test_step4[] = {0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0}; //78
uint8_t test_step5[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; //ff
int testing = 1;
int main(void)
{
  Notepad.cursorPosition = 0;
  Notepad.currentLine = 0;
  Notepad.displayFirst = false;
  pages.index = 0;
  pages.currentPage = 0;
  pages.totalPage = 0;
  read.totalLine = 0;
  read.MainCurrentLine = 0;
  ROMR.endReadFile = false;
  ROMR.countdata_Temp512 = 0;
  ROMR.lastAscii = 0;
  ROMR.addressWriteFlashTemp = 0;

  buffAs[0] = (char)enterSign; //เครื่องหมาย Enter ใช้ใน notepad_main();

  /* Configure the GPIO ports */
  GPIO_Configuration();   //if config this can't use printf
  UART4_Configuration();  //9600 ,ble
  USART2_Configuration(); //115200 ,keyboard
  USART1_Configuration(); //115200 ,ft232
  USART3_Configuration(); //115200 ,ch376

  Init_SPI();
  delay_init();
  sendUart(1);
  configDisplay();

  displayPrepare();
  //prepareSD_Card();
  MemSize();

  //printf("press enter\r\n");
  // appendToSECTOR();
  //if(DeleteFile("D.TBT")) //ใช้ได้
  //getMarker("a.txt",5);
  ch376_status = CH376Init();
  if (ch376_status != USB_INT_SUCCESS)
    printf("ch376 init error0");
  MainPrograme();
  /* delay_ms(2000);
  while(1){
    beep4();
    stringToUnicodeAndSendToDisplay("Synchrotron light");
    delay_ms(2000);

    beep4();
    stringToUnicodeAndSendToDisplay("research institute");
    delay_ms(2000);
    
    beep4();
    stringToUnicodeAndSendToDisplay("ir}r(nun|*vod");
    delay_ms(2000);

    
    beep4();
    stringToUnicodeAndSendToDisplay("g*r<sd}pl,?*-s*:)y");
    delay_ms(2000);
    
    
    beep4();
    stringToUnicodeAndSendToDisplay(")dsov #abcdefghi");
    delay_ms(2000);
    
    beep4();
    stringToUnicodeAndSendToDisplay("test #abcdefghi");
    delay_ms(2000);
  }*/
  //getFileSize("/AAAAAA~1.TXT");
  //printf("get file size \r\n");
  //ch376_status = CH376_GetFileSize();
  //printf("file size %d byte\r\n", ch376_status);
  //getLongName("/avccc~1.txt", "test");
  //readConfigFileFormSD();
}
//-------------- เอาไฟล์ที่อ่านใน ROM เก็บเข้า Array เพื่อแก้ไขต่อไป ----------------

void editFile()
{
  //printf("Editing ----------------\r\n");
}
void prepareEditor()
{
  int i__ = 0; // วนหลูป
  int jjcopy = 0;
  int last_line_break = 0;
  int last_line_break_pre = 0;
  int editor_currentLine = 0; // เลื่อนบรรทัด
  int editor_position = 0;
  int count_member_in_Line = 0;
  int MaxInLine__ = 40;
  uint8_t count_in_line = 0;
  uint8_t fillEnterLine = 0;
  uint8_t buffer_count_in_line = 0;
  /*
  AmountSector
  AmountSectorT
  */
  // Sector ที่ได้จะได้ไม่เกิน 4096 byte
  //printf("sector = %d & %d\r\n", AmountSector, AmountSectorT);
  // store data to array
  for (i__ = 0; i__ < AmountSectorT; i__++) // วนหลูปตามจำนาน
  {
    //printf("%c", SST25_buffer[i__]); // แสดงค่าที่อ่านมา อยู่ในตัวแปล SST25_bufferฃ
    if (SST25_buffer[i__] == '\r')
    { // check line break
      // \r \n 0x0a
      last_line_break = i__; // เก็บแค่ตัวหน้า ไม่เอา \r\n
      //copy string hear;
      printf("data is : ");
      for (jjcopy = last_line_break_pre; jjcopy < last_line_break; jjcopy++) // copy str จนถึง \r\n หรือหมด
      {
        // copy charactor here
        Notepad.buffer_string[Notepad.currentLine][count_in_line] = SST25_buffer[jjcopy];
        //printf("%c", Notepad.buffer_string[Notepad.currentLine][count_in_line]);
        count_in_line++; // นับว่า copy ไปกี่ตัวแล้ว เพราะจะเอาที่เหลือไปเติมเครื่องหมาย Enter or fill lin
      }
      //printStringInLine(Notepad.buffer_string[Notepad.currentLine]);
      //printf("\r\n");
      //printf("count_in_line copy is %d\r\n", count_in_line);
      //fillEnterLine = strlen(Notepad.buffer_string[Notepad.currentLine]);
      //printf("Fill (%d)\r\n", 40 - count_in_line);
      for (jjcopy = 40; jjcopy >= count_in_line; jjcopy--)
      {
        //printf("|");
        // Notepad.buffer_string
        Notepad.buffer_string[Notepad.currentLine][jjcopy] = (char)enterSign;
      }
      /*for (i = 0; i < 40; i++)
      {
        printf("%c", Notepad.buffer_string[Notepad.currentLine][i]);
      }*/
      //printf("\r\n======================================================\r\n");
      last_line_break_pre = last_line_break + 2; // เก็บค่าก่อนหน้า
      //printf("last_line_break_pre is %d\r\n", last_line_break_pre);
      Notepad.currentLine++; //เลื่อนบรรทัด
      buffer_count_in_line = count_in_line;
      count_in_line = 0; // ล้างนับจำนวนในบรรทัด
    }
  }
  Notepad.cursorPosition = buffer_count_in_line; //เก็บตำแหน่ง Curser
  //printf("-------------------------------------------------------end presents \r\n");
}
/* 
Function:  readTextFileToMemmory(char *fileName)
---------------------------------------
-> อ่านไฟล์จาก SD Card ไปยัง ROM และทำการอ่าานมาแก้ไขใน memory
->
RETURN :  ถ้าไฟล์น้อยกว่า 4k อ่านได้ || ถ้ามากกว่าให้รีเทิร์น error
987
*/
void readTextFileToMemmory(char *fileName____)
{
  readFileStatus___ = 1;    //ค่าเริ่มต้นการอ่าน เพื่อให้ติดหลูปการอ่านไฟล์
  ROMR.endReadFile = false; //จบการอ่่าน
  ROMR.countdata_Temp512 = 0;
  ROMR.lastAscii = 0;
  ROMR.addressWriteFlashTemp = 0;
  addressSector = 0;
  countSector512 = 0;
  countSector4096 = 0;
  countSector = 0;
  // stringToUnicodeAndSendToDisplay("Reading....");
  // SendCH370(ResetAll, sizeof(ResetAll)); //reset chip
  // printf("reading............ all\r\n");
  delay_ms(10);
  command_ = 4;
  while (readFileStatus___ == 1)
  {
    if (command_ == 4)
    {
      setFilename(fileName____); // ใช้คำสั่ง set ชื่อไฟล์
      //printf("opening file (%s)\r\n", fileName____); // กำลังเปิดไฟล์ ชื่อไฟล์
      command_++; // 5
      delay_ms(45);
    }
    else if (command_ == 5)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      command_++; //6
      //printf("openfile %s status  \r\n", fileName____);
      delay_ms(45);
    }
    else if (command_ == 6)
    {
      SendCH370(SetByteRead, sizeof(SetByteRead));
      //printf("SetByteRead 3\r\n");
      //result 1D can read
      //0x1d สามารถอ่านไฟล์ได้
      command_ = 99;
      delay_ms(30);
    }
    else if (command_ == 95)
    {
      //left (prevois line)
      //printf("continueRead 4\r\n");
      SendCH370(continueRead, sizeof(continueRead));
      command_++; //96
    }
    else if (command_ == 98)
    {
      //printf("reading 5\r\n");
      //right (next line)
      command_ = 6;
    }
    else if (command_ == 99)
    {
      //printf("read data 6\r\n");
      SendCH370(ReadData, sizeof(ReadData));
      command_++;
    }

    //menu_s();
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      // printf("a");
      i1 = USART_ReceiveData(USART3);
      //printf("command = %d ,%x", i1,command_); //ปริ้นแล้วอ่านไม่ทัน
      if (command_ == 96 && i1 == 0x14)
      {
        command_ = 6;
      }
      if (i1 == 0x80 || i1 == 0x14)
      {
        // printf("funk\r\n");
      }
      else if (command_ == 100 && countSector512 < 4 && ROMR.countdata_Temp512 < 512)
      {
        if (i1 == '\0')
        {
          // printf("End of File\r\n");-
        }
        else if (ROMR.countdata_Temp512 < ((128) * (countSector512 + 1)) - 1)
        {
          //------------------------------128 byte----------------------------
          // เก็บcharactor 128 byte
          ////////////////////////////////////////////////////////////////////
          ROMR.dataTemp512[ROMR.countdata_Temp512] = i1;
          ROMR.countdata_Temp512++;

          ROMR.waitEnd = 0;
          ROMR.lastAscii = i1;
        }
        else
        {
          //-----------------------------------------------------------------
          // ข้อมูลที่่อ่านครบ 128*4 = 512
          // ทำการเพิ่มคัวแปร countSector512
          ///////////////////////////////////////////////////////////////////

          countSector512++;
          ROMR.dataTemp512[ROMR.countdata_Temp512] = i1;
          ROMR.countdata_Temp512++;
          if (countSector512 >= 4)
          {
            //printf("store \r\n");
            //---------------------------512 byte-----------------------------
            //  เอาข้อมูล 512 เก็บลง buffer ยาว [4096] จนครบ
            //
            //////////////////////////////////////////////////////////////////
            command_ = 95;
            for (i = ROMR.addressWriteFlashTemp; i < ROMR.addressWriteFlashTemp + 512; i++)
            {

              SST25_buffer[i - (sector * countSector)] = ROMR.dataTemp512[i - ROMR.addressWriteFlashTemp];
              ROMR.dataTemp512[i - ROMR.addressWriteFlashTemp] = '\0'; //clear buffer
              // printf("92 2\r\n");
            }
            // printf("store 2\r\n");
            //  ทำการเพิ่มจำนวนตัวแปรที่นับ sector:countSector4096
            countSector4096++;
            //writeFlash(ROMR.addressWriteFlashTemp);
            ROMR.addressWriteFlashTemp += ROMR.countdata_Temp512;
            if (countSector4096 >= 8)
            {
              //---------------------------4096 byte---------------------------
              // ข้อมูลครบ 512*8 = 4096 ----> 1 sector
              // push (string 4096 charactor) to flash rom.
              //
              //////////////////////////////////////////////////////////////////
              //use variable:SST25_W_BLOCK //4096
              // stringToUnicodeAndSendToDisplay("Reading....");
              writeFlash(addressSector); //1*4096
              //clearUnsignChar();

              //Delay(0xff);
              stringToUnicodeAndSendToDisplay("On read file Reading....");
              addressSector += sector;
              countSector4096 = 0;
              //--------------------------------------------------------------------------------------------
              //check this value `ROMR.addressWriteFlashTemp`--
              //--------------------------------------------------------------------------------------------
              countSector++;
            }
            //--- reset temp sector---//
            ROMR.countdata_Temp512 = 0;
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
      if (ROMR.lastAscii == i1)
      {
        ROMR.waitEnd++;
        //กำหนดระยะเวลา หากไม่มีข้อมูลแล้ว จะเข้าในเงื่อนไขนี้
        if (ROMR.waitEnd == 100 * 100) // end of file check time out
        {
          SendCH370(FileClose0, sizeof(FileClose0));
          beepError();
          for (i = ROMR.addressWriteFlashTemp; i < ROMR.addressWriteFlashTemp + 512; i++)
          {
            SST25_buffer[i - (sector * countSector)] = ROMR.dataTemp512[i - ROMR.addressWriteFlashTemp];
          }
          configFlash();
          writeFlash(addressSector);
          //SendCH370(ResetAll, sizeof(ResetAll));
          ROMR.addressWriteFlashTemp += ROMR.countdata_Temp512;
          ROMR.waitEnd++;
          //----------------------store last sector to flash rom-----------------------
          //
          /////////////////////////////////////////////////////////////////////////////
          // readSecter(0);
          //printf("string** : %s \r\nEND**", SST25_buffer);
          /*SPI_FLASH_CS_LOW();
                    SST25_R_BLOCK(0, SST25_buffer, sector);
                    SPI_FLASH_CS_HIGH();
                    Delay(0xffff);
                    convert_text2_buffer(SST25_buffer);*/

          readFileStatus___ = 0; //exit read text

          AmountSector = ROMR.addressWriteFlashTemp / sector;  // ---- จำนวน sector ----
          AmountSectorT = ROMR.addressWriteFlashTemp % sector; // ---- เศษ ที่เหลือของ sector ---

          //----------------------------------
          // เก็บบรรทัด
          // จำนวน sector และตัวเศษ
          /*
          mark_init(); // set mark position at 0
          mark_readFormROM();
          initSlidingMode();
          readSecter(0);
          */
          /* while (Editing == true)
          {
            // query string-
            // menu_s();
            keyRead();
            // printf("9999999999999999srwetre\r\n");
            // readFileStatus___
          }*/

        } // ทดสอบ
      }
    }
  }
  //return 0;
}
/*
void prerpare_edit_notepad()
{

}*/
int mark_findListTotalInArray()
{
  int acce = 0;
  int count_maxMark = 0;
  for (acce = 0; mark_MarkerPage[acce] != NULL; acce++) // find total
  {
    count_maxMark++;
  }
  return count_maxMark;
}
void mark_init()
{
  mark_currentPostion = 0;
}
void mark_readFormROM()
{
  memset(SST25_buffer, 0, strlen(SST25_buffer));
  readSecter(4096 * 999);
  //printf("data:%s\r\n", SST25_buffer);
  if (mark_CreateMarkFileConfig(fileLists[fileSelect])) //สร้างสำเร็จ
  {
    mark_insertLine(fileLists[fileSelect], read.currentLine + 1);
  }
  else // หรือมีไฟล์อยู่แล้ว
  {
    mark_insertLine(fileLists[fileSelect], read.currentLine + 1);
  }
}
/******************************************
 * 0-15 store name 
 * 
 * 
 * */
void setMark()
{
  //0xffbf6a
}

/*
===================================================================
===================================================================
===================================================================
===================================================================
*/
int mark_hasFileConfig(char *filename)
{
  int *ptr;
  ptr = (int *)strstr(SST25_buffer, filename);
  if (ptr == NULL)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
int mark_CreateMarkFileConfig(char *filename)
{
  int range = 0;
  char fileTemp[15];
  if (!mark_hasFileConfig(filename)) // มีไฟล์่ชื้อนี้ยัง ถ้ายัง
  {                                  // not found
    range = strlen(SST25_buffer);    //หาความยาว
    strcpy(fileTemp, filename);      //เก็บ
    strcat(fileTemp, "/:");
    if (range == 0)
    {
      //SST25_buffer = malloc(sizeof(fileTemp));
      strcpy(SST25_buffer, fileTemp);
    }
    else
    {
      strcat(SST25_buffer, fileTemp);
    }
    printf("%s", SST25_buffer);
    return 1;
  }
  else // มีไฟล์ชื่อนี้แล้ว
  {
    //printf("ee"); // has
    return 0;
  }
}
//==============================================
// fn:releace string
// origin,rex,new text
//==============================================
char *mark_str_replace(char *orig, char *rep, char *with)
{
  char *result;  // the return string
  char *ins;     // the next insert point
  char *tmp;     // varies
  int len_rep;   // length of rep (the string to remove)
  int len_with;  // length of with (the string to replace rep with)
  int len_front; // distance between rep and end of last rep
  int count;     // number of replacements

  // sanity checks and initialization
  if (!orig || !rep)
    return NULL;
  len_rep = strlen(rep);
  if (len_rep == 0)
    return NULL; // empty rep causes infinite loop during count
  if (!with)
    with = "";
  len_with = strlen(with);

  // count the number of replacements needed
  ins = orig;
  for (count = 0; tmp = strstr(ins, rep); ++count)
  {
    ins = tmp + len_rep;
  }

  tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

  if (!result)
    return NULL;

  // first time through the loop, all the variable are set correctly
  // from here on,
  //    tmp points to the end of the result string
  //    ins points to the next occurrence of rep in orig
  //    orig points to the remainder of orig after "end of rep"
  while (count--)
  {
    ins = strstr(orig, rep);
    len_front = ins - orig;
    tmp = strncpy(tmp, orig, len_front) + len_front;
    tmp = strcpy(tmp, with) + len_with;
    orig += len_front + len_rep; // move to next "end of rep"
  }
  strcpy(tmp, orig);
  return result;
}
//---------------------------------------------------------------
//
//
//---------------------------------------------------------------
int mark_searchName(char *fileName)
{
  int startPos, EndPos, start2;
  char *ptr;
  ptr = strstr(SST25_buffer, fileName);
  memset(data_temp, 0, sizeof(data_temp));
  //printf("file name :%s\r\n", fileName);
  if (ptr != NULL)
  {
    startPos = ptr - SST25_buffer; // find start index
    start2 = startPos;
    startPos = strlen(fileName) + startPos;
    EndPos = mark_getEndSign(startPos); // find end index
    //printf("%d\r\n",strlen(data));
    //printf("start %d end %d\r\n", startPos, EndPos);
    strncpy(data_temp, SST25_buffer + start2, EndPos - start2);
    //printf("end :%c\r\n",data[EndPos]); // index
    //printf("file name %s\r\n",fileName);
    //	printf("start at (%d), end at (%d)\r\n", startPos,EndPos);
    //printf("char start (%c), end (%c)\r\n",data[startPos],data[EndPos]);
    mark_filterPageToArray(startPos, EndPos);
    return 1;
  }
  else
  {
    return 0;
  }
}
int mark_getEndSign(int StartAt)
{
  int ccCO;
  int DataSize;
  DataSize = strlen(SST25_buffer);
  //printf("data size:%d",DataSize);
  ccCO = StartAt;
  while (ccCO < DataSize)
  {
    if (SST25_buffer[ccCO] == ':')
      break;
    if (ccCO > DataSize)
      break;
    ccCO++;
  }
  return ccCO;
}
void mark_filterPageToArray(int pStart, int pEnd)
{
  int countDigit = 0;
  int numPrepare = 0;
  int arrayIndex = 0;
  int count = pStart;
  char buffNumber[7];

  for (countDigit = 0; countDigit < mark_MarkMaxBufferPage; countDigit++)
    mark_MarkerPage[countDigit] = 0x00;
  countDigit = 0;
  while (count < pEnd)
  {
    //printf("%c",data[count]);
    if (SST25_buffer[count] >= 48 && SST25_buffer[count] <= 57)
    { // is number
      //printf("%c\r\n",data[count]);
      buffNumber[countDigit++] = SST25_buffer[count];
    }
    else if (SST25_buffer[count] == 47)
    { // is slash
      // push to array here
      numPrepare = atoi(buffNumber);
      memset(buffNumber, 0, sizeof(buffNumber));
      mark_MarkerPage[arrayIndex] = numPrepare;
      arrayIndex++;
      //printf("number is %d\r\n",numPrepare);
      countDigit = 0;
    }
    count++;
  }
  arrayIndex = 0;
  count = 0;
  countDigit = 0;
  //printf("start %c, end %c",data[pStart],data[pEnd]);
}
void mark_sortPageAndStore()
{ //remove page and store to rom
  readSecter(4096 * markSector);
  for (j = 0; mark_MarkerPage[j] != NULL; j++) //วนหลูปตามจำนวน page
  {
  }
}
int mark_RemoveArrayAtIndex(int position)
{
  int c;
  int n;
  n = mark_sizeOfList();
  //printf("size of list %d\r\n",n);
  for (c = position; c < n; c++)
    mark_MarkerPage[c] = mark_MarkerPage[c + 1];
  mark_MarkerPage[c] = 0x00; //clear
  return 1;
}
int mark_sizeOfList()
{
  int cc = 0;
  while (mark_MarkerPage[cc] != NULL)
  {
    cc++;
  }
  return cc;
}
int mark_findIndexOfArray(int target)
{
  int cc = 0;
  while (mark_MarkerPage[cc] != NULL)
  {
    if (mark_MarkerPage[cc] == target)
      break;
    cc++;
  }
  return cc;
}
int mark_findIndexForInsert(int value)
{
  int cc;
  cc = mark_sizeOfList() - 1;
  while (cc >= 0)
  {
    if (value > mark_MarkerPage[cc])
      break;
    cc--;
  }
  cc++;
  return cc;
}
int mark_insertMark(int line)
{
  int n, index;
  int cc = 0;
  int position;
  n = mark_sizeOfList();
  index = mark_findIndexOfArray(line);
  position = mark_findIndexForInsert(line); // find
  //printf("index %d,position %d, n %d\r\n", index, position, n);
  //printf("n = %d index %d---------\r\n",n,index);
  if (index != n)
  {
    return 0; //error;
  }
  else
  {
    for (cc = n - 1; cc >= position - 1; cc--)
      mark_MarkerPage[cc + 1] = mark_MarkerPage[cc];
    mark_MarkerPage[position] = line;
    return 1;
  }
}
void mark_printfPageOfMarker(char *filename)
{
  int cc = 0;
  mark_searchName(filename);
  while (mark_MarkerPage[cc] != NULL)
  {
    printf("page:%d\r\n", mark_MarkerPage[cc]);
    cc++;
  }
}
/***************************************************************************
*
*         เพิ่มตำแหน่งหน้าไปยังชุด string abx.txt1/2/6 ที่อยู่ในตัวแปร SST25_buffer 
* ที่อ่านมาสำหรับเก็บค่า page ใน rom หลังจากใช้ฟังก์ชั่นนี้ค่าใน SST25_buffer จะมีการเปลี่ยนแปลง
* เช่นอ่านไฟล์อยู่ หรือกำลังพิมพ์ 
....
*/
void mark_insertLine(char *filename, int line)
{
  char temp__[30];
  char numStr[7];
  int cc = 0;
  //printf("insert (%d)\r\n", line);
  if (mark_searchName(filename))
  {                            // found file
    if (mark_insertMark(line)) //ถ้าเพิ่ม Mark ไปยัง array global ได้สำเร็จ
    {
      //----------------เก็บ buffer สำหรับ repleace file name และหน้า page
      strcpy(temp__, filename);           // store file name to `temp__`
      while (mark_MarkerPage[cc] != NULL) //ต่อหน้า /xxx/xxx
      {
        sprintf(numStr, "%d", mark_MarkerPage[cc]); // num to string
        strcat(temp__, numStr);                     // เอา string ที่ได้มา ต่อท้าย
        strcat(temp__, "/");                        // / ต่อท้ายstring
        cc++;
      }
      //printf("data temp -> %s\r\n",temp__);
      strcpy(data2, mark_str_replace(SST25_buffer, data_temp, temp__)); // แทนที่ ใหม่ แทน เกา xxx/xxx -> xxx/xxx/xxx
      //free(SST25_buffer);
      //SST25_buffer = malloc(sizeof(data2));
      memset(SST25_buffer, 0x0, strlen(SST25_buffer)); //ลบค่าใน SST25_buffer
      strcpy(SST25_buffer, data2);                     // เก็บค่าเดิม
      printf("->>>>%s\r\n", SST25_buffer);
      //memset(data,0,sizeof(data));
      //strcpy(data,data2);
      //
    }
  }
}
/*
===================================================================
===================================================================
===================================================================
===================================================================
*/

void MainPrograme()
{
  while (1)
  {
    keyRead(); // mode = 0; เลื่อนเมนู enter, exit
    while (mode == MODE_NOTEPAD)
    {
      doing = 1; //ตัวแปร global สำหรับทำงาน notepad :D
      //printf("--------------------------reg-------------\r\n");
      notepad_main();
    }
    while (mode == MODE_READ)
    {
      searchFile2(); //   keyRead();
    }
    if (mode == MODE_BLUETOOTH)
    {
      GPIO_SetBits(GPIOC, GPIO_Pin_0); // on Bluetooth
      delay_ms(800);                   //wait bluetooth boot
      strcpy(bluetoothNambuffer, getBluetoothName());
      if (strlen(bluetoothNambuffer) > 0)
        stringToUnicodeAndSendToDisplay(bluetoothNambuffer);
      else // Error
        stringToUnicodeAndSendToDisplay("Please check Module");
      while (mode == 3)
      {
        BluetoothMode();
        if (becon == 0) // ถ้ายังไม่มีการเชื่อมต่อ
        {
          keyRead();
          //menu_s();
        }
        else
        {
          keyboardMode();
        }
      }
    }
    while (mode == MODE_TOOLS)
    {
      mode4();
    }
  }
}

uint8_t CH376Init()
{
  uint8_t status;
  status = CH376_SetReadForm_SD();
  if (status != CMD_RET_SUCCESS)
    return status;
  status = CH376_Disk_Connect();
  if (status == 0)
    return status;
  status = CH376_UsbDiskMount();
  if (status != 0x14)
    return status;
  return status;
}
int CH376_GetFileSize()
{
  filesize__[0] = 0;
  filesize__[1] = 0;
  filesize__[2] = 0;
  filesize__[3] = 0;
  SendCH370(FileSize, sizeof(FileSize));
  // delay_ms(45);

  filesize__[0] = waitRespon(100000);
  filesize__[1] = waitRespon(1000);
  filesize__[2] = waitRespon(1000);
  filesize__[3] = waitRespon(1000);
  /* while(1){

  }*/
  printf("%d %d %d %d\r\n", filesize__[0], filesize__[1], filesize__[2], filesize__[3]);
  return (filesize__[0] + (filesize__[1] * 256) + (filesize__[2] * 256 * 256) + (filesize__[3] * 256 * 256 * 256)); // คำนวณขนาดไฟล์ https://arduinobasics.blogspot.com/2015/05/ch376s-usb-readwrite-module.html
}
// function

uint8_t CH376_Disk_Connect()
{
  SendCH370(DiskConnect, sizeof(DiskConnect));
  return waitRespon(10000);
}
uint8_t CH376_UsbDiskMount()
{
  SendCH370(USBDiskMount, sizeof(USBDiskMount)); // has delay response
  delay_ms(45);
  return waitRespon(10000);
}
uint8_t CH376_SetReadForm_SD()
{
  SendCH370(setSDCard, sizeof(setSDCard));
  return waitRespon(10000);
}

int waitRespon(int time)
{
  int retur_;
  timeOut_resp = time; //timeout
  while (timeOut_resp--)
  {
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      retur_ = USART_ReceiveData(USART3);
      break;
    }
  }
  if (timeOut_resp <= 0) // time out
    return 0;
  else
    return retur_;
}
// REMOVE DATA IN FILE TEMP
int removeDataInFileTemp(char *fileRe)
{
  int intdexPo = 0;
  int lastIndex = 0;
  //fileRe
  //fileLists
  for (i = 0; i < sizeof(fileLists) / sizeof(fileLists[0]); i++) //find last index
  {
    if (strlen(fileLists[i]) != 0)
    {
      lastIndex++;
    }
    else
    {
      break;
    }
  }
  for (i = 0; i < lastIndex; i++) //หา ตำแหน่ง
  {
    if (strlen(fileLists[i]) != 0) // name of file not null
    {
      if (strcmp(fileRe, fileLists[i]) == 0) //compare str
      {
        intdexPo = i + 1;
        break;
      }
    }
  }
  if (intdexPo >= lastIndex + 1)
  {
    printf("Delete not possible.\n");
  }
  else
  {
    //printf("delete on position %d \r\n", intdexPo);
    for (i = intdexPo - 1; i < lastIndex - 1; i++)
    {
      strcpy(fileLists[i], fileLists[i + 1]);
    }
    //printf("Resultant array:\n");
    for (i = 0; i < lastIndex - 1; i++)
    {
      printf("%s\n", fileLists[i]);
    }
    maxFile--;
    fileSelect--;
  }
  return 1;
}
/*******************************************************************************
* Function Name  : confirm_delete function
* Description    : 
* Input          : None
* Output         : None
* Return         : 1 or 0 
*******************************************************************************/
int confirm_delete()
{
  int status = 2;
  int tmod = 1;
  while (tmod)
  {
    notepad_readKey();       // key recieve
    if (countKey >= maxData) // do events
    {
      //Recieve & checking key
      seeHead = 0;
      if (checkKeyError == 0xff)
      {
        //clear error key
        countKey = 0;
        SeeHead = 0;
      }
      printf("See key %x,%x,%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
      if (checkKeyError == 0xff)
      {
        //clear error key
        countKey = 0;
        SeeHead = 0;
      }
      if (bufferKey3digit[0] == 0x80 && bufferKey3digit[1] == 0 && bufferKey3digit[2] == 0)
      {
        // key 8
        status = 1;
      }
      else if (bufferKey3digit[0] == 0x40 && bufferKey3digit[1] == 0 && bufferKey3digit[2] == 0)
      {
        // key 7
        status = 0;
      }
      else if (bufferKey3digit[0] == 00 && bufferKey3digit[1] == 0 && bufferKey3digit[2] == 0x02)
      {
        // right joy enter
        status = 1;
      }
      else if (bufferKey3digit[0] == 00 && bufferKey3digit[1] == 0x10 && bufferKey3digit[2] == 0)
      {
        // left joy enter
        status = 1;
      }
      else if (bufferKey3digit[0] == 00 && bufferKey3digit[1] == 0 && bufferKey3digit[2] == 0x04)
      {
        // right joy arrow left
        status = 0;
      }
      else if (bufferKey3digit[0] == 00 && bufferKey3digit[1] == 0x20 && bufferKey3digit[2] == 0)
      {
        // left joy arrow left
        status = 0;
      }
      if (status != 2)
        tmod = 0;
      // key mapping //
      // keyCode = keyMapping(bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
      clearKeyValue();
    }
  }
  return status;
}
uint16_t page_getCurrentPage() // bug
{

  //pages.page
  int page_Sum = sumLineInPreviousSec();
  k = 0;
  for (k = 0; k < pages.totalPage; k++)
  {
    if (pages.page[k] > page_Sum + read.currentLine)
    {
      break;
    }
  }
  //  printf("---------current line %d  k is %d\r\n", read.currentLine, k);
  return k;
  //pages.page[i];
}

uint16_t get_CurrentLine()
{
  int jCC = 0;
  int CUR_SEC = read.currentSector;
  int CUR_LINE = read.currentLine;
  int SumLine = 0;
  // จำนวนบรรทัดใน sector ก่อนหน้า + currentLine = บรรทัดจริงๆ
  for (jCC = 0; jCC < CUR_SEC; jCC++)
  {

    SumLine += read.lineInsector[jCC];
  }

  return SumLine + CUR_LINE;
}
uint16_t page_findNextPage()
{
  int page_Sum = sumLineInPreviousSec();
  // printf("sumPrevoiusPage %d current line %d sum all %d \r\n", page_Sum, read.currentLine, page_Sum + read.currentLine);
  k = 0;
  for (k = 0; k < pages.totalPage; k++)
  {
    if (pages.page[k] > page_Sum + read.currentLine + 1)
    {
      break;
    }
  }
  // printf("page at %d\r\n");
  // return [page number]
  return pages.page[k];
}
int sumLineInPreviousSec()
{
  int cc = 0;
  k = 0;
  // printf("---------current secter  %d  line in sector %d\r\n", read.currentSector, read.lineInsector[read.currentSector]);
  for (k = 0; k < read.currentSector; k++)
  {
    cc += read.lineInsector[k];
    cc++;
  }

  // return บรรทัดทั้งหมดของ sector ก่อนหน้า
  return cc;
}
int findPreviousPage()
{
  int currentLine_ = get_CurrentLine();
  int compareInLine = findCurrentPageInArr();
  int start99 = pages.totalPage - 1;
  //printf("----currentline -*-*-* %d \r\n", currentLine_);
  while (start99 > 0)
  {
    if (pages.page[start99] < currentLine_)
    {
      //printf("pages.page[start99] = %d start99 = %d\r\n", pages.page[start99], start99);
      break;
    }
    start99--;
  }
  if (pages.page[start99] == compareInLine)
  {
    start99--;
  }
  // printf("ggg currentPage -- %d -- \r\n", findCurrentPageInArr());
  // return เลข
  return start99;
}
int findCurrentPageInArr()
{
  int currentLine_ = get_CurrentLine();
  int start99 = pages.totalPage - 1;
  //printf("################################################################\r\n");
  while (start99 > 0)
  {
    if (currentLine_ >= pages.page[start99])
    {
      // printf("int fff pages.page[start99] = %d start99 = %d\r\n", pages.page[start99], start99);
      break;
    }
    start99--;
  }
  //printf("################################################################\r\n");
  return pages.page[start99];
}
void upperASCII(char *str)
{
  int i = 0;
  while (*str != '\0')
  {
    if (*str >= 97 && *str <= 122)
    {
      buffascii[i] = *str - 32;
    }
    else
    {
      buffascii[i] = *str;
    }
    i++;
    str++;
  }
}
void getFileSize(char *filename)
{
  // check root path

  if (ex_countPath(Dirpath) == 1)
  {
    //intsertRoottoName(filename);
    memset(rootBuffer, 0, sizeof(rootBuffer));
    rootBuffer[0] = '/';
    strcat(rootBuffer, filename); // name in root / + filename
    setFilename(rootBuffer);
  }
  else
  {
    setFilename(filename);
  }
  //printf("set file name %s size\r\n", filename);
  delay_ms(45);
  SendCH370(FileOpen, sizeof(FileOpen));
  delay_ms(45);
  //ch376_status = CH376_FileOpen();
  while (1)
  {
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      //printf("open status:%x\r\n", i1);
      break;
    }
  }
  //printf("open status:%d", ch376_status);
}
void setFoldername(char *filename)
{
  //setFilenameForFunction //0x00 0x57 0xab 0x2f
  int loovar = 0;
  uint8_t tempLL[14];
  SendCH370(setFilenameForFunction, sizeof(setFilenameForFunction));
  while (filename[loovar] != '\0')
  {
    tempLL[loovar] = filename[loovar];
    loovar++;
  }
  tempLL[loovar] = '\0';
  SendCH370(tempLL, sizeof(tempLL));
}
int DeleteFolder(char *name)
{
  int timeOut = 0;
  int status_delete = 0;
  command_ = 4;
  // printf("delete folder");
  while (1)
  {

    if (command_ == 4)
    {
      // setFoldername
      // convert uppercase to name
      upperASCII(name);
      setFoldername(buffascii);
      //printf(" file temp %s\r\n", buffascii);
      command_++; //8
      delay_ms(50);
    }
    else if (command_ == 5)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      delay_ms(80);
      command_++; //10
    }
    else if (command_ == 5)
    {
      SendCH370(Folder_DELETE, sizeof(Folder_DELETE));
      delay_ms(80);
      command_++; //10
    }
    else if (command_ == 6)
    {

      command_++; //10
    }

    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      if (i1 == 0x41)
      {
        //printf("\r\n ERR_OPEN_DIR \r\n");
      }
      if (i1 == 0x82)
      {
        //printf("\r\n ERR_DISK_DISCON  \r\n");
      }
      if (i1 == 0xa8)
      {
        //printf("\r\n CMD_CHECK_EXIST \r\n");
      }
    }

    if (timeOut > 300)
    {
      status_delete = 0;
      //printf("\r\nError Folder Delete\r\n");
      break;
    }
    if (command_ == 7 && i1 == 0x14)
    {
      status_delete = 1;
      //printf("\r\nFolder Delete complete\r\n");
      break;
    }
    if (command_ == 7 && i1 != 14)
    {
      command_ = 1;
    }
  }
  return status_delete;
}
/****************************************************
ฟังก์ชั่น ลบไฟล์
*****************************************************/
int DeleteFile(char *name)
{
  int timeOut = 0;
  int status_delete = 0;
  command_ = 4;
  //printf("delete file");
  while (1)
  {
    timeOut++;
    if (command_ == 4)
    {
      // ตั้งคือไฟล์สำหรับใช้งานคำสั่งต่อไป
      //printf("removing file %s\r\n", name);
      setFilename(name);
      command_++; //8
      delay_ms(50);
    }
    else if (command_ == 5)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      delay_ms(80);
      command_++; //10
    }
    else if (command_ == 6)
    {
      SendCH370(Folder_DELETE, sizeof(Folder_DELETE));
      delay_ms(80);
      command_++; //10
    }

    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      switch (i1)
      {
      case 0x41:
        //printf("\r\n ERR_OPEN_DIR \r\n");
        break;
      case 0x82:
        //printf("\r\n ERR_DISK_DISCON  \r\n");
        break;
      case 0xa8:
        //printf("\r\n CMD_CHECK_EXIST \r\n");
        break;
      }
    }

    if (timeOut > 300)
    {
      status_delete = 0;
      //printf("\r\nError file Delete\r\n");
      break;
    }
    if (command_ == 7 && i1 == 0x14)
    {
      status_delete = 1;
      //printf("\r\ffile Delete complete\r\n");
      break;
    }
    if (command_ == 7 && i1 != 14)
    {
      command_ = 1;
    }
  }
  return status_delete;
}
//======================================================
//--------------fn go to line-------------------------*
int gotoLine_getSectorInline(int line) //หาว่าบรรทัดนั้นอยู่ใน Sector ไหนใน ROM
{
  int gg = 0;
  int ccLine = 0;
  int sector___ = 0;
  while (ccLine < line)
  {
    ccLine += read.lineInsector[gg];
    if (ccLine >= line)
    {
      sector___ = gg;
      break;
    }
    gg++;
  }
  return sector___;
  //--- return หมายเลข sector
}
int gotoLine_getLine(int Line, int Sector) //หาใน Sector นั้นว่าบรรทัดที่เท้่าไหร่
{
  int line = Line;
  int ccSector = 0;
  if (Sector > 0)
  {
    while (1)
    {
      if (line > read.lineInsector[ccSector])
        line -= read.lineInsector[ccSector];
      if (line <= read.lineInsector[ccSector])
      {
        break;
      }
      if (ccSector >= sizeof(read.lineInsector) / sizeof(read.lineInsector[0])) //à¸šà¸±à¸‡à¸„à¸±à¸šà¹„à¸¡à¹ˆà¹ƒà¸«à¹‰à¹€à¸à¸´à¸™ Size
        break;
      else
        ccSector++;
    }
  }
  return line;
}
int gotoLine_get_int_len(int value)
{
  int l = 1;
  while (value > 9)
  {
    l++;
    value /= 10;
  }
  return l;
}
unsigned int gotoLine_EnterLine(int maxLine)
{
  unsigned int Line = 0;
  int state = 1;
  int curline = 0;
  int i = 0;
  char line[5];
  char mainText[45];
  char numbuffer[5];
  int NumLength;
  strcpy(mainText, "Go to Line:"); //เก็บข้อความ
  strcpy(line, " ");
  memset(line, 0, 5);
  // strcpy(mainText, "Go to Line:");
  strcat(mainText, line);
  strcat(mainText, "/"); //cursor+12
  sprintf(numbuffer, "%d", maxLine);
  strcat(mainText, numbuffer); //num to text
  NumLength = gotoLine_get_int_len(maxLine);
  while (state)
  {
    notepad_readKey();       // key recieve
    if (countKey >= maxData) // do events
    {
      //Recieve & checking key
      seeHead = 0;
      if (checkKeyError == 0xff)
      {
        //clear error key
        countKey = 0;
        SeeHead = 0;
      }
      //printf("key a:%x,b:%x,c:%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
      if (bufferKey3digit[0] == 0x11 && bufferKey3digit[1] == 0x00 && bufferKey3digit[2] == 0x00)
      {
        state = 0;
        Line = -1;
        // printf("Exit ---------------------------\r\n");
      }
      else if (bufferKey3digit[0] == 0x80 && bufferKey3digit[1] == 0x00 && bufferKey3digit[2] == 0x00) //enter
      {
        // check key enter
        Line = atoi(line);
        // printf("line:%d, NumLength:%d\r\n", Line, NumLength);
        if (Line > maxLine)
        {
          stringToUnicodeAndSendToDisplay("Over Max line !!");
          beep5(); //error
          delay_ms(1000);
          state = 1;
          memset(line, 0, 5); //set default value
          curline = 0;
        }
        else
        {
          state = 0; //exit
        }
      }
      else if (bufferKey3digit[0] == 0x40 && bufferKey3digit[1] == 0x00 && bufferKey3digit[2] == 0x00)
      {
        //remove
        if (curline > 0)
          curline--;
        line[curline] = 0x00;
      }
      i = unicode_to_ASCII(bufferKey3digit[0]); // convert key to ascii
      if (i >= 48 && i <= 57)                   //ถ้าเป็นเลข
      {
        // check num 0 - 9
        if (curline < NumLength)
        {
          line[curline] = i;
          // if (curline = 2)
          curline++;
          if (curline >= 5)
            line[curline] = 0x00;
        }
        else
        {
          line[curline] = 0x00;
        }
        //printf("this is line : %s\r\n", line);
      }
      //sprintf(str, "%d", someInt);
      strcpy(mainText, "Go to Line:");
      strcat(mainText, line);
      strcat(mainText, " /");
      strcat(mainText, numbuffer);
      if (!toggleCur)
        stringToUnicodeAndSendToDisplay(mainText);
      else
        stringToUnicodeAndSendToDisplayC(mainText, curline + 11);
      printf("\r\n gotoLine_EnterLine : %s/%d\r\n", line, maxLine);
      //rintf("------------max line %d----------\r\n", );

      clearKeyValue();
    }
    d_Time++;
    if (d_Time >= delayCur)
    {
      //blink cu
      if (toggleCur == 0)
        toggleCur = 1;
      else
        toggleCur = 0;
      if (!toggleCur)
        stringToUnicodeAndSendToDisplay(mainText);
      else
        stringToUnicodeAndSendToDisplayC(mainText, curline + 11);
      d_Time = 0;
    }
  }
  if (Line == (-1))
    clearDot();
  return Line;
}
//---------------------------- end fn. go to line ---------------------------
void testCell()
{
  int i_ = 0;
  int dl = 0;
  int step_testing = 1;
  int countTime = 0;
  int sw_blink = 0;
  testing = 1;
  clearDot();
  printDot(test_step1, sizeof(test_step1));
  while (testing == 1)
  {
    if (USART_GetITStatus(USART2, USART_IT_RXNE))
    {
      //----------------------------- uart to key--------------------------------
      uart2Buffer = USART_ReceiveData(USART2); //-
      if (uart2Buffer == 0xff && SeeHead == 0)
      {
        //-
        SeeHead = 1;  //-
        countKey = 0; //-
      }

      if (countKey >= 4 && countKey <= 6)
      {
        //-
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
        {
          //check error key
          //printf("Key Error");
          countKey = 0;
          SeeHead = 0;
        }
        //keyCode = keyMapping(bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
        printf("keyCode %d %d %d \r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
        if (bufferKey3digit[0] != 0 || bufferKey3digit[1] != 0 || bufferKey3digit[2] != 0) //enter
        {
          step_testing++;
          if (step_testing >= 6)
          {
            testing = 0;
          }
          delay_ms(10);
        }
        countKey = 0;
        keyCode = 0;
        seeCur = 0;
        if (step_testing == 2)
        {
          printDot(test_step2, sizeof(test_step2));
        }
        else if (step_testing == 3)
        {
          printDot(test_step3, sizeof(test_step3));
        }
        else if (step_testing == 4)
        {
          printDot(test_step4, sizeof(test_step4));
        }
        else if (step_testing == 5)
        {
          printDot(test_step5, sizeof(test_step5));
        }
      }
    }
  }
  /*for (i_ = 0; i_ < 20; i_++)
  {
    SPI_DISPLAY_CS_LOW();
    cell_sentdata(~0xff);
    SPI_DISPLAY_CS_HIGH();
    dl++;
    delay_ms(200);
  }*/

  clearDot();
  delay_ms(200);
  printDot(st_0, sizeof(st_0));
  delay_ms(500);
  clearDot();
}
void displayPrepare()
{
  printDot(st_0, sizeof(st_0));
  delay_ms(1200);
  stringToUnicodeAndSendToDisplay("notepad");
}
void mode4()
{
  int status____ = 0;
  char stringBu[10];
  char buff22__[4];
  notepad_readKey();
  if (countKey >= maxData)
  {
    seeHead = 0;
    printf("See key %x,%x,%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
    if (checkKeyError == 0xff) // error catch
    {
      countKey = 0;
      SeeHead = 0;
    }
    keyCode = keyMapping(bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
    printf("keycode:%d\r\n", keyCode);
    if (!selectMode4) //0
    {
      switch (keyCode)
      {
      case 40:                               //down
        if (countMenuInMode4 < maxMenu4 - 1) //0-1
          countMenuInMode4++;
        break;
      case 38: //up
        if (countMenuInMode4 > 0)
          countMenuInMode4--;
        break;
      case 39: //enter
        selectMode4 = true;
        printf("Enter mode (%d)\r\n", countMenuInMode4);
        break;
      case 37: //exit
        printf("exit to mode (4)\r\n");
        stringToUnicodeAndSendToDisplay("Tools");
        mode = 0;
        selectMode4 = false;
        break;
      }
    }
    if (selectMode4) //1
    {
      if (keyCode == 37)
      {
        selectMode4 = false;
      }
      switch (countMenuInMode4)
      {
      case 0:
        memset(stringBu, 0, sizeof(stringBu));
        memset(buff22__, 0, sizeof(buff22__));
        strcpy(stringBu, "Battery ");
        sprintf(buff22__, "%d", getBatterry());
        strcat(stringBu, buff22__);
        strcat(stringBu, "%");
        stringToUnicodeAndSendToDisplay(stringBu);
        break;
      case 1:
        memset(stringBu, 0, sizeof(stringBu));
        strcpy(stringBu, "Sound:");

        //stringToUnicodeAndSendToDisplay("Beep:");
        status____ = getMuteStatus();
        if (status____ != 1 || status____ != 0)
        {
          status____ = getMuteStatus();
        }
        if ((bufferKey3digit[0] == 0 && bufferKey3digit[1] == 0 && bufferKey3digit[2] == 2) || (bufferKey3digit[0] == 0 && bufferKey3digit[1] == 0x10 && bufferKey3digit[2] == 0))
        {
          if (status____ == 1)
          {
            sendCommandtoAtmega(148);
            delay_ms(300);
            //printf("Send mute\r\n");
          }
          else
          {
            sendCommandtoAtmega(149);
            delay_ms(200);
            //printf("Send mute\r\n");
          }
        }
        status____ = getMuteStatus();

        if (status____ == 1)
        {
          strcat(stringBu, "On");
        }
        else
        {
          strcat(stringBu, "Off");
        }
        stringToUnicodeAndSendToDisplay(stringBu);
        //printf("status:%d\r\n", status____);
        break;
      case 2:
        testCell();
        selectMode4 = 0;
        break;
      }
    }
    if (selectMode4 == 0 && mode != 0)
      mainMenuDisplayMode4(countMenuInMode4);
    clearKeyValue();
  }
}
void mainMenuDisplayMode4(int numb)
{
  switch (numb)
  {
  case 0:
    stringToUnicodeAndSendToDisplay("Battery level");
    break;
  case 1:
    stringToUnicodeAndSendToDisplay("Sound Setting");
    break;
  case 2:
    stringToUnicodeAndSendToDisplay("Test cell");
    break;
  default:
    stringToUnicodeAndSendToDisplay("Battery level");
    break;
  }
}
//
//--- อ่านชื่อบูลทูธ ส่งกลับไปยังสตริง inCC
//
char *getBluetoothName()
{
  char *pp;
  int onS = 1, inCC = 0;
  int timeOut = 0;
  pp = malloc(20 + 1);
  if (pp == NULL)
  {
    printf("Unable to allocate memory.\n");
    // exit(1);
  }
  while (onS)
  {
    timeOut++;
    if (sendStatus)
    {
      printf("Send command\r\n");
      SendCommandToBLE(atName, sizeof(atName));
      sendStatus = 0;
    }
    if (USART_GetITStatus(UART4, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(UART4);
      if (inCC >= 8)
      {
        *(pp + inCC - 8) = i1;
      }
      inCC++;
      //printf("%c", i1);
    }
    if (inCC > 27)
    {
      sendStatus = 1;
      break;
    }
    if (timeOut > 300000)
    {
      sendStatus = 1;

      //
      printf("timeOut\r\n");
      break;
    }
  }
  return pp;
}
void queryLine(int line)
{
  int a, cc = 0, a2 = 0;
  int length__ = 0;
  length__ = strlen(SST25_buffer);
  memset(bufferQueryLine, 0, sizeof(bufferQueryLine));
  for (a = 0; a < length__; a++)
  {
    if (SST25_buffer[a] == '\n')
    {
      cc++;
      a++;
    }
    if (cc > line) //ถ้าเกิน
    {
      break;
    }
    if (cc == line)
    {
      if (SST25_buffer[a] != 0x0d)
      {
        if (a2 == 0 && SST25_buffer[a] < 32) //error
        {
          a++;
        }
        bufferQueryLine[a2++] = SST25_buffer[a];
      }
    }
  }
  // printf("a is %d\r\n", a);
}
/*
// sliding text after read text to rom
▒▒▒▒▒▒▒█▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀█
▒▒▒▒▒▒▒█░▒▒▒▒▒▒▒▓▒▒▓▒▒▒▒▒▒▒░█
▒▒▒▒▒▒▒█░▒▒▓▒▒▒▒▒▒▒▒▒▄▄▒▓▒▒░█░▄▄
▒▒▄▀▀▄▄█░▒▒▒▒▒▒▓▒▒▒▒█░░▀▄▄▄▄▄▀░░█
▒▒█░░░░█░▒▒▒▒▒▒▒▒▒▒▒█░░░░░░░░░░░█
▒▒▒▀▀▄▄█░▒▒▒▒▓▒▒▒▓▒█░░░█▒░░░░█▒░░█
▒▒▒▒▒▒▒█░▒▓▒▒▒▒▓▒▒▒█░░░░░░░▀░░░░░█
▒▒▒▒▒▄▄█░▒▒▒▓▒▒▒▒▒▒▒█░░█▄▄█▄▄█░░█
▒▒▒▒█░░░█▄▄▄▄▄▄▄▄▄▄█░█▄▄▄▄▄▄▄▄▄█
▒▒▒▒█▄▄█░░█▄▄█░░░░░░█▄▄█░░█▄▄█
*/

void QueryLineWithCommand(int line)
{
  int gg = 0;
  int ccLine = 0;
  while (ccLine < line)
  {
    ccLine += read.lineInsector[gg]; // นับบรรทัด
    if (ccLine > line)
    {
      read.currentSector = gg;
    }
    gg++;
  }
  /*{
      if (read.currentLine < read.lineInsector[read.currentSector])
      {
        read.currentLine++;
      }
      if (read.currentLine == read.lineInsector[read.currentSector] && read.currentSector < read.TotalSector)
      {
        queryLine(read.currentLine);
        strcpy(read.strTemp, bufferQueryLine);
        read.currentSector++;
        readSecter(read.currentSector * sector);
        read.currentLine = 0;
        queryLine(read.currentLine);
        strcat(read.strTemp, bufferQueryLine);
        printf("=========================================\r\n");
        printf("next sector \r\n");
        printf("=========================================\r\n");
      }
    }*/
}
/*-----------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
ฟังก์ชั่นสำหรับแสดงผลไฟล์ที่อ่านลงรอม โดยฟังก์ชั่นนี้ทำหน้าที่อ่านข้อมูลจาก Rom มาแสดงที่ Display ทีละ sector
-------------------------------------------------------------------------------------*/
void slidText2Displayv2()
{
  //clearUnsignChar();

  if (keyCode == SHORTCUT_KEY_GOTO_LINE) // function go to line
  {
    //bool loopT = true;
    // printf("go to line 98 99 \r\n");
    gt_Line = gotoLine_EnterLine(read.totalLine); // รับค่าบรรทัดที่จะไป
                                                  //printf("return line is %d\r\n", gt_Line);

    if (gt_Line == 0)
    {
      beep2();
    }
    if (gt_Line > 0) //  != -1
    {
      beep4();
      gt_Sector = gotoLine_getSectorInline(gt_Line);  // หาว่าบรรทัดที่จะไปอยู่ Sector ไหน
      gt_Line = gotoLine_getLine(gt_Line, gt_Sector); // แล้วดูว่า Sector นั้นอยู่บรรทัดไหน
      read.currentSector = gt_Sector;                 // ทำการเปลี่ยน Sector ที่จะอ่าน
      read.currentLine = gt_Line;                     // ทำการเปลี่ยนบรรทัดไปยังบรรทัดนั้น
      readSecter(read.currentSector * sector);        // ทำการอ่าน Sector นั้น
      queryLine(read.currentLine);                    // ทำการอ่านไฟล์ในบรรทัดนั้น
      //printf("current page is (%d) next page at line (%d)\r\n", page_getCurrentPage(), gt_Line);
    }
    //printf("found Line in Sector (%d)\r\n", gt_Sector);
    //printf("In line %d\r\n", gt_Line);
  }
  else if (keyCode == SHORTCUT_KEY_NEXT_PAGE) // next page
  {
    gt_Line = page_findNextPage();
    printf("------------gt_Line :%d-------------------\r\n", gt_Line);

    if (gt_Line == 0)
    {
      beep2();
    }
    if (gt_Line > 0) //  != -1
    {
      beep4();
      gt_Sector = gotoLine_getSectorInline(gt_Line);  // หาว่าบรรทัดที่จะไปอยู่ Sector ไหน
      gt_Line = gotoLine_getLine(gt_Line, gt_Sector); // แล้วดูว่า Sector นั้นอยู่บรรทัดไหน
      read.currentSector = gt_Sector;                 // ทำการเปลี่ยน Sector ที่จะอ่าน
      read.currentLine = gt_Line;                     // ทำการเปลี่ยนบรรทัดไปยังบรรทัดนั้น
      readSecter(read.currentSector * sector);        // ทำการอ่าน Sector นั้น
      queryLine(read.currentLine);                    // ทำการอ่านไฟล์ในบรรทัดนั้น
      //printf("current page is (%d) next page at line (%d)\r\n", page_getCurrentPage(), gt_Line);
    }
  }
  else if (keyCode == SHORTCUT_KEY_PREVOIUS_PAGE) //previous page
  {
    // k = page_getCurrentPage();
    //printf("-------------------------k:%d----------------------- %d \r\n ", k);FFF
    //j = k - 1;
    gt_Line = pages.page[findPreviousPage()];
    i = get_CurrentLine();
    // printf("- current page is (%d) \r\n", page_getCurrentPage());
    printf("--------------------befor--------------------------------go to line %d \r\n ", gt_Line);
    // k = pages.page[page_getCurrentPage() - 1]; //105 k = true
    if (!i)
    {
      beep2();
    }
    //printf("-------go to %d -----------\r\n", gt_Line);
    if (gt_Line >= 0 && i) //  != -1
    {
      beep4();
      gt_Sector = gotoLine_getSectorInline(gt_Line);  // หาว่าบรรทัดที่จะไปอยู่ Sector ไหน
      gt_Line = gotoLine_getLine(gt_Line, gt_Sector); // แล้วดูว่า Sector นั้นอยู่บรรทัดไหน
      read.currentSector = gt_Sector;                 // ทำการเปลี่ยน Sector ที่จะอ่าน
      read.currentLine = gt_Line;                     // ทำการเปลี่ยนบรรทัดไปยังบรรทัดนั้น
      readSecter(read.currentSector * sector);        // ทำการอ่าน Sector นั้น
      queryLine(read.currentLine);                    // ทำการอ่านไฟล์ในบรรทัดนั้น
      //printf("current page is (%d) previous page at line (%d)\r\n", page_getCurrentPage(), gt_Line);
    }
  }
  else if (keyCode == 703) // test
  {
    i = get_CurrentLine();
    printf("-------------------------getCurrent Line:%d----------------------- \r\n ", i);
    printf("-------------------------get find previous page:%d---------------- \r\n ", pages.page[findPreviousPage()]);
    gt_Line = marker99;
    if (gt_Line >= 0) //  != -1
    {
      beep4();
      gt_Sector = gotoLine_getSectorInline(gt_Line);  // หาว่าบรรทัดที่จะไปอยู่ Sector ไหน
      gt_Line = gotoLine_getLine(gt_Line, gt_Sector); // แล้วดูว่า Sector นั้นอยู่บรรทัดไหน
      read.currentSector = gt_Sector;                 // ทำการเปลี่ยน Sector ที่จะอ่าน
      read.currentLine = gt_Line;                     // ทำการเปลี่ยนบรรทัดไปยังบรรทัดนั้น
      readSecter(read.currentSector * sector);        // ทำการอ่าน Sector นั้น
      queryLine(read.currentLine);                    // ทำการอ่านไฟล์ในบรรทัดนั้น
      //printf("current page is (%d) previous page at line (%d)\r\n", page_getCurrentPage(), gt_Line);
    }
    printf("==============================\r\n");
    printf("==============================\r\n");
    // printf("current page =  %d ,line in page %d\r\n", j, pages.page[j - 1]);

    /* printf("==============================\r\n");
    for (k = j; k >= 0; k--)
    {
      printf("page %d\r\n", pages.page[k]);
    }*/
    printf("==============================\r\n");
    printf("==============================\r\n");
    printf("==============================\r\n");
  }
  else if (keyCode == CLOSE_FILE) //Exit read Mode
  {
    printf("Exit\r\n");
    ROMR.endReadFile = false;
    readFileStatus___ = 0;
    /* SendCH370(ResetAll,sizeof(ResetAll));
        printf("resetall\r\n");*/
    keyCode = 0;

    maxFile = 0;
    for (i = 0; i < maxfileListBuffer; i++)
      memset(fileLists[i], 0, sizeof(filelist[i]));
    memset(Dirpath, 0, sizeof(Dirpath)); //clear path
    // delay_ms(5000);
    //prepareSD_Card();
    printf("reset all \r\n");
    mode = 0;
    ABCCCCC = 0;
    beep4();
    stringToUnicodeAndSendToDisplay("read");

    //mode = 0;
  }

  if (strlen(bufferQueryLine) <= 20) // ถ้้า text น้อยกว่า 20 char
    read.DisplayLine = 0;

  if (keyCode == 2) // printf right
  {
    if (strlen(bufferQueryLine) <= 20 || read.DisplayLine == 1)
    {
      keyCode = 40;         //if current digit at last go to next line
      read.DisplayLine = 0; //start at digit 0 - 19
    }
    else
    {
      read.DisplayLine = 1; // read digit 0-19
    }
  }
  else if (keyCode == 1) // printf left
  {
    if (read.DisplayLine == 0)
    {
      keyCode = 38;         //if current digit at last go to next line
      read.DisplayLine = 1; //start at digit 0 - 19
    }
    else if (strlen(bufferQueryLine) <= 20)
    {
      //read.DisplayLine = 0;
      keyCode = 38;
    }
    else
    {
      read.DisplayLine = 0;
    }
  }
  //======================================================================

  if (keyCode == 38) //ยพ
  {
    if (read.currentLine > 0)
    {
      read.currentLine--;
    }
    // อ่าน Sector ก่อนหน้า
    if (read.currentSector != 0 && read.currentLine == 0) //
    {
      queryLine(read.currentLine);
      strcpy(read.strTemp2, bufferQueryLine); // line 0
      read.currentSector--;
      readSecter(read.currentSector * sector);                  //
      read.currentLine = read.lineInsector[read.currentSector]; //last line in sector
      queryLine(read.currentLine);
      strcpy(read.strTemp, bufferQueryLine);
      strcat(read.strTemp, read.strTemp2);

      printf("=========================================\r\n");
      printf("prevoiuse sector \r\n");
      printf("=========================================\r\n");
    }
  }
  else if (keyCode == 40) // next text
  {
    if (read.currentLine < read.lineInsector[read.currentSector])
    {
      read.currentLine++;
    }
    /*=========================================================
         แก้รอยต่อระหว่าง sector ตัวอย่างเช่น
        sector 1: aaaaaaaaaaaaaaaaaaa
                   bbbbbbbbb
        sector 2: bbbbbbbbbb
                  ccccccccccccccccccc
        จะเห็นว่าบรรทัดสุด้ายของ Sector 1 ขาดไปอยู่ที่บรรทัดแรกของ Sector 2
        ส่วนนี้จะทำหน้าที่ต่อเข้ากัน โดนใช้ strTemp in struct และอ่าน sector ถีดไป
        =========================================================*/
    if (read.currentLine == read.lineInsector[read.currentSector] && read.currentSector < read.TotalSector)
    {
      queryLine(read.currentLine);
      strcpy(read.strTemp, bufferQueryLine);
      read.currentSector++;
      readSecter(read.currentSector * sector);
      read.MainCurrentLine += read.currentLine;
      read.currentLine = 0;
      queryLine(read.currentLine);
      strcat(read.strTemp, bufferQueryLine);
      printf("=========================================\r\n");
      printf("next sector \r\n");
      printf("=========================================\r\n");
    }
  }
  else if (keyCode == MARK_SET_MARK) // set mark 1-3-4
  {
    printf("Set mark\r\n");
    printf("keycode testing marker line (%d)\r\n", read.currentLine + 1); // แสดง บรรทัดปัจจุบัน
    printf("file name:%s\r\n", fileLists[fileSelect]);                    // แสดงชื่อไฟล์
    //printf("data:%s\r\n", SST25_buffer);
    memset(SST25_buffer, 0, strlen(SST25_buffer)); // เคลียค่าใน SST25_buffer
    readSecter(4096 * markSector);                 // อ่านค่าใน sector
    printf("data:%s\r\n", SST25_buffer);
    if (mark_CreateMarkFileConfig(fileLists[fileSelect])) //สร้างสำเร็จ
    {
      mark_insertLine(fileLists[fileSelect], read.currentLine + 1);
    }
    else // หรือมีไฟล์อยู่แล้ว
    {
      mark_insertLine(fileLists[fileSelect], read.currentLine + 1);
    }
    printf("data SST:%s\r\n", SST25_buffer); //แสดง mark
    writeFlash(4096 * markSector);           // ทดสอบ
    readSecter(read.currentSector * sector); //อ่านไฟล์บรรทัดกลับมา
    beep();
  }
  else if (keyCode == MARK_JUMP_TO_MARK)
  {
    // clear mark in rom 2-4-5
    memset(SST25_buffer, 0, strlen(SST25_buffer));
    printf("clear all\r\n");
    writeFlash(4096 * 999); //16777216
    memset(mark_MarkerPage, 0, sizeof(mark_MarkerPage));
    mark_currentPostion = 0;
    /*readSecter(read.currentSector * sector);
    read.MainCurrentLine += read.currentLine;
    read.currentLine = 0;
    queryLine(read.currentLine);*/
  }
  else if (keyCode == MARK_JUMP_NEXT) // mark next 624
  {

    // (1-4-5-8 + Space),2 4 5 8
    // mark_MarkerPage
    //  printf("file :%s\r\n",fileLists[fileSelect]);
    printf("next mark \r\n");
    mark_maxmarkMaxMax = mark_findListTotalInArray();
    //printf("----------------- list of marker --------------");
    if (mark_currentPostion < mark_maxmarkMaxMax)
    {                                                 // ยังไม่เกิน
      gt_Line = mark_MarkerPage[mark_currentPostion]; // กระโดดไปยังบรรทัดของ mark
      mark_currentPostion++;
      beep4();
      //gt_Line = mark_MarkerPage[0]; // edit hear
      printf("next mark (%d)\r\n", gt_Line);
      gt_Sector = gotoLine_getSectorInline(gt_Line);  // หาว่าบรรทัดที่จะไปอยู่ Sector ไหน
      gt_Line = gotoLine_getLine(gt_Line, gt_Sector); // แล้วดูว่า Sector นั้นอยู่บรรทัดไหน
      read.currentSector = gt_Sector;                 // ทำการเปลี่ยน Sector ที่จะอ่าน
      read.currentLine = gt_Line;                     // ทำการเปลี่ยนบรรทัดไปยังบรรทัดนั้น
      readSecter(read.currentSector * sector);        // ทำการอ่าน Sector นั้น
      queryLine(read.currentLine);                    // ทำการอ่านไฟล์ในบรรทัดนั้น
    }
  }
  else if (keyCode == MARK_JUMP_PREVOIUS) // mark previous 2 4 5 7
  {
    printf("previous mark\r\n");
    // mark_maxmarkMaxMax = mark_findListTotalInArray();
    if (mark_currentPostion > 0)
    {                                                 // ยังไม่เกิน
      gt_Line = mark_MarkerPage[mark_currentPostion]; // กระโดดไปยังบรรทัดของ mark
      // if (mark_currentPostion >= 0)                   // ไม่หมด
      mark_currentPostion--;
      beep4();
      printf("prevoius mark (%d)\r\n", gt_Line);
      gt_Sector = gotoLine_getSectorInline(gt_Line);  // หาว่าบรรทัดที่จะไปอยู่ Sector ไหน
      gt_Line = gotoLine_getLine(gt_Line, gt_Sector); // แล้วดูว่า Sector นั้นอยู่บรรทัดไหน
      read.currentSector = gt_Sector;                 // ทำการเปลี่ยน Sector ที่จะอ่าน
      read.currentLine = gt_Line;                     // ทำการเปลี่ยนบรรทัดไปยังบรรทัดนั้น
      readSecter(read.currentSector * sector);        // ทำการอ่าน Sector นั้น
      queryLine(read.currentLine);                    // ทำการอ่านไฟล์ในบรรทัดนั้น
    }
  }
  else if (keyCode == MARK_DELETE_MARK) // 1-4-5-8 + Space
  {
    if (mark_RemoveArrayAtIndex(mark_findIndexOfArray(read.currentLine))) // ลบ ตำแหน่ง mark ออกจาก mark_MarkerPage และทำการเลื่อนข้อมูลเข้า
    {
      mark_sortPageAndStore();
      for (k = 0; mark_MarkerPage[k] != NULL; k++)
        printf("page->%d\r\n", mark_MarkerPage[k]); // แสดงค่า page ของ mark
      // ลบข้อมูลใน mark_MarkerPage
    }
  }
  printf(" mark position (%d) page: (%d)\r\n", mark_currentPostion, mark_MarkerPage[mark_currentPostion]);
  // printf("keycode %d\r\n",keyCode);
  if (strlen(read.strTemp) != 0)
  {
    printf("string:%s\r\n", read.strTemp);
    printStringLR(read.strTemp, read.DisplayLine);
    memset(read.strTemp, '\0', sizeof(read.strTemp));
    memset(read.strTemp2, '\0', sizeof(read.strTemp2));
  }
  else
  {
    queryLine(read.currentLine);
    printStringLR(bufferQueryLine, read.DisplayLine);
    /*for (i = 0; i < sizeof(bufferQueryLine) / 2; i++)
        {
          printf("0x%x,", bufferQueryLine[i]);
      }*/
    printf("Line (%d) :%s\r\n", read.currentLine, bufferQueryLine);
  }
  printf("sector:(%d) total Sector (%d)\r\n", read.currentSector, read.TotalSector);
}
// เก็บค่า 0x0c เป็นตัวแบ่งหน้าในไฟล์ต่างๆ โดยจะชี้ไปที่บรรทัดนั้นๆ
void StoreLine()
{
  for (i = 0; i <= read.TotalSector; i++)
  {
    readSecter(i * sector);                                                // *4096
    read.lineInsector[i] = readmode_countLineInOneSector(SST25_buffer, i); // เก็บบรรทัดที่นับได้จาก ROM
    //StorePage(i);
    // printf("Line (%d) : %d\r\n",i,read.lineInsector[i]);

    read.totalLine += read.lineInsector[i]; // เก็บทรรทัดทั้งหมด
  }
}
void StoreLineClear()
{
  for (i = 0; i < sizeof(read.lineInsector) / sizeof(uint16_t); i++)
  {
    read.lineInsector[i] = '\0';
  }
}
// อ่านค่าจาก Sector โดยการใส่ตำแหน่ง sector *4096
// จะเก็บค่าไว้ที่ตัวแปร SST25_buffer
void readSecter(int sector_)
{
  configFlash();
  SPI_FLASH_CS_LOW();
  SST25_R_BLOCK(sector_, SST25_buffer, sector); // secter = 4096
  SPI_FLASH_CS_HIGH();
  Delay(0xffff);
}
void convert_text2_buffer(char *str)
{
  int ccInLine = 0;
  int lineCC = 0;
  while (str[ccMain] != '\0')
  {
    if (lineCC >= readmode_MaxLineBuffer)
    {
      read_mode_currentIndex_after = ccMain;
      break;
    }
    if (str[ccMain] != '\r')
    {
      if (ccInLine >= readmode_maxsizeInLine - 1)
      {
        //  printf("=:%c\r\n",str[ccMain]);
        lineCC++;
        ccInLine = 0;
        readmode_bufferStr[lineCC][ccInLine] = str[ccMain];
        ccInLine++;
      }
      else
      {
        readmode_bufferStr[lineCC][ccInLine] = str[ccMain];
        ccInLine++;
      }
    }
    else
    {
      lineCC++;
      ccInLine = 0;
      ccMain++;
    }
    ccMain++; // jump \r\n
  }
}
//--------------------------------------------------
// เตรียมสตริง สำหรับเขียนลง SD-card
// โดยลบเครื่องหมาย Enter (Enter sign) และเติม \r\n
////////////////////////////////////////////////////
/*char *regexStrEnter(char *str)
{
  int cc = 0;
  char buffer_ln[2];
  char *pp;
  strcpy(buffer_ln, "\r\n");
  while (cc < notepad_MaxinLine)
  {
    if (str[cc] != enterSign)
      cc++;
    else
      break;
  }
  *pp = *substring(str, 1, cc);
  return pp;
}*/
void substringRemoveEnter2(char *string)
{
  int c;
  int cc = 0;
  int length;
  memset(buffer_afterRemove, 0, 42);
  while (cc < notepad_MaxinLine)
  {
    if (string[cc] != enterSign)
      cc++;
    else
      break;
  }
  length = cc;
  for (c = 0; c < length; c++)
  {
    buffer_afterRemove[c] = *(string);
    string++;
  }
  buffer_afterRemove[c] = '\0';
}
/*
char *substringRemoveEnter(char *string, int position, int length)
{
  char *pointer;
  int c;
  int cc = 0;
  while (cc < notepad_MaxinLine) 
  {
    if (string[cc] != enterSign)
      cc++;
    else
      break;
  }
  length = cc;
  pointer = malloc(length + 1);

  if (pointer == NULL)
  {
    printf("Unable to allocate memory.\n");
    // exit(1);
  }
  for (c = 0; c < length; c++)
  {
    *(pointer + c) = *(string + position - 1);
    string++;
  }
  *(pointer + c) = '\0';
  return pointer;
}
*/
int str_cut(char *str, int begin, int len)
{
  int l = strlen(str);

  if (len < 0)
    len = l - begin;
  if (begin + len > l)
    len = l - begin;
  memmove(str + begin, str + begin + len, l - len + 1);
  return len;
}
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
void prepareSD_Card()
{
  preStatus = 1;
  command_ = 1;
  while (preStatus)
  {

    if (command_ == 1)
    {
      SendCH370(setSDCard, sizeof(setSDCard));
      if (debug)
        printf("Set sd card\r\n");
      command_++; //4
      delay_ms(45);
    }
    else if (command_ == 2)
    {
      SendCH370(DiskConnect, sizeof(DiskConnect));
      if (debug)
        printf("Check connecttion \r\n");
      command_++; //2
      delay_ms(45);
    }
    else if (command_ == 3)
    {
      SendCH370(USBDiskMount, sizeof(USBDiskMount));
      if (debug)
        printf("Usb disk mount \r\n");
      command_++; //6
      delay_ms(45);
      preStatus = 0;
    }
    /* else if (command_ == 4)
    {
      SendCH370(setAllName, sizeof(setAllName));
      if (debug)
        printf("Fet all file \r\n");
      command_++; //10
      delay_ms(45);
    }
    else if (command_ == 5)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      if (debug)
        printf("File open \r\n");
      command_++; //10
      delay_ms(45);
    }
    else if (command_ == 6)
    {
      SendCH370(FileClose0, sizeof(FileClose0));
      //printf(" File Close \r\n");
      command_++; //10
      delay_ms(45);
      preStatus = 0;
    }*/
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      i1 = USART_ReceiveData(USART3);
      if (debug)
        printf("Recieve:%x\r\n", i1);
    }
  }
  if (debug)
    printf("End prepare\r\n");
}
void beepError()
{
  sendCommandtoAtmega(155);
}
void sendCommandtoAtmega(int data)
{
  USART_SendData(USART2, data);
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {
  }
}
void beep()
{
  USART_SendData(USART2, 154);
  delay_ms(45);
}
void beep2()
{
  USART_SendData(USART2, 155);
  delay_ms(45);
}
void beep3()
{
  USART_SendData(USART2, 156);
  delay_ms(45);
}
void beep4()
{
  USART_SendData(USART2, 157);
  delay_ms(45);
}
void beep5()
{
  USART_SendData(USART2, 158);
  delay_ms(45);
}
void errorBreak()
{
  USART_SendData(USART2, 151);
  delay_ms(45);
}
uint8_t getBatterry()
{
  USART_SendData(USART2, 150);
  delay_ms(45);
  if (USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    uart2Buffer = USART_ReceiveData(USART2);
  }
  return uart2Buffer;
}
int getMuteStatus()
{
  USART_SendData(USART2, 147);
  delay_ms(45);
  if (USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    uart2Buffer = USART_ReceiveData(USART2);
  }
  return uart2Buffer;
}
/*
ฟังก์ชั่น Notepad สำหรับพิมพ์
*/
void notepad_main()
{
  char nameBuff2[15];
  clearDot(); //clear screen
  //status for do something n notepad mode
  while (doing) // do in notepad notepad0
  {
    notepad_readKey();       // key recieve
    if (countKey >= maxData) // do events
    {
      // Recieve & checking key
      seeHead = 0;
      // printf("See key %x,%x,%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
      if (checkKeyError == 0xff) // ดักจับเออเร่อ
      {
        countKey = 0;
        SeeHead = 0;
      }
      keyCode = keyMapping(bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]); // รับค่า key
      printf("========kyCode %d===========\r\n", keyCode);
      if ((bufferKey3digit[1] > 3 || bufferKey3digit[2] != 0) && seeCur != 1) // key control ไม่ใช่  cursor
      {
        //--- เลื่อนบรรทัด ----
        notepad_checkMaxLine();
        if (keyCode == 40)
        {
          // next line
          if (Notepad.currentLine < notepad_Line && Notepad.currentLine < maxLineN)
            Notepad.currentLine++;
          if (notepad_countLinewithOutLNsign(Notepad.buffer_string[Notepad.currentLine]) < notepad_MaxinLine / 2) //
            Notepad.displayFirst = false;
          Notepad.cursorPosition = notepad_getnullPostion(Notepad.buffer_string[Notepad.currentLine]);
          // บังคับห้ามมีค่าเกิน max
          if (Notepad.cursorPosition >= notepad_MaxinLine)
            Notepad.cursorPosition = notepad_MaxinLine;
          k = notepad_countLinewithOutLNsign(Notepad.buffer_string[Notepad.currentLine]); //ไม่นับ enter
          if (Notepad.cursorPosition > k)
            Notepad.cursorPosition = k;
          Notepad.multiplyCursor = 0;
          // printf("cursor position at :%d \r\n", Notepad.cursorPosition);
        }
        else if (keyCode == 38)
        {
          // previous line
          if (Notepad.currentLine > 0)
            Notepad.currentLine--;
          if (notepad_countLinewithOutLNsign(Notepad.buffer_string[Notepad.currentLine]) < notepad_MaxinLine / 2)
            Notepad.displayFirst = false;
          Notepad.cursorPosition = notepad_getnullPostion(Notepad.buffer_string[Notepad.currentLine]);
          if (Notepad.cursorPosition >= notepad_MaxinLine)
            Notepad.cursorPosition = notepad_MaxinLine;
          k = notepad_countLinewithOutLNsign(Notepad.buffer_string[Notepad.currentLine]); //ไม่นับ enter
          if (Notepad.cursorPosition > k)
            Notepad.cursorPosition = k;
          Notepad.multiplyCursor = 0;
          //  printf("cursor position  k at :%d \r\n", Notepad.cursorPosition);
        }
        //----------------------------------------------------------------------------------------
        if (keyCode == 1)
        {
          // left
          Notepad.displayFirst = false;
          k = 20;                         // ไม่นับถึง enter
          if (Notepad.cursorPosition > k) //กดได้ไม่เกิน enter
            Notepad.cursorPosition = k;
          else if (Notepad.cursorPosition > strlen(Notepad.buffer_string[Notepad.currentLine]))
            Notepad.cursorPosition = strlen(Notepad.buffer_string[Notepad.currentLine]);
          Notepad.multiplyCursor = 0;
        }
        else if (keyCode == 2)
        {
          // right
          if (notepad_countLinewithOutLNsign(Notepad.buffer_string[Notepad.currentLine]) > 20)
            Notepad.displayFirst = true;
        }
      }                                      // จบส่วนของ key control
      else if (keyCode == NOTEPAD_SAVE_FILE) // save key (space + s) // save file
      {
        // save
        // บันทึกไฟล์ หลังจากทำการพิมพ์ในโหมด Notepad
        // หน้าจอ Braille Display จะแสดง Name:_
        // ไฟล์ที่ได้จะเป็นไฟล์ .TBT
        stringToUnicodeAndSendToDisplay("Saving..");
        if (debug)
          printf("\r\n-----------Save:------------\r\n");

        if (useFEdit) //มาจาก edit file
        {
          //saveNameEdit();
          //ex_cdWithPath(Dirpath); // go to curretn directorie
          strcpy(nameBuff2, fileLists[fileSelect]); // ตั้งชื่อไฟล์ไว้ที่ตัวแปร nameBuff2
          printf("Saving file name %s\r\n", fileLists[fileSelect]);
          createFileAndWrite_hisPath(nameBuff2);
          printf("-----------------Deleted file and save -----------------\r\n");
        }
        else // มาจาก notepad
        {
          saveName();
        }
      }
      // exit (space + e)
      else if (keyCode == CLOSE_FILE)
      {
        // ออกจากโหมด notepad โดยจะยังไม่เคลียค่าที่พิมพ์
        // clear data after exit
        Notepad.cursorPosition = 0;
        Notepad.currentLine = 0;
        Notepad.displayFirst = false;
        Notepad.multiplyCursor = 0;
        printf("size of max line %d");
        printf("exit \r\n");
        for (i = 0; i < notepad_Line; i++)
        {
          memset(Notepad.buffer_string[i], 0, sizeof(Notepad.buffer_string[i])); // clear data in line ;
        }
        //SendCH370(ResetAll,sizeof(ResetAll));[]

        if (useFEdit) //มาจาก edit file
        {
          useFEdit = 0;
          memset(Dirpath, 0, sizeof(Dirpath));

          ROMR.endReadFile = false;
          readFileStatus___ = 0;
          /* SendCH370(ResetAll,sizeof(ResetAll));
        printf("resetall\r\n");*/
          keyCode = 0;

          maxFile = 0;
          for (i = 0; i < maxfileListBuffer; i++)
            memset(fileLists[i], 0, sizeof(filelist[i]));
          memset(Dirpath, 0, sizeof(Dirpath)); //clear path
          // delay_ms(5000);
          //prepareSD_Card();
          printf("reset all \r\n");
          mode = 0;
          ABCCCCC = 0;
          beep4();
        }
        mode = 0;
        doing = 0; // ออกจาก notepad
      }

      else if (bufferKey3digit[0] == 0x80 && bufferKey3digit[1] == 0 && bufferKey3digit[2] == 0 && seeCur != 1) // enter
      {
        //enter key                                                                                                       // printf("New line \r\n");
        if (Notepad.cursorPosition + Notepad.multiplyCursor < notepad_MaxinLine)
        {
          while (Notepad.cursorPosition + Notepad.multiplyCursor < notepad_MaxinLine) //40 ตัวอักษร
          {
            notepad_append(Notepad.buffer_string[Notepad.currentLine], buffAs, Notepad.cursorPosition + Notepad.multiplyCursor);
            if (Notepad.cursorPosition + Notepad.multiplyCursor >= notepad_MaxinLine) //defualt 40 charactor
            {
              // new line
              Notepad.currentLine++;
              Notepad.cursorPosition = 1;
            }
            else
            {
              Notepad.cursorPosition++;
              if (Notepad.cursorPosition + Notepad.multiplyCursor == notepad_MaxinLine) // max size in line
              {
                Notepad.cursorPosition = 0;
                Notepad.currentLine++;
                break;
              }
            }
          }
          // Notepad.currentLine++;
        }
      }
      else if (bufferKey3digit[0] == 0x40 && bufferKey3digit[1] == 0 && bufferKey3digit[2] == 0 && seeCur != 1)
      {
        //remove str at index
        //Notepad.cursorPosition = notepad_getnullPostion(Notepad.buffer_string[Notepad.currentLine]);
        if (notepad_lineIsEnter(Notepad.buffer_string[Notepad.currentLine]) == 1)
        {
          Notepad.cursorPosition = notepad_MaxinLine;
          if (debug)
            printf("---------------line is enter all -----------------");
        }
        if (debug)
          printf("cca aateaasd at :%d\r\n", Notepad.cursorPosition + Notepad.multiplyCursor);

        if (notepad_countStr(Notepad.buffer_string[Notepad.currentLine]) < notepad_MaxinLine / 2) //กันบัค cursor
          Notepad.multiplyCursor = 0;
        //  printf("remove char %d %c\r\n", Notepad.cursorPosition, Notepad.buffer_string[Notepad.currentLine][Notepad.cursorPosition + Notepad.multiplyCursor]);
        if (Notepad.cursorPosition > 0) //มากกว่า 0
        {
          Notepad.cursorPosition--;
        }
        else if (Notepad.currentLine > 0) // เป็น 0 และไม่ใช่บรรทัด 0
        {
          Notepad.cursorPosition = notepad_MaxinLine - 1;
          Notepad.currentLine--;
        }
        //------------ถ้าเจอ enter ลบจนกว่าจะหมดไปใน line----------------------
        if (Notepad.buffer_string[Notepad.currentLine][Notepad.cursorPosition] == enterSign) //ลบ enter
        {
          if (notepad_lineIsEnter(Notepad.buffer_string[Notepad.currentLine]) == 1)
          {
            if (debug)
              printf("---------------line is enter all -----------------");
          }
          // printf("\r\n--------------------remove enter------------------------\r\n");
          while (Notepad.buffer_string[Notepad.currentLine][Notepad.cursorPosition] == enterSign && Notepad.cursorPosition >= 0) //ยังเจอสัญลักษณ์ Enter จะลบไปจนถึง 0
          {
            removeChar(Notepad.buffer_string[Notepad.currentLine], Notepad.cursorPosition);
            if (Notepad.cursorPosition > 0)
              Notepad.cursorPosition--;
            if (Notepad.buffer_string[Notepad.currentLine][Notepad.cursorPosition] != enterSign) //ห้ามลบตัวอักษร
              break;
          }
        }
        else
        {
          removeChar(Notepad.buffer_string[Notepad.currentLine], Notepad.cursorPosition + Notepad.multiplyCursor); //ลบตัวอักษร
        }
        notepad_checkenterAndpush(Notepad.buffer_string[Notepad.currentLine]); //-----บัค
      }
      // ถ้าคีย์ที่กดเป็น router keys
      else if (seeCur == 1) // cursor key
      {
        Notepad.cursorPosition = mapCursor(bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
        k = notepad_countLinewithOutLNsign(Notepad.buffer_string[Notepad.currentLine]); // ไม่นับถึง enter
        if (Notepad.cursorPosition > k)                                                 //กดได้ไม่เกิน enter
          Notepad.cursorPosition = k;
        else if (Notepad.cursorPosition > strlen(Notepad.buffer_string[Notepad.currentLine]))
          Notepad.cursorPosition = strlen(Notepad.buffer_string[Notepad.currentLine]);

        //printf("set at------------------------%d ----k:%d---------------------\r\n", Notepad.cursorPosition, k);
        if (Notepad.displayFirst == true)
        {
          //กำหนด cursor ตำแหน่ง 20-40
          Notepad.multiplyCursor = 20;
          if (Notepad.cursorPosition + Notepad.multiplyCursor > k)
            Notepad.cursorPosition = k - Notepad.multiplyCursor;
          // printf("set at------------------------%d ----k:%d---------------------\r\n", Notepad.cursorPosition, k);
        }
        else
        {
          Notepad.multiplyCursor = 0;
        }
        //   printf("cursor set at:%d\r\n", Notepad.cursorPosition + Notepad.multiplyCursor);
      }                     // end router keys
      else if (seeCur != 1) // keyที่รับมา ไม่ใช่ cursor key
      {
        // เก็บข้อความที่พิมพ์ตรงนี้
        // 3508 text editor
        printf("dec-> buf1(%d) buf2(%d) buf3(%d)\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
        keyCode = unicode_to_ASCII(bufferKey3digit[0]); // แปลง keyboard เป็น code

        printf("buffer Key 0x%x key code 0x%x\r\n", bufferKey3digit[0], keyCode);
        /*
          key 5-6 ได้จุด 2
          keybuffer input - > (dec:48, Hex:30)
        */
        if (keyCode > 127)
          keyCode = ~bufferKey3digit[0]; //One's Complement
        if (keyCode == 0)                //space bar edit in unicode table
          keyCode = 32;
        if (notepad_checkEnterSignInLine(Notepad.buffer_string[Notepad.currentLine]) == 1) //เ ช็คเครื่องหมาย enter
        {
          notepad_removeEnterSign(Notepad.buffer_string[Notepad.currentLine]); // ลบเครื่องหมาย Enter ออก
        }
        keybuff[0] = (char)keyCode; //แปลง Char เป็นสตริงยาว 1  Ex. [?],[\0]
        // printf("current po at %d \r\n", Notepad.cursorPosition);
        // เครื่องหมาย enterตัวสุดท้ายจะถูกเบียดลง จะเกิน max แต่จะไม่ให้มันเบียด
        // printf("///////////////////Notepad.cursorPosition:%d Notepad.multiplyCursor:%d //////////////////\r\n", Notepad.cursorPosition, Notepad.multiplyCursor);
        notepad_append(Notepad.buffer_string[Notepad.currentLine], keybuff, Notepad.cursorPosition + Notepad.multiplyCursor);

        if (Notepad.cursorPosition >= notepad_MaxinLine) //เลื่อนบรรทัดอัตโนมัติ
        {
          //เลื่อนบรรทัดอัตโนมัติ
          Notepad.currentLine++;
          Notepad.cursorPosition = 1;
        }
        else
        {
          Notepad.cursorPosition++;

          if (Notepad.cursorPosition == notepad_MaxinLine / 2) //เลื่อนชุดเซลล์อัตโนมัติ
            Notepad.displayFirst = true;
        }
      } //end key input -----------------------------------------------------------------------------------------------------
      notepad_checkMaxLine();
      if (notepad_checkEnterSignInLine(Notepad.buffer_string[Notepad.currentLine]) == 1)
      {
        notepad_fillEnterSign(Notepad.buffer_string[Notepad.currentLine]); //--เติม enter ถ้าไม่เต็มบรรทัด
      }
      // เช็คความยาว ตัดมาแสดง ไม่เกิน 20 ตัว
      //
      subStringLanR(Notepad.buffer_string[Notepad.currentLine], Notepad.displayFirst, Notepad.cursorPosition + Notepad.multiplyCursor);
      if (debug)
      {
        printf("|////////////////////////////////////////////|\r\n");
        printStringInLine(Notepad.buffer_string[Notepad.currentLine]);
        printf("\r\n|=================sub========================|\r\n");
        subStringLanR(Notepad.buffer_string[Notepad.currentLine], Notepad.displayFirst, Notepad.cursorPosition);
        // }
        printf("\r\n|============================================|\r\n");
        printf("|current line (%d)", Notepad.currentLine);
        printf("\r\n|============================================|\r\n");
        printf("|current position index:(%d) and multi cur :(%d) \r\n", Notepad.cursorPosition, Notepad.multiplyCursor);
        printf("|////////////////////////////////////////////|\r\n");
      }
      clearKeyValue(); // clear key buffer
    }                  //end key event

    /*d_Time++;
    //กระพริบ cursor
    if (d_Time >= delayCur) //blink cursor
    {
      //blink cu
      if (toggleCur == 0)
        toggleCur = 1;
      else
        toggleCur = 0;

      // cursor กระพริบ
      if (!toggleCur)
      {
        // left
        subStringLanR(Notepad.buffer_string[Notepad.currentLine], Notepad.displayFirst, 99);
      }
      else
      {
        subStringLanR(Notepad.buffer_string[Notepad.currentLine], Notepad.displayFirst, Notepad.cursorPosition + Notepad.multiplyCursor);
      }
      d_Time = 0;
    } // end blink cursor
    */

    //กดออก
    if (doing == 0 && mode == 0) // after exit form notepad mode display 'notepad'
    {
      // หลังจากออกจากโหมด notepad โดยการกด key+e ให้แสดงข้อความ notepad
      stringToUnicodeAndSendToDisplay("notepad");
    }
  } //end while loop
}
//-------------------- maxline--------------------
//
//
////////////////////////////////////////////////////////////
int notepad_lineIsEnter(char *str)
{
  int ccm = 0, ccm2 = 0, ccm3 = 0;

  if (str[0] == enterSign)
    ccm = 1;

  if (str[notepad_MaxinLine - 1] == enterSign)
    ccm2 = 1;

  if (ccm == 1 && ccm2 == 1)
    ccm3 = 1;
  return ccm3;
}
void notepad_checkMaxLine()
{
  int a = 0;
  a = Notepad.currentLine;
  while (a < notepad_MaxinLine)
  {
    if (notepad_countStr(Notepad.buffer_string[a]) != 40)
    {
      break;
    }
    else
    {
      a++;
    }
  }
  maxLineN = a;
  //printf("$$$ff max in line %d\r\n", maxLineN);
}
int notepad_countStr(char *str)
{
  int aaa = 0;
  int cccccc = 0;
  while (aaa < notepad_MaxinLine)
  {
    if (str[aaa] != 0)
    {
      //printf("%c", str[aaa]);
      cccccc++;
    }
    aaa++;
  }
  //printf("max length in line :%d\r\n", cccccc);
  return cccccc;
}
//--------------------------------------------------------------
// เช็ค chr ตัวสุดท้าย
// ex.
//  ----------------------------------x
//  Checked[✔]
////////////////////////////////////////////////////////////////
void notepad_checkenterAndpush(char *str) //-------------------x
{
  int cc = 0;
  int las = 0;
  if (notepad_countStr(str) == notepad_MaxinLine)
  {
    while (cc < 40)
    {
      if (str[cc] != '\0')
      {
        if (str[cc] == enterSign)
        {
          las = cc;
        }
      }
      cc++;
    }
  }
  if (las != notepad_MaxinLine && las != 0 && notepad_countStr(str) == notepad_MaxinLine)
  {
    keybuff[0] = (char)enterSign;
    //printf("bug herre----------------- \r\n");
    notepad_append(str, keybuff, las);
  }
}
//test return ""
char *subStringLanR(char *str, int p, int cur__)
{
  char buff[20];
  int begin = 0;
  int end = 0;
  memset(buff, 0, strlen(buff));

  if (strlen(str) > 20)
  {
    //clear buff
    if (!p)
    {
      // split left
      begin = 0;
      end = 20;
    }
    else
    {
      // split right
      begin = 20;
      end = 40;
    }
    strncpy(buff, str + begin, end - begin);
    if (cur__ > 20 && cur__ != 99)
      cur__ -= 20;
    if (cur__ != 99)
      stringToUnicodeAndSendToDisplayC(buff, cur__);
    else
      stringToUnicodeAndSendToDisplay(buff);
    printf("|%s\r\n", buff);
  }
  else
  {
    if (cur__ != 99)
      stringToUnicodeAndSendToDisplayC(str, cur__);
    else
      stringToUnicodeAndSendToDisplay(str);
    printf("|%s\r\n", str);
  }
  return "";
}
void printStringLR(char *str, int s)
{
  char buff[20];
  int begin = 0;
  int end = 0;

  if (strlen(str) > 20)
  {
    //clear buff
    if (!s) //0
    {
      // split left
      begin = 0;
      end = 20;
      printf("printf left\r\n");
    }
    else
    {
      // split right
      printf("printf right\r\n");
      begin = 20;
      end = 41;
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
  int a, b, c;
  if (notepad_checkEnterSignInLine(str) == 1)
  {
    a = notepad_countLinewithOutLNsign(str);
    b = notepad_countEnterSign(str);
    if (debug)
      printf("////a = %d,b = %d //// \r\n", a, b);
    //notepadmax-(b+a)
    if (a + b != notepad_MaxinLine)
    {
      c = notepad_MaxinLine - (b + a);
      if (debug)
        printf(" ok yes-----------------------\r\n");
      while (c > 0)
      {
        keybuff[0] = (char)enterSign;
        notepad_append(str, keybuff, a++);
        c--;
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
  int indexSign = notepad_countLinewithOutLNsign(str); //ไม่เต็มบรรทัด 40 ch
  if (indexSign != 39)
    removeChar(str, indexSign);
}
//--check in line "-"
int notepad_checkEnterSignInLine(char *str)
{
  int cc = 0;
  int st = 0;
  if (strlen(str) > 0)
  {
    while (str[cc] != 0x00 && cc < notepad_MaxinLine)
    {
      if (str[cc] == enterSign)
      {
        st = 1;
        break;
      }
      cc++;
    }
  }
  return st;
}
/*
 * 
 * 
 * 
 * 
*/
int notepad_countLinewithOutLNsign(char *str)
{
  int cc = 0;
  while (str[cc] != enterSign && cc < notepad_MaxinLine && str[cc] != '\0')
  {
    if (str[cc] == enterSign)
      break;
    cc++;
  }
  return cc;
}
//--------------
// แทรกสตริง (Stringต้นทาง, char arry, ตำแหน่งที่แทรก)
void notepad_append(char subject[], const char insert[], int pos)
{
  char buf[bufferMaxSize] = {0}; // 100 so that it's big enough. fill with zeros
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
  int cccccc = 0;
  printf("|");
  while (aaa < notepad_MaxinLine)
  {
    if (str[aaa] != 0)
    {
      printf("%c", str[aaa]);
      cccccc++;
    }
    aaa++;
  }
}
int notepad_getnullPostion(char *str)
{
  int cc_m = 0;
  while (str[cc_m] != 0)
  {
    // null byte
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
    {
      //-
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
  //---------------------------------------------------------------
  // a
  // b
  // c
  //---------------------------------------------------------------
  //---------------------------------------------------------------

  //-----------------------joyRight--------------------------------
  if (a == 0x00 && b == 0x00 && c == 0x04)
  {
    keyCode__ = ARROW_LEFT; // ArrowLeft
  }
  else if (a == 0x00 && b == 0x00 && c == 0x10)
  {
    keyCode__ = ARROW_UP; // ArrowUp
  }
  else if (a == 0x00 && b == 0x00 && c == 0x08)
  {
    keyCode__ = ARROW_RIGHT; // ArrowRight
  }
  else if (a == 0x00 && b == 0x00 && c == 0x20)
  {
    keyCode__ = ARROW_DOWN; // ArrowDown
  }
  //---------------------------joyleft------------------------------
  else if (a == 0x00 && b == 0x20 && c == 0x00)
  {
    keyCode__ = 37; // ArrowLeft
  }
  else if (a == 0x00 && b == 0x80 && c == 0x00)
  {
    keyCode__ = 38; // ArrowUp
  }
  else if (a == 0x00 && b == 0x40 && c == 0x00)
  {
    keyCode__ = 39; // ArrowRight
  }
  else if (a == 0x00 && b == 0x00 && c == 0x01)
  {
    keyCode__ = 40; // ArrowDown
  }
  //----------------------------------------------------------
  else if (b == 64 || c == 8)
  {
    keyCode__ = 13; // enter
  }
  else if (a == 0x00 && isSpaceKey(b) && c == 0x00) //space bar
  {
    keyCode__ = 32; // space
  }

  else if (a == 0x40 && b == 0x00 && c == 0x00) // enter button 7
  {
    keyCode__ = KEY_ENTER;
  }
  //-----------------on read sector----------------
  //
  else if (a == 0x00 && b == 0x04 && c == 0x00)
  {
    // left button (readMode)
    keyCode__ = 1;
  }
  else if (a == 0x00 && b == 0x08 && c == 0x00)
  {
    // right button (readMode)
    keyCode__ = 2;
  }
  /*else if (a == 0x11 && b != 0x0)
  {
    //space + e
    keyCode__ = 661;
  }*/
  else if (a == 0x1b && isSpaceKey(b) && c == 0x00)
  {
    // go to line
    // space + g
    keyCode__ = SHORTCUT_KEY_GOTO_LINE;
  }
  else if (a == 0x0e && isSpaceKey(b) && c == 0x00)
  {
    // space + s
    keyCode__ = NOTEPAD_SAVE_FILE;
  }
  // on read Mx0dmode set mark
  else if (a == 0x0d && isSpaceKey(b) == true && c == 0x00)
  { // set mark 1-3-4 + spaces
    // space + m
    keyCode__ = MARK_SET_MARK;
  }
  else if (a == 0x1a && isSpaceKey(b) == true && c == 0x00)
  { // jump  to Mark (Space + j; 2-4-5 + Space) next
    // space + j
    // space + 2 + 4 + 5
    keyCode__ = MARK_JUMP_TO_MARK;
  }
  else if (a == 0x11 && isSpaceKey(b) == true && c == 0x00)
  {
    // (Close) space + e
    keyCode__ = CLOSE_FILE;
  }
  else if (a == 0x9a && isSpaceKey(b) == true && c == 0x00)
  {
    // 2-4-5-8 + Space
    keyCode__ = MARK_JUMP_NEXT;
  }
  else if (a == 0x99 && isSpaceKey(b) == true && c == 0x00)
  {
    // 1-4-5-8 +Space
    keyCode__ = MARK_DELETE_MARK;
  }
  else if (a == 0x5a && isSpaceKey(b) == true && c == 0x00)
  {
    // 2-4-5-7+ Space
    keyCode__ = MARK_JUMP_PREVOIUS;
  }
  // next page in read mode
  else if ((a == 0x00 && b == 0x41 && c == 0x00) || (a == 0x00 && b == 0x42 && c == 0x00) || (a == 0x00 && b == 0x43 && c == 0x00))
  {
    keyCode__ = SHORTCUT_KEY_NEXT_PAGE;
  }
  // previous page
  else if ((a == 0x00 && b == 0x21 && c == 0x00) || (a == 0x00 && b == 0x22 && c == 0x00) || (a == 0x00 && b == 0x23 && c == 0x00))
  {
    keyCode__ = SHORTCUT_KEY_PREVOIUS_PAGE;
  }
  // debug shortcut key next and prevoious pages
  else if ((a == 0x00 && b == 0x11 && c == 0x00)) //tclearest
  {
    keyCode__ = 703;
  }
  else if ((a == 0x07 && b == 0x00 && c == 0x00)) //test
  {
    keyCode__ = 787;
  }
  else if ((a == 0x07 && b == 0x01 && c == 0x00)) //test
  {
    keyCode__ = 789;
  }
  else if ((a == 0x91 && isSpaceKey(b) == true && c == 0x00))
  { // [test] edit file (1+5+8 + space)
    keyCode__ = 987;
  }
  return keyCode__;
}
bool isSpaceKey(int a)
{
  if (a == 1 || a == 2 || a == 3)
    return true;
  else
    return false;
}
/*====================== manage key ==================
//
//
//
==================ฟั่งก์ชั่นจัดการ key ReadKey===============
=======================================================
*/
void keyRead()
{
  //used
  if (USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    //----------------------------- uart to key--------------------------------
    uart2Buffer = USART_ReceiveData(USART2); //-
    if (uart2Buffer == 0xff && SeeHead == 0)
    {

      SeeHead = 1;  //-
      countKey = 0; //-
    }               //-
    if (countKey == 2 && uart2Buffer == 0xa4)
    {
      seeCur = 1;
    }
    if (countKey >= 4 && countKey <= 6)
    {
      //-
      bufferKey3digit[countKey - 4] = uart2Buffer; //-
    }
    if (countKey == 2) // checkKeyError
    {
      checkKeyError = uart2Buffer;
    }
    countKey++;
    // ---------------------------- end uart to key ----------------------------
    // printf("[%x]\r\n",uart2Buffer);
  }
  if (countKey >= maxData)
  {
    //Recieve & checking key
    seeHead = 0;
    printf("See key %x,%x,%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
    if (checkKeyError == 0xff)
    {
      //clear error key
      countKey = 0;
      SeeHead = 0;
    }

    // key mapping //
    keyCode = keyMapping(bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
    printf("Keycode: %d\r\n", keyCode);
    printf("Mode: %d\r\n", mode);
    if (mode == 0)
    {
      //printf("999999999999999999999999999999999999\r\n");
      //key in mode 0
      if (count_menu >= 1 && count_menu <= maxMenu && mode == 0)
      {
        //select menu //modemenu
        if (keyCode == 40)
        {
          //up
          if (count_menu != maxMenu)
          {
            count_menu++;
          }
          else
          {
            delay_ms(50);
            beepError();
            printf("beep");
          }
        }
        else if (keyCode == 38)
        {
          //down
          if (count_menu != 1)
          {
            count_menu -= 1;
          }
          else
          {
            delay_ms(50);
            beepError();
            printf("beep");
          }
        }
      }
      if (keyCode == 39)
      {
        if (mode == 0) //enter to mode
        {
          //
          previousMode = mode;
          mode = count_menu;
          printf("enter to mode:%d", mode);
          if (mode == 2) //reset
          {
            ABCCCCC = 1;
          }
        }
      }
      caseMenu(count_menu);
    }
    /*
    *============================================================
    *โหมด 2 หลังจากอ่านไฟล์ 
    * mode 2 และทำการเปิดอ่านไฟล์แล้ว จะแยกกันกับฟังก์ชั่น path
    *============================================================
    */
    //-----------------------------------end mode (10)-------------------------------
    if (mode == MODE_READ && ROMR.endReadFile == true)
    {
      slidText2Displayv2();
    }
    /*if (mode == MODE_READ && useFEdit == true) //edit useFEdit Editing
    {
      printf("editor\r\n");
      editFile();
    }*/
    /*
    ============================================================
    โหมด 2 ก่อนอ่านไฟล์
    ============================================================
    */
    if (mode == MODE_READ && ROMR.endReadFile == false) // ถ้ายังไม่เปิดไฟล์
    {

      // mode 2 แต่ยังไม่ได้ทำการเปิดอ่านไฟล์
      // key in mode 2
      if (keyCode == 38) //เลือกไฟล์
      {
        if (fileSelect > 0)
        {
          fileSelect--; // เลื่อนลง
        }
        else
        {
          beepError(); // สุด
        }
      }
      else if (keyCode == 40) //เลือกไฟล์
      {
        if (fileSelect < maxFile - 1)
        {
          fileSelect++; //เลื่อนขึ้น
        }
        else
        {
          beepError(); // สุด
        }
      }
      printf("%s\r\n", fileLists[fileSelect]);
      stringToUnicodeAndSendToDisplay(fileLists[fileSelect]);
      // printf("%s\r\n", Dirpath);

      // delete item (key 7 + space)
      if (bufferKey3digit[0] == 0x40 && bufferKey3digit[1] != 0 && bufferKey3digit[2] == 0)
      {
        stringToUnicodeAndSendToDisplay("Delete this Folder");
        if (confirm_delete()) // confirm_delete delete
        {
          // ยืนยันทำการลบ
          if (ex_checkFileType(fileLists[fileSelect])) // ถ้าประเภทไฟล์ไม่เป็น 0
          {
            if (DeleteFile(fileLists[fileSelect]))
            {
              stringToUnicodeAndSendToDisplay("the file has Deleted");
              removeDataInFileTemp(fileLists[fileSelect]);
            }
          }
          else if (DeleteFolder(fileLists[fileSelect]))
          {
            stringToUnicodeAndSendToDisplay("Deleted");
            removeDataInFileTemp(fileLists[fileSelect]);
            //beep();
          }
        }
        else
        {
          stringToUnicodeAndSendToDisplay(fileLists[fileSelect]);
        }
      }
      /*==============================================================
       * ------------------------โยกจอยทางขวา เข้าอ่านอไฟล์ หรือเปิดโฟลเดอร์
       *==============================================================
       */
      if (keyCode == 39) //เปิด folder of file
      {
        // enter // right joy
        // command_ = 16;
        printf("Opening %s\r\n", fileLists[fileSelect]);
        printf("type:%d\r\n", ex_checkFileType(fileLists[fileSelect])); // เช็คประเภทไฟล์
        /* =============================================
         * ------------------ถ้าเป็นโฟลเดอร์
         * =============================================
         */
        if (ex_checkFileType(fileLists[fileSelect]) == 0) //  type 0 is folder
        {
          // is folder
          maxFile = 0;
          delay_ms(45);
          if (ex_openDir(fileLists[fileSelect]))
          {
            if (strstr(fileLists[fileSelect], "..") != NULL) //ถ้าเป็น .. ไม่ต้องบันทึกเส้นทาง
            {
              if (ex_countPath(Dirpath) > 0)
              {
                //back and clear path
                ex_exitOncePath();      // remove current path
                ex_cdWithPath(Dirpath); //back one path
              }
            }
            else
            {
              //save path
              ex_savePath(fileLists[fileSelect]); // บันทึกเส้นทาง
            }
            fileSelect = 0;
            //printf("seach all file sucess \r\n");
            command_ = 4;
          }
        } //readFileFromCH376sToFlashRom

        /* ====================================================================
         * -----------ถ้าเป็นไฟล์ tbt brf หรือ txt---------------------------------
         * ====================================================================
         * */
        else if (ex_checkFileType(fileLists[fileSelect]) == 1 || ex_checkFileType(fileLists[fileSelect]) == 2 || ex_checkFileType(fileLists[fileSelect]) == 3)
        {
          // read text to rom
          readFileFromCH376sToFlashRom(fileLists[fileSelect]); //เปิดไฟล์ open file
        }
      }
      if (keyCode == 987) // แก้ไขไฟล์ (1+5+8 + space) edit
      {                   // test edit file
        useFEdit = 1;     // ตั้งค่าสถานะบอก notepad ว่าเข้าใช้จากโหมด edit file

        printf("====================================================\r\n");
        printf("---------------------Edit file----------------------\r\n");
        printf("====================================================\r\n");
        readTextFileToMemmory(fileLists[fileSelect]); // อ่านไฟล์จาก sd card  ,อ่านเข้า ROM for edit
        printf("end read %d\r\n", readFileStatus___);
        Notepad.cursorPosition = 0;
        Notepad.currentLine = 0;
        Notepad.displayFirst = false;
        prepareEditor(); // เตรียมแก้ไขไฟล์ อ่านข้อมูลจาก ROM เข้าแรม
                         /*if (USART_GetITStatus(USART2, USART_IT_RXNE))
          i = USART_ReceiveData(USART2);*/
        bufferKey3digit[0] = 0;
        bufferKey3digit[1] = 0;
        bufferKey3digit[2] = 0;
        //mode = 5;
        while (useFEdit)
        {
          doing = 1; // enable notepad mode
          printf("--------------------------reg-------------\r\n");
          notepad_main();
        }

        //editFile();
        /*  getFileSize(fileLists[fileSelect]);        // เปิดไฟล์
        ch376_status = CH376_GetFileSize();        // อ่านขนาดไฟล์
        SendCH370(FileClose0, sizeof(FileClose0)); // ปิดไฟล์
        ex_cdWithPath(Dirpath);                    // เข้า path ใหม่ เพราะถูกรีเซ็ทจาก CH376_GetFileSize
        if (ch376_status <= 4096)                  // ขนาดไฟล์ไม่ใหญ่เกินขนาดแรม
        {
          printf("file size: (%d)\r\n", ch376_status);  // แสดงขนาดไฟล์ห
         
          //prerpare_edit_notepad();                      // อ่านจาก ROM เข้า MEMMORY();
          //notepad_main();                               // ใช้ notepad เดิม ไม่ต้องแก้เพิ่ม
        }*/
      }
      if (keyCode == 2) // 787 display file size
      {
        getFileSize(fileLists[fileSelect]);        // เปิดไฟล์
        ch376_status = CH376_GetFileSize();        // อ่านขนาดไฟล์
        SendCH370(FileClose0, sizeof(FileClose0)); // ปิดไฟล์
        if (ch376_status >= 1000)                  // แปลงแบบย่อ
        {
          sprintf(fileByte, "%d", ch376_status / 1000);
          strcat(fileByte, " Kbytes");
        }
        else
        {
          sprintf(fileByte, "%d", ch376_status);
          strcat(fileByte, " bytes");
        }
        printf("file size %s byte\r\n", fileByte); // แสดง
        stringToUnicodeAndSendToDisplay(fileByte); // ex. 1000 bytes
        printf("path is %s\r\n", Dirpath);
        ex_cdWithPath(Dirpath); // เข้าพาทใหม่ เพราะถูก รีเซ็ทจากการอ่าน File size
        //========================
        //ch376_status = CH376_FileClose();
        //printf("file close status %d \r\n", ch376_status);
        //SendCH370(FileClose0,sizeof(FileClose0));
      }
      else if (keyCode == 1) // display name
      {
        stringToUnicodeAndSendToDisplay(fileLists[fileSelect]);
        // display file name
      }
      /*
        ====================================================
        ------------โยกจอยทางซ้าย ย้อนกลับออกจาก directory-----
        ====================================================
      */
      if (keyCode == ARROW_LEFT) // left joy <-
      {
        printf("exit\r\n");
        maxFile = 0;
        printf("pathD: %d\r\n", ex_countPath(Dirpath));
        printf("dir path :%s \r\n", Dirpath);
        if (ex_countPath(Dirpath) == 0) // ถ้าเป็น root ก่อนหน้าเป็น 1 งงเลย
        {
          maxFile = 0;
          for (i = 0; i < maxfileListBuffer; i++) // reset value in fileLists
            memset(fileLists[i], 0, sizeof(filelist[i]));

          memset(Dirpath, 0, sizeof(Dirpath)); //clear path
          printf("reset all \r\n");
          mode = 0;
          ABCCCCC = 0;                             // reset แต่ก่อนบัค เพราะประกาศเป็น private int ABCCCCC = 0 เลยประกาศเป็น global variable
          stringToUnicodeAndSendToDisplay("read"); // display standy  show "raed".
        }
        else
        {
          ex_exitOncePath();
          ex_cdWithPath(Dirpath);
          command_ = 4;
        }
      }
    }
    /*
    ============================================================
    โหมด 3 (หลังจากออกจากโหมด 3)
    ============================================================
    */
    if (mode == MODE_BLUETOOTH)
    {
      if (keyCode == 37)
      {
        mode = 0;
        GPIO_ResetBits(GPIOC, GPIO_Pin_0); // turn Off Bluetooth module
        stringToUnicodeAndSendToDisplay("Bluetooth");
        //printf("exit from mode 3 \r\n");
      }
    }
    //-----------------------------------end mode (4)-------------------------------
    /*
    ============================================================
    โหมด 4  หลังจากออกจากโหมด 4
    ============================================================
    */
    if (mode == MODE_TOOLS)
    {
      stringToUnicodeAndSendToDisplay("Battery level");
    }
    /*
      reset value
    */
    countKey = 0;
    keyCode = 0;
    seeCur = 0;
  }
}
/*==========================================================================
-------------readFileFromCH376sToFlashRom-----------------------------------
 อ่านข้อมูลจาก SD Card โดยใช้ chip CH376 ลง Flash rom
 read text to rom
 789
============================================================================*/
int readFileFromCH376sToFlashRom(char *fileName___)
{
  readFileStatus___ = 1;    //ค่าเริ่มต้นการอ่าน เพื่อให้ติดหลูปการอ่านไฟล์
  ROMR.endReadFile = false; //จบการอ่่าน
  ROMR.countdata_Temp512 = 0;
  ROMR.lastAscii = 0;
  ROMR.addressWriteFlashTemp = 0;
  addressSector = 0;
  countSector512 = 0;
  countSector4096 = 0;
  countSector = 0;
  //stringToUnicodeAndSendToDisplay("Reading....");
  //SendCH370(ResetAll, sizeof(ResetAll)); //reset chip
  //printf("reading............ all\r\n");
  delay_ms(10);
  command_ = 4;
  while (readFileStatus___ == 1)
  {
    if (command_ == 4)
    {
      setFilename(fileName___);                     // ใช้คำสั่ง set ชื่อไฟล์
      printf("opening file (%s)\r\n", fileName___); // กำลังเปิดไฟล์ ชื่อไฟล์
      command_++;                                   // 5
      stringToUnicodeAndSendToDisplay("Reading....");
      delay_ms(45);
    }
    else if (command_ == 5)
    {
      SendCH370(FileOpen, sizeof(FileOpen));
      command_++; //6
      printf("openfile %s status  \r\n", fileName___);
      delay_ms(45);
    }
    else if (command_ == 6)
    {
      SendCH370(SetByteRead, sizeof(SetByteRead));
      printf("SetByteRead 3\r\n");
      //result 1D can read
      //0x1d สามารถอ่านไฟล์ได้
      command_ = 99;
      delay_ms(30);
    }
    else if (command_ == 95)
    {
      //left (prevois line)
      printf("continueRead 4\r\n");
      SendCH370(continueRead, sizeof(continueRead));
      command_++; //96
    }
    else if (command_ == 98)
    {
      //printf("reading 5\r\n");
      //right (next line)
      command_ = 6;
    }
    else if (command_ == 99)
    {
      printf("read data 6\r\n");
      SendCH370(ReadData, sizeof(ReadData));
      command_++;
    }

    //menu_s();
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
      // printf("a");
      i1 = USART_ReceiveData(USART3);
      //printf("command = %d ,%x", i1,command_); //ปริ้นแล้วอ่านไม่ทัน
      if (command_ == 96 && i1 == 0x14)
      {
        command_ = 6;
      }
      if (i1 == 0x80 || i1 == 0x14)
      {
        // printf("funk\r\n");
      }
      else if (command_ == 100 && countSector512 < 4 && ROMR.countdata_Temp512 < 512)
      {
        if (i1 == '\0')
        {
          // printf("End of File\r\n");-
        }
        else if (ROMR.countdata_Temp512 < ((128) * (countSector512 + 1)) - 1)
        {
          //------------------------------128 byte----------------------------
          // เก็บcharactor 128 byte
          ////////////////////////////////////////////////////////////////////
          ROMR.dataTemp512[ROMR.countdata_Temp512] = i1;
          ROMR.countdata_Temp512++;

          ROMR.waitEnd = 0;
          ROMR.lastAscii = i1;
        }
        else
        {
          //-----------------------------------------------------------------
          // ข้อมูลที่่อ่านครบ 128*4 = 512
          // ทำการเพิ่มคัวแปร countSector512
          ///////////////////////////////////////////////////////////////////

          countSector512++;
          ROMR.dataTemp512[ROMR.countdata_Temp512] = i1;
          ROMR.countdata_Temp512++;
          if (countSector512 >= 4)
          {
            //printf("store \r\n");
            //---------------------------512 byte-----------------------------
            //  เอาข้อมูล 512 เก็บลง buffer ยาว [4096] จนครบ
            //
            //////////////////////////////////////////////////////////////////
            command_ = 95;
            for (i = ROMR.addressWriteFlashTemp; i < ROMR.addressWriteFlashTemp + 512; i++)
            {

              SST25_buffer[i - (sector * countSector)] = ROMR.dataTemp512[i - ROMR.addressWriteFlashTemp];
              ROMR.dataTemp512[i - ROMR.addressWriteFlashTemp] = '\0'; //clear buffer
              // printf("92 2\r\n");
            }
            // printf("store 2\r\n");
            //  ทำการเพิ่มจำนวนตัวแปรที่นับ sector:countSector4096
            countSector4096++;
            //writeFlash(ROMR.addressWriteFlashTemp);
            ROMR.addressWriteFlashTemp += ROMR.countdata_Temp512;
            if (countSector4096 >= 8)
            {
              //---------------------------4096 byte---------------------------
              // ข้อมูลครบ 512*8 = 4096 ----> 1 sector
              // push (string 4096 charactor) to flash rom.
              //
              //////////////////////////////////////////////////////////////////
              //use variable:SST25_W_BLOCK //4096
              // stringToUnicodeAndSendToDisplay("Reading....");
              writeFlash(addressSector); //1*4096
              //clearUnsignChar();

              //Delay(0xff);
              stringToUnicodeAndSendToDisplay("On read file Reading....");
              addressSector += sector;
              countSector4096 = 0;
              //--------------------------------------------------------------------------------------------
              //check this value `ROMR.addressWriteFlashTemp`--
              //--------------------------------------------------------------------------------------------
              countSector++;
            }
            //--- reset temp sector---//
            ROMR.countdata_Temp512 = 0;
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
      if (ROMR.lastAscii == i1)
      {
        ROMR.waitEnd++;
        //กำหนดระยะเวลา หากไม่มีข้อมูลแล้ว จะเข้าในเงื่อนไขนี้
        if (ROMR.waitEnd == 100 * 100) // end of file check time out
        {
          SendCH370(FileClose0, sizeof(FileClose0));
          beepError();
          for (i = ROMR.addressWriteFlashTemp; i < ROMR.addressWriteFlashTemp + 512; i++)
          {
            SST25_buffer[i - (sector * countSector)] = ROMR.dataTemp512[i - ROMR.addressWriteFlashTemp];
          }
          configFlash();
          writeFlash(addressSector);

          ROMR.addressWriteFlashTemp += ROMR.countdata_Temp512;
          ROMR.waitEnd++;
          //----------------------store last sector to flash rom-----------------------
          //
          /////////////////////////////////////////////////////////////////////////////
          // readSecter(0);
          //printf("string** : %s \r\nEND**", SST25_buffer);
          /*SPI_FLASH_CS_LOW();
                    SST25_R_BLOCK(0, SST25_buffer, sector);
                    SPI_FLASH_CS_HIGH();
                    Delay(0xffff);
                    convert_text2_buffer(SST25_buffer);*/

          ROMR.endReadFile = true;

          AmountSector = ROMR.addressWriteFlashTemp / sector;  // ---- จำนวน sector ----
          AmountSectorT = ROMR.addressWriteFlashTemp % sector; // ---- เศษ ที่เหลือของ sector ---

          //----------------------------------
          // เก็บบรรทัด
          // จำนวน sector และตัวเศษ

          mark_init(); // set mark position at 0
          mark_readFormROM();
          initSlidingMode();
          readSecter(0);

          while (ROMR.endReadFile == true)
          {
            // query string-
            // menu_s();
            keyRead();
            // printf("9999999999999999srwetre\r\n");
            // readFileStatus___
          }

        } // ทดสอบ
      }
    }
  }
  return 0;
}

void initSlidingMode()
{
  read.TotalSector = AmountSector;
  //read.sesString = AmountSectorT;
  read.currentLine = 0;
  read.currentSector = 0;
  read.totalLine = 0;
  clearReadlineInsector();
  StoreLine();
  pages.totalPage = pages.index; //เก็บ จำนวนหน้า

  printf("======================================================\r\n");
  printf("======================================================\r\n");
  printf("pages:(%d)\r\n", pages.totalPage);
  printf("======================================================\r\n");
  printf("======================================================\r\n");
  printf("paage index is %d \r\n", pages.index);
  for (i = 0; i < pages.index; i++)
  {
    printf("page (%d) at line(%d) \r\n", i, pages.page[i]);
  }
  printf("total sector (%d)", read.TotalSector);
  for (i = 0; i <= read.TotalSector; i++)
  {
    printf("line in sectore (%d) at line(%d) \r\n", i, read.lineInsector[i]);
  }
  pages.index = 0;
  printf("======================================================\r\n");
  //printf("line in last sectore (%d) \r\n", read.lineInsector[read.TotalSector]);
  printf("======================================================\r\n");
}
void clearReadlineInsector()
{
  for (i = 0; i < sizeof(read.lineInsector) / sizeof(read.lineInsector[0]); i++)
    read.lineInsector[i] = '\0';
}

//----------------------- check file type ------------------------
// *.TBT file return 1
// *.BRF file return 2
// *.TXT file return 3
/////////////////////////////////////////////////////////////////
int ex_checkFileType(char *file)
{
  //used
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
{
  //used
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
/* ==================================================================================
 * ----------- [search file] first version. current version not use..----------------
 * ฟังก์ชั่นค้นหาไฟล์ใน sd card ด้วย ch376 chip
 * ==================================================================================
 * */
void searchFile2()
{
  int time_check = 0;
  printf("Seaching.............19...............\r\n");
  while (ABCCCCC == 1)
  {
    if (command_ == 1)
    {
      // SendCH370(setSDCard, sizeof(setSDCard));
      SendCH370(checkConnection, sizeof(checkConnection));
      //printf("Check Connection str %s\r\n", FileName_buffer);
      command_++; //2
      delay_ms(45);
    }
    else if (command_ == 2)
    {
      // SendCH370(DiskConnect, sizeof(DiskConnect));
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
      stringToUnicodeAndSendToDisplay("Please wait...");
      //ex_exitOncePath();
      delay_ms(45);
      if (ex_openDir("*"))
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
      {
        // if see file
        nextAgain = 1;
        DataForWrite[0] = 42;
      }
      else if (i1 == 0x42 && countFileLegth == 0) // จบการค���นหาไฟล์
      {
        //ABCCCCC = 0; //fix bug
        SendCH370(FileClose0, sizeof(FileClose0));
        delay_ms(45);
        stringToUnicodeAndSendToDisplay(fileLists[fileSelect]);
        // printf("file--00 %s\r\n",fileLists[fileSelect]);
        // printf("--------------------end -------------------------\r\n");
      }
      if (command_ == 2 && i1 == 0x82)
      {
        ABCCCCC = 0;
        // break;
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
          DataForWrite[countFileLegth] = 42; //42
          countFileLegth++;
          time_check = 0;
        }
      }
    }
    if (countFileLegth >= 33)
    {
      // printf("length:%d\r\n",countFileLegth);
      // for(j = 0; j < 32; j++)
      // printf("%s max %d:\r\n",fileName(),maxFile);
      strcpy(fileLists[maxFile], fileName());
      maxFile++;
      countFileLegth = 0;
      // printf("\r\n%s",fileName());
      if (nextAgain == 1)
      {
        // printf("nextttttttttttttt\r\n");
        delay_ms(45);
        command_ = 16;
        // next agian
      }
    }
    time_check++;
    if (time_check > 10005)
    {
      time_check = 0;
      if (nextAgain == 1)
      {
        // printf("nextttttttttttttt\r\n");
        delay_ms(45);
        command_ = 16;
        // next agian
      }
    }
  }
}
//----------------------------- open directory ------------------------------
// [ex_openDir] use for open directory with ch376 module
// เปิดโฟลเดอร์ โดยจะเช็ค root ด้วย
// * เป็นการค้นหา
// .. ย้อนกลับ
//////////////////////////////////////////////////////////////////////////////
int ex_openDir(char *dirPath__)
{
  int Open_stats = 1, output = 0;
  int time_check = 0;
  char buff[30] = "/";
  char buff2[30] = "/";
  command_ = 1;
  //memset(buff,0,30);
  if (strstr(dirPath__, "..") != NULL)
  {
    // if see ".." -> /..
    strcat(buff, dirPath__);
  }
  while (Open_stats)
  {
    if (command_ == 1)
    {
      if (ex_checkSlash(Dirpath) == 0) // current path is root
      {
        strcat(buff2, dirPath__);
        setFilename(buff2); // open in root
        printf("print file name:%s\r\n", buff2);
      }
      else if (strlen(buff) > 1) // not root
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
      {
        // error not found path
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
// บันทึกพาทไปที่ ตัวแปร Dirpath เป็นตัวแปรแบบ global
//////////////////////////////////////////////////////////////////////////////
int ex_savePath(char *pathName)
{
  //used
  char prepareStr[30] = "";
  if (ex_checkSlash(pathName))
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
void ex_exitOncePath()
{
  //used
  int start = ex_countPath(Dirpath);
  int longL = strlen(Dirpath);
  if (start > 0)
    while (start == ex_countPath(Dirpath))
    {
      Dirpath[longL] = 0x0; //remove path
      if (longL == 0)
        break;
      longL--;
    }
}
/*------------------------------open dir with path --------------------------
 ex. path = "abc/def"
 ex_cdWithPath(path);
 เปิดพาท ตัวอย่าง a/b/c เป็นสตริง
*/
void ex_cdWithPath(char *path)
{
  //used
  int L = 0;
  char buffer[15] = "";
  int bbuf = 0;
  int root = 1;
  char rootD[15] = "/";
  //printf("********************************************\r\n");
  ex_openDir("..");
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
          ex_openDir(rootD);
        }
        else
        {
          //printf("\r\nbuffer2: %s\r\n", buffer);
          ex_openDir(buffer);
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
  ex_openDir(buffer);
  //printf("\r\nbuffer3: %s\r\n", buffer);
}
//---------------------- count path ------------------------------------------
//นับจำนวน path ของตัวแปร DirPath
//Output: number of path
//////////////////////////////////////////////////////////////////////////////
int ex_countPath(char *pathSource)
{
  //used
  int ex_countPath = 0;
  while (*pathSource)
  {
    if (*pathSource == '/')
    {
      ex_countPath++;
    }
    pathSource++;
  }
  return ex_countPath;
}
/**====================================
 * เช็คเครื่องหมาย / ใน path
 * */
int ex_checkSlash(char *pathName)
{
  //used
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
// เหมือน createFileAndWrite แต่จะเก็บเส้นทาง
// ex_cdWithPath(Dirpath);

int createFile2(char *name)
{
  int status_create = 0;
  if (command_ == 4)
  {
    setFilename(name);
    printf("set filename \r\n");
    command_++; //8
    delay_ms(50);
  }
  else if (command_ == 5)
  {
    printf("file create \r\n");
    SendCH370(FileCreate, sizeof(FileCreate));
    delay_ms(80);
    command_++; //10
  }
  else if (command_ == 6)
  {
    printf("file close \r\n");
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
/*-------------------------- Create and write file()------------------------
// สร้างไฟล์ และเขียนไฟล์
//
// test function --
//////////////////////////////////////////////////////////////////////////////*/
void createFileAndWrite(char *fname)
{
  //use
  command_ = 0;
  command_++;
  while (1)
  {
    if (createFile(fname) == 1) // สร้างไฟล์
      break;
  }
  SendCH370(ResetAll, sizeof(ResetAll)); // reset all
  delay_ms(100);
  command_ = 1;
  PrepareText(); // เตรียมข้อความ ลบ Enter Sign add '\r\n'
  //printf("all text :%s\r\n====================================================== ", str_test);
  //fileWrite(0,fname,"test head \r\n my name is surasak");
  writeFile4096(fname, SST25_buffer); //เขียนไฟล์
}
/*====================================================
 * ลบเครื่องหมาย enter sign
 * เติม \r\n
 *  */
void PrepareText()
{
  int c;
  memset(SST25_buffer, 0, 4096); //clear
  for (c = 0; c <= maxLineN; c++)
  {
    // delay_ms(200);
    printf("=====================removed======================\r\n");
    substringRemoveEnter2(Notepad.buffer_string[c]);
    printf("%s\r\n", buffer_afterRemove);
    strcat(SST25_buffer, buffer_afterRemove);
    strcat(SST25_buffer, "\r\n"); // ใส่ \r\n
    delay_ms(40);
    if (1)
    { //เกิน 4096
    }
  }
  //
  //
}
//-----------------------write file less than 4096 or equal------------------
//
//////////////////////////////////////////////////////////////////////////////
void writeFile4096(char *fname, char *strSource)
{
  //use
  int maxSize = 0, sizeWrite = 128;
  int loop255 = 0;
  int Sloop255 = 0;
  int iL = 0, pp = 0;
  char buffForWrite[128], buffForWrite2[128];
  int WF = 0; //write before
  memset(buffForWrite2, 0, 256);
  maxSize = strlen(strSource);
  loop255 = maxSize / sizeWrite;
  Sloop255 = maxSize % sizeWrite;
  while (iL < loop255)
  {
    //
    buffForWrite[pp - iL * sizeWrite] = strSource[pp];
    pp++;
    if (pp % sizeWrite == 0)
    {
      command_ = 1;
      while (1)
      {
        //save 255
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
  {
    //save 255
    if (fileWrite(WF, fname, buffForWrite2) == 1)
    {
      break;
    }
  }
}
void writeFile40962(char *fname, char *strSource)
{
  //use
  int maxSize = 0, sizeWrite = 128;
  int loop255 = 0;
  int Sloop255 = 0;
  int iL = 0, pp = 0;
  char buffForWrite[128], buffForWrite2[128];
  int WF = 0; //write before
  memset(buffForWrite2, 0, 256);
  maxSize = strlen(strSource);
  loop255 = maxSize / sizeWrite;
  Sloop255 = maxSize % sizeWrite;
  while (iL < loop255)
  {
    //
    buffForWrite[pp - iL * sizeWrite] = strSource[pp];
    pp++;
    if (pp % sizeWrite == 0)
    {
      command_ = 4;
      while (1)
      {
        //save 255
        if (fileWrite2(WF, fname, buffForWrite) == 1)
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
  command_ = 4;
  while (1)
  {
    //save 255
    if (fileWrite2(WF, fname, buffForWrite2) == 1)
    {
      break;
    }
  }
}
//------------------------------       -------------------------------------
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
      {
        //-
        SeeHead = 1;  //-
        countKey = 0; //-
      }
      if (countKey == 2 && uart2Buffer == 0xa4)
      {
        // if cursor
        seeCur = 1;
      }
      if (countKey >= 4 && countKey <= 6)
      {
        //-
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
        {
          //check error key
          //printf("Key Error");
          countKey = 0;
          SeeHead = 0;
        }
        if ((bufferKey3digit[0] == 0x00 && bufferKey3digit[1] == 0x20 && bufferKey3digit[2] == 0x00) || (bufferKey3digit[0] == 0x00 && bufferKey3digit[1] == 0x00 && bufferKey3digit[2] == 0x04))
        {
          //back to notepad
          saving = 0;
        }
        if (bufferKey3digit[0] == 0x80 && bufferKey3digit[1] == 0 && bufferKey3digit[2] == 0)
        {
          keyCode = 8;
        }
        if (bufferKey3digit[2] == 2 || bufferKey3digit[1] == 0x10)
        {
          // save file
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
          strcat(nameBuff, ".TBT"); // tbt file
          //printf("\r\nSaving file :%s\r\n", nameBuff);
          createFileAndWrite(nameBuff);
          stringToUnicodeAndSendToDisplay("Saved successfully"); //Saved successfully
          beep4();
          delay_ms(1200);
          saving = 0;
        }
        if (keyCode == 8)
        {
          // delete file name
          if (cc > 1)
          {
            // steel save root /
            cc--;
            nameBuff[cc] = '\0';
            //printf("Delete:%s \r\n", nameBuff);
          }
          else if (cc == 1)
          {
            // last cha
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
          {
            // name length less than 10
            nameBuff[cc] = i;
            //printf("Filename:%s\r\n", nameBuff);

            cc++;
          }
        }
        memset(bufferDisplay, 0, 20);    //clear buffer
        strcpy(bufferDisplay, "Name:");  // copy string
        strcat(bufferDisplay, nameBuff); // ต่อสตริง
        //stringToUnicodeAndSendToDisplay(bufferDisplay);
        stringToUnicodeAndSendToDisplayC(bufferDisplay, cc + 5);
        /*-----------------------
                  print file name here
                  -----------------------*/
        countKey = 0;
        keyCode = 0;
        seeCur = 0;
      }
    }
    d_Time++;
    if (d_Time >= delayCur) //blink cursor
    {
      //blink cu
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
  int asciT = 0;
  printf("\r\nkey B is (%d) \r\n", key);
  for (asciT = 0; asciT < 255 && asciT < sizeof(unicodeTable); asciT++) //วนหลูปตามจำนวนตาราง
  {
    if (key == unicodeTable[asciT]) //ถ้าค่าาที่เช็คมีในตาราง
    {
      asciT = asciT; //ให้ส่งตำแหน่งนั้นออก
      break;
    }
  }

  //printf("ascii is %c HEX:%x\r\n", asciT, asciT);
  return asciT;
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
      //cell_sentdata(~unicodeTable[((int)*(string + j))]); //send to display
      //printf(" %c unicode:%x ascii:%d lum: %d\r\n", *(string + j), unicodeTable[((int)*(string + j))], (int)*(string + j), j);
      if (unicodeTable[((int)(string[j]))] != 0 || string[j] == 32)
      {
        cell_sentdata(~unicodeTable[((int)*(string + j))]); //send to display
      }
      else if (string[j] == enterSign) //enter sign
      {
        cell_sentdata(~unicodeTable[32]);
      }
      else
      {
        cell_sentdata(((int)*(string + j)));
      }
    }
    else
    {
      cell_sentdata(0xff); // null dot send to display
    }
  }
  SPI_DISPLAY_CS_HIGH();
  //Delay(0x55F);
}
void stringToUnicodeAndSendToDisplayL(char *string, int ls)
{
  int strleng = 0;
  configDisplay();
  for (j = 0 + (20 * ls); j < 20 && string[j] != '\0' && string[j] != 0x0a; j++) //find length
    strleng++;
  SPI_DISPLAY_CS_LOW();
  for (j = 20; j >= 0; j--)
  {
    if (j < strleng)
    {
      if (unicodeTable[((int)(string[j]))] != 0 || string[j] == 32)
        cell_sentdata(~unicodeTable[((int)*(string + j))]); //send to display
      else if (string[j] == enterSign)                      //enter sign
        cell_sentdata(~unicodeTable[32]);                   //remove enter sign
      else
        cell_sentdata(((int)*(string + j)));
    }
    else
    {
      cell_sentdata(0xff); // null dot send to display
    }
  }
  SPI_DISPLAY_CS_HIGH();
}
void stringToUnicodeAndSendToDisplayC(char *string, int po)
{
  //cur position
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
    if (j == po) //ตำแหน่งตรงกับ cursor
    {

      cell_sentdata((~unicodeTable[((int)*(string + j))] & (~0xc0))); //curPostion
    }
    else if (j < strleng)
    {
      Delay(0x55F);
      //printf("as--%x", ((int)(string[j])));
      printf("hex:%d\r\n", string[j]);
      if (unicodeTable[((int)(string[j]))] != 0 || string[j] == 32)
      {
        cell_sentdata(~unicodeTable[((int)*(string + j))]); //send to display
      }
      else if (string[j] == enterSign) //enter sign
      {
        cell_sentdata(~unicodeTable[32]);
      }
      else
      {
        cell_sentdata(((int)*(string + j)));
      }
      //printf(" %c unicode:%x ascii:%d lum: %d\r\n", *(string + j), unicodeTable[((int)(string[j]))], (int)*(string + j), j);
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
  uint8_t jjr = 0;
  uint8_t FileLength222[] = {0x00, 0x57, 0xab, 0x3c, 0x00, 0x00};
  while (str___[jjr] != '\0')
  {
    jjr++;
  }
  if (jjr > 128)
  {
    jjr = 128;
  }
  FileLength222[4] = jjr;
  if (debug)
  {
    printf("\r\n str length in set file length:%d \r\n", FileLength222[4]);
  }
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
  {
    //FileLength

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
//------------------------write string to ch376s--------------------
//
//
////////////////////////////////////////////////////////////////////
int fileWrite2(int k, char *filename, char *string__)
{
  int status = 0;
  if (command_ == 4)
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
  {
    //FileLength

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
/*
void copy_string(char *target, char *source)
{
  while (*source)
  {
    *target = *source;
    source++;
    target++;
  }
  *target = '\0';
}*/
//----------------------write string to flash rom-------------------
//max address ‭0x1000000‬ ~16mbit || 16777216
//last sector at 0xffbf6a because	 minus with sector (4096 byte)
////////////////////////////////////////////////////////////////////
void writeFlash(int address)
{
  printf("-----------------------------------------------------\r\n");
  printf(">>>>>>%d address \r\n", address);
  printf("-----------------------------------------------------\r\n");
  printf("-----------------------------------------------------\r\n");
  printf("-----------------------------------------------------\r\n");
  SPI_DISPLAY_CS_LOW(); //
  configFlash();
  SPI_FLASH_CS_LOW();
  SST25_W_BLOCK(address, SST25_buffer, sector);
  SPI_FLASH_CS_HIGH();
}
void clearSector(int address)
{
  SPI_DISPLAY_CS_LOW();
  for (i = 0; i < 4096; i++)
    SST25_bufferCC[i] = '\0';
  configFlash();
  SPI_FLASH_CS_LOW();
  SST25_W_BLOCK(address, SST25_bufferCC, sector);
  SPI_FLASH_CS_HIGH();
}

/*---------------------------------------------------------------------------
// ฟังก์ชั่น set ชื่อไฟล์สำหรับ CH376
// ตั้งชื่อไฟล์ หรือโฟลเดอร์ที่จะเปิด อ้างอิงถึงคำสั่ง 0x57 0xab 0x2f .... ใสเอกสาร ch376
----------------------------------------------------------------------------*/
void setFilename(char *filename)
{
  //setFilenameForFunction //0x00 0x57 0xab 0x2f
  loovar = 0;

  SendCH370(setFilenameForFunction, sizeof(setFilenameForFunction));

  printf("file buffer :");
  while (filename[loovar] != '\0') // วนค่าส่งสำหรับเอา character จากตัวแปร filename เก็บทีละตัวอักษรฝากไว้กับตัวแปร temlLL
  {
    tempLL[loovar] = filename[loovar];
    printf("%c", tempLL[loovar]);
    loovar++;
  }
  /* printf("setfile 0buffer ");
  for (i = 0; i < sizeof(tempLL); i++)
  {
    printf("%c", tempLL[i]);
  }*/
  tempLL[loovar] = '\0';
  SendCH370(tempLL, sizeof(tempLL)); // ส่งไปยัง CH376s
}
/*-------------------------------create file in sd card --------------------------
//return status:1
//
*/

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
    // ex_cdWithPath(Dirpath);
    // delay_ms(300);
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
//---------------------------- fillter file name for display----------------------
// ฟังก์ชั่นสำหรับกรองชื่อไฟล์ หรือโฟลเดอร์ แต่ยังไม่เสถียร สำหรับ '.' และหากมีไฟล์ที่มีตัอัักษรเดียวกันกับ
// นามสกุลของไฟล์ จะเพี้ยนทันที
//////////////////////////////////////////////////////////////////////////////////
const char *fileName()
{
  int loop = 0;
  int count_char = 0;
  int seeT = 0, seeBr = 0, seeTx = 0;
  char DataForWrite_aftersort[32] = "";
  /*printf("data for write \r\n");
    for (i = 0; i < 32; i++)
    {
      printf("%d ", DataForWrite[i]);
    }*/

  for (loop = 0; loop < 32; loop++)
  {
    if (((int)DataForWrite[loop] >= 33 && (int)DataForWrite[loop] <= 126) && DataForWrite[loop] != 42 && loop < 12) // เอาส่วนที่มีชื่อไฟล์เก็บไว้ในตัวแปร [DataForWrite_aftersort]
    {
      //end of 12
      DataForWrite_aftersort[count_char] = DataForWrite[loop];
      count_char++;
    }
  }
  while (count_char < 32) // ทำอะไรไม่รู้ ลืม
  {
    DataForWrite_aftersort[count_char] = 0;
    count_char++;
  }
  count_char = 0;
  while (DataForWrite_aftersort[count_char] != '\0') // เช็คไฟล์ tbt
  {
    if (DataForWrite_aftersort[count_char] == 'T' && seeT == 0)
    {
      if (DataForWrite_aftersort[count_char + 1] == 'B' && DataForWrite_aftersort[count_char + 2] == 'T') // เริ่มต้นด้วย t ตามด้วย b และ t
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
      if (DataForWrite_aftersort[count_char + 1] == 'R' && DataForWrite_aftersort[count_char + 2] == 'F') // เริ่มต้นด้วย b ตามด้วย r และ f
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
      if (DataForWrite_aftersort[count_char + 1] == 'X' && DataForWrite_aftersort[count_char + 2] == 'T') // เริ่มต้นด้วย t ตามด้วย x และ t
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
    uart2Buffer = USART_ReceiveData(USART2); // อ่านค่าจาก serial 2
    if (uart2Buffer == 0xff && SeeHead == 0) // ทำการตัดค่าออก
    {
      SeeHead = 1;
      countKey = 0;
    }
    if (countKey == 2 && uart2Buffer == 0xa4) // ถ้า key ที่กดเป็น cursor
    {
      seeCur = 1;
    }
    if (countKey >= 4 && countKey <= 6) // ตัดค่าเอาเฉพาะตำแหน่ง 4 5 6 เก็บลงใน bufferkey3digit
    {
      bufferKey3digit[countKey - 4] = uart2Buffer;
    }
    if (countKey == 2) // checkKeyError
    {
      checkKeyError = uart2Buffer;
    }
    countKey++;
    if (countKey >= maxData)
    {
      // Recieve & checking key
      seeHead = 0;
      if (seeCur == 1) // ถ้าปุ่มที่กด เป็น cursor
      {
        printf("see cur \r\n");
      }
      else // ถ้าปุ่มที่กดเป็น key ทั่วไป
      {
        printf("See key %x,%x,%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
      }
      if (checkKeyError == 0xff) // check error key
      {

        countKey = 0;
        SeeHead = 0;
      }
      printf("current mode:%d\r\n", mode);
      //printf("%d\r\n",sizeof(mode_1)/sizeof(int));
      //////////////////////////////////menu selector ///////////////////////////////////
      //joy menu
      if (bufferKey3digit[1] != 0 || bufferKey3digit[2] != 0)
      {

        // ---------------------------- to key code -----------------------------
        if (bufferKey3digit[2] == 1 || bufferKey3digit[2] == 0x20) // joy is down
        {
          keyCode = 40; // arrow down mapping key code
        }
        else if (bufferKey3digit[1] == 1 || bufferKey3digit[1] == 3 || bufferKey3digit[1] == 2) // joy is up
        {
          keyCode = 55; // arrow up
        }
        else if (bufferKey3digit[1] == 128 || bufferKey3digit[2] == 0x10) // joy is up
        {
          keyCode = 38;
        }
        else if (bufferKey3digit[1] == 64 || bufferKey3digit[2] == 8) // enter
        {
          keyCode = 13;
        }
        else if (bufferKey3digit[1] == 32 || bufferKey3digit[2] == 4)
        {
          keyCode = 8; // backspace
        }
        else if (bufferKey3digit[1] == 4)
        {
          keyCode = 38; // left
        }
        else if (bufferKey3digit[1] == 8)
        {
          keyCode = 40; // right
        }

        if (bufferKey3digit[0] == 0x9f) // คือไร ลืม
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
    printDot(st_notepad, sizeof(st_notepad)); // notepad
    break;
  case 2:
    printDot(st_read, sizeof(st_read)); // read
    break;
  case 3:
    printDot(st_bluetooth, sizeof(st_bluetooth)); // bluetooth
    break;
  case 4:
    stringToUnicodeAndSendToDisplay("Tools"); // tools
    break;
  }
}

void printDot(uint8_t data[], uint8_t size)
{
  int tsize = size / sizeof(uint8_t);
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
    /*for (j = 0; j < 7; j++)
    {
      printf("%c", keyCodeRecieve[j]);
    }*/
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
  {
    //if serial available
    hexbuffer = USART_ReceiveData(UART4);
    printf("%c", hexbuffer);
    if (hexbuffer == 0xff && SeeHead == 0)
    {
      SeeHead = 1;
      count = 0;
    }
    if (count >= 23 && SeeHead == 1)
    {
      SeeHead = 0;
      pushToDisplay = 1;
    }
    BLEConBuffer[count] = hexbuffer;
    count++;
    if (count > 1 && BLEConn == 0)
    {
      //
      if (BLEConBuffer[0] == 0xff && BLEConBuffer[1] == 0xff && BLEConBuffer[2] == 0xa1 && BLEConn == 0)
      {
        //connect
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
  else // หลังจากเชื่อมต่อ
  {
    sendUart(1);
    if (pushToDisplay == 1)
    {
      SPI_DISPLAY_CS_LOW();
      Delay(0x55F);
      for (j = 20; j >= 0; j--)
      {
        cell_sentdata(0xff); // clear
      }
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
        {
          //------------------ send to braille display ----------------------
          hexPrep = BLEConBuffer[j];
          if (hexPrep != 0xff && hexPrep != 0xa3 && hexPrep != 0x14)
          {
            //ff ff 3a
            cell_sentdata(~hexPrep); // ~กลับบิต
          }
        }
      }
      SPI_DISPLAY_CS_HIGH();
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
  while (x < sizeof(connectData) / sizeof(uint8_t))
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

//---------------------------send command to ch376s---------------------------
// ส่งคำสั่งไปยัง ch376
//
//////////////////////////////////////////////////////////////////////////////
void SendCH370(uint8_t data[], uint8_t sizeOfData)
{
  int x = 0;
  while (x < sizeOfData / sizeof(uint8_t))
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

void sendUart(uint8_t data)
{
  sendUart1 = 0;
  sendUart2 = 0;
  sendUart3 = 0;
  sendUart4 = 0;
  switch (data)
  {
  case 1:
    sendUart1 = true;
    break;
  case 2:
    sendUart2 = true;
    break;
  case 3:
    sendUart3 = true;
    break;
  case 4:
    sendUart4 = true;
    break;
  default:
    sendUart1 = false;
    sendUart2 = false;
    sendUart3 = false;
    sendUart4 = false;
    break;
  }
}
//-------------------------------------GPIO------------------------------------
//
//-----------------------------------------------------------------------------

/*******************************************************************************
  Function Name  : GPIO_Configuration
  Description    : Configures the different GPIO ports.
  Input          : None
  Output         : None
  Return         : None
  Attention    : None
*******************************************************************************/
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
  if (sendUart1 == true)
  {
    USART_SendData(USART1, (uint8_t)ch);
  }
  if (sendUart2 == true)
  {
    USART_SendData(USART2, (uint8_t)ch);
  }
  if (sendUart3 == true)
  {
    USART_SendData(USART3, (uint8_t)ch);
  }
  if (sendUart4 == true)
  {
    USART_SendData(UART4, (uint8_t)ch);
  }
  //USART_SendData(UART4, (uint8_t) ch);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
  {
  }
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
