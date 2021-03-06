//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Relative calibration of Si gains for SX3 Step 3
// See readme.md for general instructions.
// Usage: root -l SiRelativeGains_Step3.C+
//
// Edited by : John Parker , 2016Jan22
// Developed by : Jon Lighthall, 2017.04
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//C++
#include <fstream>
#include <exception>
//ROOT
#include <TMath.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
//Methods
#include "SiRelativeGains.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SiRelativeGains_Step3(void) {
  using namespace std;
  //f1 = new TFile("/home/lighthall/anasen/root/main/run1227mQ2S3.root");
  f1 = new TFile("/home/lighthall/anasen/root/main/spacer7mQ2S3.root");
  //f1 = new TFile("/home/lighthall/anasen/root/main/run1255-61mQ2S3.root");
  if ( !f1->IsOpen() ){
    cout << "Error: Root file does not exist\n";
    exit(EXIT_FAILURE);
  }
  
  //Input the .dat file used by Main.cpp to generate the .root file given above
  Gains gains;
  gains.Load("saves/X3RelativeGains_Step3_170525.dat");
  gains.Save("saves/X3RelativeGains_Step3");
  Offsets offsets;
  offsets.Load("saves/X3FinalFix_Step3_170525.dat");
  offsets.Save("saves/X3FinalFix_Step3");
  
  BadDetectors bad;
  GainMatch gainmatch;

  for (Int_t DetNum=4; DetNum<ndets+4; DetNum++) {
    for (Int_t BackChNum=1; BackChNum<4; BackChNum++) {
      TH2F *hist = NULL;
      TString hname=Form("back_vs_front%i_b%i",DetNum,BackChNum);
      hist = (TH2F*)f1->Get(hname.Data());
      if (hist==NULL) {
	cout << "****" << hname << " histogram does not exist\n";
	bad.Add(DetNum,-1,BackChNum);
	gains.Add(DetNum-4,BackChNum,0,0);
	offsets.Add(DetNum-4,BackChNum,0,0);
	continue;
      }
      
      Double_t gain = gainmatch.Fit4(hist,gains.old[DetNum-4][BackChNum]);
      //Double_t gain = gainmatch.Fit8(DetNum,BackChNum);
      gains.Add(DetNum-4,BackChNum,gain,1.0/gain);
      offsets.Add(DetNum-4,BackChNum,offset,-offset);
    }
    for (Int_t i=0; i<12; i++){
      outfile        << DetNum << "\t" << i << "\t" <<   gains.old[DetNum-4][i] << endl;
      outfile_offset << DetNum << "\t" << i << "\t" << offsets.old[DetNum-4][i] << endl;
    }
  }
  bad.Print();
}
