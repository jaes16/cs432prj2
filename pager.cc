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
  int resident = -1;
  int zeroed;
  int valid;
  int read = 0;
  int write = 0;
  pid_t pid;
} vpage;


typedef struct {
  pid_t pid;
  page_table_t ptbr;
  map < int, vpage* > vpages;
  int num_vpages;
} process;



//keeping track of state of pmemory
static stack<int> d_blocks;
static stack<int> m_pages;

//list of processes
static map < pid_t, process*> process_list;

//list of vpages in physical memory
static queue<vpage*> phys_vpages;

//current process and page table
static process *current_process;

//clock hand
static int clock_hand = 0;



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
  process_list.insert(pair <pid_t,process*>(pid,pr));
}

void vm_switch(pid_t pid){
  // switch pointers
  current_process = process_list[pid];
  page_table_base_register = &(current_process->ptbr);

}

//evicts a page and returns the physical page of what was evicted
unsigned long clock_alg(){
  
  bool toEvictFound = false;
  unsigned long physPage = 0;
  //cout << "current process: " << current_process << endl;
  while(!toEvictFound){
    
    //if the vp has been referenced, set reference bit to 0 and continue
    if(phys_vpages.front()->referenced == 1){
      phys_vpages.front()->referenced = 0;
      phys_vpages.front()->pte->read_enable = 0;
      phys_vpages.front()->pte->write_enable = 0;
      phys_vpages.push(phys_vpages.front());
      phys_vpages.pop();
    }
    else { //if not, this is the page to evict
      toEvictFound = true;
      //set this page's ppage to be the evictee's ppage, and set the evictee's resident bit to 0
      physPage = phys_vpages.front()->pte->ppage;
      phys_vpages.front()->resident = 0;
      //write evictee out to disk, unless it hasn't been changed or is zeroed
      if(phys_vpages.front()->dirty == 1){
	disk_write(phys_vpages.front()->diskblock, phys_vpages.front()->pte->ppage);
	phys_vpages.front()->dirty = 0;
      }
      phys_vpages.front()->pte->read_enable = 0;
      phys_vpages.front()->pte->write_enable = 0;
      phys_vpages.front()->read = 0;
      phys_vpages.front()->write = 0;
      phys_vpages.pop();
      break;
    }
  }
  return physPage;
}


