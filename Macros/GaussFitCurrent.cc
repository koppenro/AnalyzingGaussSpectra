/* ***************************************************************************** */
/* *                                                                           * */
/* *               Fit a gauss function to a charge distribution peak          * */
/* *                                                                           * */
/* ***************************************************************************** */
//Creates .pdf files and the .root file "Spectrum-Rate.root"
//Please change the settings in the config file "GaussFitCurrent.config"

#include <iostream>
#include <vector>
#include "string.h"
#include "TString.h"
#include "TFile.h" 
#include "TH1.h"
#include "TCanvas.h"
#include <iostream>
#include <fstream>
#include "TStyle.h"
#include <cmath>

using namespace std;

gROOT->Reset();

//global variables
Double_t mean=0;
//Int_t RunNr=0;
char FileName[20];
char Source[2];
char output[256];
int current;
double min, max;

Double_t occup=0;

void GaussFitCurrent(TString configfile = "GaussFitCurrent.config", Double_t limitlow = 0.6, Double_t limithigh = 1.2 , int clustersize = 1, int gmean = 0){
	
	char linecont[500], xtitle[256], ytitle[256], Source[256], outputtitle[256];
	int n;

	fstream fc,df;

	// read config file
	fc.open(configfile, ios::in);
	fc.getline(linecont,500);
	cout <<  linecont << endl;
//	fc.getline(linecont,500);
//	fc.getline(xtitle,256);
//	cout << "x: " << xtitle << endl;
//	fc.getline(ytitle,256);
//	cout << "y: " << ytitle << endl;
	fc.getline(linecont,500);
	fc.getline(outputtitle, 256);
	cout << "Output File title: " << outputtitle << endl;
	fc.getline(linecont, 500);
	fc.getline(Source, 256);
	cout << "Source " << Source << endl;
	// config file read
	
	
	std::ifstream FileTest(outputtitle);
	if(!FileTest) {
		//Write header in output document
		ofstream outputfile;
		outputfile.open(outputtitle, ios::out);
		outputfile << "#Output from GaussFitRate.C, Roehrenstrom und gemessene Peakposition fuer ";
		outputfile << Source;
		outputfile << "\n";
		outputfile << "#Current (mA)\tPeak (Vcal)\tLeft Bin\tRight Bin\tSpectrum\n";
		outputfile.close();
	}

	//User input
	cout << "Please type the root file (Spectrum_YYY.root)" << endl;
	cin >> FileName;
	cout << "Please type the current (mA)" << endl;
	cin >> current;

	//adjust variables
	Char_t *rootfile = Form("Spectrum_%s.root", FileName);
	cout << "Untersuchte Datei " << rootfile << endl;
	Char_t *histoname = Form("q_%s_C0_V0",Source);
	cout << "Histoname " << histoname << endl;
	Char_t *output = Form("%s",outputtitle);
	Char_t *source = Form("%s", Source);

//	//Create a canvas
//	TCanvas *c1 = new TCanvas("c1","Roentgenpeak",10,10,1000,500);
//	c1->SetLeftMargin(0.15);
//	gPad->SetLeftMargin(0.17);
//	gPad->SetBottomMargin(0.17);
//	c1->SetFillColor(0);
//	c1->SetBorderMode(0);
	
	//Show histogram data
	getTH1(rootfile,histoname, limitlow, limithigh,gmean, output, Source);
	
	//Do fit
	FitGauss(rootfile, histoname, limitlow, limithigh, gmean, output, Source);
}


//Definition of the function for importing and analysing the spectra
TH1 *getTH1(Char_t *rootfile,Char_t *histoname, 
Double_t limitlow, Double_t limithigh, int gmean, Char_t *output, char *Source){	
	
	TFile *fr = new TFile(rootfile);
	TH1 *histo, *hDummy;
	fr->Cd("Xray");

	gDirectory->GetObject(histoname, hDummy);
	if (hDummy != 0) {
		histo = (TH1*)hDummy->Clone("histo");
		//histo->SetDirectory(0);
		//histo->Draw("colz");
	}
	else { 
		cout << "Histogrammname ueberpruefen" << endl;
	}

	histo->GetYaxis()->SetTitleOffset(1);
	histo->GetYaxis()->SetTitleSize(0.05);
	histo->GetYaxis()->SetTitle("# Events");
	histo->GetXaxis()->SetTitleSize(0.05);
	histo->GetXaxis()->SetTitle("VcalDAC");

	//zooming the x-axis
	TAxis *axis = histo->GetXaxis();
	//   axis->SetRangeUser(50,220);  
	//   axis->SetRangeUser(50,350);  
	axis->SetRangeUser(0,350);  

	//extracting the histogram mean to adjust fit range
	Double_t histogram_mean =histo->GetMean();
	Double_t histogram_RMS =histo->GetRMS();
	Double_t histogram_maxbin = histo->GetMaximumBin();
	cout << "histogram  mean = " << histogram_mean << endl;
	cout << "histogram RMS = " << histogram_RMS << endl;
	cout << "histogram maximum bin = " << histogram_maxbin << endl;
	
	//histo->Draw();

	return (histo);
}

