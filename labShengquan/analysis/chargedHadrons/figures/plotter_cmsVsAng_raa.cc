
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

void plotter_cmsVsAng_raa(string inRootCms, string inRootAng){
  //importing ROOT files
  char cInRootCms[inRootCms.length()+1];
  char cInRootAng[inRootAng.length()+1];
  strcpy(cInRootCms, inRootCms.c_str());
  strcpy(cInRootAng, inRootAng.c_str());
  TFile *inFileCms = new TFile(cInRootCms,"read");  
  TFile *inFileAng = new TFile(cInRootAng,"read");  

  TH1D::SetDefaultSumw2();

  // Creating histograms data structures
  TH1D *histCms[nCentralityBins];
  TH1D *histAng[nCentralityBins];  

  // Open histograms from root file.
  for(int k = 0; k < nCentralityBins; k++){
    histCms[k] = (TH1D*)inFileCms->Get(Form("cmsRaa_%d",k));
    histAng[k] = (TH1D*)inFileAng->Get(Form("raa_pt_%d",k));
  }
  
  // -----------------------------------------------------------------------------------
  // MAKING FIGURES
  // ----------------------------------------------------------------------------------

  // ----------------------------------------------------------------------------------
  // Figure 1: Angantyr and CMS on same plot for most and least central.

    // Defining canvas
    TCanvas *c2 = new TCanvas("c2","c2",1.1*1*650,1*650);
    //c1->Divide(3,2);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);

    // Defining histogram
    TH1D* hist2 = new TH1D("hist2","",200,0.4,200.);
    hist2->SetXTitle("p_{T} (GeV)");
    hist2->SetYTitle("R_{AA}");
    hist2->SetMinimum(0.);
    hist2->SetMaximum(1.1);
    hist2->GetYaxis()->SetNdivisions(505);
    hist2->GetXaxis()->SetNdivisions(505);
    hist2->GetXaxis()->CenterTitle(1);
    hist2->GetYaxis()->CenterTitle(1);
    hist2->GetYaxis()->SetTitleOffset(1);
    hist2->GetXaxis()->SetTitleOffset(1.165);
    hist2->GetXaxis()->SetTitleSize(0.06);
    hist2->GetYaxis()->SetTitleSize(0.05);
    hist2->GetXaxis()->SetLabelSize(0.05);
    hist2->GetYaxis()->SetLabelSize(0.05);
    hist2->Draw();

    gPad->SetTopMargin(0.075);
    gPad->SetBottomMargin(0.16);
    gPad->SetLeftMargin(0.11);
    gPad->SetRightMargin(0.02);
    gPad->SetTicks(-1);
    //gPad->SetLogy();
    gPad->SetLogx();
   
    // Adding histogram data points
    int cmsColors[] = {15, 12};
    int cmsMarkers[] = {8, 21};
    int angColors[] = {2, 4};
    int angMarkers[] = {24, 25};

    histCms[0]->SetMarkerStyle(cmsMarkers[0]);
    histCms[0]->SetMarkerSize(1);
    histCms[0]->SetMarkerColor(cmsColors[0]);
    histCms[0]->Draw("same E1");
    histCms[5]->SetMarkerStyle(cmsMarkers[1]);
    histCms[5]->SetMarkerSize(1);
    histCms[5]->SetMarkerColor(cmsColors[1]);
    histCms[5]->Draw("same E1");

    histAng[0]->SetMarkerStyle(angMarkers[0]);
    histAng[0]->SetMarkerSize(1);
    histAng[0]->SetMarkerColor(angColors[0]);
    histAng[0]->Draw("same E1");
    histAng[5]->SetMarkerStyle(angMarkers[1]);
    histAng[5]->SetMarkerSize(1);
    histAng[5]->SetMarkerColor(angColors[1]);
    histAng[5]->Draw("same E1");

    // Creating legend
    TLegend *leg1 = new TLegend(0.35,0.77,0.4,0.865);
    TLegend *leg2 = new TLegend(0.48,0.77,0.53,0.865);

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

    leg1->AddEntry(histCms[0],"  0-5%","p");
    leg2->AddEntry(histCms[5]," 70-90%","p");
    leg1->AddEntry(histAng[0],"  0-5%","p");
    leg2->AddEntry(histAng[5]," 70-90%","p");

    leg1->Draw();
    leg2->Draw();
   
    // Adding text
    TLatex *tex = new TLatex(.4,1.12,"|#eta|<1");
    tex->SetTextSize(0.04);
    tex->SetLineWidth(2);
    tex->SetTextFont(42);
    tex->Draw();
 
    tex = new TLatex(10,1.12,"R_{AA} Comparison at 5.02 TeV");
    tex->SetTextSize(0.04);
    tex->SetLineWidth(2);
    tex->SetTextFont(42);
    tex->Draw();
 
    tex = new TLatex(0.6,0.905,"ANGANTYR");
    tex->SetTextSize(0.03);
    tex->SetLineWidth(2);
    tex->SetTextFont(42);
    tex->Draw();
 
    tex = new TLatex(0.6,0.97,"CMS");
    tex->SetTextSize(0.03);
    tex->SetLineWidth(2);
    tex->SetTextFont(42);
    tex->Draw();


  // ----------------------------------------------------------------------------------
  // Figure 2: PT Spectra ratio for ANGANTYR

    // Defining canvas
    TCanvas *c3 = new TCanvas("c3","c3",1.1*1*650,1*650);
    //c1->Divide(3,2);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);

    // Defining histogram
    TH1D* hist3 = new TH1D("hist3","",200,0.4,80.);
    hist3->SetXTitle("p_{T} (GeV)");
    hist3->SetYTitle("R_{AA}");
    hist3->SetMinimum(0.0001);
    hist3->SetMaximum(1.1);
    hist3->GetYaxis()->SetNdivisions(505);
    hist3->GetXaxis()->SetNdivisions(505);
    hist3->GetXaxis()->CenterTitle(1);
    hist3->GetYaxis()->CenterTitle(1);
    hist3->GetYaxis()->SetTitleOffset(1);
    hist3->GetXaxis()->SetTitleOffset(1.17);
    hist3->GetXaxis()->SetTitleSize(0.05);
    hist3->GetYaxis()->SetTitleSize(0.05);
    hist3->GetXaxis()->SetLabelSize(0.05);
    hist3->GetYaxis()->SetLabelSize(0.05);
    hist3->Draw();

    gPad->SetTopMargin(0.075);
    gPad->SetBottomMargin(0.127);
    gPad->SetLeftMargin(0.11);
    gPad->SetRightMargin(0.02);
    gPad->SetTicks(-1);
    //gPad->SetLogy();
    gPad->SetLogx();

    // Create histgram for holding ratio of Angantyr/CMS and add to plot
    TH1D *angSpectraRatio[nCentralityBins];
    int colors[nCentralityBins] = {1, 4, 2, 1, 4, 2};
    int markers[nCentralityBins] = {24, 25, 28, 20, 21, 34};

    for(int k = 0; k < nCentralityBins; k++){
      angSpectraRatio[k] = (TH1D*)histAng[0]->Clone();
      angSpectraRatio[k]->Divide(histAng[k]);

      angSpectraRatio[k]->SetMarkerStyle(markers[k]);
      angSpectraRatio[k]->SetMarkerSize(1);
      angSpectraRatio[k]->SetMarkerColor(colors[k]);

      if(k > 0){
        angSpectraRatio[k]->Draw("same E1");
      }
    }

    // Creating legend
    TLegend *leg3 = new TLegend(0.35,0.77,0.4,0.865);

    leg3->SetFillColor(10);
    leg3->SetBorderSize(0);
    leg3->SetTextFont(42);
    leg3->SetTextColor(1);
    leg3->SetTextSize(0.04);

    leg3->AddEntry(angSpectraRatio[1],"70-90%","p");
    leg3->AddEntry(angSpectraRatio[5],"70-90%","p");

    leg3->Draw();
   
    // Adding text
    TLatex *tex3 = new TLatex(.4,1.12,"|#eta|<1");
    tex3->SetTextSize(0.04);
    tex3->SetLineWidth(2);
    tex3->SetTextFont(42);
    tex3->Draw();
 
    tex3 = new TLatex(6,1.12,"R_{AA} ratios at 5.02 TeV using ANGANTYR");
    tex3->SetTextSize(0.04);
    tex3->SetLineWidth(2);
    tex3->SetTextFont(42);
    tex3->Draw();
}






























