//Creates .txt files to plot energy kalibration lines of analyzed spectra with GaussFitCurrent.C
//Please change the settings in the config file "EnergycalibrationCurrent.config"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std;

void EnergycalibrationCurrent(){
	
	char linecont[500], Spectrum[256], outputbegin[256], Source[2];
	fstream fc,df; 
	int n;
	int current, leftbin, rightbin, currenttest;
	double peakposition, peakpositiontest;
	
	//read config file
	fc.open("EnergycalibrationCurrent.config", ios::in);
	fc.getline(linecont, 500);
	cout << linecont << endl;
	fc.getline(linecont, 500);
	cout << linecont;
	fc >> n;
	cout << ": " << n << endl; 
	fc.getline(linecont, 500);
	fc.getline(linecont, 500);
	cout << linecont << endl;
	fc.getline(outputbegin, 256);
	cout << "Outputbegin " << outputbegin << endl;
	fc.getline(linecont, 500);
	//cout << linecont << endl;
	
	for(int i = 0; i < n; i++) {
		cout << "------------------------------------------------------------------" << endl;
		char inputfile[256];
		fc.getline(Source, 3);
		cout << "Source " << Source << endl;
		fc.getline(inputfile, 256);
		cout << "Analysing file " << inputfile << endl;
		
		df.open(inputfile, ios::in);
		df.getline(linecont, 500);
		cout << linecont << endl;
		df.getline(linecont, 500);
		cout << linecont << endl;
		int zaehler = 0;
		while(df) {
			df >> current >> peakposition >> leftbin >> rightbin;
			df.getline(Spectrum, 256);
			if(zaehler == 0) {zaehler = 1;}
			if(zaehler == 1) {
				if(currenttest == current) {
					if(peakpositiontest == peakposition) break;
				}
			}
			cout << current << "\t" << peakposition << "\t" << leftbin << "\t" << rightbin;
			cout << Spectrum << endl;
			
			//char outputtitle[256];
			Char_t *outputtitle = Form("%s%dmA-Calibration-Gauss-single.txt", outputbegin,current);
			//Pruefen, ob Zieldatei schon vorhanden
			std::ifstream FileTest(outputtitle);
			if(!FileTest) {
				//Write header in output document
				ofstream outputfile;
				outputfile.open(outputtitle, ios::out);
				outputfile << "#Output from EnergiekalibrationStrom.C, PeakPosition und erwartete Elektronenanzahl aus Strommessung bei ";
				outputfile << current;
				outputfile << "mA \n";
				outputfile << "#Measurement (Vcal)\tExpectation (Electrons)\tLeft Bin\tRight Bin\tSpectrum\n";
				outputfile.close();
			}
			double lit = findsourcelit(Source);
			if (lit == -1){ 
				cout << "Source not identified" << endl;
				break;
			}
			
			currenttest = current;
			peakpositiontest = peakposition;
			
			//Generate output
			ofstream outputfile;
			outputfile.open(outputtitle, ios::out | ios::app);
			outputfile << peakposition;
			outputfile << "\t";
			outputfile << lit;
			outputfile << "\t";
			outputfile << leftbin;
			outputfile << "\t";
			outputfile << rightbin;
			outputfile << "\t";
			outputfile << Spectrum;
			outputfile << "\n";
			outputfile.close();		
			
		}
		df.close();
	}
	fc.close();
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
