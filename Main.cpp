#include <iostream>
#include "Definitions.h"
#include <ctime>
#include <limits>
#include <cassert>
#include <cstdlib>

#include <map>
#include <vector>

using namespace std;

std::multimap<int, std::vector<TestSymbol*> > screens;

typedef struct stats_s {
	int count;
	int win;
} stats_t;

#ifdef _DEBUG
int FEATURE_WRITES = 0;
int FEATURE_COUNT = 0;
#endif

int currentWinline = 0;
int currentWinCount = 0;

namespace CurrentSymbol {
	static int CurrentEligibleSymbol;
};

namespace statics {
	static int accumulatedTotal;
	static int featureInfo;
	static const char * PATH = "outcomes\\";
};

void PrintWinValueConversion()
{
	int index = 0;
	int win = 0;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 3; j++) {
			index = GetFeatureWinIndex(i, j);
			win = GetWinFrom1DIndex(index);
			std::cout << "index : " << index << " : win " << win << std::endl;
		}
	}
}

void SetSampleCount( stats_t& stats )
{
	if (stats.count >= 1000) stats.count = 1000;
	else if (stats.count >= 500) stats.count = 500;
	else if (stats.count >= 100) stats.count = 100;
	else if (stats.count >= 30) stats.count = 30;
	else if (stats.count >= 10) stats.count = 10;
	else stats.count = 1;
}

void WriteWins();

int main(int argc, char *argv[]) {
	//PrintWinValueConversion();
	//srand(time(NULL));
	using namespace statics;
	accumulatedTotal = 0;

#ifdef _DEBUG
	FEATURE_WRITES = 0;
	FEATURE_COUNT = 0;
#endif

	CurrentSymbol::CurrentEligibleSymbol = 0;

	int cycle = 20000000;
	//int cycle = 100000;
	//int cycle = 10000;

	unsigned int feature = 0;
	unsigned char isFeature = 0;
	unsigned char featureIndex = 0;
	unsigned int winValue;
	unsigned int lossCount = 0;

	using namespace std; 
	
	do {
		cycle--;
		PickReels();
		
		feature = CheckForFeature();

		winValue = CheckForWin() / 10;		
		
		if (winValue > 0 && !feature) {
			WinResultArray[winValue]++;

			TestSymbol *top = new TestSymbol(ReelScreen[0][0], ReelScreen[1][0], ReelScreen[2][0],
				ReelScreen[3][0], ReelScreen[4][0]);
			TestSymbol *mid = new TestSymbol(ReelScreen[0][1], ReelScreen[1][1], ReelScreen[3][1],
				ReelScreen[3][1], ReelScreen[4][1]);
			TestSymbol *bot = new TestSymbol(ReelScreen[0][2], ReelScreen[1][2], ReelScreen[2][2],
				ReelScreen[3][2], ReelScreen[4][2]);
			
			vector<TestSymbol*> symbols;
				
			symbols.push_back(top);
			symbols.push_back(mid);
			symbols.push_back(bot);

#ifdef _DEBUG
#ifdef TESTING
			for (int i = 0;i<3;i++) {
				cout << (int)symbols[i]->_r1 << " " << (int)symbols[i]->_r2 << " "<< 
					(int)symbols[i]->_r3 << " " << (int)symbols[i]->_r4 << " " << (int)symbols[i]->_r5 << endl;
			}
#endif
#endif
			screens.insert(make_pair(winValue, symbols));
		}

		if (feature && !winValue) {
			int cs = feature >> 4; // current symbol being used 
			int count = feature & cs | 1; // the amount of them in the winline.
			int symbol = cs; 
			FoundFeature(symbol);
			feature = 0;
		}
		else if (lossCount < 100000) {
			WriteLoses();
			lossCount++;
		}

	} while (cycle);

	cycle = 0;
	
	WriteWins();

	//CleanUp();
	
	cout << "Press enter to quit.\n";
	cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
	return 0;
}

/* function bodies */
int SegRand() {
	return rand() % 36 + 1;
}

bool IsFiveOfAKind(const TestSymbol* t)
{
	assert (t != NULL);
	return ReelScreen[0][t->_r1] == ReelScreen[1][t->_r2] && 
		   ReelScreen[1][t->_r2] == ReelScreen[2][t->_r3] &&
		   ReelScreen[2][t->_r3] == ReelScreen[3][t->_r4] &&
		   ReelScreen[3][t->_r4] == ReelScreen[4][t->_r5];
}

