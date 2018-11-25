#include <iostream>
#include "vm_app.h"

using namespace std;

int main()
{
  char *p;
  p = (char *) vm_extend();
  p[0] = 'h';
  char *q1 = (char *) vm_extend();
  char c = q1[0];
  char *q2 = (char *) vm_extend();
  c = q2[0];
  c = p[0];
  c = q1[0];
  c = q2[0];
  ///////// test 8
  cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
  cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
  p = (char *) vm_extend();
  p[0] = 'h';
  q1 = (char *) vm_extend();
  c = q1[0];
  q2 = (char *) vm_extend();
  c = q2[0];
  p[0] = 'h';
  c = q1[0];
  c = q2[0];
}
