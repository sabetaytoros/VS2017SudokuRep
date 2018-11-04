#pragma once

/*****************************************************************T*************/
struct TBox {
	TBox(TSudoku *o, Uint  tn_) noexcept;
	friend ostream& operator<<(ostream& os, const TBox& v);
	void AddCell(TCell& Cell);
	void Solution();
	bool Solution(Uint MsClNo);
	TBoxMtrx GenerateChechSubMatrix(TDigit& Digit);
	optional<TLoc> FindInsertionLocation(TBoxMtrx& cMtrx);
	void InsertFinalCell();
	void InsertOtherCells();
	bool RowMatrixhasaDigit(const Uint Entry);
	void UpdateRowDataDigit(const TLoc& l, const Uint No);

	TSudoku &Osdk;
	Uint tn;                    // Triple No
	const TLoc Lct;                     // Absolute Location 
	const TLoc Rbn;
	TLineType CRbn = ltr;
	TBoxMtrx srMtrx{ nullptr };   //  Row Box Sub Matrix
	TBoxMtrx scMtrx{ nullptr };   //  Clmn Box Sub Matrix
	TvCell Cells;    //  
};