bool IsFourOfAKind(int index, const TestSymbol* t)
{
	assert (t != NULL);
	if (index == 0) {
		return ReelScreen[0][t->_r1] == ReelScreen[1][t->_r2] && 
			   ReelScreen[1][t->_r2] == ReelScreen[2][t->_r3] &&
			   ReelScreen[2][t->_r3] == ReelScreen[3][t->_r4];
	}
	else {
		return ReelScreen[1][t->_r2] == ReelScreen[2][t->_r3] && 
			   ReelScreen[2][t->_r3] == ReelScreen[3][t->_r4] &&
		       ReelScreen[3][t->_r4] == ReelScreen[4][t->_r5];
	}
}

bool IsThreeOfAKind(int index, const TestSymbol* t)
{
	assert (t != NULL);
	
	if (index == 0) {
		return ReelScreen[0][t->_r1] == ReelScreen[1][t->_r2] && 
			   ReelScreen[1][t->_r2] == ReelScreen[2][t->_r3];
	}
	else if (index == 1) {
		return ReelScreen[1][t->_r2] == ReelScreen[2][t->_r3] && 
			   ReelScreen[2][t->_r3] == ReelScreen[3][t->_r4];
	}
	else {
		return ReelScreen[2][t->_r3] == ReelScreen[3][t->_r4] && 
			   ReelScreen[3][t->_r4] == ReelScreen[4][t->_r5];
	}
}

int CheckForWin() 
{
	using namespace statics;
	using namespace CurrentSymbol;

	for (int i = 0; i < TOTAL_WINLINES; ++i) {		
		int pos1 = WinLines[i][0];
		int pos2 = WinLines[i][1];
		int pos3 = WinLines[i][2];
		int pos4 = WinLines[i][3];
		int pos5 = WinLines[i][4];
			
		TestSymbol symbols(pos1,pos2,pos3,pos4,pos5);

		if (IsFiveOfAKind(&symbols)) {
			CurrentEligibleSymbol = ReelScreen[0][pos1];
			if (CurrentEligibleSymbol < ACE)
				GetFivesWin(CurrentEligibleSymbol);
		} // 4 from the left.
		else if (IsFourOfAKind(0, &symbols)) {
			CurrentEligibleSymbol = ReelScreen[0][pos1];
			if (CurrentEligibleSymbol < ACE)
				GetFoursWin(CurrentEligibleSymbol);
		} // 4 from the right.
		else if (IsFourOfAKind(1, &symbols)) {
			CurrentEligibleSymbol = ReelScreen[1][pos2];
			if (CurrentEligibleSymbol < ACE)
				GetFoursWin(CurrentEligibleSymbol);
		} // 3 from the left.
		else if (IsThreeOfAKind(0, &symbols)) {
			CurrentEligibleSymbol = ReelScreen[0][pos1];
			if (CurrentEligibleSymbol < ACE)
				GetThreesWin(CurrentEligibleSymbol);
		} // three from centre.
		else if (IsThreeOfAKind(1, &symbols)) {
			CurrentEligibleSymbol = ReelScreen[1][pos2];
			if (CurrentEligibleSymbol < ACE)
				GetThreesWin(CurrentEligibleSymbol);
		} // three from the right
		else if (IsThreeOfAKind(2, &symbols)) {
			CurrentEligibleSymbol = ReelScreen[2][pos3];
			if (CurrentEligibleSymbol < ACE)
				GetThreesWin(CurrentEligibleSymbol);
		}
	}
	
	//FoundFeature(symbolValue);

	// set featureFound flag if we find a feature.

	int total = accumulatedTotal;
	accumulatedTotal = 0;
	return total;
}

void ExtractFeature( unsigned int feature, unsigned char * featureIndex, 
					 unsigned char * isFeature )
{
	*featureIndex = feature;
	feature >>= 8;	
	*isFeature = feature;
}

// pass in the win value instead?
unsigned int PackFeature(const char isFeature, const char index)
{
	unsigned int feature = 0;
	feature = isFeature;
	feature <<= 8;
	feature |= index;
	
	return feature;
}

