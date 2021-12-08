// main111.cc is a part of the PYTHIA event generator.
// Copyright (C) 2021 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Authors: Leif Lonnblad <leif.lonnblad@thep.lu.se>.
//	    Robert Pierrard <rpierrard3@gatech.edu>.
// Keywords: heavy ions; rivet; angantyr;

// This is a simple test program equivalent to main01.cc but using the
// Angantyr model for Heavy Ion collisions. It is still proton
// collisions, but uses the Angantyr impact parameter description to
// select collisions. It fits on one slide in a talk.  It studies the
// charged multiplicity distribution at the LHC.

// Optionally (by compiling with the flag -DRIVET and
// linking with rivet - see output of the command "rivet-config
// --cppflags --libs") it will send the event to Rivet for an ATLAS
// jet-analysis.

#include "Pythia8/Pythia.h"

#ifdef RIVET
#include "Pythia8/HeavyIons.h"
#include "Pythia8Plugins/Pythia8Rivet.h"
#endif

#include "Pythia8Plugins/ProgressLog.h"

// ROOT, for histogramming.
#include "TH1.h"

// ROOT, for interactive graphics.
#include "TVirtualPad.h"
#include "TApplication.h"

// ROOT, for saving file.
#include "TFile.h"
#include "TTree.h"


using namespace Pythia8;

int main(int argc, char* argv[]) {

  // Create the ROOT application environment.
  TApplication theApp("hist", &argc, argv);

  // Generator. Process selection. LHC initialization. Histogram.
  Pythia pythia;

  // This forces the HeavyIons model to be used even for pp collisons.
  pythia.readString("HeavyIon:mode = 2");

  pythia.readString("Beams:eCM = 5020.");
  pythia.readString("HardQCD:all = on");
  pythia.readString("PhaseSpace:pTHatMin = 20.");

  // Only do a couple generations in the fitting to cross sections.
  pythia.readString("HeavyIon:SigFitNGen = 4");

  int nEvents = 20; // Number of events simulated

  pythia.init();

  const int maxMultiplicity = 50000;
  Int_t nMultiplicityTree;//, nPartTree; //tracks the total number of particles generated?
//  Int_t pidTree[maxMultiplicity];
  Float_t ptTree[maxMultiplicity], etaTree[maxMultiplicity], phiTree[maxMultiplicity];
  Double_t weightTree;
  int savedMultiplicity;

  TFile *outTreeFile = new TFile("angantyr_pp_5020GeV_hadron_tree.root", "RECREATE");
  TTree *hadronTree = new TTree("hadronTree", "Tree with hadron output from PYTHIA8 using ANGANTYR");
  hadronTree->Branch("nMultiplicityTree", &nMultiplicityTree, "nMultiplicityTree/I");
//  hadronTree->Branch("nPartTree", &nPartTree, "nPartTree/I");
  hadronTree->Branch("weightTree", &weightTree, "weightTree/D");
//  hadronTree->Branch("pidTree", &pidTree, "pidTree[nMultiplicityTree]/I");
  hadronTree->Branch("ptTree", &ptTree, "ptTree[nMultiplicityTree]/F");
  hadronTree->Branch("etaTree", &etaTree, "etaTree[nMultiplicityTree]/F");
  hadronTree->Branch("phiTree", &phiTree, "phiTree[nMultiplicityTree]/F");

  // Book a histogram of the multiplicity distribution
  Hist mult("charged multiplicity", 100, -0.5, 799.5);

  // Initialise the printout of run progress information.
  ProgressLog logger(nEvents);

  // Begin event loop. Generate event. Skip if error. List first one.
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    if (!pythia.next()) continue;

    // Find number of all final charged particles and fill histogram.
    int nCharged = 0;
    double weight = pythia.info.weight();
    for (int i = 0; i < pythia.event.size(); ++i){
      	Particle & p = pythia.event[i];
	if (p.isFinal() && p.isCharged() && 
          abs(p.eta()) < 2.4 && p.pT() > 0.1){
        	ptTree[nCharged] = p.pT();
	        etaTree[nCharged] = p.eta();
	        phiTree[nCharged] = p.phi();
	        nCharged++;
		mult.fill(nCharged);
	}
    }

    weightTree = weight;
    nMultiplicityTree = nCharged;

    hadronTree->Fill();
    
    // Intermittently report run progress.
    logger(iEvent);
  }

    // End of event loop. Statistics. Histogram. Done.
    hadronTree->Write();
    outTreeFile->Write();

    pythia.stat();
    cout << mult;

    // Done.
    return 0;
}
