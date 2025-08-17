/* == LLSTAT.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Stat' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Stat
/* ------------------------------------------------------------------------- */
// ! Allows creating of tables with properly justified and proportioned data.
// ! Useful for the neat display of debug data to the console or logging to
// ! a file.
/* ========================================================================= */
namespace LLStat {                     // Stat namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStat::P;              using namespace IStd::P;
using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Stat common helper classes                                          ## **
** ######################################################################### **
** -- Read Stat class argument --------------------------------------------- */
struct AgStat : public ArClass<Stat> {
  explicit AgStat(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Stat>(lS, iArg, *cStats)}{} };
/* -- Create Stat class argument ------------------------------------------- */
struct AcStat : public ArClass<Stat> {
  explicit AcStat(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Stat>(lS, *cStats)}{} };
/* -- Read column argument (with maximum check) ---------------------------- */
struct AgColumn : public AgSSizeTLGE {
  explicit AgColumn(lua_State*const lS, const int iArg, const Stat &stCref) :
    AgSSizeTLGE{lS, iArg, 0, static_cast<ssize_t>(stCref.Headers())}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Stat:* member functions                                             ## **
** ######################################################################### **
** ========================================================================= */
// $ Stat:Cells
// < Count:integer=Number of cells added
// ? Returns the number of cells in the table
/* ------------------------------------------------------------------------- */
LLFUNC(Cells, 1, AgStat{lS, 1}().Cells())
/* ========================================================================= */
// $ Stat:Data
// > String:string=Data string to add to the cell.
// ? Inserts the specified string into the next cell.
/* ------------------------------------------------------------------------- */
LLFUNC(Data, 0, AgStat{lS, 1}().Data(AgString{lS, 2}))
/* ========================================================================= */
// $ Stat:DataB
// > String:string=Number of bytes to convert to readable.
// > Precision:integer=Amount of precision to use on the number.
// ? Formats the specified cell as bytes.
/* ------------------------------------------------------------------------- */
LLFUNC(DataB, 0,
  const AgStat aStat{lS, 1};
  const AgLuaInteger aValue{lS, 2};
  const AgInt aPrecision{lS, 3};
  aStat().DataB(aValue(), aPrecision))
/* ========================================================================= */
// $ Stat:DataFB
// > String:string=Number of bytes to convert to readable.
// > Precision:integer=Amount of precision to use on the number.
// ? Formats the specified cell as bytes but with a human readable number.
/* ------------------------------------------------------------------------- */
LLFUNC(DataFB, 0,
  const AgStat aStat{lS, 1};
  const AgLuaInteger aValue{lS, 2};
  const AgInt aPrecision{lS, 3};
  aStat().DataFB(aValue(), aPrecision))
/* ========================================================================= */
// $ Stat:DataFI
// > Value:integer=Integer to add to the next cell.
// ? Inserts the specified integer into the next cell but as a human readable
// ? number.
/* ------------------------------------------------------------------------- */
LLFUNC(DataFI, 0, AgStat{lS, 1}().DataFN(AgLuaInteger{lS, 2}()))
/* ========================================================================= */
// $ Stat:DataFN
// > Value:number=Number to add to the next cell.
// > Precision:integer=Amount of precision to use on the number.
// ? Inserts the specified number into the next cell but as a human readable
// ? number.
/* ------------------------------------------------------------------------- */
LLFUNC(DataFN, 0,
  const AgStat aStat{lS, 1};
  const AgLuaNumber aValue{lS, 2};
  const AgInt aPrecision{lS, 3};
  aStat().DataFN(aValue(), aPrecision))
/* ========================================================================= */
// $ Stat:DataI
// > Value:integer=Integer to add to the next cell.
// ? Inserts the specified integer into the next cell.
/* ------------------------------------------------------------------------- */
LLFUNC(DataI, 0, AgStat{lS, 1}().DataN(AgLuaInteger{lS, 2}()))
/* ========================================================================= */
// $ Stat:DataN
// > Value:number=Number to add to the next cell.
// > Precision:integer=Amount of precision to use on the number.
// ? Inserts the specified number into the next cell.
/* ------------------------------------------------------------------------- */
LLFUNC(DataN, 0,
  const AgStat aStat{lS, 1};
  const AgLuaNumber aValue{lS, 2};
  const AgInt aPrecision{lS, 3};
  aStat().DataN(aValue(), aPrecision))
/* ========================================================================= */
// $ Stat:Destroy
// ? Destroys the stat object and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Stat>(lS, 1, *cStats))
/* ========================================================================= */
// $ Stat:Finish
// > OmitLF:boolean=Omits the ending linefeed.
// > Gap:integer=Number of whitespaces between each header field.
// < Output:String=The final formatted output.
// ? Builds the whole formatted output and returns a string. All data and
// ? headers are erased and the class can be reused. Sizes and thus memory
// ? are preserved as per C++ STL rules when being reused.
/* ------------------------------------------------------------------------- */
LLFUNC(Finish, 1,
  const AgStat aStat{lS, 1};
  const AgBoolean aOmitLF{lS, 2};
  const AgSizeT aGap{lS, 3};
  LuaUtilPushVar(lS, aStat().Finish(aOmitLF, aGap)))
/* ========================================================================= */
// $ Stat:Header
// > Text:string=Header string to add to the cell.
// > Right:boolean=Header is right justified?
// ? Inserts the specified string into the next header. This has no effect if
// ? data has been added.
/* ------------------------------------------------------------------------- */
LLFUNC(Header, 0,
  const AgStat aStat{lS, 1};
  const AgString aText{lS, 2};
  const AgBoolean aRight{lS, 3};
  aStat().Header(aText, aRight))
/* ========================================================================= */
// $ Stat:HeaderDupe
// > Count:integer=Number of times to duplicate the current headers.
// ? Duplicates the currently stored headers.
/* ------------------------------------------------------------------------- */
LLFUNC(HeaderDupe, 0, AgStat{lS, 1}().DupeHeader(AgSizeT{lS, 2}))
/* ========================================================================= */
// $ Stat:Headers
// < Count:integer=Number of headers registered
// ? Returns the number of headers in the table
/* ------------------------------------------------------------------------- */
LLFUNC(Headers, 1, AgStat{lS, 1}().Headers())
/* ========================================================================= */
// $ Stat:Id
// < Id:integer=The id number of the Stat object.
// ? Returns the unique id of the Stat object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgStat{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Stat:Name
// < Id:string=The stat identifier
// ? Returns the identifier of the Stat object.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgStat{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Stat:Reserve
// > Rows:integer=Reserve memory for this many rows.
// ? Reserves memory for this many rows.
/* ------------------------------------------------------------------------- */
LLFUNC(Reserve, 0, AgStat{lS, 1}().Reserve(AgSizeT{lS, 2}))
/* ========================================================================= */
// $ Stat:Rows
// < Count:integer=Number of rows registered
// ? Returns the number of complete rows in the table
/* ------------------------------------------------------------------------- */
LLFUNC(Rows, 1, AgStat{lS, 1}().Rows())
/* ========================================================================= */
// $ Stat:Sort
// > Column:integer=Sort from this column
// > Reverse:boolean=Sort the list in descending order
// ? Sorts the data by the specified column.
/* ------------------------------------------------------------------------- */
LLFUNC(Sort, 0,
  const AgStat aStat{lS, 1};
  const AgColumn aColumn{lS, 2, aStat};
  const AgBoolean aReverse{lS, 3};
  aStat().Sort(aColumn, aReverse))
/* ========================================================================= */
// $ Stat:SortTwo
// > Column1:Integer=Sort from this primary column
// > Column2:Integer=Sort from this secondary column if primary is the same
// > Reverse:Boolean=Sort the list in descending order
// ? Sorts the data by the specified columns.
/* ------------------------------------------------------------------------- */
LLFUNC(SortTwo, 0,
  const AgStat aStat{lS, 1};
  const AgColumn aColumn1{lS, 2, aStat},
                 aColumn2{lS, 3, aStat};
  const AgBoolean aReverse{lS, 4};
  aStat().SortTwo(aColumn1, aColumn2, aReverse))
/* ========================================================================= **
** ######################################################################### **
** ## Stat:* member functions structure                                   ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // stat:* member functions begin
  LLRSFUNC(Cells),   LLRSFUNC(Data),   LLRSFUNC(DataB),  LLRSFUNC(DataFB),
  LLRSFUNC(DataFI),  LLRSFUNC(DataFN), LLRSFUNC(DataI),  LLRSFUNC(DataN),
  LLRSFUNC(Destroy), LLRSFUNC(Finish), LLRSFUNC(Header), LLRSFUNC(HeaderDupe),
  LLRSFUNC(Headers), LLRSFUNC(Id),     LLRSFUNC(Name),   LLRSFUNC(Reserve),
  LLRSFUNC(Rows),    LLRSFUNC(Sort),   LLRSFUNC(SortTwo),
LLRSEND                                // Stat:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Stat:* namespace functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Stat.Count
// < Count:integer=Total number of stats created.
// ? Returns the total number of stat classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(Count, 1, LuaUtilPushVar(lS, cStats->CollectorCount()))
/* ========================================================================= */
// $ Stat.Create
// > Name:string=Name of the class
// ? Creates an empty stat table.
/* ------------------------------------------------------------------------- */
LLFUNC(Create, 1, AcStat{lS}().IdentSet(AgNeString{lS, 1}))
/* ========================================================================= **
** ######################################################################### **
** ## Stat:* namespace functions structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Bin.* namespace functions begin
  LLRSFUNC(Count), LLRSFUNC(Create),
LLRSEND                                // Bin.* namespace functions end
/* ========================================================================= */
}                                      // End of Bin namespace
/* == EoF =========================================================== EoF == */
