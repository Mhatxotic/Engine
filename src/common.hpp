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
  const StdStringView ssvDir, ssvEnt, ssvFs, ssvHttp, ssvHttps, ssvLuaName,
    ssvTimeout, ssvPipe, ssvColon, ssvSpace, ssvQuote, ssvEquals, ssvBlank,
    ssvZero, ssvOne, ssvDblSpace, ssvFSlash, ssvUnknown, ssvTrue, ssvFalse,
    ssvY, ssvN;
  /* -- Common strings ----------------------------------------------------- */
  const StdString strSpace, strBlank, strCr, strLf, strCrLf, strCrLf2, strLfCr,
    strUnspec, strNull, strPeriod, str2Period, strEllipsis, strPrivate,
    strProtected, strEmpty, strInvalid, strAsterisk, strNil, strUnresolved,
    strZero, strOne;
  /* -- Miscellaneous common variables ------------------------------------- */
  const char*const cpBlank;            // Blank C-String
  StdLocale        lLocaleCurrent;     // Current locale
  /* --------------------------------------------------------------- */ public:
  const StdLocale &CommonLocale() const { return lLocaleCurrent; }
  void CommonSetLocale(const StdString &strLocale)
    { lLocaleCurrent = StdLocale{ strLocale }; }
  /* -- Return string view functions --------------------------------------- */
  const StdStringView &CommonFsV() const { return ssvFs; }
  const StdStringView &CommonDirV() const { return ssvDir; }
  const StdStringView &CommonEntV() const { return ssvEnt; }
  const StdStringView &CommonHttpV() const { return ssvHttp; }
  const StdStringView &CommonHttpsV() const { return ssvHttps; }
  const StdStringView &CommonLuaNameV() const { return ssvLuaName; }
  const StdStringView &CommonTimeoutV() const { return ssvTimeout; }
  const StdStringView &CommonPipeV() const { return ssvPipe; }
  const StdStringView &CommonColonV() const { return ssvColon; }
  const StdStringView &CommonSpaceV() const { return ssvSpace; }
  const StdStringView &CommonQuoteV() const { return ssvQuote; }
  const StdStringView &CommonEqualsV() const { return ssvEquals; }
  const StdStringView &CommonBlankV() const { return ssvBlank; }
  const StdStringView &CommonZeroV() const { return ssvZero; }
  const StdStringView &CommonOneV() const { return ssvOne; }
  const StdStringView &CommonDblSpaceV() const { return ssvDblSpace; }
  const StdStringView &CommonFSlashV() const { return ssvFSlash; }
  const StdStringView &CommonUnknownV() const { return ssvUnknown; }
  const StdStringView &CommonTrue() const { return ssvTrue; }
  const StdStringView &CommonFalse() const { return ssvFalse; }
  const StdStringView &CommonYes() const { return ssvY; }
  const StdStringView &CommonNo() const { return ssvN; }
  /* -- Return string functions -------------------------------------------- */
  const StdString &CommonBlank() const { return strBlank; }
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
    ssvDir{ "<DIR>" },                 ssvEnt{ "&#x" },
    ssvFs{ "<FS>" },                   ssvHttp{ "http" },
    ssvHttps{ "https" },               ssvLuaName{ "__name" },
    ssvTimeout{ "Frame timeout!" },    ssvPipe{ "|" },
    ssvColon{ ":" },                   ssvSpace{ " " },
    ssvQuote{ "\"" },                  ssvEquals{ "=" },
    ssvZero{ "0" },                    ssvOne{ "1" },
    ssvDblSpace{ "  " },               ssvFSlash{ "/" },
    ssvUnknown{ "Unknown" },           ssvTrue{ "true" },
    ssvFalse{ "false" },               ssvY{ "Y" },
    ssvN{ "N" },
    /* -- String initialisers ---------------------------------------------- */
    strSpace{ ssvSpace },              strCr{ "\r" },
    strLf{ "\n" },                     strCrLf{ "\r\n" },
    strCrLf2{ "\r\n\r\n" },            strLfCr{ "\n\r" },
    strUnspec{ "<Unspecified>" },      strNull{ "<Null>" },
    strPeriod{ "." },                  str2Period{ ".." },
    strEllipsis{ "..." },              strPrivate{ "<Private>" },
    strProtected{ "<Protected>" },     strEmpty{ "<Empty>" },
    strInvalid{ "<Invalid>" },         strAsterisk{ "*" },
    strNil{ "nil" },                   strUnresolved{ "<Unresolved>" },
    strZero{ ssvZero },                strOne{ ssvOne },
    /* -- Other initialisers ----------------------------------------------- */
    cpBlank(strBlank.data()),          lLocaleCurrent{ strBlank }
    /* -- Set global pointer to static class ------------------------------- */
    { cCommon = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
