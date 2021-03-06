#include "Graph.hpp" 
#include "Node.hpp"
#include "Ancestry.hpp"
#include "Survivors.hpp"
#include "SequenceArray.hpp"
#include "AddrArray.hpp"

#include <sstream>
#include <cassert>
#include <cstring>


Graph::Graph(nat initSize, const ProgramOptions &progOpt)
  : mutNodes(1000)
  , recNodes(1000)
  , nodMan(1000, 
	   progOpt.hasOption("refForCoal") ? 
	   progOpt.get<nat>("refForCoal") - 1 
	   : 0 , 
	   NOT progOpt.hasOption("refForCoal"))
  , buffer(100)
#ifndef NDEBUG
  , survivorsContainStartingNode(true)
#endif
{
  previousState.resize(initSize);
  fill(previousState.begin(), previousState.end(), nullptr); 
}

/** 
    @param invertedOrientation in case of recombination, indicates the orientation of the node 
*/ 
void Graph::touchNode(seqLen_t loc, nat indiNr, nat genNum, NodeType type, bool invertedOrientation)
{
  assert(type != NOTHING); 
  assert(type == MUTATION || type == RECOMBINATION) ;

  Node *node = nullptr ; 
  switch(type)
    {
    case NodeType::MUTATION: 
      node = mutNodes.getNext();
      break; 
    case NodeType::RECOMBINATION:
      node = recNodes.getNext();
      break;
    default: assert(0); 
    }  
  assert(node != nullptr);

  nodMan.registerNextNode(*node); 

  node->loc = loc; 
  node->indiNr = indiNr; 
  node->originGen = genNum; 
  node->type = type;

  if(type == RECOMBINATION)
    {
      NodeExtraInfo *info = nodMan.getInfo(node->id); 
      if(invertedOrientation)
	info->invertOrientation();
    }
}


void Graph::propagateSurvivorNodes(nat genC, nat chromId, Node **nowBuffer, Node **prevBuffer, 
				   const Ancestry &ancestry, const Survivors &survivors)
{
  // iterate through survivors from generation, starting with past 
  int *end = survivors.getFirstSurvivorForward(genC-1); 

  assert(survivors.getFirstSurvivorForward(genC) < end); 
  
#ifndef NDEBUG
  // that guy assures that we can never have the starting node in the
  // population again, after all instances vanished (due to events)
  bool startingNodePresent = false;
#endif

  nat i = 0; 
  for(auto iter = survivors.getFirstSurvivorForward(genC) ; iter != end ; ++iter)
    { 
      nat ancest; 
      int iterValue; 
      
      if(IS_INVERTED(*iter)) 
	{
	  iterValue = REVERT_INT(*iter);
	  ancest = ancestry.getAddrOfParent(genC, chromId, iterValue); 	  
	  ancest = GET_OTHER_ANCESTOR(ancest); 
	}
      else 
	{
	  iterValue = *iter; 
	  ancest = ancestry.getAddrOfParent(genC, chromId, iterValue); 
	}

      assert(iterValue >= 0); 
      nowBuffer[iterValue] = prevBuffer[ancest];

#ifndef NDEBUG 
      startingNodePresent |= (nowBuffer[iterValue] == nullptr ); 
      assert(NOT nowBuffer[iterValue]
	     || nodMan.getNode(GET_ID_IF(nowBuffer[iterValue]))->originGen <= genC); 
#endif
      
#ifdef DEBUG_HOOKUP
      cout << i++ << " PROPAGATE: " << iterValue << " survives [anc=" << ancest << "]; preNode= "
	   << GET_ID_IF(nowBuffer[iterValue])
	   << endl;
#endif
    }


#ifndef NDEBUG
  if(NOT survivorsContainStartingNode)
    assert(NOT startingNodePresent); 
  else 
    survivorsContainStartingNode = startingNodePresent; 
#endif
}


