#include <iostream>
#include "vm_app.h"

using namespace std;

int main()
{
  char *p;
  for(int i = 0; i < 10000 ; i++){
    p = (char *) vm_extend();
    if(p == NULL){
      cout << "----------------hi---------------" << endl;
      break;
    }
    p[0] = 'i';
    if(i > 0){
      (p-8000)[0] = 'h';
    }
  }
}
