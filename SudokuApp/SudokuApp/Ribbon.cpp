#include "pch.h"  // Precompiled header
#include "SudokuApp.h"
#include "Digit.h"
#include "Box.h"
#include "Line.h"
#include "Ribbon.h"

extern const char* str[];
namespace SudokuApp {
	/**************************************************************************/
	void PrintRibbons(const TvsRibbon& Ribbons) {
		for (Uint k = 0u; k < two; ++k) {
			cout << endl << (k == 0 ? " Row  Ribbons " : " Column Ribbons ") << endl;
			//           for (const TvsRbn& e : Ribbons[k])cout << *e << endl;

		}
	}
}
/******************************************************************/
ostream& operator<<(ostream& os, const TRibbon& v) {
	os << str[v.lt] << " Ribbons " << v.No << endl;
	for (auto& r : v.RvLines) {
		os << *r << endl;
	}
	os << " Boxes " << endl;
	for (auto& t : v.vpBoxes)
		os << *t << endl;

	os << " Counts " << endl;
	for (const auto& e : v.Counts)
		os << e << " ";
	os << endl;
	return os;
}
/******************************************************************************/
TRibbon::TRibbon(TSudoku *O, Uint n, TLineType t) noexcept
	: Osdk{ *O }, No{ n }, lt{ t }    {
	auto r = n * 3;
	auto m = (lt == ltr) ? r : n;
	for (Uint i = 0; i < 3; i++) {
		TsLine& sLine = Osdk.Rows[lt][r];
		RvLines.push_back(sLine.get());
		vpBoxes.push_back(&Osdk.Boxes[m]);
		++r;
		m += (lt == ltr) ? 1 : 3;
	}
}
/******************************************************************************/
void TRibbon::AddCount(TCell& Cell) {
	++Counts[Cell.Digit.No - 1];
}
/*******************************************************************************/
void TRibbon::CheckMissingOtherCell(TCell& Cell) {
	if (Counts[Cell.Digit.No - 1] == tree) return;
	if (CheckMissingCellinOtherLines(Cell)) {
		for (auto& e : vpBoxes) {
			auto Box(*e);
			if (Box.tn == Cell.ta || Box.Cells.size() < nine) continue;
			auto itt = find_if(Box.Cells.begin(), Box.Cells.end(), [&](auto& eb) {
				return eb->Digit.No == Cell.Digit.No; });
			if (itt == Box.Cells.end()) {
				Box.CRbn = lt;
				Box.Solution(Cell.Digit.No);
				if (Osdk.Cells.size() == eightyone || Counts[Cell.Digit.No - 1] == tree) return;
			}
		}
	}
	else CheckMissingCellinOtherBoxes(Cell);
	if (Cell.Digit.Count > 5) {
		if (Osdk.Cells.size() == eightyone) return;
		Cell.Digit.Solution();
	}
}

/*******************************************************************************/
bool TRibbon::CheckMissingCellinOtherLines(TCell& Cell) {
	auto count = count_if(RvLines.begin(), RvLines.end(),
		[&](auto & eLn) {
			TLine& Ln{ *eLn };
			auto it = find_if(Ln.Cells.begin(), Ln.Cells.end(), [&](auto& ec) {
				return ec->Digit.No == Cell.Digit.No;  });
			return it != eLn->Cells.end();
		});
	return count == 1;
}
/*******************************************************************************/
void TRibbon::CheckMissingCellinOtherBoxes(TCell& Cell) {
	for (auto& e : vpBoxes) {
		auto& Box(*e);
		if (Box.tn == Cell.ta) continue;
		auto itt = find_if(Box.Cells.begin(), Box.Cells.end(), [&](auto& ti) {
			return ti->Digit.No == Cell.Digit.No; });
		if (itt == Box.Cells.end()) {
			Box.CRbn = lt;
			Box.Solution(Cell.Digit.No);
			if (Counts[Cell.Digit.No - 1] == tree) return;
		}
	}
}
/*******************************************************************************/
void TRibbon::CheckMissingThirdDigit(const Uint& n) {
	TDigit& Digit = *(next(Osdk.Digits.begin(), n));
	auto it = find_if(RvLines.begin(), RvLines.end(), [&](auto & el) {
		auto &Line{ *el };
		auto itc = find_if(Line.Cells.begin(), Line.Cells.end(), [&](auto& ei) {
			return ei->Digit.No == Digit.No;
		});
		return itc == Line.Cells.end();
	});
	TLine& Ln{ **it };
	auto itb = find_if(vpBoxes.begin(), vpBoxes.end(), [&](auto& eb) {
		auto itc = find_if(eb->Cells.begin(), eb->Cells.end(), [&](auto& ei) {
			return ei->Digit.No == Digit.No;
		});
		return itc == eb->Cells.end();
	});
	const auto& Loc = (*itb)->Lct;
	TRw cv{ lt == ltr ? Digit.tstV : Digit.tstH };
	auto itc{ next(cv.begin(), lt == ltr ? Loc.y : Loc.x) };
	auto itl{ next(Ln.RwData.begin(),  lt == ltr ? Loc.y : Loc.x) };
	auto c = 0u;
	auto fit = cv.end();
	for (auto i = 0; i < 3; ++i) {
		if (*itl != nullptr)
			*itc = *itl;
		if (*itc != nullptr)
			++c;
		else if (fit == cv.end())
			fit = itc;
		else  return;
		++itc;
		++itl;
	}
	if (c == two) {
		auto d = static_cast<Uint> (distance(cv.begin(), fit));
		TLoc nl{ (lt == ltr ? Ln.No : d) ,  (lt == ltr ? d : Ln.No) };
		Digit.InsertNewItem(nl);
	}
}