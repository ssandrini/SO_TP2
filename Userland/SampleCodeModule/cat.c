// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "cat.h"

void cat(int argc, char *argv[])
{
      if (argc != 1)
      {
            printError("Cantidad de parametros incorrecto.\n");
            return;
      }

      int c;
      while ((c = getChar()) != -1)
      {
            putChar(c);
      }
      putChar(c);
      return;
}