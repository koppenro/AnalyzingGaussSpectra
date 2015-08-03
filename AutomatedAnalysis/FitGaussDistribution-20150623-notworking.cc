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

void MoReWebAlgorithm(TString rootfile, TString Source, int actualCurrent, double peak, const char * temp, double trimvcal, bool module, bool rocbool, int roc);
TF1 * FitGaus(TH1 * histo, double initguess, double xmin, double xmax);
TString * listofRootFiles(DIR *, int *, const char *);
TString * identifySource(TString, TString *, const int *);
int identifyCurrent(TString, TString *, int *, int);
void getTH1(TString, TString, int, double, double, double, int, const char *, bool, bool, int);
double * defaultPeakPosition(TString, int);
int nrTriggers(TString, TString);
double findsourcelit(TString);
void deleteLinesTxt(const char*, TString);
double pixelSurface(char*, int, int);
double calculateRate(int, int);
double ChiSquare(TH1* histo, TF1*Fit, double leftborder, double rightborder);


int main( int argc, char *argv[] ){
	
	//Possible ways to execute: ./FitGaussDistribution (temp is "p20" as standard) or ./FitGaussDistribution $temp or ./FitGaussDistribution $temp $searchOption (e.g. ./FitGaussDistribution p20 Nd -> analyzing all Files including Nd) or 
	// ./FitGaussDistribution $temp $searchOption $mean $fitBorder (e.g. ./FitGaussDistribution p20 Nd 240 20)
	//First argument: temp (e.g. p20, p10, p0, m10, m20)
	//Second argument: Filter condition for analyzing FileNames
	//Third argument: Mean of peak (int)
	//Fourth argument: Fit Border x as mean +- fitBorder (int)
	
	//DEFINITION OF OPTIONS
	const char * temp;
	temp = new char[5];
	temp = "p20";
	bool module = false;
	long int trimvcal = 35;
	const char * searchOption;
	searchOption = new char[50];
	searchOption = "Spectrum_";
	int fitBorder = 20;
	int fitPeak = 0;
	int currentBegin = 2;
	int currentEnd = 30;
	int currentStep = 4;
	bool fitBorderbool = false;
	bool fitPeakbool = false;
	bool rocbool = false;
	long roc = 0;
	
	for(int i = 1; i < argc; i++) {
		if(strstr(argv[i], "-temp") != NULL) { temp = argv[i+1]; }		//Set temperature, standard: "p20"
		if(strstr(argv[i], "-m") != NULL) { module = true; }			//Set module, standard: false
		if(strstr(argv[i], "-roc") != NULL) { roc = int(strtol(argv[i+1], NULL, 0)); rocbool = true;}
		if(strstr(argv[i], "-T") != NULL) { trimvcal = strtol(argv[i+1], NULL, 0);}		//Set trim value, standard: 35
		if(strstr(argv[i], "-s") != NULL) { searchOption = argv[i+1]; }		//Set search option, standard: "Spectrum_"
		if(strstr(argv[i], "-fb") != NULL) { fitBorder = int(strtol(argv[i+1], NULL, 0)); fitBorderbool = true; }	//Set fitBorder to do fit around peak, standard: "20"
		if(strstr(argv[i], "-fp") != NULL) { fitPeak = int(strtol(argv[i+1], NULL, 0)); fitPeakbool = true; }		//Set expected peak value
		if(strstr(argv[i], "-c") != NULL) { 
			currentBegin = int(strtol(argv[i+1], NULL, 0));		//Set start value of current, standard: 2mA
			currentEnd = int(strtol(argv[i+2], NULL, 0));		//Set end value of current, standard: 30mA
			currentStep = int(strtol(argv[i+3], NULL, 0));		//Set current step, standard: 4mA
		}
		if(strstr(argv[i], "-nc") != NULL) { currentBegin = 0; currentEnd = 0; currentStep = 1; }
	}
	//Produce list of Currents
	int * Currents;
	Currents = new int[30];
	Currents[0] = 0;
	TString *CurrentString;
    CurrentString = new TString[30];
	int numberofcurrents = (currentEnd - currentBegin)/currentStep+1;
	for(int i = 0; i < numberofcurrents; i++) {
		Currents[i] = currentBegin + currentStep*i;
		CurrentString[i] = Form("_%imA", Currents[i]);
	}
	//Variables to determine Source
	const int numbersources = 8;
    TString *Sources;
	Sources = new TString[numbersources];
	Sources[0] = "_Fe"; Sources[1] = "_Cu", Sources[2] = "_Zn", Sources[3] = "_Mo";
	Sources[4] = "_Ag", Sources[5] = "_In"; Sources[6] = "_Sn", Sources[7] = "_Nd";
	
	
	//###########################################################################################################################
	//#														BEGIN: MAIN WORK													#
	//###########################################################################################################################
	
	int intTest = mkdir("results/", 0777);
	if(intTest == 0) {}
	
	//READ IN ROOT FILES WITH SEARCH OPTION
	DIR *datadir;
	TString * RootFiles;
	RootFiles = new TString[200];
	
	int n;
	datadir = opendir("out");
	RootFiles = listofRootFiles(datadir, &n, searchOption);		//Read in the root-Files in the directory "out"
	closedir(datadir);
	
	//intTest = chdir("../");
	//CREATE DIRECTORY FOR MODULES
	if(module) {
		intTest = chdir("results");	
		intTest = mkdir("C0/", 0777); intTest = mkdir("C1/", 0777); intTest = mkdir("C2/", 0777); intTest = mkdir("C3/", 0777); intTest = mkdir("C4/", 0777); intTest = mkdir("C5/", 0777);
		intTest = mkdir("C6/", 0777); intTest = mkdir("C7/", 0777); intTest = mkdir("C8/", 0777); intTest = mkdir("C9/", 0777); intTest = mkdir("C10/", 0777); intTest = mkdir("C11/", 0777);
		intTest = mkdir("C12/", 0777); intTest = mkdir("C13/", 0777); intTest = mkdir("C14/", 0777); intTest = mkdir("C15/", 0777); 
		intTest = chdir("../");
	}
	
	//READ IN HISTOGRAMS AND FIT
	TString * actualSource;
	actualSource = new TString[1];
	int actualCurrent;
	for(int i = 0; i < n; i++) {
		//cout << RootFiles[i] << endl;
		actualSource = identifySource(RootFiles[i], Sources, &numbersources);	//Find Source of document in Filename
		if(currentBegin == 0 and currentBegin == 0) {
			actualCurrent = 0;
		}
		else {
			actualCurrent = identifyCurrent(RootFiles[i], CurrentString, Currents, numberofcurrents);	//Find Current of document in Filename
		}
		if(strstr(actualSource[0], "FALSE") == NULL) {
			if(actualCurrent != -1) {
				cout << RootFiles[i] << "\t" << actualSource[0] << "\t" << actualCurrent << endl;
				
				//Find and change borders and peak of fit as wished from the user
				double * fitParameter;
				fitParameter = new double[3];
				fitParameter = defaultPeakPosition(actualSource[0], fitBorder);
				if(fitPeakbool) {
					fitParameter[0] = fitPeak;
					fitParameter[1] = fitParameter[0] - fitBorder;
					fitParameter[2] = fitParameter[0] + fitBorder;
				}
				if(fitBorderbool) {
					fitParameter[1] = fitParameter[0] - fitBorder;
					fitParameter[2] = fitParameter[0] + fitBorder;
				}
				
				
				//~ TFile *fr = new TFile("out/Spectrum_Zn_6mA-run010.root");
				//~ TH1 *histo[16], *hDummy;
				//~ fr->Cd("Xray");
				//~ 
				//~ Char_t *histoname = "q_Zn_C0_V0";	//Build name of histogram with Source and chip number
				//~ cout << "Histogramm " << histoname << " eingelesen" << endl;
				//~ gDirectory->GetObject(histoname, hDummy);
				//~ if (hDummy != 0) {
					//~ histo[0] = (TH1*)hDummy->Clone("histo");
					//~ //histo->SetDirectory(0);
					//~ //histo->Draw("colz");
					//~ histo[0]->GetYaxis()->SetTitleOffset(1.3);
					//~ histo[0]->GetYaxis()->SetTitleSize(0.05);
					//~ histo[0]->GetYaxis()->SetTitle("# Events");
					//~ histo[0]->GetXaxis()->SetTitleSize(0.05);
					//~ histo[0]->GetXaxis()->SetTitle("VcalDAC");
				//~ }
				//~ else {
					//~ cout << "Histogrammname ueberpruefen" << endl;
					//~ //histo = NULL;
				//~ }
				//~ histo[0]->GetMaximumBin();
				//~ fr->Close();
				//cout << Form("out/%s", RootFiles[0]) << endl;
				
				
				//Single Gaussian fit
				getTH1(RootFiles[i], actualSource[0], actualCurrent, fitParameter[0], fitParameter[1], fitParameter[2], fitBorder, temp, module, rocbool, roc);
				
				//MoReWeb Fit
				//MoReWebAlgorithm(RootFiles[i], actualSource[0], actualCurrent, fitParameter[0], temp, trimvcal, module, rocbool, roc);
			}
		}
	}
	
	return 0;
}