Node* Graph::hookRecombinations(Node *anc1, Node *anc2)
{
  Node *tmp = buffer.at(0); 
  NodeExtraInfo *info = nodMan.getInfo(tmp->id); 
  Node *current = info->hasInvertedOrientation() ? anc1 : anc2; 
  
  for(nat i = 0; i < buffer.getUsed(); ++i) 
    {
      tmp = buffer.at(i); 
      info = nodMan.getInfo(tmp->id);
      
      tmp->ancId1 = info->hasInvertedOrientation() ? GET_ID_IF(anc2) : GET_ID_IF(anc1) ; 
      tmp->ancId2 = GET_ID_IF(current); 

      current = tmp; 

      assert( tmp->ancId1 != tmp->ancId2 ); 
    }

#ifdef DEBUG_HOOKUP
  Node *tmp1 = nodMan.getNode(current->ancId1);
  Node *tmp2 = nodMan.getNode(current->ancId2);  
  cout << "REC:"  << *current
       << " ===>" <<  (GET_ID_IF(tmp1)) 
       << "\t===>" << (GET_ID_IF(tmp2)) << "\t" << endl; 
#endif

  return current;   
}


void Graph::insertMutEvents(nat genC, AddrArrayBackwardIter<Node,true> &mutBackIter, Node** nodeBufferNowGen, Chromosome &chrom, Randomness &rng)
{
  bool canGoBack = NOT mutBackIter.empty();

  nat chromId = chrom.getId();
  
  while(canGoBack && mutBackIter()->originGen == genC)
    {
      Node *node = mutBackIter();
      nat indiNr = node->indiNr; 

      node->ancId1 = nodeBufferNowGen[indiNr] ? nodeBufferNowGen[indiNr]->id : NO_NODE ; 
      
      // determine the base
      // :TODO: this currently does not allow for a double mutation occuring in one line 
      node->base = chrom.mutateSite(rng, node->loc);
      
#ifdef DEBUG_HOOKUP
      cout << "MUT: [" << *node  << "]" << "\t===>"  << (nodeBufferNowGen[indiNr] ? nodeBufferNowGen[indiNr]->id : 0) << endl;
#endif
      
      nodeBufferNowGen[indiNr] = node;
      canGoBack = mutBackIter.back();
    }
}


/** Algorithm:  
 *   * two buffers, nodes are propagated according to survivor information 
 *   * when recombinations are introduced, the "directions" of these nodes have to alternate
 *   * laying the mutations on top of the recombinations should not be a problem  
 */
void Graph::hookup(const Survivors &survivors, const Ancestry &ancestry, const PopulationManager &popMan, Chromosome &chrom, Randomness &rng, nat startGen, nat endOfSection)
{
  nat chromId = chrom.getId(); 

  // set up both buffers 
  Node **nodeBufferPrevGen = (Node**) malloc(previousState.size() * sizeof(Node*)),
    **nodeBufferNowGen  = (Node**) malloc(popMan.getTotalNumHaploByGen(startGen) * sizeof(Node*)); 

  nat i = 0; 
  for(Node *node : previousState)
    nodeBufferPrevGen[i++] = node;
  
  AddrArrayBackwardIter<Node,true> mutBackIter; 
  mutNodes.getEnd(mutBackIter);

  AddrArrayBackwardIter<Node,true> recBackIter; 
  recNodes.getEnd(recBackIter);

  for(nat genC = startGen; genC < endOfSection; ++genC)
    {
#ifdef DEBUG_HOOKUP
      cout << "======== hooking up generation "  << genC << endl; 
#endif
      // resize buffer 
      nodeBufferNowGen = (Node**) myRealloc(nodeBufferNowGen, popMan.getTotalNumHaploByGen(genC) * sizeof(Node*));
      memset(nodeBufferNowGen,0, popMan.getTotalNumHaploByGen(genC) * sizeof(Node*));
      
      propagateSurvivorNodes(genC, chromId, nodeBufferNowGen, nodeBufferPrevGen, ancestry, survivors); 
      insertRecEvents(genC, chromId, recBackIter, nodeBufferNowGen, nodeBufferPrevGen, ancestry);
      insertMutEvents(genC, mutBackIter, nodeBufferNowGen, chrom, rng);

      std::swap(nodeBufferPrevGen, nodeBufferNowGen);
    }

  // we swapped once too much  
  std::swap(nodeBufferNowGen, nodeBufferPrevGen); 
  
  nat lastSize = popMan.getTotalNumHaploByGen(endOfSection-1);

  previousState.resize(lastSize);
  for(nat i = 0; i < lastSize; ++i)
    previousState[i] = nodeBufferNowGen[i];  

  free(nodeBufferPrevGen);
  free(nodeBufferNowGen); 

#ifdef DEBUG_HOOKUP
  cout << "================> hookup finished"<< endl; 
#endif
}

						