int CheckForFeature()
{
	using namespace statics;
	using namespace CurrentSymbol;

	int featureInfo = 0; // return value;
	accumulatedTotal = 0;

	unsigned char isFeature = 1;
	unsigned char featureWinIndex = 0;

	int tempSymbol = 0;
	int numOfFeatures = 0;

	int featureData = 0;

	for (int i = 0; i < TOTAL_WINLINES; i++) {
		int pos1 = WinLines[i][0];
		int pos2 = WinLines[i][1];
		int pos3 = WinLines[i][2];
		int pos4 = WinLines[i][3];
		int pos5 = WinLines[i][4];
		
		TestSymbol symbol(pos1, pos2, pos3, pos4, pos5);

		//if (numOfFeatures > 0) break;
		
		if (IsFiveOfAKind(&symbol)) {
			currentWinCount = 5;
			CurrentEligibleSymbol = ReelScreen[0][pos1];
			if(CurrentEligibleSymbol>ACE) {
				featureData = (CurrentEligibleSymbol << 4);
				featureData |= currentWinCount;
				//featureWinIndex = GetFeatureWinIndex(CurrentEligibleSymbol, 5);
				//featureInfo = PackFeature(isFeature, featureWinIndex);
				++numOfFeatures;
				currentWinline = i;
#ifdef _DEBUG
				FEATURE_COUNT++;
				
#endif 
			}
		}
		
		// from the left
		else if (IsFourOfAKind(0, &symbol)) {
			currentWinCount = 4;
			CurrentEligibleSymbol = ReelScreen[0][pos1];
			if(CurrentEligibleSymbol>ACE) {
				featureData = (CurrentEligibleSymbol << 4);
				featureData |= currentWinCount;
				//featureWinIndex = GetFeatureWinIndex(CurrentEligibleSymbol, 4);
				//featureInfo = PackFeature(isFeature, featureWinIndex);			
				++numOfFeatures;
				currentWinline = i;
#ifdef _DEBUG
				FEATURE_COUNT++;
#endif 
			}
		}
		
		// from the right
		else if (IsFourOfAKind(1, &symbol)) {
			currentWinCount = 4;
			CurrentEligibleSymbol = ReelScreen[1][pos2];
			if(CurrentEligibleSymbol>ACE) {
				featureData = (CurrentEligibleSymbol << 4);
				featureData |= currentWinCount;
				//featureWinIndex = GetFeatureWinIndex(CurrentEligibleSymbol, 4);
				//featureInfo = PackFeature(isFeature, featureWinIndex);
				++numOfFeatures;
				currentWinline = i;
#ifdef _DEBUG
				FEATURE_COUNT++;
#endif 
			}
		}
		
		// from the left
		else if (IsThreeOfAKind(0, &symbol)) {
			currentWinCount = 3;
			CurrentEligibleSymbol = ReelScreen[0][pos1];
			if(CurrentEligibleSymbol>ACE) {
				featureData = (CurrentEligibleSymbol << 4);
				featureData |= currentWinCount;
				//featureWinIndex = GetFeatureWinIndex(CurrentEligibleSymbol, 3);
				//featureInfo = PackFeature(isFeature, featureWinIndex);
				++numOfFeatures;
				currentWinline = i;
#ifdef _DEBUG
				FEATURE_COUNT++;
#endif 
			}
		}
		
		// centre 
		else if (IsThreeOfAKind(1, &symbol)) {
			currentWinCount = 3;
			CurrentEligibleSymbol = ReelScreen[1][pos2];
			if(CurrentEligibleSymbol>ACE) {
				featureData = (CurrentEligibleSymbol<<4);
				featureData |= currentWinCount;
				//featureWinIndex = GetFeatureWinIndex(CurrentEligibleSymbol, 3);
				//featureInfo = PackFeature(isFeature, featureWinIndex);
				++numOfFeatures;
				currentWinline = i;
#ifdef _DEBUG
				FEATURE_COUNT++;
#endif
			}
		}
		
		// from the right
		else if (IsThreeOfAKind(2, &symbol)) {
			currentWinCount = 3;
			CurrentEligibleSymbol = ReelScreen[2][pos3];
			if(CurrentEligibleSymbol>ACE) {
				featureData = (CurrentEligibleSymbol << 4);
				featureData |= currentWinCount;
				//featureWinIndex = GetFeatureWinIndex(CurrentEligibleSymbol, 3);
				//featureInfo = PackFeature(isFeature, featureWinIndex);
				++numOfFeatures;
				currentWinline = i;
#ifdef _DEBUG
				++FEATURE_COUNT;
#endif
			}
		}
	}

	if (numOfFeatures > 1)
		return 0;

	return featureData;
}

