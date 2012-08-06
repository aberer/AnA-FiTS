#include "HaploTimeWindow.hpp"


// :TRICKY: we have one object per chromosome, so the number of haplotypes is two times the number of individuals (for diploid)  


// :TRICKY: for diploids currently
HaploTimeWindow::HaploTimeWindow(nat initPopSize, SelectedArray *selectedInit)
{
  oldState = (SelectedArray**)calloc(initPopSize * 2, sizeof(SelectedArray*)); 
  newState = (SelectedArray**)calloc(initPopSize * 2, sizeof(SelectedArray*)); 
  fill(oldState, oldState + (initPopSize * 2) , selectedInit); 
  
  numBuf = (uint32_t*)malloc(sizeof(uint32_t)); 
}


HaploTimeWindow::~HaploTimeWindow()
{
  free(oldState); 
  free(newState); 
  free(numBuf); 
}


void HaploTimeWindow::switchPastAndPresent()
{
  swap(oldState,newState); 
}


