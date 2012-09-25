#include "NodeManager.hpp"


NodeManager::NodeManager(nat initSize)
  : length(initSize)
  , highestId(1)          // 0 is the non-existing starting node
  , allocatedSeqs(100)
  , allocatedMuts(100)
{
  relevantNodes = (Node**)calloc(length, sizeof(Node*)); 
  relevantNodes[0] = nullptr; 
  extraInfo = (NodeExtraInfo*) calloc(length, sizeof(NodeExtraInfo));   
}


NodeManager::~NodeManager()
{  
  nat end = allocatedSeqs.getUsed(); 
  for(nat i = 0; i < end; ++i)
    delete allocatedSeqs.at(i); 

  end = allocatedMuts.getUsed();
  for(nat i = 0; i < end; ++i)
    delete allocatedMuts.at(i); 

  free(relevantNodes); 
  free(extraInfo); 
}


void NodeManager::resize()
{
  length *= 2 ;
  relevantNodes = (Node**)myRealloc(relevantNodes, length * sizeof(Node*)); 
  extraInfo = (NodeExtraInfo*)myRealloc(extraInfo, length * sizeof(NodeExtraInfo)); 

  memset(extraInfo + (length / 2) , 0, (length / 2) * sizeof(NodeExtraInfo)); 
}


void NodeManager::registerNextNode(Node &node)
{
  if(highestId == length-1)
    resize();
  relevantNodes[highestId] = &node; 
  node.id = highestId++;  
}


void NodeManager::checkConsistency()
{
  for(nat i = 1; i < highestId; ++i)
    {
      Node *node = getNode(i); 
      assert(node) ; 
      assert(node->id == i);
    }
}


/** 
 * Mark for each node the ancestrial region needed.  
 */ 
void NodeManager::markAncestrialMaterial(Node &node, seqLen_t start, seqLen_t end)
{
  NodeExtraInfo *info = getInfo(node.id);   
  bool wasInit = info->wasInitialized; 

  if(NOT wasInit)
    {
      info->start = start; 
      info->wasInitialized = true; 
    }
  
  bool extendsStart = (NOT wasInit || start < info->start), 
    extendsEnd = (NOT wasInit || info->end < end ); 
  
  
  if(start < info->start)
    info->start = start; 
  if(info->end < end )
    info->end = end; 

#ifdef DEBUG_BACKTRACE
  cout << (NOT wasInit ? " INIT: " : "") << " entering " << node << "\t" << *info << endl; 
#endif

  assert(start < end && info->start < info->end);   
  
  if( NOT node.id ) 		// excluding start node 
    return; 

  switch(node.type)
    {
    case MUTATION:
      {
	Node *anc1 = getNode(node.ancId1);
	if( (extendsStart || extendsEnd ) && anc1 && info->start < info->end)
	  markAncestrialMaterial(*anc1,info->start,info->end);
      }
      break; 
    case RECOMBINATION:
      {
	Node
	  *anc1 = getNode(node.ancId1),
	  *anc2 = getNode(node.ancId2);

	if(anc1 && extendsStart && info->start < node.loc-1)
	  markAncestrialMaterial(*anc1, info->start, node.loc-1);

	if(anc2 && extendsEnd && node.loc < info->end)
	  markAncestrialMaterial(*anc2, node.loc, info->end); 
	    
	break; 
      }
    default: 
      {
	cout << "aborting: node type is " << node.type << endl; 
	abort(); 
      }
    }
}


/** 
 *  just jump over unneeded recombination nodes 
 */ 
nat NodeManager::getTrueAncestor(Node *node)
{
  if(NOT node)
    return NO_NODE; 

  auto myInfo = getInfo(node->id); 
  switch(node->type)
    {
    case MUTATION:
      {
	if(NODE_IS_RELEVANT(node->loc,myInfo->start, myInfo->end) ) 
	  {
	    return node->id;
	  }
	else
	  {
	    myInfo->skip = true; 
	    return getTrueAncestor(getNode(node->ancId1)); 
	  }
	break; 
      }
    case RECOMBINATION:
      {
	if(NODE_IS_RELEVANT(node->loc,myInfo->start, myInfo->end))
	  return node->id; 	
	else 
	  {
	    myInfo->skip = true; 
	    if(myInfo->start <= node->loc)
	      return getTrueAncestor(getNode(node->ancId1)); 
	    else 
	      {
		assert(node->loc <= myInfo->end); 
		return getTrueAncestor(getNode(node->ancId2)); 
	      }
	  }
	break; 
      }
    default: assert(0); 
    }

  cerr << "should never trigger" << endl; 
  abort();
  
  return NO_NODE; 
}