//####################################################################################################################
//#Definition of the functions                                                                                       #
//####################################################################################################################

//Not sure, that the calculation is right!!!
double ChiSquare(TH1* histo, TF1* Fit, double leftborder, double rightborder) {
	
	double chisquare = 0;
	for(int i = leftborder; i <= rightborder; i++) {
		int nMeasured = histo->GetBinContent(i);
		double nExpected = Fit->Eval(i);
		chisquare = chisquare + pow((nMeasured-nExpected),2)/(1.0*nExpected);
	}
	chisquare = chisquare/Fit->GetNDF();
	
	return(2.0);
}

TString * listofRootFiles(DIR *datadir, int * n, const char * searchOption) {
    struct dirent *entry;
    TString * RootFiles;
    RootFiles = new TString[200];
    
    int i = 0;
    do
    {
        entry = readdir(datadir);
        if (entry)
        {
			if ( strstr(entry->d_name, ".root") != NULL and strstr(entry->d_name, searchOption) != NULL) 	// Nur Dateien mit Spectrum_ und .root werden analysiert
			{
				RootFiles[i] = entry->d_name;
				//cout << i << " " << RootFiles[i] << endl;
				i++;
			}
        }
    } while (entry);
    
    *n = i;
    return RootFiles;
}

//--------------------------------------------------------------------------------------------------------------------
TString * identifySource(TString RootFile, TString * Sources, const int * numbersources) {
//	for(int i = 0; i < *numbersources; i++) {
//		Sources[i].Insert(0, "_");	//Insert at the beginning of the sources an underscore
//	}
	//cout << *numbersources << endl;
//	cout << RootFile << endl;
	int j = 0;
	while (j < *numbersources) {		//search for Source
		if (strstr(RootFile, Sources[j]) != NULL) {break;}
		j++;
	}
	if(j == *numbersources) {		//Source not found
		//cout << "if " << endl;
		TString * False;
		False = new TString[1];
		False[0] = "FALSE";
		return False;
	}
	TString * Source;
	Source = new TString[1];
//	for(int i = 0; i < *numbersources; i++) {
//		Sources[i].Remove(0, 1);	//delete underscore in front of Sources[j]
//	}
	Source[0] = Sources[j];
	Source[0].Remove(0,1);
	return Source;
}

