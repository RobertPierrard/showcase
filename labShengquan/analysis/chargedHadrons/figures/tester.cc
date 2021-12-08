

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

#include "../cmsVariables.h"

using namespace std;

// Right now pp is ommitted because the ROOT File is still being produced


//void tester(string inRootFileFilelist){
void tester(string inRootFilelist){
  ifstream flist;
  flist.open(inRootFilelist);
  char ffname[300]; //string for filenames from filelist.
  
  TFile *inputData[nCentralityBins+1];
  TH1D *ptSpectra[nCentralityBins+1];
  string dir; //directory
  int k = 0;
  char cDir[nCentralityBins+1][17];
  while(flist >> ffname){
    dir = "Table ";
    dir = dir.append(to_string(k+1));
    dir = dir.append("/Hist1D_y1");
    strcpy(cDir[k], dir.c_str());

    cout << "opening file: " << ffname << endl;
    cout << "using directory: \"" << cDir[k] << "\"" << endl;

    inputData[k] = TFile::Open(ffname);
    ptSpectra[k] = (TH1D*)inputData[k]->Get(cDir[k]);  

    cout << inputData[k] << endl << endl;

    cout << ptSpectra[k] << endl << endl;
    k++;
  }
  flist.close();
  ptSpectra[5]->Draw();


  TH1D::SetDefaultSumw2();
}




























