#include <iostream>
#include "vm_app.h"

using namespace std;

int main()
{
  cout << "hi" << endl;
  char *p;
  p = (char *) vm_extend();
  p[0] = 'h';
  char c = p[0];
  p[1] = 'e';
  p[2] = 'l';
  p[3] = 'l';
  p[4] = 'o';
  char *x = (char *) vm_extend();
  x[8000] = 'a';
  char *t = (char *) vm_extend();
  t[1] = 'a';
  char *s = (char *) vm_extend();
  s[0] = 'i';
  vm_syslog(x,8200);
  vm_syslog(p,8200);
  vm_syslog(p,25000);
  vm_syslog(t-1,1);
  x[0] = 'h';
  vm_syslog(s-(0x6000),(0x6000));
  vm_syslog(p-10,234);
  x[0] = 'a';
  t[0] = 'a';
  vm_syslog(s+10000, 312314);
}
