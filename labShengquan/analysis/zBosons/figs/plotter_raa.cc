
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

void plotter_raa(string inRootAng){
  //importing ROOT files
  char cInRootAng[inRootAng.length()+1];
  strcpy(cInRootAng, inRootAng.c_str());
  TFile *inFileAng = new TFile(cInRootAng,"read");  

  TH1D::SetDefaultSumw2();

  // Creating histograms data structures
  TH1D *raa_pt[nCentralityBins];

  // Open histograms from root file.
  for(int k = 0; k < nCentralityBins; k++){
    raa_pt[k] = (TH1D*)inFileAng->Get(Form("raa_pt_%d",k));
  }
  
  // -----------------------------------------------------------------------------------
  // Making Figures

    // Defining canvas
    TCanvas *c1 = new TCanvas("c1","c1",1.1*1*650,1*650);
    //c1->Divide(3,2);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);

    // Defining histogram
    TH1D* hist2 = new TH1D("hist2","",200,0.4,200.);
    hist2->SetXTitle("p_{T} (GeV)");
    hist2->SetYTitle("R_{AA}");
    hist2->SetMinimum(0.);
    hist2->SetMaximum(0.4);
    hist2->GetYaxis()->SetNdivisions(505);
    hist2->GetXaxis()->SetNdivisions(505);
    hist2->GetXaxis()->CenterTitle(1);
    hist2->GetYaxis()->CenterTitle(1);
    hist2->GetYaxis()->SetTitleOffset(1);
    hist2->GetXaxis()->SetTitleOffset(1.21);
    hist2->GetXaxis()->SetTitleSize(0.05);
    hist2->GetYaxis()->SetTitleSize(0.05);
    hist2->GetXaxis()->SetLabelSize(0.05);
    hist2->GetYaxis()->SetLabelSize(0.05);
    hist2->Draw();

    gPad->SetTopMargin(0.075);
    gPad->SetBottomMargin(0.14);
    gPad->SetLeftMargin(0.11);
    gPad->SetRightMargin(0.02);
    gPad->SetTicks(-1);
    //gPad->SetLogy();
    gPad->SetLogx();
   
    // Adding histogram data points
    int colors[nCentralityBins] = {1, 4, 2, 1, 4, 2};
    int markers[nCentralityBins] = {24, 25, 28, 20, 21, 34};
    for(int i=0; i<nCentralityBins; i++){
      raa_pt[i]->SetMarkerStyle(markers[i]);
      raa_pt[i]->SetMarkerSize(1);
      raa_pt[i]->SetMarkerColor(colors[i]);
      raa_pt[i]->Draw("same E1");
    }

    // Creating legend
    TLegend *leg1 = new TLegend(0.5,0.75,0.7,0.86);
    TLegend *leg2 = new TLegend(0.7,0.75,0.9,0.86);

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

    leg1->AddEntry(raa_pt[0],"  0-5%","p");
    leg1->AddEntry(raa_pt[1]," 5-10%","p");
    leg1->AddEntry(raa_pt[2],"10-30%","p");
    leg2->AddEntry(raa_pt[3],"30-50%","p");
    leg2->AddEntry(raa_pt[4],"50-70%","p");
    leg2->AddEntry(raa_pt[5],"70-90%","p");

    leg1->Draw();
    leg2->Draw();
   
    // Adding text
    TLatex *tex = new TLatex(.4,1.12,"|#eta|<1");
    tex->SetTextSize(0.04);
    tex->SetLineWidth(2);
    tex->SetTextFont(42);
    tex->Draw();
 
    tex = new TLatex(20,1.12,"ANGANTYR 5.02 TeV");
    tex->SetTextSize(0.04);
    tex->SetLineWidth(2);
    tex->SetTextFont(42);
    tex->Draw();
 
}
