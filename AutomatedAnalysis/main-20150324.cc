#include <iostream>
#include <stdio.h>
#include <dirent.h> 
#include <string>
#include <fstream>
#include <sys/stat.h>

#include "TCanvas.h"
#include "TH1.h"
#include "TFile.h"
#include "TF1.h"
#include "TStyle.h"

using namespace std;

TString * listofRootFiles(DIR *, int *, const char *);
TString * identifySource(TString, TString *, const int *);
int identifyCurrent(TString, TString *, int *, const int *);
TH1 * getTH1(TString, TString, int, double, double, double, int);
double * defaultPeakPosition(TString, int);
int nrTriggers(TString, TString);
double findsourcelit(TString);

int main( int argc, char *argv[] ){
	
	//First argument: Filter condition for analyzing FileNames
	//Second argument: Mean of peak (int)
	//Third argument: Fit Border x as mean +- fitBorder (int)
	
	int intTest = mkdir("results/", 0777);
	 
	long int argv2 = strtol(argv[2], NULL, 0);
	long int argv3 = strtol(argv[3], NULL, 0);
	
	DIR *datadir;
	TString * RootFiles;
	RootFiles = new TString[200];
    //struct dirent *entry;
    const int numbersources = 8;
    int fitBorder = 20;
    if(argc == 4) {
		fitBorder = int(argv3);
	}
    
    //Variables to determine Source
    TString *Sources;
	Sources = new TString[numbersources];
	Sources[0] = "_Fe"; Sources[1] = "_Cu", Sources[2] = "_Zn", Sources[3] = "_Mo";
	Sources[4] = "_Ag", Sources[5] = "_In"; Sources[6] = "_Sn", Sources[7] = "_Nd";
	
	//Variables to determine Current
	int * Currents;
    Currents = new int[8];
    for(int i = 0; i < 8; i++) {
		Currents[i] = 2+4*i;		//Currents from 2 to 30 mA with distance 4mA
	}
    const int numbercurrents = 8;
    TString *CurrentString;
    CurrentString = new TString[numbercurrents];
    const char * searchOption;
    searchOption = new char[50];
    for(int i = 0; i < numbercurrents; i++) {
		CurrentString[i] = Form("_%imA", Currents[i]);
		//cout << CurrentString[i] << endl;
	}
	if(argc == 4) {
		searchOption = argv[1];
	}
	else {
		searchOption = "Spectrum_";		//Standard option to open files that include this string
	}
	
	int n;
	datadir = opendir("out");
	RootFiles = listofRootFiles(datadir, &n, searchOption);		//Read in the root-Files in the directory "out"
	closedir(datadir);
	
	TCanvas *c2 = new TCanvas("c2","Roentgenpeak Gauss",10,10,1500,1000);
	c2->SetLeftMargin(0.15);
	gPad->SetLeftMargin(0.17);
	gPad->SetBottomMargin(0.17);
	c2->SetFillColor(0);
	c2->SetBorderMode(0);
	
	TH1 * histo;
	TString * actualSource;
	actualSource = new TString[1];
	int actualCurrent;
	for(int i = 0; i < n; i++) {
		//cout << RootFiles[i] << endl;
		actualSource = identifySource(RootFiles[i], Sources, &numbersources);
		actualCurrent = identifyCurrent(RootFiles[i], CurrentString, Currents, &numbercurrents);
		if(strstr(actualSource[0], "FALSE") == NULL) {
			if(actualCurrent != -1) {
				cout << RootFiles[i] << "\t" << actualSource[0] << "\t" << actualCurrent << endl;
				
				double * fitParameter;
				fitParameter = new double[3];
				fitParameter = defaultPeakPosition(actualSource[0], fitBorder);
				if(argc == 4) {
					fitParameter[0] = int(argv2);
					fitParameter[1] = fitParameter[0] - fitBorder;
					fitParameter[2] = fitParameter[0] + fitBorder;
				}
				
				histo = getTH1(RootFiles[i], actualSource[0], actualCurrent, fitParameter[0], fitParameter[1], fitParameter[2], fitBorder);
				histo->Draw();
				 
				intTest = chdir("results");		
				if(intTest == 1) {}			//this line is only to get rid of the warning, that intdir is initialized but not used
				
				TString Substring;
				Substring = TString(RootFiles[i](0, RootFiles[i].Length()-5));
				Char_t *outputpdf = Form("%s.pdf", Substring.Data());
				c2->SaveAs(outputpdf);
				Char_t *outputpng = Form("%s.png", Substring.Data());
				c2->SaveAs(outputpng);
				c2->Clear();
				chdir("../out");
			}
		}
		//chdir("../");
	}
	//~ actualSource = identifySource(RootFiles[0], Sources, &numbersources);
	//~ actualCurrent = identifyCurrent(RootFiles[0], CurrentString, Currents, &numbercurrents);
	//~ if(strstr(actualSource[0], "False") == NULL) {
		//~ if(actualCurrent != -1) {
			//~ cout << RootFiles[0] << "\t" << actualSource[0] << "\t" << actualCurrent << endl;
		//~ }
	//~ }
	
	//~ TCanvas *c2 = new TCanvas("c2","Roentgenpeak Gauss",10,10,1500,1000);
	//~ c2->SetLeftMargin(0.15);
	//~ gPad->SetLeftMargin(0.17);
	//~ gPad->SetBottomMargin(0.17);
	//~ c2->SetFillColor(0);
	//~ c2->SetBorderMode(0);
	//~ 
	//~ double * fitParameter;
	//~ fitParameter = new double[3];
	//~ fitParameter = defaultPeakPosition(actualSource[0], fitBorder);
	//~ if(argc == 4) {
		//~ fitParameter[0] = int(argv2);
		//~ fitParameter[1] = fitParameter[0] - fitBorder;
		//~ fitParameter[2] = fitParameter[0] + fitBorder;
	//~ }
	//~ 
	//~ TH1 * histo;
	//~ histo = getTH1(RootFiles[0], actualSource[0], actualCurrent, fitParameter[0], fitParameter[1], fitParameter[2], fitBorder);
	//~ histo->Draw();
	 //~ 
	//~ intTest = chdir("results");		
	//~ if(intTest == 1) {}			//this line is only to get rid of the warning, that intdir is initialized but not used
	//~ 
	//~ TString Substring;
	//~ Substring = TString(RootFiles[0](0, RootFiles[0].Length()-5));
	//~ Char_t *outputpdf = Form("%s.pdf", Substring.Data());
	//~ c2->SaveAs(outputpdf);
	//~ Char_t *outputpng = Form("%s.png", Substring.Data());
	//~ c2->SaveAs(outputpng);
	//~ 
	return 0;
}


