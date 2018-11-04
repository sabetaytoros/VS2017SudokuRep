
#include "pch.h"  // Precompiled header
#include "SudokuApp.h"
#include "Digit.h"
#include "Box.h"
#include "Line.h"
#include "Ribbon.h"
#include "Posibility.h"

 /******************************************************************************/
ostream& operator<<(ostream& os, const TPosibility& v) {
	os << " Cell No " << (v.Next - 1) << "  { " << v.Lct.x << " , " << v.Lct.y << " } ";
	os << " Row  No " << v.Row.No << " "; SudokuApp::PrintLine(v.Row.RwData);
	
	//  os << " Clmn "; PrintLine(v.Clm.RwData);
	// PrintBoxMatrix(v.Box.srMtrx);
	// cout << "Search Entry ";
	return os;
}
/******************************************************************************/
TPosibility::TPosibility(TLine* er, TLine* ec, Uint& Prv) noexcept
	: Osdk{ er->Osdk }, Row{ *er }, Clm{ *ec }
	, Next{ Prv }
	, Lct{ Row.No, Clm.No }
	, Box{ Osdk.Boxes[(Lct.x / tree * tree) + Lct.y / tree] }
	, dVal{ Osdk.Mtrx[Lct.x][Lct.y] }
	, cVal{ Osdk.Mtrx[Lct.x][Lct.y] }
	, sPsbDigits({ 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u }) {
	++Prv;
	if (dVal == nullptr)
		GenerateSetofPosibility();
}
/******************************************************************************/
void TPosibility::GenerateSetofPosibility() {
	for (auto i = 0u; i < 2; ++i) {
		const TRw& RwData{ i == 0 ? Row.RwData : Clm.RwData };
		for (const auto& erw : RwData) {
			if (erw != nullptr) {
				auto it = find_if(sPsbDigits.begin(), sPsbDigits.end(),
					[&](auto& e) { return e == *erw;  });
				if (it != sPsbDigits.end())
					sPsbDigits.erase(it);
			}
		}
	}
	for (const auto& ebc : Box.Cells) {
		auto it = find_if(sPsbDigits.begin(), sPsbDigits.end(),
			[&](auto& e) { return e == ebc->Digit.No;  });
		if (it != sPsbDigits.end())
			sPsbDigits.erase(it);
	}
}
/******************************************************************************/
bool TPosibility::BackTrace() {
	bool found = false;
	Uint Entry;
	if (dVal == nullptr) {
		auto its{ sPsbDigits.begin() };
		while (its != sPsbDigits.end()) {
			tie(found, Entry) = CanbeInserted(its);
			if (found) {
				if (ContinueBackTrace(Entry))
					return true;
			}
			if (its == sPsbDigits.end()) break;
			++its;
		}
		return false;
	}
	if (Next > eighty) {
		Osdk.LastTrace = mdTerminate;
		return true;
	}
	if (Osdk.LastTrace == mdInsert)
		return ContinueBackTrace(ten);
	return false;
}

/******************************************************************************/
bool TPosibility::ContinueBackTrace(Uint Entry) {
	if (Entry < ten)
		InsertEntry(Entry);
	if (Next > 80) {
		Osdk.LastTrace = mdTerminate;
		return true;
	}
	auto it = next(Osdk.vPosible.begin(), Next);
	bool found{ (*it)->BackTrace() };
	if (found == false) {
		if (dVal == nullptr) {
			Row.UpdateRowDataDigit(Lct.y, nine);
			Clm.UpdateRowDataDigit(Lct.x, nine);
			TLoc bl{ Lct.x % tree, Lct.y % tree };
			Box.UpdateRowDataDigit(bl, nine);
			cVal = nullptr;
			Osdk.LastTrace = mdDelete;
		}
	}
	return found;
}
/******************************************************************************/
PNewItem TPosibility::CanbeInserted(TItvUint& its) {
	auto found = true;
	while (its != sPsbDigits.end()) {
		its = find_if(its, sPsbDigits.end(), [&](auto& Entry) {
			if (Box.RowMatrixhasaDigit(Entry))
				return true;
			if (Row.RowDatahasaDigit(Entry))
				return true;
			if (Clm.RowDatahasaDigit(Entry))
				return true;
			found = false;
			return  true;
		});
		if (!found) break;
		++its;
	}
	return make_pair(!found && its != sPsbDigits.end(), *its);

}
/******************************************************************************/
void TPosibility::InsertEntry(Uint Entry) {
	auto No{ Entry - 1 };
	Row.UpdateRowDataDigit(Lct.y, No);
	Clm.UpdateRowDataDigit(Lct.x, No);
	TLoc bl{ Lct.x % tree, Lct.y % tree };
	Box.UpdateRowDataDigit(bl, No);
	cVal = &Osdk.Digits[No].No;
	Osdk.LastTrace = mdInsert;
}