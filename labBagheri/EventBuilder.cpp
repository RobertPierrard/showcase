R__LOAD_LIBRARY(libget-root.so)
R__LOAD_LIBRARY(libMultiFrame.so)

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "TBDecoder.cpp"

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

#define Bifocal     1
#define DiscTest    2
#define Internal    4
#define External    8
#define GPS         16
#define LED         32

using namespace std;

/**
 * Prints out various parameters for a specified eventID in a TBDecoder object.
 * @param TBEvents is a TBDecoder object which stores the data for N events from the TriggerBoard.
 * @param eventID is an integer specifying the event to print.
 */
void logTBevent(TBDecoder TBEvents, int eventID)
{
    cout << "Event number: " << TBEvents.getEventNumber(eventID) << endl;
    cout << "Clock Count: " << TBEvents.getClockCount(eventID) << endl;
    cout << "Trigger Source: " << (uint16_t)TBEvents.getTriggerSource(eventID) << endl;
    cout << "Confirm Event #: " << TBEvents.confirmEventNumber(eventID) << endl;
    cout << "Active SDB: " << (int)TBEvents.getActiveSdb(eventID) << endl;
}

/**
 * Why does this function exist?
 */
uint16_t FindTrigSource(TBDecoder TBEvents, int eventID)
{
    uint16_t TrigSource = 0;
    TrigSource = (uint16_t)TBEvents.getTriggerSource(eventID);
    return TrigSource;
}

/**
 * Given x and y coordinates of a pixel, returns the pixelID. Bottom left is x=y=0.
 * @param nx x coordinate
 * @param ny y coordinate
 * @return PixelID
 */
int FindPixel(int nx, int ny)
{
    int pixelID;
    pixelID = (nx*4+(ny%4)+(ny/4)*128);
    return pixelID;
}


/**
 * Given a pixel, return the x and y coords via a pointer.
 * @param pixel
 * @param nx x coordinate
 * @param ny y coordinate
 */
void FindBin(int pixelID,int *nx, int *ny)
{
    int MUSIC_ID = pixelID/8;
    int MUSIC_Channel = pixelID%8; 
    int MUSIC_row = MUSIC_ID/16;
    int MUSIC_column = MUSIC_ID%16;

    *ny = 4*MUSIC_row+MUSIC_Channel%4; 
    *nx = 2*MUSIC_column+MUSIC_Channel/4; 
}

/**
 * Function that uses a CSV lookup table to return asadID, agetID, and channelID given sipmID.
 * @param string pixelMapCSV the csv file containing the lookup table. Assumming format of the csv is the following:
 * first row is a header row. Columns are in order sipmID, asadID, agetID, channelID.
 * @param sipmID the search parameter
 * @param *asadID one of the return parameters
 * @param *agetID one of the return parameters
 * @param *channelID one of the return parameters
**/
void pixelMapDecoder(string pixelMapCSV, int sipmID, int *asadID, int *agetID, int *channelID){
	// create input file stream
	ifstream pixelMap(pixelMapCSV);

	// make sure file is open
	if(!pixelMap.is_open()) throw runtime_error("Can not open pixelMapCSV file.");

	// vars for storing temp data
	string line;
	string curVal;

	// read header line
	getline(pixelMap,line);
	// read data lines
	while(getline(pixelMap,line)){
		// search for desired sipmID
		curVal = line.substr(0,line.find(","));
		if(stoi(curVal)==sipmID) break;
	}

	cout << "return vals: " << endl;

	// set associated return values
	line = line.substr(line.find(",")+1);
	*asadID = stoi(line.substr(0,line.find(",")));
	line = line.substr(line.find(",")+1);
	*agetID = stoi(line.substr(0,line.find(",")));
	line = line.substr(line.find(",")+1);
	*channelID = stoi(line.substr(0,line.find(",")));

	// close the file.
	pixelMap.close();
}

/**
 * Function that uses a CSV lookup table to return sipmID given asadID, agetID, and channelID. Completes the opposite
 * of pixelMapDecoder.
 * @param string pixelMapCSV the csv file containing the lookup table. Assumming format of the csv is the following:
 * first row is a header row. Columns are in order sipmID, asadID, agetID, channelID.
 * @param *sipmID the return parameter
 * @param asadID one of the search parameters
 * @param agetID one of the search parameters
 * @param channelID one of the search parameters
**/
void pixelMapEncoder(string pixelMapCSV, uint16_t *sipmID, uint16_t asadID, uint16_t agetID, uint16_t channelID){
	// create input file stream
	ifstream pixelMap(pixelMapCSV);

	// make sure file is open
	if(!pixelMap.is_open()) throw runtime_error("Can not open pixelMapCSV file.");

	// vars for storing temp data
	string line;
	string sSipm, sAsad, sAget, sChannel;

	// read header line
	getline(pixelMap,line);
	// read data lines
	while(getline(pixelMap,line)){
		// extract IDs from line
		sSipm = line.substr(0,line.find(","));
		line = line.substr(line.find(",")+1);
		sAsad = line.substr(0,line.find(","));
		line = line.substr(line.find(",")+1);
		sAget = line.substr(0,line.find(","));
		line = line.substr(line.find(",")+1);
		sChannel = line.substr(0,line.find(","));

		// check if desired asadID, agetID, and channelID match those in the line - if so set the return param (sipmID)
		if((stoi(sAsad)==asadID) && (stoi(sAget)==agetID) && (stoi(sChannel)==channelID)){
			*sipmID = stoi(sSipm);
		}
	}

	// close the file.
	pixelMap.close();
}

