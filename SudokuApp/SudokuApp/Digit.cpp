#include "pch.h"  // Precompiled header
#include "SudokuApp.h"
#include "Digit.h"
#include "Box.h"
#include "Line.h"
#include "Ribbon.h"

TvUint vint{ 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u };
/******************************************************************************/
TCell::TCell(TDigit* o, TLoc lct, bool d)
	: Digit{ *o }, Lct{ move(lct) }, ta{ (Lct.x / tree * tree) + Lct.y / tree }
	, tr{ Lct.x % tree }, tc{ Lct.y % tree }, isData{ d } {
	++Digit.Count;
	Digit.Osdk.Mtrx[Lct.x][Lct.y] = &Digit.No;
	Digit.Osdk.Clmn[Lct.y][Lct.x] = &Digit.No;
	Digit.tstV[Lct.y] = &Digit.No;
	Digit.tstH[Lct.x] = &Digit.No;
	Digit.Osdk.Boxes[ta].AddCell(*this);
	Digit.Osdk.Rows[ltr][Lct.x]->AddCell(this);
	Digit.Osdk.Rows[ltc][Lct.y]->AddCell(this);
	++Digit.Osdk.Counts[0][Digit.No - 1];                       // CellNo sayisi
	++Digit.Osdk.Counts[1][Lct.x];                              // Row Count
	++Digit.Osdk.Counts[two][Lct.y];                            // ColumnCount
	Digit.Osdk.Cells.push_back(this);
	Digit.Osdk.Inserted = true;
}
/******************************************************************/
ostream& operator<<(ostream& os, const TDigit& v) {
	os << "CellNo " << v.No << endl;
	for (auto& e : v.Cells)
		cout << *e;
	return os;
}
/*****************************************************************************/
TDigit::TDigit(TSudoku * o, Uint No_)
	: Osdk{ *o }, No(vint[No_]) {
}

/****************************************************************************/
void TDigit::GenerateItem(TLoc& lct, bool Dflt) {
	Cells.emplace_back(make_shared<TCell>(this, lct, Dflt));
}
/******************************************************************/
bool TDigit::FillTestRowWithNonCellValues(TRw& MtrxLn, TRw& tstV, Uint mr, TLineType t) {
	TRw ntstV = { nullptr }; // new test Vector
	auto f = false;
	Uint tc{ 0 };
	auto im = MtrxLn.begin();
	copy(tstV.begin(), tstV.end(), ntstV.begin());
	auto ci = ntstV.end();
	for_each(ntstV.begin(), ntstV.end(), [&](auto& e) {
		if (e == nullptr)
			e = *im;
		++im;
	});
	auto it = ntstV.begin();
	for (size_t i = 0; i < two; i++) {
		it = find_if(it, ntstV.end(), [](auto& e) {
			return e == nullptr; });
		if (it != ntstV.end()) {
			if (i == 0) {
				tc = (unsigned)distance(ntstV.begin(), it);
				++it;
			}
			else return false;
		}
	}
	auto ip{ mr > Cells.size() ? Cells.end() : next(Cells.begin(), mr) };
	TLoc nl{ t == ltr ? mr : tc , t == ltr ? tc : mr };
	Cells.insert(ip, make_shared<TCell>(this, nl, false));
	if (Cells.size() == eight) {
		InsertFinalCell();
		return true;
	}
	return Osdk.Inserted;
}
/******************************************************************/
bool TDigit::InsertNewItem(TLoc& lc) {
	auto ip = find_if(Cells.begin(), Cells.end(), [&](auto& e) {
		return e->Lct.x > lc.x; });
	ip = Cells.insert(ip, make_shared<TCell>(this, lc, false));
	if (Osdk.Cells.size() == eightyone) return true;
	TCell& Cell = *ip->get();
	for (int i = 0; i < two; ++i) {
		auto j = i == 0 ? Cell.Lct.x : Cell.Lct.y;
		auto& Line = *Osdk.Rows[i][j];
		if (Line.Cells.size() == eight) Line.InsertFinalCell();
	}
	if (Osdk.Boxes[Cell.ta].Cells.size() == eight)
		Osdk.Boxes[Cell.ta].InsertFinalCell();
	for (Uint i = 0; i < two; i++) {
		Uint j = (i == 0) ? Osdk.Boxes[Cell.ta].Rbn.x : Osdk.Boxes[Cell.ta].Rbn.y;
		Osdk.Ribbons[i][j]->CheckMissingOtherCell(Cell);
		if (Osdk.Cells.size() == eightyone) return  true;
	}
	return false;
}
/***************************************************************************/
void TDigit::InsertFinalCell() {
	auto ih = find_if(tstH.begin(), tstH.end(), [](auto& e) {
		return e == nullptr; });
	auto iv = find_if(tstV.begin(), tstV.end(), [](auto& e) {
		return e == nullptr; });
	TLoc Lct{ (unsigned)distance(tstH.begin(), ih), (unsigned)distance(tstV.begin(), iv) };
	Cells.insert(next(Cells.begin(), Lct.x), make_shared<TCell>(this, Lct, false));
}
/******************************************************************/
void TDigit::FindMissingRow(TRw& Vect, Uint& mr) {
	auto it = next(Vect.begin(), mr);
	it = find_if(it, Vect.end(), [&](const auto& e) { return e == nullptr; });
	mr = distance(Vect.begin(), it);
}
/******************************************************************************/
void TDigit::Solution() {
	for (Uint i = 0; i < two; ++i) {
		Uint mr{ 0 };
		while (mr < nine) {
			FindMissingRow((i == 0 ? tstH : tstV), mr); // If Fitted Itm insertion position
			if (mr < nine) {
				if (FillTestRowWithNonCellValues(i == 0 ? *next(Osdk.Mtrx.begin(), mr) : *next(Osdk.Clmn.begin(), mr),
					i == 0 ? tstV : tstH, mr, static_cast<TLineType>  (i))) {
					return;
				}
				++mr;
			}
		}
	}
}
/*******************************************************************************/
void TDigit::InsertFoundLineLocation(const Uint& Ln, const TLineType& lt, TRw& cRow) {
	auto it = find_if(cRow.begin(), cRow.end(), [&](const auto& er) {
		return er == nullptr; });
	if (it != cRow.end()) {
		Uint d = (unsigned)distance(cRow.begin(), it);
		TLoc nl(lt == ltr ? Ln : d, lt == ltr ? d : Ln);
		InsertNewItem(nl);
	}
}
/*******************************************************************************/
bool TDigit::InserttoOneofLocations(const Uint& Ln, const TLineType& lt, TRw& cRow, TvpBox& Box) {
	auto si = 0u;
	auto c = 0u;
	for (Uint i = 0; i < nine; ++i) {
		if (cRow[i] == nullptr) {
			Uint t = Box[i / 3]->tn;
			auto it = find_if(Cells.begin(), Cells.end(), [&](auto& e) {
				return e->ta == t;
			});
			if (it != Cells.end()) continue;
			if (c == 1) return false;
			si = i;
			++c;
		}
	}
	TLoc nl(lt == ltr ? Ln : si, lt == ltr ? si : Ln);
	InsertNewItem(nl);
	return true;
}
