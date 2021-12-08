#include "cmsVariables.h"

void raa(string inRootFilePbpb, string inRootFilePp){

  char cInRootFilePbpb[inRootFilePbpb.length()+1];
  char cInRootFilePp[inRootFilePp.length()+1];

  strcpy(cInRootFilePbpb, inRootFilePbpb.c_str());
  strcpy(cInRootFilePp, inRootFilePp.c_str());

  TFile *inFilePbPb = new TFile(cInRootFilePbpb,"read");  
  TFile *inFilepp = new TFile(cInRootFilePp,"read");  

  double dEtaBin = etaMax - etaMin;
  double dPtBins[nPtBins];
  for(int i=0; i<nPtBins; i++){
    dPtBins[i] = ptBins[i+1] - ptBins[i];
  }

  TH1D::SetDefaultSumw2();

  // for pbpb
  TH1D *bDist[nCentralityBins];  
  TH1D *ptSpectra[nCentralityBins];
  TH1D *ptSpectraWeighted[nCentralityBins];

  // for pp
  TH1D *bDistpp;
  TH1D *ptSpectrapp;
  TH1D *ptSpectraWeightedpp;
 // bDistpp = (TH1D*)inFilepp->Get("hist_normalizedCentralityParamDist");
 // ptSpectrapp = (TH1D*)inFilepp->Get("hist_normalizedPtSpectra");
 // ptSpectraWeightedpp = (TH1D*)inFilepp->Get("hist_normalizedPtSpectraWeighted");
  bDistpp = (TH1D*)inFilepp->Get("hist_centralityParamDistPp");
  ptSpectrapp = (TH1D*)inFilepp->Get("hist_ptSpectraPp");
  ptSpectraWeightedpp = (TH1D*)inFilepp->Get("hist_ptSpectraWeightedPp");

  // for pp spectra
  for(int j=0; j< ptSpectrapp->GetNbinsX(); j++){
  ptSpectrapp->SetBinContent(j+1, ptSpectrapp->GetBinContent(j+1)*1.0/dEtaBin/dPtBins[j]/(2.0*TMath::Pi()*ptSpectrapp->GetBinCenter(j+1)));
    ptSpectrapp->SetBinError(j+1, ptSpectrapp->GetBinError(j+1)*1.0/dEtaBin/dPtBins[j]/(2.0*TMath::Pi()*ptSpectrapp->GetBinCenter(j+1)));
    ptSpectraWeightedpp->SetBinContent(j+1, ptSpectraWeightedpp->GetBinContent(j+1)*1.0/dEtaBin/dPtBins[j]/(2.0*TMath::Pi()*ptSpectraWeightedpp->GetBinCenter(j+1)));
    ptSpectraWeightedpp->SetBinError(j+1, ptSpectraWeightedpp->GetBinError(j+1)*1.0/dEtaBin/dPtBins[j]/(2.0*TMath::Pi()*ptSpectraWeightedpp->GetBinCenter(j+1)));
  }
  if(bDistpp->GetEntries()>0){
    for(int j=0; j< ptSpectrapp->GetNbinsX(); j++){
      ptSpectrapp->SetBinContent(j+1, ptSpectrapp->GetBinContent(j+1)*1.0/bDistpp->GetEntries());
      ptSpectrapp->SetBinError(j+1, ptSpectrapp->GetBinError(j+1)*1.0/bDistpp->GetEntries());
      ptSpectraWeightedpp->SetBinContent(j+1, ptSpectraWeightedpp->GetBinContent(j+1)*1.0/bDistpp->GetEntries());
      ptSpectraWeightedpp->SetBinError(j+1, ptSpectraWeightedpp->GetBinError(j+1)*1.0/bDistpp->GetEntries());
    }
  }

  // for PbPb spectra
  for(int i=0; i<nCentralityBins; i++){
    bDist[i] = (TH1D*)inFilePbPb->Get(Form("hist_centralityParamDist_%d",i));
    ptSpectra[i] = (TH1D*)inFilePbPb->Get(Form("hist_ptSpectra_%d",i));
    ptSpectraWeighted[i] = (TH1D*)inFilePbPb->Get(Form("hist_ptSpectraWeighted_%d",i));

    for(int j=0; j< ptSpectra[i]->GetNbinsX(); j++){
     ptSpectra[i]->SetBinContent(j+1, ptSpectra[i]->GetBinContent(j+1)*1.0/dEtaBin/dPtBins[j]/(2.0*TMath::Pi()*ptSpectra[i]->GetBinCenter(j+1)));
      ptSpectra[i]->SetBinError(j+1, ptSpectra[i]->GetBinError(j+1)*1.0/dEtaBin/dPtBins[j]/(2.0*TMath::Pi()*ptSpectra[i]->GetBinCenter(j+1)));
      ptSpectraWeighted[i]->SetBinContent(j+1, ptSpectraWeighted[i]->GetBinContent(j+1)*1.0/dEtaBin/dPtBins[j]/(2.0*TMath::Pi()*ptSpectraWeighted[i]->GetBinCenter(j+1)));
      ptSpectraWeighted[i]->SetBinError(j+1, ptSpectraWeighted[i]->GetBinError(j+1)*1.0/dEtaBin/dPtBins[j]/(2.0*TMath::Pi()*ptSpectraWeighted[i]->GetBinCenter(j+1)));
    }
    if(bDist[i]->GetEntries()>0){
      for(int j=0; j< ptSpectra[i]->GetNbinsX(); j++){
        ptSpectra[i]->SetBinContent(j+1, ptSpectra[i]->GetBinContent(j+1)*1.0/bDist[i]->GetEntries());
        ptSpectra[i]->SetBinError(j+1, ptSpectra[i]->GetBinError(j+1)*1.0/bDist[i]->GetEntries());
        ptSpectraWeighted[i]->SetBinContent(j+1, ptSpectraWeighted[i]->GetBinContent(j+1)*1.0/bDist[i]->GetEntries());
        ptSpectraWeighted[i]->SetBinError(j+1, ptSpectraWeighted[i]->GetBinError(j+1)*1.0/bDist[i]->GetEntries());
      }
    }

  } // end for cent loop

  double nColl[nCentralityBins] = {1820, 1430, 805, 267, 65.4, 10.7};
  TH1D *raa_pt[nCentralityBins];
  TH1D *raaWeighted_pt[nCentralityBins];

  // calculating RAA in each centrality and pT bins
  for(int i=0; i<nCentralityBins; i++){
   raa_pt[i] = (TH1D*)ptSpectra[i]->Clone(Form("raa_pt_%d",i));
   raaWeighted_pt[i] = (TH1D*)ptSpectraWeighted[i]->Clone(Form("raaWeighted_pt_%d",i));

   raa_pt[i]->Divide(ptSpectrapp);
   raaWeighted_pt[i]->Divide(ptSpectraWeightedpp);

   raa_pt[i]->Scale(1.0/nColl[i]);
   raaWeighted_pt[i]->Scale(1.0/nColl[i]);
  //pythia.readString("HardQCD:all = on");
  //pythia/pythia..read/pythia.String("HardQCD:all = on");
  //pythia.readString("PhaseSpace:pTHatMin = 0.");
  //pythia.readString("PhaseSpace:pTHatMin = 0.");

  }

     TFile *outFile = new TFile("output_hist_raa2.root","recreate");
     outFile->cd();
     for(int i=0; i<nCentralityBins; i++){
      raa_pt[i]->Write();
      raaWeighted_pt[i]->Write();
      ptSpectra[i]->Write();
      ptSpectraWeighted[i]->Write();
     }
     ptSpectrapp->Write();
     ptSpectraWeightedpp->Write();

}


