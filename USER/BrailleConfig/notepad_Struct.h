typedef struct notepad
{
  uint8_t cursorPosition;
  uint8_t multiplyCursor;
  bool displayFirst;
  int currentLine;
  char buffer_string[notepad_Line][notepad_MaxinLine];
} notepad;