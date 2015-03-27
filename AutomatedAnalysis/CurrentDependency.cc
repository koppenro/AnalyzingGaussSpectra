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

using namespace std;

int findCurrentPlace(int, int*);
void readin(vector <double> *, vector <double> *, int *);
void PlotGraph(vector <double> *, vector <double> *, const int, int *, TString, const char *);


int main( int argc, char *argv[] ){
	
	//Possible ways to execute: ./CalibrationLine or ./CalibrationLine $legendtitle (e.g. ./CalibrationLine M0307:p20 -> spaces in the title with ":" ) 
	//First argument: Legend Title
	const int n = 8;		//number of different targets 
	vector <double> *data_x, *data_y;
	data_x = new vector <double>[n];
	data_y = new vector <double>[n];
	TString legtitle;
	const char * outputtxt;
	outputtxt = new char[256];
	outputtxt = "CurrentDependency-Slopes.txt";
	
	
	TString * availableTargets;
	availableTargets = new TString[n];
	availableTargets[0] = "Fe";
	availableTargets[1] = "Cu";
	availableTargets[2] = "Zn";
	availableTargets[3] = "Mo";
	availableTargets[4] = "Ag";
	availableTargets[5] = "In";
	availableTargets[6] = "Sn";
	availableTargets[7] = "Nd";
	
	
	if(argc == 2) {
		legtitle.Form("%s", argv[1]);
		if (legtitle.Contains(":")) legtitle.ReplaceAll(":"," ");
		//cout << legtitle << endl;
	}
	else {
		legtitle = "";
	}
	
	readin(data_x, data_y, availableCurrents);
	int testdir = chdir("results/");
	if(testdir == 1) {}
	remove(outputtxt);
	testdir = chdir("../");
	PlotGraph(data_x, data_y, n, availableCurrents, legtitle, outputtxt);
	
	return 0;
}


//####################################################################################################################
//#Definition of the functions                                                                                       #
//####################################################################################################################

int findCurrentPlace( int current, int * availableCurrents ) {
	int i = 0;
	while( i < 8 ) {
		if(availableCurrents[i] == current) {break;}
		i++;
	}
	return(i);
}

void readin(vector <double> *data_x, vector <double> *data_y, int * availableCurrents) {
	
	char linecont[500];
	fstream fc;
	fc.open("results/Analysis-GaussFit.txt", ios::in);
	fc.getline(linecont, 500);
	fc.getline(linecont, 500);
	
	char source[3];
	while(fc >> source) {
		//cout << source << endl;
		int current;
		fc >> current;
		//cout << current << endl;
		double measurement, expectation; 
		fc >> measurement;
		int i = findCurrentPlace( current, availableCurrents );
		//cout << "i " << i << " ";
		data_x[i].push_back(measurement);		// data_x[0] = 2mA, data_x[1] = 6mA, etc.
		//cout << data_x[i][0] << " ";
		fc >> measurement >> expectation;
		//cout << measurement << " " << expectation << endl;
		data_y[i].push_back(expectation);
		fc.getline(linecont, 500);
	}
}

void PlotGraph(vector <double> *data_x, vector <double> *data_y, const int n, int * availableCurrents, TString legtitle, const char * outputtxt) {
	
	const char *xtitle, *ytitle;
	int *n_size;
	double *p1, *p0, *p0err, *p1err; 
	
	TString *legentry;
	TGraph **graph;
	TMultiGraph *multi = new TMultiGraph(); 
	TF1 *p1fit;
	fstream fc,df;  
	
	TLegend *leg = new TLegend(0.53,0.12,0.9,0.5);
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
	graph = new TGraph*[n];
	xtitle = new char[256];
	ytitle = new char[256];
	

	if(legtitle != "") {
		//cout << legtitle << endl;
		leg->SetHeader(legtitle);
	}
	xtitle = "signal ( Vcal DAC )";
	ytitle = "charge ( e^{-} )";
	
	
	TCanvas *c1 = new TCanvas("c1","PlotGraph",10,10,1500,1000); 
	//  TCanvas *c1 = new TCanvas("c1","PlotGraph",10,10,800,500);
	c1->SetFillColor(0);
	c1->SetBorderMode(0);
	c1->SetLeftMargin(0.12);
	
	for(int i = 0; i < n; i++) {
		n_size[i] = data_x[i].size();
		//cout << "nsize i " << i << " " << n_size[i] << endl;
		if(n_size[i] != 0) {
			graph[i] = new TGraph(n_size[i], &data_x[i][0], &data_y[i][0]);
		    if(i > 3) {
				graph[i]->SetLineColor(i+2);	//LineColor(4) is yellow and not good to see
				graph[i]->SetMarkerColor(i+2);
			}
			else {
				graph[i]->SetLineColor(i+1);
				graph[i]->SetMarkerColor(i+1);
			}
		    graph[i]->SetMarkerStyle(20+i);
		    graph[i]->SetMarkerSize(0.75);
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
		     
		    graph[i]->Fit("p1fit","R");
		    //graph[i]->Fit("pol1","R");
		    p0[i]=p1fit->GetParameter(0);
		    p0err[i]=p1fit->GetParError(0);
		    p1[i]=p1fit->GetParameter(1);
		    p1err[i]=p1fit->GetParError(1);
		    
		    //Save data in txt file
			int testdir = chdir("results/");
			if(testdir == 0) {}
			
			std::ifstream FileTest(outputtxt);
			if(!FileTest) {
				//Write header in output document
				ofstream outputfile;
				outputfile.open(outputtxt, ios::out);
				outputfile << "//Linear fit (p0 +- p0err) + (p1 +- p1err)*x with parameters \n";
				outputfile << legtitle << "\n";
				outputfile.close();
			}
			ofstream test;
			test.open(outputtxt, ios::out | ios::app);
			test << "------------------------------------------------\n";
			test << legentry[i];
			test << "\n";
			test << p0[i];
			test << "\t+-\t";
			test << p0err[i];
			test << "\n";
			test << p1[i];
			test << "\t+-\t";
			test << p1err[i];
			test << "\n";
			test.close();
			testdir = chdir("../");
		    
		    cout << " adding graph[" << i <<"]" << endl; 
		    multi->Add(graph[i]);		    	    
			legentry[i].Append(Form("; p1=%4.1f e^{-}/Vcal",p1[i])); 	//http://www.cplusplus.com/reference/cstdio/printf/
			leg->AddEntry(graph[i],legentry[i],"LP");
		    
		    multi->Draw("AP");
			multi->GetXaxis()->SetTitleOffset(0.75);
			multi->GetXaxis()->SetTitleSize(0.06);
			multi->GetXaxis()->SetTitle(xtitle);
			
			multi->GetYaxis()->SetTitleOffset(1.0);
			multi->GetYaxis()->SetTitleSize(0.06);
			multi->GetYaxis()->SetTitle(ytitle);
			leg->Draw();
		}
	}
	
	c1->SaveAs("results/Test.pdf");
}

