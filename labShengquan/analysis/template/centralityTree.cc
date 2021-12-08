#include <stdio.h>
#include <complex>
#include <iostream>
#include <fstream>
#include <string>
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TRandom.h"
#include "TTree.h"
#include <TMath.h>

using namespace std; 

// ----------------------------------------------------------------------------------------
// Helper function for putting input stream of numbers represented as strings
// into their arrays of floats.
// ----------------------------------------------------------------------------------------
void addToBins(ifstream *pFlist, Float_t *bins){
     string sBuffer;
     for(int boundaryCount = 0; boundaryCount < 101; ++boundaryCount){
       *pFlist >> sBuffer;
       sBuffer = sBuffer.substr(0,sBuffer.length()-1);
       bins[boundaryCount] = stof(sBuffer);
     }
}

// ----------------------------------------------------------------------------------------
// Main macro for creating centrality tree.
// ----------------------------------------------------------------------------------------
void centralityTree(string binBoundaryFileName, string rootListFileName){
     // Open bin boundary text file
     ifstream binBoundaryFile;
     binBoundaryFile.open(binBoundaryFileName);
     string sBuffer; //line buffer

     const int nCentralityBin = 100;
     Float_t bBins[nCentralityBin+1];
     Float_t npartBins[nCentralityBin+1];
     Float_t etfwdBins[nCentralityBin+1];
     Float_t weightedEtfwdBins[nCentralityBin+1];

     int binCounter = 0;
     while(binBoundaryFile >> sBuffer){
       string cur(sBuffer);
       if(!cur.compare("boundaries:")){
          if(binCounter == 0) {
              addToBins(&binBoundaryFile, bBins);
          }else if(binCounter == 1){
              addToBins(&binBoundaryFile, npartBins);
          }else if(binCounter == 2){
              addToBins(&binBoundaryFile, etfwdBins);
          }else if(binCounter == 3){
              addToBins(&binBoundaryFile, weightedEtfwdBins);
          }
          binCounter++;          
       }
     }

     binBoundaryFile.close();

// ----------------------------------------------------------------------------------------
     // Create TChain object to link together collection of TTrees
     //  - records number of entries in total TChain
     //  - sets branch addresses for parameters of interest

     // Open file containing list of root files
     ifstream rootListFile;
     rootListFile.open(rootListFileName);
     char cBuffer[300]; //line buffer
     
     long nEvt; //number of entries

     // parameters of interest
     Double_t b;
     Double_t etfwd;
     Double_t weight;
     Int_t npart;

     // centrality bin data type
     Int_t bCentralityBinTree;
     Int_t etfwdCentralityBinTree;
     Int_t weightedEtfwdCentralityBinTree;
     Int_t npartCentralityBinTree;

     // loop through file list to construct TChain
     TChain t1("hadronTree");
     while (rootListFile >> cBuffer){
       cout << cBuffer << endl;
       t1.Add(cBuffer);
     }
     rootListFile.close();

     // set branch addresses and record number of entries/events.
     t1.SetBranchAddress("bTree", &b);
     t1.SetBranchAddress("etfwdTree", &etfwd);
     t1.SetBranchAddress("weightTree", &weight);
     t1.SetBranchAddress("nPartTree", &npart);
     nEvt=t1.GetEntries();
     cout << "nEvt = " << nEvt << endl;

// ----------------------------------------------------------------------------------------
     // Create output file name and create file
     //  - input files should be of the format fileList_pbpb5tev_ba5.list,
     //    fileList_ppb5tev_ba5.list, or fileList_pp5tev_ba5.list
     //  - this naming convention is application specific and should probably
     //    be changed if this code is being reused.

     string outFileName = "mergedTree_centrality";
     int start = rootListFileName.find("_p");
     int end = rootListFileName.find(".");
     outFileName.append(rootListFileName.substr(start, end-start));
     outFileName.append(".root");
     char charOutFileName[outFileName.length()+1];
     strcpy(charOutFileName, outFileName.c_str());

// ----------------------------------------------------------------------------------------

     // Create new root file and clone the opened tree.
     TFile *newFile = new TFile(charOutFileName ,"recreate");
     cout << "Making a clone of the tree ... ..." << endl;
     TTree *newTree = t1.CloneTree();

     // Create new branches for centrality bins
     auto bCBBranch = newTree->Branch("bCentralityBinTree", &bCentralityBinTree, "bCentralityBinTree/I");
     auto etfwdCBBranch = newTree->Branch("etfwdCentralityBinTree", &etfwdCentralityBinTree, "etfwdCentralityBinTree/I");
     auto wEtfwdCBBranch = newTree->Branch("weightedEtfwdCentralityBinTree", &weightedEtfwdCentralityBinTree, "weightedEtfwdCentralityBinTree/I");
     auto npartCBBranch = newTree->Branch("npartCentralityBinTree", &npartCentralityBinTree, "npartCentralityBinTree/I");

// ----------------------------------------------------------------------------------------
     int wEtfwd;
     int moduloElement = (int)(nEvt/100)+1; //display every 1% of completion
     for(long k = 0; k < nEvt; k++){
       if(k%moduloElement == 0) cout << "Complete: " << k << " Total: " << nEvt << endl;

       t1.GetEntry(k);

       for(int i=0; i<nCentralityBin; i++){
         if(b > bBins[i] && b <= bBins[i+1]){
           bCentralityBinTree = i;
         }
         if(etfwd > etfwdBins[i] && b <= etfwdBins[i+1]){
           etfwdCentralityBinTree = i;
         }
         wEtfwd = weight*etfwd;
         if(wEtfwd > weightedEtfwdBins[i] && wEtfwd <= weightedEtfwdBins[i+1]){
           weightedEtfwdCentralityBinTree = i;
         }
         if(npart > npartBins[i] && npart <= npartBins[i+1]){
           npartCentralityBinTree = i;
         }
       }

       bCBBranch->Fill();
       etfwdCBBranch->Fill();
       wEtfwdCBBranch->Fill();
       npartCBBranch->Fill();
     }
     newTree->Write();
     newFile->Write();
     newFile->Close();
}
       