//--------------------------------------------------------------------------------------------------------------------
int identifyCurrent(TString RootFile, TString * CurrentString, int * Currents , int numbercurrents) {
	int j = 0;
	while (j < numbercurrents) {		//search for Current
		if (strstr(RootFile, CurrentString[j]) != NULL) {break;}
		j++;
	}
	if(j == numbercurrents) {		//Current not found
		//cout << "if " << endl;
		return(-1);
	}
	return Currents[j];
}

//--------------------------------------------------------------------------------------------------------------------

//Definition of the function for importing and analysing the spectra with MoReWebAlgorithm
void MoReWebAlgorithm(TString rootfile, TString Source, int actualCurrent, double peak, const char * temp, double trimvcal, bool module, bool rocbool, int roc){	
	
	int intdir = chdir("out");
	char pfad[256];
	const char * GETCWD;
	GETCWD = getcwd(pfad, 256);
	if(GETCWD == NULL) {}
	//cout << pfad << endl;
	TFile *fr = new TFile(rootfile);
	TH1 *histo[16], *hDummy;
	fr->Cd("Xray");
	double maxpeak, maxpeakerror;
	int nrchips = 0; 
	
	//GRAPHICAL OUTPUT
	TCanvas *c2 = new TCanvas("c2","Roentgenpeak Gauss",10,10,1500,1000);
	c2->SetLeftMargin(0.15);
	gPad->SetLeftMargin(0.17);
	gPad->SetBottomMargin(0.17);
	c2->SetFillColor(0);
	c2->SetBorderMode(0);
	
	int chipnr = 0;
	if(module == false) {		//Analyze only single chip assembly
		nrchips = 1;
	}
	if(module and rocbool) {	//Analyze only single roc
		chipnr = roc;
		nrchips = roc+1;
	}
	else {					//Analyze whole module
		nrchips = 16;
	}
	
	for(; chipnr < nrchips; chipnr++) {
		
		Char_t *histoname = Form("q_%s_C%i_V0",Source.Data(), chipnr);	//Build name of histogram with Source and chip number
		cout << "Histogramm " << histoname << " eingelesen" << endl;
		gDirectory->GetObject(histoname, hDummy);
		if (hDummy != 0) {
			histo[chipnr] = (TH1*)hDummy->Clone("histo");
			//histo->SetDirectory(0);
			//histo->Draw("colz");
			histo[chipnr]->GetYaxis()->SetTitleOffset(1.3);
			histo[chipnr]->GetYaxis()->SetTitleSize(0.05);
			histo[chipnr]->GetYaxis()->SetTitle("# Events");
			histo[chipnr]->GetXaxis()->SetTitleSize(0.05);
			histo[chipnr]->GetXaxis()->SetTitle("VcalDAC");
		}
		else {
			cout << "Histogrammname ueberpruefen" << endl;
		}
		
		
		//zooming the x-axis
		TAxis *axis = histo[chipnr]->GetXaxis();
		//   axis->SetRangeUser(50,220);  
		//   axis->SetRangeUser(50,350);  
		axis->SetRangeUser(0,350);  	
		
		//const char * comment;
		//comment = new char[500];
		//First Gaussian fit around the expectation
		double xmin = histo[chipnr]->GetBinLowEdge(1);
		double xmax = histo[chipnr]->GetBinLowEdge(histo[chipnr]->GetNbinsX());
		TF1 * gausfit;
		gausfit = FitGaus(histo[chipnr], peak, xmin, xmax);
		double gaus0 = gausfit->GetParameter(0);
		double gaus1 = gausfit->GetParameter(1);
		double gaus2 = gausfit->GetParameter(2);
		cout << "Found Start parameter: " << gaus0 << " " << gaus1 << " " << gaus2 << endl;
		
		
		//Second Fit with more complexe function
		const char * name;
		name = new char[30];
		name = Form("MoReWeb-fit_%s", histo[chipnr]->GetName());
		cout << "name " << name << endl;
		TF1 * myfit = new TF1(name, "([0]+[1]*x+gaus(2)+gaus(5))*(1+TMath::Erf((x-[8])/[9]))/2", xmin, xmax);
	
		//Find the overall average in the y-direction to define some good starting parameters
	    double y_avg = histo[chipnr]->Integral() / histo[chipnr]->GetNbinsX();
		double maxbin = gaus1;
	    double maximum = gaus0;
	    double signalSigma = gaus2;
	
	    // Find the overall mean
		double mean = histo[chipnr]->GetMean();
	
		//Hard coded limit on the slope of the linear part
		double param1limit = 0.5;
		
		//Hard coded guess at the noise spread
		double noiseSigma = 30;
	
		//Hard coded trimvalue for the Erf turn on
		double trimvalue = trimvcal;
	
		//Initial guess of constant part is half of the overall y-average
		myfit->SetParameter(0, y_avg / 2);
		cout << "SetParameter0: " << myfit->GetParameter(0) << endl;
		
		//Limit on the constant part; it should be positive, and below the y-average
		//because the y-average is biased above the noise by the signal peak
		myfit->SetParLimits(0, 0, 2 * y_avg);
		cout << "SetParameterLimits0: " << 0 << " " << 2*y_avg << endl;
		
		//Initial guess of the linear part is flat
		myfit->SetParameter(1, 0);
		cout << "SetParameter1: " << myfit->GetParameter(1) << endl;
		
		//Limits on the linear part, from the hardcoded value above
		myfit->SetParLimits(1, -4 * param1limit, 4 * param1limit);
		cout << "SetParameterLimits1: " << -4*param1limit << " " << 4*param1limit << endl;
		
		//Initial guess for the size of the signal is the maximum of the histogram
		myfit->SetParameter(2, maximum);
		cout << "SetParameter2: " << myfit->GetParameter(2) << endl;
		myfit->SetParLimits(2, 0.5 * maximum, 2 * maximum);
		cout << "SetParameterLimits2: " << 0.5*maximum << " " << 2*maximum << endl;
		
		//Initial guess for the center of the signal to be where the maximum bin is located
		myfit->SetParameter(3, maxbin);
		cout << "SetParameter3: " << myfit->GetParameter(3) << endl;
		
		double low = maxbin - 2 * signalSigma;
		//if low < trimvalue:
		//   low = trimvalue/2
		myfit->SetParLimits(3, low, maxbin + 2 * signalSigma);
		cout << "SetParameterLimits3: " << maxbin + 2*signalSigma << endl;
		
		//Initial guess for the sigma of the signal, from the hardcoded value above
		myfit->SetParameter(4, signalSigma);
		cout << "SetParameter4: " << myfit->GetParameter(4) << endl;
		
		myfit->SetParLimits(4, signalSigma - 10, signalSigma + 10);
		cout << "SetParameterLimits4: " << signalSigma - 10 << signalSigma + 10 << endl;
		
		//Initial guess for the size of the guassian noise to be half of the overall y-average (other half is the constant term)
		myfit->SetParameter(5, y_avg * 10);
		
		cout << "SetParameter5: " << myfit->GetParameter(5) << endl;
		// Limits on the amount of gaussian noise, should be below y-average
		// but above 0 for the same reasons as listed for Par0
		myfit->SetParLimits(5, 0, y_avg * 20);
		cout << "SetParameterLimits4: " << 0 << y_avg *20 << endl;
		
		//Initial guess for gaussian noise at the mean of the histogram
		myfit->SetParameter(6, mean);
		//Limits on the location of the noise to be somewhere in the fit region
		myfit->SetParLimits(6, xmin, xmax);
		
		//Initial guess for the noise sigma, hardcoded above
		myfit->SetParameter(7, noiseSigma);
		//Limits on noise sigma, used to make sure the noise guassian doesn't accidentally try to fit the signal
		myfit->SetParLimits(7, noiseSigma, 10 * noiseSigma);
		
		//Initial guess for the turn on is at the hardcorded trimvalue
		myfit->SetParameter(8, trimvalue);
		//Limits on where the turn on occurs are guessed at +-10 away from the given trim value
		//goes to very low values, but doesn't seem to affect the fit too much. Maybe a lower bound can be set to e-5
		myfit->SetParLimits(8, 0, trimvalue + 30);
		
		//Initial guess for the turn on speed is set to 5
		myfit->SetParameter(9, 5);
		//Limit on turn on speed between 0.1 and 10. This value should be positive and shouldn't be much more below 0.1 otherwise it will affect the rest of the fit
		myfit->SetParLimits(9, 0.01, 20);
		myfit->SetLineColor(kBlue);	
	
		histo[chipnr]->Fit(myfit, "");
		histo[chipnr]->GetXaxis()->SetRange(0, peak+100);
		//TF1 * myfunc = histo->GetFunction("myfit");
		cout << "Chi Quadrat " << myfit->GetChisquare()/myfit->GetNDF() << endl;
		cout << myfit->GetNDF() << endl;
		cout << "Chi Quadrat 2 " << histo[chipnr]->Chisquare(myfit)/myfit->GetNDF() << endl;
		
		TF1 * backgroundFit = new TF1(name, "([0]+[1]*x+gaus(2))*(1+TMath::Erf((x-[5])/[6]))/2", xmin, xmax);
		backgroundFit->FixParameter(0, myfit->GetParameter(0));
		backgroundFit->FixParameter(1, myfit->GetParameter(1));
		backgroundFit->FixParameter(2, myfit->GetParameter(5));
		backgroundFit->FixParameter(3, myfit->GetParameter(6));
		backgroundFit->FixParameter(4, myfit->GetParameter(7));
		backgroundFit->FixParameter(5, myfit->GetParameter(8));
		backgroundFit->FixParameter(6, myfit->GetParameter(9));
		backgroundFit->SetLineColor(kBlue);
		backgroundFit->SetLineStyle(2);
		histo[chipnr]->Fit(backgroundFit, "+Q");
		
		histo[chipnr]->SetStats(0);
		histo[chipnr]->SetLineColor(1);
		histo[chipnr]->Draw();			
		
		
		//Save diagram
		intdir = chdir("../results");	
		if(module) {
			Char_t *subdirectory = Form("C%i", chipnr);
			intdir = chdir(subdirectory);
		}	
	
		TString Substring, outputpdf, outputpng;
		Substring = TString(rootfile(0, rootfile.Length()-5));
		outputpdf = Form("%s-C%i-MoReWeb.pdf", Substring.Data(), chipnr);
		c2->SaveAs(outputpdf);
		outputpng = Form("%s-C%i-MoReWeb.png", Substring.Data(), chipnr);
		c2->SaveAs(outputpng);
		c2->Clear();
		if(module) {
			intdir = chdir("../");
		}			
		intdir = chdir("../out");
		
		
		//Save data as txt and root file
		intdir = chdir("../results/");
		Char_t *outputtitle = Form("C%i-Analysis-MoReWebFit.txt", chipnr);
		//outputtitle = new char[250];
		//outputtitle = "Analysis-MoReWebFit.txt";
		//Save data in .txt
		std::ifstream FileTest(outputtitle);
		if(!FileTest) {
			//Write header in output document
			ofstream outputfile;
			outputfile.open(outputtitle, ios::out);
			outputfile << "//Output from FitGaussDistribution, Automatisierte Spektrenauswertung mit MoReWeb Algorithmus fuer chip ";
			outputfile << chipnr << "\n";
			outputfile << "//Source\tTemperature\t Current (mA)\tPeak (Vcal)\tErrorPeak (Vcal)\t#electrons(expected)\tFile\n";
			outputfile.close();
		}
		else{
			deleteLinesTxt(outputtitle, rootfile);
		}
		double lit = findsourcelit(Source);					//Expected number of electrons (NIST)
		//Extract peak position and error on peak position
		maxpeak = myfit->GetParameter(3);
		maxpeakerror = myfit->GetParError(3);
		
		//Save measurement as .txt
		ofstream outputfile;
		outputfile.open(outputtitle, ios::out | ios::app);
		outputfile << Source.Data() << "\t" << temp << "\t" << actualCurrent << "\t" << maxpeak << "\t" << maxpeakerror << "\t" << lit << "\t"; 
		outputfile << rootfile.Data() << "\n";
		outputfile.close();
		if(intdir == 0) {} 	//this line is only to get rid of the warning, that intdir is initialized but not used
		
		//Save measurement as .root
		Substring = TString(rootfile(0, rootfile.Length()-5));
		Char_t * outputhisto = Form("%s-%s-Histo-MoReWeb", Substring.Data(), histoname);
		Char_t * outputhistodelete = Form("%s-%s-Histo-MoReWeb;1", Substring.Data(), histoname);
		TFile *savehisto = new TFile("Analysis-GaussFit-MoReWebFit.root", "UPDATE");
		savehisto->Delete(outputhistodelete);
		histo[chipnr]->Write(outputhisto);
		Char_t * outputfit = Form("%s-%s-MoReWebAlgorithm", Substring.Data(), histoname);
		Char_t * outputfitdelete = Form("%s-%s-MoReWebAlgorithm;1", Substring.Data(), histoname);
		savehisto->Delete(outputfitdelete);
		myfit->Write(outputfit);
		savehisto->Close();
		savehisto->Delete();
		
		intdir = chdir("../out");
		
	}
	
	intdir = chdir("../");
	
	
}