//####################################################################################################################
//#Definition of the functions                                                                                       #
//####################################################################################################################

TString * listofRootFiles(DIR *datadir, int * n, const char * searchOption)
{
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
int identifyCurrent(TString RootFile, TString * CurrentString, int * Currents , const int * numbercurrents) {
	int j = 0;
	while (j < *numbercurrents) {		//search for Current
		if (strstr(RootFile, CurrentString[j]) != NULL) {break;}
		j++;
	}
	if(j == *numbercurrents) {		//Current not found
		//cout << "if " << endl;
		return(-1);
	}
	return Currents[j];
}

//--------------------------------------------------------------------------------------------------------------------

//Definition of the function for importing and analysing the spectra
TH1 *getTH1(TString rootfile, TString Source, int actualCurrent, double peak, double leftborder, double rightborder, int fitBorder){	
	
	int intdir = chdir("out");
	char pfad[256];
	getcwd(pfad, 256);
	cout << pfad << endl;
	TFile *fr = new TFile(rootfile);
	TH1 *histo, *hDummy;
	fr->Cd("Xray");
	double maxpeak, maxpeakerror; 
	int nrEntries, nrTrigger;
	
	Char_t *histoname = Form("q_%s_C0_V0",Source.Data());	//Build name of histogram with Source
	cout << histoname << endl;

	gDirectory->GetObject(histoname, hDummy);
	if (hDummy != 0) {
		histo = (TH1*)hDummy->Clone("histo");
		//histo->SetDirectory(0);
		//histo->Draw("colz");
		histo->GetYaxis()->SetTitleOffset(1.3);
		histo->GetYaxis()->SetTitleSize(0.05);
		histo->GetYaxis()->SetTitle("# Events");
		histo->GetXaxis()->SetTitleSize(0.05);
		histo->GetXaxis()->SetTitle("VcalDAC");
	
		//zooming the x-axis
		TAxis *axis = histo->GetXaxis();
		//   axis->SetRangeUser(50,220);  
		//   axis->SetRangeUser(50,350);  
		axis->SetRangeUser(0,350);  	
		
		
		const char * comment;
		comment = new char[500];
		int maxbin = histo->GetMaximumBin();
		if(maxbin > peak - 25 and maxbin < peak + 25) {
			leftborder = maxbin - fitBorder;
			rightborder = maxbin + fitBorder;
			comment = " ";
		}
		else{
			cout << "Fit aorund expected peak value and NOT maximum bin!" << endl;
			comment = "Fit around expected peak value and NOT maximum bin!";
		}
						
		//Begin Fit
		histo->Fit("gaus","L","",leftborder,rightborder);
		TF1 *Fit = histo->GetFunction("gaus");
		Fit->SetLineWidth(1);
		Fit->SetLineStyle(1);
		gStyle->SetOptFit(111111); 
		maxpeak = Fit->GetParameter(1); 						//Mean
		maxpeakerror = Fit->GetParError(1);						//Error on mean
		nrEntries = histo->GetEntries();						//Number of entries
		nrTrigger = nrTriggers(rootfile, Source);				//Number of triggers
		double lit = findsourcelit(Source);					//Expected number of electrons (NIST)
		
		intdir = chdir("../results/");
		const char * outputtitle;
		outputtitle = new char[250];
//		char outputtitle[250];
		outputtitle = "Analysis-GaussFit.txt";
		//~ //Save data in .txt
		std::ifstream FileTest(outputtitle);
		if(!FileTest) {
			//Write header in output document
			ofstream outputfile;
			outputfile.open(outputtitle, ios::out);
			outputfile << "//Output from main.cc, Automatisierte Spektrenauswertung \n";
			outputfile << "//Source \t Current (mA)\tPeak (Vcal)\tErrorPeak (Vcal)\t#Electrons(expected)\tLeftFitBorder\tRightFitBorder\t#Events\t#Triggers\tRate (Hz/cm^3)\tFile\tComment\n";
			outputfile.close();
		}
		//Save measurement as .txt
		ofstream outputfile;
		outputfile.open(outputtitle, ios::out | ios::app);
		outputfile << Source.Data() << "\t" << actualCurrent << "\t" << maxpeak << "\t" << maxpeakerror << "\t" << lit << "\t";
		outputfile << leftborder << "\t" << rightborder << "\t" << nrEntries << "\t" << nrTrigger << "\t" << rootfile.Data() << "\t";
		outputfile << comment << "\t";
		outputfile << "\n";
		outputfile.close();
		if(intdir == 0) {} 	//this line is only to get rid of the warning, that intdir is initialized but not used
		
		//Save measurement as .root
		TString Substring;
		Substring = TString(rootfile(0, rootfile.Length()-5));
		Char_t * outputhisto = Form("%s-Histo", Substring.Data());
		Char_t * outputhistodelete = Form("%s-Histo;1", Substring.Data());
		TFile *savehisto = new TFile("Analysis-GaussFit.root", "UPDATE");
		savehisto->Delete(outputhistodelete);
		histo->Write(outputhisto);
		Char_t * outputfit = Form("%s-GaussFit", Substring.Data());
		Char_t * outputfitdelete = Form("%s-GaussFit;1", Substring.Data());
		savehisto->Delete(outputfitdelete);
		Fit->Write(outputfit);
		savehisto->Close();
		
		//intdir = chdir("../out");
	}
	else { 
		cout << "Histogrammname ueberpruefen" << endl;
		histo = NULL;
	}
	intdir = chdir("../");
	getcwd(pfad, 256);
	cout << pfad << endl;
	
	return (histo);
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
		defaultParameter[0] = 250;	//+-25 cause of variations between the used single chip assemblies
	}
	defaultParameter[1] = defaultParameter[0] - fitBorder;
	defaultParameter[2] = defaultParameter[0] + fitBorder;
	
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
	if(strstr(Source, "Zn")== 0) return(8639.10/3.6);
	if(strstr(Source, "Cu")== 0) return(8048.11/3.6);
	if(strcmp(Source, "Fe")== 0) return(6403.13/3.6);
	if(strcmp(Source, "Mo")== 0) return(17479.10/3.6);
	if(strcmp(Source, "In")== 0) return(24209.78/3.6);
	if(strcmp(Source, "Sn")== 0) return(25271.34/3.6);
	if(strcmp(Source, "Nd")== 0) return(37361.4/3.6);
	else return(-1);
}	
