
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

void plotter_ptSpectra(string inRootFilenamePbpb, string inRootFilenamePp){
  //importing ROOT files
  char cInRootFilenamePbpb[inRootFilenamePbpb.length()+1];
  char cInRootFilenamePp[inRootFilenamePp.length()+1];
  strcpy(cInRootFilenamePbpb, inRootFilenamePbpb.c_str());
  strcpy(cInRootFilenamePp, inRootFilenamePp.c_str());
  TFile *inFilePbpb = new TFile(cInRootFilenamePbpb,"read");  
  TFile *inFilePp = new TFile(cInRootFilenamePp,"read");  

  TH1D::SetDefaultSumw2();

  // -----------------------------------------------------------------------------------
  // Creating histograms data structures

  // for pp
  TH1D *centralityParamDistPp;
  TH1D *ptSpectraPp;
  TH1D *ptSpectraWeightedPp;

  // for pbpb
  TH1D *centralityParamDist[nCentralityBins];  
  TH1D *ptSpectra[nCentralityBins];
  TH1D *ptSpectraWeighted[nCentralityBins];

  // -----------------------------------------------------------------------------------
  // Importing histograms from ROOT files

  // for pp
  centralityParamDistPp = (TH1D*)inFilePp->Get("hist_normalizedCentralityParamDistPp");
  ptSpectraPp = (TH1D*)inFilePp->Get("hist_normalizedPtSpectraPp");
  ptSpectraWeightedPp = (TH1D*)inFilePp->Get("hist_normalizedPtSpectraWeightedPp");

  // for pbpb
  for(int i=0; i<nCentralityBins; i++){
    // opening histograms from file
    centralityParamDist[i] = (TH1D*)inFilePbpb->Get(Form("hist_normalizedCentralityParamDist_%d",i));
    ptSpectra[i] = (TH1D*)inFilePbpb->Get(Form("hist_normalizedPtSpectra_%d",i));
    ptSpectraWeighted[i] = (TH1D*)inFilePbpb->Get(Form("hist_normalizedPtSpectraWeighted_%d",i));
  }
  
  // -----------------------------------------------------------------------------------
  // Making Figures

  TCanvas *c1 = new TCanvas("c1","c1",1.1*1*650,1*650);
  //c1->Divide(3,2);
  gStyle->SetOptStat(0);
  gStyle->SetErrorX(0);

  TH1D* hist = new TH1D("hist","",200,0.4,200.);

  // title and axes
  hist->SetXTitle("p_{T} (GeV)");
  hist->SetYTitle("#frac{d^{2}N}{2#pip_{T}N_{evt}dp_{T}d#eta}  (GeV^{-2})");
  hist->SetMinimum(0.5e-11);
  hist->SetMaximum(100000);
  hist->GetYaxis()->SetNdivisions(505);
  hist->GetXaxis()->SetNdivisions(505);
  hist->GetXaxis()->CenterTitle(1);
  hist->GetYaxis()->CenterTitle(1);
  hist->GetYaxis()->SetTitleOffset(1.7);
  hist->GetXaxis()->SetTitleOffset(1.6);
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

  ptSpectraPp->SetMarkerStyle(5);
  ptSpectraPp->SetMarkerSize(1.2);
  ptSpectraPp->SetMarkerColor(1);
  ptSpectraPp->SetLineColor(1);
  ptSpectraPp->Draw("same E1");

  // set markers and draw histograms
  int colors[nCentralityBins] = {1, 4, 2, 1, 4, 2};
  int markers[nCentralityBins] = {24, 25, 28, 20, 21, 34};
  ptSpectra[0]->Scale(10.0);
  ptSpectra[1]->Scale(3.0);
  for(int i=0; i<nCentralityBins; i++){
    ptSpectra[i]->SetMarkerStyle(markers[i]);
    ptSpectra[i]->SetMarkerSize(1);
    ptSpectra[i]->SetMarkerColor(colors[i]);
       
    ptSpectra[i]->Draw("same E1");
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
  leg->AddEntry(ptSpectraPp," pp","p");
  leg->Draw();

  // text
  TLatex *tex = new TLatex(0.42,200000,"|#eta|<1");
  tex->SetTextSize(0.04);
  tex->SetLineWidth(2);
  tex->SetTextFont(42);
  tex->Draw();
  tex = new TLatex(14,195000,"ANGANTYR 5.02 TeV");
  tex->SetTextSize(0.04);
  tex->SetLineWidth(2);
  tex->SetTextFont(42);
  tex->Draw();

}
