R__LOAD_LIBRARY(libget-root.so)
R__LOAD_LIBRARY(libMultiFrame.so)

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

#include <mfm/Frame.h>
#include <mfm/Item.h>
#include <mfm/Field.h>
#include <mfm/Serializer.h>
#include <mfm/BitField.h>
#include <mfm/Common.h>
#include <mfm/PrimaryHeader.h>
#include <mfm/Header.h>
#include <mfm/StandardHeader.h>
#include <mfm/BlobHeader.h>
#include <mfm/AbstractFieldContainer.h>
#include <mfm/FrameDictionary.h>

#include <TTree.h>

#include "TBDecoder.cpp"

#define Bifocal     1
#define DiscTest    2
#define Internal    4
#define External    8
#define GPS         16
#define HLED        32

#define ASAD0		0
#define ASAD1		1

#define MaxNofChannels          512
#define ROIMaxNofChannels       36
#define MaxTimeBucket           512
#define HLEDMaxTimeBucket       10
#define NChannelsPerAget	    64

using namespace std;

// Define self-made data types
struct STRUCT_Bifocal
{
    ULong64_t tb_time;
    ULong64_t cobo_time;
    ULong64_t unix_time;
    UInt_t ROIMusicID[2];
    UInt_t NofPixels;
    UInt_t ROIPixelID[ROIMaxNofChannels];
    UInt_t ROISignalValue[ROIMaxNofChannels][MaxTimeBucket];
    Float_t Pedestal[MaxNofChannels];
};

struct STRUCT_Background
{
    ULong64_t tb_time;
    ULong64_t cobo_time;
    ULong64_t unix_time;
    UInt_t SignalValue[MaxNofChannels][MaxTimeBucket];
};

struct STRUCT_Hled
{
    ULong64_t tb_time;
    ULong64_t cobo_time;
    ULong64_t unix_time;
    UInt_t SignalValue[MaxNofChannels][HLEDMaxTimeBucket];
};

// Class Definition
class EventBuilder{
private: //member variables
	// some vars
	int totalfiles;
	int numEvents;

	// pointer to TBDecoder object for accessing triggerboard data
	TBDecoder *TBEvents;

	// var containing filename
	std::ifstream f_raw;
	std::ifstream f_raw_0;
	std::ifstream f_raw_1;

	// vectors for storing the order of events in a file for each asadID
	// necessary because events are stored sequentially for each asad but not for the file as a whole.
    vector<int> vAsAd0;
    vector<int> vAsAd1;
    int numEventsFromVAsad;

	// mysterious mfm stuff
	std::auto_ptr<mfm::Frame> curFrame;
	mfm::Item item;
	mfm::Field field;
	mfm::BitField agetIdxField;
	mfm::BitField sampleValueField;

	// current frame header variables from cobo file. Note: 2 frames per event (1 for each asad)
    size_t itemCount_CoBo;
    uint64_t eventTime_CoBo;
    uint32_t eventIdx_CoBo;
    uint8_t asadIdx_CoBo;
    uint32_t eventIdx_CoBo_asad0; //used to confirm data from asad0 and asad1 are paired properly.

    // current event variables from trigger board
    int eventNumber_TB;
    int eventTime_TB;
    int triggerSource_TB;
    uint32_t FirstTrigMusic_TB;

	// pointers to TTrees and TFile for storing data
    TTree *Bifocal_tree;
    TTree *Background_tree;
    TTree *Hled_tree;

	// some constants
	// numChips corresponds with AGETs, numChannels with channel
	// there are actually only 64 channel indices, some are skipped
	const size_t numChips = 4u;
	const size_t numChannels = 68u;

public:
	// Constructors
	EventBuilder();

private:
	// helper function definitions
	void logTBevent(int eventID);
	uint16_t FindTrigSource(int eventID);
	int FindPixel(int nx, int ny);
	void FindBin(int iPix,int *nx, int *ny);
	void FindNeighborPixels(int FirstTrigMusic, vector<int> &vNeighbor);
	// void pixelMapDecoder();
	// void pixelMapEncoder();

	void LoadFirstCoBoFrame(char CoBo_filename[20], int asadID);
	bool FrameCounter(char CoBo_filename[20]);
	void logCoBoEvent(std::auto_ptr<mfm::Frame> &tempFrame);
	void Set_Branch_Bifocal(STRUCT_Bifocal *Bifocal_struct);
	void Set_Branch_Background(STRUCT_Background *Background_struct);
	void Set_Branch_Hled(STRUCT_Hled *Hled_struct);
	void Write_BifocalEvent(int event_index, STRUCT_Bifocal *Bifocal_struct);
	void Write_BackgroundEvent(STRUCT_Background *Background_struct);
	void StoreDataBackStruct_BackgroundEvent(char CoBo_filename[20], STRUCT_Background *Background_struct, int asadIdx, int event_index);
	void Write_HledEvent(STRUCT_Hled *Hled_struct);
	void Write_Tree_to_file();

public:
	// member function definitions
	void mainFlow();

};