void Graph::printRaw(FILE *fh) 
{
  // print survivors first 
  nat num = previousState.size();
  BIN_WRITE(num,fh);
  
  for(nat i = 0; i < num; ++i)
    {
      Node *node = previousState[i]; 
      nat localNum = 0; 
      if(node)
	localNum = node->id; 
      BIN_WRITE(localNum,fh); 
    }
  
  nat numberOfNodes = 0; 
  // determine number 
  nat end = nodMan.getNumberOfNodesUsed();
  for(nat i =  1 ; i < end ; ++i)
    {
      Node *node = nodMan.getNode(i); 
      assert(node); 
      auto myInfo = nodMan.getInfo(node->id); 
      
      if(NOT myInfo->isSkip())
	++numberOfNodes; 
    }

  
  BIN_WRITE(numberOfNodes,fh); 

  for(nat i = 1; i < end ; ++i)
    {
      Node *node = nodMan.getNode(i);
      assert(node) ; 

      auto myInfo = nodMan.getInfo(node->id); 
      
      if(myInfo->isSkip())	
	continue;
      else 
	node->printRaw(fh);
    }
}


void Graph::createSequencesInGraph(const Chromosome &chromo)
{
  seqLen_t start = 0; 
  seqLen_t end = chromo.getSeqLen();

  for(Node *node : previousState)    
    if(node)
      nodMan.markAncestrialMaterial(*node, start, end);  

  for(Node *node : previousState)    
    if(node)
      nodMan.determineCoalescentNodes(node);
    

  nodMan.initBvMeaning();
  nodMan.computeOptimalRefed();

  // a slight hack to initialize the dummy node 
  nat numNeutMut = getBvMeaning().size();
  NodeExtraInfo *info = nodMan.getInfo(0); 
  info->bv = new BitSetSeq(numNeutMut); 
  
  for(nat i = 0; i < previousState.size(); ++i)
    {
      Node *node = previousState[i]; 

#ifdef DEBUG_SEQUENCE_EXTRACTION      
      cout << "starting extraction for sequence "  << i << "\t" << previousState.size() << endl; 
#endif

      if(node)
	nodMan.createSequenceForNode(node);  
    }    
}




void Graph::insertRecEvents(nat genC, nat chromId,  AddrArrayBackwardIter<Node,true> &recBackIter, Node** nodeBufferNowGen, Node** nodeBufferPrevGen, const Ancestry &ancestry)
{
  bool canGoBack = true;
  
  // insert recombinations
  while(canGoBack && recBackIter()->originGen == genC)
    {
      nat indiNr = recBackIter()->indiNr, 
	gen = recBackIter()->originGen, 
	ancst1 = ancestry.getAddrOfParent(genC, chromId, indiNr),
	ancst2 = GET_OTHER_ANCESTOR(ancst1);

      Node
	*ancNode1 = nodeBufferPrevGen[ancst1],
	*ancNode2 = nodeBufferPrevGen[ancst2];

      assert(nodeBufferNowGen[indiNr] == ancNode1); 

      buffer.resetUsed();
      buffer.setNext(recBackIter());
      
      // determine how many rec events we have to handle  
      while((canGoBack = recBackIter.back()) && recBackIter()->originGen == gen && recBackIter()->indiNr == indiNr)
	buffer.setNext(recBackIter());
      
      if(ancNode1 != ancNode2)  
	nodeBufferNowGen[indiNr] = hookRecombinations(ancNode1, ancNode2);
    }
}


/** 
    Takes care of non-SNP mutations. See the respective NodeManager
    function.
    :TRICKY: This is still part of the mutation process
 */ 
void Graph::rectifyMultipleNP(Randomness &rng)
{  
  vector<BitSetSeq*> seqs; 
  for(Node *node : previousState)
    {
      if(node)
	{
	  auto info = nodMan.getInfo(node->id);       
	  seqs.push_back(info->bv); 
	}
    }

  nodMan.rectifyMultipleNP(seqs, rng);   
}


void Graph::getRawSequences(vector<BitSetSeq*> &bvs)
{
  for(Node *node : previousState)
    {      
      if(node )
	{
	  auto info = nodMan.getInfo(node->id); 
	  bvs.push_back(info->bv); 
	}
      else 			// in this case, starting node survived or (a sub-case, we do not have any graph at all ) 
	{
	  auto info = nodMan.getInfo(0); 
	  bvs.push_back(info->bv); 
	}
    }
} 
