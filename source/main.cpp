#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iomanip>
#include "oscilloscope.h"
#include "bode.h"
#include "DSA.h"
#include "data_logger.h"
#include "FAQ.h"

using namespace std;

void main() {
	bool mainfinished = false;
	cout << "NI ELVISmx Intruments data to LaTeX tool" << endl;
	cout << "Made by Herman K. Dieset (2016)" << endl;

	while (!mainfinished) {
		int selection = 0;
		cout << endl << "1. Bode" << endl;
		cout << "2. Data logger" << endl;
		cout << "3. Dynamic Signal Analyzer" << endl;
		cout << "4. Oscilloscope" << endl;
		cout << "Enter selection: ";
		cin >> selection;

		if (selection == 1) {
			bodeFormatter();
		}
		else if (selection == 2) {
			data_logger_formatter();
		}
		else if (selection == 3) {
			DSA_formatter();
		}
		else if (selection == 4) {
			oscilloscopeFormatter();
		}
		else {
			mainfinished = true;
		}
		if (!mainfinished) {
			cout << endl << "Finished? (y/n): ";
			char choice;
			cin >> choice;
			if (choice == 'y') {
				mainfinished = true;
			}
		}
		for (int i = 0; i < 80; ++i) {
			cout << endl;
		}
	}
	for (int i = 0; i < 80; ++i) {
		cout << endl;
	}
	cout << endl << "Bye bye." << endl;
}
