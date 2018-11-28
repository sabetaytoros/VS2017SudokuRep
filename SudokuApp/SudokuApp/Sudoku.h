
/* 
 * File:   Sudoku.h
 * Author: Sabetay Toros 
 *
 * Created on  27 Sep 18.15
 */
#ifndef SUDOKU_H
#define SUDOKU_H

using Uint = unsigned int;
using Uint = unsigned int;
constexpr Uint zero = 0u;
constexpr Uint one = 1u;
constexpr Uint two = 2u;
constexpr Uint tree = 3u;
constexpr Uint four = 4u;
constexpr Uint five = 5u;
constexpr Uint eight = 8u;
constexpr Uint nine = 9u;
constexpr Uint ten = 10u;
constexpr Uint eighty = 80u;
constexpr Uint eightyone = 81u;
using TRw =  array<Uint *, nine>;
using TDblMtrx = array<TRw, nine>;
using TBoxMtrx = array<array<Uint*, tree>, tree>;
struct TCell;
struct TDigit;
struct TBox;
struct TLine;
struct TRibbon;
struct TSudoku;
using TvUint = vector<Uint>;
using Tsfstream = shared_ptr<fstream>;
/******************************************************************************/
struct TSdkData {
    string Identity;
    bool intr = false;
    TvUint Data; 
    bool State = false;
    TSdkData(string s, fstream& fs);
};

struct TLoc;
using TvLoc = vector<TLoc>;

using TsCell = shared_ptr<TCell>;
using TvsCell = vector <TsCell > ;
using TvCell = vector<TCell *>;
enum TLineType { ltr, ltc } ;
struct TBox;
using TvBox = vector<TBox>;
using TvpBox = vector<TBox *>;
using TvDigits = vector<TDigit>;
using TvBox = vector<TBox>;
using TvpBox = vector<TBox *>;
using PNewItem = pair <bool, Uint>;
using TsLine = shared_ptr<TLine>;
using TvsLn = vector<TsLine>;
using TvsLine = vector<TvsLn>;
using TvpLine = vector<TLine *>;
using TCount = array<Uint, nine>;
using FoundP = pair<TLine *, TCell *>;
using TvRow = vector<TRw>;
struct TPosibility;
using TvsPosibility = vector<shared_ptr<TPosibility> >;
using TItvsPosibility =  TvsPosibility::iterator; 
using TItvUint = TvUint::iterator;
using TCounts = array<TCount, four>;
enum mdTrace { mdInsert, mdDelete, mdTerminate };
using TvsRbn = vector<shared_ptr<TRibbon> >;
using TvsRibbon = vector< TvsRbn >; 
/******************************************************************************/
namespace SudokuApp {
    template<typename T, size_t N> void stdPrintArray(const array<T,N>& A);
    void PrintCounts(const TCounts& Counts);
    void PrintLine(const TRw& e);
    void PrintDoubleArray(const string& s, const TDblMtrx& v);
    void PrintBoxMatrix(const TBoxMtrx& Box); 
    void PrintDigitDetails(const TvsCell& Cells);
    void PrintRows(const TvsLine& Rows);
    void PrintRibbons(const TvsRibbon& Ribbons);
};
/******************************************************************************/
struct TSudoku {
                ~TSudoku() {};
                 TSudoku(TSdkData& d) noexcept;
            void SelectaSolution();    
            void DigitSolution(); 
            void LineSolution(Uint n);
            void RibbonSolution();
            void BackTraceSolution();

        TDblMtrx Mtrx{ nullptr};        // Sudoku Row Matrix
        TDblMtrx Clmn{nullptr};         // Sudoku Column Matrix
         TCounts Counts{0};     
        TvDigits Digits;                //{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
           TvBox Boxes;                 //{ 0, 1, 2, 3, 4, 5, 6, 7, 8};
         TvsLine Rows;
       TvsRibbon Ribbons;
          TvCell Cells;
   TvsPosibility vPosible;
            bool Inserted;
            bool ChckR = false;
         mdTrace LastTrace = mdInsert;
};
/******************************************************************************/

#endif /* SUDOKU_H */