void NodeManager::determineCoalescentNodes(Node *node)
{
  if(NOT node)
    return; 

  auto myInfo = getInfo(node->id);

  if(myInfo->referenced)  
    {
      ++myInfo->referenced; 
      return;
    }
  else 
    ++myInfo->referenced;  

  switch(node->type)
    {
    case MUTATION:
      {
#ifdef FEATURE_REDUCE_GRAPH
	node->ancId1 = getTrueAncestor(getNode(node->ancId1)); 	
#endif
	Node *anc1 = getNode(node->ancId1); 

	if(anc1)
	  {
	    if(NOT (anc1->originGen <= node->originGen)) {	      cout << *node << "\t================>" <<  *anc1  << endl;  abort() ; } 
	    determineCoalescentNodes(anc1); 	    
	  }
	break;
      }
    case RECOMBINATION: 
      {
	Node* anc1 = getNode(node->ancId1); 
	Node* anc2  = getNode(node->ancId2); 
	NodeExtraInfo *anc1Info = getInfo(node->ancId1); 
	NodeExtraInfo *anc2Info = getInfo(node->ancId2); 
	
	if(anc1 && myInfo->start < node->loc -1 )
	  {
	    if(NOT (anc1->originGen <= node->originGen)) {	      cout << *node << "\t================>" <<  *anc1  << endl;  abort() ; } 

#ifdef FEATURE_REDUCE_GRAPH
	    node->ancId1 = getTrueAncestor(anc1); 	    
#endif
	    determineCoalescentNodes(getNode(node->ancId1)); 
	  }

	if(anc2 && node->loc < myInfo->end)
	  {
	    if(NOT (anc2->originGen <= node->originGen)) {	      cout << *node << "\t================>" <<  *anc2  << endl;  abort() ; }

#ifdef FEATURE_REDUCE_GRAPH
	    node->ancId2 = getTrueAncestor(anc2); 
#endif	    
	    determineCoalescentNodes(getNode(node->ancId2)); 
	  }

	break;
      }
    default: assert(0);
    }
}



NeutralMutation* NodeManager::createNeutralMutation(Node *node)
{
  assert(node->type == MUTATION);
  NeutralMutation *res = new NeutralMutation;
  res->absPos = node->loc;
  res->base = node->base;
  res->generation = node->originGen;
  allocatedMuts.setNext(res);
  return res; 
}


void NodeManager::gatherMutations(Node *node, NeutralArray* seq,seqLen_t start, seqLen_t end)
{
#ifdef DEBUG_SEQUENCE_EXTRACTION
  cout << "VISITING " << *node << endl; 
#endif

  assert(getInfo(node->id)->referenced == 1  ); 

  switch(node->type)
    {
    case MUTATION: 
      {
	NeutralMutation *mut =  (createNeutralMutation(node)); 
	seq->mutate(*mut, false);
	Node *anc = getNode(node->ancId1); 
#ifdef DEBUG_SEQUENCE_EXTRACTION
	cout << "\tVISIT: inserting mutation " << *mut << endl; 
#endif
	if(anc)
	  handleAncestor(seq, anc, start,end);
	break; 
      }
    case RECOMBINATION:
      {
	Node *anc1 = getNode(node->ancId1), 
	  *anc2 = getNode(node->ancId2); 
	
	if( anc1 && start < node->loc-1 )
	  handleAncestor(seq, anc1, start, min(end, node->loc-1)); 
	if( anc2 && node->loc < end )
	  handleAncestor(seq, anc2, max(node->loc, start), end); 
	break; 
      }
    default : assert(0); 
  }
}


void NodeManager::handleAncestor(NeutralArray *seq, Node *anc , seqLen_t start, seqLen_t end) 
{
  NodeExtraInfo *ancInfo = getInfo(anc->id); 

  assert(ancInfo->referenced); 

  // it is a coalescent node 
  if(ancInfo->referenced > 1)
    {
      simulateNode(anc);
      assert(start < end); 
      seq->conditionallyJoinOtherSeq(*(ancInfo->sequence), start,end); 
    }
  else 
    gatherMutations(anc, seq, start, end); 
}


void NodeManager::simulateNode(Node *node)
{
#ifdef DEBUG_SEQUENCE_EXTRACTION
  cout << "SIMULATING " << *node  << endl; 
#endif

  // starting node or we already simluated  
  if(NOT node || getInfo(node->id)->sequence )
    {
#ifdef DEBUG_SEQUENCE_EXTRACTION
      if(node)
	cout << "ALREADY" << *node << endl; 
      else 
	cout << "START" << endl; 
#endif
      return;
    }

  auto info = getInfo(node->id); 
  if(NOT info->sequence)
    {
      info->sequence = new NeutralArray(100); 
      allocatedSeqs.setNext(info->sequence); 
    }

  switch(node->type)
    {
    case MUTATION: 
      {
	Node *anc = getNode(node->ancId1);
	if(anc)
	  handleAncestor(info->sequence, anc, info->start, info->end);
	NeutralMutation *mut = createNeutralMutation(node); 
#ifdef DEBUG_SEQUENCE_EXTRACTION
	cout << "\tSIM: inserting mutation " << *mut << endl; 
#endif
	info->sequence->mutate(*mut, false);
	break;
      }
    case RECOMBINATION: 
      {
	Node *anc1 = getNode(node->ancId1);
	Node *anc2 = getNode(node->ancId2);

	seqLen_t start = info->start,
	  end = info->end; 
	
	if(anc1 && start < node->loc-1 )
	  handleAncestor(info->sequence,anc1, start, min(end, node->loc-1));
	if(anc2 && node->loc < end) 
	  handleAncestor(info->sequence,anc2, max(node->loc, start), end); 
	break;
      }
    default: assert(0); 
    }

#ifdef DEBUG_SEQUENCE_EXTRACTION
  cout << "\t" << *(info->sequence) << endl;  
#endif
}


