# Log
##  May 27, 2018
 1. เอาฟังก์ชั่นแสดงขนาดไฟล์มาใช้กับปุ่ม Left,Right
 2. ปรับฟังก์ชั่นแสดงขนาดไฟล์ให้ย่อความยาวไฟล์แสดงเป็นหน่วย เช่น Kilo bytes
 3. นำฟังก์ชั่นไปปรับใช้แล้ว
 ###### #ปัญหา จากการทดสอบไฟล์บางไฟล์ในโฟลเดอร์ยังมีการแสดงขนาดไฟล์ผิดพลาด ยังไม่ทราบสาเหตุ
##  May 28, 2018
 1. แก้ไขคีย์ Space bar
 2. แก้ไข `Key code` บางปุ่มให้ดูเข้าใจง่ายขึ้น
 3. แก้ไข Buffer Line ของ โหมด Notepad จาก 102 เป็น 204

 ###### #ปัญหา จากการแก้ไขขนาดของBuffer ส่งผลถึงฟังก์ชั่นการบันทึกไฟล์โดยมีขนาดไฟล์ที่กำหนดไว้ยังไม่เกิด 4096 โดยสามารถปรับแก้ไขได้ในส่วนนี้

##  May 29, 2019

  1. สร้างฟังก์ชั่นสำหรับจัดการ Mark รันบน Dev c++ สำหรับทดสอบและเตรียมนำไปปรับใช้กับโค้ดหลัก 
```c
void searchName(char * fileName); // search file
void filterPageToArray(int pS,int pE); // data string to num array
int getEndSign(int startAt); //get position of end string
int MarkerPage[MarkMaxBufferPage]; // buffer
void printfPageOfMarker(); // print lst
int findIndexOfArray(int target); // find index of line
int RemoveArrayAtIndex(int position);
int findIndexForInsert(int value); 
int insertMark(int);
int sizeOfList();
```
ยังขาดฟังก์ชั่นเก็บการตั้งค่าลง ROM และฟังก์ชั่นการอ่าน

 - [https://gist.github.com/moomdate/9bf238ccb9f6307d56d4d316e07ad2e2](https://gist.github.com/moomdate/9bf238ccb9f6307d56d4d316e07ad2e2)

## May 30, 2019
  1. เพิ่มฟังก์ชั่น Replace string โดยจะแปลงข้อมูล `ชื่อไฟล์` `ข้อมูลบรรทัด` ที่ทำการเปลี่ยนแปลงค่าแล้ว ทำการแปลงกลับเป็นสตริง และเขียนทับด้วยข้อมูลใน สำหรับเขียนลงROM ต่อไป 
  2. เพิ่มฟังก์ชั่น `insertLine()`  
  ```c
  char *str_replace(char *orig, char *rep, char *with);
  ```
## May 31, 2019
  1. เพิ่มฟังก์ชั่น `int CreateMarkFileConfig(char * filename)` สำหรับเพิ่มชื่อไฟล์ใหม่สำหรับเก็บตำแหน่งของ mark โดยจะไม่สร้างชื่อไฟล์ซ้ำกัน 
[https://gist.github.com/moomdate/21493fa674fd59244e16adb14f5a6476](https://gist.github.com/moomdate/21493fa674fd59244e16adb14f5a6476)