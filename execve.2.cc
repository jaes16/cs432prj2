#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vm_app.h"
int
main()
{
  const char *newargv[] = { NULL };
  const char *newenviron[] = { NULL };
  
  char *p = (char *) vm_extend();
  char *q = (char *) vm_extend();
  p[0] = 'h';
  q[0] = 'i';
  
  execl("./test78");//, newargv, newenviron);

}
