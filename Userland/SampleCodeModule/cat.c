#include "cat.h"

int cat(int argc, char *argv[])
{
      if (argc != 1)
      {
            printError("Cantidad de parametros incorrecto.\n");
            return -1;
      }

      int c;
      while ((c = getChar()) != -1){
            putChar(c);
      }
      return 1;
}