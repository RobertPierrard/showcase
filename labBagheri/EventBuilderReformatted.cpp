#include "EventBuilder.h"

/**
 * Prints out various parameters for a specified eventID in a TBDecoder object, a pointer to which
 * is stored as an EventBuilder instance variable.
 * @param TBEvents is a TBDecoder object which stores the data for N events from the TriggerBoard.
 * @param eventID is an integer specifying the event to print.
 */
void EventBuilder::logTBevent(int eventID){
    eventNumber_TB = TBEvents->getEventNumber(eventID);
    eventTime_TB = TBEvents->getClockCount(eventID);
    triggerSource_TB = TBEvents->getTriggerSource(eventID);
    FirstTrigMusic_TB = TBEvents->getActiveSdb(eventID);

    cout << "TB: Event number: " << eventNumber_TB << endl;
    // cout << "TB: Clock Count: " << eventTime_TB << endl;
    // cout << "TB: Trigger Source: " << triggerSource_TB << endl;
    // cout << "TB: Confirm Event #: " << TBEvents->confirmEventNumber(eventID) << endl;
    // cout << "TB: Active SDB: " << FirstTrigMusic_TB << endl;
}

/**
 * Given x and y coordinates of a pixel, returns the pixelID. Bottom left is x=y=0.
 * @param nx x coordinate
 * @param ny y coordinate
 * @return PixelID
 */
