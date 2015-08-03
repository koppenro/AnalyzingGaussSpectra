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
void readin(vector <double> *, vector <double> *, int *, const char *);
void PlotGraph(vector <double> *, vector <double> *, const int, int *, TString, const char *, TString, int);


int main( int argc, char *argv[] ){
	char *outputtxt, *outputtxtMW, *directory, *modulename, *legend, *linecont;
	linecont = new char[500];
	//Possible ways to execute: ./CalibrationLine or ./CalibrationLine $legendtitle (e.g. ./CalibrationLine M0307:p20 -> spaces in the title with ":" ) 
	//First argument: Legend Title
	outputtxt = new char[256];
	outputtxt = "../CalibrationLine-Slopes-Analyse/Slopes-CalibrationLines-SingleGauss.txt";
	outputtxtMW = new char[256];
	outputtxtMW = "../CalibrationLine-Slopes-Analyse/Slopes-CalibrationLines-MoReWeb.txt";
	remove(outputtxt);
	remove(outputtxtMW);
	
	//read in config file
	fstream fc;
	fc.open("MakeTxt-Config.config", ios::in);
	fc.getline(linecont, 500);
	fc.getline(linecont, 500);
	directory = new char[500];
	modulename = new char[256];
	int testdir = 0;
	int module = 0;
	double p0, p1, p0err, p1err, chisquare, NDF;
	legend = new char[256];
	const char *datafile;
	datafile = new char[256];
	while(fc >> directory) {
		cout << directory << endl;
		testdir = chdir(directory);
		fc >> modulename >> module;
		int nrchips = 1;
		if(module == 1) {
			nrchips = 16;
		}
		for(int chipnr = 0; chipnr < nrchips; chipnr++) {
			//Gaussian Fit
			datafile = Form("results/C%i-CalibrationLine-Slopes-Errors.txt", chipnr);
			fstream dataopen;
			dataopen.open(datafile, ios::in);
			dataopen.getline(linecont, 500);
			dataopen.getline(linecont, 500);
			dataopen.getline(linecont, 500);
			std::ifstream FileTest(outputtxt);
			if(!FileTest) {
				//Write header in output document
				ofstream outputfile;
				outputfile.open(outputtxt, ios::out);
				outputfile << "//Slopes of Calibration Lines for every analyzed module with Single Gaussian\n";
				outputfile << "//Name\tCurrent\tm Single Gauss\tmerr\tChisquare\tNdf\n";
				outputfile.close();
			}
			ofstream datawrite;
			datawrite.open(outputtxt, ios::out | ios::app);
			int chip = -1;
			while(dataopen >> chip) {
				dataopen >> p0 >> p0err >> p1 >> p1err >> chisquare >> NDF >> legend;
				dataopen.getline(linecont, 500);
				datawrite << modulename << "-C" << chip << "\t" << legend << "\t" << p1 << "\t" << p1err << "\t" << chisquare << "\t" << NDF << "\n";
				cout << modulename << "-C" << chip << "\t" << legend << "\t" << p1 << "\t" << p1err << "\t" << chisquare << "\t" << NDF << "\n";
			}
			dataopen.close();
			datawrite.close();
			
			//MoReWeb Fit
			datafile = Form("results/C%i-CalibrationLine-Slopes-MoReWeb-Errors.txt", chipnr);
			fstream dataMWopen;
			dataMWopen.open(datafile, ios::in);
			dataMWopen.getline(linecont, 500);
			dataMWopen.getline(linecont, 500);
			dataMWopen.getline(linecont, 500);
			std::ifstream FileTestMW(outputtxtMW);
			if(!FileTestMW) {
				//Write header in output document
				ofstream outputfileMW;
				outputfileMW.open(outputtxtMW, ios::out);
				outputfileMW << "//Slopes of Calibration Lines for every analyzed module with MoReWeb Algorithm\n";
				outputfileMW << "//Name\tCurrent\tm Single Gauss\tmerr\tChisquare\tNdf\n";
				outputfileMW.close();
			}
			ofstream datawriteMW;
			datawriteMW.open(outputtxtMW, ios::out | ios::app);
			chip = -1;
			while(dataMWopen >> chip) {
				dataMWopen >> p0 >> p0err >> p1 >> p1err >> chisquare >> NDF >> legend;
				dataMWopen.getline(linecont, 500);
				datawriteMW << modulename << "-C" << chip << "\t" << legend << "\t" << p1 << "\t" << p1err << "\t" << chisquare << "\t" << NDF << "\n";
				cout << modulename << "-C" << chip << "\t" << legend << "\t" << p1 << "\t" << p1err << "\t" << chisquare << "\t" << NDF << "\n";
			}
			dataMWopen.close();
			datawriteMW.close();
		}
	}
	
	return 0;
}
