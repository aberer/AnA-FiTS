#include "BinaryWriter.hpp" 


BinaryWriter::BinaryWriter(string id, nat _numHaplo)
  : numHaplo(_numHaplo)
{
  // create file 
  stringstream fileName; 
  fileName << SEQ_FILE_NAME << "." << id ; 
  fh = openFile(fileName.str(), "w"); 
}


BinaryWriter::~BinaryWriter()
{
  fclose(fh);
}



/* 
   :TRICKY: requires mutations to be unclaimed
 */ 
void BinaryWriter::write(Graph &graph, HaploTimeWindow &w, Chromosome &chrom)
{ 
  NeutralArray dummy(10); 

#ifdef PRINT_SEQ_STDOUT	  
  cout << "fixed: " ; 
#endif

  // print fixed mutations first
  SelectedArray *fixed = chrom.getFixedMutations(0);
  if(fixed)
    {
      nat numFixed = fixed->size();
      BIN_WRITE(numFixed,fh); 
      for(SelectedMutation **mut = fixed->begin(); mut != fixed->end(); ++mut)
	{
#ifdef PRINT_SEQ_STDOUT	  
	  cout << **mut; 
#endif
	  (*mut)->printRaw(fh); 
	}
    }
  else 
    {
#ifdef PRINT_SEQ_STDOUT	
      cout << "none" ; 
#endif
      nat tmp = 0; 
      BIN_WRITE(tmp,fh); 
    }

#ifdef  PRINT_SEQ_STDOUT
  cout <<  endl; 
#endif

  // gather all neutral mutations 
  vector<Node*> nodes = graph.getState();
  vector<NeutralArray*> arrays; 
  for(auto node : nodes)
    {
      if(node)
	{
	  NeutralArray *array = graph.getSequenceFromNode(node); 
	  arrays.push_back(array); 
	}
      else 
	arrays.push_back(&dummy); 
    }

  vector<NeutralMutation*> nMuts; 
  getSortedMutations<NeutralMutation>(nMuts, arrays); 

  // gather selected mutations 
  vector<SelectedArray*> arrays2; 
  for(nat i = 0; i < numHaplo; ++i )
    arrays2.push_back(w.at(i)); 
  vector<SelectedMutation*> sMuts; 
  getSortedMutations<SelectedMutation>(sMuts,arrays2); 

  nat totalNum = sMuts.size() + nMuts.size(); 
  BIN_WRITE(totalNum,fh); 	// WRITE 
  printMutations(cout, fh, nMuts, sMuts);

  vector<BitSet<uint32_t>*> allBS; 
  for(nat i = 0; i < numHaplo; ++i)
    allBS.push_back(new BitSet<uint32_t>(totalNum)); 


  // // DEBUG 
  // cout << "all " ; 
  // for(auto mut : nMuts)
  //   cout << *mut;
  // cout << endl; 
  // // END


  // print out every haplotype 
  for(nat i = 0; i < numHaplo; ++i)
    {
      auto nSeq = arrays[i]; 
      auto sSeq = arrays2[i];       
      // cout << "local " <<  *nSeq << endl; 
      convertToBitSet(*(allBS[i]), nSeq, sSeq, nMuts, sMuts);
    }

#ifndef NDEBUG
  BitSet<uint32_t> noWhere(nMuts.size() + sMuts.size());
  for(nat i = 0; i < numHaplo; ++i)
    noWhere.orify(*(allBS[i])); 
  if(noWhere.count() != nMuts.size() + sMuts.size())
    {
      cout << "#mut anywhere=" << noWhere.count() << "\ttotal nMuts="  << nMuts.size()  << "\ttotalsMuts=" << sMuts.size() << endl; 
      for(auto mut: nMuts)
	cout << *mut ; 
      cout << endl; 
      assert(0); 
    }
#endif
      
  nat numSeq = numHaplo;
  BIN_WRITE(numSeq, fh); 
  for(nat i = 0; i < numSeq; ++i)	
    {
      allBS[i]->printRaw(fh); 
#ifdef PRINT_SEQ_STDOUT
      cout << *(allBS[i]) << endl;
#endif
    }
      
  for(nat i = 0; i < numSeq; ++i)
    delete allBS[i]; 
}


