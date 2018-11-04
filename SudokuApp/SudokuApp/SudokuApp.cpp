// SudokuApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
/*
 * File:   SudokuApp.cpp
 * Author: Sabetay Toros 27 Sep 18.15
 *
 * Created on 06 August 2018, 18:05
 */
 /******************************************************************************/
#include "pch.h"
#include "SudokuApp.h"
#include "Digit.h"
#include "Box.h"
#include "Line.h"
#include "Ribbon.h"
#include "Posibility.h" 
static Uint Counter = 0;
/******************************************************************************/
auto mxelem = [&](auto& l, auto& r) {
	if (l == nine) return true;
	return r < nine && l < r;
};

const char* str[] = { " Row Details ", " Column Details " };

/******************************************************************************/
ostream& operator<<(ostream& os, const Uint* ptr) {
	if (ptr == nullptr) os << "0 ";
	else os << *ptr << " ";
	return os;
}
/******************************************************************************/
ostream& operator<<(ostream& os, const TCell& It) {
	return os << '\t' << " Digit No " << It.Digit.No << It.Lct // << " r " << It.r << " c " << It.c 
		<< " ta " << It.ta << " tr " << It.tr << " tc " << It.tc << endl;
}
/******************************************************************/
ostream& operator<<(ostream& os, const TLoc& v) {
	return os << " Row " << v.x << " Column " << v.y;
}
/******************************************************************************/
namespace SudokuApp {

	bool intrm = false;
	/**************************************************************************/
	void PrintCounts(const TCounts& Counts) {
		cout << "Max Counts  CelNo Row Column Triple" << endl;
		for (auto& a : Counts) {
			for (auto& e : a)
				cout << " " << e;
			cout << endl;
		}
	}
	/**************************************************************************/
	void PrintLine(const TRw& e) {
		for (const Uint* ptr : e)
			cout << ptr;
		cout << endl;
	}
	/**************************************************************************/
	void PrintDoubleArray(const string& s, const TDblMtrx& v) {
		cout << s << endl;
		int r = 0;
		for (const TRw& er : v) {
			cout << r << '\t';
			PrintLine(er);
			++r;
		}
		cout << endl;
	}
	/**************************************************************************/
	void PrintBoxMatrix(const TBoxMtrx& Box) {
		cout << " Box Matrix ";
		for (const auto& a : Box) {
			for (const auto& e : a)
				cout << e;
		}
		cout << endl;
	}
	/**************************************************************************/
	void PrintDigitDetails(const TvsCell& Cells) {
		cout << " Digit Details " << endl << endl;
		for (auto& e : Cells) {
			cout << *e;
		}
	}
}
using namespace SudokuApp;
/*******************************************************************************/
optional<Tsfstream> GetFileHandle() {
	Tsfstream fs{ make_shared<fstream>() };
	fs->open("SudokuTest.txt", ios::in);
	if (fs->fail()) {
		cout << " SudokuTest.txt File not found  " << endl;
		return {};
	}
	return fs;
}
/*******************************************************************************/
TSdkData::TSdkData(string s, fstream& fs) : Identity(move(s)) {
	auto v{ 0u };
	for (auto j = 0u; j < eightyone; ++j) {
		fs >> v;
		Data.emplace_back(v);
		if (fs.eof()) return;
	}
	string flsh;
	getline(fs, flsh); // 
	State = true;
}
/*******************************************************************************/
optional<TSdkData> GetData(fstream* fs) {
	std::string id;
	for (;;) {
		getline(*fs, id);
		if (fs->eof())
			return {};
		if (id[0] == '*') break;
		cout << " Identity first char must be ' * ' not " << id[0] << endl;
	}
	SudokuApp::intrm = id[1] == ' ' ? false : true;
	TSdkData Data(id, *fs);
	if (Data.State) return Data;
	return {};
}
/*******************************************************************************/
int main(int argc, char** argv) {
	optional<Tsfstream> fs = GetFileHandle();
	if (fs) {
		const Tsfstream& sfs = fs.value();
		fstream *fp = sfs.get();
		for (;;) {
			optional<TSdkData> SdkData = GetData(sfs.get());
			if (SdkData)
				TSudoku Sudoku(SdkData.value());
			else break;
		}
	}
	return 0;
}
/*****************************************************************************/
TSudoku::TSudoku(TSdkData& SdkData) noexcept {
	for (Uint j = 1; j < 10; ++j) Digits.emplace_back(this, j);
	for (Uint j = 0; j < nine; ++j) Boxes.emplace_back(this, j);

	// Settomg Rows & Collums
	auto it = Mtrx.begin();
	auto t = ltr;
	for (Uint k = 0; k < two; ++k) {
		Rows.emplace_back();
		auto& r = Rows.back();
		for (Uint j = 0; j < nine; ++j, ++it)
			r.emplace_back(make_shared<TLine>(this, it, j, t));
		t = ltc;
		it = Clmn.begin();
	}
	// Ribbons
	t = ltr;
	for (auto k = 0u; k < 2u; ++k) {
		Ribbons.emplace_back();
		auto& r{ Ribbons.back() };
		for (Uint j = 0; j < tree; ++j)
			r.emplace_back(make_shared<TRibbon>(this, j, t));
		t = ltc;
	}
	auto c = 0;
	Uint i{ 0 };
	Uint j{ 0 };
	for (auto& e : SdkData.Data) {
		TLoc cLoc{ c / nine, c % nine };
		if (e > 0) {
			auto n = e - 1;
			Digits[n].GenerateItem(cLoc, true);
		}
		++c;
	}
	PrintDoubleArray(SdkData.Identity, Mtrx);
	if (intrm) PrintCounts(Counts);
	auto sz = Cells.size();
	auto nsz = sz;
	for (;;) {
		SelectaSolution();
		nsz = Cells.size();
		if (nsz == eightyone || nsz - sz == 0) break;
		sz = nsz;
		if (intrm) {
			cout << " size " << Cells.size() << endl;
			PrintDoubleArray("Ara Cikis", Mtrx);
			PrintCounts(Counts);
		}
	}
	if (nsz != eightyone) BackTraceSolution();
	auto flg = nsz == eightyone || LastTrace == mdTerminate;
	cout << (flg ? " Puzzle Solved " : " Puzzle not Solved Number of Filled Items ");
	if (!flg) cout << Cells.size() << " ";
	PrintDoubleArray(SdkData.Identity, Mtrx);
	if (!flg) PrintCounts(Counts);
}
/******************************************************************************/
void TSudoku::BackTraceSolution() {
	for (auto& e : Cells)
		e->isData = true;
	vector<TsLine>& rw = *Rows.begin();
	Uint Prv{ 1 };
	for (const auto& erw : rw) {
		vector<TsLine>& cl = *(next(Rows.begin(), 1));
		for (const auto& ecl : cl)
			vPosible.emplace_back(make_shared<TPosibility>(erw.get(), ecl.get(), Prv));
	}
	vPosible.begin()->get()->BackTrace();
}

