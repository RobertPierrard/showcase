void raa(){

  TFile *inFilePbPb = new TFile("pbpb/output_hist_bCentralityParam.root","read");  
  TFile *inFilepp = new TFile("pp/output_hist_pp.root","read");  

  const int nCentralityBins = 6;
  double centralityBins[nCentralityBins+1] = {0, 5, 10, 30, 50, 70, 90};
  const int nPtBins = 29;
  double ptBins[nPtBins+1] = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 3.2, 4.0, 4.8, 5.6, 6.4, 7.2, 9.6, 12.0, 14.4, 19.2, 24.0, 28.8, 35.2, 41.6, 48.0, 60.8};
  double etaMin = -1.0;
  double etaMax = 1.0;
  double dEtaBin = etaMax - etaMin;
  double dPtBins[nPtBins];
  for(int i=0; i<nPtBins; i++){
    dPtBins[i] = ptBins[i+1] - ptBins[i];
  }

  TH1D::SetDefaultSumw2();

  TH1D *bDist[nCentralityBins];  
  TH1D *ptSpectra[nCentralityBins];
  TH1D *ptSpectraWeighted[nCentralityBins];
  TH1D *bDistpp;
  TH1D *ptSpectrapp;
  TH1D *ptSpectraWeightedpp;
  bDistpp = (TH1D*)inFilepp->Get(Form("hist_centralityParamDistPp"));
  ptSpectrapp = (TH1D*)inFilepp->Get(Form("hist_ptSpectraPp"));
  ptSpectraWeightedpp = (TH1D*)inFilepp->Get(Form("hist_ptSpectraWeightedPp"));

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
  //double nColl[nCentralityBins] = {525.831, 456.355, 326.81, 175.519, 76.7348, 23.4572};
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

  }

     TFile *outFile = new TFile("output_hist_raa.root","recreate");
     outFile->cd();
     for(int i=0; i<nCentralityBins; i++){
      raa_pt[i]->GetXaxis()->SetTitle("p_{T}");
      raa_pt[i]->GetYaxis()->SetTitle("R_{AA}");
      raa_pt[i]->SetMarkerStyle(20);
      raa_pt[i]->Write();
      raaWeighted_pt[i]->GetXaxis()->SetTitle("p_{T}");
      raaWeighted_pt[i]->GetYaxis()->SetTitle("R_{AA} Weighted");
      raaWeighted_pt[i]->SetMarkerStyle(20);
      raaWeighted_pt[i]->Write();
      ptSpectra[i]->GetXaxis()->SetTitle("p_{T}");
      ptSpectra[i]->GetYaxis()->SetTitle("Spectra");
      ptSpectra[i]->SetMarkerStyle(20);
      ptSpectra[i]->Write();
      ptSpectraWeighted[i]->GetXaxis()->SetTitle("p_{T}");
      ptSpectraWeighted[i]->GetYaxis()->SetTitle("Spectra Weighted");
      ptSpectraWeighted[i]->SetMarkerStyle(20);
      ptSpectraWeighted[i]->Write();
     }
     ptSpectrapp->GetXaxis()->SetTitle("p_{T}");
     ptSpectrapp->GetYaxis()->SetTitle("Spectra");
     ptSpectrapp->SetMarkerStyle(20);
     ptSpectrapp->SetName("hist_ptSpectra_pp");
     ptSpectrapp->Write();
     ptSpectraWeightedpp->GetXaxis()->SetTitle("p_{T}");
     ptSpectraWeightedpp->GetYaxis()->SetTitle("Spectra Weighted");
     ptSpectraWeightedpp->SetMarkerStyle(20);
     ptSpectraWeightedpp->SetName("hist_ptSpectraWeighted_pp");
     ptSpectraWeightedpp->Write();


    // making plots start from here 

        TCanvas *c1 = new TCanvas("c1","c1",1.1*1*650,1*650);
        //c1->Divide(3,2);
        gStyle->SetOptStat(0);
        gStyle->SetErrorX(0);

 TH1D* hist = new TH1D("hist","",160,0.4,80.);
 hist->SetXTitle("p_{T} (GeV)");
 hist->SetYTitle("#frac{d^{2}N}{2#pip_{T}N_{evt}dp_{T}d#eta}(GeV^{-2})");
 hist->SetMinimum(0.5e-10);
 hist->SetMaximum(9900);
 hist->GetYaxis()->SetNdivisions(505);
 hist->GetXaxis()->SetNdivisions(505);
 hist->GetXaxis()->CenterTitle(1);
 hist->GetYaxis()->CenterTitle(1);
 hist->GetYaxis()->SetTitleOffset(1.6);
 hist->GetXaxis()->SetTitleOffset(1.15);
 hist->GetXaxis()->SetTitleSize(0.05);
 hist->GetYaxis()->SetTitleSize(0.05);
 hist->GetXaxis()->SetLabelSize(0.05);
 hist->GetYaxis()->SetLabelSize(0.05);
 hist->Draw();

        gPad->SetTopMargin(0.02);
        gPad->SetBottomMargin(0.15);
        gPad->SetLeftMargin(0.20);
        gPad->SetRightMargin(0.02);
        gPad->SetTicks(-1);
        gPad->SetLogy();
        gPad->SetLogx();
      ptSpectrapp->SetMarkerStyle(5);
      ptSpectrapp->SetMarkerSize(1.2);
      ptSpectrapp->SetMarkerColor(1);
      ptSpectrapp->SetLineColor(1);
      ptSpectrapp->Draw("psameez");
     int colors[nCentralityBins] = {1, 4, 2, 1, 4, 2};
     int markers[nCentralityBins] = {24, 25, 28, 20, 21, 34};
     ptSpectra[0]->Scale(10.0);
     ptSpectra[1]->Scale(3.0);
     for(int i=0; i<nCentralityBins; i++){
       ptSpectra[i]->SetMarkerStyle(markers[i]);
       ptSpectra[i]->SetMarkerSize(1);
       ptSpectra[i]->SetMarkerColor(colors[i]);
       
       ptSpectra[i]->Draw("psameez");
     }

    TLegend *leg = new TLegend(0.27,0.2,0.37,0.56);
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
    leg->AddEntry(ptSpectrapp," pp","p");
    leg->Draw();

        TLatex *tex = new TLatex(0.8,0.001,"|#eta|<1");
        tex->SetTextSize(0.04);
        tex->SetLineWidth(2);
        tex->SetTextFont(42);
        tex->Draw();

        tex = new TLatex(6,300,"ANGANTYR 5.02 TeV");
        tex->SetTextSize(0.04);
        tex->SetLineWidth(2);
        tex->SetTextFont(42);
        tex->Draw();


    // making plots start from here for RAA

        TCanvas *c2 = new TCanvas("c2","c2",1.1*1*650,1*650);
        //c1->Divide(3,2);
        gStyle->SetOptStat(0);

 TH1D* hist2 = new TH1D("hist2","",160,0.4,80.);
 hist2->SetXTitle("p_{T} (GeV)");
 hist2->SetYTitle("R_{AA}");
 hist2->SetMinimum(0.);
 hist2->SetMaximum(1.6);
 hist2->GetYaxis()->SetNdivisions(505);
 hist2->GetXaxis()->SetNdivisions(505);
 hist2->GetXaxis()->CenterTitle(1);
 hist2->GetYaxis()->CenterTitle(1);
 hist2->GetYaxis()->SetTitleOffset(1.6);
 hist2->GetXaxis()->SetTitleOffset(1.15);
 hist2->GetXaxis()->SetTitleSize(0.05);
 hist2->GetYaxis()->SetTitleSize(0.05);
 hist2->GetXaxis()->SetLabelSize(0.05);
 hist2->GetYaxis()->SetLabelSize(0.05);
 hist2->Draw();

        gPad->SetTopMargin(0.02);
        gPad->SetBottomMargin(0.15);
        gPad->SetLeftMargin(0.20);
        gPad->SetRightMargin(0.02);
        gPad->SetTicks(-1);
        //gPad->SetLogy();
        gPad->SetLogx();
     //int colors[nCentralityBins] = {1, 4, 2, 1, 4, 2};
     //int markers[nCentralityBins] = {24, 25, 28, 20, 21, 34};
     for(int i=0; i<nCentralityBins; i++){
       raa_pt[i]->SetMarkerStyle(markers[i]);
       raa_pt[i]->SetMarkerSize(1);
       raa_pt[i]->SetMarkerColor(colors[i]);
       
       raa_pt[i]->Draw("psameez");
     }

    TLegend *leg2 = new TLegend(0.27,0.6,0.37,0.86);
    leg2->SetFillColor(10);
    leg2->SetBorderSize(0);
    leg2->SetTextFont(42);
    leg2->SetTextColor(1);
    leg2->SetTextSize(0.04);
    leg2->AddEntry(raa_pt[0]," 0-5%","p");
    leg2->AddEntry(raa_pt[1]," 5-10%","p");
    leg2->AddEntry(raa_pt[2]," 10-30%","p");
    leg2->AddEntry(raa_pt[3]," 30-50%","p");
    leg2->AddEntry(raa_pt[4]," 50-70%","p");
    leg2->AddEntry(raa_pt[5]," 70-90%","p");
    leg2->Draw();

        tex = new TLatex(0.8,1.4,"|#eta|<1");
        tex->SetTextSize(0.04);
        tex->SetLineWidth(2);
        tex->SetTextFont(42);
        tex->Draw();

        tex = new TLatex(6,1.3,"ANGANTYR 5.02 TeV");
        tex->SetTextSize(0.04);
        tex->SetLineWidth(2);
        tex->SetTextFont(42);
        tex->Draw();



}