//--------------------------------------------------------------------------------------------------------------------

TF1 * FitGaus(TH1 * histo, double initguess, double xmin, double xmax) {
	
	//cout << "xmax " << xmax << endl;
	
	TF1 * fit = new TF1("gausFit", "gaus(0)", xmin, xmax);
	fit->SetLineColor(4);
	double y_avg = histo->Integral() / histo->GetNbinsX();
	//Signal peak should be have a sigma of ~10
	double signalSigma = 10;
	
	//former: initguess = self.GetInitialEnergyGuess(self.Attributes["Target"])
	double left = 0;
	double right = 0;
	if (initguess < 0) {
		//No predefined position, so guess in the middle and put limits and lower and upper bounds and the guess at the mean value
        initguess = histo->GetMean();
        left = xmin;
        right = xmax;
	}
    else {
		left = initguess - 40;
		right = initguess + 40;
	}
	
	fit->SetParameter(0, 0.5 * y_avg);
	fit->SetParLimits(0, 0, histo->Integral());
	fit->SetParameter(1, initguess);
	fit->SetParLimits(1, left, right);
	fit->SetParameter(2, signalSigma);
	//Make sure we actually fit a 'signal like' peak, nothing to brad or narrow
	fit->SetParLimits(2, signalSigma - 5, signalSigma +5);
	fit->SetLineColor(kBlue);
	
	histo->Fit(fit, "");
	//histo->GetXaxis()->SetRange(0, 250);
	//histo->SetStats(0);
	//histo->SetLineColor(1);
	//histo->Draw();
	
	return fit;
}

