//
// Created by Yusuf Pisan on 4/26/18.
//

#include <climits>
#include <cstdlib>
#include <iostream>

#include "skiplist.h"

using namespace std;

// display with level
ostream &operator<<(ostream &Out, const SkipList &SkipL) {
	for (int I = SkipL.Depth - 1; I >= 0; I--) {
		Out << "Level: " << I << " -- ";
		SkipList::SNode* CurPtr = SkipL.FrontGuards[I];
		while(CurPtr != nullptr){
			Out << CurPtr->Data << ", ";
			CurPtr = CurPtr->Next;
		}
		Out << endl;
	}
  return Out;
}

//SNode constructor initializes Data
SkipList::SNode::SNode(int Data) : 
	Data(Data), Next(nullptr), Prev(nullptr), UpLevel(nullptr), DownLevel(nullptr) 
{}

//SkipList constructor initializes FrontGuards and RearGuards
//default SkipList has Depth of 1, just one doubly-linked list
SkipList::SkipList(int Depth) : Depth(Depth) {
	FrontGuards = new SNode * [Depth];
	RearGuards = new SNode * [Depth];
	for (int I = 0; I < Depth; I++) {
		FrontGuards[I] = new SNode(INT_MIN);
		RearGuards[I] = new SNode(INT_MAX);
	}
	for (int I = 0; I < Depth - 1; I++) {
		FrontGuards[I]->UpLevel = FrontGuards[I + 1];
		RearGuards[I]->UpLevel = RearGuards[I + 1];
	}
	for (int I = Depth - 1; I > 0; I--) {
		FrontGuards[I]->DownLevel = FrontGuards[I - 1];
		RearGuards[I]->DownLevel = RearGuards[I - 1];
	}
	for (int I = 0; I < Depth; I++) {
		FrontGuards[I]->Next = RearGuards[I];
		RearGuards[I]->Prev = FrontGuards[I];
	}
}

// return true 50% of time,
// each node has a 50% chance of being at higher level
bool SkipList::alsoHigher() const {
	return (rand() % 2) == 1;
}

// return true if successfully added, no duplicates
bool SkipList::add(int Data) {
	SNode* CurPtr = FrontGuards[0];
	while (CurPtr != nullptr) {
		if (CurPtr->Data == Data) {
			cout << "Duplicates not allowed:" << Data << endl;
			return false;
		}
		CurPtr = CurPtr->Next;
	}
	CurPtr = FrontGuards[0];
	SNode* NodeToAddPtr = new SNode(Data);
	bool Added = false;
	while (CurPtr != nullptr && !Added) {
		if (Data < CurPtr->Data) {
			addBefore(NodeToAddPtr, CurPtr);
			Added = true;
		}
		CurPtr = CurPtr->Next;
	}
	int CurrentLevel = 1;
	while (alsoHigher() && CurrentLevel < Depth && Added) {
		CurPtr = FrontGuards[CurrentLevel];
		SNode* NodeToAddPtr = new SNode(Data);
		bool SubAdded = false;
		while (CurPtr != nullptr && !SubAdded) {
			if (Data < CurPtr->Data) {
				addBefore(NodeToAddPtr, CurPtr);
				SNode* BelowPtr = find(Data, CurrentLevel - 1);
				BelowPtr->UpLevel = NodeToAddPtr;
				NodeToAddPtr->DownLevel = BelowPtr;
				SubAdded = true;
			}
			CurPtr = CurPtr->Next;
		}
		CurrentLevel++;
	}
	return Added;
}

// given a SNode, place it before the given NextNode
void SkipList::addBefore(SNode *NewNode, SNode *NextNode) {
	NewNode->Prev = NextNode->Prev;
	NewNode->Next = NextNode;
	NewNode->Prev->Next = NewNode;
	NextNode->Prev = NewNode;
}

// destructor
SkipList::~SkipList() {
	for (int I = 0; I < Depth; I++) {
		SNode* CurPtr = FrontGuards[I];
		while (CurPtr != nullptr) {
			SNode* DeletePtr = CurPtr;
			CurPtr = CurPtr->Next;
			delete DeletePtr;
		}
	}
	delete[] FrontGuards;
	delete[] RearGuards;
}

// return true if successfully removed
bool SkipList::remove(int Data) {
	if (find(Data, 0) == nullptr) {
		return false;
	}
	SNode* CurPtr = find(Data, 0);
	while (CurPtr != nullptr) {
		CurPtr->Prev->Next = CurPtr->Next;
		CurPtr->Next->Prev = CurPtr->Prev;
		SNode* DeletePtr = CurPtr;
		CurPtr = CurPtr->UpLevel;
		delete DeletePtr;
	}
	return true;
}

// return true if found in SkipList
bool SkipList::contains(int Data) {
	SNode* CurPtr = FrontGuards[Depth - 1];
	while (CurPtr != nullptr) {
		if (CurPtr->Next->Data < Data) {
			CurPtr = CurPtr->Next;
		}
		if (CurPtr->Next->Data == Data) {
			return true;
		}
		else {
			CurPtr = CurPtr->DownLevel;
		}
	}
	return false;
}

// find function
SkipList::SNode* SkipList::find(int Data, int Level) {
	SNode* CurPtr = FrontGuards[Level];
	while (CurPtr != nullptr) {
		if (CurPtr->Data == Data) {
			return CurPtr;
		}
		CurPtr = CurPtr->Next;
	}
	return CurPtr;
}
