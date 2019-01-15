#include "bode.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iomanip>

using namespace std;

void bodeFormatter() {
	string filename;
	
	cout << "Enter filename (.txt): ";
	cin >> filename;

	string savefilename;
	cout << "Enter filename to save to (.csv): ";
	cin >> savefilename;
	
	ifstream ifile(filename);
	if (ifile.fail()) {
		cout << "File opening failed. Exiting." << endl;
		exit(1);
	}
	ofstream saveFile(savefilename);
	if (saveFile.fail()) {
		cout << "Output file creation failed." << endl;
		exit(2);
	}

	saveFile << "freq,gain,phase" << endl;

	string line;
	getline(ifile, line);
	getline(ifile, line);

	int countSamples = -2;
	while (!ifile.eof()) {
		getline(ifile, line);
		if (line[0] >= '0' && line[0] <= '9') {
			for (int j = 0; j <= line.length(); ++j) {
				if (line[j] == ' ' && line[j+1] == ' ') {
					line.erase(j + 1, 1);
					--j;
				}
			}

			string freq, gain, phase;

			int it = 0;
			while (line[it] != ' ') {
				freq.push_back(line[it]);
				++it;
			}
			++it;
			while (line[it] != ' ') {
				gain.push_back(line[it]);
				++it;
			}
			++it;
			while (it < line.length()) {
				phase.push_back(line[it]);
				++it;
			}

			for (int i = 0; i < freq.length(); ++i) {
				if (freq[i] == ',') {
					freq[i] = '.';
				}
			}
			for (int i = 0; i < gain.length(); ++i) {
				if (gain[i] == ',') {
					gain[i] = '.';
				}
			}
			for (int i = 0; i < phase.length(); ++i) {
				if (phase[i] == ',') {
					phase[i] = '.';
				}
			}
			saveFile << freq << "," << gain << "," << phase << endl;
		}
		++countSamples;
	}
	cout << "Sucessfully formatted " << countSamples << " data points." << endl;

	saveFile.close();
	ifile.close();
}