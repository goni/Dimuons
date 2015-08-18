/*
Macro to plot the Efficiency vs centrality, for all the analysis bins.(in the traditional way, yield  x correction, and the weighted way)

Input: root files produced by the readFitTable/makeHisto_raa.C (check there the input file names)

Output: the Raa vs rpaidity.

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

void makeRaa_y(   bool bSavePlots=1,
		  bool bDoDebug = 0, // adds some numbers, numerator, denominator, to help figure out if things are read properly
		  bool bAddLumi = 0, // add the lumi boxes at raa=1
		  bool bAddLegend = 1, 
		  bool bOnlyLowPtCent=0,
		  int weight = 1, //0=raw yields, will be corrected on the fly, in the traditional way, 1=corrected yields
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


  // name of input files
  const char* yieldHistFile_yesWeight[2] = {"histsRaaYields_20150817_PbPb_raa_Lxyz_weightedEff_Lxyz_pTtune_PRMC.root",
  					    "histsRaaYields_20150817_pp_Lxyz_weightedEff_Lxyz_finerpT_PRMC.root"};
  
  const char* yieldHistFile_noWeight[2]  = {"histsRaaYields_20150817_PbPb_raa_Lxyz_noWeight_Lxyz_pTtune_PRMC.root",
					    "histsRaaYields_20150817_pp_Lxyz_noWeight_Lxyz_finerpT_PRMC.root"};
  const char* effHistFile[2]             = {"histEff_pbpb_tradEff_nov12.root",
					    "histEff_pp_tradEff_nov12.root"};
  const int nInHist = 3;
  const char* yieldHistNames[nInHist] = {"y", "y_mb", "mb"};

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
      double scale_cent = 1/(adTaaMB[0]*adDeltaCentMB[0]);
    
      int numBins = 0;
      if(ih==0) numBins = nBinsY;
      if(ih==1) numBins = nBinsY3;
      if(ih==3) numBins = nBinsMB;

      cout << "Number of bins: " << numBins<<endl;

      for(int ibin=1; ibin<=numBins; ibin++)
	{
	  double raa_pr=0, raaErr_pr=0, raa_npr=0, raaErr_npr=0;

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

	    raa_npr    = yieldRatio_npr * scaleFactor * scale_cent;
	    raaErr_npr = TMath::Sqrt(TMath::Power(dRelErrRaw_npr_pp,2)+TMath::Power(dRelErrRaw_npr_aa,2))*raa_npr;
	   
	    // fill the corresponding array
	    switch(ih){
	    case 0:
	      prJpsi_y[ibin-1]    = raa_pr;
	      prJpsiErr_y[ibin-1] = raaErr_pr;
	    	      
	      nonPrJpsi_y[ibin-1]    = raa_npr;
	      nonPrJpsiErr_y[ibin-1] = raaErr_npr;

	      if(bDoDebug)
	      {
		// cout<<"weight_pr_aa = "<<phEff_pr_aa->GetBinContent(ibin)<<"\t weight_pr_pp = "<<phEff_pr_pp->GetBinContent(ibin)<<endl;
		cout<<"yield_pr_aa "<<phCorr_pr_aa->GetBinContent(ibin)<<"\t yield_pr_pp "<<phCorr_pr_pp->GetBinContent(ibin)<<endl;
		cout<<"yield_npr_aa "<<phCorr_npr_aa->GetBinContent(ibin)<<"\t yield_npr_pp "<<phCorr_npr_pp->GetBinContent(ibin)<<endl;
		//	cout<<setprecision(2);
		cout<<"!!!!! raa_pr = "<<raa_pr<<"\t raa_npr= "<<raa_npr<<endl;
		
	      }

	      break;
	 
	    case 1:
	      prJpsi_y_y[ibin-1]        = raa_pr;
	      prJpsiErr_y_y[ibin-1]     = raaErr_pr;

	      nonPrJpsi_y_y[ibin-1]     = raa_npr;
	      nonPrJpsiErr_y_y[ibin-1]  = raaErr_npr;

	      if(bDoDebug)
	      {
	      	// cout<<"weight_pr_aa = "<<phEff_pr_aa->GetBinContent(ibin)<<"\t weight_pr_pp = "<<phEff_pr_pp->GetBinContent(ibin)<<endl;
	      	//cout<<"yield_pr_aa "<<phCorr_pr_aa->GetBinContent(ibin)<<"\t yield_pr_pp "<<phCorr_pr_pp->GetBinContent(ibin)<<endl;
	      	cout<<"yield_npr_aa "<<phCorr_npr_aa->GetBinContent(ibin)<<"\t yield_pr_pp "<<phCorr_npr_pp->GetBinContent(ibin)<<endl;
	
	      	//cout<<"pr_aa= "<<phRaw_pr_aa->GetBinContent(ibin)/phEff_pr_aa->GetBinContent(ibin)<<"\t pr_pp= "<<phRaw_pr_pp->GetBinContent(ibin)/phEff_pr_pp->GetBinContent(ibin)<<endl;
	      	cout<<setprecision(2);
	      	cout<<"!!!!! raa_pr = "<<raa_pr<<"\t raa_npr= "<<raa_npr<<endl;
		
	      }

	      break;

	    case 2:
	      // mb
	      prJpsi_mb[0]     = raa_pr;
	      prJpsiErr_mb[0]  = raaErr_pr;
	      
	      nonPrJpsi_mb[0]  = raa_npr;
	      nonPrJpsi_mb[0]  = raaErr_npr;
	      break;
	    }
	}//loop over mini centrlaity bins
	

    }//loop oever kinematic bins

  // ***** //Drawing
  // pr
  TGraphErrors *gPrJpsi     = new TGraphErrors(nBinsY, binsY, prJpsi_y, binsYErr, prJpsiErr_y);
  TGraphErrors *gPrJpsiP    = new TGraphErrors(nBinsY, binsY, prJpsi_y, binsYErr, prJpsiErr_y);
  TGraphErrors *gPrJpsiSyst = new TGraphErrors(nBinsY, binsY, prJpsi_y, binsYX, prJpsiErrSyst_y);

  TGraphErrors *gPrJpsi_mb     = new TGraphErrors(nBinsMB, binsYMB, prJpsi_mb, binsYMBErr, prJpsiErr_mb);
  TGraphErrors *gPrJpsiP_mb    = new TGraphErrors(nBinsMB, binsYMB, prJpsi_mb, binsYMBErr, prJpsiErr_mb);
  TGraphErrors *gPrJpsiSyst_mb = new TGraphErrors(nBinsMB, binsYMB, prJpsi_mb, binsYMBX  , prJpsiErrSyst_mb);
 
  TGraphErrors *gPrJpsi_y_y     = new TGraphErrors(nBinsY3, binsY3, prJpsi_y_y, binsY3Err, prJpsiErr_y_y);
  TGraphErrors *gPrJpsiP_y_y    = new TGraphErrors(nBinsY3, binsY3, prJpsi_y_y, binsY3Err, prJpsiErr_y_y);
  TGraphErrors *gPrJpsiSyst_y_y = new TGraphErrors(nBinsY3, binsY3, prJpsi_y_y, binsY3X,   prJpsiErrSyst_y_y);
 
  // nonPr   
  TGraphErrors *gNonPrJpsi     = new TGraphErrors(nBinsY, binsY, nonPrJpsi_y, binsYErr, nonPrJpsiErr_y);
  TGraphErrors *gNonPrJpsiP    = new TGraphErrors(nBinsY, binsY, nonPrJpsi_y, binsYErr, nonPrJpsiErr_y);
  TGraphErrors *gNonPrJpsiSyst = new TGraphErrors(nBinsY, binsY, nonPrJpsi_y, binsYX, nonPrJpsiErrSyst_y);

  TGraphErrors *gNonPrJpsi_mb     = new TGraphErrors(nBinsMB, binsYMB, nonPrJpsi_mb, binsYMBErr, nonPrJpsiErr_mb);
  TGraphErrors *gNonPrJpsiP_mb    = new TGraphErrors(nBinsMB, binsYMB, nonPrJpsi_mb, binsYMBErr, nonPrJpsiErr_mb);
  TGraphErrors *gNonPrJpsiSyst_mb = new TGraphErrors(nBinsMB, binsYMB, nonPrJpsi_mb, binsYMBX, nonPrJpsiErrSyst_mb);


  TGraphErrors *gNonPrJpsi_y_y     = new TGraphErrors(nBinsY3, binsY3, nonPrJpsi_y_y, binsY3Err, nonPrJpsiErr_y_y);
  TGraphErrors *gNonPrJpsiP_y_y    = new TGraphErrors(nBinsY3, binsY3, nonPrJpsi_y_y, binsY3Err, nonPrJpsiErr_y_y);
  TGraphErrors *gNonPrJpsiSyst_y_y = new TGraphErrors(nBinsY3, binsY3, nonPrJpsi_y_y, binsY3X,   nonPrJpsiErrSyst_y_y);

  //-------------------------------------------------------------------
  // **************** marker colors
  if(weight==1)
    {
      //prompt
      gPrJpsi->SetMarkerColor(kRed);
      // non-prompt
      gNonPrJpsi->SetMarkerColor(kOrange+2);
    }

  //mnbias colors
  gPrJpsi_mb->SetMarkerColor(kCyan+2);
  gNonPrJpsi_mb->SetMarkerColor(kCyan+2);

  gPrJpsi_y_y->SetMarkerColor(kBlue-4);
  gNonPrJpsi_y_y->SetMarkerColor(kBlue-4);

  //--------- marker style  
 // pr
  gPrJpsi->SetMarkerStyle(21);
  gPrJpsiP->SetMarkerStyle(25);

  gPrJpsi_y_y->SetMarkerStyle(34);
  gPrJpsiP_y_y->SetMarkerStyle(28);

  // non-pr
  gNonPrJpsi->SetMarkerStyle(29);
  gNonPrJpsiP->SetMarkerStyle(30);
  
  gNonPrJpsi_y_y->SetMarkerStyle(34);
  gNonPrJpsiP_y_y->SetMarkerStyle(28);

  //mb
  gPrJpsi_mb->SetMarkerStyle(28);
  gNonPrJpsi_mb->SetMarkerStyle(28);
  
  // ************** contour
   // pr
  gPrJpsiP->SetMarkerColor(kBlack);
  gPrJpsiP_mb->SetMarkerColor(kBlack);

  // nonPr
  gNonPrJpsiP->SetMarkerColor(kBlack);
  gNonPrJpsiP_mb->SetMarkerColor(kBlack);


  // marker size
  // pr
  gPrJpsi->SetMarkerSize(1.2);
  gPrJpsiP->SetMarkerSize(1.2);

  gPrJpsiP_y_y->SetMarkerSize(1.7);
  gPrJpsi_y_y->SetMarkerSize(1.7);

  gPrJpsi_mb->SetMarkerSize(1.5);
  gNonPrJpsi_mb->SetMarkerSize(1.5);

  // nonPr
  gNonPrJpsi->SetMarkerSize(1.7);
  gNonPrJpsiP->SetMarkerSize(1.7);
  
  gNonPrJpsiP_y_y->SetMarkerSize(1.7);
  gNonPrJpsi_y_y->SetMarkerSize(1.7);

  //stat boxes
  gPrJpsiSyst->SetFillColor(kRed-9);
  gPrJpsiSyst_y_y->SetFillColor(kViolet-9);

  // non-pr
  gNonPrJpsiSyst->SetFillColor(kOrange-9);
  gNonPrJpsiSyst_y_y->SetFillColor(kViolet-9);

  //-------------------------------------------
  TF1 *f4 = new TF1("f4","1",0,2.4);
  f4->SetLineWidth(1);
  f4->GetXaxis()->SetTitle("|y|");
  f4->GetYaxis()->SetTitle("R_{AA}");
  f4->GetYaxis()->SetRangeUser(0.0,1.5);
  f4->GetXaxis()->CenterTitle(kTRUE);

 // sqrt(sig_lumi(6%)*sig_lumi(6%)+sig_taa(5.7%)*sig_taa(5.7%)) = 0.083
  TBox *lumi = new TBox(0.0,0.917,1.,1.083);
  lumi->SetFillColor(kGray+1);


  //---------------- general stuff
  TLatex *pre = new TLatex(0.2,1.375,"CMS Preliminary");//0.78125
  pre->SetTextFont(42);
  pre->SetTextSize(0.05);
  TLatex *l1 = new TLatex(0.2,1.3,"PbPb #sqrt{s_{NN}} = 2.76 TeV");
  l1->SetTextFont(42);
  l1->SetTextSize(0.05);

  TLatex *lpt = new TLatex(0.12,0.075,"6.5 < p_{T} < 30 GeV/c");
  lpt->SetTextFont(42);
  lpt->SetTextSize(0.05);
  TLatex *lcent = new TLatex(0.12,0.16875,"Cent. 0-100%");
  lcent->SetTextFont(42);
  lcent->SetTextSize(0.05);


  // ##################################################### pr plots
  TCanvas *c1 = new TCanvas("c1","c1");
  // general stuff
  f4->Draw();// axis
  if(bAddLumi) 
    {
      lumi->Draw();
      f4->Draw("same");
    }
  if(bAddLegend)
    {
      pre->Draw();
      l1->Draw();
      lcent->Draw();
      lpt->Draw();
      pre->DrawLatex(0.2,1.05,"Prompt J/#psi");
    }
  
  gPrJpsiSyst->Draw("2");
  gPrJpsi->Draw("P");
  gPrJpsiP->Draw("P");

  if(bSavePlots)
    {
      c1->SaveAs(Form("%s/pdf/PrJpsi_vsY_weight%d.pdf",outputDir,weight));
      c1->SaveAs(Form("%s/png/PrJpsi_vsY_weight%d.pdf",outputDir,weight));
    }
  
  //-------------------minbias dependence
  TCanvas *c11b = new TCanvas("c11b","c11b");
  f4->Draw();
  if(bAddLumi)
    {
      lumi->Draw();
      f4->Draw("same");
    }
 
  gPrJpsiSyst_mb->Draw("2");
  gPrJpsi_mb->Draw("P");
 
  gPrJpsiSyst_y_y->Draw("2");
  gPrJpsi_y_y->Draw("P");
 
  if(bAddLegend)
    {
      pre->Draw();
      l1->Draw();
      lcent->Draw();
      lpt->Draw();

      pre->DrawLatex(0.2,1.05,"Prompt J/#psi");
    }
  gPad->RedrawAxis();

  if(bSavePlots)
    {
      c11b->SaveAs(Form("%s/pdf/PrJpsi_vsY_mb_weight%d_onlyLow%d.pdf",outputDir,weight,bOnlyLowPtCent));
      c11b->SaveAs(Form("%s/png/PrJpsi_vsY_mb_weight%d_onlyLow%d.png",outputDir,weight,bOnlyLowPtCent));
    }

  
 //   // ############################################## non-pr 
 //   // ############################################## non-pr
  TCanvas *c2 = new TCanvas("c2","c2");
  // general stuff
  f4->Draw();// axis
  if(bAddLumi) 
    {
      lumi->Draw();
      f4->Draw("same");
    }
  if(bAddLegend)
    {
      pre->DrawLatex(0.2,1.4,"CMS Preliminary");
      pre->DrawLatex(0.2,1.05,"Non-prompt J/#psi");
      l1->Draw();
      lcent->Draw();
      lpt->Draw();
    }
  
  gNonPrJpsiSyst->Draw("2");
  gNonPrJpsi->Draw("P");
  gNonPrJpsiP->Draw("P");

  if(bSavePlots)
    {
      c2->SaveAs(Form("%s/pdf/nonPrJpsi_vsY_weight%d.pdf",outputDir,weight));
      c2->SaveAs(Form("%s/png/nonPrJpsi_vsY_weight%d.pdf",outputDir,weight));
    }
  
  //-------------------minbias dependence
  TCanvas *c22b = new TCanvas("c22b","c22b");
  f4->Draw();
  if(bAddLumi)
    {
      lumi->Draw();
      f4->Draw("same");
    }
 
  gNonPrJpsiSyst_mb->Draw("2");
  gNonPrJpsi_mb->Draw("P");
 
  gNonPrJpsiSyst_y_y->Draw("2");
  gNonPrJpsi_y_y->Draw("P");

  if(bAddLegend)
    {
      pre->DrawLatex(0.2,1.4,"CMS Preliminary");
      l1->Draw();
      lcent->Draw();
      lpt->Draw();
      pre->DrawLatex(0.2,1.05,"Non-prompt J/#psi");
    }
  gPad->RedrawAxis();

  if(bSavePlots)
    {
      c22b->SaveAs(Form("%s/pdf/nonPrJpsi_vsY_mb_weight%d_onlyLow%d.pdf",outputDir,weight,bOnlyLowPtCent));
      c22b->SaveAs(Form("%s/png/nonPrJpsi_vsY_mb_weight%d_onlyLow%d.png",outputDir,weight,bOnlyLowPtCent));
    }

  
}