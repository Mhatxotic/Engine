/* == STAT.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## A class that arranges data into a table and neatly provides a       ## **
** ## cleanly, human-readable, easy to read formatted output. Mainly used ## **
** ## for justified data output in console.                               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IStat {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IError::P;
using namespace IIdent::P;             using namespace ILuaLib::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace IUtf;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Statistic class ------------------------------------------------------ */
class Statistic
{ /* -- Private typedefs --------------------------------------------------- */
  typedef ios_base &(*JCCallback)(ios_base&);
  /* ----------------------------------------------------------------------- */
  struct Head                          // Column data
  { /* --------------------------------------------------------------------- */
    const string   strName;            // Column name
    JCCallback     jccFunc;            // Justification callback function
    int            iMaxLen;            // Maximum header length
  };/* --------------------------------------------------------------------- */
  typedef deque<Head> HeadDeque;       // Column header data list
  /* -- Private variables -------------------------------------------------- */
  HeadDeque        hdHeaders;          // Headers dataz
  StrVector        svValues;           // Values list
  /* -- Format output iterator data without a suffix ----------------------- */
  void ProcValNoSuf(ostringstream &osS, const StrVectorConstIt &svciIt,
    const Head &hRef)
  { osS << hRef.jccFunc << setw(hRef.iMaxLen) << StdMove(*svciIt); }
  /* -- Format output iterator data with a suffix -------------------------- */
  void ProcValSuf(ostringstream &osS, const StrVectorConstIt &svciIt,
    const Head &hRef, const string &strSuffix)
  { osS << hRef.jccFunc << setw(hRef.iMaxLen)
        << StdMove(*svciIt) << strSuffix; }
  /* -- Format empty output data without a suffix -------------------------- */
  void ProcHdrNoSuf(ostringstream &osS, const Head &hRef)
    { osS << hRef.jccFunc << setw(hRef.iMaxLen) << StdMove(hRef.strName); }
  /* -- Format empty output data with a suffix ----------------------------- */
  void ProcHdrSuf(ostringstream &osS, const Head &hRef,
    const string &strSuffix)
  { osS << hRef.jccFunc << setw(hRef.iMaxLen)
        << StdMove(hRef.strName) << strSuffix; }
  /* -- Used by Finish() which returns the last adjusted header item ------- */
  Head &GetLastHdr(const size_t stHM1)
  { // Get the last header item
    Head &hRef = hdHeaders[stHM1];
    // Set the length to zero if left align is selected
    if(hRef.jccFunc == left) hRef.iMaxLen = 0;
    // Return the iterator
    return hRef;
  }
  /* -- Check that we have headers ----------------------------------------- */
  void CheckHeaderCount(void) const
    { if(hdHeaders.empty()) XC("No headers!"); }
  /* -- Check row not finished --------------------------------------------- */
  void CheckRowNotFinished(void) const
    { if(const size_t stRow = svValues.size() % hdHeaders.size())
        XC("Row not finished!",
           "ExpectCols", hdHeaders.size(), "ActualCols", stRow,
           "RowCount", svValues.size() / hdHeaders.size()); }
  /* -- Finish with existing string stream ------------------------- */ public:
  void Finish(ostringstream &osS, const bool bAddLF=true, const size_t stGap=1)
  { // Check that there are headers
    CheckHeaderCount();
    // Get headers size minus one
    const size_t stHM1 = Headers() - 1;
    // Create string for gap
    const string strGap(stGap, ' '), &strLF = cCommon->Lf();
    // Proc headers except the last header item
    for(size_t stHIndex = 0; stHIndex < stHM1; ++stHIndex)
      ProcHdrSuf(osS, hdHeaders[stHIndex], strGap);
    // We're on the last header cell and no values? so we're done
    if(svValues.empty())
    { // Write last item with line-feed?
      if(bAddLF) ProcHdrSuf(osS, GetLastHdr(stHM1), strLF);
      // Write last item without line-feed?
      else ProcHdrNoSuf(osS, GetLastHdr(stHM1));
    } // We have values?
    else
    { // Write the last item with a line feed
      ProcHdrSuf(osS, GetLastHdr(stHM1), strLF);
      // Fill in rest of missing header columns with blanks. This is so we
      // don't need to add extra condition checks which would increase
      // processing time.
      while(svValues.size() % Headers()) svValues.push_back({});
      // Get last iterator and iterator to first item
      const StrVectorConstIt vLast{ prev(svValues.cend()) };
      StrVectorIt svciIt{ svValues.begin() };
      // Get total row count and if we have more than zero rows to print?
      if(size_t stRows = svValues.size() / Headers())
      { // Get last iterator of the penultimate row
        const StrVectorConstIt svciLastRowIt{
          prev(svValues.cend(), static_cast<ssize_t>(Headers())) };
        // If we have more than 1 row? Repeat...
        if(stRows > 1) do
        { // Proc headers except the last header item
          for(size_t stHIndex = 0; stHIndex < stHM1; ++stHIndex, ++svciIt)
            ProcValSuf(osS, svciIt, hdHeaders[stHIndex], strGap);
          // Add the last item in row
          ProcValSuf(osS, svciIt, hdHeaders[stHM1], strLF);
        } // ...Until we are at the last row and first header in the last
        while(++svciIt != svciLastRowIt);
        // Proc headers on the last row except the last header item
        for(size_t stHIndex = 0; stHIndex < stHM1; ++stHIndex, ++svciIt)
          ProcValSuf(osS, svciIt, hdHeaders[stHIndex], strGap);
      } // Proc the last item with carriage return if reuqested
      if(bAddLF) ProcValSuf(osS, vLast, hdHeaders[stHM1], strLF);
      // Not requested so process the last item without a carriage return
      else ProcValNoSuf(osS, vLast, hdHeaders[stHM1]);
      // Clear values
      svValues.clear();
    } // Clear headers
    hdHeaders.clear();
  }
  /* -- Headers count ------------------------------------------------------ */
  size_t Headers(void) const { return hdHeaders.size(); }
  /* -- Finish with new string stream -------------------------------------- */
  const string Finish(const bool bAddLF=true, const size_t stGap=1)
  { // Output stream
    ostringstream osS;
    // Do the format
    Finish(osS, bAddLF, stGap);
    // Return the string
    return osS.str();
  }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    Statistic &DataN(const IntType tNum, const int iP=0)
      { return Data(StdMove(StrFromNum<IntType>(tNum, 0, iP))); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    Statistic &DataB(const IntType tNum, const int iP=0)
      { return Data(StrToBytes(static_cast<uint64_t>(tNum), iP)); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    Statistic &DataH(const IntType tNum, const int iP=0)
      { return Data(StdMove(StrHexFromInt<IntType>(tNum, iP))); }
  /* -- Data by character -------------------------------------------------- */
  Statistic &DataC(const char cCharacter = '?')
  { // Return if there are no headers
    if(hdHeaders.empty()) return *this;
    // Get pointer to header data
    Head &hRef = hdHeaders[svValues.size() % Headers()];
    // Copy the character into the last slot
    svValues.insert(svValues.cend(), { &cCharacter, 1 });
    // Initialise maximum length if not set
    if(hRef.iMaxLen <= 0) hRef.iMaxLen = 1;
    // Return self so we can daisy chain
    return *this;
  }
  /* -- Data by read only c-string ----------------------------------------- */
  Statistic &Data(const char*const cpStr)
  { // Return if there are no headers
    if(hdHeaders.empty()) return *this;
    // Get pointer to header data
    Head &hRef = hdHeaders[svValues.size() % Headers()];
    // Copy the value into the last and put the string we inserted into a
    // decoder and get length of the utf8 string
    const int iLength = UtilIntOrMax<int>(
      UtfDecoder{ *svValues.insert(svValues.cend(), cpStr) }.Length());
    // If the length of this value is longer and is not the last header value
    // then set the header longer
    if(iLength > hRef.iMaxLen) hRef.iMaxLen = iLength;
    // Return self so we can daisy chain
    return *this;
  }
  /* -- Data by read only string view -------------------------------------- */
  Statistic &Data(const string_view &strvVal)
  { // Return if there are no headers
    if(hdHeaders.empty()) return *this;
    // Get pointer to header data
    Head &hRef = hdHeaders[svValues.size() % Headers()];
    // Copy the value into the last and put the string we inserted into a
    // decoder and get length of the utf8 string
    const int iLength = UtilIntOrMax<int>(
      UtfDecoder{ *svValues.insert(svValues.cend(),
        string{ strvVal }) }.Length());
    // If the length of this value is longer and is not the last header value
    // then set the header longer
    if(iLength > hRef.iMaxLen) hRef.iMaxLen = iLength;
    // Return self so we can daisy chain
    return *this;
  }
  /* -- Appended data ------------------------------------------------------ */
  template<typename ...VarArgs>Statistic &DataA(const VarArgs &...vaArgs)
    { return Data(StrAppend(vaArgs...)); }
  /* -- Formatted data ----------------------------------------------------- */
  template<typename ...VarArgs>Statistic &DataF(const char*const cpFormat,
    const VarArgs &...vaArgs) { return Data(StrFormat(cpFormat, vaArgs...)); }
  /* -- Data by read-only lvalue string copy ------------------------------- */
  Statistic &Data(const string &strVal=cCommon->Blank())
  { // Return if there are no headers
    if(hdHeaders.empty()) return *this;
    // Get pointer to header data
    Head &hRef = hdHeaders[svValues.size() % Headers()];
    // Copy the value into the last and put the string we inserted into a
    // decoder and get length of the utf8 string
    const int iLength = UtilIntOrMax<int>(
      UtfDecoder{ *svValues.insert(svValues.cend(), strVal) }.Length());
    // If the length of this value is longer and is not the last header value
    // then set the header longer
    if(iLength > hRef.iMaxLen) hRef.iMaxLen = iLength;
    // Return self so we can daisy chain
    return *this;
  }
  /* -- Reserve memory for this many rows ---------------------------------- */
  Statistic &Reserve(const size_t stRows)
    { svValues.reserve(stRows * Headers()); return *this; }
  /* -- Data by rvalue string move ----------------------------------------- */
  Statistic &Data(string &&strVal)
  { // Return if there are no headers
    if(hdHeaders.empty()) return *this;
    // Get pointer to header data
    Head &hRef = hdHeaders[svValues.size() % Headers()];
    // Move the value into the list and put the tring we inserted into
    // a decoder and get length of the utf8 string
    const int iLength = UtilIntOrMax<int>(
      UtfDecoder(*svValues.insert(svValues.cend(), StdMove(strVal))).Length());
    // If the length of this value is longer and is not the last header value
    // then set the header longer
    if(iLength > hRef.iMaxLen) hRef.iMaxLen = iLength;
    // Return self so we can daisy chain
    return *this;
  }
  /* -- Data by read-only lvalue widestring copy --------------------------- */
  Statistic &DataW(const wstring &wstrVal={})
    { return Data(UtfFromWide(wstrVal.c_str())); }
  /* -- Duplicate current headers ------------------------------------------ */
  Statistic &DupeHeader(const size_t stCount=1)
  { // Have headers? Duplicate the headers the specified number of times
    if(!hdHeaders.empty())
      for(size_t stIndex = 0, stHdrCount = Headers();
                 stIndex < stCount;
               ++stIndex)
        for(size_t stHdrIndex = 0; stHdrIndex < stHdrCount; ++stHdrIndex)
          hdHeaders.emplace_back(hdHeaders[stHdrIndex]);
    // Return self so we can daisy chain
    return *this;
  }
  /* -- Sort a table by specified primary or secondary column -------------- */
  void SortTwo(const ssize_t sstColPri, const ssize_t sstColSec,
    const bool bDescending=false)
  { // Check that there are headers
    CheckHeaderCount();
    // Check that both sort columns aren't the same
    if(sstColPri == sstColSec) XC("Both columns same!");
    // Check that the row has been finished
    CheckRowNotFinished();
    // Sorting list
    struct StrRef { StrVectorIt sviRowIt, sviColPri, sviColSec; };
    typedef vector<StrRef> StrRefVec;
    StrRefVec srvList;
    srvList.reserve(svValues.size() / Headers());
    // Get headers as ssize_t (prevents signed casting warning).
    const ssize_t sstHeaders = UtilIntOrMax<ssize_t>(Headers());
    // For each value. Add row start iterator and column iterator to list
    for(StrVectorIt sviRowIt{ svValues.begin() };
                    sviRowIt != svValues.end();
                    sviRowIt = next(sviRowIt, sstHeaders))
      srvList.push_back({ sviRowIt, next(sviRowIt, sstColPri),
                                  next(sviRowIt, sstColSec) });
    // Now enumerate all the primary and secondary columns and sort them
    StdSort(par_unseq, srvList.begin(), srvList.end(), bDescending ?
      [](const StrRef &srRow1, const StrRef &srRow2)
      { // Cache primary and secondary string refs of each column
        const string &strRow1Pri = *srRow1.sviColPri,
                     &strRow1Sec = *srRow1.sviColSec,
                     &strRow2Pri = *srRow2.sviColPri,
                     &strRow2Sec = *srRow2.sviColSec;
        // Return if primary greater, or equal and secondary is greater
        return strRow1Pri > strRow2Pri ||
              (strRow1Pri == strRow2Pri &&
               strRow1Sec > strRow2Sec);
      } :
      [](const StrRef &srRow1, const StrRef &srRow2)
      {  // Cache primary and secondary string refs of each column
        const string &strRow1Pri = *srRow1.sviColPri,
                     &strRow1Sec = *srRow1.sviColSec,
                     &strRow2Pri = *srRow2.sviColPri,
                     &strRow2Sec = *srRow2.sviColSec;
        // Return if primary lesser, or equal and secondary is lesser
        return strRow1Pri < strRow2Pri ||
              (strRow1Pri == strRow2Pri &&
               strRow1Sec < strRow2Sec);
      });
    // Now we have the sorted list we can rebuilding the new list
    StrVector svValuesNew;
    svValuesNew.reserve(svValues.size());
    for(const StrRef &srRow : srvList)
      for(StrVectorIt sviColIt{ srRow.sviRowIt },
                      sviColEnd{ next(sviColIt, sstHeaders) };
                      sviColIt != sviColEnd;
                    ++sviColIt)
        svValuesNew.emplace_back(StdMove(*sviColIt));
    // Swap new list with old one
    svValues.swap(svValuesNew);
  }
  /* -- Sort a table by specified primary column --------------------------- */
  void Sort(const ssize_t sstColumn, const bool bDescending=false)
  { // Check that there are headers
    CheckHeaderCount();
    // Check that the row has been finished
    CheckRowNotFinished();
    // Sorting list
    struct StrRef { StrVectorIt sviRowIt, sviColIt; };
    typedef vector<StrRef> StrRefVec;
    StrRefVec srvList;
    srvList.reserve(svValues.size() / Headers());
    // Get headers as ssize_t (prevents signed casting warning).
    const ssize_t sstHeaders = UtilIntOrMax<ssize_t>(Headers());
    // For each value. Add row start iterator and column iterator to list
    for(StrVectorIt sviRowIt{ svValues.begin() };
                    sviRowIt != svValues.end();
                    sviRowIt = next(sviRowIt, sstHeaders))
      srvList.push_back({ sviRowIt, next(sviRowIt, sstColumn) });
    // Now enumerate all the primary columns and sort them
    StdSort(par_unseq, srvList.begin(), srvList.end(), bDescending ?
      [](const StrRef &srRow1, const StrRef &srRow2)
        { return *srRow1.sviColIt > *srRow2.sviColIt; } :
      [](const StrRef &srRow1, const StrRef &srRow2)
        { return *srRow1.sviColIt < *srRow2.sviColIt; });
    // Now we have the sorted list we can rebuilding the new list
    StrVector svValuesNew;
    svValuesNew.reserve(svValues.size());
    for(const StrRef &srRow : srvList)
      for(StrVectorIt sviColIt{ srRow.sviRowIt },
                      sviColEnd{ next(sviColIt, sstHeaders) };
                      sviColIt != sviColEnd;
                    ++sviColIt)
        svValuesNew.emplace_back(StdMove(*sviColIt));
    // Swap new list with old one
    svValues.swap(svValuesNew);
  }
  /* -- Add a header and return self --------------------------------------- */
  Statistic &Header(const string &strH, const bool bRJ, const size_t stL=0)
  { // Push the header item if there are values as this will mess everything
    // up. Make sure the first column is always left justified.
    if(svValues.empty())
      hdHeaders.push_back({ strH, bRJ ? right : left,
        UtilIntOrMax<int>(UtilMaximum(stL, strH.length())) });
    // Return self so we can daisy chain
    return *this;
  }
  /* -- Add an empty header ------------------------------------------------ */
  Statistic &Header(const string &strH=cCommon->Blank(), const size_t stL=0)
    { return Header(strH, !hdHeaders.empty(), stL); }
  /* -- Add data by pointer ------------------------------------------------ */
  Statistic &DataV(const void*const vpAddr) { return Data(StrAppend(vpAddr)); }
  /* -- Constructor that does nothing -------------------------------------- */
  Statistic(void) { }
};/* ----------------------------------------------------------------------- */
/* == Stat object collector and object class =============================== */
CTOR_BEGIN_DUO(Stats, Stat, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua garbage collect instruction
  public Ident,                        // Identifier
  public Statistic                     // Statistics data
{ /* -- Constructor ------------------------------------------------ */ public:
  Stat(void) :                         // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperStat{ cStats, this },      // Automatic (de)registration
    IdentCSlave{ cParent->CtrNext() }  // Initialise identification number
    /* -- No code ---------------------------------------------------------- */
    { }                                // Do nothing else
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Stat)                // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Stats, Stat, STAT)    // End of stat objects collector
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* == EoF =========================================================== EoF == */
