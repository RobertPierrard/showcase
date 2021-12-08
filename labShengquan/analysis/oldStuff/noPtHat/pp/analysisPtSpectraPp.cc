/**
 * This is a ROOT macro for nomralizing histograms produced from the ptSpectraPbpb.cc code.
 * Specifically, we normalize the centrality parameter distribution, the pt spectra, and
 * the weighted pt spectra by the size of their pt bins, eta bins, and the total number of
 * entries in those bins.
 *
 * @author: Robert Pierrard
 * @author: Shengquan Tuo
 * @param inRootFile a string containing the filename of the ROOT file where the histograms
 * are stored.
 *
**/

#include "cmsVariables.h"

void analysisPtSpectraPp(string inRootFile){
  char cInRootFile[inRootFile.length()+1];
  strcpy(cInRootFile, inRootFile.c_str());

  TFile *inFile = new TFile(cInRootFile,"read");  

  double dEtaBin = etaMax - etaMin;
  double dPtBins[nPtBins];
  for(int i=0; i<nPtBins; i++){
    dPtBins[i] = ptBins[i+1] - ptBins[i];
  }

  TH1D::SetDefaultSumw2();

  TH1D *centralityParamDist;  
  TH1D *ptSpectra;
  TH1D *ptSpectraWeighted;

  centralityParamDist = (TH1D*)inFile->Get("hist_centralityParamDist");
  ptSpectra = (TH1D*)inFile->Get("hist_ptSpectra");
  ptSpectraWeighted = (TH1D*)inFile->Get("hist_ptSpectraWeighted");

  centralityParamDist->SetName("hist_normalizedCentralityParamDist");
  ptSpectra->SetName("hist_normalizedPtSpectra");
  ptSpectraWeighted->SetName("hist_normalizedPtSpectraWeighted");

  for(int j=0; j< ptSpectra->GetNbinsX(); j++){
    ptSpectra->SetBinContent(j+1, ptSpectra->GetBinContent(j+1)*1.0/dEtaBin/dPtBins[j]);
    ptSpectra->SetBinError(j+1, ptSpectra->GetBinError(j+1)*1.0/dEtaBin/dPtBins[j]);
    ptSpectraWeighted->SetBinContent(j+1, ptSpectraWeighted->GetBinContent(j+1)*1.0/dEtaBin/dPtBins[j]);
    ptSpectraWeighted->SetBinError(j+1, ptSpectraWeighted->GetBinError(j+1)*1.0/dEtaBin/dPtBins[j]);
  }
  if(centralityParamDist->GetEntries()>0){
    for(int j=0; j< ptSpectra->GetNbinsX(); j++){
      ptSpectra->SetBinContent(j+1, ptSpectra->GetBinContent(j+1)*1.0/centralityParamDist->GetEntries());
      ptSpectra->SetBinError(j+1, ptSpectra->GetBinError(j+1)*1.0/centralityParamDist->GetEntries());
      ptSpectraWeighted->SetBinContent(j+1, ptSpectraWeighted->GetBinContent(j+1)*1.0/centralityParamDist->GetEntries());
      ptSpectraWeighted->SetBinError(j+1, ptSpectraWeighted->GetBinError(j+1)*1.0/centralityParamDist->GetEntries());
    }
  } 

  TFile *outFile = new TFile("normalized_output_hist_test.root","recreate");
  outFile->cd();
  centralityParamDist->Write();
  ptSpectra->Write();
  ptSpectraWeighted->Write();

  inFile->Close();
}


