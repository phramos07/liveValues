#include "LiveValues.h"
#include <iostream>
#include <string>
// #define DEBUG_

using namespace llvm;

#define DEBUG_TYPE "livevalues"

namespace llvm
{
STATISTIC(liveValuesSets, "Number of LiveValueSets");
STATISTIC(liveValues, "Number of LiveValues");
}

char LiveValues::ID = 0;
static RegisterPass<LiveValues> W("livevalues", 
	"Get Live Values in each BB in the LLVM's IR");

void LiveValues::getAnalysisUsage(AnalysisUsage &AU) const
{
	AU.setPreservesAll();
}

bool LiveValues::runOnFunction(Function &func)
{
	__init__(&func);
	// #ifdef DEBUG_
	// 	errs() << "\n\nBEGINNING. Function: " << func.getName() << "\n\n";
	// 	printAllSets();
	// #endif

	while (!worklist.empty())
	{
		BasicBlock* BB = getNextBasicBlock();
		ValueSet newIN = getLiveValuesSet_IN(BB);
		if (!compareTwoValueSets(&newIN, &IN[BB]))
		{
			for (auto pred = pred_begin(BB); pred != pred_end(BB); ++pred)
			{
				addToWorkList(*pred);
			}
		}
		IN[BB] = newIN;
		OUT[BB] = getLiveValuesSet_OUT(BB);
	}

	liveValues = allValues.size();


	#ifdef DEBUG_
		errs() << "\n\nEND. Function: " << func.getName() << "\n\n";
		printAllSets();
	#endif

	return false;
}

void LiveValues::__init__(Function* F)
{
	for (auto &BB : *F)
	{
		GEN[&BB] = getUsedValues(&BB);
		KILL[&BB] = getDefinedValues(&BB);
		IN[&BB] = ValueSet();
		OUT[&BB] = ValueSet();
		worklist.push(&BB);
		liveValuesSets++;
	}
}

ValueSet LiveValues::getLiveValuesSet_OUT(BasicBlock* BB)
{
	ValueSet newOUT;
	for (auto succ = succ_begin(BB); succ != succ_end(BB); ++succ)
	{
		newOUT.insert(IN[*succ].begin(), IN[*succ].end());
	}

	allValues.insert(newOUT.begin(), newOUT.end());
	return newOUT;
}

ValueSet LiveValues::getLiveValuesSet_IN(BasicBlock* BB)
{
	ValueSet newIN;
	newIN = OUT[BB];
	for (auto &v : KILL[BB])
	{
		newIN.erase(v);
	}
	newIN.insert(GEN[BB].begin(), GEN[BB].end());

	allValues.insert(newIN.begin(), newIN.end());
	return newIN;
}

ValueSet LiveValues::getDefinedValues(Instruction* I)
{
	ValueSet definedValues;
	if (dyn_cast<TerminatorInst>(I))
	{
		
	}
	// else if (dyn_cast<CallInst>(I))
	// {

	// }
	else
	{
		definedValues.insert(I);
	}
	

	return definedValues;
}

ValueSet LiveValues::getDefinedValues(BasicBlock* BB)
{
	ValueSet definedValues;
	for (BasicBlock::iterator i = BB->begin(), e = BB->end(); i != e; ++i)
	{
		ValueSet definedInInstruction = getDefinedValues(i);
		definedValues.insert(definedInInstruction.begin(), definedInInstruction.end());
	}

	return definedValues;
}

ValueSet LiveValues::getUsedValues(Instruction* I)
{
	ValueSet usedValues;
	for (Use &u : I->operands())
	{
		Value *v = u.get();
		if (dyn_cast<Instruction>(v))
			usedValues.insert(v);
		else if (dyn_cast<ConstantInt>(v))
			usedValues.insert(v);
	}

	return usedValues;
}

ValueSet LiveValues::getUsedValues(BasicBlock* BB)
{
	ValueSet usedValues;
	for (BasicBlock::iterator i = BB->begin(), e = BB->end(); i != e; ++i)
	{
		ValueSet usedInInstruction = getUsedValues(i);
		usedValues.insert(usedInInstruction.begin(), usedInInstruction.end());
	}
	for (BasicBlock::iterator i = BB->begin(), e = BB->end(); i != e; ++i)
	{
		usedValues.erase(i);
	}	

	return usedValues;
}

void LiveValues::addToWorkList(BasicBlock* BB)
{
	worklist.push(BB);
}

BasicBlock* LiveValues::getNextBasicBlock()
{
	BasicBlock* BB = worklist.top();
	worklist.pop();
	
	return BB;
}

bool LiveValues::compareTwoValueSets(ValueSet* A, ValueSet* B)
{
	if (A->size() != B->size())
		return false;
	else
	{
		for (auto &value : *A)
		{
			if (B->find(value) == B->end())
			{
				return false;
			}
		}
	}

	return true;
}

void LiveValues::printValueSet(ValueSet* set)
{
	for (auto &v : *set)
	{
		v->print(errs());
		errs() << "\n";
	}
}

void LiveValues::printBBValueSet(BBValueSets *bbValueSet)
{
	for (BBValueSets::iterator it = bbValueSet->begin(); it != bbValueSet->end();
		++it)
	{
		errs() << "\nBasicBlock: " << (*it).first->getName() << "\n";
		ValueSet set = (*it).second;
		printValueSet(&set);
	}
	errs() << "\n";
}

std::list<ValueSet> returnLiveValuesSets()
{
	std::list<ValueSet> valueSetList;
	// for (auto &valueSet : IN)
	// {
		
	// }

	return valueSetList;
}


void LiveValues::printAllSets()
{
	errs() << "\n-------\nGEN SETS\n-------\n";
	printBBValueSet(&GEN);
	errs() << "\n-------\nKILL SETS\n-------\n";
	printBBValueSet(&KILL);
	errs() << "\n-------\nIN SETS\n-------\n";
	printBBValueSet(&IN);
	errs() << "\n-------\nOUT SETS\n-------\n";
	printBBValueSet(&OUT);
}