ostream& BinaryWriter::printMutations(ostream &rhs , FILE *fh, vector<NeutralMutation*> &neutralMutations, vector<SelectedMutation*> &selMutations)
{
  // print all mutations 
  auto nIter = neutralMutations.begin(),
    nEnd = neutralMutations.end();
  auto sIter = selMutations.begin(),
    sEnd = selMutations.end();
  while(nIter != nEnd && sIter != sEnd)
    {
      if((*nIter)->absPos < (*sIter)->absPos)
	{	  
#ifdef PRINT_SEQ_STDOUT
	  rhs << **nIter << ";" ; 
#endif
	  (*nIter)->printRaw(fh); 
	  nIter++; 
	}
      else 
	{
#ifdef PRINT_SEQ_STDOUT
	  rhs << **sIter << ";"; 
#endif
	  (*sIter)->printRaw(fh); 
	  sIter++; 
	}
    }
  while(nIter != nEnd)
    {
#ifdef PRINT_SEQ_STDOUT
      rhs << **nIter << ";" ; 
#endif
      (*nIter)->printRaw(fh); 
      nIter++; 
    }

  while(sIter != sEnd)
    {
#ifdef PRINT_SEQ_STDOUT
      rhs << **sIter << ";" ; 
#endif
      (*sIter)->printRaw(fh); 
      sIter++; 
    }      
#ifdef PRINT_SEQ_STDOUT
  rhs << endl; 
#endif

  return rhs; 
}



#define SET_IF_FOUND(subset, subEnd, allset, bv,bvIdx)	\
  if( subset != subEnd && *subset == *allset  )		\
    {							\
      assert(NOT bv.test(bvIdx));			\
      bv.set(bvIdx);					\
      ++subset;						\
    }							\
  

// #define DEBUG_BITVECTOR_CONVERSION 

void BinaryWriter::convertToBitSet(BitSet<uint32_t> &bs, NeutralArray *nSeq, SelectedArray *sSeq,  const vector<NeutralMutation*> &neutralMutations, const vector<SelectedMutation*> &selMutations) 
{  
  SelectedMutation** sIter = sSeq->begin(), 
    **sEnd = sSeq->end();

  NeutralMutation **nIter = nSeq ? nSeq->begin() : nullptr,
    **nEnd = nSeq ? nSeq->end() : nullptr; 

  auto allSIter = selMutations.begin(); 
  auto allSEnd = selMutations.end();
  auto allNIter = neutralMutations.begin(); 
  auto allNEnd = neutralMutations.end();


#ifdef DEBUG_BITVECTOR_CONVERSION
  cout << "\tstarting new conversion"   << endl;   
#endif
  
  nat bsIndex = 0;
  
  for( ; allNIter != allNEnd && allSIter != allSEnd ; ++bsIndex) 
    {      
      if(sIter != sEnd)
	{
#ifdef DEBUG_BITVECTOR_CONVERSION
	  cout << "SELECT/ " << (*allSIter)->absPos << "/"  << (*sIter)->absPos << endl ;
#endif
	  if((*allSIter)->absPos >  (*sIter)->absPos)
	    {
	      cout << *sSeq << endl; 
	      for(auto mut : selMutations)
		cout << mut->absPos  << ","; 
	      cout << endl; 
	      cout << *nSeq << endl; 
	      for(auto mut: neutralMutations)
		cout << mut->absPos << ","; 
	      cout << endl; 
	      abort();
	    }
	}

      if(nIter != nEnd)
	{
#ifdef DEBUG_BITVECTOR_CONVERSION
	  cout << "NEUT/ " << (*allNIter)->absPos << "/" << (*nIter)->absPos << endl;      
#endif
	  if((*allNIter)->absPos >  (*nIter)->absPos)
	    cout << *nSeq << endl; 
	  assert((*allNIter)->absPos <= (*nIter)->absPos); 
	}


      if((*allSIter)->absPos < (*allNIter)->absPos)
	{	  
	  SET_IF_FOUND(sIter, sEnd, allSIter, bs, bsIndex); 
	  ++allSIter;
	}
      else
	{
	  SET_IF_FOUND(nIter, nEnd, allNIter, bs, bsIndex); 
	  ++allNIter;
	}
    }

  for( ; allNIter != allNEnd ; ++bsIndex, ++allNIter) 
    {
      if(nIter != nEnd)
	{
#ifdef DEBUG_BITVECTOR_CONVERSION
	  cout << "NEUT " << (*nIter)->absPos << "\t" <<  (*allNIter)->absPos << endl;      
#endif
	  assert((*allNIter)->absPos <= (*nIter)->absPos); 	
	}

      SET_IF_FOUND(nIter, nEnd, allNIter, bs, bsIndex); 
    }

  for( ; allSIter != allSEnd ; ++allSIter, ++bsIndex )
    {

      if(sIter != sEnd)
	{
#ifdef DEBUG_BITVECTOR_CONVERSION
	  cout << "SELECT " << (*sIter)->absPos << "\t" <<  (*allSIter)->absPos << endl;      
#endif
	  assert((*allSIter)->absPos <= (*sIter)->absPos); 	
	}

      SET_IF_FOUND(sIter, sEnd, allSIter, bs, bsIndex); 
    }

  assert(bsIndex == bs.size()); 
  assert(allNIter == allNEnd && allSIter == allSIter); 
}






void BinaryWriter::writeInt(nat theInt)
{
  BIN_WRITE(theInt,fh);
}