void PickReels() 
{
	int index = SegRand();
	int reel = 0;
	int reel_position = 0;
	
	for (; reel < NUM_OF_REELS; ++reel) {
		if (reel != 2) {
			ReelScreen[reel][reel_position] = Symbols[index-1];
			ReelScreen[reel][++reel_position] = Symbols[index];
			ReelScreen[reel][++reel_position] = Symbols[index+1];
		}	
		else {
			ReelScreen[reel][reel_position] = CentreSymbols[index-1];
			ReelScreen[reel][++reel_position] = CentreSymbols[index];
			ReelScreen[reel][++reel_position] = CentreSymbols[index+1];
		}

		index = SegRand();
		reel_position = 0;
	}
}

void GetFivesWin( int sym )
{
	using namespace statics;
	
	switch (sym) {
	case TEN: // Ten
		accumulatedTotal += WinValueTable[0][2]; break;	
	case JACK: // Jack
		accumulatedTotal += WinValueTable[1][2]; break;
	case QUEEN: // Queen
		accumulatedTotal += WinValueTable[2][2]; break;
	case KING: // King						
		accumulatedTotal += WinValueTable[3][2]; break; // £25 
	case ACE: // Ace
		accumulatedTotal += WinValueTable[4][2]; break;	
	default:
		//	featureFound = true;
		accumulatedTotal += 0; break;
	}
}

void GetFoursWin(int sym)
{
	using namespace statics;
	switch (sym) {
	case TEN: // Ten 0
		accumulatedTotal += WinValueTable[0][1]; break;
	case JACK: // Jack 1
		accumulatedTotal += WinValueTable[1][1]; break;	
	case QUEEN: // Queen 2
		accumulatedTotal += WinValueTable[2][1]; break;
	case KING: // King 3
		accumulatedTotal += WinValueTable[3][1]; break; // £10
	case ACE: // Ace 4
		accumulatedTotal += WinValueTable[4][1]; break;
	default:
		//	featureFound = true;
		accumulatedTotal += 0; break;
	}
}

void GetThreesWin(int sym)
{
	using namespace statics;
	switch (sym) {
	case TEN: // Ten
		accumulatedTotal += WinValueTable[0][0]; break;
	case JACK: // Jack
		accumulatedTotal += WinValueTable[1][0]; break;
	case QUEEN: // Queen
		accumulatedTotal += WinValueTable[2][0]; break;
	case KING: // King
		accumulatedTotal += WinValueTable[3][0]; break; // £25	
	case ACE: // Ace
		accumulatedTotal += WinValueTable[4][0]; break;
	default:
		//	featureFound = true;
		accumulatedTotal += 0; break;				
	}
}

void FoundFeature(int sym) 
{
	char filename[60];
	strcpy(filename, statics::PATH);
	strcat(filename, "features\\");

	switch (sym) {
	case ACE:
		strcat(filename, "ACE_OUTCOMES.txt"); break;
	case TEN:
		strcat(filename, "TEN_OUTCOMES.txt"); break;
	case JACK:
		strcat(filename, "JACK_OUTCOMES.txt"); break;
	case QUEEN:
		strcat(filename, "QUEEN_OUTCOMES.txt"); break;
	case KING:
		strcat(filename, "KING_OUTCOMES.txt"); break;
	case SHARK:
		strcat(filename, "SHARK_OUTCOMES.txt"); break;
	case CRAB:
		strcat(filename, "CRAB_OUTCOMES.txt"); break;
	case SHELL:
		strcat(filename, "SHELL_OUTCOMES.txt"); break;
	case PUFF:
		strcat(filename, "PUFF_OUTCOMES.txt"); break;
	case WONGA:
		strcat(filename, "WONGA_OUTCOMES.txt"); break;
	case LOBSTER:
		strcat(filename, "LOBSTER_OUTCOMES.txt"); break;
	case STARFISH:
		strcat(filename, "STARFISH_OUTCOMES.txt"); break;
	case DIVER:
		strcat(filename, "DIVER_OUTCOMES.txt"); break;
	default:
		return;
	}

	TestSymbol top;
	top._r1 = ReelScreen[0][0]; top._r2 = ReelScreen[1][0]; top._r3 = ReelScreen[2][0];
	top._r4 = ReelScreen[3][0]; top._r5 = ReelScreen[4][0];

	TestSymbol middle;
	middle._r1 = ReelScreen[0][1]; middle._r2 = ReelScreen[1][1]; middle._r3 = ReelScreen[2][1];
	middle._r4 = ReelScreen[3][1]; middle._r5 = ReelScreen[4][1];

	TestSymbol bottom;
	bottom._r1 = ReelScreen[0][2]; bottom._r2 = ReelScreen[1][2]; bottom._r3 = ReelScreen[2][2];
	bottom._r4 = ReelScreen[3][2]; bottom._r5 = ReelScreen[4][2];

	WriteFeature(filename, &top, &middle, &bottom);
}

