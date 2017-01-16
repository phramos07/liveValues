//===------------------------------- TaskMiner.h --------------------------===//
//
// Authors: Pedro Ramos (pedroramos@dcc.ufmg.br)
//
//
//===----------------------------------------------------------------------===//
//
//                           The LLVM Compiler Infrastructure
//
//	This Pass implements LIVENESS RANGE ANALYSIS for the LLVM's IR. It computes
//the live sets for each basic block, which basically is the sets of values alive
//in each basic block of the LLVM's IR. 
//
//
//===----------------------------------------------------------------------===//

#ifndef __LIVE_VALUES_H__
#define __LIVE_VALUES_H__

//LLVM IMPORTS
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/Interval.h"
#include "llvm/ADT/Statistic.h"


//STL IMPORTS
#include <set>
#include <list>
#include <stack>

namespace llvm
{
	typedef std::set<Value*> ValueSet;
	typedef std::map<BasicBlock*, ValueSet> BBValueSets;
	typedef std::stack<BasicBlock*> BBList;

	class LiveValues : public FunctionPass
	{
	public:
		static char ID;
		LiveValues() : FunctionPass(ID) {}
		~LiveValues() {};
		void getAnalysisUsage(AnalysisUsage &AU) const override;
		bool runOnFunction(Function &func) override;

	private:
		BBValueSets GEN;
		BBValueSets KILL;
		BBValueSets IN;
		BBValueSets OUT;
		BBList worklist;
		ValueSet allValues;
		ValueSet getLiveValuesSet_OUT(BasicBlock* BB);
		ValueSet getLiveValuesSet_IN(BasicBlock* BB);
		bool compareTwoValueSets(ValueSet* A, ValueSet* B);
		ValueSet getDefinedValues(Instruction* I);
		ValueSet getDefinedValues(BasicBlock* BB);
		ValueSet getUsedValues(Instruction* I);
		ValueSet getUsedValues(BasicBlock* BB);
		void addToWorkList(BasicBlock* BB);
		BasicBlock* getNextBasicBlock();
		void __init__(Function* F);
		void printValueSet(ValueSet* set);
		void printAllSets();
		void printBBValueSet(BBValueSets *bbValueSet);
		std::list<ValueSet> returnLiveValuesSets();
	};


}


#endif