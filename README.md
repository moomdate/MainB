# Braille display


## Feature 
  - โหมดอ่าน
    -	รับรองไฟล์ tbt,brf,txt
    -	ไฟล์ tbt, brf ,txt จะต้องทำการแปลงเป็น unicode จากโปรแกรม BrailleConverter หรือ Duxburyหรือ tbt
    -	สามารถแสดงรายการไฟล์ใน Sd card สามารถเข้าโฟลเดอร์เพื่อดูรายการไฟล์ต่างๆได้
    -	รับรองไฟล์ 20 และ 40 ตัวอักษร (ในแค่ละบรรทัด)
    - ค้นหาไฟล์ TBT,BRF,TXT ใน SD-Card ได้
  
  - โหมดบลูทูธ
    - เชื่อมต่อกับอุปกรณ์ android ทำงานเป็น navigator 
    - Read text ด้วย Talkback และแสดงผ่านจุดเบรลล์
    - Keyboard Braille  
    - โหมดพิมพ์ เป็นแป้นพิมพ์อักษรเบรลล์

## การควบคุม
- โหมดอ่าน
  -	เลื่อนจอนขวา หรือ กด `Enter` เพื่อเข้าอ่านไฟล์ หรือโฟลเดอร์
  -	เลื่อนจอยซ้ายเพื่อออกจากโฟลเดอร์ หรือ กดเข้าโฟลเดอร์ `..` เพื่อย้อนกลับ
  -	เลื่อนจอยลงเพื่อเลือกไฟล์ถัดไป หรืออ่านข้อความบรรทัดถัดไป
  -	เลื่อนจอยขึ้นเพื่อเลือกไฟล์ก่อนหน้า หรืออ่านข้อความบรรทัดก่อนหน้า
  -	กดปุ่มขวาเพื่ออ่านข้อความในบรรทัด (ข้อความถัดไปในบรรทัด) 
  -	กดปุ่มซ้ายเพื่ออ่านข้อความในบรรทัด (ข้อความก่อนหน้าในบรรทัด) 
  -	`Space + e` เพื่อออกจากการอ่าน

## ที่กำลังทำ 
  - บันทึกไฟล์ที่พิมพ์ในโหมด notepad 
  - โหมด notepad เหลือทำให้รับรองความยาวเกิน 4096 และเอาไปใส่ในรอม
## ✖ ที่ต้องแก้ไข 
  - ความยาวในโหมดพิมพ์ยังไม่เกิน 97*40 ตัวอักษร ต้องเอาเก็บไว้ในรอม
  - ยังไม่ได้รวมบันทึกไฟล์
  - ไม่มีฟังก์ชั่นจัดการไฟล์
  - สระภาษาไทยยังไม่ได้ทดสอบ
  - เจอบัคในโหมดพิมพ์ แวบๆ 
  - ในโหมดอ่านไฟล์ หากบรรทัดก่อนหน้าเว้บว่าง หรือเป็นเครื่องหมาย line feed \r\n จะไม่สามารถย้อนกลับได้ต้องแก้ไขที่อัลกอริทึม
## 🗹 ปัญหาที่แก้ไขแล้ว  
  - ก-ฮ แสดงถูกต้อง
  - a-z แสดงถูกต้อง 
  - ปรับความเร็ว Serial3  เป็น 115200
  - แก้อาการค้างเมื่อค้นหาไฟล์
  - ย้อนกลับจากโหมดอ่านไม่ได้ ต้องรีเซ็ท
  - ยังเปิดและดูไฟล์ในโฟลเดอร์ไม่ได้
  - ไม่สามารถปรับ baud rate จาก 9600 115200 ขณะรันโปรแกรม ใน STM32 ได้
  - อ่านไฟล์ช้ามากๆ
  - Refresh dot ช้าา
  - อ่านไฟล์ sector ถัดไป Charator>4096 
  - เลื่อนบรรทัด ไป/กลับ (ไม่เกิน 4096 ตัวอักษร)
#### Serial config 
```c
  //9600 connect to bluetooth module
  UART4_Configuration(); 
  //115200 connect with keyboard
  USART2_Configuration(); 
  //115200 defualt serial
  USART1_Configuration();
  //115200 connect to ch376 (sd card module)
  USART3_Configuration();
```

#### Output Function
```c
   //ส่ง Braille unicode ไปแสดงที่จุดเบรลล์
   printDot(brailleUnicodeByte, sizeof(brailleUnicodeByte)); 
   // ส่งสตริงไปแสดงที่จุดเบรลล์
   stringToUnicodeAndSendToDisplay("String here"); 
    // ส่งสตริงยาว 40 ตัวอักษร params(สตริงยาว<=40, 0 หรือ 1)
   printStringLR("String", 0);
    //beep sound 
    ส่งออก Serial2 
```
Serial 2 command [Docs here](https://github.com/moomdate/Bkeyboard/blob/master/readme.md)
#### Keyboard Input 
```c
  //อ่านคีย์บอร์ด
  notepad_readKey(); 
  //เมื่ออ่านครบแล้วให้ทำใน if
  if (countKey >= maxData)
  { 
    //reset ค่าของ Event
    seeHead = 0;
    //แสดง Key
    printf("See key %x,%x,%x\r\n", bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
    // ดักจับเออเร่อ
    if (checkKeyError == 0xff) // error catch 
    { 
      countKey = 0;
      SeeHead = 0;
    }
    // คีย์ที่ได้ Keycode
    keyCode = keyMapping(bufferKey3digit[0], bufferKey3digit[1], bufferKey3digit[2]);
    printf("keycode:%d\r\n", keyCode);
    // Clear ค่า
    clearKeyValue();
  }
```
#### Mode (in void main)
###### Bluetooth Mode
```c
  notepad();
```
###### Bluetooth Mode
```c
   while (mode == 5) { // while in bluetooth mode
      BluetoothMode(); // 
      // keyboardMode();
      if (becon == 0) { 
        menu_s();
      }
      else {
        keyboardMode();  //connected use keyboard for bluetooth
      }
    }
```
###### Read Mode
```c
    while (mode == 3 && openFileStatus == 0) { //if enter to mode 3 search file and display
      searchFile();
    }
    while (mode == 3 && openFileStatus == 1) { //if open file 
      ReadFile();
    }
```