void WriteFeature(const char* filename, const TestSymbol* t1, const TestSymbol* t2, 
				  const TestSymbol* t3) 
{
	assert(filename != NULL && t1 != NULL && t2 != NULL && t3 != NULL);
	
	ofstream file;
	file.open(filename, ios::app|ios::out);
	
	WriteToFile(file, t1, t2, t3);

	file.close();
}

void WriteToFile( ofstream& file, const TestSymbol* t1, 
				  const TestSymbol* t2, const TestSymbol* t3 )
{
	assert (t1 != NULL && t2 != NULL && t3 != NULL);

#ifdef _DEBUG
	++FEATURE_WRITES;

	file << "{" << SelectSymbolForWrite(t1->_r1) << ", " << SelectSymbolForWrite(t1->_r2) << ", "
		<< SelectSymbolForWrite(t1->_r3) << ", " << SelectSymbolForWrite(t1->_r4) << ", " << 
		SelectSymbolForWrite(t1->_r5) << ",\n ";
	
	file << SelectSymbolForWrite(t2->_r1) << ", " << SelectSymbolForWrite(t2->_r2) << ", "
		<< SelectSymbolForWrite(t2->_r3) << ", " << SelectSymbolForWrite(t2->_r4) << ", " << 
		SelectSymbolForWrite(t2->_r5) << ",\n ";
	
	file << SelectSymbolForWrite(t3->_r1) << ", " << SelectSymbolForWrite(t3->_r2) << ", "
		<< SelectSymbolForWrite(t3->_r3) << ", " << SelectSymbolForWrite(t3->_r4) << ", " << 
		SelectSymbolForWrite(t3->_r5) << "}," << " // winline = " << currentWinline 
		<< ", winCount = " << currentWinCount << "\n\n";
#else
	file << "{" << SelectSymbolForWrite(t1->_r1) << ", " << SelectSymbolForWrite(t1->_r2) << ", "
		<< SelectSymbolForWrite(t1->_r3) << ", " << SelectSymbolForWrite(t1->_r4) << ", " << 
		SelectSymbolForWrite(t1->_r5) << ",\t";
	
	file << SelectSymbolForWrite(t2->_r1) << ", " << SelectSymbolForWrite(t2->_r2) << ", "
		<< SelectSymbolForWrite(t2->_r3) << ", " << SelectSymbolForWrite(t2->_r4) << ", " << 
		SelectSymbolForWrite(t2->_r5) << ",\t";
	
	file << SelectSymbolForWrite(t3->_r1) << ", " << SelectSymbolForWrite(t3->_r2) << ", "
		<< SelectSymbolForWrite(t3->_r3) << ", " << SelectSymbolForWrite(t3->_r4) << ", " << 
		SelectSymbolForWrite(t3->_r5) << "}," << " // winline = " << currentWinline 
		<< ", winCount = " << currentWinCount << "\n";
#endif
}

unsigned char * SelectSymbolForWrite(const int symbol)
{
	unsigned char * returnCode;
	switch (symbol) {
	case ACE:
		returnCode = &OutcomeSymbols[0][0];
		break;
	case CRAB:
		returnCode = &OutcomeSymbols[1][0];
		break;
	case DIVER:
		returnCode = &OutcomeSymbols[2][0];
		break;
	case JACK:
		returnCode = &OutcomeSymbols[3][0];
		break;
	case KING:
		returnCode = &OutcomeSymbols[4][0];
		break;
	case LOBSTER:
		returnCode = &OutcomeSymbols[5][0];
		break;
	case PUFF:
		returnCode = &OutcomeSymbols[6][0];
		break;
	case QUEEN:
		returnCode = &OutcomeSymbols[7][0];
		break;
	case SHELL:
		returnCode = &OutcomeSymbols[8][0];
		break;
	case SHARK:
		returnCode = &OutcomeSymbols[9][0];
		break;
	case STARFISH:
		returnCode = &OutcomeSymbols[10][0];
		break;
	case TEN:
		returnCode = &OutcomeSymbols[11][0];
		break;
	case WONGA:
		returnCode = &OutcomeSymbols[12][0];
		break;
	}
	
	assert(returnCode != NULL);
	return returnCode;
}

