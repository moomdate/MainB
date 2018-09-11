int readmode_countLineInOneSector(char *str)
{
  int count = 0;
  int sum = 0;
  while (str[count] != '\0' && count < 4096)
  {
    if (str[count] == 0x0d)
    {
      if (count != 4096 - 1)
      {
        if (str[count + 1] == 0x0a)
        {
          sum++;
        }
      }
    }
    count++;
  }
  return sum;
}
