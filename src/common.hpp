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
{ /* -- Common strings ----------------------------------------------------- */
  const StdString strTrue, strFalse, strY, strN, strEquals, strNOne, strZero,
    strOne, strTwo, strSpace, strDblSpace, strBlank, strCr, strLf, strCrLf,
    strCrLf2, strLfCr, strFSlash, strUnspec, strNull, strPeriod, str2Period,
    strEllipsis, strPrivate, strProtected, strEmpty, strInvalid, strAsterisk,
    strEnt, strNil, strUnresolved;
  /* -- Common string views ------------------------------------------------ */
  const StdStringView svDir, svFs, svHttp, svHttps, svLuaName, svTimeout;
  /* -- Miscellaneous common variables ------------------------------------- */
  const char*const cpBlank;            // Blank C-String
  locale           lLocaleCurrent;     // Current locale
  /* --------------------------------------------------------------- */ public:
  const locale &CommonLocale() const { return lLocaleCurrent; }
  void CommonSetLocale(const StdString &strLocale)
    { lLocaleCurrent = locale{ strLocale }; }
  /* -- Return string functions -------------------------------------------- */
  const StdString &CommonBlank() const { return strBlank; }
  const char *CommonCBlank() const { return cpBlank; }
  const StdString &CommonTrue() const { return strTrue; }
  const StdString &CommonFalse() const { return strFalse; }
  const StdString &CommonYes() const { return strY; }
  const StdString &CommonNo() const { return strN; }
  const StdString &CommonEquals() const { return strEquals; }
  const StdString &CommonNegOne() const { return strNOne; }
  const StdString &CommonZero() const { return strZero; }
  const StdString &CommonOne() const { return strOne; }
  const StdString &CommonTwo() const { return strTwo; }
  const StdString &CommonCr() const { return strCr; }
  const StdString &CommonLf() const { return strLf; }
  const StdString &CommonCrLf() const { return strCrLf; }
  const StdString &CommonCrLf2() const { return strCrLf2; }
  const StdString &CommonLfCr() const { return strLfCr; }
  const StdString &CommonSpace() const { return strSpace; }
  const StdString &CommonDblSpace() const { return strDblSpace; }
  const StdString &CommonEllipsis() const { return strEllipsis; }
  const StdString &CommonFSlash() const { return strFSlash; }
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
  const StdString &CommonEnt() const { return strEnt; }
  const StdString &CommonNil() const { return strNil; }
  /* -- Return string view functions --------------------------------------- */
  const StdStringView &CommonFs() const { return svFs; }
  const StdStringView &CommonDir() const { return svDir; }
  const StdStringView &CommonHttp() const { return svHttp; }
  const StdStringView &CommonHttps() const { return svHttps; }
  const StdStringView &CommonLuaName() const { return svLuaName; }
  const StdStringView &CommonTimeout() const { return svTimeout; }
  /* -- Default constructor ------------------------------------- */ protected:
  Common() :
    /* -- Initialisers ----------------------------------------------------- */
    strTrue{ "true" },                 strFalse{ "false" },
    strY{ "Y" },                       strN{ "N" },
    strEquals{ "=" },                  strNOne{ "-1" },
    strZero{ "0" },                    strOne{ "1" },
    strTwo{ "2" },                     strSpace{ " " },
    strDblSpace{ "  " },               strCr{ "\r" },
    strLf{ "\n" },                     strCrLf{ "\r\n" },
    strCrLf2{ "\r\n\r\n" },            strLfCr{ "\n\r" },
    strFSlash{ "/" },                  strUnspec{ "<Unspecified>" },
    strNull{ "<Null>" },               strPeriod{ "." },
    str2Period{ ".." },                strEllipsis{ "..." },
    strPrivate{ "<Private>" },         strProtected{ "<Protected>" },
    strEmpty{ "<Empty>" },             strInvalid{ "<Invalid>" },
    strAsterisk{ "*" },                strEnt{ "&#x" },
    strNil{ "nil" },                   strUnresolved{ "<Unresolved>" },
    svDir{ "<DIR>" },                  svFs{ "<FS>" },
    svHttp{ "http" },                  svHttps{ "https" },
    svLuaName{ "__name" },             svTimeout{ "Script timed out!" },
    cpBlank(strBlank.data()),          lLocaleCurrent{ strBlank }
    /* -- Set global pointer to static class ------------------------------- */
    { cCommon = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
