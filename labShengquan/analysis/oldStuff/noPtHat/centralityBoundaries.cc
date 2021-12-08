/** Centrality Boundaries.cc
 *  
 *  @author Shengquen Tuo
 *  @author Robert Pierrard
 *  
 *  Description:
 *  This is a ROOT macro which takes in a collection of root files specified by the
 *  directories in the input file. It extracts the impact parameter (b), etfwd,
 *  weight, and npart from the files. Then it sorts the various observables and
 *  divides them into 100 bins. It outputs these bins in a .txt file.
 *
 *  @param fileName A file containing the directories of various ROOT files cotaining
 *  the impact parameter (b), etfwd, weight, and npart for heavy ion collisions. This
 *  file should be named like the following: fileList_pbpb5tev_ba5.list
 *
*/

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
void centralityBoundaries(string filename, string outFilename){
// ----------------------------------------------------------------------------------------
     // Create input file stream
     ifstream flist;
     flist.open(filename);
     char ffname[300]; //line buffer, file path should not exceed 300 chars

// ----------------------------------------------------------------------------------------
     // Create TChain object to link together collection of TTrees
     //  - records number of entries in total TChain
     //  - sets branch addresses for parameters of interest

     long nEvt; //number of entries

     // parameters of interest
     Double_t b;
     Double_t etfwd;
     Double_t weight;
     Int_t npart;

     // local data structures for storing parameters of interest
     vector<float> bVec;
     vector<float> etfwdVec;
     vector<float> weighted_etfwdVec;
     vector<int> npartVec;

     // loop through file list to construct TChain
     TChain t1("hadronTree");
     while ( flist >> ffname ){
       t1.Add(ffname);
     }

     // set branch addresses and record number of entries/events.
     t1.SetBranchAddress("bTree", &b);
     t1.SetBranchAddress("etfwdTree", &etfwd);
     t1.SetBranchAddress("weightTree", &weight);
     t1.SetBranchAddress("nPartTree", &npart);
     nEvt=t1.GetEntries();

     // display info to console
     cout << "Example tree entry: " << endl << endl;
     t1.Show(0);
     cout << "Number of entries: " << nEvt << endl << endl;

// ----------------------------------------------------------------------------------------
     // First event loop for adding data to local data structures (vectors).

     int moduloElement = (int)(nEvt/100)+1; //display every 1% of completion
     for(long k = 0; k < nEvt; k++){
       if(k%moduloElement == 0) cout << "Complete: " << k << " Total: " << nEvt << endl;

       t1.GetEntry(k);
       bVec.push_back(b);
       npartVec.push_back(npart); 
       etfwdVec.push_back(etfwd);
       weighted_etfwdVec.push_back(etfwd*weight);
     }

// ----------------------------------------------------------------------------------------
     // Sort each vector in ascending order and create float arrays
     // for storing bin boundaries.

     sort(bVec.begin(), bVec.end());
     sort(npartVec.begin(), npartVec.end());
     sort(etfwdVec.begin(), etfwdVec.end());
     sort(weighted_etfwdVec.begin(), weighted_etfwdVec.end());

     //cout<<"size: "<<npartVec.size()<<endl;

     const int nCentralityBin = 100;
     float binBoundaries_impactPram[nCentralityBin+1];
     binBoundaries_impactPram[0] = 0.0;
     float binBoundaries_npart[nCentralityBin+1];
     binBoundaries_npart[0] = 0.0; 
     float binBoundaries_etfwd[nCentralityBin+1];
     binBoundaries_etfwd[0] = 0.0; 
     float binBoundaries_weighted_etfwd[nCentralityBin+1];
     binBoundaries_weighted_etfwd[0] = 0.0; 

// ----------------------------------------------------------------------------------------
     // Second event loop for filling bin boundary arrays with appropriate vector data.

     for(int i=0; i< nCentralityBin; i++){
       binBoundaries_impactPram[i+1] = bVec[(i+1)*bVec.size()/nCentralityBin];
       binBoundaries_npart[i+1] = npartVec[(i+1)*npartVec.size()/nCentralityBin];
       binBoundaries_etfwd[i+1] = etfwdVec[(i+1)*etfwdVec.size()/nCentralityBin];
       binBoundaries_weighted_etfwd[i+1] = weighted_etfwdVec[(i+1)*weighted_etfwdVec.size()/nCentralityBin];
       //cout<<"boundaries: "<<binBoundaries[i+1]<<endl;
     }

     // fill final boundary element
     binBoundaries_impactPram[nCentralityBin] = bVec[bVec.size()-1];
     binBoundaries_npart[nCentralityBin] = npartVec[npartVec.size()-1];
     binBoundaries_etfwd[nCentralityBin] = etfwdVec[etfwdVec.size()-1];
     binBoundaries_weighted_etfwd[nCentralityBin] = weighted_etfwdVec[weighted_etfwdVec.size()-1];

// ----------------------------------------------------------------------------------------
     // Create output file name and create file
     //  - input files should be of the format fileList_pbpb5tev_ba5.list,
     //    fileList_ppb5tev_ba5.list, or fileList_pp5tev_ba5.list
     //  - this naming convention is application specific and should probably
     //    be changed if this code is being reused.

//     string outFileName = "binBoundaries";
//     int start = fileName.find("_p");
//     int end = fileName.find(".");
//     outFileName.append(fileName.substr(start, end-start));
//     outFileName.append(".txt");

     ofstream outFile;
     outFile.open(outFilename);

// ----------------------------------------------------------------------------------------
     outFile<<"impact_param boundaries: "<<endl;
     for(int i=0; i< nCentralityBin+1; i++){
       outFile<<binBoundaries_impactPram[i]<<", ";
     }
     outFile<<endl<<endl;

     outFile<<"npart boundaries: "<<endl;
     for(int i=0; i< nCentralityBin+1; i++){
       outFile<<binBoundaries_npart[i]<<", ";
     }
     outFile<<endl<<endl;
 
     outFile<<"etfwd boundaries: "<<endl;
     for(int i=0; i< nCentralityBin+1; i++){
       outFile<<binBoundaries_etfwd[i]<<", ";
     }
     outFile<<endl<<endl;
      
     outFile<<"weighted_etfwd boundaries: "<<endl;
     for(int i=0; i< nCentralityBin+1; i++){
       outFile<<binBoundaries_weighted_etfwd[i]<<", ";
     }
     outFile<<endl<<endl;
      
}

