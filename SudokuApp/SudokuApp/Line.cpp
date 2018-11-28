#include "pch.h"  // Precompiled header
#include "SudokuApp.h"
#include "Digit.h"
#include "Box.h"
#include "Line.h"
#include "Ribbon.h"

extern const char* str[];

/**************************************************************************/
ostream& operator<<(ostream& os, const TLine& v) {
	os << str[v.lt] << v.No << endl;
	for (auto& e : v.Cells)
		cout << *e;
	return os;
}
/**************************************************************************/
namespace SudokuApp {
	void PrintRows(const TvsLine& Rows) {
		for (auto k = 0u; k < two; ++k) {
			cout << endl << (k == 0 ? " Rows " : " Columns ") << endl;
			for (const auto& e : Rows[k])
				cout << *e << endl;
		}
	}
}
/****************************************************************************/
TLine::TLine(TSudoku *O, TRw &l, Uint n_, TLineType t) noexcept
	: Osdk{ *O }, RwData( l ) , No{ n_ }, lt{ t }    {
	auto m = (lt == ltr) ? No / tree * tree : No / tree;
	for (Uint i = 0; i < 3; i++) {
		LnBoxes.push_back(&Osdk.Boxes[m]);
		m += (lt == ltr) ? 1 : 3;
	}
}
/*******************************************************************************/
TvUint TLine::GenerateMisCellVect() {
	TvUint vMisCellNo;
	for (Uint i = 1; i < 10; ++i) {
		auto it = find_if(Cells.begin(), Cells.end(), [&](const auto& e) { return e->Digit.No == i; });
		if (it == Cells.end())
			vMisCellNo.emplace_back(i);
	}
	if (vMisCellNo.size() + Cells.size() != nine) {
		cout << " vMisCellNo.size() + Cells.size() != nine Line No ";
		SudokuApp::stdPrintArray(RwData);
		cout << " vMisCell No  : size() " << vMisCellNo.size() << " vector ";
		for (auto& e : vMisCellNo) cout << e << " "; cout << endl;
		cout << " vector of Line Cells : size () " << Cells.size() << " ";
		for (auto& e : Cells) cout << e->Digit.No << " "; cout << endl;
		SudokuApp::PrintDoubleArray(" Exit 25", Osdk.Mtrx);
		exit(25);
	}
	return vMisCellNo;
}
/*******************************************************************************/
TvUint TLine::GenerateMisLocVect() {
	TvUint vMisLoclNo;
	Uint ml{ 0 };
	for (auto& el : RwData) {
		if (el == nullptr) vMisLoclNo.emplace_back(ml);
		++ml;
	}
	return vMisLoclNo;
}
/*******************************************************************************/
bool TLine::InsertMissingCell(TDigit& MisDigit) {
	auto il = RwData.begin();
	while (il != RwData.end()) {
		il = find_if(il, RwData.end(), [&](auto& e) { return e == nullptr; });
		if (il == RwData.end()) return false;
		Uint i = (unsigned)distance(RwData.begin(), il) / tree;
		auto& t = LnBoxes[i];
		auto ip = find_if(t->Cells.begin(), t->Cells.end(), [&](auto& e) { return e->Digit.No == MisDigit.No; });
		if (ip == t->Cells.end()) {
			auto o = No - ((lt == ltr) ? LnBoxes[i]->Lct.x : LnBoxes[i]->Lct.y);
			auto& s = *(t->srMtrx.begin() + o);
			array<Uint*, 3u> sTrpLine = { nullptr };
			copy(s.begin(), s.end(), sTrpLine.begin());
			auto& rb = (lt == ltr) ? Osdk.Ribbons[1][t->tn % tree]
				: Osdk.Ribbons[0][t->tn / tree];
			Uint j{ 0 };
			Uint c{ 0 }; // count
			Uint l{ 0 }; // location
			for (auto& rl : rb->RvLines) {
				auto it = find_if(rl->Cells.begin(), rl->Cells.end(), [&](auto& e) {
					return e->Digit.No == MisDigit.No; });
				if (it != rl->Cells.end()) {
					if (sTrpLine[j] == nullptr)
						sTrpLine[j] = &MisDigit.No;
					++c;
				}
				else {
					l = j;
				}
				++j;
			}
			if (c == two) {
				TLoc nl(lt == ltr ? No : LnBoxes[i]->Lct.x + l, lt == ltr ? LnBoxes[i]->Lct.y + l : No);
				MisDigit.InsertNewItem(nl);
				return true;
			}
		}
		++il;
	}
}
/*******************************************************************************/
void TLine::Solution() {
	auto vMisCellNo = GenerateMisCellVect();
	TRw vTestModify{ nullptr };
	auto rv = false;
	while (vMisCellNo.size() > 0) {
		auto it = find_if(vMisCellNo.begin(), vMisCellNo.end(), [&](auto e) {
			return this->TrytoInsertMisingCell(e, vMisCellNo, rv, vTestModify);
		});
		if (it != vMisCellNo.end())
			vMisCellNo.erase(remove_if(vMisCellNo.begin(), vMisCellNo.end(), [&](const auto& e) { return e == *it; }), vMisCellNo.end());
		if (vMisCellNo.size() == 0 || it == vMisCellNo.end()) return;
	}
}
/*******************************************************************************/
void TLine::InsertFinalCell() {
	auto vMisCellNo = GenerateMisCellVect();
	auto vMisLocNo = GenerateMisLocVect();
	auto& MisCell = Osdk.Digits[vMisCellNo.front() - 1];
	TLoc nl(lt == ltr ? No : vMisLocNo.front(), lt == ltr ? vMisLocNo.front() : No);
	MisCell.InsertNewItem(nl);
}
/*******************************************************************************/
PNewItem TLine::MissingOtherCellsContains(const Uint mce, const TvUint& vMisCellNo) {
	auto found{ false };
	for (auto& e : vMisCellNo) {
		if (e == mce) continue;
		auto it = find_if(RwData.begin(), RwData.end(), [&](auto& le)
		{ return le != nullptr && *le == e; });
		if (it == RwData.end()) return make_pair(false, No);
	}
	return make_pair(true, No);
}
/*******************************************************************************/
PNewItem TLine::FillTstVectorLineTripleshasMissingCell(TRw& tstV, TDigit& MisCell) {
	for (auto i = 0u; i < tree; ++i) {
		auto it = find_if(MisCell.Cells.begin(), MisCell.Cells.end(), [&](auto& ei) {
			return ei->ta == LnBoxes[i]->tn;
		});
		if (it != MisCell.Cells.end()) {
			auto it = next(tstV.begin(), (lt == ltr ? LnBoxes[i]->Lct.y : LnBoxes[i]->Lct.x));
			for (auto j = 0; j < 3; ++j, ++it)
				if (*it == nullptr)
					*it = &MisCell.No;
		}
	}
	int c = count_if(tstV.begin(), tstV.end(),
		[&](auto& et) { return et == nullptr; });
	if (c == 1) {
		auto it = find_if(tstV.begin(), tstV.end(), [&](auto& et) {return et == nullptr; });
		return make_pair(true, static_cast<Uint> (distance(tstV.begin(), it)));
	}
	return make_pair(false, zero);
}

