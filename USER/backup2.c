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
          pointer22char = 1;
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
    // printf("//sector: %d //send: %d-- %s -\r\n", pointerSector, pointer22char, buffer22Char);
    printf("==============================\r\n");
    printf("string:%s\r\n", buffer22Char);
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
  if (keyCode == 27)
  { //exit

    endReadFile = 0;
    mode = 0;
    readFileStatus___ = 0;
    SendCH370(ResetAll, sizeof(ResetAll));
    delay_ms(45);
    stringToUnicodeAndSendToDisplay("Exit...");
    //printf("exit-------------\r\n");
  }
}
