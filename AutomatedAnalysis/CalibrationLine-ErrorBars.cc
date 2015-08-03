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
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TGraphErrors.h"

using namespace std;

int findCurrentPlace(int, int*);
void readin(vector <double> *, vector <double> *, vector <double> *, vector <double> *, int *, const char *, bool);
void PlotGraph(vector <double> *, vector <double> *, vector <double> *, vector <double> *, const int, int *, TString, TString, TString, int);


int main( int argc, char *argv[] ){
	
	const int n = 9;		//number of different currents 
	vector <double> *data_x, *data_y, *data_xerr, *data_yerr;
	data_x = new vector <double>[n];
	data_y = new vector <double>[n];
	data_xerr = new vector <double>[n];
	data_yerr = new vector <double>[n];
	TString legtitle;
	//const char * outputtxt;
	//outputtxt = new char[500];
	TString outputtxt;
	outputtxt = "CalibrationLine-Slopes-Errors.txt";
	TString outputpdf;
	outputpdf = "results/CalibrationLine-Errors";
	//const char* outputtxt2;
	//outputtxt2 = new char[500];
	TString outputtxt2;
	outputtxt2 = "CalibrationLine-Slopes-MoReWeb-Errors.txt";
	TString outputpdf2;
	outputpdf2 = "results/CalibrationLine-MoReWeb-Errors";
	
	//DEFINITION OF OPTIONS
	bool module = false;
	long roc = 0;
	bool rocbool = false;
	bool legbool = false;
	bool MoReWeb = false;
	legtitle = "";
	
	for(int i = 1; i < argc; i++) {
		if(strstr(argv[i], "-m") != NULL) { module = true; }			//Set module, standard: false
		if(strstr(argv[i], "-roc") != NULL) { roc = int(strtol(argv[i+1], NULL, 0)); rocbool = true; }
		if(strstr(argv[i], "-l") != NULL) { legtitle = argv[i+1]; legbool = true; }
	}
	if (legtitle.Contains(":")) legtitle.ReplaceAll(":"," ");
	
	int * availableCurrents;
	availableCurrents = new int[n];
	//~ for(int i = 0; i < n; i++) {
		//~ availableCurrents[i] = 2+4*i;
	//~ }
	availableCurrents[0] = 2;
	availableCurrents[1] = 6;
	availableCurrents[2] = 10;
	availableCurrents[3] = 14;
	availableCurrents[4] = 18;
	availableCurrents[5] = 22;
	availableCurrents[6] = 26;
	availableCurrents[7] = 30;
	availableCurrents[8] = 20;
	//~ availableCurrents[3] = 20;
	//~ availableCurrents[4] = 30;
	
	// Calibration line with single gaussian fit
	const char *datafile;
	datafile = new char[500];
	int chipnr = 0;
	int nrchips = 1;
	if(module) {
		nrchips = 16;
	}
	if(rocbool) {
		chipnr = roc;
		nrchips = roc+1;
	}
	//const char * outputtxtakt;
	//outputtxtakt = new char[600];
	//const char * outputMoreWeb;
	//outputMoreWeb = new char[600];
	TString outputtxtakt;
	TString outputMoreWeb;
	TString outputpdfakt;
	TString outputpdfakt2;
	
	for(; chipnr < nrchips; chipnr++) {
		//cout << "###################################################################" << endl;
		
		legtitle.Append(Form(" - Auslesechip %i", chipnr));
		datafile = Form("results/C%i-Analysis-GaussFit.txt", chipnr);
		readin(data_x, data_y, data_xerr, data_yerr, availableCurrents, datafile, MoReWeb);
		//cout << data_x[1][0] << "\t" << data_xerr[1][0] << "\t" << data_y[1][0] << "\t" << data_yerr[1][0] << endl;
		int testdir = chdir("results/");
		if(testdir == 1) {}
		//outputtxtakt = Form("C%i-%s", chipnr, outputtxt);
		outputtxtakt.Form("C%i-CalibrationLine-Slopes-Errors.txt", chipnr);
		remove(outputtxtakt);
		testdir = chdir("../");
		outputpdf.Form("results/C%i-CalibrationLine-Errors", chipnr);
		outputpdf2.Form("results/C%i-CalibrationLine-MoReWeb-Errors", chipnr);
		//cout << "--------------------------------- " << outputMoreWeb << endl << endl;
		PlotGraph(data_x, data_y, data_xerr, data_yerr, n, availableCurrents, legtitle, outputtxtakt, outputpdf, chipnr);
		//cout << "---------------------------------nachPlotGraph " << outputMoreWeb << endl << endl;
		for(int i = 0; i < n; i++) {
			data_x[i].clear();
			data_y[i].clear();
			data_xerr[i].clear();
			data_yerr[i].clear();
		}
		//Calibration line with MoReWeb fit
		vector <double> *dataMW_x, *dataMW_y, *dataMW_xerr, *dataMW_yerr;
		dataMW_x = new vector <double>[n];
		dataMW_y = new vector <double>[n];
		dataMW_xerr = new vector <double>[n];
		dataMW_yerr = new vector <double>[n];
		MoReWeb = true;
		datafile = Form("results/C%i-Analysis-MoReWebFit.txt", chipnr);
		//datafile = "results/Analysis-MoReWebFit.txt";
		readin(dataMW_x, dataMW_y, dataMW_xerr, dataMW_yerr, availableCurrents, datafile, MoReWeb);
		testdir = chdir("results/");
		//outputMoreWeb = Form("C%i-%s", chipnr, outputtxt2);
		outputMoreWeb.Form("C%i-CalibrationLine-Slopes-MoReWeb-Errors.txt", chipnr);
		cout << "outputMoreWeb "<< outputMoreWeb << endl;
		remove(outputMoreWeb);
		testdir = chdir("../");
		legtitle.Append(" - MWeb");
		//cout << "--------------------------------- " << outputMoreWeb << endl << endl;
		//outputMoreWeb = Form("C%i-%s", chipnr, outputtxt2);
		cout << "outputMoreWeb "<< outputMoreWeb << endl;
		PlotGraph(dataMW_x, dataMW_y, dataMW_xerr, dataMW_yerr, n, availableCurrents, legtitle, outputMoreWeb, outputpdf2, chipnr);
		legtitle = TString(legtitle(0,legtitle.Length()-7));
		for(int i = 0; i < n; i++) {
			dataMW_x[i].clear();
			dataMW_y[i].clear();
		}
		if(chipnr < 10) {
			legtitle = TString(legtitle(0, legtitle.Length()-5));
		}
		else {
			legtitle = TString(legtitle(0, legtitle.Length()-6));
		}
		//~ 
		if(chipnr < 10) {
			outputpdf = TString(outputpdf(3, outputpdf.Length()));
			outputpdf2 = TString(outputpdf2(3, outputpdf2.Length()));
		}
		else {
			outputpdf = TString(outputpdf(4, outputpdf.Length()));
			outputpdf2 = TString(outputpdf2(4, outputpdf2.Length()));
		}
	}
	
	return 0;
}


