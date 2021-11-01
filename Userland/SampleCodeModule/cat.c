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