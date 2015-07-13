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

int findTargetPlace(char *, TString *);
void readin(vector <double> *, vector <double> *, TString *, const char *);
void PlotGraph(vector <double> *, vector <double> *, const int, TString *, TString, const char *, double, double, TString);


int main( int argc, char *argv[] ){
	
	//Possible ways to execute: ./CurrentDependecy or ./CurrentDependency $legendtitle (e.g. ./CurrentDependency M0307:p20 -> spaces in the title with ":" ) 
	//First argument: Legend Title
	const int n = 8;		//number of different targets 
	vector <double> *data_x, *data_y;
	data_x = new vector <double>[n];
	data_y = new vector <double>[n];
	TString legtitle;
	const char * outputtxt;
	outputtxt = new char[256];
	outputtxt = "CurrentDependency-Slopes.txt";
	TString outputpdf;
	outputpdf = "results/CurrentDependency";
	const char* outputtxt2;
	outputtxt2 = new char[256];
	outputtxt2 = "CurrentDependency-Slopes-MoReWeb.txt";
	TString outputpdf2;
	outputpdf2 = "results/CurrentDependency-MoReWeb";
	double ylimitup = 250;
	double ylimitdown = 0;
	
	//DEFINITION OF OPTIONS
	const char * temp;
	temp = new char[5];
	temp = "p20";
	bool module = false;
	long roc = 0;
	bool rocbool = false;
	bool legbool = false;
	legtitle = "";
	
	for(int i = 1; i < argc; i++) {
		if(strstr(argv[i], "-temp") != NULL) { temp = argv[i+1]; }		//Set temperature, standard: "p20"
		if(strstr(argv[i], "-m") != NULL) { module = true; }			//Set module, standard: false
		if(strstr(argv[i], "-roc") != NULL) { roc = int(strtol(argv[i+1], NULL, 0)); rocbool = true; }
		if(strstr(argv[i], "-l") != NULL) { legtitle = argv[i+1]; legbool = true; }
	}
	if (legtitle.Contains(":")) legtitle.ReplaceAll(":"," ");
	
	
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
	
	//~ if(argc == 2) {
		//~ legtitle.Form("%s", argv[1]);
		//~ if (legtitle.Contains(":")) legtitle.ReplaceAll(":"," ");
		//~ //cout << legtitle << endl;
	//~ }
	//~ else {
		//~ legtitle = "";
	//~ }
	
	
	// Current dependency line with single gaussian fit
	const char *datafile;
	datafile = new char[256];
	int chipnr = 0;
	int nrchips = 1;
	if(module) {
		nrchips = 16;
	}
	if(rocbool) {
		chipnr = roc;
		nrchips = roc+1;
	}
	const char * outputtxtakt;
	outputtxtakt = new char[256];
	const char * outputtxtakt2;
	outputtxtakt2 = new char[256];
	TString outputpdfakt;
	TString outputpdfakt2;	
	
	
	for(; chipnr < nrchips; chipnr++) {
		legtitle.Append(Form(" - C%i", chipnr));
		datafile = Form("results/C%i-Analysis-GaussFit.txt", chipnr);
		//datafile = "results/Analysis-GaussFit.txt";
		readin(data_x, data_y, availableTargets, datafile);
		//~ for(int i = 0; i < n; i++) {
			//~ cout << data_x[i][0] << " " << data_y[i][0] << endl;
		//~ }
		int testdir = chdir("results/");
		if(testdir == 1) {}
		outputtxtakt = Form("C%i-%s", chipnr, outputtxt);
		outputtxtakt2 = Form("C%i-%s", chipnr, outputtxt2);
		remove(outputtxtakt);
		testdir = chdir("../");
		outputpdf.Form("results/C%i-CurrentDependency", chipnr);
		outputpdf2.Form("results/C%i-CurrentDependency-MoReWeb", chipnr);
		PlotGraph(data_x, data_y, n, availableTargets, legtitle, outputtxtakt, ylimitup, ylimitdown, outputpdf);
		for(int i = 0; i < n; i++) {
			data_x[i].clear();
			data_y[i].clear();
		}
		
		//Analyse MoReWeb fit
		datafile = Form("results/C%i-Analysis-MoReWebFit.txt", chipnr);
		//datafile = "results/Analysis-MoReWebFit.txt";
		vector <double> *dataMW_x, *dataMW_y;
		dataMW_x = new vector <double>[n];
		dataMW_y = new vector <double>[n];
		readin(dataMW_x, dataMW_y, availableTargets, datafile);
		testdir = chdir("results/");
		remove(outputtxtakt2);
		testdir = chdir("../");
		legtitle.Append(" - MWeb");
		PlotGraph(dataMW_x, dataMW_y, n, availableTargets, legtitle, outputtxtakt2, ylimitup, ylimitdown, outputpdf2);
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
		
	}
	
	return 0;
}


//####################################################################################################################
//#Definition of the functions                                                                                       #
//####################################################################################################################

int findTargetPlace( char * source, TString * availableCurrents ) {
	int i = 0;
	while( i < 8 ) {
		if(strstr(availableCurrents[i].Data(), source) != NULL) {break;}
		i++;
	}
	//cout << "i = " << i << endl;
	return(i);
}

void readin(vector <double> *data_x, vector <double> *data_y, TString * availableTargets, const char * txt) {
	
	char linecont[500];
	fstream fc;
	fc.open(txt, ios::in);
	fc.getline(linecont, 500);
	fc.getline(linecont, 500);
	
	char temp[5];
	char * source;
	source = new char[3];
	while(fc >> source) {
		cout << source << " ";
		int current;
		double measurement; 
		fc >> temp >> current >> measurement;
		cout << current << " " << measurement << endl;
		
		int i = findTargetPlace( source, availableTargets );
		//~ //cout << "i " << i << " ";
		data_x[i].push_back(current);		// data_x[0] ^= Fe, data_x[1] ^= Cu, etc.
		//~ //cout << data_x[i][0] << " ";
		data_y[i].push_back(measurement);
		fc.getline(linecont, 500);
	}
}

void PlotGraph(vector <double> *data_x, vector <double> *data_y, const int n, TString * availableTargets, TString legtitle, const char * outputtxt, double ylimitup, double ylimitdown, TString outputpdf) {
	
	const char *xtitle, *ytitle;
	int *n_size;
	double *p1, *p0, *p0err, *p1err; 
	
	TString *legentry;
	TGraph **graph;
	TMultiGraph *multi = new TMultiGraph(); 
	TF1 *p1fit;
	fstream fc,df;  
	
	TLegend *leg = new TLegend(0.1,0.1,0.9,0.9);
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
	xtitle = "current ( mA )";
	ytitle = "signal ( Vcal DAC )";
	
	
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
		    //graph[i]->SetMarkerSize(0.75);
		    graph[i]->SetMarkerSize(1.5);
		    //graph[i]->SetLineWidth(2);
		    
		    if(i == n-1) {
				graph[i]->GetYaxis()->SetRange(0,250);
			}
		    //graph[i]->GetYaxis()->SetRange(0,250);
		    //graph[i]->Draw("ALP");
		    legentry[i] = Form("%s", availableTargets[i].Data());
		    
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
		    p1fit->SetLineWidth(3);
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
			legentry[i].Append(Form("; p1=%4.2f Vcal/mA",p1[i])); 	//http://www.cplusplus.com/reference/cstdio/printf/
			leg->AddEntry(graph[i],legentry[i],"LP");
		    
		    multi->SetMaximum(ylimitup);	//Set upper y Axis limit
		    multi->SetMinimum(ylimitdown);	//Set lower y Axis limit
		    multi->Draw("AP");
			multi->GetXaxis()->SetTitleOffset(0.75);
			multi->GetXaxis()->SetTitleSize(0.06);
			multi->GetXaxis()->SetTitle(xtitle);
			
			multi->GetYaxis()->SetTitleOffset(1.0);
			multi->GetYaxis()->SetTitleSize(0.06);
			multi->GetYaxis()->SetTitle(ytitle);
			//leg->Draw();
		}
	}
	
	//~ TList * test;
	//~ test = multi->GetListOfGraphs();
	//~ cout << test << endl;
 
	//c1->SaveAs("results/CurrentDependency-woLegend.pdf");
	//c1->SaveAs("results/CurrentDependency-woLegend.png");
	c1->SaveAs(outputpdf.Append("-woLegend.pdf"));
	c1->SaveAs(outputpdf.ReplaceAll(".pdf",".png"));
	
	//c1->Clear();
	//Plot legend in second pad right of graph
	c1->Divide(2,1);
	//Get the pads and set the size properly
	TVirtualPad *pad1 = new TPad();
	TVirtualPad *pad2 = new TPad();
	pad1 = c1->GetPad(1);
	pad2 = c1->GetPad(2);
	pad1->SetPad(0,0,0.75,1);
	pad2->SetPad(0.7,0,1,1);
	c1->cd(1);
	pad1->SetLeftMargin(0.14);
	multi->Draw("AP");
	c1->cd(2);	
	leg->Draw();
	//c1->SaveAs("results/CurrentDependency.pdf");
	//c1->SaveAs("results/CurrentDependency.png");
	c1->SaveAs(outputpdf.ReplaceAll("-woLegend",""));
	c1->SaveAs(outputpdf.ReplaceAll(".png",".pdf"));
}

