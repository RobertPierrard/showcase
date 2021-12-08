
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

void plotter_cmsDataPtSpectra(string inRootFilelist){
  // -----------------------------------------------------------------------------------
  // Reading Data

  // importing ROOT file list
  ifstream flist;
  flist.open(inRootFilelist);
  char ffname[300]; //string for filenames from filelist.
  
  // create array for storing root files pt spectra
  TFile *inputData[nCentralityBins+1];
  TH1D *ptSpectra[nCentralityBins+1];
  TH1D *ptError[nCentralityBins+1];

  // opening files and extracting pt spectra 
  string spectraDir; //directory
  string errorDir; //directory
  int k = 0;
  char cSpectraDir[nCentralityBins+1][20]; //hard coded, oops
  char cErrorDir[nCentralityBins+1][20]; //hard coded, oops
  while(flist >> ffname){
    spectraDir = "Table ";
    errorDir = "Table ";
    spectraDir = spectraDir.append(to_string(k+1));
    errorDir = errorDir.append(to_string(k+1));
    spectraDir = spectraDir.append("/Hist1D_y1");
    errorDir = errorDir.append("/Hist1D_y1_e1");
    strcpy(cSpectraDir[k], spectraDir.c_str());
    strcpy(cErrorDir[k], errorDir.c_str());

    inputData[k] = TFile::Open(ffname);
    ptSpectra[k] = (TH1D*)inputData[k]->Get(cSpectraDir[k]);  
    ptError[k] = (TH1D*)inputData[k]->Get(cErrorDir[k]);  

    k++;
  }
  flist.close();

  // -----------------------------------------------------------------------------------
  // Making Figures

  // define canvas and default histogram
  TCanvas *c1 = new TCanvas("c1","c1",1.1*1*650,1*650);
  
  int nBins = ptSpectra[0]->GetNbinsX();
  TH1D* hist = new TH1D("hist","",nBins,0.4,300.);

  // set errors
  gStyle->SetOptStat(0);
  gStyle->SetErrorX(0);
//  gStyle->SetEndErrorSize(0);
//  for(int k = 0; k < nBins; k++){
//    hist->SetBinError(k,0);
//  }
  for(int i = 0; i < nCentralityBins+1; i++){
    for(int k = 0; k < ptSpectra[i]->GetNbinsX(); k++){
      ptSpectra[i]->SetBinError(k, ptError[i]->GetBinContent(k));
    }
  }

  // title and axes
  hist->SetXTitle("p_{T} (GeV)");
  hist->SetYTitle("#frac{d^{2}N}{2#pip_{T}N_{evt}dp_{T}d#eta}  (GeV^{-2})");
  hist->SetMinimum(0.5e-13);
  hist->SetMaximum(100000);
  hist->GetYaxis()->SetNdivisions(505);
  hist->GetXaxis()->SetNdivisions(505);
  hist->GetXaxis()->CenterTitle(1);
  hist->GetYaxis()->CenterTitle(1);
  hist->GetYaxis()->SetTitleOffset(1.7);
  hist->GetXaxis()->SetTitleOffset(1.4);
  hist->GetXaxis()->SetTitleSize(0.05);
  hist->GetYaxis()->SetTitleSize(0.05);
  hist->GetXaxis()->SetLabelSize(0.05);
  hist->GetYaxis()->SetLabelSize(0.05);
  hist->Draw();

  // margins
  gPad->SetTopMargin(0.075);
  gPad->SetBottomMargin(0.15);
  gPad->SetLeftMargin(0.20);
  gPad->SetRightMargin(0.04);
  gPad->SetTicks(-1);
  gPad->SetLogy();
  gPad->SetLogx();

  // set markers and draw histograms
  int colors[nCentralityBins+1] = {1, 4, 2, 1, 4, 2, 1};
  int markers[nCentralityBins+1] = {24, 25, 28, 20, 21, 34, 5};
  ptSpectra[0]->Scale(10.0);
  ptSpectra[1]->Scale(3.0);
  for(int i=0; i<nCentralityBins+1; i++){
    ptSpectra[i]->SetMarkerStyle(markers[i]);
    ptSpectra[i]->SetMarkerSize(1);
    ptSpectra[i]->SetMarkerColor(colors[i]);

    ptSpectra[i]->Draw("SAME");
  }

  // legend
  TLegend *leg = new TLegend(0.25,0.2,0.35,0.56);
  leg->SetFillColor(10);
  leg->SetBorderSize(0);
  leg->SetTextFont(42);
  leg->SetTextColor(1);
  leg->SetTextSize(0.04);
  leg->AddEntry(ptSpectra[0]," 0-5% (x10)","p");
  leg->AddEntry(ptSpectra[1]," 5-10% (x3)","p");
  leg->AddEntry(ptSpectra[2]," 10-30%","p");
  leg->AddEntry(ptSpectra[3]," 30-50%","p");
  leg->AddEntry(ptSpectra[4]," 50-70%","p");
  leg->AddEntry(ptSpectra[5]," 70-90%","p");
  leg->AddEntry(ptSpectra[6]," pp","p");
  leg->Draw();

  // text
  TLatex *tex = new TLatex(0.42,200000,"|#eta|<1");
  tex->SetTextSize(0.04);
  tex->SetLineWidth(2);
  tex->SetTextFont(42);
  tex->Draw();
  tex = new TLatex(45,195000,"CMS 5.02 TeV");
  tex->SetTextSize(0.04);
  tex->SetLineWidth(2);
  tex->SetTextFont(42);
  tex->Draw();

  // -----------------------------------------------------------------------------------
  // Changing names of ptSpectra

  for(int k = 0; k < nCentralityBins+1; k++){
    if(k < nCentralityBins){
      ptSpectra[k]->SetNameTitle(Form("hist_cmsPtSpectraPbpb_%d",k),Form("cmsPtSpectraPbpb_%d",k));
    }else{
      ptSpectra[k]->SetNameTitle("hist_cmsPtSpectraPp","cmsPtSpectraPp");
    }
  }
  

  // -----------------------------------------------------------------------------------
  // Saving Data to file

  TFile *outputFile = new TFile("cmsDataPtSpectra.root","RECREATE");
  outputFile->cd();
  for(int k = 0; k < nCentralityBins+1; k++){
    ptSpectra[k]->Write();
  }

}
