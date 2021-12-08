
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

void plotter_cmsDataRaa(string inRootFilelist){
  // -----------------------------------------------------------------------------------
  // Reading Data

  // importing ROOT file list
  ifstream flist;
  flist.open(inRootFilelist);
  char ffname[300]; //string for filenames from filelist.
  
  // create array for storing root files pt spectra
  TFile *inputData[nCentralityBins];
  TH1D *raa[nCentralityBins];
  TH1D *error[nCentralityBins];

  // opening files and extracting pt spectra 
  string raaDir; //directory
  string errorDir; //directory
  int k = 0;
  char cRaaDir[nCentralityBins][20]; //hard coded, oops
  char cErrorDir[nCentralityBins][20]; //hard coded, oops
  while(flist >> ffname){
    raaDir = "Table ";
    errorDir = "Table ";
    raaDir = raaDir.append(to_string(k+8));
    errorDir = errorDir.append(to_string(k+8));
    raaDir = raaDir.append("/Hist1D_y1");
    errorDir = errorDir.append("/Hist1D_y1_e1");
    strcpy(cRaaDir[k], raaDir.c_str());
    strcpy(cErrorDir[k], errorDir.c_str());

    cout << "filenames: " << cRaaDir[k] << " , " << cErrorDir[k] << endl;

    inputData[k] = TFile::Open(ffname);

    cout << "pInputData: " << inputData[k] << endl;

    raa[k] = (TH1D*)inputData[k]->Get(cRaaDir[k]);  
    error[k] = (TH1D*)inputData[k]->Get(cErrorDir[k]);  

    cout << raa[k] << endl << error[k] << endl;

    k++;
  }
  flist.close();
  cout << "Input data has been loaded" << endl;
  // -----------------------------------------------------------------------------------
  // Making Figures

    // Defining canvas
    TCanvas *c1 = new TCanvas("c1","c1",1.1*1*650,1*650);
    //c1->Divide(3,2);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);

    // Defining histogram
    int nBins = raa[0]->GetNbinsX();
    TH1D* hist1 = new TH1D("hist1","",nBins,0.6,200.);
    hist1->SetXTitle("p_{T} (GeV)");
    hist1->SetYTitle("R_{AA}");
    hist1->SetMinimum(0.);
    hist1->SetMaximum(1.1);
    hist1->GetYaxis()->SetNdivisions(505);
    hist1->GetXaxis()->SetNdivisions(505);
    hist1->GetXaxis()->CenterTitle(1);
    hist1->GetYaxis()->CenterTitle(1);
    hist1->GetYaxis()->SetTitleOffset(1);
    hist1->GetXaxis()->SetTitleOffset(1.17);
    hist1->GetXaxis()->SetTitleSize(0.05);
    hist1->GetYaxis()->SetTitleSize(0.05);
    hist1->GetXaxis()->SetLabelSize(0.05);
    hist1->GetYaxis()->SetLabelSize(0.05);
    hist1->Draw();

    gPad->SetTopMargin(0.075);
    gPad->SetBottomMargin(0.127);
    gPad->SetLeftMargin(0.11);
    gPad->SetRightMargin(0.02);
    gPad->SetTicks(-1);
    //gPad->SetLogy();
    gPad->SetLogx();
  
  // set errors
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
//  gStyle->SetEndErrorSize(5);
//  for(int k = 0; k < nBins; k++){
//    hist->SetBinError(k,0);
//  }
  for(int i = 0; i < nCentralityBins; i++){
    for(int k = 0; k < raa[i]->GetNbinsX(); k++){
      raa[i]->SetBinError(k, error[i]->GetBinContent(k));
    }
  }
  cout << "Errors have been set" << endl;
  // title and axes
    // Defining histogram
    hist1->SetXTitle("p_{T} (GeV)");
    hist1->SetYTitle("R_{AA}");
    hist1->SetMinimum(0.);
    hist1->SetMaximum(1.1);
    hist1->GetYaxis()->SetNdivisions(505);
    hist1->GetXaxis()->SetNdivisions(505);
    hist1->GetXaxis()->CenterTitle(1);
    hist1->GetYaxis()->CenterTitle(1);
    hist1->GetYaxis()->SetTitleOffset(1);
    hist1->GetXaxis()->SetTitleOffset(1.17);
    hist1->GetXaxis()->SetTitleSize(0.05);
    hist1->GetYaxis()->SetTitleSize(0.05);
    hist1->GetXaxis()->SetLabelSize(0.05);
    hist1->GetYaxis()->SetLabelSize(0.05);
    hist1->Draw();

    gPad->SetTopMargin(0.075);
    gPad->SetBottomMargin(0.127);
    gPad->SetLeftMargin(0.11);
    gPad->SetRightMargin(0.02);
    gPad->SetTicks(-1);
    //gPad->SetLogy();
    gPad->SetLogx();

    // set markers and draw histograms
    int colors[nCentralityBins] = {1, 4, 2, 1, 4, 2};
    int markers[nCentralityBins] = {24, 25, 28, 20, 21, 34};
    for(int i=0; i<nCentralityBins; i++){
      raa[i]->SetMarkerStyle(markers[i]);
      raa[i]->SetMarkerColor(colors[i]);
      raa[i]->SetMarkerSize(1);
      raa[i]->Draw("same E1");
    }

    // Creating legend
    TLegend *leg1 = new TLegend(0.15,0.75,0.35,0.86);
    TLegend *leg2 = new TLegend(0.35,0.75,0.5,0.86);

    leg1->SetFillColor(10);
    leg1->SetBorderSize(0);
    leg1->SetTextFont(42);
    leg1->SetTextColor(1);
    leg1->SetTextSize(0.04);
    leg2->SetFillColor(10);
    leg2->SetBorderSize(0);
    leg2->SetTextFont(42);
    leg2->SetTextColor(1);
    leg2->SetTextSize(0.04);

    leg1->AddEntry(raa[0],"  0-5%","p");
    leg1->AddEntry(raa[1]," 5-10%","p");
    leg1->AddEntry(raa[2],"10-30%","p");
    leg2->AddEntry(raa[3],"30-50%","p");
    leg2->AddEntry(raa[4],"50-70%","p");
    leg2->AddEntry(raa[5],"70-90%","p");

    leg1->Draw();
    leg2->Draw();

    // Adding text
    TLatex *tex = new TLatex(.6,1.12,"|#eta|<1");
    tex->SetTextSize(0.04);
    tex->SetLineWidth(2);
    tex->SetTextFont(42);
    tex->Draw();
 
    tex = new TLatex(48,1.12,"CMS 5.02 TeV");
    tex->SetTextSize(0.04);
    tex->SetLineWidth(2);
    tex->SetTextFont(42);
    tex->Draw();

  // -----------------------------------------------------------------------------------
  // Saving Data to file

  TFile *outputFile = new TFile("cmsDataRaa.root","RECREATE");
  outputFile->cd();
  char histName[20];
  for(int k = 0; k < nCentralityBins; k++){
    sprintf(histName, "cmsRaa_%d", k);
    raa[k]->SetName(histName);
    raa[k]->Write();
  }

}
