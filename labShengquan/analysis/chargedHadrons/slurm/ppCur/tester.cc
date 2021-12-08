#include <string>

void tester(string rootFilelistFilename){
     string outFileName;
     outFileName = outFileName.append(rootFilelistFilename.substr(0,rootFilelistFilename.length()-5));
     outFileName.append("_outputHistPp.root");
     cout << outFileName << endl;
}