//####################################################################################################################
//#Definition of the functions                                                                                       #
//####################################################################################################################

//Return errors on number of electrons for each source (data from NIST)
double erroronElectrons(TString actualSource) {
	double yerror = -1;
	//cout << actualSource << endl;
	if(strstr(actualSource, "Fe") != NULL) {yerror = 0.43/3.6;}
	else if(strstr(actualSource, "Cu") != NULL) {yerror = 0.45/3.6;}
	else if(strstr(actualSource, "Zn") != NULL) {yerror = 0.45/3.6;}
	else if(strstr(actualSource, "Mo") != NULL) {yerror = 0.55/3.6;}
	else if(strstr(actualSource, "Ag") != NULL) {yerror = 0.66/3.6;}
	else if(strstr(actualSource, "In") != NULL) {yerror = 0.69/3.6;}
	else if(strstr(actualSource, "Sn") != NULL) {yerror = 0.72/3.6;}
	else if(strstr(actualSource, "Nd") != NULL) {yerror = 0.10/3.6;}
	else { 
		cout << "Source not found, program will end" << endl;
		cout << yerror << endl;
		exit(0); 
	}
	return(yerror);
}

//Return characteristic integer for each used current to fill the data at the right position in an array
int findCurrentPlace( int current, int * availableCurrents ) {
	int i = 0;
	while( i < 8 ) {
		if(availableCurrents[i] == current) {break;}
		i++;
	}
	return(i);
}

