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

void analysisPtSpectraPbpb(string inRootFile){
  char cInRootFile[inRootFile.length()+1];
  strcpy(cInRootFile, inRootFile.c_str());

  TFile *inFile = new TFile(cInRootFile,"read");  

  // variable derived from CMS data
  double dEtaBin = etaMax - etaMin;
  double dPtBins[nPtBins];
  for(int i=0; i<nPtBins; i++){
    dPtBins[i] = ptBins[i+1] - ptBins[i];
  }

  // defining new histograms
  TH1D::SetDefaultSumw2();

  TH1D *centralityParamDist[nCentralityBins];  
  TH1D *ptSpectra[nCentralityBins];
  TH1D *ptSpectraWeighted[nCentralityBins];

  // looping through histograms (each histogram is a different centrality region)
  for(int i=0; i<nCentralityBins; i++){
    // opening histograms from file
    centralityParamDist[i] = (TH1D*)inFile->Get(Form("hist_centralityParamDist_%d",i));
    ptSpectra[i] = (TH1D*)inFile->Get(Form("hist_ptSpectra_%d",i));
    ptSpectraWeighted[i] = (TH1D*)inFile->Get(Form("hist_ptSpectraWeighted_%d",i));
    // renaming histograms
    centralityParamDist[i]->SetName(Form("hist_normalizedCentralityParamDist_%d",i));
    ptSpectra[i]->SetName(Form("hist_normalizedPtSpectra_%d",i));
    ptSpectraWeighted[i]->SetName(Form("hist_normalizedPtSpectraWeighted_%d",i));
    // normalizing for dEtaBins and dPtBins
    for(int j=0; j< ptSpectra[i]->GetNbinsX(); j++){
      ptSpectra[i]->SetBinContent(j+1, ptSpectra[i]->GetBinContent(j+1)*1.0/dEtaBin/dPtBins[j]);
      ptSpectra[i]->SetBinError(j+1, ptSpectra[i]->GetBinError(j+1)*1.0/dEtaBin/dPtBins[j]);
      ptSpectraWeighted[i]->SetBinContent(j+1, ptSpectraWeighted[i]->GetBinContent(j+1)*1.0/dEtaBin/dPtBins[j]);
      ptSpectraWeighted[i]->SetBinError(j+1, ptSpectraWeighted[i]->GetBinError(j+1)*1.0/dEtaBin/dPtBins[j]);
    }
    // normalizing for number of entries in each centrality bins.
    if(centralityParamDist[i]->GetEntries()>0){
      for(int j=0; j< ptSpectra[i]->GetNbinsX(); j++){
        ptSpectra[i]->SetBinContent(j+1, ptSpectra[i]->GetBinContent(j+1)*1.0/centralityParamDist[i]->GetEntries());
        ptSpectra[i]->SetBinError(j+1, ptSpectra[i]->GetBinError(j+1)*1.0/centralityParamDist[i]->GetEntries());
        ptSpectraWeighted[i]->SetBinContent(j+1, ptSpectraWeighted[i]->GetBinContent(j+1)*1.0/centralityParamDist[i]->GetEntries());
        ptSpectraWeighted[i]->SetBinError(j+1, ptSpectraWeighted[i]->GetBinError(j+1)*1.0/centralityParamDist[i]->GetEntries());
      }
    }
  } // end of centrality loop

  // create output file
  TFile *outFile = new TFile("normalized_output_hist_test.root","recreate");
  outFile->cd();
  // write modified histograms to new output file.
  for(int i=0; i<nCentralityBins; i++){
    centralityParamDist[i]->Write();
    ptSpectra[i]->Write();
    ptSpectraWeighted[i]->Write();
  }

  // close input file
  inFile->Close();
}


