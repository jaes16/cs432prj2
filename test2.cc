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
  cout << "hi" << endl;
}