//--------------------------------------------------------------------------------------------------------------------

//Definition of the function for importing and analysing the spectra with single Gaussian peak
void getTH1(TString rootfile, TString Source, int actualCurrent, double peak, double leftborder, double rightborder, int fitBorder, const char * temp, bool module, bool rocbool, int roc){	
	
	int intdir = chdir("out");
	//int intdir;
	char pfad[256];
	const char * GETCWD;
	GETCWD = getcwd(pfad, 256);
	if(GETCWD == NULL) {}
	cout << "Pfad getTH1 " << pfad << endl;
	
	double maxpeak, maxpeakerror; 
	int nrEntries, nrTrigger;
	int nrchips = 0;
	
	//GRAPHICAL OUTPUT
	TCanvas *c2 = new TCanvas("c2","Roentgenpeak Gauss",10,10,1500,1000);
	c2->SetLeftMargin(0.15);
	gPad->SetLeftMargin(0.17);
	gPad->SetBottomMargin(0.17);
	c2->SetFillColor(0);
	c2->SetBorderMode(0);	
	
	int chipnr = 0;
	if(module == false) {		//Analyze only single chip assembly
		nrchips = 1;
	}
	if(module and rocbool) {	//Analyze only single roc
		chipnr = roc;
		nrchips = roc+1;
	}
	else {					//Analyze whole module
		nrchips = 16;
	}
	
	//TH1 *histo[16], *hDummy;
	TH1 *histo, *hDummy;
	TFile *fr = new TFile(rootfile, "READ");
	fr->Cd("Xray");
	cout << "Rootfile " << rootfile << endl;
		
	for(; chipnr < nrchips; chipnr++) {
		
		Char_t *histoname = Form("q_%s_C%i_V0",Source.Data(), chipnr);	//Build name of histogram with Source and chip number
		gDirectory->GetObject(histoname, hDummy);
		if (hDummy != 0) {
			//histo[chipnr] = (TH1*)hDummy->Clone("histo");
			//~ histo[chipnr] = (TH1*)fr->Get(histoname);
			//~ //histo->SetDirectory(0);
			//~ //histo->Draw("colz");
			//~ histo[chipnr]->GetYaxis()->SetTitleOffset(1.3);
			//~ histo[chipnr]->GetYaxis()->SetTitleSize(0.05);
			//~ histo[chipnr]->GetYaxis()->SetTitle("# Events");
			//~ histo[chipnr]->GetXaxis()->SetTitleSize(0.05);
			//~ histo[chipnr]->GetXaxis()->SetTitle("VcalDAC");
			
			histo = (TH1*)fr->Get(histoname);
			//histo->SetDirectory(0);
			//histo->Draw("colz");
			histo->GetYaxis()->SetTitleOffset(1.3);
			histo->GetYaxis()->SetTitleSize(0.05);
			histo->GetYaxis()->SetTitle("# Events");
			histo->GetXaxis()->SetTitleSize(0.05);
			histo->GetXaxis()->SetTitle("VcalDAC");
			cout << "Histogramm " << histoname << " eingelesen" << endl;
		}
		else {
			cout << "Histogrammname ueberpruefen" << endl;
			//histo = NULL;
		}
	}
	intdir = chdir("../");
	cout << "Einlesen von Datei " << rootfile << " beendet " << endl << endl << endl << endl;
	
	
	//~ chipnr = 0;
	//~ if(module == false) {		//Analyze only single chip assembly
		//~ nrchips = 1;
	//~ }
	//~ if(module and rocbool) {	//Analyze only single roc
		//~ chipnr = roc;
		//~ nrchips = roc+1;
	//~ }
	//~ else {					//Analyze whole module
		//~ nrchips = 16;
	//~ }
	//~ 
	//~ for(; chipnr < nrchips; chipnr++) {
		//~ Char_t *histoname = Form("q_%s_C%i_V0",Source.Data(), chipnr);	//Build name of histogram with Source and chip number
		//~ 
		//~ //zooming the x-axis
		//~ TAxis *axis = histo[chipnr]->GetXaxis();
		//~ //   axis->SetRangeUser(50,220);  
		//~ //   axis->SetRangeUser(50,350);  
		//~ //	 axis->SetRangeUser(140,180);  	
		//~ axis->SetRangeUser(0,peak+100);
		//~ 
		//~ 
		//~ const char * comment;
		//~ comment = new char[500];
		//~ int maxbin = histo[chipnr]->GetMaximumBin();
		//~ if(maxbin > peak - 25 and maxbin < peak + 25) {
			//~ leftborder = maxbin - fitBorder;
			//~ rightborder = maxbin + fitBorder;
			//~ comment = " ";
		//~ }
		//~ else{
			//~ cout << "Fit aorund expected peak value and NOT maximum bin!" << endl;
			//~ comment = "Fit around expected peak value and NOT maximum bin!";
		//~ }
				//~ 
		//~ //Begin Fit
		//~ histo[chipnr]->Fit("gaus","L","",leftborder,rightborder);
		//~ TF1 *Fit = histo[chipnr]->GetFunction("gaus");
		//~ Fit->SetLineWidth(1);
		//~ Fit->SetLineStyle(1);
		//~ gStyle->SetOptFit(111111); 
		//~ maxpeak = Fit->GetParameter(1); 						//Mean
		//~ maxpeakerror = Fit->GetParError(1);						//Error on mean
		//~ nrEntries = histo[chipnr]->GetEntries();				//Number of entries
		//~ nrTrigger = nrTriggers(rootfile, Source);				//Number of triggers
		//~ double lit = findsourcelit(Source);					//Expected number of electrons (NIST)
		//~ double rate = calculateRate(nrEntries, nrTrigger);		//Calculated rate: Rate = Hits / Trigger / 25 ns / 0,6561 cm² with zero masked pixels
		//~ histo[chipnr]->Draw();
		//~ 
		//~ intdir = chdir("results/");
		//~ //const char * outputtitle;
		//~ //outputtitle = new char[250];
		//~ Char_t *outputtitle = Form("C%i-Analysis-GaussFit.txt", chipnr);
		//~ //	char outputtitle[250];
		//~ //outputtitle = "Analysis-GaussFit.txt";
		//Save data in .txt
		//~ std::ifstream FileTest(outputtitle);
		//~ if(!FileTest) {
			//~ //Write header in output document
			//~ ofstream outputfile;
			//~ outputfile.open(outputtitle, ios::out);
			//~ outputfile << "//Output from main.cc, Automatisierte Spektrenauswertung fuer chip ";
			//~ outputfile << chipnr << " \n";
			//~ outputfile << "//Source\tTemperature\t Current (mA)\tPeak (Vcal)\tErrorPeak (Vcal)\t#electrons(expected)\tLeftFitBorder\tRightFitBorder\t#Events\t#Triggers\tRate (Hz/cm^3)\tFile\tComment\n";
			//~ outputfile.close();
		//~ }
		//~ else{
			//~ deleteLinesTxt(outputtitle, rootfile);
		//~ }
		//~ //Save measurement as .txt
		//~ ofstream outputfile;
		//~ outputfile.open(outputtitle, ios::out | ios::app);
		//~ outputfile << Source.Data() << "\t" << temp << "\t" << actualCurrent << "\t" << maxpeak << "\t" << maxpeakerror << "\t" << lit << "\t";
		//~ outputfile << leftborder << "\t" << rightborder << "\t" << nrEntries << "\t" << nrTrigger << "\t" << rate << "\t"; 
		//~ outputfile << rootfile.Data() << "\t" << comment << "\t";
		//~ outputfile << "\n";
		//~ outputfile.close();
		//~ 
		//~ //Save measurement as .root
		//~ TString Substring;
		//~ Substring = TString(rootfile(0, rootfile.Length()-5));
		//~ Char_t * outputhisto = Form("%s-%s-Histo", Substring.Data(), histoname);
		//~ Char_t * outputhistodelete = Form("%s-%s-Histo;1", Substring.Data(), histoname);
		//~ TFile *savehisto = new TFile("Analysis-GaussFit-MoReWebFit.root", "UPDATE");
		//~ savehisto->Delete(outputhistodelete);
		//~ histo[chipnr]->Write(outputhisto);
		//~ Char_t * outputfit = Form("%s-%s-GaussFit", Substring.Data(), histoname);
		//~ Char_t * outputfitdelete = Form("%s-%s-GaussFit;1", Substring.Data(), histoname);
		//~ savehisto->Delete(outputfitdelete);
		//~ Fit->Write(outputfit);
		//~ savehisto->Close();
		//~ savehisto->Delete();
		//~ 
		//~ //cout << "Single Gaussian Chi Quadrat " << endl;
		//~ //cout << "Chi Quadrat " << Fit->GetChisquare()/Fit->GetNDF() << endl;
		//~ //cout << Fit->GetNDF() << endl;
		//~ //cout << "Chi Quadrat 2 " << histo->Chisquare(Fit)/Fit->GetNDF() << endl;
		//~ intdir = chdir("../");
		//~ 
		//~ 
		//~ intdir = chdir("results");		
		//~ if(module) {
			//~ Char_t *subdirectory = Form("C%i", chipnr);
			//~ intdir = chdir(subdirectory);
		//~ }
	//~ 
		//~ Substring = TString(rootfile(0, rootfile.Length()-5));
		//~ Char_t *outputpdf = Form("%s-C%i-SingleGauss.pdf", Substring.Data(), chipnr);
		//~ c2->SaveAs(outputpdf);
		//~ Char_t *outputpng = Form("%s-C%i-SingleGauss.png", Substring.Data(), chipnr);
		//~ c2->SaveAs(outputpng);
		//~ c2->Clear();
		//~ if(module) {
			//~ intdir = chdir("../");
		//~ }
		//~ intdir = chdir("../");
		//~ 
		//~ //intdir = chdir("../");
		//~ GETCWD = getcwd(pfad, 256);
		//~ cout << pfad << endl;
		//~ c2->Clear();
	//~ }
	fr->Close();
}

