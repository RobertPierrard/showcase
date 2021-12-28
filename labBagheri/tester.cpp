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

#define MaxNofChannels          512
#define ROIMaxNofChannels       36
#define MaxTimeBucket           512
#define HLEDMaxTimeBucket       10
#define NChannelsPerAget	    64

using namespace std;

void tester(){
	// var containing filename
	std::ifstream f_raw;

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

	char CoBo_filename[20];
	int file_index = 0;
	sprintf(CoBo_filename, "CoBo_data_%d.graw", file_index);

    // setup file so that frames and individual items can be read.
    f_raw.open(CoBo_filename, std::ios::in | std::ios::binary);
    mfm::Frame::seekFrame(f_raw, 0); //moves frame pointer to 0th frame
    curFrame = mfm::Frame::read(f_raw); //calling line goes to the next frame
    item = curFrame->itemAt(0);
    field = item.field("");
    agetIdxField = field.bitField("agetIdx");
    sampleValueField = field.bitField("sample");


	eventIdx_CoBo = curFrame->headerField("eventIdx").value<uint32_t>();
    asadIdx_CoBo = curFrame->headerField("asadIdx").value<uint8_t>();
    cout << "Cobo: Event ID: " << eventIdx_CoBo <<endl;
    cout << "Cobo: AsAd ID: " << (short) asadIdx_CoBo <<endl;


    // for(int frameCounter=0; frameCounter<20; frameCounter++){
     	// logCoBoEvent();
    // }

}