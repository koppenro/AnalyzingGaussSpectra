#include <iostream>
#include <stdio.h>
#include <dirent.h> 
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <cmath>

#include "TCanvas.h"
#include "TH1.h"
#include "TFile.h"
#include "TF1.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TLegend.h"

using namespace std;

void HistoCalibrationSlopes();

int main () {
	
	HistoCalibrationSlopes();
	return (0);
}

void HistoCalibrationSlopes(){
	
	char *datadirectory, *measuredat, *fitdat;
	datadirectory = new char[500];
	measuredat = new char[600];
	fitdat = new char[600];
	char linecont[500];
	int measuredata[10];
	double fitparameter[4];
	
	// !!!!!!!!!!!!!!!!!!!!!!! TO COMPLETE WITH DIRECTORY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	datadirectory = "";
	int pixcol = 4;
	int pixrow = 42;
	
	int lowbin = 40;
	int maxbin = 55;
	int nbin = 2;
	TH1F *h1 = new TH1F("Gaussfunktion", "Calibration Line slopes for all measured ROCs at 30mA", (maxbin-lowbin)*nbin, lowbin, maxbin);
	fstream df, dc;
	measuredat = Form("%s/Slopes-CalibrationLines-SingleGauss.txt", datadirectory);
	//Find line in measuredat
	df.open(measuredat, ios::in);
	df.getline(linecont, 500);
	df.getline(linecont, 500);
	double actvalue;
	int current;
	while(df >> linecont) {
		df >> current;
		df >> actvalue;
		cout << actvalue << endl;
		df.getline(linecont, 500);
		if(current == 30) {
			h1->Fill(actvalue);
		}
	}
	
	TCanvas *c1 = new TCanvas("c1","PlotGraph",10,10,1800,1000); // for present.
	//  TCanvas *c1 = new TCanvas("c1","PlotGraph",10,10,800,500);
	c1->SetFillColor(0);
	c1->SetBorderMode(0);
	c1->SetLeftMargin(0.12);
	h1->SetFillColor(2);
	h1->SetFillStyle(3005);
	h1->SetLineColor(2);
	h1->Draw();
	c1->Update(); //this will force the generation of the "stats" box
	TPaveStats *ps1 = (TPaveStats*)h1->GetListOfFunctions()->FindObject("stats");
	ps1->SetX1NDC(0.73); ps1->SetX2NDC(0.98);
	ps1->SetY1NDC(0.72); ps1->SetY2NDC(0.91);
	ps1->SetTextColor(kRed);
	c1->Modified();
	//gStyle->SetOptStat(1001);
	//~ 
	//c1->SaveAs("HistoCalibrationSlopes-SingleGauss.pdf");
	//c1->SaveAs("HistoCalibrationSlopes-SingleGauss.png");
	
	TH1F *h2 = new TH1F("MoReWeb-Algorithmus", "Calibration Line slopes for all measured ROCs at 30mA", (maxbin-lowbin)*nbin, lowbin, maxbin);
	measuredat = Form("%s/Slopes-CalibrationLines-MoReWeb.txt", datadirectory);
	//Find line in measuredat
	dc.open(measuredat, ios::in);
	dc.getline(linecont, 500);
	dc.getline(linecont, 500);
	while(dc >> linecont) {
		dc >> current;
		dc >> actvalue;
		cout << actvalue << endl;
		dc.getline(linecont, 500);
		if(current == 30) {
			h2->Fill(actvalue);
		}
	}
	//h2->SetFillColor(4);
	//h2->SetFillStyle(3004);
	//~ TCanvas *c2 = new TCanvas("c2","PlotGraph",10,10,1800,1000); // for present.
	//~ //  TCanvas *c1 = new TCanvas("c1","PlotGraph",10,10,800,500);
	//~ c2->SetFillColor(0);
	//~ c2->SetBorderMode(0);
	//~ c2->SetLeftMargin(0.12);
	h1->GetYaxis()->SetRangeUser(0,37);
	h1->GetYaxis()->SetTitleSize(0.05);
	h1->GetYaxis()->SetTitleOffset(0.52);
	h1->GetYaxis()->SetTitle("Anzahl an Eintraegen");
	h1->GetXaxis()->SetTitleSize(0.05);
	h1->GetXaxis()->SetTitle("p_{1} (e^{-}/Vcal)");
	h1->GetXaxis()->SetTitleOffset(0.85);
	//h1->SetLineColor(4);
	h2->SetFillColor(4);
	h2->SetFillStyle(3004);
	h2->SetStats(0);
	gStyle->SetOptStat(1111);
	h2->SetStats(1);
	h2->SetTitle("");
	h1->SetTitle("");
	h2->GetYaxis()->SetTitleSize(0.05);
	h2->GetYaxis()->SetTitle("Anzahl an Eintraegen");
	h2->GetYaxis()->SetTitleOffset(0.52);
	h2->GetXaxis()->SetTitleSize(0.05);
	h2->GetXaxis()->SetTitle("p_{1} (e^{-}/Vcal)");
	h2->GetXaxis()->SetTitleOffset(0.8);
	//h2->Draw();
	h2->Draw("sames");
	c1->Update();
	TPaveStats *ps2 = (TPaveStats*)h2->GetListOfFunctions()->FindObject("stats");
	//~ ps2->SetX1NDC(0.75); ps2->SetX2NDC(0.98);
	//~ ps2->SetY1NDC(0.75); ps2->SetY2NDC(0.94);
	ps2->SetX1NDC(0.73); ps2->SetX2NDC(0.98);
	ps2->SetY1NDC(0.52); ps2->SetY2NDC(0.71);
	ps2->SetTextColor(kBlue);
	//c1->Modified();
	
	TLegend *leg;
	leg = new TLegend(0.15,0.75,0.7,0.9);
	//leg->SetHeader();
	leg->AddEntry(h1,"Steigungen mit einzelner Gaussanpassung bestimmt","f");
	leg->AddEntry(h2,"Steigungen mit MoReWeb Algorithmus bestimmt","f");
	leg->Draw();
	
	TFile *savehisto = new TFile("CalibrationLineHisto.root", "UPDATE");
	savehisto->Delete("GaussianFitSlopes");
	h1->Write("GaussianFitSlopes");
	savehisto->Delete("MoReWebFitSlopes");
	h2->Write("MoReWebFitSlopes");
	savehisto->Close();
	
	c1->SaveAs("HistoCalibrationSlopes.pdf");
	c1->SaveAs("HistoCalibrationSlopes.png");
	
	c1->Clear();
	h1->SetStats(1);
	gStyle->SetOptStat(1111);
	h1->Draw();
	c1->SaveAs("HistoCalibrationSlopes-SingleGaussian.pdf");
	c1->SaveAs("HistoCalibrationSlopes-SingleGaussian.png");
	
	c1->Clear();
	h2->SetStats(0);
	gStyle->SetOptStat(1111);
	h2->SetStats(1);
	h2->Draw();
	c1->SaveAs("HistoCalibrationSlopes-MoReWeb.pdf");
	c1->SaveAs("HistoCalibrationSlopes-MoReWeb.png");
	//c1->SaveAs("HistoCalibrationSlopes-MoReWeb.pdf");
	//c1->SaveAs("HistoCalibrationSlopes-MoReWeb.png");
  //~ 
}
