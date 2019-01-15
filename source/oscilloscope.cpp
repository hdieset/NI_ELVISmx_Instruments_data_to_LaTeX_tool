#include "oscilloscope.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iomanip>

using namespace std;

struct numExp {
	long double number = 0.0;
	int exponent = 0;
	int precision = 0;
};

double time2sec(int hour, int min, int sec, double msec,
	int HOUR, int MIN, int SEC, double MSEC) {

	double startTime = HOUR * 60 * 60 + MIN * 60 + SEC + MSEC;
	double newTime = hour * 60 * 60 + min * 60 + sec + msec;

	return newTime - startTime;
}

string fixedfloat(float x, int precision)
{
	std::ostringstream ss;
	ss << std::fixed << setprecision(precision) << x;
	string str = ss.str();
	return str.substr(0, str.find_last_not_of('0') + 1);
}

numExp e2num(string numberStr) {
	numExp ret;

	bool isPositive = true;
	if (numberStr[0] == '-') {
		isPositive = false;
		numberStr.erase(0, 1);
	}

	long double number = numberStr[0] - '0';

	for (int i = 2; i < numberStr.length() - 3; ++i) {
		number += (numberStr[i] - '0') * pow(10, 1 - i);
	}

	int exponent = numberStr[numberStr.length() - 1] - '0';

	int precision = 6 + exponent;

	if (numberStr[numberStr.length() - 2] == '-') {
		exponent *= -1;
	}

	ret.exponent = exponent;

	if (exponent > 0) {
		while (exponent > 0) {
			number *= 10;
			--exponent;
		}
	}
	else {
		while (exponent < 0) {
			number /= 10;
			++exponent;
		}
	}

	if (!isPositive) {
		number *= -1;
	}
	ret.number = number;
	ret.precision = precision;
	return ret;
}

void oscDualFormatter(string filename, string savefilename) {
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

	string val0Name, val1Name;
	cout << "Enter meassurement 1 name (no spaces): ";
	cin >> val0Name;
	cout << "                   2 name (no spaces): ";
	cin >> val1Name;

	saveFile << "time," << val0Name << "," << val1Name << endl;

	string line;
	int skiplines = 3;
	getline(ifile, line);
	getline(ifile, line);
	string startTimeStr = line.substr(15, 15);

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

	string time, value0, value1;
	double relTime = 0.0;
	int countSamples = -1;
	while (!ifile.eof()) {
		getline(ifile, line);
		if (line[0] == '0' || line[0] == '1' || line[0] == '2') {
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
			value0 = line.substr(27, 12);

			if (value0[0] == '-') {
				value0[2] = '.';
			}
			else {
				value0[1] = '.';
				value0.pop_back();
			}

			if (value0[0] == '-') {
				if (line[67] == '-') {
					value1 = line.substr(67, 14);
					value1[2] = '.';
				}
				else {
					value1 = line.substr(67, 13);
					value1[1] = '.';
				}
			}
			else {
				if (line[66] == '-') {
					value1 = line.substr(66, 14);
					value1[2] = '.';
				}
				else {
					value1 = line.substr(66, 13);
					value1[1] = '.';
				}
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

			numExp val0 = e2num(value0);
			numExp val1 = e2num(value1);

			saveFile << setprecision(6) << fixedfloat(relTime, 6) << ","
				<< setprecision(val0.precision)
				<< fixedfloat(val0.number, val0.precision) << ","
				<< setprecision(val1.precision)
				<< fixedfloat(val1.number, val1.precision) << endl;
		}
		++countSamples;
	}
	cout << "Sucessfully formatted " << countSamples*2 << " data points." << endl;
	saveFile.close();
	ifile.close();
}

void oscSingleFormatter(string filename, string savefilename) {
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

	string val0Name;
	cout << "Enter meassurement name (no spaces): ";
	cin >> val0Name;

	saveFile << "time," << val0Name << endl;

	string line;
	int skiplines = 3;
	getline(ifile, line);
	getline(ifile, line);
	string startTimeStr = line.substr(15, 15);

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

	string time, value0;
	double relTime = 0.0;
	int countSamples = -1;
	while (!ifile.eof()) {
		getline(ifile, line);
		if (line[0] == '0' || line[0] == '1' || line[0] == '2') {
			for (int j = 0; j <= line.length(); ++j) {
				if (line[j] == '	') {
					line[j] = ' ';
				}
			}

			time = line.substr(12, 15);
			time[8] = '.';
			value0 = line.substr(28, 12);

			if (value0[0] == '-') {
				value0[2] = '.';
			}
			else {
				value0[1] = '.';
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

			numExp val0 = e2num(value0);

			saveFile << setprecision(6) << fixedfloat(relTime, 6) << ","
				<< setprecision(val0.precision)
				<< fixedfloat(val0.number, val0.precision) << endl;
		}
		++countSamples;
	}
	cout << "Sucessfully formatted " << countSamples << " data points." << endl;
	saveFile.close();
	ifile.close();
}


void oscilloscopeFormatter() {
	bool finished = false;
	int command = 0;
	while (!finished) {
		cout << endl << "Oscilloscope data formatting options:" << endl;
		cout << "1. Single channel formatting" << endl << "2. Dual channel formatting" << endl;
		cout << endl << "Enter selection: ";
		cin >> command;

		if (command == 1) {
			string filename;

			cout << "Enter filename (.txt): ";
			cin >> filename;

			string savefilename;
			cout << "Enter filename to save to (.csv): ";
			cin >> savefilename;

			oscSingleFormatter(filename, savefilename);
		}
		else if (command == 2) {
			string filename;

			cout << "Enter filename (.txt): ";
			cin >> filename;

			string savefilename;
			cout << "Enter filename to save to (.csv): ";
			cin >> savefilename;

			oscDualFormatter(filename, savefilename);
		}
		else {
			finished = true;
		}

		if (!finished) {
			cout << endl << "Finished formatting oscilloscope data? (y/n): ";
			char yesorno;
			cin >> yesorno;
			if (yesorno == 'y') {
				finished = true;
			}
		}
	}
}