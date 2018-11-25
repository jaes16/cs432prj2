#include <iostream>
#include "vm_app.h"

using namespace std;

int main()
{
  cout << "hi" << endl;
  char *p;
  p = (char *) vm_extend();
  p[0] = 'h';
  for(int i = 0; i < 10; i++){
    char *q = (char *) vm_extend();

  }
  vm_syslog((void *) p, 81900);
}
