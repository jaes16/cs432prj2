#include <iostream>
#include "vm_app.h"

using namespace std;

int main()
{
  char *p;
  p = (char *) vm_extend();
  p[0] = 'h';
  char *q;
  for(int i = 0; i < 2; i++){
    q = (char *) vm_extend();
    q[0] = 'h';
  }
  char c = p[0];
  c = q[0];
  c = ((char *) q-8000)[0];
  c = p[0];
  c = q[0];
}
