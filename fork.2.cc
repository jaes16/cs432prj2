#include "vm_app.h"
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

int main()
{
  
  
  char *p = (char *) vm_extend();
  char *q = (char *) vm_extend();
  p[0] = 'h';
  q[0] = 'i';

  pid_t pid = vfork();

  if(pid == 0){
    char *j = (char *) vm_extend();
    char *k = (char *) vm_extend();
    j[0] = 'h';
    k[0] = 'i';
    return 0;
  }
  else if(pid > 0){
    sleep(100);
    p[0] = 'i';
    q[0] = 'h';
  } else {
    
  }

  return 0;
  
}

