#include "data_logger.h"
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

void data_logger_formatter() {
	bool finished = false;
	int command = 0;
	while (!finished) {
		cout << endl << "Data logger data formatting options:" << endl;
		cout << "1. Single channel formatting" << endl;
		cout << "2. Dual channel formatting" << endl;
		cout << endl << "Enter selection: ";
		cin >> command;

		if (command == 1) {
			string filename;
			
			cout << "Enter filename (.lvm): ";
			cin >> filename;

			string savefilename;
			cout << "Enter filename to save to (.csv): ";
			cin >> savefilename;

			single_data_formatter(filename, savefilename);
		}
		else if (command == 2) {
			string filename;

			cout << "Enter filename (.lvm): ";
			cin >> filename;

			string savefilename;
			cout << "Enter filename to save to (.csv): ";
			cin >> savefilename;

			dual_data_formatter(filename, savefilename);
		}
		else {
			finished = true;
		}
		
		if (!finished) {
			cout << endl << "Finished formatting data logging? (y/n) ";
			char yesorno = 'y';
			cin >> yesorno;
			if (yesorno == 'y') {
				finished = true;
			}
		}
	}
}

void single_data_formatter(string filename, string savefilename) {
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
	getline(ifile, line);
	getline(ifile, line);
	
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
	string startTimeStr = line.substr(11, 15);
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

	double relTime = 0.0;
	string time, value;
	value = line.substr(27, 12);

	if (value[0] == '-') {
		value[2] = '.';
	}
	else {
		value[1] = '.';
	}

	numExp val = e2num(value);

	saveFile << setprecision(5) << "0.0,"
		<< setprecision(val.precision)
		<< fixedfloat(val.number, val.precision) << endl;

	int countSamples = 1;
	while (!ifile.eof()) {
		getline(ifile, line);
		if (line[0] >= '0' && line[0] <= '9') {
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
			value = line.substr(27, 12);

			if (value[0] == '-') {
				value[2] = '.';
			}
			else {
				value[1] = '.';
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

			val = e2num(value);

			saveFile << setprecision(5) << fixedfloat(relTime, 5)
				<< "," << setprecision(val.precision)
				<< fixedfloat(val.number, val.precision)
				<< endl;

			++countSamples;
		}
	}

	cout << "Successfully formatted " << countSamples << " data-points" << endl;

	ifile.close();
	saveFile.close();
}

void dual_data_formatter(string filename, string savefilename) {
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
	getline(ifile, line);
	getline(ifile, line);

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

	string startTimeStr = line.substr(11, 15);
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

	double relTime = 0.0;
	string time, value0, value1;

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

	numExp val0 = e2num(value0);
	numExp val1 = e2num(value1);

	saveFile << setprecision(6) << "0.0,"
		<< setprecision(val0.precision)
		<< fixedfloat(val0.number, val0.precision) << ","
		<< setprecision(val1.precision)
		<< fixedfloat(val1.number, val1.precision) << endl;

	int countSamples = 1;
	while (!ifile.eof()) {

		getline(ifile, line);
		if (line[0] >= '0' && line[0] <= '9') {
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

			val0 = e2num(value0);
			val1 = e2num(value1);

			saveFile << setprecision(6) << fixedfloat(relTime, 6) << ","
				<< setprecision(val0.precision)
				<< fixedfloat(val0.number, val0.precision) << ","
				<< setprecision(val1.precision)
				<< fixedfloat(val1.number, val1.precision) << endl;

			++countSamples;
		}
	}

	cout << "Successfully formatted " << countSamples*2 << " data-points" << endl;

	ifile.close();
	saveFile.close();
}