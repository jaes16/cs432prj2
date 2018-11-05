#include "vm_pager.h"
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <sys/types.h>

using namespace std;



typedef struct {
  unsigned long ppage;
  int diskblock;
  int dirty;
  int referenced;
  int zeroed;
  int first;
  

}vpage;


typedef struct {
  pid_t pid;
  page_table_t ptbr;
  map <int, *vpage> vpages;
} process;



//keeping track of state of pmemory
static vector <int> disk_blocks;
static vector <int> memory_pages;

//list of processes
static map < pid_t, *process > processList;

//current process and page table
static pid_t cur_pid;



void vm_init(unsigned int memory_pages, unsigned int disk_blocks){

  // to keep track of state of physical memory
  for (int i = 0; i < memory_pages; i++){
    memory_pages[i] = i;
  }
  for (int i = 0; i < disk_blocks; i++){
    disk_blocks[i] = i;
  }
}

void vm_create(pid_t pid){

  //create new process
  process *pr = new process;
  pr->pid = pid;
  pr->ptbr = page_table_t;

  //for loop to set r/w bits to zero, ppage to -1 for all ppages
  
  //add to list of processes
  processList.insert(pair <pid_t,*process>(pid,pr));
    
}

void vm_switch(pid_t pid){

  
  


}

int vm_fault(void *addr, bool write_flag){

}

void vm_destroy(){
  
}

void * vm_extend(){

  //currently assuming that we have enough space in pmem
  for(int i = 0; i < memory_pages.size(); i++){
    
  }
  
}

int vm_syslog(void *message, unsigned int len){

}


