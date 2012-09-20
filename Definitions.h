#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#pragma warning (disable : 4786)

#include <fstream>

// #defines
#define ONE_POUND_GAME
//#define TWO_POUND_GAME

// constants
const int NUM_OF_REELS			= 5;
const int SYMBOLS_PER_REEL		= 3;
const int VISIBLE_REEL_SYMBOLS	= NUM_OF_REELS * SYMBOLS_PER_REEL;
const int MAX_REEL_LENGTH		= 38;
const int TOTAL_WINLINES		= 10;
const int TOTAL_REELS			= 5;
const int TOTAL_COLUMNS			= 3;
const int WIN_ARRAY_SIZE		= 50000 / 10;

enum SymbolImage {TEN, JACK, QUEEN, KING, ACE, CRAB, DIVER, LOBSTER, PUFF, SHARK, SHELL, STARFISH, WONGA};
enum EType {THREE, FOUR, FIVE};

// tables
int WinResultArray[WIN_ARRAY_SIZE] = {0};
int ReelScreen[NUM_OF_REELS][SYMBOLS_PER_REEL];
int VisibleFeatureScreen[1000][15] = {0};

/*  AC,		// Ace
	CR,		// Crab
	DV,		// Diver
	JA,		// Jack
	KN,		// King
	LO,		// Lobster
	PF,		// Puff
	QN,		// Queen
	SH,		// Shell
	SK,		// Shark
	ST,		// Starfish
	TE,		// Ten
	WN		// Wonga	*/
unsigned char OutcomeSymbols[13][3] = {
	"AC", "CR", "DV", "JA", "KN", "LO", "PF", "QN", "SH", "SK", "ST", "TN", "WN"
};

int Symbols[MAX_REEL_LENGTH] = 
{
	LOBSTER,
	PUFF,
	ACE, ACE, ACE,
	CRAB,
	KING, KING, KING,
	SHARK,
	QUEEN, QUEEN, QUEEN,
	SHELL,
	JACK, JACK, JACK,
	WONGA,
	TEN, TEN, TEN,
	QUEEN,
	LOBSTER,
	STARFISH,
	JACK,
	ACE,
	TEN, 
	PUFF,
	JACK,
	CRAB,
	TEN,
	SHARK,
	JACK,
	SHELL,
	TEN,
	WONGA,
	LOBSTER,
	PUFF
};

int CentreSymbols[MAX_REEL_LENGTH] = {
	PUFF,
	LOBSTER,
	ACE, ACE, ACE,
	PUFF,
	KING, KING, KING,
	CRAB,
	QUEEN, QUEEN, QUEEN,
	SHARK,
	JACK, JACK, JACK,
	SHELL,
	TEN, TEN, TEN,
	QUEEN,
	WONGA,
	LOBSTER,
	JACK,
	STARFISH,
	TEN,
	ACE,
	JACK,
	SHARK,
	TEN,
	PUFF,
	JACK,
	WONGA,
	TEN,
	SHELL,
	PUFF,
	LOBSTER 
};

#if defined ONE_POUND_GAME

int WinValueTable[6][4] = {
	0, 50, 200, 1000,	 // 10
	0, 100, 300, 1500,	 // Jack
	0, 150, 500, 2000,   // Queen
	0, 200, 1000, 2500,  // King
	0, 300, 1500, 5000,  // Ace
	100, 500, 2000, 5000 // Shark
};

#elif defined TWO_POUND_GAME

int WinValueTable[6][4] = {
	0, 100, 400, 2000,	 // 10
	0, 100, 600, 3000,	 // Jack
	0, 300, 1000, 4000,  // Queen
	0, 400, 2000, 5000,  // King
	0, 600, 3000, 5000,  // Ace
	0, 400, 1000, 5000	 // Shark
};

#endif

int WinLines[TOTAL_WINLINES][TOTAL_REELS] = {
	1,1,1,1,1,   // line 1
	0,0,0,0,0,	 // line 2
	2,2,2,2,2,   // line 3
	0,0,1,2,2,	 // line 4
	2,2,1,0,0,   // line 5 
	0,0,0,1,2,   // line 6
	2,2,2,1,0,	 // line 7
	2,1,0,0,0,   // line 8
	0,1,2,2,2,	 // line 9
	2,1,0,1,2,   // line 10
};

// structures
struct TestSymbol
{
	unsigned char _r1, _r2, _r3, _r4, _r5;
	
	TestSymbol() {}
	TestSymbol(unsigned char r1, unsigned char r2, unsigned char r3, unsigned char r4, unsigned char r5) :
	_r1(r1), _r2(r2), _r3(r3), _r4(r4), _r5(r5) {}

	~TestSymbol() {}
};

struct OutputFormat
{
	TestSymbol* topLine;
	TestSymbol* middleLine;
	TestSymbol* bottomLine;
	SymbolImage symbolFound;

	OutputFormat() {}
};

// functions
bool IsFiveOfAKind(const TestSymbol* t);
bool IsFourOfAKind(int index, const TestSymbol* t);
bool IsThreeOfAKind(int index, const TestSymbol* t);

void GetThreesWin(int sym);
void GetFoursWin(int sym);
void GetFivesWin(int sym);

int GetFeatureWinIndex(int symbol, int count);

void PickReels();
int SegRand();

void WriteFeature(const char* filename, const TestSymbol* t1, 
				  const TestSymbol* t2, const TestSymbol* t3);

void WriteWin(const char * winValue, TestSymbol* top, TestSymbol* mid, TestSymbol* bot);

void WriteLoses();

int CheckForWin();
int CheckForFeature();
void ExtractFeature(unsigned int feature, unsigned char * featureIndex, unsigned char * isFeature);
unsigned int PackFeature(const char isFeature, const char index);
void FoundFeature(int sym);
unsigned char* SelectSymbolForWrite(const int symbol);
void WriteToFile( std::ofstream& file, const TestSymbol* t1, const TestSymbol* t2, 
				 const TestSymbol* t3 );
int GetWinFrom1DIndex( int i );
void CleanUp();
int CountPuffs();
void CheckDiver();

#endif
