#include <iostream>
#include "vm_app.h"

using namespace std;

int main()
{
  char *p;
  //cout << "hi" << endl;
  p = (char *) vm_extend();
  //cout << "got here" << endl;
  p[0] = 'h';
  p[1] = 'e';
  p[2] = 'l';
  p[3] = 'l';
  p[4] = 'o';
}