/*******************************************************************************/
bool TLine::TrytoInsertMisingCell(const Uint e, const TvUint& vMisCellNo, const bool v, const TRw& mv) {
	auto& MisCell = Osdk.Digits[e - 1];
	TRw tstV{ (lt == ltr ? MisCell.tstV : MisCell.tstH) };
	if (v) {
		auto it{ tstV.begin() };
		for_each(mv.begin(), mv.end(), [&](auto& em) {
			if (em != nullptr) *it = em;
			++it;
		});
	}
	auto  it{ RwData.begin() };
	for_each(tstV.begin(), tstV.end(), [&](auto& et) {
		if (et == nullptr) et = *it;
		++it;
	});
	bool found;
	Uint Lct;
	tie(found, Lct) = FillTstVectorLineTripleshasMissingCell(tstV, MisCell);
	if (found == false) {
		auto vMisLocNo = GenerateMisLocVect();
		Uint j = (lt == ltr) ? 1 : 0;
		vector<TsLine>& r = Osdk.Rows[j];
		for (auto& ml : vMisLocNo) {
			TsLine& re = *(next(r.begin(), ml));
			tie(found, Lct) = re->MissingOtherCellsContains(e, vMisCellNo);
			if (found) break;
		}
	}
	if (found) {
		TLoc nl(lt == ltr ? No : Lct, lt == ltr ? Lct : No);
		MisCell.InsertNewItem(nl);
		true;
	}
	return false;
}
/******************************************************************************/
bool TLine::RowDatahasaDigit(const Uint Entry) {
	auto it = find_if(RwData.begin(), RwData.end(), [&](auto& e) {
		if (e != nullptr)
			return Entry == *e;
		return false;
	});
	return it != RwData.end();
}
/******************************************************************************/
void TLine::UpdateRowDataDigit(const Uint i, const Uint No) {
	if (No < nine)
		RwData[i] = &Osdk.Digits[No].No;
	else RwData[i] = nullptr;
}
/*****************************************************************************/
void TLine::AddCell(TCell *cItm) {
	auto it = find_if(Cells.begin(), Cells.end(), [&](TCell * Entry) {
		return Entry->Digit.No > cItm->Digit.No;    });
	Cells.insert(it, cItm);
}
/****************************************************************************/
auto MxLineItem = [&](auto& l, auto& r) {
	if (l->Cells.size() == nine) return true;
	return l->Cells.size() < r->Cells.size() && r->Cells.size() < nine;
};
