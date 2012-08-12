#include "Graph.hpp" 
#include "Node.hpp"
#include "Ancestry.hpp"		// 
#include "Survivors.hpp"
#include "SequenceArray.hpp"
#include "AddrArray.hpp"

#include <sstream>
#include <cassert>
#include <cstring>


Graph::Graph(nat initSize)
  : mutNodes(1000)
  , recNodes(1000)
  , nodMan(1000)
  , buffer(100)
{
  previousState.resize(initSize);
  fill(previousState.begin(), previousState.end(), nullptr); 
}


void Graph::touchNode(seqLen_t loc, nat indiNr, nat genNum, NodeType type)
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

}


#define GET_ID_IF(x) (x ? x->id : NO_NODE) 


void Graph::propagateSurvivorNodes(nat genC, nat chromId, Node **nowBuffer, Node **prevBuffer, 
				   const Ancestry &ancestry, const Survivors &survivors)
{
  // iterate through survivors from generation, starting with past 
  nat *end = survivors.getFirstSurvivorForward(genC-1); 

  assert(survivors.getFirstSurvivorForward(genC) < end); 

  nat i = 0; 
  for(auto iter = survivors.getFirstSurvivorForward(genC) ; iter != end ; ++iter)
    {
      nat ancest = ancestry.getAddrOfParent(genC, chromId, *iter);
      nowBuffer[*iter] = prevBuffer[ancest];      
      assert(NOT nowBuffer[*iter]
	     || nodMan.getNode(GET_ID_IF(nowBuffer[*iter]))->originGen <= genC); 

#ifdef DEBUG_HOOKUP
      cout << i++ << " PROPAGATE: " << *iter << " survives; preNode= "
	   << GET_ID_IF(nowBuffer[*iter])  
	   << endl;
#endif
    }
}


Node* Graph::hookRecombinations(Node *anc1, Node *anc2)
{
  Node *current = IS_ODD(buffer.getUsed() ) ? anc2 : anc1; 
  bool sameBefore = anc1 == anc2; 
  
  // :TRICKY: starting with LAST event
  for(nat i = buffer.getUsed(); i > 0;  --i)
    {
      Node *tmp = buffer.at(i-1); 
      tmp->ancId1 = IS_ODD(i) ? GET_ID_IF(anc1) : GET_ID_IF(anc2); 
      tmp->ancId2 = GET_ID_IF(current);
      current = tmp; 
      assert(sameBefore || (tmp->ancId1 != tmp->ancId2));       
    }

#ifdef DEBUG_HOOKUP
  Node *tmp1 = nodMan.getNode(current->ancId1);
  Node *tmp2 = nodMan.getNode(current->ancId2);
  cout << "REC:"  << *current << " ===> " 
       << (tmp1 == nullptr ? 0 : tmp1->id ) << "\t===>"
       << (tmp2 == nullptr ? 0 : tmp2->id ) << "\t" << endl; 
#endif
  return current; 
}


void Graph::insertMutEvents(nat genC, AddrArrayBackwardIter<Node,true> &mutBackIter, Node** nodeBufferNowGen, Chromosome &chrom, Randomness &rng)
{
  bool canGoBack = true;
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
void Graph::hookup(const Survivors &survivors, const Ancestry &ancestry, const PopulationManager &popMan, Chromosome &chrom, Randomness &rng, nat startGen, nat endGen)
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

  for(nat genC = startGen; genC < endGen; ++genC)
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
  
  nat lastSize = popMan.getTotalNumHaploByGen(endGen);
  previousState.resize(lastSize);
  for(nat i = 0; i < lastSize; ++i)
    previousState[i] = nodeBufferNowGen[i];

  free(nodeBufferPrevGen);
  free(nodeBufferNowGen); 

#ifdef DEBUG_HOOKUP
  cout << "================> hookup finished"<< endl; 
#endif
}

						

void Graph::printArg(FILE *fh)
{
  nat end = nodMan.getNumberOfNodesUsed();
  BIN_WRITE(end,fh); 

  for(nat i = 1; i < end ; ++i)
    {
      Node *node = nodMan.getNode(i);
      assert(node) ; 

      auto myInfo = nodMan.getInfo(node->id); 
      
      if(myInfo->skip)
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

  for(Node *node : previousState)
    if(node)
      nodMan.simulateNode(node);
}

#ifdef VERIFICATION 
// void Graph::getSequencesSlow(vector<NeutralArray*> &seqs, Chromosome &chrom, Randomness &rng)
// {
//   for(Node *node : previousState)
//     {
//       NeutralArray *seq = new NeutralArray(100);  // :MAGIC: 
//       gatherSequencesSlow(node, seq, 0, chrom.getSeqLen(), chrom, rng);
//       seqs.push_back(seq); 
//     }
// }

// void Graph::gatherSequencesSlow(Node *node, NeutralArray *seq, seqLen_t start, seqLen_t end, Chromosome &chrom, Randomness &rng)
// {
//   if(start >= end || NOT node)
//     return; 

//   switch(node->type)
//     {
//     case MUTATION: 
//       {
// 	gatherSequencesSlow(nodMan.getNode(node->ancId1), seq, start, end, chrom, rng); 
// 	auto neutMut = new NeutralMutation;
	
// 	if(start <= node->loc && node->loc <= end)
// 	  {
// 	    neutMut->absPos = node->loc; 
// 	    neutMut->generation = node->originGen; 	
// 	    neutMut->base = chrom.mutateSite(rng, node->loc); 
// 	    seq->mutate(*neutMut); 
// 	  }
// 	break; 
//       }
//     case RECOMBINATION:
//       {
// 	gatherSequencesSlow(nodMan.getNode(node->ancId1),seq, start, min(node->loc-1, end), chrom, rng); 
// 	gatherSequencesSlow(nodMan.getNode(node->ancId2),seq, max(node->loc,start),end, chrom, rng);
// 	break; 
//       }
//     default: assert(0);   
//   }
// }
#endif 


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
