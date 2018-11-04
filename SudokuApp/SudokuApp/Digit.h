#pragma once
/******************************************************************************/
struct TLoc {
	const Uint x;
	const Uint y;
	TLoc(Uint i, Uint j) : x{ i }, y{ j } {};
	~TLoc() {  } // cout <<  " Loc destroy edildi "  << endl;
	TLoc operator+(const TLoc& l) { return TLoc(x + l.x, y + l.y); }
	TLoc operator-(const TLoc& l) { return TLoc(x - l.x, y - l.y); }
	friend ostream& operator<<(ostream& os, const TLoc& v);
};
/******************************************************************************/
struct TCell {
	TDigit &Digit;
	const TLoc Lct;
	const Uint ta; // Triple No
	const Uint tr;
	const Uint tc;
	bool isData = false;
	TCell(TDigit* o, TLoc lct, bool d);
	friend ostream& operator<<(ostream& os, const TCell& It);
};
/******************************************************************************/
struct TDigit {
	TDigit(TSudoku * o, Uint No_);
	bool operator<(const TDigit& c) const {
		return  Count < c.Count && c.Count < nine;
	}
	friend ostream& operator<<(ostream& os, const TDigit& v);
	void GenerateItem(TLoc& lc, bool Dflt);
	void Solution();
	void FindMissingRow(TRw& Row, Uint&  mr); // Visual Studio does not accept auto parameter
	void InsertFinalCell();
	bool FillTestRowWithNonCellValues(TRw& Ln, TRw& tstV, Uint mr, TLineType t);
	bool InsertNewItem(TLoc& lc);
	void InsertFoundLineLocation(const Uint& Ln, const TLineType& lt, TRw& cRow);
	bool InserttoOneofLocations(const Uint& Ln, const TLineType& lt, TRw& cRow, TvpBox& Box);

	TSudoku &Osdk;
	Uint &No;
	TvsCell Cells;
	Uint Count{ 0 };
	TRw tstH = { nullptr };
	TRw tstV = { nullptr };
};