int vm_fault(void *addr, bool write_flag){
  // get the vpage at address
  int vpNum = (int)((unsigned long)addr - (unsigned long)VM_ARENA_BASEADDR) / VM_PAGESIZE;

  // see if this is a valid virtual page
  if(vpNum >= current_process->vpages.size()){
    return -1;
  }
  // if it is a valid vpage...
  vpage *vp = current_process->vpages[vpNum];
  
  // check read_enable - if 0, we know not in pmem, so we should bring it into pmem
  if(vp->pte->read_enable == 0){

    //if the page is resident, we still need to set its write bit to what it was in case we did something in clock
    if(vp->resident == 1){
      vp->read = 1;
      vp->valid = 1;
      vp->referenced = 1;
      vp->pte->read_enable = 1;
      vp->pte->write_enable = vp->write;
      if(write_flag){
	vp->write = 1;
	vp->pte->write_enable = 1;
	vp->dirty = 1;
	vp->zeroed = 0;
      }
      return 0;
    }

    // then we know its not in physical memory
    
    // check if physical memory is full
    if(m_pages.empty()){
      //run clock algorithm on virtual pages
      vp->pte->ppage = clock_alg();
      vp->pmem = vp->pte->ppage;
    }
    else {
      // if there is space in physical memory, allocate space
      vp->pte->ppage = m_pages.top();
      vp->pmem = m_pages.top();
      m_pages.pop();
    }

    // if it should be zero filled, zero fill physical page
    if(vp->zeroed == 1){
      memset((void*)((unsigned long)pm_physmem + ((unsigned long)VM_PAGESIZE * vp->pte->ppage)), 0, VM_PAGESIZE);
    }
    // otherwise it must be read in from disk
    else {
      disk_read(vp->diskblock, vp->pte->ppage);
    }
    // its readable and referenced, resident, not dirty yet, and only zeroes for now
    vp->pte->read_enable = 1;
    vp->read = 1;
    vp->valid = 1;
    vp->referenced = 1;
    vp->resident = 1;
    vp->dirty = 0;

    //insert page into queue of pages in physical memory
    phys_vpages.push(vp);


    // if we removed from physical memory and need to bring it back from disk
  }
  // if we are writing to this page, make it write enable and dirty
  if(write_flag){
    vp->pte->write_enable = 1;
    vp->pte->read_enable = 1;
    vp->read = 1;
    vp->write = 1;
    vp->dirty = 1;
    vp->zeroed = 0;
    vp->referenced = 1;
    vp->resident = 1;
  }
  return 0;
}
/*

int vm_fault(void *addr, bool write_flag){
  // get the vpage at address
  int vpNum = (int)((unsigned long)addr - (unsigned long)VM_ARENA_BASEADDR) / VM_PAGESIZE;

  // see if this is a valid virtual page
  if(vpNum >= current_process->vpages.size()){
    return -1;
  }
  // if it is a valid vpage...
  vpage *vp = current_process->vpages[vpNum];
  
  // check read_enable - if 0, we know not in pmem, so we should bring it into pmem
  if(vp->pte->read_enable == 0){

    //if the page is resident, we still need to set its write bit to what it was in case we did something in clock
    if(vp->resident == 1){
      vp->read = 1;
      vp->valid = 1;
      vp->referenced = 1;
      vp->resident = 1;
      vp->pte->read_enable = 1;
      vp->pte->write_enable = vp->write;
      if(write_flag){
	vp->write = 1;
	vp->pte->write_enable = 1;
	vp->dirty = 1;
	vp->zeroed = 0;
      }
      return 0;
    }

    // then we know its not in physical memory
    
    // check if physical memory is full
    if(m_pages.empty()){
      //run clock algorithm on virtual pages
      vp->pte->ppage = clock_alg();
      vp->pmem = vp->pte->ppage;
    }
    else {
      // if there is space in physical memory, allocate space
      vp->pte->ppage = m_pages.top();
      vp->pmem = m_pages.top();
      m_pages.pop();
    }
    // zero fill physical page
    memset((void*)((unsigned long)pm_physmem + ((unsigned long)VM_PAGESIZE * vp->pte->ppage)), 0, VM_PAGESIZE);
    // if we removed from physical memory and need to bring it back from disk
    if(vp->resident == 0 && (vp->zeroed == 0)){
      disk_read(vp->diskblock, vp->pte->ppage);
      vp->zeroed = 0;
      vp->pte->read_enable = 1;
      vp->read = 1;
      vp->valid = 1;
      vp->referenced = 1;
      vp->resident = 1;
      vp->dirty = 0;
      if(write_flag){
	vp->pte->write_enable = 1;
	vp->write = 1;
      }
      phys_vpages.insert(phys_vpages.begin()+clock_hand, vp);
      clock_hand++;
      return 0;
    }
    vp->pte->read_enable = 1;
    vp->read = 1;
    vp->valid = 1;
    vp->referenced = 1;
    vp->resident = 1;
    vp->dirty = 0;
    vp->zeroed = 1;
    //insert page into list right before the clock hand index
    phys_vpages.insert(phys_vpages.begin()+clock_hand, vp);
    clock_hand++;
  }
  // if we are writing to this page, make it write enable and dirty
  if(write_flag){
    vp->pte->write_enable = 1;
    vp->pte->read_enable = 1;
    vp->read = 1;
    vp->write = 1;
    vp->dirty = 1;
    vp->zeroed = 0;
  }
  return 0;
}
*/