//Read in data from results of FitGaussDistribution.cc and set errors on the data
void readin(vector <double> *data_x, vector <double> *data_y, vector <double> *data_xerr, vector <double> *data_yerr, int * availableCurrents, const char * txt, bool MoReWeb) {
	
	char linecont[500];
	fstream fc;
	fc.open(txt, ios::in);
	fc.getline(linecont, 500);
	fc.getline(linecont, 500);
	
	TString actualSource;
	//actualSource = new TString[1];
	char source[3], temp[5];
	while(fc >> source) {
		actualSource = TString(source);
		//cout << source << endl;
		int current;
		fc >> temp >> current;
		//cout << current << endl;
		double measurement, expectation; 
		fc >> measurement;
		double xerror, yerror;
		int i = findCurrentPlace( current, availableCurrents );
		//cout << "i " << i << " ";
		data_x[i].push_back(measurement);		// data_x[0] = 2mA, data_x[1] = 6mA, etc.
		//cout << data_x[i][0] << " ";
		fc >> xerror >> expectation;
		//data_xerr[i].push_back(xerror);
		if(MoReWeb == true) {
			data_xerr[i].push_back(1.2);
		}
		else {
			data_xerr[i].push_back(1.5);
		}
		//~ //cout << measurement << " " << expectation << endl;
		data_y[i].push_back(expectation);
		yerror = erroronElectrons(actualSource);
		data_yerr[i].push_back(yerror);
		fc.getline(linecont, 500);
	}
}

