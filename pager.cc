#include "vm_pager.h"
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <string.h>
#include <sys/types.h>

using namespace std;



typedef struct {
  page_table_entry_t *pte;
  int diskblock;
  int pmem;
  int dirty;
  int referenced;
  int zeroed;
  int valid;
  int read = 0;
  int write = 0;

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
    pr->ptbr.ptes[i].ppage = (unsigned long) -1;
  }
  
  //add to list of processes
  processList.insert(pair <pid_t,process*>(pid,pr));
}

void vm_switch(pid_t pid){
  // switch pointers
  current_process = processList[pid];
  page_table_base_register = &(current_process->ptbr);

}

int vm_fault(void *addr, bool write_flag){
  // get the vpage at address
  int virtAddr = (int)((unsigned long)addr - (unsigned long)VM_ARENA_BASEADDR) / VM_PAGESIZE;

  // see if this is a valid virtual page
  if(virtAddr >= current_process->vpages.size()){
    return -1;
  }
  // if it is a valid vpage
  vpage *vp = current_process->vpages[virtAddr];
  
  // check read_enable - if 0, we know not in pmem, so we should bring it into pmem
  if(current_process->ptbr.ptes[virtAddr].read_enable == 0){
    // check if there is space in physical memory
    if(m_pages.empty()){
      return -1;
    }
    // if there is space in physical memory, allocate space
    vp->pte->ppage = m_pages.top();
    current_process->ptbr.ptes[virtAddr].ppage = m_pages.top();
    // flush physical page
    memset((void*)((unsigned long)pm_physmem+((unsigned long)VM_PAGESIZE*m_pages.top())), 0, VM_PAGESIZE);
    
    vp->pmem = m_pages.top();
    m_pages.pop();
    vp->pte->read_enable = 1;
    current_process->ptbr.ptes[virtAddr].read_enable = 1;
    vp->read = 1;
    vp->valid = 1;
    vp->zeroed = 1;
    vp->referenced = 1;
  }
  if(write_flag){
    vp->pte->write_enable = 1;
    current_process->ptbr.ptes[virtAddr].write_enable = 1;
    vp->pte->read_enable = 1;
    current_process->ptbr.ptes[virtAddr].read_enable = 1;
    vp->read = 1;
    vp->write = 1;
  }
  return 0;
}


void vm_destroy(){
  // open up all of the ppages and diskblocks
  /*  for(int i =0; i < current_process->num_vpages; i++){
    d_blocks.push(current_process->vpages[i]->diskblock);
    m_pages.push(current_process->vpages[i]->pte->ppage);
    
    delete current_process->vpages[i];
  }
  // remove process from list of processors, then delete
  processList.erase(current_process->pid);
  delete current_process;*/
  return;
}

void * vm_extend(){
  //cout << "mah nutz" << endl;
  //currently assuming that we have enough space in pmem
  if(!d_blocks.empty()){
    vpage *vp = new vpage;
    vp->diskblock = d_blocks.top();
    d_blocks.pop();
    vp->valid = 0;
    vp->zeroed = 0;
    vp->dirty = 0;
    int count = 0;

    //finding the next invalid page address
    for(int i = 0; i < VM_ARENA_SIZE/VM_PAGESIZE; i++){
      if(count < 1){
	count++;
      }
      if((int) page_table_base_register->ptes[i].ppage == -1){
	page_table_base_register->ptes[i].ppage = 2147483647; //INT_MAX
	vp->pte = &(page_table_base_register->ptes[i]);
	
	//adding the virtual page to the current process' vpage map
	current_process->vpages.insert(pair <int, vpage*>(current_process->num_vpages, vp));
	return (void*) (((unsigned long)current_process->num_vpages++ * VM_PAGESIZE) + ((unsigned long) VM_ARENA_BASEADDR));
	
      }
    }
    //if there aren't anymore empty pages in pagetable
    return (void*)-1;
  }
  else {
    //no diskblock to back it up
    return (void*)-1;
  }
}

int vm_syslog(void *message, unsigned int len){

}


