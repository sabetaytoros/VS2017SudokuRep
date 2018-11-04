#include "pch.h"  // Precompiled header
#include "SudokuApp.h"
#include "Digit.h"
#include "Line.h"
#include "Box.h"
#include "Ribbon.h"

/******************************************************************/
ostream& operator<<(ostream& os, const TBox& v) {
	os << " Triple Detail No " << v.tn << " " << v.Lct << endl;
	os << " Ribbon No " << v.Rbn.x << " " << v.Rbn.y << endl;
	os << " Triple Digit Items " << v.Cells.size() << endl;
	for (const auto& e : v.Cells)
		os << *e;
	os << endl;
	return os;
}
/*************************************************************************C*****/
TBox::TBox(TSudoku *o, Uint tn_) noexcept
	: Osdk{ *o }, tn{ tn_ }, Lct{ tn / tree * tree, tn % tree * tree }
	, Rbn{ tn / tree, tn % tree }    {
}
/******************************************************************************/
void TBox::AddCell(TCell& Cell) {
	srMtrx[Cell.tr][Cell.tc] = &Cell.Digit.No;
	scMtrx[Cell.tc][Cell.tr] = &Cell.Digit.No;
	auto it = find_if(Cells.begin(), Cells.end(), [&](const TCell* Entry) {
		return Entry->Digit.No > Cell.Digit.No;    });
	Cells.insert(it, &Cell);
	++Osdk.Counts[tree][Cell.ta]; // Triple Count
	Osdk.ChckR = true;
	for (Uint i = 0; i < two; ++i) {
		Uint j = i == 0 ? Rbn.x : Rbn.y;
		Osdk.Ribbons[i][j]->AddCount(Cell);
	}
	Osdk.Inserted = true;
}
/*******************************************************************************/
void TBox::Solution() {
	auto MisClNo = one;
	auto it = Cells.begin();
	auto c = one;
	while (it != Cells.end() && MisClNo < 10 && Osdk.Cells.size() < eightyone  && Osdk.Counts[3][tn] < nine) {
		if (it != Cells.end()) {
			if (MisClNo == one)
				it = Cells.begin();
			it = find_if(it, Cells.end(), [&](const TCell * Entry) {
				if (MisClNo == Entry->Digit.No) {
					++MisClNo;
					return false;
				}
				return true; });
		}
		if (MisClNo < ten) {
			auto endNo = it != Cells.end() ? (*it)->Digit.No : nine;
			for (;;) {
				if (Solution(MisClNo)) {
					MisClNo = 1;
				}
				else ++MisClNo;
				if (MisClNo >= endNo || MisClNo == 1) break;
			}
			if (c > 4) exit(26);
		}
	}
}
/*******************************************************************************/
bool TBox::Solution(Uint No) {
	TDigit& Digit = *next(Osdk.Digits.begin(), No - 1);
	auto cMtrx = GenerateChechSubMatrix(Digit);                           // Checking Sub Matrix
	optional<TLoc> fl = FindInsertionLocation(cMtrx);         // SearcchforAviableCell
	if (fl) {
		TLoc nl{ fl->x + Lct.x, fl->y + Lct.y };
		Digit.InsertNewItem(nl);
		if (Osdk.Cells.size() == eightyone) return true;
		if (Osdk.Counts[3][tn] == eight)
			InsertFinalCell();
		if (Cells.size() > five && Cells.size() < nine)
			InsertOtherCells();
		return true;
	}
	return false;
}
/*******************************************************************************/
TBoxMtrx TBox::GenerateChechSubMatrix(TDigit& Digit) {
	TBoxMtrx cMtrx{ nullptr };
	auto itc = cMtrx.begin();
	for (auto& a : srMtrx) {
		auto ita = itc->begin();
		copy(a.begin(), a.end(), ita);
		++itc;
	}
	for (auto j = 0; j < two; ++j) {
		auto& rb = (j == 0) ? Osdk.Ribbons[0][tn / tree]
			: Osdk.Ribbons[1][tn % tree];
		for (auto& el : rb->RvLines) {
			TLine& Line{ *el };
			auto it = find_if(Line.Cells.begin(), Line.Cells.end(), [&](auto& ec) {
				return ec->Digit.No == Digit.No;  });
			if (it != Line.Cells.end()) {
				TCell& Cell = **it;
				TLoc nLoc{ Cell.Lct.x - Lct.x, Cell.Lct.y - Lct.y };
				auto xy = (j == 0) ? nLoc.x : nLoc.y;
				if (j == 0) {
					for (auto k = 0; k < 3; ++k)
						if (cMtrx[xy][k] == nullptr)
							cMtrx[xy][k] = &Digit.No;
				}
				else {
					for (auto k = 0; k < tree; ++k)
						if (cMtrx[k][xy] == nullptr)
							cMtrx[k][xy] = &Digit.No;
				}
			}
		}
	}
	return cMtrx;
}
/*******************************************************************************/
optional<TLoc> TBox::FindInsertionLocation(TBoxMtrx& cMtrx) {
	auto i{ 0 };
	auto j{ 0 };
	vector <TLoc> vLct;
	for (auto& a : cMtrx) {
		auto it{ a.begin() };
		while (it != a.end()) {
			auto k = 0;
			it = find_if(it, a.end(), [&k](auto& e) { return e == nullptr; });
			if (it != a.end()) {
				vLct.emplace_back(i, distance(a.begin(), it));
				++it;
			}
		}
		++i;
	}
	if (vLct.size() == 1)
		return vLct.back();
	return {};
}
/*******************************************************************************/
void TBox::InsertOtherCells() {
	for (auto c = 0; c < 4; ++c) {
		Osdk.ChckR = false;
		for (auto& e : Cells) {
			for (Uint i = 0; i < two; i++) {
				Uint j = (i == 0) ? Rbn.x : Rbn.y;
				Osdk.Ribbons[i][j]->CheckMissingOtherCell(*e);
			}
		}
		if (Osdk.ChckR == false) break;
	}
}
/******************************************************************************/
bool TBox::RowMatrixhasaDigit(const Uint Entry) {
	for (auto& a : srMtrx) {
		for (auto& e : a)
			if (e != nullptr && *e == Entry) return true;
	}
	return false;
}
/******************************************************************************/
void TBox::UpdateRowDataDigit(const TLoc& Lct, const Uint No) {
	if (No < nine)
		srMtrx[Lct.x][Lct.y] = &Osdk.Digits[No].No;
	else srMtrx[Lct.x][Lct.y] = nullptr;
}

/*******************************************************************************/
void TBox::InsertFinalCell() {
	Uint MisClNo{ 1 };
	auto it = find_if(Cells.begin(), Cells.end(), [&](TCell * Entry) {
		if (MisClNo != Entry->Digit.No) return true;
		++MisClNo;
		return false;
	});
	auto MisCell = next(Osdk.Digits.begin(), MisClNo - 1);
	TBoxMtrx cMtrx{ nullptr };
	copy(srMtrx.begin(), srMtrx.end(), cMtrx.begin());
	optional<TLoc> fl = FindInsertionLocation(cMtrx);
	if (fl) {
		TLoc nl{ fl->x + Lct.x, fl->y + Lct.y };
		MisCell->InsertNewItem(nl);
	}
}