int EventBuilder::FindPixel(int nx, int ny){
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
void EventBuilder::FindBin(int pixelID,int *nx, int *ny){
    int MUSIC_ID = pixelID/8;
    int MUSIC_Channel = pixelID%8; 
    int MUSIC_row = MUSIC_ID/16;
    int MUSIC_column = MUSIC_ID%16;

    *ny = 4*MUSIC_row+MUSIC_Channel%4; 
    *nx = 2*MUSIC_column+MUSIC_Channel/4; 
}

void EventBuilder::FindNeighborPixels(int FirstTrigMusic, vector<int> &vNeighbor){
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
 * Open cobo file and read data from its first frame. Needs to be done for each file.
 */
void EventBuilder::LoadFirstCoBoFrame(char CoBo_filename[20], int asadID){
    if(asadID == 0){
        // setup file so that frames and individual items can be read.
        f_raw_0.open(CoBo_filename, std::ios::in | std::ios::binary);

        curFrame = mfm::Frame::read(f_raw_0); //calling line goes to the next frame
        item = curFrame->itemAt(0);
        field = item.field("");
        agetIdxField = field.bitField("agetIdx");
        sampleValueField = field.bitField("sample");
    }else if(asadID == 1){
        f_raw_1.open(CoBo_filename, std::ios::in | std::ios::binary);

        curFrame = mfm::Frame::read(f_raw_1); //calling line goes to the next frame
        item = curFrame->itemAt(0);
        field = item.field("");
        agetIdxField = field.bitField("agetIdx");
        sampleValueField = field.bitField("sample");
    }else{
        return;
    }   
}

/**
 * Function for dealing with frames not comming in event by event. Stores indices of correct order of frames in two
 * queues (one for each asad, stored as member vars)
 */
bool EventBuilder::FrameCounter(char CoBo_filename[20]){
    // parse through file to organize events by asadID, store order in vectors which are member variables.
    f_raw.open(CoBo_filename, std::ios::in | std::ios::binary);
    
    vAsAd0.clear();
    vAsAd1.clear();

    size_t frameCount = 0;
    size_t maxFrames = 3000;
    mfm::Frame::seekFrame(f_raw, 0);
    try {
        do {
            if (frameCount >= maxFrames) break;
            try {
                curFrame = mfm::Frame::read(f_raw);
            }
            catch (const std::ifstream::failure & e) {
                if (f_raw.rdstate() & std::ifstream::eofbit) {
                    LOG_WARN() << "EOF reached.";
                    break;
                }
                else {
                    LOG_ERROR() << "Error reading frame: " << e.what();
                }
                return false;
            }
            frameCount++;
            if ((curFrame->header().frameType() !=0x1) and (curFrame->header().frameType() != 2))
            {
                LOG_DEBUG() << "Skipping frame #" << frameCount << "...";
                continue;
            }
            else if (frameCount%50 == 1)
            {
                LOG_DEBUG() << "Adding frame #" << frameCount << " to tree...";
            }

            uint8_t asadIdx = 2;
            asadIdx = curFrame->headerField("asadIdx").value<uint8_t>();
            if(asadIdx == 0) {vAsAd0.push_back(frameCount-1);}
            if(asadIdx == 1) {vAsAd1.push_back(frameCount-1);}
        }
        while (f_raw.good());
    }
    catch (const std::exception & e) {
        LOG_ERROR() << e.what();
    }

    // determine which vector has the least elements
    numEventsFromVAsad = 0;
    if(vAsAd0.size() > vAsAd1.size()){
        numEventsFromVAsad = vAsAd1.size();
    }else{
        numEventsFromVAsad = vAsAd0.size();
    }

    f_raw.close();

    return true;
}


/**
 * Extract information from the frame header
 * itemCount_CoBo=4 for frame type 1 and itemCount_CoBo=2 for frame type 2
 * eventTime_CoBo = start time of frame relative to start of run
 * eventIdx_CoBo = the event to which this frame belongs in a run, starts at 0
 * asadIdx_CoBo =  asad index from which the frame originates.
 */
void EventBuilder::logCoBoEvent(std::auto_ptr<mfm::Frame> &tempFrame){
    size_t t_itemCount_CoBo   = tempFrame->itemCount();
    uint64_t t_eventTime_CoBo = tempFrame->headerField("eventTime").value<uint64_t>();
    uint32_t t_eventIdx_CoBo  = tempFrame->headerField("eventIdx").value<uint32_t>();
    uint8_t t_asadIdx_CoBo    = tempFrame->headerField("asadIdx").value<uint8_t>();

    // cout << "Cobo: Item Count: " << t_itemCount_CoBo << endl;
    // cout << "Cobo: Event time: " << t_eventTime_CoBo << endl;
    // cout << "Cobo: Event ID: " << t_eventIdx_CoBo << endl;
    // cout << "Cobo: AsAd ID: " << (short)t_asadIdx_CoBo << endl;
    cout << "EventID: " << t_eventIdx_CoBo << " | AsAd: " << (short)t_asadIdx_CoBo << endl;
}

void EventBuilder::Set_Branch_Bifocal(STRUCT_Bifocal *Bifocal_struct){
    Bifocal_tree->Branch("TBTime", &Bifocal_struct->tb_time, "tb_time/l");
    Bifocal_tree->Branch("CoBoTime", &Bifocal_struct->cobo_time, "cobo_time/l");
    Bifocal_tree->Branch("UNIXTime", &Bifocal_struct->unix_time, "unix_time/l");
    Bifocal_tree->Branch("ROIMusicID", Bifocal_struct->ROIMusicID, "ROI_MusicID[2]/i");
    Bifocal_tree->Branch("NofPixels", &Bifocal_struct->NofPixels, "N_Pixels/i");
    Bifocal_tree->Branch("ROIPixelsID", Bifocal_struct->ROIPixelID, "ROI_Pixel_ID[36]/i");
    Bifocal_tree->Branch("ROIPixelsSignalValue", Bifocal_struct->ROISignalValue, "ROI_Signal_Value[36][512]/i");
    Bifocal_tree->Branch("Pedestal", Bifocal_struct->Pedestal, "Pedestal[512]/F");    
}

void EventBuilder::Set_Branch_Background(STRUCT_Background *Background_struct){
    Background_tree->Branch("TBTime", &Background_struct->tb_time, "tb_time/l");
    Background_tree->Branch("CoBoTime", &Background_struct->cobo_time, "cobo_time/l");
    Background_tree->Branch("UNIXTime", &Background_struct->unix_time, "unix_time/l");
    Background_tree->Branch("SignalValue", Background_struct->SignalValue, "Signal_Value[512][512]/i");   
}

void EventBuilder::Set_Branch_Hled(STRUCT_Hled *Hled_struct){
    Hled_tree->Branch("TBTime", &Hled_struct->tb_time, "tb_time/l");
    Hled_tree->Branch("CoBoTime", &Hled_struct->cobo_time, "cobo_time/l");
    Hled_tree->Branch("UNIXTime", &Hled_struct->unix_time, "unix_time/l");
    Hled_tree->Branch("SignalValue", Hled_struct->SignalValue, "Signal_Value[512][10]/i");    
}

void EventBuilder::Write_BifocalEvent(int event_index, STRUCT_Bifocal *Bifocal_struct){
    return;
}

void EventBuilder::StoreDataBackStruct_BackgroundEvent(char CoBo_filename[20], STRUCT_Background *Background_struct, int asadIdx, int event_index){
    // ----------------------------------------------------------------------------------------------------------------
    // // Print size of each asad vAsad vector, set the number of events.
    // cout << "vAsAd0 size: " << vAsAd0.size() << endl;
    // cout << "vAsAd1 size: " << vAsAd1.size() << endl;

    // // For testing, loop through each element in vAsad vectors
    // cout << "vAsAd0:" << endl;
    // for(int k=0; k<numEventsFromVAsad; k++){
    //     cout << vAsAd0.at(k) << ", ";
    // }
    // cout << endl << endl;
    // cout << "vAsAd1:" << endl;
    // for(int k=0; k<numEventsFromVAsad; k++){
    //     cout << vAsAd1.at(k) << ", ";
    // }
    // cout << endl << endl;

    // ----------------------------------------------------------------------------------------------------------------
    int asad0Indx;
    int asad1Indx;
    if(event_index == 0){
        if(asadIdx == 0){
            LoadFirstCoBoFrame(CoBo_filename, 0);
            if(vAsAd0.at(0)==1){
                curFrame = mfm::Frame::read(f_raw_0);
            }
        }else{
            LoadFirstCoBoFrame(CoBo_filename, 1);
            if(vAsAd1.at(0)==1){
                curFrame = mfm::Frame::read(f_raw_1);
            }
        }
        logCoBoEvent(curFrame);

    }else{
        // set frame pointer to correct location in file.
        if(asadIdx == 0){
            asad0Indx = vAsAd0.at(event_index) - vAsAd0.at(event_index-1);
            mfm::Frame::seekFrame(f_raw_0, asad0Indx-1); //moves frame pointer to event specified by vAsAd0
            curFrame = mfm::Frame::read(f_raw_0);
        }else {
            asad1Indx = vAsAd1.at(event_index) - vAsAd1.at(event_index-1);
            mfm::Frame::seekFrame(f_raw_1, asad1Indx-1); //moves frame pointer to event specified by vAsAd1
            curFrame = mfm::Frame::read(f_raw_1);
        }
        logCoBoEvent(curFrame);
    }
    // ----------------------------------------------------------------------------------------------------------------
    //Store cobo header data and TB Time (from TB header data) in background struct 
    if(asadIdx == 0){
        Background_struct->tb_time = eventTime_TB;
        Background_struct->cobo_time = eventTime_CoBo;
        //unix time will be changed in future
        auto p1 = std::chrono::system_clock::now();
        Background_struct->unix_time = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
        eventIdx_CoBo_asad0 = eventIdx_CoBo;

        cout << "TB Time: " << Background_struct->tb_time << endl;
        cout << "CoBo Time: " << Background_struct->cobo_time << endl;
        cout << "Unix Time: " << Background_struct->unix_time << endl;
    }else { //check if data corresponsds to the same event.
        try{
            if(eventIdx_CoBo != eventIdx_CoBo_asad0){
                throw 1; //throw exception if 
            }
        }catch(int e){
            cout << "Event indices of ASAD_0 and ASAD_1 do not match!";
        }
        
    }
    // ----------------------------------------------------------------------------------------------------------------
    // Store item data in background struct

    // vectors for storing channel and bucket indices
    // why is 0u included? set initial values to 0?
    std::vector<uint32_t> chanIdx = std::vector<uint32_t>(numChips, 0u);
    std::vector<uint32_t> buckIdx = std::vector<uint32_t>(numChips, 0u);

    // // local vars for storing data
    uint32_t agetIdx;
    uint32_t sampleValue;
    int backStructChannelIndx;

    // loop through each item in frame
    int maxBucket = 0; //debugging
    int maxChannel = 0; //debugging
    //change to itemCount_cobo and check behavior.
    for (size_t itemId=0; itemId < (MaxNofChannels*MaxTimeBucket/2); ++itemId){
        //set current item
        item = curFrame->itemAt(itemId);

        //store values from item in local vars
        field = item.field(field); // contains 2 main values, agetID and sampleValue
        agetIdxField = field.bitField(agetIdxField);
        sampleValueField = field.bitField(sampleValueField);

        agetIdx = agetIdxField.value<uint32_t>();
        sampleValue = sampleValueField.value<uint32_t>();

        if (chanIdx[agetIdx] >= NChannelsPerAget){
            chanIdx[agetIdx] = 0u;
            buckIdx[agetIdx]++;
        }

        //debugging
            if(buckIdx[agetIdx] > maxBucket){
                maxBucket = buckIdx[agetIdx];
            }
            if(chanIdx[agetIdx] > maxChannel){
                maxChannel = chanIdx[agetIdx];
            }

        // store data in background struct
        // using asadIdx, agetIdx (from item), and chanIdx and buckIdx (from iteration)
        backStructChannelIndx = ((MaxNofChannels/2)*asadIdx) + (agetIdx*NChannelsPerAget) + chanIdx[agetIdx];
        //cout << backStructChannelIndx << endl;
        // cout << "backStructChannelIndx: " << backStructChannelIndx << endl;
        // cout << "Buck Index: " << buckIdx[agetIdx] << endl;
        Background_struct->SignalValue[backStructChannelIndx][buckIdx[agetIdx]] = sampleValue;

        //cout << "AGET: " << agetIdx << " Channel: " << chanIdx[agetIdx] << " Bucket: " << buckIdx[agetIdx] << " Value: " << sampleValue <<endl;
        chanIdx[agetIdx]++;
    }
    //debugging
    cout << "Max Bucket: " << maxBucket << endl;
    cout  << "Max Channel: " << maxChannel << endl;
}

void EventBuilder::Write_BackgroundEvent(STRUCT_Background *Background_struct){
    return;
}

void EventBuilder::Write_HledEvent(STRUCT_Hled *Hled_struct){
    // Hled_tree->Fill();
}

void EventBuilder::Write_Tree_to_file(){
    Bifocal_tree->Write();
    Background_tree->Write();
    Hled_tree->Write();
}

EventBuilder::EventBuilder(){
    {
        //member variables
        totalfiles = 1;
        numEvents = 8;

        mfm::FrameDictionary::instance().addFormats("/usr/local/share/get-bench/format/CoboFormats.xcfg");

        // create TTrees for storing data and create ouput tree file.
        Bifocal_tree = new TTree("Bifocal_tree", "Bifocal Tree");
        Background_tree = new TTree("Background_tree", "Background Tree");
        Hled_tree = new TTree("Hled_tree", "Hled Tree");
    }
}

void EventBuilder::mainFlow(){
    // create buffers for filenames and create input file stream
    char TB_filename[20];
    char CoBo_filename[20];
    char Output_filename[20];

    // loop through all files
    for(int file_index=0; file_index < totalfiles; file_index++){
        // set filenames
        sprintf(TB_filename, "TB_data_%d.bin", file_index);
        sprintf(CoBo_filename, "CoBo_data_%d.graw", file_index);
        sprintf(Output_filename, "Output_%d.root", file_index);

        // create TBDecoder object to read trigger board file
        // NEED TO DESTROY OLD TBDECODER OBJECT??
        TBEvents = new TBDecoder(numEvents, TB_filename);
        

        //Need to confirm trigger board and cobo data line up with each other.
            //some code...        

        // create variables for background structs
        STRUCT_Bifocal Bifocal_event;
        STRUCT_Background Background_event;
        STRUCT_Hled Hled_event;

        // create the appropriate branches in the bifocal, background, and hled trees.
        Set_Branch_Bifocal(&Bifocal_event);
        Set_Branch_Background(&Background_event);
        Set_Branch_Hled(&Hled_event);

        FrameCounter(CoBo_filename);
        // loop through each event in the data (2 frames per event)
        for (int event_index=0; event_index < numEvents; event_index++){
            // extract data from trigger board file, store in member variables.
            cout << "----------------------" << endl;

            //display TB header data
            logTBevent(event_index);

            // state machine for writing event depending on trigger source
            switch (triggerSource_TB){
                case(Bifocal):
                    // Write_BifocalEvent(event_index, &Bifocal_event);
                    // cout << "Bifocal Switch, event_index: " << event_index << endl;
                    break;
                case(Internal):
                    cout << "INTERNAL TRIGGER" << endl;           
                    StoreDataBackStruct_BackgroundEvent(CoBo_filename, &Background_event, 0, event_index);
                    StoreDataBackStruct_BackgroundEvent(CoBo_filename, &Background_event, 1, event_index);
                    Background_tree->Fill();
                    break;
                case(HLED):
                    // Write_HledEvent(&Hled_event);
                    // cout << "Hled Switch, event_index: " << event_index << endl;
                    break;
                default:
                    break;
            }
        }
        
        f_raw_0.close();
        f_raw_1.close();

        // create output tree file, write all three trees to that file, close the file
        TFile f(Output_filename, "recreate");
        Write_Tree_to_file();
        f.Close();
    }
}