int GetFeatureWinIndex(int symbol, int count)
{
	if(symbol > ACE) {
		int row = symbol;
		int column = (count == 0 || count == 1 || count == 2) ? count : count - SYMBOLS_PER_REEL;
		return row * SYMBOLS_PER_REEL + column; 
	}
	return 0;
}

int GetWinFrom1DIndex( int i )
{
	int indexR = i / SYMBOLS_PER_REEL;
	int indexC = i % SYMBOLS_PER_REEL;
	
	return WinValueTable[indexR][indexC];
}

//void WriteWin(const char* winValue, const TestSymbol* top, const TestSymbol* mid,
//			  const TestSymbol* bot)
void WriteWin(const char* winValue, TestSymbol* top, TestSymbol* mid, TestSymbol* bot)
{
	ofstream file;
	char filename[60];
	strcpy(filename, statics::PATH);
	strcat(filename, "\\wins\\");
	strcat(filename, winValue);
	strcat(filename, ".txt");

	file.open(filename, ios::app|ios::out);
	
	/*TestSymbol top;
	top._r1 = ReelScreen[0][0]; top._r2 = ReelScreen[1][0]; top._r3 = ReelScreen[2][0];
	top._r4 = ReelScreen[3][0]; top._r5 = ReelScreen[4][0];
	
	TestSymbol middle;
	middle._r1 = ReelScreen[0][1]; middle._r2 = ReelScreen[1][1]; middle._r3 = ReelScreen[2][1];
	middle._r4 = ReelScreen[3][1]; middle._r5 = ReelScreen[4][1];
	
	TestSymbol bottom;
	bottom._r1 = ReelScreen[0][2]; bottom._r2 = ReelScreen[1][2]; bottom._r3 = ReelScreen[2][2];
	bottom._r4 = ReelScreen[3][2]; bottom._r5 = ReelScreen[4][2];*/
	
	WriteToFile(file, top, mid, bot);
	
	file.close();
}

void WriteLoses()
{
	ofstream file;
	file.open("outcomes\\loses\\lose.txt", ios::app|ios::out);
	
	TestSymbol top;
	top._r1 = ReelScreen[0][0]; top._r2 = ReelScreen[1][0]; top._r3 = ReelScreen[2][0];
	top._r4 = ReelScreen[3][0]; top._r5 = ReelScreen[4][0];

	TestSymbol middle;
	middle._r1 = ReelScreen[0][0]; middle._r2 = ReelScreen[1][0]; middle._r3 = ReelScreen[2][0];
	middle._r4 = ReelScreen[3][0]; middle._r5 = ReelScreen[4][0];

	TestSymbol bottom;
	bottom._r1 = ReelScreen[0][0]; bottom._r2 = ReelScreen[1][0]; bottom._r3 = ReelScreen[2][0];
	bottom._r4 = ReelScreen[3][0]; bottom._r5 = ReelScreen[4][0];

	WriteToFile(file, &top, &middle, &bottom);

	file.close();
}

void CleanUp()
{
	if (!screens.empty()) {
		multimap<int, vector<TestSymbol*> >::iterator it = screens.begin();
		for (; it != screens.end(); it) {
			vector<TestSymbol*>::iterator reelItor = it->second.begin();
			for (; reelItor != it->second.end(); reelItor++)
				delete *reelItor;

			//it->second.clear();
		}
	}
}

void WriteWins()
{
	multimap<int, vector<TestSymbol*> >::const_iterator it = screens.begin();
	
	stats_t stats;
	
	stats.win = it->first;
	stats.count = screens.count(it->first);
	
	SetSampleCount(stats);
	
	int counter = 0;
	
	while (it != screens.end()) {
		if (it->first /*== lastWin*/ && counter < stats.count) {
			char w[4];
			itoa(stats.win, w, 10);
			WriteWin(w, it->second[0], it->second[1], it->second[2]);
			counter++;
		}

		it++;
		
		if (counter == stats.count) {
			it = screens.upper_bound(stats.win);
			stats.win = it->first;
			stats.count = screens.count(it->first);
			SetSampleCount(stats);
			counter = 0;
		}
	}
}