#include <iostream>
#include "vm_app.h"

using namespace std;

int main()
{
  char *p;
  p = (char *) vm_extend();
  char c = p[0];
  /*p[0] = 'h';
  p[1] = 'e';
  p[2] = 'l';
  p[3] = 'l';
  p[4] = 'o';
  */
  p = (char *) vm_extend();
  c = p[0];
  //c = p[8000];
  //p[8000] = 'a';
  p = (char *) vm_extend();
  p = (char *) vm_extend();
  p = (char *) vm_extend();
  ((char*)(p-8000))[0] = 'a';
  c = p[0];
  //c = p[13000];
}
