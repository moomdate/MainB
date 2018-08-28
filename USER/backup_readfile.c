int readFileFromCH376sToFlashRom(char *fileName___)
{
  readFileStatus___ = 1;
  //stringToUnicodeAndSendToDisplay("Reading....");
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
          printStringLR(buffer22Char, 0);
          AmountSector = addressWriteFlashTemp / sector;  //---- จำนวน sector ----
          AmountSectorT = addressWriteFlashTemp % sector; //---- เศษ ที่เหลือของ sector ---
          //-----------------------------------display string 20 charactor -----------------------------------
          //
          ////////////////////////////////////////////////////////////////////////////////////////////////////
          while (endReadFile == 1)
          { // query string-
            // menu_s();
            keyRead();
            //readFileStatus___
          }
        } // ทดสอบ
      }
    }
  }
  return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
          convert_textto_buffer(SST25_buffer99);
          printf("======================\r\n value:%s",notepad_buffer_string[0]);
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
          printStringLR(buffer22Char, 0);
          AmountSector = addressWriteFlashTemp / sector;  //---- จำนวน sector ----
          AmountSectorT = addressWriteFlashTemp % sector; //---- เศษ ที่เหลือของ sector ---
          //-----------------------------------display string 20 charactor -----------------------------------
          //
          ////////////////////////////////////////////////////////////////////////////////////////////////////
          while (endReadFile == 1)
          { // query string-
            // menu_s();
            keyRead();
            //readFileStatus___
          }
        } // ทดสอบ
      }
    }

    //////////////////
    ///////////////
    /////////////////
    if ((keyCode == 38 || keyCode == 40) && endReadFile == 1)
  {
    // printf("%s", notepad_buffer_string[3]);
    if (keyCode == 40)
    {
      notepad_currentLine++;
      /* if (ccMain >= sector)
      {
        printf("Change sector\r\n");
        SPI_FLASH_CS_LOW();
        SST25_R_BLOCK(ccSector, SST25_buffer99, sector);
        SPI_FLASH_CS_HIGH();
        ccSector++;
        Delay(0xffff);
        ccMain = 0;
      }*/
    }
    else if (keyCode == 38)
    {
      notepad_currentLine--;
    }
    if (notepad_currentLine >= readmode_MaxLineBuffer)
    {
      printf("Read next----------------\r\n");
      notepad_currentLine = 0;
      memset(readmode_bufferStr, 0, sizeof(readmode_bufferStr[0][0]) * readmode_maxsizeInLine * readmode_maxsizeInLine);
      convert_textto_buffer(SST25_buffer99);
      cab++;
    }
    read_mode_contTextReaded += strlen(readmode_bufferStr[notepad_currentLine]);
    printf("%s\r\n", readmode_bufferStr[notepad_currentLine]);
    printf("====================================\r\n");
    printf("line:(%d)text size in buffer:%d \r\nmax text:(%d) text readed:(%d)\r\n", notepad_currentLine, read_mode_currentIndex_after, addressWriteFlashTemp, read_mode_contTextReaded);
    printf("====================================\r\n");
  }