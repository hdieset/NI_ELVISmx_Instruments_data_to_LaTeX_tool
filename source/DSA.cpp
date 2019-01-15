#include "DSA.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iomanip>
#include "oscilloscope.h"

using namespace std;

struct numExp {
	long double number = 0.0;
	int exponent = 0;
	int precision = 0;
};

void DSA_formatter() {
	string filename;

	cout << "Enter filename (.txt): ";
	cin >> filename;

	string savefilename;
	cout << "Enter filename to save to (no file extension): ";
	cin >> savefilename;

	ifstream ifile(filename);
	if (ifile.fail()) {
		cout << "File opening failed. Exiting." << endl;
		exit(1);
	}

	string savefilename2 = savefilename;
	savefilename2.append("_amp.csv");
	savefilename.append("_mag.csv");

	ofstream saveFile_mag(savefilename);
	if (saveFile_mag.fail()) {
		cout << "Output file creation failed." << endl;
		exit(2);
	}

	ofstream saveFile_amp(savefilename2);
	if (saveFile_amp.fail()) {
		exit(2);
	}

	// amp first
	saveFile_amp << "time,amp" << endl;
	saveFile_mag << "freq,mag" << endl;

	string line;
	getline(ifile, line);
	getline(ifile, line);
	string startTimeStr = line.substr(15, 15);

	int skiplines = 3;
	while (skiplines > 0) {
		getline(ifile, line);
		skiplines -= 1;
	}
	startTimeStr.replace(8, 1, ".");

	int hour = 0;
	int min = 0;
	int sec = 0;
	double msec = 0;

	int HOUR = (startTimeStr[0] - 48) * 10 + startTimeStr[1] - 48;
	int MIN = (startTimeStr[3] - 48) * 10 + startTimeStr[4] - 48;
	int SEC = (startTimeStr[6] - 48) * 10 + startTimeStr[7] - 48;
	double MSEC = (startTimeStr[9] - 48) * 0.1 
		+ (startTimeStr[10] - 48) * 0.01
		+ (startTimeStr[11] - 48) * 0.001 
		+ (startTimeStr[12] - 48) * 0.0001
		+ (startTimeStr[13] - 48) * 0.00001 
		+ (startTimeStr[14] - 48) * 0.000001;

	string time, amp, mag;
	double relTime = 0.0;
	int countSamplesAmp = 0;
	int countSamplesMag = 0;
	bool amp_mode = true;

	while (!ifile.eof()) {
		getline(ifile, line);
		if (line[0] >= '0' && line[0] <= '9') {
			if (amp_mode) {
				// Amp
				for (int j = 0; j <= line.length(); ++j) {
					if (line[j] == ' ' && line[j + 1] == ' ') {
						line.erase(j + 1, 1);
						--j;
					}
					else if (line[j] == '	') {
						line[j] = ' ';
						--j;
					}
				} // finished cleaning
				time = line.substr(11, 15);
				time[8] = '.';
				amp = line.substr(27, 12);

				if (amp[0] == '-') {
					amp[2] = '.';
				}
				else {
					amp[1] = '.';
				}

				hour = (time[0] - 48) * 10 + time[1] - 48;
				min = (time[3] - 48) * 10 + time[4] - 48;
				sec = (time[6] - 48) * 10 + time[7] - 48;
				msec = (time[9] - 48) * 0.1 
					+ (time[10] - 48) * 0.01
					+ (time[11] - 48) * 0.001 
					+ (time[12] - 48) * 0.0001
					+ (time[13] - 48) * 0.00001 
					+ (time[14] - 48) * 0.000001;

				relTime = time2sec(hour, min, sec, msec, HOUR, MIN, SEC, MSEC);

				numExp amp_vals = e2num(amp);

				saveFile_amp << setprecision(5) << fixedfloat(relTime, 5)
					<< "," << setprecision(amp_vals.precision)
					<< fixedfloat(amp_vals.number, amp_vals.precision)
					<< endl;

				++countSamplesAmp;
			}
			else {
				// mag
				for (int i = 0; i < line.length(); ++i) {
					if (line[i] == '	' ||  line[i] == '	') {
						line[i] = ' ';
					}
				} // finished cleaning
				
				string freq, mag;

				int it = 0;
				while (line[it] != ' ') {
					freq.push_back(line[it]);
					++it;
				}
				++it;
				while (it < line.length()) {
					mag.push_back(line[it]);
					++it;
				}

				for (int i = 0; i < freq.length(); ++i) {
					if (freq[i] == ',') {
						freq[i] = '.';
					}
				}
				for (int i = 0; i < mag.length(); ++i) {
					if (mag[i] == ',') {
						mag[i] = '.';
					}
				}

				saveFile_mag << freq << "," << mag << endl;

				++countSamplesMag;
			}
		}
		else {
			amp_mode = false;
		}
	}

	cout << "Successfully formatted " << countSamplesAmp
		<< " amplitude data-points and " << countSamplesMag
		<< " magnitude data-points." << endl;

	ifile.close();
	saveFile_amp.close();
	saveFile_mag.close();
}