//--------------------------------------------------------------------------------------------------------------------
double * defaultPeakPosition(TString Source, int fitBorder) {
	
	double * defaultParameter;
	defaultParameter = new double[3];	//[0] = position of peak, [1] = left fit border, [2] = right fit border
	if(strstr(Source.Data(), "Fe") != NULL) {
		defaultParameter[0] = 50;	//+-25 cause of variations between the used single chip assemblies
	}
	if(strstr(Source.Data(), "Cu") != NULL) {
		defaultParameter[0] = 55;	//+-25 cause of variations between the used single chip assemblies
	}
	if(strstr(Source.Data(), "Zn") != NULL) {
		defaultParameter[0] = 59;	//+-25 cause of variations between the used single chip assemblies
	}
	if(strstr(Source.Data(), "Mo") != NULL) {
		defaultParameter[0] = 114;	//+-25 cause of variations between the used single chip assemblies
	}
	if(strstr(Source.Data(), "Ag") != NULL) {
		defaultParameter[0] = 146;	//+-25 cause of variations between the used single chip assemblies
	}
	if(strstr(Source.Data(), "In") != NULL) {
		defaultParameter[0] = 160;	//+-25 cause of variations between the used single chip assemblies
	}
	if(strstr(Source.Data(), "Sn") != NULL) {
		defaultParameter[0] = 167;	//+-25 cause of variations between the used single chip assemblies
	}
	if(strstr(Source.Data(), "Nd") != NULL) {
		defaultParameter[0] = 220;	//+-25 cause of variations between the used single chip assemblies
	}
	defaultParameter[1] = defaultParameter[0] - fitBorder;
	defaultParameter[2] = defaultParameter[0] + fitBorder;
	if(strstr(Source.Data(), "Nd") != NULL) {
		if(fitBorder != 20) {
			defaultParameter[1] = defaultParameter[0] - fitBorder - 10;
			defaultParameter[2] = defaultParameter[0] + fitBorder + 10;
		}
	}
	
	return defaultParameter;
}

