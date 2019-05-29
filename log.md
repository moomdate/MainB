# Log
##  May 29,2019

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
`
ยังขาดฟังก์ชั่นเก็บการตั้งค่าลง ROM และฟังก์ชั่นการอ่าน
`
 - [https://gist.github.com/moomdate/9bf238ccb9f6307d56d4d316e07ad2e2](https://gist.github.com/moomdate/9bf238ccb9f6307d56d4d316e07ad2e2)
