# ล่าสุด


##### ที่ทำได้ :satisfied: :wink:

  - ค้นหาไฟล์ TBT ใน SD Card
  - เปิดอ่านไฟล์ TBT 
  - เลื่อนบรรทัด ไป/กลับ (ไม่เกิน 4096 ตัวอักษร)
  - โหมดบลูทูธ
    - เชื่อมต่อกับอุปกรณ์ android ทำงานเป็น navigator 
    - Read text ด้วย Talkback และแสดงผ่านจุดเบรลล์
    - Keyboard Braille  
##### ที่กำลังทำ :open_mouth: :sleeping:
  - อ่านไฟล์ sector ถัดไป Charator>4096 
  - ย้อนกลับจากโหมดอ่านไม่ได้ ต้องรีเซ็ท
  - โหมดพิมพ์ (เหมือน notepad)
  - ยังเปิดและดูไฟล์ในโฟลเดอร์ไม่ได้
#### ปัญหา :grimacing: :rage: :thumbsdown:
  - ไม่สามารถปรับ baud rate จาก 9600 115200 ขณะรันโปรแกรม ใน STM32 ได้
  - อ่านไฟล์ช้ามากๆ
  - Refresh dot ช้าาาามากๆๆๆๆๆ 
  



#### function 
```c
  UART4_Configuration(); //9600
  USART2_Configuration(); //115200
  USART1_Configuration(); //115200
  USART3_Configuration(); //9600
```



####  -------ตายๆๆๆๆ-----------------30/ มิถุนา------------------
:cold_sweat: :neutral_face: :neutral_face::neutral_face::neutral_face: