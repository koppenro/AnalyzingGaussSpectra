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

using namespace std;

int findCurrentPlace(int current) {
	int availableCurrents[8] = {2,6,10,14,18,22,26,30};
	int i = 0;
	while( i < 8 ) {
		if(availableCurrents[i] == current) {break;}
		i++;
	}
	return(i);
}

void readin(vector <double> *data_x, vector <double> *data_y) {
	
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
		int i = findCurrentPlace(current);
		data_x[i].push_back(measurement);		// data_x[0] = 2mA, data_x[1] = 6mA, etc.
		//cout << data_x[findCurrentPlace(current)].push_back(x) << endl;
		fc >> measurement >> expectation;
		cout << measurement << " " << expectation << endl;
		fc.getline(linecont, 500);
	}
	
	
	
}


//~ 
//~ void PlotGraph(bool linearfit = true, bool plotlegend = true, bool verbose = true){
//~ 
//~ //Read in
//~ 
  //~ char linecont[500], xtitle[256], ytitle[256];
  //~ int n, *n_size, *color, *hcol;
  //~ double x,y;
  //~ vector <double> *data_x, *data_y;
  //~ double *p1, *chisqr, *ndf, *chi_ndf, *p0, *p0err, *p1err; 
//~ 
  //~ TString *filename, *legentry;
  //~ TGraph **graph;
  //~ TMultiGraph *multi = new TMultiGraph(); 
  //~ TF1 *p1fit;
  //~ fstream fc,df;  
  //~ char *legtitle, *outputsave, *outputtxt, *legsave;
//~ 
  //~ TLegend *leg = new TLegend(0.53,0.12,0.9,0.5);
  //~ leg->SetFillStyle(1001);
  //~ leg->SetFillColor(0);
  //~ leg->SetTextSize(0.04);
//~ 
  //~ // read config file
  //~ fc.open(configfile, ios::in);
  //~ fc.getline(linecont,500);
  //~ if (verbose)  cout <<  linecont << endl;
  //~ fc.getline(linecont,500);
  //~ if (verbose) cout << linecont << endl;
  //~ fc >> n;
  //~ // --------------------------------
  //~ // init variables
  //~ // --------------------------------
  //~ color = new int[n];
  //~ n_size = new int[n];
  //~ p0 = new double[n];
  //~ p0err = new double[n];
  //~ p1 = new double[n];
  //~ p1err = new double[n];
  //~ chisqr = new double[n];
  //~ ndf = new double[n];
  //~ chi_ndf = new double[n];
  //~ filename = new TString[n];
  //~ legentry = new TString[n];
  //~ graph = new TGraph*[n];
  //~ data_x = new vector <double>[n];
  //~ data_y = new vector <double>[n];
  //~ legtitle = new char[256];
  //~ outputsave = new char[500];
  //~ outputtxt = new char[500];
  //~ legsave = new char[256];
  //~ // --------------------------------
  //~ if (verbose)  cout << n << endl;
  //~ fc.getline(linecont,500);
  //~ //  cout << linecont << endl;
  //~ fc.getline(linecont,500);
//~ 
  //~ if (verbose)  cout << linecont << endl;
  //~ for (int i = 0; i < n; i++){
    //~ fc >> filename[i] >> color[i] >> legentry[i];
    //~ if (legentry[i].Contains(":")) legentry[i].ReplaceAll(":"," ");
    //~ if (verbose)    cout  << filename[i] << "\t" << color[i] << "\t" << legentry[i] << endl;
  //~ }
  //~ fc.getline(linecont,500);
  //~ if (verbose)  cout << linecont << endl;
  //~ fc.getline(linecont,500);  
  //~ if (verbose)  cout << linecont << endl;
  //~ fc.getline(xtitle,256);
  //~ if (verbose)  cout << "x: " << xtitle << endl;
  //~ fc.getline(ytitle,256);
  //~ if (verbose)  cout << "y: " << ytitle << endl;
  //~ fc.getline(linecont, 500);
  //~ double min, max;
  //~ fc >> min;
  //~ cout << "min " << min << endl;
  //~ fc >> max;
  //~ cout << "max " << max << endl;
  //~ fc.getline(linecont,500);
  //~ fc.getline(linecont,500);
  //~ fc.getline(legtitle,256);
  //~ cout << legtitle << endl;
  //~ fc.getline(linecont, 500);
  //~ fc.getline(outputsave,500);
  //~ cout << outputsave << endl;
  //~ fc.getline(outputtxt, 500);
  //~ cout << outputtxt << endl;
  //~ // config file read
  //~ leg->SetHeader(legtitle);
//~ 
  //~ TCanvas *c1 = new TCanvas("c1","PlotGraph",10,10,1800,1000); // for present.
  //~ //  TCanvas *c1 = new TCanvas("c1","PlotGraph",10,10,800,500);
  //~ c1->SetFillColor(0);
  //~ c1->SetBorderMode(0);
  //~ c1->SetLeftMargin(0.12);
//~ 
  //~ char outputtitle[256];
  //~ // read data files and create graphs
//~ 
  //~ for (int i = 0; i < n; i++){
    //~ df.open(filename[i], ios::in);
    //~ df.getline(linecont,500);
    //~ if (verbose)  cout << linecont << endl;
    //~ df.getline(linecont,500);
    //~ if (verbose) cout << linecont << endl;
    //~ int zaehler = 0;
    //~ while(df){
      //~ df >> x >> y;
      //~ data_x[i].push_back(x);
      //~ data_y[i].push_back(y);
      //~ df.getline(linecont,500);
      //~ if(verbose) cout << x << "\t" << y << endl;
    //~ }
    //~ df.close();
    //~ n_size[i] = data_x[i].size() -1;
    //~ if(verbose) cout << n_size[i] << endl;
//~ 
    //~ 
//~ //##########################################################################################################    
//~ //Plotten
//~ 
    //~ graph[i] = new TGraph(n_size[i], &data_x[i][0], &data_y[i][0]);
    //~ graph[i]->SetLineColor(color[i]);
    //~ graph[i]->SetMarkerStyle(20+i);
    //~ graph[i]->SetMarkerColor(color[i]);
    //~ graph[i]->SetMarkerSize(0.75);
    //~ //graph[i]->Draw("ALP");
//~ 
//~ /*    //Find the maximal and minimal element of data[i][]
    //~ double max = data_x[i][0];
    //~ double min = data_x[i][0];
    //~ for(int k = 1; k < n_size[i]; k++) {
		//~ cout << "k = " << k << " data_x[i][k] " << data_x[i][k];
		//~ if(data_x[i][k] < min) {min = data_x[i][k];}
		//~ //if(data_x[i][k] < min) {cout << " data_x[i][k]min " << &data_x[i][k];
		//~ //	min = &data_x[i][k];}
		//~ //cout << " min = " << min << endl;
		//~ if(data_x[i][k] > max) {max = data_x[i][k];}
	//~ }
		    //~ 
    //~ cout << "---------------------------" << endl;
	//~ cout << "data_x[i][1] " << data_x[i][1] << endl;
	//~ cout << "min " << min << " max " << max << endl;
	//~ cout << "---------------------------" << endl;
//~ */
//~ /*	double max;
    //~ double min;
	//~ cout << "Bitte geben Sie die untere Fitgrenze ein ";
	//~ cin >> min;
	//~ cout << "Bitte geben Sie die obere Fitgrenze ein ";
	//~ cin >> max;
//~ */
    //~ p1fit = new TF1("p1fit","pol1",min,max);
    //~ p1fit->SetLineWidth(0.4);
    //~ p1fit->SetLineStyle(2);
    //~ p1fit->SetLineColor(color[i]);
    //~ 
    //~ if(linearfit){  
      //~ graph[i]->Fit("p1fit","R");
      //~ //graph[i]->Fit("pol1","R");
      //~ p0[i]=p1fit->GetParameter(0);
      //~ p0err[i]=p1fit->GetParError(0);
      //~ p1[i]=p1fit->GetParameter(1);
      //~ p1err[i]=p1fit->GetParError(1);
      //~ chisqr[i] = p1fit->GetChisquare() ; 
      //~ ndf[i] = p1fit->GetNDF(); 
      //~ if (ndf != 0) chi_ndf[i] = chisqr[i]/ndf[i];
      //~ else chi_ndf[i] = -1;
    //~ }
    //~ cout << " adding graph[" << i <<"]" << endl; 
    //~ multi->Add(graph[i]);
    //~ 
    //~ if(linearfit) {
	    //~ //Save data
	    //~ std::ifstream FileTest(outputtxt);
		//~ if(!FileTest) {
			//~ //Write header in output document
			//~ ofstream outputfile;
			//~ outputfile.open(outputtxt, ios::out);
			//~ outputfile << "#Linear fit (p0 +- p0err) + (p1 +- p1err)*x with parameters \n";
			//~ outputfile.close();
		//~ }
		//~ ofstream test;
		//~ test.open(outputtxt, ios::out | ios::app);
		//~ //test << "# Linear fit p0 + p1*x with parameters \n");
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
		//~ test.close();
		//~ 
		//~ legentry[i].Append(Form(" p1=%4.1f e^{-}/Vcal",p1[i])); 	//http://www.cplusplus.com/reference/cstdio/printf/
	    //~ leg->AddEntry(graph[i],legentry[i],"LP");
	//~ }
	//~ else{
	    //~ leg->AddEntry(graph[i],legentry[i],"P");
	//~ }
	//~ 
  //~ }
//~ 
  //~ if (verbose){
    //~ for (int i = 0; i < n; i++){
      //~ cout << filename[i] << ":" << endl;
      //~ for (int j = 0; j < n_size[i]; j++){
	//~ cout << data_x[i][j] << "\t" << data_y[i][j] << endl;
      //~ }
    //~ }    
  //~ }
//~ 
  //~ multi->Draw("AP");
  //~ multi->GetXaxis()->SetTitleOffset(0.75);
  //~ multi->GetXaxis()->SetTitleSize(0.06);
  //~ multi->GetXaxis()->SetTitle(xtitle);
//~ 
  //~ multi->GetYaxis()->SetTitleOffset(1.0);
  //~ multi->GetYaxis()->SetTitleSize(0.06);
  //~ multi->GetYaxis()->SetTitle(ytitle);
  //~ if (plotlegend) leg->Draw();
//~ 
  //~ if(linearfit) for (int i = 0; i < n; i++){ 
      //~ cout << "p1 for graph["<< i <<"] : " << p1[i] << " , chi2/ndf [" << i << "] : "<< chi_ndf[i] << endl;
    //~ }
  //~ 
  //~ c1->SaveAs(outputsave);
//~ }
//~ 
//~ 








int main( int argc, char *argv[] ){
	
	//Possible ways to execute: ./exe or ./exe $searchOption (e.g. ./exe Nd -> analyzing all Files including Nd) or 
	// ./exe $searchOption $mean $fitBorder (e.g. ./exe Nd 240 20)
	//First argument: Filter condition for analyzing FileNames
	//Second argument: Mean of peak (int)
	//Third argument: Fit Border x as mean +- fitBorder (int)
	const int n = 20;
	vector <double> *data_x, *data_y;
	data_x = new vector <double>[n];
	data_y = new vector <double>[n];
	
	readin(data_x, data_y);
	
	
	
	return 0;
}


//####################################################################################################################
//#Definition of the functions                                                                                       #
//####################################################################################################################

