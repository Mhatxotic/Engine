/* == COMMON.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Common constant variables.                                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICommon {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Common class with common objects ------------------------------------- */
class Common;                          // Prototype
static Common *cCommon = nullptr;      // Global access to class
class Common                           // Common variables class
{ /* -- Common string views ------------------------------------------------ */
  const StdStringView strvDir, strvEnt, strvFs, strvHttp, strvHttps,
    strvLuaName, strvTimeout, strvPipe, strvColon, strvSpace, strvQuote,
    strvEquals, strvBlank, strvZero, strvOne, strvDblSpace, strvFSlash;
  /* -- Common strings ----------------------------------------------------- */
  const StdString strTrue, strFalse, strY, strN, strSpace, strBlank, strCr,
    strLf, strCrLf, strCrLf2, strLfCr, strUnspec, strNull,
    strPeriod, str2Period, strEllipsis, strPrivate, strProtected, strEmpty,
    strInvalid, strAsterisk, strNil, strUnresolved, strZero, strOne;
  /* -- Miscellaneous common variables ------------------------------------- */
  const char*const cpBlank;            // Blank C-String
  StdLocale        lLocaleCurrent;     // Current locale
  /* --------------------------------------------------------------- */ public:
  const StdLocale &CommonLocale() const { return lLocaleCurrent; }
  void CommonSetLocale(const StdString &strLocale)
    { lLocaleCurrent = StdLocale{ strLocale }; }
  /* -- Return string view functions --------------------------------------- */
  const StdStringView &CommonFsV() const { return strvFs; }
  const StdStringView &CommonDirV() const { return strvDir; }
  const StdStringView &CommonEntV() const { return strvEnt; }
  const StdStringView &CommonHttpV() const { return strvHttp; }
  const StdStringView &CommonHttpsV() const { return strvHttps; }
  const StdStringView &CommonLuaNameV() const { return strvLuaName; }
  const StdStringView &CommonTimeoutV() const { return strvTimeout; }
  const StdStringView &CommonPipeV() const { return strvPipe; }
  const StdStringView &CommonColonV() const { return strvColon; }
  const StdStringView &CommonSpaceV() const { return strvSpace; }
  const StdStringView &CommonQuoteV() const { return strvQuote; }
  const StdStringView &CommonEqualsV() const { return strvEquals; }
  const StdStringView &CommonBlankV() const { return strvBlank; }
  const StdStringView &CommonZeroV() const { return strvZero; }
  const StdStringView &CommonOneV() const { return strvOne; }
  const StdStringView &CommonDblSpaceV() const { return strvDblSpace; }
  const StdStringView &CommonFSlashV() const { return strvFSlash; }
  /* -- Return string functions -------------------------------------------- */
  const StdString &CommonBlank() const { return strBlank; }
  const StdString &CommonTrue() const { return strTrue; }
  const StdString &CommonFalse() const { return strFalse; }
  const StdString &CommonYes() const { return strY; }
  const StdString &CommonNo() const { return strN; }
  const StdString &CommonCr() const { return strCr; }
  const StdString &CommonLf() const { return strLf; }
  const StdString &CommonCrLf() const { return strCrLf; }
  const StdString &CommonCrLf2() const { return strCrLf2; }
  const StdString &CommonLfCr() const { return strLfCr; }
  const StdString &CommonSpace() const { return strSpace; }
  const StdString &CommonEllipsis() const { return strEllipsis; }
  const StdString &CommonUnspec() const { return strUnspec; }
  const StdString &CommonUnresolved() const { return strUnresolved; }
  const StdString &CommonNull() const { return strNull; }
  const StdString &CommonPeriod() const { return strPeriod; }
  const StdString &CommonTwoPeriod() const { return str2Period; }
  const StdString &CommonPrivate() const { return strPrivate; }
  const StdString &CommonProtected() const { return strProtected; }
  const StdString &CommonEmpty() const { return strEmpty; }
  const StdString &CommonInvalid() const { return strInvalid; }
  const StdString &CommonAsterisk() const { return strAsterisk; }
  const StdString &CommonNil() const { return strNil; }
  const StdString &CommonZero() const { return strZero; }
  const StdString &CommonOne() const { return strOne; }
  const char *CommonCBlank() const { return cpBlank; }
  /* -- Default constructor ------------------------------------- */ protected:
  Common() :
    /* -- String view initialisers ----------------------------------------- */
    strvDir{ "<DIR>" },                strvEnt{ "&#x" },
    strvFs{ "<FS>" },                  strvHttp{ "http" },
    strvHttps{ "https" },              strvLuaName{ "__name" },
    strvTimeout{ "Frame timeout!" },   strvPipe{ "|" },
    strvColon{ ":" },                  strvSpace{ " " },
    strvQuote{ "\"" },                 strvEquals{ "=" },
    strvZero{ "0" },                   strvOne{ "1" },
    strvDblSpace{ "  " },              strvFSlash{ "/" },
    /* -- String initialisers ---------------------------------------------- */
    strTrue{ "true" },                 strFalse{ "false" },
    strY{ "Y" },                       strN{ "N" },
    strSpace{ strvSpace },             strCr{ "\r" },
    strLf{ "\n" },                     strCrLf{ "\r\n" },
    strCrLf2{ "\r\n\r\n" },            strLfCr{ "\n\r" },
    strUnspec{ "<Unspecified>" },      strNull{ "<Null>" },
    strPeriod{ "." },                  str2Period{ ".." },
    strEllipsis{ "..." },              strPrivate{ "<Private>" },
    strProtected{ "<Protected>" },     strEmpty{ "<Empty>" },
    strInvalid{ "<Invalid>" },         strAsterisk{ "*" },
    strNil{ "nil" },                   strUnresolved{ "<Unresolved>" },
    strZero{ strvZero },               strOne{ strvOne },
    /* -- Other initialisers ----------------------------------------------- */
    cpBlank(strBlank.data()),          lLocaleCurrent{ strBlank }
    /* -- Set global pointer to static class ------------------------------- */
    { cCommon = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
