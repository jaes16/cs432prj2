#include "vm_pager.h"
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <sys/types.h>

using namespace std;



typedef struct {
  unsigned long ppage;
  int diskblock;
  int dirty;
  int referenced;
  int zeroed;
  int first;

} vpage;


typedef struct {
  pid_t pid;
  page_table_t ptbr;
  map <int, vpage*> vpages;
  int num_vpages;
} process;



//keeping track of state of pmemory
static stack<int> d_blocks;
static stack<int> m_pages;

//list of processes
static map < pid_t, process*> processList;

//current process and page table
static process *current_process;



void vm_init(unsigned int memory_pages, unsigned int disk_blocks){

  // to keep track of state of physical memory
  for (int i = 0; i < memory_pages; i++){
    m_pages.push(i);
  }
  for (int i = 0; i < disk_blocks; i++){
    d_blocks.push(i);
  }
}

void vm_create(pid_t pid){

  //create new process
  process *pr = new process;
  pr->pid = pid;
  pr->num_vpages = 0;

  //for loop to set r/w bits to zero, ppage to -1 for all ppages
  for(int i = 0; i < VM_ARENA_SIZE/VM_PAGESIZE; i++){
    pr->ptbr.ptes[i].read_enable = 0;
    pr->ptbr.ptes[i].write_enable = 0;
    pr->ptbr.ptes[i].ppage = -1;
  }
  
  //add to list of processes
  processList.insert(pair <pid_t,process*>(pid,pr));
  
}

void vm_switch(pid_t pid){

  
  


}

int vm_fault(void *addr, bool write_flag){
  int ourAddr = (long)(addr - (long)VM_ARENA_BASEADDR) / VM_PAGESIZE;
  vpage *vp = current_process->vpages[ourAddr];
  
  if(vp->first){
    
  }
}

void vm_destroy(){
  
}

void * vm_extend(){
  
  //currently assuming that we have enough space in pmem
  if(!d_blocks.empty()){
    
    vpage *vp = new vpage;
    vp->diskblock = d_blocks.top();
    d_blocks.pop();
    vp->first = 1;
    vp->zeroed = 0;
    vp->dirty = 0;

    //finding the next invalid page address
    for(int i = 0; i < VM_ARENA_SIZE/VM_PAGESIZE; i++){
      if(page_table_base_register->ptes[i].ppage == -1){
	vp->ppage = page_table_base_register->ptes[i].ppage;
	
	//adding the virtual page to the current process' vpage map
	current_process->vpages.insert(pair <int, vpage*>(current_process->num_vpages++, vp));
	return (void*) (current_process->num_vpages * VM_PAGESIZE + VM_ARENA_BASEADDR);
      }
    }
    return (void*)-1;
  }
  else {
    return (void*)-1;
  }
}

int vm_syslog(void *message, unsigned int len){

}


