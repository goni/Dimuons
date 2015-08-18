/*
Macro to plot the Efficiency vs centrality, for all the analysis bins.(in the traditional way, yield x correction, and the weighted way)

Input: root files produced by the readFitTable/makeHisto_raa.C (check there the input file names)

Output: the Raa vs cent.

 */
#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include "TROOT.h"
#include "TStyle.h"
#include "TMath.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TPaveStats.h"
#include "TLatex.h"
#include "TLegend.h"
#include "dataBinning_2015.h"
#endif

void makeRaa_cent(bool bSavePlots=1,
		  bool bDoDebug = 1, // adds some numbers, numerator, denominator, to help figure out if things are read properly
		  bool bAddLumi = 0, // add the lumi boxes at raa=1
		  bool bAddLegend = 1, 
		  bool bOnlyLowPtCent=0,
		  int weight = 0, //0=raw yields, 1=corrected yields
		  const char* inputDir="../readFitTable", // the place where the input root files, with the histograms are
		  const char* outputDir="figs")// where the output figures will be
{
  gROOT->Macro("../logon.C+");
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(kFALSE);

  // input files: 
  // const char* yieldHistFile_yesWeight[2] = {"histsRaaYields_20150127_PbPb_raa_weightedEff_InEta.root",
  // 					    "histsRaaYields_20150127_pp_raa_weightedEff_InEta.root"};
  // const char* yieldHistFile_noWeight[2]  = {"histsRaaYields_20150127_PbPb_raa_noWeight_InEta.root",
  // 					    "histsRaaYields_20150127_pp_raa_noWeight_InEta.root"};


  const char* yieldHistFile_yesWeight[2] = {"histsRaaYields_20150817_PbPb_raa_Lxyz_weightedEff_Lxyz_pTtune_PRMC.root",
  					    "histsRaaYields_20150817_pp_Lxyz_weightedEff_Lxyz_finerpT_PRMC.root"};
  
  const char* yieldHistFile_noWeight[2]  = {"histsRaaYields_20150817_PbPb_raa_Lxyz_noWeight_Lxyz_pTtune_PRMC.root",
					    "histsRaaYields_20150817_pp_Lxyz_noWeight_Lxyz_finerpT_PRMC.root"};


  const char* effHistFile[2]             = {"histEff_pbpb_tradEff_nov12.root",
					    "histEff_pp_tradEff_nov12.root"};
  const int nInHist = 5;
  const char* yieldHistNames[nInHist] = {"cent","y012Cent", "y1216Cent", "y1624Cent", "y1624LowPtCent"};

  // open the files with yields and do the math
  TFile *fYesWeighFile_aa   = new TFile(Form("%s/%s",inputDir,yieldHistFile_yesWeight[0]));
  TFile *fYesWeighFile_pp   = new TFile(Form("%s/%s",inputDir,yieldHistFile_yesWeight[1]));
  
  TFile *fNoWeighFile_aa = new TFile(Form("%s/%s",inputDir,yieldHistFile_noWeight[0]));
  TFile *fNoWeighFile_pp = new TFile(Form("%s/%s",inputDir,yieldHistFile_noWeight[1]));

  TFile *fEffFile_aa = new TFile(Form("%s/%s",inputDir,effHistFile[0]));
  TFile *fEffFile_pp = new TFile(Form("%s/%s",inputDir,effHistFile[1]));

  TH1F *phRaw_pr_pp; 
  TH1F *phCorr_pr_pp;
  TH1F *phEff_pr_pp;
  TH1F *phRaw_pr_aa; 
  TH1F *phCorr_pr_aa;
  TH1F *phEff_pr_aa;

  TH1F *phRaw_npr_pp; 
  TH1F *phCorr_npr_pp;
  TH1F *phEff_npr_pp;
  TH1F *phRaw_npr_aa; 
  TH1F *phCorr_npr_aa;
  TH1F *phEff_npr_aa;
  
  for(int ih=0; ih<nInHist;ih++) // for each kinematic range
    {
      TString hist_pr(Form("phPrp_%s",yieldHistNames[ih]));
      TString hist_npr(Form("phNPrp_%s",yieldHistNames[ih]));
      
      cout<<"histogram input name: "<< hist_pr<<"\t"<<hist_npr<<endl; 

      // prompt histos
      phRaw_pr_pp  = (TH1F*)fNoWeighFile_pp->Get(hist_pr);
      phCorr_pr_pp = (TH1F*)fYesWeighFile_pp->Get(hist_pr);
      phRaw_pr_aa  = (TH1F*)fNoWeighFile_aa->Get(hist_pr);
      phCorr_pr_aa = (TH1F*)fYesWeighFile_aa->Get(hist_pr);
      
      // non-prompt histos
      phRaw_npr_pp  = (TH1F*)fNoWeighFile_pp->Get(hist_npr);
      phCorr_npr_pp = (TH1F*)fYesWeighFile_pp->Get(hist_npr);
      phRaw_npr_aa  = (TH1F*)fNoWeighFile_aa->Get(hist_npr);
      phCorr_npr_aa = (TH1F*)fYesWeighFile_aa->Get(hist_npr);

      // efficiency histos
      phEff_pr_pp  = (TH1F*)fEffFile_pp->Get(hist_pr);
      phEff_npr_pp = (TH1F*)fEffFile_pp->Get(hist_npr);
      phEff_pr_aa  = (TH1F*)fEffFile_aa->Get(hist_pr);
      phEff_npr_aa = (TH1F*)fEffFile_aa->Get(hist_npr);

      double scaleFactor = ppLumi/nMbEvents;
  
      int numBins = 0;
      if(ih==0) numBins = nBinsNpart12;
      if(ih==4) numBins = nBinsNpart5;
      if(ih==1 || ih==2 || ih==3) numBins = nBinsNpart6;

      for(int ibin=1; ibin<=numBins; ibin++)
	{
	  double raa_pr=0, raaErr_pr=0, raa_npr=0, raaErr_npr=0;
	  double scale_cent = 1;
	  double scale_cent_np = 1;

	  if(ih==0)
	    {
	      scale_cent = 1/(adTaa12[ibin-1]*adDeltaCent12[ibin-1]);
	      scale_cent_np = 1/(adTaa6[ibin-1]*adDeltaCent6[ibin-1]);
	    }
	  if(ih==4)          scale_cent = 1/(adTaa5[ibin-1]*adDeltaCent5[ibin-1]);
	  if(ih!=0 && ih!=4) scale_cent = 1/(adTaa6[ibin-1]*adDeltaCent6[ibin-1]);
	  //prompt
	  double dRelErrRaw_pr_pp  = phRaw_pr_pp->GetBinError(ibin)/phRaw_pr_pp->GetBinContent(ibin);
	  double dRelErrRaw_pr_aa  = phRaw_pr_aa->GetBinError(ibin)/phRaw_pr_aa->GetBinContent(ibin);
	  double yieldRatio_pr     = phCorr_pr_aa->GetBinContent(ibin)/phCorr_pr_pp->GetBinContent(ibin);
	  if(weight==0) yieldRatio_pr = (phRaw_pr_aa->GetBinContent(ibin)/phRaw_pr_pp->GetBinContent(ibin))
			  * (phEff_pr_pp->GetBinContent(ibin)/phEff_pr_aa->GetBinContent(ibin));

	  raa_pr      =  yieldRatio_pr * scaleFactor * scale_cent;
	  raaErr_pr   = TMath::Sqrt(TMath::Power(dRelErrRaw_pr_pp,2)+TMath::Power(dRelErrRaw_pr_aa,2))*raa_pr;

	    //non-prompt
	    // get the rel uncert from the raw sample
	    double dRelErrRaw_npr_pp  = phRaw_npr_pp->GetBinError(ibin)/phRaw_npr_pp->GetBinContent(ibin);
	    double dRelErrRaw_npr_aa  = phRaw_npr_aa->GetBinError(ibin)/phRaw_npr_aa->GetBinContent(ibin);
	    double yieldRatio_npr     = phCorr_npr_aa->GetBinContent(ibin)/phCorr_npr_pp->GetBinContent(ibin);
	    if(weight==0) yieldRatio_npr = phRaw_npr_aa->GetBinContent(ibin)/phRaw_npr_pp->GetBinContent(ibin)
			    * (phEff_npr_pp->GetBinContent(ibin)/phEff_npr_aa->GetBinContent(ibin));

	    if(ih==0)raa_npr= yieldRatio_npr * scaleFactor * scale_cent_np;// the 1D nonPr has 6 bins only
	    else raa_npr    = yieldRatio_npr * scaleFactor * scale_cent;
	    raaErr_npr = TMath::Sqrt(TMath::Power(dRelErrRaw_npr_pp,2)+TMath::Power(dRelErrRaw_npr_aa,2))*raa_npr;

	   
	    // fill the corresponding array
	    switch(ih){
	    case 0:
	      prJpsi_cent[ibin-1]    = raa_pr;
	      prJpsiErr_cent[ibin-1] = raaErr_pr;
	      if(bDoDebug)
	      {
		 cout<<"weight_pr_aa = "<<phEff_pr_aa->GetBinContent(ibin)<<"\t weight_pr_pp = "<<phEff_pr_pp->GetBinContent(ibin)<<endl;
		 cout<<"yield_pr_aa "<<phCorr_pr_aa->GetBinContent(ibin)<<"\t yield_pr_pp "<<phCorr_pr_pp->GetBinContent(ibin)<<endl;
	
		cout<<"pr_aa= "<<phRaw_pr_aa->GetBinContent(ibin)/phEff_pr_aa->GetBinContent(ibin)<<"\t pr_pp= "<<phRaw_pr_pp->GetBinContent(ibin)/phEff_pr_pp->GetBinContent(ibin)<<endl;
		//	cout<<setprecision(2);
		cout<<"!!!!! raa = "<<prJpsi_cent[ibin-1]<<endl;
		
		// cout<<"Scale_Cent= "<<scale_cent<<endl;
	      }

	      
	      nonPrJpsi_cent[ibin-1]    = raa_npr;
	      nonPrJpsiErr_cent[ibin-1] = raaErr_npr;
	      break;
	    case 1:
	      prJpsi_pt6530y012_cent[ibin-1]        = raa_pr;
	      prJpsiErr_pt6530y012_cent[ibin-1]     = raaErr_pr;

	      nonPrJpsi_pt6530y012_cent[ibin-1]     = raa_npr;
	      nonPrJpsiErr_pt6530y012_cent[ibin-1]  = raaErr_npr;
	     
	      break;
	    case 2:
	      prJpsi_pt6530y1216_cent[ibin-1]       = raa_pr;
	      prJpsiErr_pt6530y1216_cent[ibin-1]    = raaErr_pr;

	      nonPrJpsi_pt6530y1216_cent[ibin-1]    = raa_npr;
	      nonPrJpsiErr_pt6530y1216_cent[ibin-1] = raaErr_npr;
	      break;
	    case 3:
	      prJpsi_pt6530y1624_cent[ibin-1]       = raa_pr;
	      prJpsiErr_cent[ibin-1]                = raaErr_pr;

	      nonPrJpsi_pt6530y1624_cent[ibin-1]    = raa_npr;
	      nonPrJpsiErr_pt6530y1624_cent[ibin-1] = raaErr_npr;
	      break;
	    case 4:
	      prJpsi_pt365y1624_cent[ibin-1]        = raa_pr;
	      prJpsiErr_pt365y1624_cent[ibin-1]     = raaErr_pr;

	      nonPrJpsi_pt365y1624_cent[ibin-1]     = raa_npr;
	      nonPrJpsiErr_pt365y1624_cent[ibin-1]  = raaErr_npr;
	      break;

	      }
	}//loop over mini centrlaity bins
	

    }//loop oever kinematic bins

  // ***** //Drawing
  // pr
  TGraphErrors *gPrJpsi     = new TGraphErrors(nBinsNpart12, binsNpart12, prJpsi_cent, binsNpart12Err, prJpsiErr_cent);
  TGraphErrors *gPrJpsiP    = new TGraphErrors(nBinsNpart12, binsNpart12, prJpsi_cent, binsNpart12Err, prJpsiErr_cent);
  TGraphErrors *gPrJpsiSyst = new TGraphErrors(nBinsNpart12, binsNpart12, prJpsi_cent, binsNpart12X, prJpsiErrSyst_cent);
 
 
  TGraphErrors *gPrJpsi_pt6530y012     = new TGraphErrors(nBinsNpart6, binsNpart6_shiftMinus, prJpsi_pt6530y012_cent, binsNpart6Err, prJpsiErr_pt6530y012_cent);
  TGraphErrors *gPrJpsiP_pt6530y012    = new TGraphErrors(nBinsNpart6, binsNpart6_shiftMinus, prJpsi_pt6530y012_cent, binsNpart6Err, prJpsiErr_pt6530y012_cent);
  TGraphErrors *gPrJpsiSyst_pt6530y012 = new TGraphErrors(nBinsNpart6, binsNpart6_shiftMinus, prJpsi_pt6530y012_cent, binsNpart6X, prJpsiErrSyst_pt6530y012_cent);


  TGraphErrors *gPrJpsi_pt6530y1216     = new TGraphErrors(nBinsNpart6, binsNpart6, prJpsi_pt6530y1216_cent, binsNpart6Err, prJpsiErr_pt6530y1216_cent);
  TGraphErrors *gPrJpsiP_pt6530y1216    = new TGraphErrors(nBinsNpart6, binsNpart6, prJpsi_pt6530y1216_cent, binsNpart6Err, prJpsiErr_pt6530y1216_cent);
  TGraphErrors *gPrJpsiSyst_pt6530y1216 = new TGraphErrors(nBinsNpart6, binsNpart6, prJpsi_pt6530y1216_cent, binsNpart6X, prJpsiErrSyst_pt6530y1216_cent);
 

  TGraphErrors *gPrJpsi_pt6530y1624     = new TGraphErrors(nBinsNpart6, binsNpart6_shiftPlus, prJpsi_pt6530y1624_cent, binsNpart6Err, prJpsiErr_pt6530y1624_cent);
  TGraphErrors *gPrJpsiP_pt6530y1624    = new TGraphErrors(nBinsNpart6, binsNpart6_shiftPlus, prJpsi_pt6530y1624_cent, binsNpart6Err, prJpsiErr_pt6530y1624_cent);
  TGraphErrors *gPrJpsiSyst_pt6530y1624 = new TGraphErrors(nBinsNpart6, binsNpart6_shiftPlus, prJpsi_pt6530y1624_cent, binsNpart6X, prJpsiErrSyst_pt6530y1624_cent);


  TGraphErrors *gPrJpsi_pt365y1624     = new TGraphErrors(nBinsNpart5, binsNpart5, prJpsi_pt365y1624_cent, binsNpart5Err, prJpsiErr_pt365y1624_cent);
  TGraphErrors *gPrJpsiP_pt365y1624    = new TGraphErrors(nBinsNpart5, binsNpart5, prJpsi_pt365y1624_cent, binsNpart5Err, prJpsiErr_pt365y1624_cent);
  TGraphErrors *gPrJpsiSyst_pt365y1624 = new TGraphErrors(nBinsNpart5, binsNpart5,    prJpsi_pt365y1624_cent, binsNpart5X, prJpsiErrSyst_pt365y1624_cent);

 
  // nonPr
  TGraphErrors *gNonPrJpsi     = new TGraphErrors(nBinsNpart6, binsNpart6, nonPrJpsi_cent, binsNpart6Err, nonPrJpsiErr_cent);
  TGraphErrors *gNonPrJpsiP    = new TGraphErrors(nBinsNpart6, binsNpart6, nonPrJpsi_cent, binsNpart6Err, nonPrJpsiErr_cent);
  TGraphErrors *gNonPrJpsiSyst = new TGraphErrors(nBinsNpart6, binsNpart6, nonPrJpsi_cent, binsNpart6X, nonPrJpsiErrSyst_cent);


  TGraphErrors *gNonPrJpsi_pt6530y012     = new TGraphErrors(nBinsNpart6, binsNpart6_shiftMinus,nonPrJpsi_pt6530y012_cent, binsNpart6Err, nonPrJpsiErr_pt6530y012_cent);
  TGraphErrors *gNonPrJpsiP_pt6530y012    = new TGraphErrors(nBinsNpart6, binsNpart6_shiftMinus,nonPrJpsi_pt6530y012_cent, binsNpart6Err, nonPrJpsiErr_pt6530y012_cent);
  TGraphErrors *gNonPrJpsiSyst_pt6530y012 = new TGraphErrors(nBinsNpart6, binsNpart6_shiftMinus, nonPrJpsi_pt6530y012_cent, binsNpart6X,   nonPrJpsiErrSyst_pt6530y012_cent);
  
  TGraphErrors *gNonPrJpsi_pt6530y1216     = new TGraphErrors(nBinsNpart6, binsNpart6, nonPrJpsi_pt6530y1216_cent, binsNpart6Err, nonPrJpsiErr_pt6530y1216_cent);
  TGraphErrors *gNonPrJpsiP_pt6530y1216    = new TGraphErrors(nBinsNpart6, binsNpart6, nonPrJpsi_pt6530y1216_cent, binsNpart6Err, nonPrJpsiErr_pt6530y1216_cent);
  TGraphErrors *gNonPrJpsiSyst_pt6530y1216 = new TGraphErrors(nBinsNpart6, binsNpart6, nonPrJpsi_pt6530y1216_cent, binsNpart6X,    nonPrJpsiErrSyst_pt6530y1216_cent);
 

  TGraphErrors *gNonPrJpsi_pt6530y1624     = new TGraphErrors(nBinsNpart6, binsNpart6_shiftPlus, nonPrJpsi_pt6530y1624_cent, binsNpart6Err, nonPrJpsiErr_pt6530y1624_cent);
  TGraphErrors *gNonPrJpsiP_pt6530y1624    = new TGraphErrors(nBinsNpart6, binsNpart6_shiftPlus, nonPrJpsi_pt6530y1624_cent, binsNpart6Err, nonPrJpsiErr_pt6530y1624_cent);
  TGraphErrors *gNonPrJpsiSyst_pt6530y1624 = new TGraphErrors(nBinsNpart6, binsNpart6_shiftPlus, nonPrJpsi_pt6530y1624_cent, binsNpart6X,    nonPrJpsiErrSyst_pt6530y1624_cent);


  TGraphErrors *gNonPrJpsi_pt365y1624     = new TGraphErrors(nBinsNpart5, binsNpart5, nonPrJpsi_pt365y1624_cent, binsNpart5Err, nonPrJpsiErr_pt365y1624_cent);
  TGraphErrors *gNonPrJpsiP_pt365y1624    = new TGraphErrors(nBinsNpart5, binsNpart5, nonPrJpsi_pt365y1624_cent, binsNpart5Err, nonPrJpsiErr_pt365y1624_cent);
  TGraphErrors *gNonPrJpsiSyst_pt365y1624 = new TGraphErrors(nBinsNpart5, binsNpart5, nonPrJpsi_pt365y1624_cent, binsNpart5X, nonPrJpsiErrSyst_pt365y1624_cent);


  //-------------------------------------------------------------------
  // **************** marker colors
  if(weight==1)
    {
      //prompt
      gPrJpsi->SetMarkerColor(kRed);
      gPrJpsi_pt6530y012->SetMarkerColor(kAzure+7);
      gPrJpsi_pt6530y1216->SetMarkerColor(kRed);
      gPrJpsi_pt6530y1624->SetMarkerColor(kGreen+2);   
      gPrJpsi_pt365y1624->SetMarkerColor(kViolet+2);
      
      // non-prompt
      gNonPrJpsi->SetMarkerColor(kOrange+2);
      gNonPrJpsi_pt6530y012->SetMarkerColor(kAzure+7);
      gNonPrJpsi_pt6530y1216->SetMarkerColor(kRed);
      gNonPrJpsi_pt6530y1624->SetMarkerColor(kGreen+2);  
      gNonPrJpsi_pt365y1624->SetMarkerColor(kViolet+2);
    }
  //--------- marker style  
 // pr
  gPrJpsi->SetMarkerStyle(21);
  gPrJpsiP->SetMarkerStyle(25);

  gPrJpsi_pt6530y012->SetMarkerStyle(20);
  gPrJpsi_pt6530y1216->SetMarkerStyle(21);
  gPrJpsi_pt6530y1624->SetMarkerStyle(33);
  gPrJpsi_pt365y1624->SetMarkerStyle(34);

  gPrJpsiP_pt6530y012->SetMarkerStyle(24);
  gPrJpsiP_pt6530y1216->SetMarkerStyle(25);
  gPrJpsiP_pt6530y1624->SetMarkerStyle(27);
  gPrJpsiP_pt365y1624->SetMarkerStyle(28);

  // non-pr
  gNonPrJpsi->SetMarkerStyle(29);
  gNonPrJpsiP->SetMarkerStyle(30);
  
  gNonPrJpsi_pt6530y012->SetMarkerStyle(20);
  gNonPrJpsi_pt6530y1216->SetMarkerStyle(21);
  gNonPrJpsi_pt6530y1624->SetMarkerStyle(33);
  gNonPrJpsi_pt365y1624->SetMarkerStyle(34);

  gNonPrJpsiP_pt6530y012->SetMarkerStyle(24);
  gNonPrJpsiP_pt6530y1216->SetMarkerStyle(25);
  gNonPrJpsiP_pt6530y1624->SetMarkerStyle(27);
  gNonPrJpsiP_pt365y1624->SetMarkerStyle(28);


  // ************** contour
   // pr
  gPrJpsiP->SetMarkerColor(kBlack);
  gPrJpsiP_pt6530y012->SetMarkerColor(kBlack);
  gPrJpsiP_pt6530y1216->SetMarkerColor(kBlack);
  gPrJpsiP_pt6530y1624->SetMarkerColor(kBlack);
  gPrJpsiP_pt365y1624->SetMarkerColor(kBlack);

  // nonPr
  gNonPrJpsiP->SetMarkerColor(kBlack);
  gNonPrJpsiP_pt6530y012->SetMarkerColor(kBlack);
  gNonPrJpsiP_pt6530y1216->SetMarkerColor(kBlack);
  gNonPrJpsiP_pt6530y1624->SetMarkerColor(kBlack);
  gNonPrJpsiP_pt365y1624->SetMarkerColor(kBlack);

  // marker size
  // pr
  gPrJpsi->SetMarkerSize(1.2);
  gPrJpsiP->SetMarkerSize(1.2);

  gPrJpsiP_pt6530y012->SetMarkerSize(1.2);
  gPrJpsiP_pt6530y1216->SetMarkerSize(1.2);
  gPrJpsiP_pt6530y1624->SetMarkerSize(2.0);
  gPrJpsiP_pt365y1624->SetMarkerSize(1.7);

  gPrJpsi_pt6530y012->SetMarkerSize(1.2);
  gPrJpsi_pt6530y1216->SetMarkerSize(1.2);
  gPrJpsi_pt6530y1624->SetMarkerSize(2.0);
  gPrJpsi_pt365y1624->SetMarkerSize(1.7);

  // nonPr
  gNonPrJpsi->SetMarkerSize(2.0);
  gNonPrJpsiP->SetMarkerSize(2.0);
  
  gNonPrJpsiP_pt6530y012->SetMarkerSize(1.2);
  gNonPrJpsiP_pt6530y1216->SetMarkerSize(1.2);
  gNonPrJpsiP_pt6530y1624->SetMarkerSize(2.0);
  gNonPrJpsiP_pt365y1624->SetMarkerSize(1.7);

  gNonPrJpsi_pt6530y012->SetMarkerSize(1.2);
  gNonPrJpsi_pt6530y1216->SetMarkerSize(1.2);
  gNonPrJpsi_pt6530y1624->SetMarkerSize(2.0);
  gNonPrJpsi_pt365y1624->SetMarkerSize(1.7);

  //stat boxes
  gPrJpsiSyst->SetFillColor(kRed-9);
 
  gPrJpsiSyst_pt6530y012->SetFillColor(kAzure-9);
  gPrJpsiSyst_pt6530y1216->SetFillColor(kRed-9);
  gPrJpsiSyst_pt6530y1624->SetFillColor(kGreen-9); 
  gPrJpsiSyst_pt365y1624->SetFillColor(kViolet-9);

  // non-pr
  gNonPrJpsiSyst->SetFillColor(kOrange-9);
  gNonPrJpsiSyst_pt6530y012->SetFillColor(kAzure-9);
  gNonPrJpsiSyst_pt6530y1216->SetFillColor(kRed-9);
  gNonPrJpsiSyst_pt6530y1624->SetFillColor(kGreen-9);
  gNonPrJpsiSyst_pt365y1624->SetFillColor(kViolet-9);

  //-------------------------------------------
  TF1 *f4 = new TF1("f4","1",0,400);
  f4->SetLineWidth(1);
  f4->GetXaxis()->SetTitle("N_{part}");
  f4->GetYaxis()->SetTitle("R_{AA}");
  f4->GetYaxis()->SetRangeUser(0.0,1.5);
  f4->GetXaxis()->CenterTitle(kTRUE);

  // just pp lumi here, 6%
  TBox *lumi = new TBox(0.0,0.94,10.0,1.06);
  lumi->SetFillColor(kGray+1);

  // pp lumi + pp stat+pp syst
  TBox *lumi_pr_y024_pt6530  = new TBox(0,0.93,10.0,1.07);
  TBox *lumi_pr_y012_pt6530  = new TBox(390.0,0.91,400.0,1.09);
  TBox *lumi_pr_y1216_pt6530 = new TBox(380.0,0.89,390.0,1.11);
  TBox *lumi_pr_y1624_pt6530 = new TBox(370.0,0.91,380.0,1.09);
  TBox *lumi_pr_y1624_pt6530_pty = new TBox(390.0,0.91,400.0,1.09);
  TBox *lumi_pr_y1624_pt365      = new TBox(380.0,0.92,390.0,1.08);

  lumi_pr_y024_pt6530->SetFillColor(kGray+1);
  lumi_pr_y012_pt6530->SetFillColor(kAzure-9);
  lumi_pr_y1216_pt6530->SetFillColor(kRed-9);
  lumi_pr_y1624_pt6530->SetFillColor(kGreen-9);
  lumi_pr_y1624_pt6530_pty->SetFillColor(kGreen-9);
  lumi_pr_y1624_pt365->SetFillColor(kViolet-9);

  TBox *lumi_npr_y024_pt6530  = new TBox(0,0.93,10.0,1.07);
  TBox *lumi_npr_y012_pt6530  = new TBox(390.0,0.91,400.0,1.09);
  TBox *lumi_npr_y1216_pt6530 = new TBox(380.0,0.89,390.0,1.11);
  TBox *lumi_npr_y1624_pt6530 = new TBox(370.0,0.91,380.0,1.09);
  TBox *lumi_npr_y1624_pt6530_pty = new TBox(390.0,0.91,400.0,1.09);
  TBox *lumi_npr_y1624_pt365      = new TBox(380.0,0.92,390.0,1.08);

  lumi_npr_y024_pt6530->SetFillColor(kGray+1);
  lumi_npr_y012_pt6530->SetFillColor(kAzure-9);
  lumi_npr_y1216_pt6530->SetFillColor(kRed-9);
  lumi_npr_y1624_pt6530->SetFillColor(kGreen-9);
  lumi_npr_y1624_pt6530_pty->SetFillColor(kGreen-9);
  lumi_npr_y1624_pt365->SetFillColor(kViolet-9);

  //---------------- general stuff
  TLatex *pre = new TLatex(25,1.4,"CMS Preliminary");
  pre->SetTextFont(42);
  pre->SetTextSize(0.05);
  TLatex *l1 = new TLatex(25,1.3,"PbPb #sqrt{s_{NN}} = 2.76 TeV");
  l1->SetTextFont(42);
  l1->SetTextSize(0.05);
  TLatex *ly = new TLatex(20.0,0.16875,"|y| < 2.4");
  TLatex *lpt = new TLatex(20.0,0.075,"6.5 < p_{T} < 30 GeV/c");
  


  // ##################################################### pr plots
  TCanvas *c1 = new TCanvas("c1","c1");
  // general stuff
  f4->Draw();// axis
  if(bAddLumi) 
    {
      lumi_pr_y024_pt6530->Draw();
      f4->Draw("same");
    }
  if(bAddLegend)
    {
      pre->Draw();
      l1->Draw();
      ly->Draw();
      lpt->Draw();
    }
  
  gPrJpsiSyst->Draw("2");
  gPrJpsi->Draw("P");
  gPrJpsiP->Draw("P");

  c1->SetTitle(" ");
  TLegend *leg10 = new TLegend(0.55,0.5,0.8,0.6);
  leg10->SetFillStyle(0);
  leg10->SetFillColor(0);
  leg10->SetBorderSize(0);
  leg10->SetMargin(0.2);
  leg10->SetTextSize(0.05);
  leg10->AddEntry(gPrJpsi,"Prompt J/#psi","");

  if(bAddLegend) leg10->Draw();

  if(bSavePlots)
    {
      c1->SaveAs(Form("%s/pdf/PrJpsi_vsCent_weight%d.pdf",outputDir,weight));
      c1->SaveAs(Form("%s/png/PrJpsi_vsCent_weight%d.pdf",outputDir,weight));
    }
  
  //------------------- (pt, y) dependence
  TCanvas *c11a = new TCanvas("c11a","c11a");
  f4->Draw();
  if(bAddLumi)
    {
      lumi_pr_y012_pt6530->Draw();
      lumi_pr_y1216_pt6530->Draw();
      lumi_pr_y1624_pt6530->Draw();
        f4->Draw("same");
    }


  gPrJpsiSyst_pt6530y012->Draw("2");
  gPrJpsiSyst_pt6530y1216->Draw("2");
  gPrJpsiSyst_pt6530y1624->Draw("2");

  gPrJpsi_pt6530y012->Draw("P");
  gPrJpsi_pt6530y1216->Draw("P");
  gPrJpsi_pt6530y1624->Draw("P");

  gPrJpsiP_pt6530y012->Draw("P");
  gPrJpsiP_pt6530y1216->Draw("P");
  gPrJpsiP_pt6530y1624->Draw("P");
 

  TLegend *leg11a = new TLegend(0.6,0.5,0.85,0.69);
  leg11a->SetFillStyle(0);
  leg11a->SetFillColor(0);
  leg11a->SetBorderSize(0);
  leg11a->SetMargin(0.2);
  leg11a->SetTextSize(0.04);

  leg11a->AddEntry(gPrJpsi_pt6530y012,"|y|<1.2","P");
  leg11a->AddEntry(gPrJpsi_pt6530y1216,"1.2<|y|<1.6","P");
  leg11a->AddEntry(gPrJpsi_pt6530y1624,"1.6<|y|<2.4","P");
 
  if(bAddLegend)
    {
      leg11a->Draw();
      
      c11a->Update();
      pre->DrawLatex(25,1.4,"CMS Preliminary");
      pre->DrawLatex(25,1.3,"PbPb #sqrt{s_{NN}} = 2.76 TeV");
      pre->DrawLatex(50,1.05,"Prompt J/#psi");
      pre->DrawLatex(25,0.09,"6.5<p_{T}<30 GeV/c");
    }
  gPad->RedrawAxis();

  TCanvas *c11b = new TCanvas("c11b","c11b");
  f4->Draw();
  if(bAddLumi)
    {
      lumi_pr_y1624_pt6530_pty->Draw();
      lumi_pr_y1624_pt365->Draw();
      f4->Draw("same");
    }
  if(!bOnlyLowPtCent)
    {
      gPrJpsiSyst_pt6530y1624->Draw("2");
      gPrJpsiSyst_pt365y1624->Draw("2");

      gPrJpsi_pt6530y1624->Draw("P");
      gPrJpsi_pt365y1624->Draw("P");

      gPrJpsiP_pt6530y1624->Draw("P");
      gPrJpsiP_pt365y1624->Draw("P");
    }
  else
    {
      gPrJpsiSyst_pt365y1624->Draw("2");
      gPrJpsi_pt365y1624->Draw("P");
      gPrJpsiP_pt365y1624->Draw("P");
    }

  TLegend *leg11b = new TLegend(0.55,0.55,0.8,0.68);
  leg11b->SetFillStyle(0);
  leg11b->SetFillColor(0);
  leg11b->SetBorderSize(0);
  leg11b->SetMargin(0.2);
  leg11b->SetTextSize(0.04);
  leg11b->AddEntry(gPrJpsi_pt6530y1624,"6.5<p_{T}<30 GeV/c","P");
  leg11b->AddEntry(gPrJpsi_pt365y1624,"3<p_{T}<6.5 GeV/c","P");
 
  if(bAddLegend)
    {
      leg11b->Draw();
    
      pre->DrawLatex(25,1.4,"CMS Preliminary");
      pre->DrawLatex(25,1.3,"PbPb #sqrt{s_{NN}} = 2.76 TeV");
      pre->DrawLatex(50,1.05,"Prompt J/#psi");
      pre->DrawLatex(25,0.09,"1.6<|y|<2.4");
    }
  gPad->RedrawAxis();

  if(bSavePlots)
    {
      c11a->SaveAs(Form("%s/pdf/PrJpsi_vsCent_yHighPt_weight%d_onlyLow%d.pdf",outputDir,weight,bOnlyLowPtCent));
      c11a->SaveAs(Form("%s/png/PrJpsi_vsCent_yHighPt_weight%d_onlyLow%d.png",outputDir,weight,bOnlyLowPtCent));

      c11b->SaveAs(Form("%s/pdf/PrJpsi_vsCent_yLowPt_weight%d_onlyLow%d.pdf",outputDir,weight,bOnlyLowPtCent));
      c11b->SaveAs(Form("%s/png/PrJpsi_vsCent_yLowPt_weight%d_onlyLow%d.png",outputDir,weight,bOnlyLowPtCent));
    }
//   // ############################################## non-pr
  TCanvas *c2 = new TCanvas("c2","c2");
  // general stuff
  f4->Draw();
  if(bAddLumi) lumi_npr_y024_pt6530->Draw();
  if(bAddLegend)
    {
      pre->Draw();
      l1->Draw();
      ly->Draw();
      lpt->Draw();
    }
  gNonPrJpsiSyst->Draw("2");
  gNonPrJpsi->Draw("P");
  gNonPrJpsiP->Draw("P");
      

  TLegend *leg20 = new TLegend(0.55,0.5,0.8,0.6);
  leg20->SetFillStyle(0);
  leg20->SetFillColor(0);
  leg20->SetBorderSize(0);
  leg20->SetMargin(0.2);
  leg20->SetTextSize(0.05);
  leg20->AddEntry(gNonPrJpsi,"Non-prompt J/#psi","");
  if(bAddLegend) leg20->Draw();

  gPad->RedrawAxis();

   if(bSavePlots)
    {
      c2->SaveAs(Form("%s/pdf/NonPrJpsi_vsCent_weight%d.pdf",outputDir,weight));
      c2->SaveAs(Form("%s/png/NonPrJpsi_vsCent_weight%d.png",outputDir,weight));
    }

  //------------------- (pt, y) dependence
  TCanvas *c21a = new TCanvas("c21a","c21a");
  f4->Draw();
  // lumi->Draw();
  if(bAddLumi)
    {
      lumi_npr_y012_pt6530->Draw();
      lumi_npr_y1216_pt6530->Draw();
      lumi_npr_y1624_pt6530->Draw();
      f4->Draw("same");
    }
  
  gNonPrJpsiSyst_pt6530y012->Draw("2");
  gNonPrJpsiSyst_pt6530y1216->Draw("2");
  gNonPrJpsiSyst_pt6530y1624->Draw("2");
  //  gNonPrJpsiSyst_pt365y1624->Draw("2");
  
  gNonPrJpsi_pt6530y012->Draw("P");
  gNonPrJpsi_pt6530y1216->Draw("P");
  gNonPrJpsi_pt6530y1624->Draw("P");
  // gNonPrJpsi_pt330y1624->Draw("P");
  // gNonPrJpsi_pt365y1624->Draw("P");

  gNonPrJpsiP_pt6530y012->Draw("P");
  gNonPrJpsiP_pt6530y1216->Draw("P");
  gNonPrJpsiP_pt6530y1624->Draw("P");
  // gNonPrJpsiP_pt330y1624->Draw("P");
  // gNonPrJpsiP_pt365y1624->Draw("P");

  TLegend *leg21a = new TLegend(0.6,0.5,0.85,0.69);
  leg21a->SetFillStyle(0);
  leg21a->SetFillColor(0);
  leg21a->SetBorderSize(0);
  leg21a->SetMargin(0.2);
  leg21a->SetTextSize(0.04);
  leg21a->AddEntry(gNonPrJpsi_pt6530y012,"|y|<1.2","P");
  leg21a->AddEntry(gNonPrJpsi_pt6530y1216,"1.2<|y|<1.6","P");
  leg21a->AddEntry(gNonPrJpsi_pt6530y1624,"1.6<|y|<2.4","P");
  if(bAddLegend)
    {
      leg21a->Draw();
      c21a->Update();
      pre->DrawLatex(25,1.4,"CMS Preliminary");
      pre->DrawLatex(25,1.3,"PbPb #sqrt{s_{NN}} = 2.76 TeV");
      pre->DrawLatex(50,1.05,"Non-prompt J/#psi");
      pre->DrawLatex(25,0.09,"6.5<p_{T}<30 GeV/c");
    }
  gPad->RedrawAxis();

  // pT dependence
  TCanvas *c21b = new TCanvas("c21b","c21b");
  f4->Draw();
  if(bAddLumi)
    {
      lumi_npr_y1624_pt6530_pty->Draw();
      lumi_npr_y1624_pt365->Draw();
      f4->Draw("same");
    }

  if(!bOnlyLowPtCent)
    {
      gNonPrJpsiSyst_pt6530y1624->Draw("2");
      gNonPrJpsiSyst_pt365y1624->Draw("2");
      
      gNonPrJpsi_pt6530y1624->Draw("P");
      gNonPrJpsi_pt365y1624->Draw("P");

      gNonPrJpsiP_pt6530y1624->Draw("P");
      gNonPrJpsiP_pt365y1624->Draw("P");
    }
  else
    {
      gNonPrJpsiSyst_pt365y1624->Draw("2");
      gNonPrJpsi_pt365y1624->Draw("P");
      gNonPrJpsiP_pt365y1624->Draw("P");
    }
  TLegend *leg21b = new TLegend(0.55,0.55,0.8,0.68);
  leg21b->SetFillStyle(0);
  leg21b->SetFillColor(0);
  leg21b->SetBorderSize(0);
  leg21b->SetMargin(0.2);
  leg21b->SetTextSize(0.04);
  leg21b->AddEntry(gNonPrJpsi_pt6530y1624,"6.5<p_{T}<30 GeV/c","P");
  leg21b->AddEntry(gNonPrJpsi_pt365y1624, "3<p_{T}<6.5 GeV/c","P");

  if(bAddLegend)
    {
      leg21b->Draw();
      
      pre->DrawLatex(25,1.4,"CMS Preliminary");
      pre->DrawLatex(25,1.3,"PbPb #sqrt{s_{NN}} = 2.76 TeV");
      pre->DrawLatex(50,1.05,"Non-prompt J/#psi");
      pre->DrawLatex(25,0.09,"1.6<|y|<2.4");
    }
  gPad->RedrawAxis();


  if(bSavePlots)
    {
      c21a->SaveAs(Form("%s/pdf/NonPrJpsi_vsCent_yHighPt_weight%d_onlyLow%d.pdf",outputDir,weight,bOnlyLowPtCent));
      c21a->SaveAs(Form("%s/png/NonPrJpsi_vsCent_yHighPt_weight%d_onlyLow%d.png",outputDir,weight,bOnlyLowPtCent));
      
      c21b->SaveAs(Form("%s/pdf/NonPrJpsi_vsCent_yLowPt_weight%d_onlyLow%d.pdf",outputDir,weight,bOnlyLowPtCent));
      c21b->SaveAs(Form("%s/png/NonPrJpsi_vsCent_yLowPt_weight%d_onlyLow%d.png",outputDir,weight,bOnlyLowPtCent));
    }
 
}