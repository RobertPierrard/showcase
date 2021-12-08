
#include <stdio.h>
#include <complex>
#include <iostream>
#include <fstream>
#include <string>
#include "TChain.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TRandom.h"
#include "TTree.h"
#include <TMath.h>

#include "../../cmsVariables.h"

using namespace std; 
void histMergerPtSpectraPp(string rootFilenames, string outFilename){
// ----------------------------------------------------------------------------------------
     // Create input file stream
     ifstream flist;
     flist.open(rootFilenames);

// ----------------------------------------------------------------------------------------
     // Create TChain object to link together collection of TTrees
     //  - records number of entries in total TChain
     //  - sets branch addresses for parameters of interest

     long nEvt; //number of entries

     // Create histograms for merging histograms from files
     TH1D *centralityParamDist;
     TH1D *ptSpectra;
     TH1D *ptSpectraWeighted;

     TH1D *sumCentralityParamDist = new TH1D("hist_centralityParamDistPp","", 200, 0, 20);
     TH1D *sumPtSpectra = new TH1D("hist_ptSpectraPp","", nPtBins, ptBins);
     TH1D *sumPtSpectraWeighted = new TH1D("hist_ptSpectraWeightedPp","", nPtBins, ptBins);

     // parameters of interest
//     Double_t centralityParam;
//     Double_t ptSpectra;
//     Double_t ptSpectraWeighted;

     // local data structures for storing parameters of interest
//     vector<float> bVec;
//     vector<float> etfwdVec;
//     vector<float> weighted_etfwdVec;
//     vector<int> npartVec;

     // loop through file list to construct TChain
     //  - there is added difficulty bc each of my TTrees has s different
     //    name corresponding to their job number, in the fugure it is best
     //    to give all TTrees the same name when making a TChain.
     TFile *file;
     int k = 0;
     char charName[300]; //line buffer, file path should not exceed 300 chars
     while (flist >> charName){
       file = TFile::Open(charName);

       centralityParamDist = (TH1D*)file->Get("hist_centralityParamDist");
       ptSpectra = (TH1D*)file->Get("hist_ptSpectra");
       ptSpectraWeighted = (TH1D*)file->Get("hist_ptSpectraWeighted");

       sumCentralityParamDist->Add(centralityParamDist);
       sumPtSpectra->Add(ptSpectra);
       sumPtSpectraWeighted->Add(ptSpectraWeighted);
     }

     // set branch addresses and record number of entries/events.
//     t1.SetBranchAddress("bTree", &b);
//     t1.SetBranchAddress("etfwdTree", &etfwd);
//     t1.SetBranchAddress("weightTree", &weight);
//     t1.SetBranchAddress("nPartTree", &npart);
//     nEvt=t1.GetEntries();

// ---------------------------------------------------------------------------------------
   // Create output file and write TChain to file.

    char charOutFilename[outFilename.length()+1];
    strcpy(charOutFilename, outFilename.c_str());
    TFile *outFile = new TFile(charOutFilename,"recreate");
    outFile->cd();
    sumCentralityParamDist->Write();
    sumPtSpectra->Write();
    sumPtSpectraWeighted->Write();





}