/**
 * Takes first triggered music id and fills a vector with all its pixels plus its neighbors, it returns this vector
 * via a pointer.
 * @param FirstTrigMusic the music ID of the triggered pixel
 * @param &vNeighbor the return vector containing all pixels and neighbor pixels.
 */
void FindNeighborPixels(int FirstTrigMusic, vector<int> &vNeighbor)
{
    int nx, ny;
    int FirstTrigPix = FirstTrigMusic * 8;
    FindBin(FirstTrigPix, &nx, &ny);
    
    for (int j=ny-1; j<ny+5 && j<16; j++)
    {
        for (int i=nx-1; i<nx+5 && i<32; i++)
        {
            if((i > -1) && (j > -1))
            {
                int tmp = FindPixel(i, j);
                if((tmp > -1) && (tmp < 512)){
                    vNeighbor.push_back(tmp);
                }
            }
        }
    }
    std::sort (vNeighbor.begin(), vNeighbor.end());
}


/**
 * 
 */
void LoadFirstCoBoFrame(std::ifstream &f_raw, mfm::Field &field, mfm::BitField &agetIdxField, mfm::BitField &sampleValueField, std::auto_ptr<mfm::Frame> &myFrame)
{
    mfm::Frame::seekFrame(f_raw, 0);
    myFrame = mfm::Frame::read(f_raw);
    mfm::Item item = myFrame->itemAt(0);
    field = item.field("");
    agetIdxField = field.bitField("agetIdx");
    sampleValueField = field.bitField("sample");    
}

void logCoBoevent(std::auto_ptr<mfm::Frame> &myFrame)
{
    size_t itemCount   = myFrame->itemCount();
    uint64_t eventTime = myFrame->headerField("eventTime").value<uint64_t>();
    uint32_t eventIdx  = myFrame->headerField("eventIdx").value<uint32_t>();
    uint8_t asadIdx    = myFrame->headerField("asadIdx").value<uint8_t>();
    cout << "Item Count: " << itemCount <<endl;
    cout << "Event time: " << eventTime <<endl;
    cout << "Event ID: " << eventIdx <<endl;
    cout << "AsAd ID: " << (short) asadIdx <<endl;
}

int EventBuilder ()
{
    int totalfiles = 1;
    int numEvents = 30;

    char TB_filename[20];
    char CoBo_filename[20];
    
    uint32_t agetIdx = 0;
    uint32_t sampleValue = 0;
    const size_t numChips = 4u;
    const size_t numChannels = 68u;
    mfm::Field field;
    mfm::BitField agetIdxField, sampleValueField;
    std::vector <uint32_t> chanIdx(numChips, 0u);
    std::vector <uint32_t> buckIdx(numChips, 0u);
    std::ifstream f_raw;
    std::auto_ptr<mfm::Frame> myFrame;
    mfm::FrameDictionary::instance().addFormats("/usr/local/share/get-bench/format/CoboFormats.xcfg");

    for(int file_index=0; file_index < totalfiles; file_index++)
    {
        sprintf(TB_filename, "TB_data_%d.bin", file_index);
        sprintf(CoBo_filename, "CoBo_data_%d.graw", file_index);

        TBDecoder TBEvents = TBDecoder(numEvents, TB_filename);
        f_raw.open(CoBo_filename, std::ios::in | std::ios::binary);
        LoadFirstCoBoFrame(f_raw, field, agetIdxField, sampleValueField, myFrame);

        for (int event_index=0; event_index < numEvents; event_index++)
        {
            int TrigSource = 0;
            vector<int> vNeighbor;

            TrigSource = FindTrigSource(TBEvents, event_index);
        }

/*
        switch (TrigSource)
        {
            Case(Bifocal):
                WriteBifocalEvent();
                break;
            Case(DiscTest):
                call_disctest_func();
                break;
            Case(Internal):
                call_internal_func();
                break;
            Case(External):
                call_external_func();
                break;
            Case(GPS):
                call_gps_func();
                break;
            Case(LED):
                call_led_func();
                break;
            default:
                break;
        }
*/
    }


    for (size_t itemId=0; itemId < 10; ++itemId)
    {
        mfm::Item item = myFrame->itemAt(itemId);

        field = item.field(field);
        agetIdxField = field.bitField(agetIdxField);
        sampleValueField = field.bitField(sampleValueField);

        agetIdx = agetIdxField.value<uint32_t>();
        sampleValue = sampleValueField.value<uint32_t>();
        if (chanIdx[agetIdx] >= numChannels)
        {
        chanIdx[agetIdx] = 0u;
        buckIdx[agetIdx]++;
        }

        cout << "AGET: " << agetIdx << " Channel: " << chanIdx[agetIdx] << " Bucket: " << buckIdx[agetIdx] << " Value: " << sampleValue <<endl;
        chanIdx[agetIdx]++;
    }

    return 0;
}
