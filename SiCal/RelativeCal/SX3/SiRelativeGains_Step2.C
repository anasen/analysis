//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Relative calibration of Si gains for SX3 Step 2
// See readme.md for general instructions.
// Usage: root -l SiRelativeGains_Step2.C+
//
// Edited by : John Parker , 2016Jan22
// Edited by : Maria Anastasiou, 2016Sept20
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
void SiRelativeGains_Step2(void) {
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
  gains.Save("saves/X3RelativeGains_Step2");
  Offsets offsets;
  offsets.Load("saves/X3FinalFix_Step3_170525.dat");
  offsets.Save("saves/X3FinalFix_Step2");
  
  BadDetectors bad;
  GainMatch gainmatch;

  for (Int_t DetNum=4; DetNum<ndets+4; DetNum++) {
    //if(DetNum!=17) continue;
    for (Int_t FrontChNum=0; FrontChNum<4; FrontChNum++) {
      Int_t BackChNum = 0;   // some if-statements that differ between each data set
      if(DetNum==12&&FrontChNum>1) {
	BackChNum = 3;
      }
      TH2F *hist = NULL;
      TString hname=Form("back_vs_front%i_%i_%i",DetNum,FrontChNum,BackChNum);
      hist = (TH2F*)f1->Get(hname.Data());
      if (hist==NULL) {
	cout << "****" << hname << " histogram does not exist\n";
	bad.Add(DetNum,FrontChNum,BackChNum);
	gains.Add(DetNum-4,FrontChNum+4,0,0);
	gains.Add(DetNum-4,FrontChNum+8,0,0);
	offsets.Add(DetNum-4,FrontChNum+4,0,0);
	offsets.Add(DetNum-4,FrontChNum+8,0,0);
	continue;
      }
      
      Double_t gain = gainmatch.Fit4(hist,gains.old[DetNum-4][FrontChNum+4]);
      //Double_t gain = gainmatch.Fit7(DetNum,FrontChNum,BackChNum);
      gains.Add(DetNum-4,FrontChNum+4,gain,gain);
      gains.Add(DetNum-4,FrontChNum+8,gain,gain);
      offsets.Add(DetNum-4,FrontChNum+4,offset,offset/2);
      offsets.Add(DetNum-4,FrontChNum+8,offset,offset/2);
    }
    for (Int_t i=0; i<12; i++){
      outfile        << DetNum << "\t" << i << "\t" <<   gains.old[DetNum-4][i] << endl;
      outfile_offset << DetNum << "\t" << i << "\t" << offsets.old[DetNum-4][i] << endl;
    }
  }
  bad.Print();
}
