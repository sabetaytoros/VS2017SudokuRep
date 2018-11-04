#pragma once

 /*
  * File:   Ribbon.h
  * Author: Sabetay Tporos
  *
  * Created on 31 October 2018, 12:11
  */


/******************************************************************************/
struct TRibbon {
	TRibbon(TSudoku *O, Uint n, TLineType t) noexcept;
	~TRibbon() {  }
	friend ostream& operator<<(ostream& os, const TRibbon& v);
	void AddCount(TCell& Cell);
	void CheckMissingOtherCell(TCell& Cell);
	bool CheckMissingCellinOtherLines(TCell& Cell);
	void CheckMissingCellinOtherBoxes(TCell& Cell);
	void CheckMissingThirdDigit(const Uint& d);

	TSudoku &Osdk;             // Owner
	Uint  No;                // Ribbon No
	const TLineType lt;                // Horizontal, Vertical
	TvpLine RvLines;           // Lines
	TvpBox vpBoxes;            // Triples
	TCount Counts{ 0 };      // Cell Counts in Ribbon Max 3
	Uint ctn;               // current triple no when Checked

};