//Plot data with x and y error bars and perform a linear fit
void PlotGraph(vector <double> *data_x, vector <double> *data_y, vector <double> *data_xerr, vector <double> *data_yerr, const int n, int * availableCurrents, TString legtitle, TString outputtxt, TString outputpdf, int chipnr) {
	
	
	const char *xtitle, *ytitle;
	int *n_size;
	double *p1, *p0, *p0err, *p1err, *chisquare; 
	int *NDF;
	
	TString *legentry;
	TGraphErrors **graph;
	TMultiGraph *multi = new TMultiGraph(); 
	TF1 *p1fit;
	fstream fc,df;  
	
	TLegend *leg = new TLegend(0.15,0.47,0.52,0.89);
	//x: 0.53 - 0.9
	//y: 0.12 - 0.5
	leg->SetFillStyle(1001);
	leg->SetFillColor(0);
	leg->SetTextSize(0.04);
	
	// --------------------------------
	// init variables
	// --------------------------------
	n_size = new int[n];
	p0 = new double[n];
	p0err = new double[n];
	p1 = new double[n];
	p1err = new double[n];
	legentry = new TString[n];
	//graph = new TGraph*[n];
	graph = new TGraphErrors*[n];
	xtitle = new char[256];
	ytitle = new char[256];
	chisquare = new double[n];
	NDF = new int[n];
	

	if(legtitle != "") {
		//cout << legtitle << endl;
		leg->SetHeader(legtitle);
	}
	xtitle = "Q (Vcal)";
	ytitle = "n_{e^{-}}";
	
	
	TCanvas *c1 = new TCanvas("c1","PlotGraph",10,10,1500,1000); 
	//  TCanvas *c1 = new TCanvas("c1","PlotGraph",10,10,800,500);
	c1->SetFillColor(0);
	c1->SetBorderMode(0);
	c1->SetLeftMargin(0.12);
	c1->SetGrid();
	
	for(int i = 0; i < n; i++) {
		n_size[i] = data_x[i].size();
		//cout << "nsize i " << i << " " << n_size[i] << endl;
		if(n_size[i] != 0) {
			//graph[i] = new TGraph(n_size[i], &data_x[i][0], &data_y[i][0]);
			graph[i] = new TGraphErrors(n_size[i], &data_x[i][0], &data_y[i][0], &data_xerr[i][0], &data_yerr[i][0]);
		    if(i > 3) {
				graph[i]->SetLineColor(i+2);	//LineColor(4) is yellow and not good to see
				graph[i]->SetMarkerColor(i+2);
			}
			else {
				graph[i]->SetLineColor(i+1);
				graph[i]->SetMarkerColor(i+1);
			}
		    graph[i]->SetMarkerStyle(20+i);
		    graph[i]->SetMarkerSize(1.5);
		    //graph[i]->SetLineWidth(1);
		    //graph[i]->Draw("ALP");
		    legentry[i] = Form("%i mA", availableCurrents[i]);
		    
		    //Find maximum and minimum in data_x to set border of linear fit
		    double min = data_x[i][0];
		    double max = data_x[i][0];
		    for(int k = 1; k < n_size[i]; k++) {
				//cout << "k = " << k << " data_x[i][k] " << data_x[i][k];
				if(data_x[i][k] < min) {
					//cout << " data_x[i][k]min " << data_x[i][k];
					min = data_x[i][k];
				}
				//cout << " min = " << min << endl;
				if(data_x[i][k] > max) {
					max = data_x[i][k];
				}
			}
		    
		    p1fit = new TF1("p1fit","pol1",min,max);
		    p1fit->SetLineWidth(0.4);
		    p1fit->SetLineStyle(2);
		    if(i > 3) {
				p1fit->SetLineColor(i+2);	//LineColor(4) is yellow and not good to see
			}
			else {
				p1fit->SetLineColor(i+1);
			}
		    p1fit->SetParameter(1,49);
		    p1fit->SetParLimits(1,30,60);
		    graph[i]->Fit("p1fit","R");
		    //graph[i]->Fit("pol1","R");
		    p0[i]=p1fit->GetParameter(0);
		    p0err[i]=p1fit->GetParError(0);
		    p1[i]=p1fit->GetParameter(1);
		    p1err[i]=p1fit->GetParError(1);
		    chisquare[i]=p1fit->GetChisquare();
		    NDF[i]=p1fit->GetNDF();
		    
		    //Save data in txt file
			int testdir = chdir("results/");
			if(testdir == 0) {}
			
			//cout << outputtxt << endl << endl << endl << endl;
			std::ifstream FileTest(outputtxt);
			if(!FileTest) {
				//Write header in output document
				ofstream outputfile;
				outputfile.open(outputtxt, ios::out);
				outputfile << "//Linear fit (p0 +- p0err) + (p1 +- p1err)*x with parameters \n";
				outputfile << legtitle << "\n";
				outputfile << "Chipnr\tp0\tp0err\tp1\tp1err\tchisquare\tndf\tLegend\n";
				outputfile.close();
			}
			ofstream test;
			test.open(outputtxt, ios::out | ios::app);
			//~ test << "------------------------------------------------\n";
			//~ test << legentry[i];
			//~ test << "\n";
			//~ test << p0[i];
			//~ test << "\t+-\t";
			//~ test << p0err[i];
			//~ test << "\n";
			//~ test << p1[i];
			//~ test << "\t+-\t";
			//~ test << p1err[i];
			//~ test << "\n";
			test << chipnr << "\t" << p0[i] << "\t" << p0err[i] << "\t" << p1[i] << "\t" << p1err[i] << "\t" << chisquare[i] << "\t" << NDF[i] << "\t" << legentry[i] << "\n";
			test.close();
			testdir = chdir("../");
		    
		    cout << " adding graph[" << i <<"]" << endl; 
		    multi->Add(graph[i]);		    	    
			legentry[i].Append(Form("; p1 = %4.1f e^{-}/Vcal",p1[i])); 	//http://www.cplusplus.com/reference/cstdio/printf/
			leg->AddEntry(graph[i],legentry[i],"LP");
		    
		    multi->Draw("AP");
			multi->GetXaxis()->SetTitleOffset(0.75);
			multi->GetXaxis()->SetTitleSize(0.05);
			multi->GetXaxis()->SetTitle(xtitle);
			
			multi->GetYaxis()->SetTitleOffset(0.77);
			multi->GetYaxis()->SetTitleSize(0.05);
			multi->GetYaxis()->SetTitle(ytitle);
			multi->GetYaxis()->SetRangeUser(1800, 11900);
			//leg->Draw();
		}
	}
	
	c1->SaveAs(outputpdf.Append("-woLegend.pdf"));
	c1->SaveAs(outputpdf.ReplaceAll(".pdf",".png"));
	//c1->SaveAs(outputpdf.Form("%s-woLegend.png", outputpdf));
	
	leg->Draw();
	c1->SaveAs(outputpdf.ReplaceAll("-woLegend",""));
	c1->SaveAs(outputpdf.ReplaceAll(".png",".pdf"));
	c1->Clear();
	//c1->SaveAs(outputpdf.Form("%s.pdf", outputpdf));
	//c1->SaveAs(outputpdf.Form("%s-woLegend.png", outputpdf));
}

