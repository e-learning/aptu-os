/*
 * main.cpp
 *
 *  Created on: 25 марта 2014 г.
 *      Author: stani_000
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int procTime = 0;
int c;

struct IO {
	int IOT;
	int IOL;
	IO(int IOT, int IOL) {
		this->IOT = IOT;
		this->IOL = IOL;
	}

};

struct Proc {
	string ID;
	int Ti;
	int Tw;
	vector<IO> Io;
	int curr;
	Proc(string ID_, int Ti_, int Tw_, vector<IO> Io_) {
		ID = ID_;
		Ti = Ti_;
		Tw = Tw_;
		Io = Io_;
		curr = 0;
	}

	bool isReady() {
		return  procTime>=Ti;
	}

	bool isPrior() {
		return (isReady() && (needIO() || (c - left() > 0)));
	}

	void run() {
		cout << procTime << " " << ID << "\n";
		if (needIO()) {
			block();
			return;
		} else {
			int wT = (Tw - curr < c)?Tw - curr:c;
			if (wT>0) {
				curr += wT;
				procTime += wT;
			}
		}
	}

	int left() {
		return Tw - curr;
	}

	bool needIO() {
		if (!Io.empty()) {
			if (curr + c >= Io[0].IOT) {
				return true;
			}
		}
		return false;
	}

	void block() {
		int diff =Io[0].IOT-curr;
		curr+=diff;
		procTime+=diff;

		Ti = procTime + Io[0].IOL;
		curr += Io[0].IOL;
		Io.erase(Io.begin());

	}
};

struct Procs {
	vector<Proc> procs;

	int prior() {
		for (size_t i = 0; i < procs.size(); ++i) {
			if (procs[i].isPrior())
				return i;
		}
		return -1;
	}

	int ready() {
		for (size_t i = 0; i < procs.size(); ++i) {
			if (procTime>=procs[i].Ti)
				return i;
		}
		return -1;
	}

	void update() {
		for (size_t i = 0; i < procs.size(); ++i) {
			if (procs[i].left() <= 0) {
				procs.erase(procs.begin() + i);
				--i;
			}
		}
	}

	bool empty() {
		return procs.empty();
	}

	int nextTin() {
		update();
		if (!empty()) {
			int minTin = procs[0].Ti;
			for (size_t i = 1; i < procs.size(); ++i) {
				if (procs[i].Ti < minTin) {
					minTin = procs[i].Ti;
				}
			}
			return minTin;
		}
		return -1;
	}

}
;

int main() {


	Procs procs;

	string ID;
	int Ti, Tw, IOT, IOL;

	string line;
	stringstream ss;
	getline(cin, line);
	ss<<line;
	ss>>c;
	while (getline(cin, line)) {
		if(line.empty()){
			break;
		}
		stringstream ss;
		ss << line;
		ss >> ID >> Ti >> Tw;
		vector<IO> ios;
		while (ss >> IOT) {
			ss >> IOL;
			IO io(IOT, IOL);
			ios.push_back(io);
		}

		Proc proc(ID, Ti, Tw, ios);
		procs.procs.push_back(proc);
	}


	for (procTime = 0; !procs.empty();) {
		procs.update();
		int prior = procs.prior();
		if (prior != -1) {
			procs.procs[prior].run();
			continue;
		}

		prior = procs.ready();
		if(prior!=-1){
			procs.procs[prior].run();
			continue;
		}

		if(!procs.empty()){
			cout<<procTime<< " IDLE" <<"\n";
			procTime = procs.nextTin();
		}

	}

}