const char *cstr[] = { "Cells", "Rows", "Column", "Triple" };
/******************************************************************************/
auto MxCellItem = [&](auto& l, auto& r) {
	if (l.Cells.size() == nine) return true;
	return (l.Cells.size() < r.Cells.size() && r.Cells.size() < nine);
};
/******************************************************************************/
void TSudoku::DigitSolution() {
	auto MxCell = Digits.begin();
	while (MxCell != Digits.end()) {
		MxCell = max_element(MxCell, Digits.end(), MxCellItem);
		if (MxCell != Digits.end() && MxCell->Cells.size() > 5) {
			MxCell->Solution();
			if (Inserted) break;
			++MxCell;
		}
		else break;
	}
}
/******************************************************************************/
void TSudoku::SelectaSolution() {
	Inserted = false;
	using MxPair = pair<Uint, Uint >; // first { Digit, Row, Col, Trp } , second Maxlar
	vector<MxPair> vMxPair;
	Uint c{ 0 };
	for (auto& a : Counts) {
		auto it = max_element(a.begin(), a.end(), mxelem);
		Uint &ptr = *it;
		vMxPair.push_back(make_pair(ptr, c));
		++c;
	}
	sort(vMxPair.begin(), vMxPair.end(), [](const auto& l, const auto& r) {
		return l.first > r.first; });
	auto sz = Cells.size();
	if (intrm) {
		cout << " Maximum Counts " << endl;
		for (auto& e : vMxPair)
			if (intrm) cout << e.first << " " << " { " << cstr[e.second] << " } " << endl;
	}
	for (auto& e : vMxPair) {
		if (e.second == 0)
			DigitSolution();
		else if (e.second > 0 && e.second < tree)
			LineSolution(e.second);
		else if (e.second == tree) {
			auto i = max_element(Counts[3].begin(), Counts[3].end(), mxelem);
			auto d = distance(Counts[3].begin(), i);
			auto MaxBox = next(Boxes.begin(), d);
			if (MaxBox->Cells.size() > 3)
				MaxBox->Solution();
			if (!Inserted) {
				for (auto c = 0u; c < nine; ++c) {
					if (c == d || Boxes[c].Cells.size() < 4) continue;
					Boxes[c].Solution();
					if (Inserted) break;
				}
			}
			if (!Inserted) RibbonSolution();
		}
		auto nsz = Cells.size();
		if (nsz == eightyone || nsz > sz) return;
		if (intrm) cout << cstr[e.second] << " in Solution Cells could not be inserted  " << endl;
	}
}
/*******************************************************************************/
void TSudoku::LineSolution(Uint n) {
	auto i = (n == 1) ? ltr : ltc;
	vector<TLine *> cLines;
	for (auto& e : Rows[i]) cLines.emplace_back(e.get());
	sort(cLines.begin(), cLines.end(), [&](const auto& l, const auto& r) {
		if (l->Cells.size() == r->Cells.size()) return l->No < r->No;
		return l->Cells.size() > r->Cells.size(); });
	for (auto& e : cLines) {
		auto sz{ e->Cells.size() };
		if (e->Cells.size() > tree) {
			while (e->Cells.size() < nine) {
				e->Solution();
				auto nz = e->Cells.size();
				if ((nz - sz) == 0 || e->Cells.size() == nine) break;
				sz = nz;
			}
		}
		if (Inserted) return;
	}
}
/*******************************************************************************/
void TSudoku::RibbonSolution() {
	for (auto& ei : Ribbons) {
		for (auto& e : ei) {
			auto i = 0u;
			for_each(e->Counts.begin(), e->Counts.end(), [&](const auto& ec) {
				if (ec == two)
					e->CheckMissingThirdDigit(i);
				++i;
			});
		}
	}

}
