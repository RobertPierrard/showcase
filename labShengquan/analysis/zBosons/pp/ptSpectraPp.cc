#include <stdio.h>
#include <complex>
#include <iostream>
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TRandom.h"
#include "TTree.h"
#include <TMath.h>

#include "cmsVariables.h"

using namespace std;

// ----------------------------------------------------------------------------------------
// Helper function for putting input stream of numbers represented as strings
// into their arrays of floats.
// ----------------------------------------------------------------------------------------
void addToBins(ifstream *pInstream, Float_t *bins){
     string sBuffer;
     for(int boundaryCount = 0; boundaryCount < 101; ++boundaryCount){
       *pInstream >> sBuffer;
       sBuffer = sBuffer.substr(0,sBuffer.length()-1);
       bins[boundaryCount] = stof(sBuffer);
     }
}

// ----------------------------------------------------------------------------------------
// Function call specifiying which centrality parameter to use.
// ----------------------------------------------------------------------------------------
void ptSpectraPp(string binBoundaryFilename, string rootFilelistFilename, string centralityParam){
  // variables derived from CMS variables
  double dEtaBin = etaMax - etaMin;
  double dPtBins[nPtBins];
  for(int i=0; i<nPtBins; i++){
    dPtBins[i] = ptBins[i+1] - ptBins[i];
  }

  // ----------------------------------------------------------------------------------------
  // Storing Centrality bins from text file in float arrays
  
  // variable derived from data
  double centralityBounds[nCentralityBins+1];

  // Open bin boundary text file
  ifstream binBoundaryFile;
  binBoundaryFile.open(binBoundaryFilename);
  string sBuffer; //line buffer
  
  // Create arrays   
  const int nBinsTxt = 100; //number of bins in text file
  Float_t txtBins[nBinsTxt+1];

  // Fill arrays with helper function.
  bool foundParam = false;
  while(binBoundaryFile >> sBuffer){
    string cur(sBuffer);
    if(!cur.compare(centralityParam)) {
        binBoundaryFile >> sBuffer; //skip 1 more word
        addToBins(&binBoundaryFile, txtBins);
	foundParam = true;
    }
  }
  if(foundParam == false){
     cout << "You must include a valid centrality parameter: \"impact_param\", \"npart\", \"etfwd\", and \"weighted_etfwd\"" << endl;
     return;
  }

  // print centrality bins being used
  cout << endl << "Using " << centralityParam << " for the following 100 centrality bins: " << endl;
  for(int k = 0; k < nBinsTxt+1; k++){
    cout << txtBins[k] << ", ";
  }
  cout << endl << endl;
 
  binBoundaryFile.close();
  
  // -----------------------------------------------------------------------------------------
  // Initializing Centrality Boundary arrays.

  for(int k = 0; k < (nCentralityBins+1); ++k){
    if(!centralityParam.compare("impact_param")) {
      centralityBounds[k] = txtBins[centralityBins[k]];
    }else{
      centralityBounds[k] = txtBins[nBinsTxt-centralityBins[k]];
    }
  }

  TH1D::SetDefaultSumw2();

  // Create  histograms for stories centrality parameters and pT values in respective centrality bins
  TH1D *centralityParamDist;
  TH1D *ptSpectra;
  TH1D *ptSpectraWeighted;

  centralityParamDist = new TH1D("hist_centralityParamDistPp","", 200, 0, 20);
  ptSpectra = new TH1D("hist_ptSpectraPp","", nPtBins, ptBins);
  ptSpectraWeighted = new TH1D("hist_ptSpectraWeightedPp","", nPtBins, ptBins);

  // -----------------------------------------------------------------------------------------
  // input root files files
     ifstream flist;
     flist.open(rootFilelistFilename);
     
     const int maxMultiplicity = 50000;
     char ffname[300];
     long nEvt;

     Double_t bTree;
     Double_t etfwdTree;
     Double_t weightTree;
     Int_t nMultiplicityTree;//, nPartTree;
     Float_t ptTree[maxMultiplicity], etaTree[maxMultiplicity];

     TChain t1("hadronTree");
     while ( flist >> ffname ){
       t1.Add(ffname);
     }

     t1.SetBranchAddress("nMultiplicityTree",&nMultiplicityTree);
 //    t1.SetBranchAddress("nPartTree",&nPartTree);
     t1.SetBranchAddress("bTree", &bTree);
     t1.SetBranchAddress("etfwdTree", &etfwdTree);
     t1.SetBranchAddress("weightTree", &weightTree);
     t1.SetBranchAddress("ptTree", &ptTree);
     t1.SetBranchAddress("etaTree", &etaTree);
     nEvt=t1.GetEntries();

  // -----------------------------------------------------------------------------------------
  // Loop through events and add to centrallity bins for respective centrallity parameters
  //   - there are two times where we need to branch depending on the centrality paramter used.
  
    //nEvt = 10000;
    cout<<"nEvt = "<<nEvt<<endl;
    int curCentralityBin;
    int percentage = 0;
    int moduloElement = (int)(nEvt/100)+1; //display every 1% of completion
    for(long ne=0; ne<nEvt; ne++)
    {
      if(ne%moduloElement == 0) cout << "Complete: " << ne << " | Total: " << nEvt << " | " << percentage++ << "%" << endl;
      t1.GetEntry(ne);

      curCentralityBin = -1;
 
      if(!centralityParam.compare("impact_param")){
        for(int j=0; j<nCentralityBins; j++){
          if(bTree>=centralityBounds[j] && bTree<centralityBounds[j+1]){
            curCentralityBin = j;
          }
        }
      }else if(!centralityParam.compare("etfwd")){
        for(int j=0; j<nCentralityBins; j++){
          if(etfwdTree>=centralityBounds[j] && etfwdTree<centralityBounds[j+1]){
            curCentralityBin = j;
          }
        }
      }else if(!centralityParam.compare("weighted_etfwd")){
        for(int j=0; j<nCentralityBins; j++){
          if((etfwdTree*weightTree)>=centralityBounds[j] && (etfwdTree*weightTree)<centralityBounds[j+1]){
            curCentralityBin = j;
          }
        }
      }

      if(curCentralityBin>-1){
        if(!centralityParam.compare("impact_param")){
          centralityParamDist->Fill(bTree);
        }else if(!centralityParam.compare("etfwd")){
          centralityParamDist->Fill(etfwdTree);
        }else if(!centralityParam.compare("weighted_etfwd")){
          centralityParamDist->Fill(etfwdTree*weightTree);
        }
           
        for(int i=0; i<nMultiplicityTree; i++){
          if(etaTree[i]>etaMin && etaTree[i]<etaMax){
            ptSpectra->Fill(ptTree[i]); 
            ptSpectraWeighted->Fill(ptTree[i], weightTree); 
          } 
        }
      }
    } // end of event loop
    cout << endl << endl; //formatting
  

  // -----------------------------------------------------------------------------------------
  // Create output file and write histograms to file.
  //
// ----------------------------------------------------------------------------------------
     // Create output file name and create file
     //  - input files should be of the format fileList_pbpb5tev_ba5.list,
     //    fileList_ppb5tev_ba5.list, or fileList_pp5tev_ba5.list
     //  - this naming convention is application specific and should probably
     //    be changed if this code is being reused.

     string outFileName = "output_hist_pp";
//     outFileName.append(centralityParam);
//     outFileName.append("CentralityParam");
//     int start = rootFilelistFilename.find("_p");
//     int end = rootFilelistFilename.find(".");
//     outFileName.append(rootFilelistFilename.substr(start, end-start));
     outFileName.append(".root");
     char charOutFileName[outFileName.length()+1];
     strcpy(charOutFileName, outFileName.c_str());

     TFile *outFile = new TFile(charOutFileName,"recreate");
     outFile->cd();
     centralityParamDist->Write();
     ptSpectra->Write();
     ptSpectraWeighted->Write();
}

// ----------------------------------------------------------------------------------------
// Function call withough specifiying which centrality parameter to use.
//   - defualt centrality parameter is impact paramter.
// ----------------------------------------------------------------------------------------
void ptSpectraPp(string binBoundaryFilename, string rootFilelistFilename){
  string centralityParam = "impact_param";
  ptSpectraPp(binBoundaryFilename, rootFilelistFilename, centralityParam);
}



