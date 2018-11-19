#include <iostream>
#include "vm_app.h"

using namespace std;


int main(){
  char *a = (char *) vm_extend();
  a[0] = 'y';
  a[1] = 'u';
  a[2] = 'x';
  a[3] = 'i';
  a[4] = 'n';

  char *b = (char *) vm_extend();
  char *c = (char *) vm_extend();

  for (int i = 0; i < 5; i++){
    b[i] = a[i];
    c[i] = b[i];
  }

  char *d = (char *) vm_extend();
  char *e = (char *) vm_extend();

  for (int i = 0; i < 5; i++){
    d[i] = c[i];
    e[i] = d[i];
  }

  vm_syslog(a, 5);
  vm_syslog(b, 5);
  vm_syslog(c, 1);
  vm_syslog(d, 5);
  vm_syslog(e, 5);

  vm_syslog(a, 9000);
  vm_syslog(b - 9999, 9999);
  vm_syslog(c - 8192, 9000);
  vm_syslog(d - 8190, 10);
  vm_syslog(e - 10, 10);
  vm_syslog(a - 1, 1);
}
