#include <iostream>
#include "vm_app.h"

using namespace std;

int main()
{
  char *p;
  p = (char *) vm_extend();
  p[0] = 'h';
  char *q1 = (char *) vm_extend();
  q1[0] = 'h';
  char *q2 = (char *) vm_extend();
  q2[0] = 'h';
  vm_syslog(p,1);
  p[1] = 'i';
  char *p2 = (char *) vm_extend();
  char c = p2[0];
  vm_syslog(p2,1);
  vm_syslog(q1-8100,8200);
  vm_syslog(q2-9000,9010);
  vm_syslog(p, 16390);
  vm_syslog(q2,20000);
  vm_syslog(q1,1);
  vm_syslog(q2,1);
  q1[0] = 'h';
  c = q1[0];
  c = p[0];
  p2[0] = 'i';
  vm_syslog(q2,8200);
  vm_syslog(p-100,1000);
  vm_syslog(p2+20100101,100);
  vm_syslog(q1,1);
  vm_syslog(p,1);
  vm_syslog(q2,1);
  vm_syslog(p2,1);
  c =  p[2];
}
