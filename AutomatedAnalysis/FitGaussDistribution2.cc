#include <iostream>
#include <stdio.h>
#include <dirent.h> 
#include <string>

#include"TCanvas.h"

using namespace std;


void gebeAlleDateienAus(char directory[ ], char endung[ ] )
{
    DIR *hdir;
    struct dirent *entry;

    hdir = opendir(directory);
    do
    {
        entry = readdir(hdir);
        if (entry)
        {
			if ( strstr(entry->d_name, endung) != NULL )	// Nur Dateien anzeigen, die .txt im Dateinamen haben
			{
				cout << entry->d_name << endl;
			}
            //cout << entry->d_name << endl;
        }
    } while (entry);
    closedir(hdir);

}

TString * actualRootFile(DIR *datadir, int * n)
{
    struct dirent *entry;
    char * filename;
    filename = new char[200];
    TString * RootFiles;
    RootFiles = new TString[200];
    
    int i = 0;
    do
    {
        entry = readdir(datadir);
        if (entry)
        {
			if ( strstr(entry->d_name, ".root") != NULL )	// Nur Dateien anzeigen, die .txt im Dateinamen haben
			{
				RootFiles[i] = entry->d_name;
				cout << i << " " << RootFiles[i] << endl;
				i++;
			}
        }
    } while (entry);
    
    *n = i-1;
    return RootFiles;
}



char * actualRootFileName(DIR *datadir) {
	char * filename;
	filename = new char[200];
	//DIR *datadir;
	struct dirent *entry;
	
	std::string point (".");
	std::string points ("..");
	
	entry = readdir(datadir);
	filename = entry->d_name;
	if(point.compare(string(filename)) == 0 or points.compare(string(filename)) == 0) {
		entry = readdir(datadir);
		filename = entry->d_name;
	}
	if(strstr(entry->d_name, ".root") == NULL) {
		entry = readdir(datadir);
		filename = entry->d_name;
	}
	if(not entry){
		filename = "end";
		return filename;
	}
	//cout << filename << endl;
	return filename;
}



int main(){
	//gebeAlleDateienAus("out", ".txt");
	
	char *filename;
	filename = new char[200];
	DIR *datadir;
    //struct dirent *entry;
    std::string point (".");
    std::string points ("..");
    std::string end ("end");
    
    int n;
    datadir = opendir("out");
    
/*    filename = actualRootFileName(datadir);
    while(end.compare(string(filename))) {
		cout << filename << endl;
		filename = actualRootFileName(datadir);
	}
	
	if(strstr("Test.log", ".root") == NULL) {
		cout << "Null " << endl;
	}
	cout << strstr("Test.log", ".root") << endl;
*/	
	
/*    
    entry = readdir(datadir);
    while (entry) {
		filename = entry->d_name;
		if(point.compare(string(filename)) == 0 or points.compare(string(filename)) == 0) {
			entry = readdir(datadir);
			filename = entry->d_name;
		}
		cout << filename << endl;
		entry = readdir(datadir);
	 }
*/    
    
/*   
    
    do
    {
		//filename = actualFileName(datadir);
		//zaehler++;
		filename = entry->d_name;
		cout << filename << endl;
        entry = readdir(datadir);
        if (entry)
        {
			filename = entry->d_name;
			cout << filename << endl;
			//cout << str1.compare(string(test));
			//cout << typeid(string(test)).name();
			//cout << test[0] << endl;
/*            if(point.compare(string(filename)) != 0){
				if(points.compare(string(filename)) != 0) {
					cout << entry->d_name << endl;
				}
*/
//		}
//   } while(true);
//    closedir(datadir);

	TString * RootFiles;
	RootFiles = new TString[200];
	
	RootFiles = actualRootFile(datadir, &n);
	
	closedir(datadir);
    
	return 0;
}
