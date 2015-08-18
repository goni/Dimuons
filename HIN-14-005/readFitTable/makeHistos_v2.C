#include <Riostream.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TBrowser.h>
#include <TROOT.h>
#include <TGraph.h>
#include <TNtuple.h>
#include <TString.h>
#include <TH1D.h>
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TMath.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TInterpreter.h>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include "Riostream.h"

// remember to replace, in the case of the 'weighted' case, the rel. stat uncertianties with the one from the 'unweighted' yields

void makeHisto(int nDphiBins = 2, 
	       // const char* inputFitDataFileName   = "/Users/eusmartass/Documents/cmswrite/hin-14-005/v2/plots_v2/yieldsData/20140807/weightedEff/summary",
	       // const char* outputHistDataFileName = "/Users/eusmartass/Documents/cmswrite/hin-14-005/v2/plots_v2/yieldsData/yieldsRootFiles/20140807_weightedEff_histsDPhiYields.root"
	       // const char* inputFitDataFileName   = "/Users/eusmartass/Documents/cmswrite/hin-14-005/data/raa/20141020_PbPb_raa_v2_fwdCorrPt3/v2/20141020/v2noW_InEta/summary",
	       // const char* outputHistDataFileName = "/Users/eusmartass/Documents/cmswrite/hin-14-005/v2/plots_v2/yieldsData/rootFile/20141020_noWeight_histsDPhiYields.root"
	       const char* inputFitDataFileName   = "/Users/eusmartass/Documents/cmswrite/hin-14-005/data/raa/20141028_PbPb_v2_2dPhiBins/v2W_InEta_2dphiBins/summary",
	       const char* outputHistDataFileName = "/Users/eusmartass/Documents/cmswrite/hin-14-005/v2/plots_v2/yieldsData/rootFile/20141028_yesW_2dphi_histsDPhiYields.root"
	       ) 
{
  double PI = TMath::Pi();

  ifstream in;
  in.open(Form("%s/fit_table",inputFitDataFileName));
  
  double x[300];
 
  TFile *pfOutput;
  pfOutput = new TFile(Form("%s",outputHistDataFileName),"RECREATE");
  
  TH1F *hInc[300];
  TH1F *hPrp[300];
  TH1F *hNPrp[300];
  
  double prpt[300], prptErr[300], nprpt[300], nprptErr[300];
  double inc[300], incErr[300];
  double rap1[300], rap2[300], pT1[300], pT2[300];//, cent1[300], cent2[300];
  int cent1[300], cent2[300];
  char tmp[512];
  int nline = 0;
  while (1) 
    {
      in >> x[0] >> x[1] >> x[2] >> x[3] >> x[4] >> x[5] >> x[6] >> x[7] >> x[8] >> x[9] >> x[10] >> x[11] >> x[12] >> x[13] >> x[14] >> x[15] >> x[16] >> x[17];
      rap1[nline]  = x[0];  rap2[nline]     = fabs(x[1]); // rapidity (second value comes with '-')
      pT1[nline]   = x[2];  pT2[nline]      = fabs(x[3]); // pt (second value comes with '-')
      cent1[nline] = x[4];  cent2[nline]    = fabs(x[5]);// centrlaity (second value comes with '-')
      // [6]&[7] is the phi interval
      inc[nline]   = x[8];  incErr[nline]   = fabs(x[9]);  // inclusive yield and error
      //[10]&[11] is the bkg and bkg error
      prpt[nline]  = x[12]; prptErr[nline]  = fabs(x[13]); // prompt yield and error
      nprpt[nline] = x[14]; nprptErr[nline] = fabs(x[15]); // non-prompt yield and error
      //[16]&[17] is the b-fraction
      if (!in.good()) break;
      if(nline == 11)
	{
	  cout<<tmp<<endl;
	  cout<<"Stored values in line : "<<nline<<endl;
	  cout<<rap1[nline]<<" "<<rap2[nline]<<" ";
	  cout<<pT1[nline]<<" "<<pT2[nline]<<" ";
	  cout<<cent1[nline]<<" "<<cent2[nline]<<" ";
	      cout<<inc[nline]<<" "<<prpt[nline] <<" "<<prptErr[nline]<<" "<<nprpt[nline] <<" "<<nprptErr[nline]<<endl;
	}
      nline++;
    }
  printf(" found %d points\n",nline);
  
  pfOutput->cd();
  // Make histograms

  for(int j = 0; j < nline/(nDphiBins+1); j++)
    {
 

      if(nDphiBins==4)
	{
     // inclusive histograms
	  TString histIncl(Form("Rap_%0.1f%0.1f_pT_%0.1f%0.1f_Cent_%d%d_Inc",rap1[5*j],rap2[5*j],pT1[5*j],pT2[5*j],cent1[5*j],cent2[5*j]));
	  histIncl.ReplaceAll(".","");
	  hInc[j] = new TH1F(histIncl,";#Delta #phi;",4,0,PI/2);
	  
	  hInc[j]->Sumw2();
	  hInc[j]->SetBinContent(1, inc[5*j]);
	  hInc[j]->SetBinContent(2, inc[5*j+2]);
	  hInc[j]->SetBinContent(3, inc[5*j+3]);
	  hInc[j]->SetBinContent(4, inc[5*j+4]);
	  hInc[j]->SetBinError(1, incErr[5*j]);
	  hInc[j]->SetBinError(2, incErr[5*j+2]);
	  hInc[j]->SetBinError(3, incErr[5*j+3]);
	  hInc[j]->SetBinError(4, incErr[5*j+4]);
	  //cout<<tmp<<endl;
	  hInc[j]->Write();
      
      //prompt histograms
	  TString histPrompt(Form("Rap_%0.1f%0.1f_pT_%0.1f%0.1f_Cent_%d%d_Prp",rap1[5*j],rap2[5*j],pT1[5*j],pT2[5*j],cent1[5*j],cent2[5*j]));
	  histPrompt.ReplaceAll(".","");
	  hPrp[j] = new TH1F(histPrompt,";#Delta #phi;",4,0,PI/2);
	  hPrp[j]->SetBinContent(1, prpt[5*j]);
	  hPrp[j]->SetBinContent(2, prpt[5*j+2]);
	  hPrp[j]->SetBinContent(3, prpt[5*j+3]);
	  hPrp[j]->SetBinContent(4, prpt[5*j+4]);
	  hPrp[j]->SetBinError(1, prptErr[5*j]);
	  hPrp[j]->SetBinError(2, prptErr[5*j+2]);
	  hPrp[j]->SetBinError(3, prptErr[5*j+3]);
	  hPrp[j]->SetBinError(4, prptErr[5*j+4]);
	  //cout<<tmp<<endl;
	  hPrp[j]->Write();
      
	  //non-prompt histograms
	  TString histNonPrompt(Form("Rap_%0.1f%0.1f_pT_%0.1f%0.1f_Cent_%d%d_NPrp",rap1[5*j],rap2[5*j],pT1[5*j],pT2[5*j],cent1[5*j],cent2[5*j]));
	  histNonPrompt.ReplaceAll(".","");
	  hNPrp[j] = new TH1F(histNonPrompt,";#Delta #phi;",4,0,PI/2);
	  hNPrp[j]->SetBinContent(1, nprpt[5*j]);
	  hNPrp[j]->SetBinContent(2, nprpt[5*j+2]);
	  hNPrp[j]->SetBinContent(3, nprpt[5*j+3]);
	  hNPrp[j]->SetBinContent(4, nprpt[5*j+4]);
	  hNPrp[j]->SetBinError(1, nprptErr[5*j]);
	  hNPrp[j]->SetBinError(2, nprptErr[5*j+2]);
	  hNPrp[j]->SetBinError(3, nprptErr[5*j+3]);
	  hNPrp[j]->SetBinError(4, nprptErr[5*j+4]);
	  //cout<<tmp<<endl;
	  hNPrp[j]->Write();
	}

      if(nDphiBins==2)
	{
     // inclusive histograms
	  TString histIncl(Form("Rap_%0.1f%0.1f_pT_%0.1f%0.1f_Cent_%d%d_Inc",rap1[3*j],rap2[3*j],pT1[3*j],pT2[3*j],cent1[3*j],cent2[3*j]));
	  histIncl.ReplaceAll(".","");
	  hInc[j] = new TH1F(histIncl,";#Delta #phi;",2,0,PI/2);
	  
	  hInc[j]->Sumw2();
	  hInc[j]->SetBinContent(1, inc[3*j]);
	  hInc[j]->SetBinContent(2, inc[3*j+2]);
	  hInc[j]->SetBinError(1, incErr[3*j]);
	  hInc[j]->SetBinError(2, incErr[3*j+2]);

	  //cout<<tmp<<endl;
	  hInc[j]->Write();
      
      //prompt histograms
	  TString histPrompt(Form("Rap_%0.1f%0.1f_pT_%0.1f%0.1f_Cent_%d%d_Prp",rap1[3*j],rap2[3*j],pT1[3*j],pT2[3*j],cent1[3*j],cent2[3*j]));
	  histPrompt.ReplaceAll(".","");
	  hPrp[j] = new TH1F(histPrompt,";#Delta #phi;",2,0,PI/2);
	  hPrp[j]->SetBinContent(1, prpt[3*j]);
	  hPrp[j]->SetBinContent(2, prpt[3*j+2]);
	  hPrp[j]->SetBinError(1, prptErr[3*j]);
	  hPrp[j]->SetBinError(2, prptErr[3*j+2]);
		  //cout<<tmp<<endl;
	  hPrp[j]->Write();
      
	  //non-prompt histograms
	  TString histNonPrompt(Form("Rap_%0.1f%0.1f_pT_%0.1f%0.1f_Cent_%d%d_NPrp",rap1[3*j],rap2[3*j],pT1[3*j],pT2[3*j],cent1[3*j],cent2[3*j]));
	  histNonPrompt.ReplaceAll(".","");
	  hNPrp[j] = new TH1F(histNonPrompt,";#Delta #phi;",2,0,PI/2);
	  hNPrp[j]->SetBinContent(1, nprpt[3*j]);
	  hNPrp[j]->SetBinContent(2, nprpt[3*j+2]);
	  hNPrp[j]->SetBinError(1, nprptErr[3*j]);
	  hNPrp[j]->SetBinError(2, nprptErr[3*j+2]);
	  //cout<<tmp<<endl;
	  hNPrp[j]->Write();

	}
      
      
      in.close();
      
      pfOutput->Write();
    }

}
