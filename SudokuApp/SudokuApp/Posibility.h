#pragma once

/*
 * File:   Posibility.h
 * Author: Sabetay Toros
 *
 * Created on 31 October 2018, 12:00
 */


 /******************************************************************************/
struct TPosibility {
	TPosibility(TLine* re, TLine* ce, Uint& Prv) noexcept;
	void GenerateSetofPosibility();
	bool BackTrace();
	PNewItem CanbeInserted(TItvUint& its);
	bool ContinueBackTrace(Uint Entry);
	void InsertEntry(Uint Entry);
	friend ostream& operator<<(ostream& os, const TPosibility& v);

	TSudoku& Osdk;                  // Owner 
	TLine& Row;                   // Row
	TLine& Clm;                   // Column 
	Uint Next;                 // Next this is obselete
	const TLoc Lct;                   // Location
	TBox& Box;                   // Box 
	const Uint* dVal;                   // Assigned Default Digit 
	Uint* cVal;                   // Current value
	TvUint sPsbDigits;            // Set of Posibble Digits 
};