void vm_destroy(){
  for(int i = 0; i < phys_vpages.size(); i++){
    if(phys_vpages.front()->pid == current_process->pid){
      phys_vpages.pop();
    } else {
      phys_vpages.push(phys_vpages.front());
      phys_vpages.pop();
    }
  }
  // open up all of the ppages and diskblocks
  for(int i = 0; i < current_process->num_vpages; i++){
    d_blocks.push(current_process->vpages[i]->diskblock);
    if(current_process->vpages[i]->resident == 1){
      m_pages.push(current_process->vpages[i]->pte->ppage);
    }
    delete current_process->vpages[i];
  }
  // remove process from list of processors, then delete
  process_list.erase(current_process->pid);
  //cout << "current process: " << current_process << endl;
  delete current_process;
  
  return;
}

void * vm_extend(){
    
  if(!d_blocks.empty()){
    
    vpage *vp = new vpage;
    vp->diskblock = d_blocks.top();
    d_blocks.pop();
    vp->valid = 0;
    vp->zeroed = 1;
    vp->dirty = 0;
    vp->pid = current_process->pid;
    
    //finding the next invalid page address
    for(int i = 0; i < VM_ARENA_SIZE/VM_PAGESIZE; i++){
      
      if((int) page_table_base_register->ptes[i].ppage == -1){
	page_table_base_register->ptes[i].ppage = 2147483647; //INT_MAX, set as no longer available
	vp->pte = &(page_table_base_register->ptes[i]);
	
	//adding the virtual page to the current process' vpage map
	current_process->vpages.insert(pair <int, vpage*>(current_process->num_vpages, vp));
	return (void*) (((unsigned long)current_process->num_vpages++ * VM_PAGESIZE) + ((unsigned long) VM_ARENA_BASEADDR));
	
      }
    }
    //if there aren't anymore empty pages in pagetable
    return NULL;
  }
  
  else {
    //no diskblock to back it up
    return NULL;
  }
}

int vm_syslog(void *message, unsigned int len){
  if(((unsigned long) message) < ((unsigned long)VM_ARENA_BASEADDR)){
    return -1;
  }
  // get the vpage at address
  int vpNum = (int)((unsigned long)message - (unsigned long)VM_ARENA_BASEADDR) / VM_PAGESIZE;

  // see if this is a valid virtual page
  if(vpNum >= current_process->vpages.size()){
    return -1;
  }
  if(current_process->vpages[vpNum]->pte->read_enable == 0){
    if(vm_fault((void *)( vpNum * VM_PAGESIZE)+(unsigned long)VM_ARENA_BASEADDR, false)){
      return -1;
    }
  }
  // if it is a valid vpage...
  vpage *vp = current_process->vpages[vpNum];

  unsigned long dif_from_start = (unsigned long)message - ((unsigned long)(vpNum*VM_PAGESIZE) + ((unsigned long) VM_ARENA_BASEADDR));

  unsigned long dif_to_end = ((unsigned long) VM_PAGESIZE) - dif_from_start;

  string s;
  int position_in_physmem = (int)(VM_PAGESIZE * vp->pte->ppage + dif_from_start);

  //if the message is within one page
  if(len <= dif_to_end){
    s.append(((char*)pm_physmem + position_in_physmem), len);
  }
  else{
    unsigned int counter = len - dif_to_end;
    s.append(((char*)pm_physmem + position_in_physmem), dif_to_end);
    
    while(counter > 0){
      vpNum++;
      //if it's a valid vpage
      if(vpNum >= current_process->vpages.size()){
	return -1;
      }
      //if not in physical memory, bring it into physical memory
      if(current_process->vpages[vpNum]->pte->read_enable == 0){
	if(vm_fault((void *)( vpNum * VM_PAGESIZE)+(unsigned long)VM_ARENA_BASEADDR, false)){
	  return -1;
	}
      }
      vp = current_process->vpages[vpNum];
      position_in_physmem = (int)(VM_PAGESIZE * vp->pte->ppage);
      //if we have more than a page left to copy, append a whole page
      if(counter >= VM_PAGESIZE){
	s.append(((char*)pm_physmem + position_in_physmem), VM_PAGESIZE);
	counter -= (unsigned int)VM_PAGESIZE;
      }
      //if not, then append what is left
      else{
	s.append(((char*)pm_physmem + position_in_physmem), counter);
	break;
      }
    }
    
  }

  cout << "syslog \t\t\t" << s << endl;
  return 0;
}


