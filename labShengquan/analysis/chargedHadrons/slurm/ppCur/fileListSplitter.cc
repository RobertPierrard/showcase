#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
using namespace std;

void fileListSplitter(string fileList){
  // Open file list text file
  ifstream files;
  files.open(fileList);
  string sBuffer; //line buffer
  
  // Create output files
  int numSplits = 50;
  ofstream outFile[numSplits];
  for(int k = 0; k < numSplits; ++k){
    outFile[k].open(Form("jobs/jb%d/rootFiles_jb%d.list",k,k));
  }

  // Write to output files with filenames from input filelist
  int fileCounter = 0;
  while(files >> sBuffer){
    outFile[fileCounter] << sBuffer << endl;
    if(fileCounter < (numSplits-1)){
      fileCounter++;
    }else{
      fileCounter = 0;
    }
  }
  
  // Close files
  files.close();
  for(int k = 0; k < numSplits; ++k){
    outFile[k].close();
  }
}
