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
  const string strTrue, strFalse, strY, strN, strEquals, strNOne, strZero,
    strOne, strTwo, strSpace, strDblSpace, strBlank, strCr, strLf, strCrLf,
    strCrLf2, strLfCr, strFSlash, strUnspec, strNull, strPeriod, str2Period,
    strEllipsis, strLuaName, strPrivate, strProtected, strEmpty, strInvalid,
    strAsterisk, strEnt, strNil, strFs, strDir, strHttp, strHttps;
  /* -- Miscellaneous common variables ------------------------------------- */
  const char*const cpBlank;            // Blank C-String
  locale           lLocaleCurrent;     // Current locale
  /* --------------------------------------------------------------- */ public:
  const locale &CommonLocale() const { return lLocaleCurrent; }
  void CommonSetLocale(const string &strLocale)
    { lLocaleCurrent = locale(strLocale); }
  /* ----------------------------------------------------------------------- */
  const string &CommonBlank() const { return strBlank; }
  const char *CommonCBlank() const { return cpBlank; }
  const string &CommonTrue() const { return strTrue; }
  const string &CommonFalse() const { return strFalse; }
  const string &CommonYes() const { return strY; }
  const string &CommonNo() const { return strN; }
  const string &CommonEquals() const { return strEquals; }
  const string &CommonNegOne() const { return strNOne; }
  const string &CommonZero() const { return strZero; }
  const string &CommonOne() const { return strOne; }
  const string &CommonTwo() const { return strTwo; }
  const string &CommonCr() const { return strCr; }
  const string &CommonLf() const { return strLf; }
  const string &CommonCrLf() const { return strCrLf; }
  const string &CommonCrLf2() const { return strCrLf2; }
  const string &CommonLfCr() const { return strLfCr; }
  const string &CommonSpace() const { return strSpace; }
  const string &CommonDblSpace() const { return strDblSpace; }
  const string &CommonEllipsis() const { return strEllipsis; }
  const string &CommonFSlash() const { return strFSlash; }
  const string &CommonUnspec() const { return strUnspec; }
  const string &CommonNull() const { return strNull; }
  const string &CommonPeriod() const { return strPeriod; }
  const string &CommonTwoPeriod() const { return str2Period; }
  const string &CommonLuaName() const { return strLuaName; }
  const string &CommonPrivate() const { return strPrivate; }
  const string &CommonProtected() const { return strProtected; }
  const string &CommonEmpty() const { return strEmpty; }
  const string &CommonInvalid() const { return strInvalid; }
  const string &CommonAsterisk() const { return strAsterisk; }
  const string &CommonEnt() const { return strEnt; }
  const string &CommonNil() const { return strNil; }
  const string &CommonFs() const { return strFs; }
  const string &CommonDir() const { return strDir; }
  const string &CommonHttp() const { return strHttp; }
  const string &CommonHttps() const { return strHttps; }
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
    strLuaName{ "__name" },            strPrivate{ "<Private>" },
    strProtected{ "<Protected>" },     strEmpty{ "<Empty>" },
    strInvalid{ "<Invalid>" },         strAsterisk{ "*" },
    strEnt{ "&#x" },                   strNil{ "nil" },
    strFs{ "<FS>" },                   strDir{ "<DIR>" },
    strHttp{ "http" },                 strHttps{ "https" },
    cpBlank(strBlank.data()),          lLocaleCurrent{ strBlank }
    /* -- Set global pointer to static class ------------------------------- */
    { cCommon = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