//--------------------------------------------------------------------------------------------------------------------
int nrTriggers(TString rootfile, TString Source) {
	TFile *fr = new TFile(rootfile);
	int nrTrigger = 0;
	TH1 *histo2, *hDummy2;
	fr->Cd("Xray");
	Char_t *histoname2 = Form("ntrig_%s_V0", Source.Data());
	gDirectory->GetObject(histoname2, hDummy2);
	if(hDummy2 != 0) {
		histo2 = (TH1*)hDummy2->Clone("histo2");
		nrTrigger = histo2->GetBinContent(1);
	}
	else{
		cout << "Histogrammname ueberpruefen" << endl;
	}
	return (nrTrigger);
}

//--------------------------------------------------------------------------------------------------------------------
double findsourcelit(TString Source) {
	if(strstr(Source, "Ag") != NULL) return(22162.99/3.6);
	if(strstr(Source, "Zn") != NULL) return(8639.10/3.6);
	if(strstr(Source, "Cu") != NULL) return(8048.11/3.6);
	if(strstr(Source, "Fe") != NULL) return(6403.13/3.6);
	if(strstr(Source, "Mo") != NULL) return(17479.10/3.6);
	if(strstr(Source, "In") != NULL) return(24209.78/3.6);
	if(strstr(Source, "Sn") != NULL) return(25271.34/3.6);
	if(strstr(Source, "Nd") != NULL) return(37361.4/3.6);
	else return(-1);
}	