TH1 *FitGauss(Char_t *rootfile,Char_t *histoname, 
Double_t limitlow, Double_t limithigh, int gmean, Char_t *output, char *Source){

	//Create a canvas
	TCanvas *c2 = new TCanvas("c2","Roentgenpeak Gauss",10,10,1000,500);
	c2->SetLeftMargin(0.15);
	gPad->SetLeftMargin(0.17);
	gPad->SetBottomMargin(0.17);
	c2->SetFillColor(0);
	c2->SetBorderMode(0);
	
	
	TFile *fr = new TFile(rootfile);
	TH1 *histo, *hDummy;
	fr->Cd("Xray");

	gDirectory->GetObject(histoname, hDummy);
	if (hDummy != 0) {
		histo = (TH1*)hDummy->Clone("histo");
		//histo->SetDirectory(0);
		//histo->Draw("colz");
	}
	else { 
		cout << "Histogrammname ueberpruefen" << endl;
	}
	
	//User input
	double leftborder, rightborder;
	cout << "Linke Grenze (double): ";
	cin >> leftborder;
	cout << "Rechte Grenze (double): ";
	cin >> rightborder;
	
	TAxis *axis = histo->GetXaxis();
	//   axis->SetRangeUser(50,220);  
	//   axis->SetRangeUser(50,350);  
	axis->SetRangeUser(0,350);
	
	double sigmait[2], maxit[2];
	cout << "------------------------------------------------------------------------" << endl;
	cout << "Iteration 1" << endl;
	histo->Fit("gaus","L","",leftborder,rightborder);
	TF1 *Fit = histo->GetFunction("gaus");
	sigmait[0] = Fit->GetParameter(2);
	maxit[0] = Fit->GetParameter(1);
	gStyle->SetOptFit(111111); 
	Fit->SetLineWidth(1);
	Fit->SetLineStyle(1);
	
/*	//second Fit
	cout << "------------------------------------------------------------------------" << endl;
	cout << "Iteration 2" << endl;
	histo->Fit("gaus","L","",maxit[0]-sigmait[0],maxit[0]+sigmait[0]); 
	sigmait[1] = Fit->GetParameter(2);
	maxit[1] = Fit->GetParameter(1);
	//cout << abs(sigmait[1] - sigmait[0]) << " , " << 0.1*sigmait[0] << endl;
	gStyle->SetOptFit(111111); 
*/
	
	histo->Draw();
	int zufrieden;
	cout << "Zufrieden? Ja (1), Nein (0) ";
	cin >> zufrieden;
	
	if(zufrieden == 1) {
		//~ //Expected data for the source
		double lit = findsourcelit(Source);
		if (lit == -1){ 
			cout << "Source not identified" << endl;
			break;
		}
		mean = maxit[0];
		//mean = maxit[1];
		//Save measurement
		ofstream outputfile;
		outputfile.open(output, ios::out | ios::app);
		outputfile << current;
		outputfile << "\t";
		outputfile << mean;
		outputfile << "\t";
		outputfile << leftborder;
		outputfile << "\t";
		outputfile << rightborder;
		outputfile << "\t";
		outputfile << rootfile;
		outputfile << "\n";
		outputfile.close();
				
		//Save histogramm in root-File
		Char_t *savenamehisto = Form("%s-Gauss.root",FileName);
		TFile *savehisto = new TFile("Spectrum-Rate.root", "UPDATE");
		histo->Write(savenamehisto);
		savehisto->Close();
		
		//Save histogram as pdf
		Char_t *outputpdf = Form("Ergebnisse/Rate_%s.pdf", FileName);
		c2->SaveAs(outputpdf);
	}
	
	return(histo);
}


double findsourcelit(char *Source) {
	char Silber[] = "Ag";
	char Zink[] = "Zn";
	char Kupfer[] = "Cu";
	char Eisen[] = "Fe";
	char Molybdaen[] = "Mo";
	char Indium[] = "In";
	char Zinn[] = "Sn";
	char Neodym[] = "Nd";
	if(strcmp(Source, Silber) == 0) return(22162.99/3.6);
	if(strcmp(Source, Zink)== 0) return(8639.10/3.6);
	if(strcmp(Source, Kupfer)== 0) return(8048.11/3.6);
	if(strcmp(Source, Eisen)== 0) return(6403.13/3.6);
	if(strcmp(Source, Molybdaen)== 0) return(17479.10/3.6);
	if(strcmp(Source, Indium)== 0) return(24209.78/3.6);
	if(strcmp(Source, Zinn)== 0) return(25271.34/3.6);
	if(strcmp(Source, Neodym)== 0) return(37361.4.10/3.6);
	else return(-1);
}