//--------------------------------------------------------------------------------------------------------------------
void deleteLinesTxt(const char * outputtitle, TString rootfile) {
	
	fstream fin, fout;
	char linecont[1000];
	
	fin.open(outputtitle, ios::in);
	const char * outputtmp;
	outputtmp = new char[255];
	outputtmp = Form("%s.tmp", outputtitle);
	fout.open(outputtmp, ios::out);
	
	//cout << "Rootfile " <<  rootfile << endl;
	
	while(fin.getline(linecont, 1000)) {
		if(strstr(linecont, rootfile.Data()) == NULL) {
			// cout << linecont << endl;
			fout << linecont << endl;
		}
	}
	fin.close();
	fout.close();
	remove(outputtitle);
	rename(outputtmp, outputtitle);
	
}

//--------------------------------------------------------------------------------------------------------------------
double pixelSurface(char * keyword, int col, int row) {
	
	double pxSURFACEcm3;
	if(strstr(keyword, "col") != NULL) {
		if(col == 0 or col == 51) {
			pxSURFACEcm3 = 79*(300*0.000001)*(100*0.000001) + (300*0.000001)*(200*0.000001);
		}
		else {
			pxSURFACEcm3 = 79*(150*0.000001)*(100*0.000001) + (150*0.000001)*(200*0.000001);
		}
	}
	if(strstr(keyword, "row") != NULL) {
		if(row == 79) {
			pxSURFACEcm3 = 50*(150*0.000001)*(200*0.000001) + 2*(300*0.000001)*(200*0.000001);
		}
		else {
			pxSURFACEcm3 = 50*(150*0.000001)*(100*0.000001) + 2*(300*0.000001)*(100*0.000001);
		}
	}
	else {
		if(col == 0 and row == 79) {pxSURFACEcm3 = (200*0.000001)*(300*0.000001);}
		if(col == 0 and row != 79) {pxSURFACEcm3 = (100*0.000001)*(300*0.000001);}
		if(col == 51 and row == 79) {pxSURFACEcm3 = (200*0.000001)*(300*0.000001);}
		if(col == 51 and row != 79) {pxSURFACEcm3 = (100*0.000001)*(300*0.000001);}
		if(row == 79 and col != 0) {pxSURFACEcm3 = (200*0.000001)*(150*0.000001);}
		else {pxSURFACEcm3 = (100*0.000001)*(150*0.000001);}
	}
	return(pxSURFACEcm3);
}

//--------------------------------------------------------------------------------------------------------------------
double calculateRate(int nrEntries, int nrTrigger) {
	
	fstream fin;
	fin.open("defaultMaskFile.dat", ios::in);
	char linecont[256], keyword[4];
	int rocNr, col, row;
	double SURFACEcm3 = 0.6561;
	
	while(fin >> keyword) {
		if(strstr(keyword, "#") != NULL) {
			fin.getline(linecont, 256);
		}
		else {
			fin >> rocNr;
			if(strstr(keyword, "pix") != NULL) {
				fin >> col >> row;
				//cout << keyword << " " << rocNr << " " << col << " " << row << endl;
			}
			if(strstr(keyword, "row") != NULL) {
				fin >> row;
				col = -1;
				//cout << keyword << " " << rocNr << " " << row << endl;
			}
			if(strstr(keyword, "col") != NULL) {
				fin >> col;
				row = -1;
				//cout << keyword << " " << rocNr << " " << col << endl;
			}
			fin.getline(linecont, 256);
		}
		SURFACEcm3 = SURFACEcm3 - pixelSurface(keyword, col, row);
	}
	fin.close();
	
	double rate = (1.0*nrEntries)/nrTrigger/(25*pow(10,-9))/SURFACEcm3;
	
	return(rate);
}

