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
    strAsterisk, strEnt, strNil, strFs, strDir;
  /* -- Miscellaneous common variables ------------------------------------- */
  const char*const cpBlank;            // Blank C-String
  locale           lLocaleCurrent;     // Current locale
  /* --------------------------------------------------------------- */ public:
  const locale &CommonLocale(void) const { return lLocaleCurrent; }
  void CommonSetLocale(const string &strLocale)
    { lLocaleCurrent = locale(strLocale); }
  /* ----------------------------------------------------------------------- */
  const string &CommonBlank(void) const { return strBlank; }
  const char *CommonCBlank(void) const { return cpBlank; }
  const string &CommonTrue(void) const { return strTrue; }
  const string &CommonFalse(void) const { return strFalse; }
  const string &CommonYes(void) const { return strY; }
  const string &CommonNo(void) const { return strN; }
  const string &CommonEquals(void) const { return strEquals; }
  const string &CommonNegOne(void) const { return strNOne; }
  const string &CommonZero(void) const { return strZero; }
  const string &CommonOne(void) const { return strOne; }
  const string &CommonTwo(void) const { return strTwo; }
  const string &CommonCr(void) const { return strCr; }
  const string &CommonLf(void) const { return strLf; }
  const string &CommonCrLf(void) const { return strCrLf; }
  const string &CommonCrLf2(void) const { return strCrLf2; }
  const string &CommonLfCr(void) const { return strLfCr; }
  const string &CommonSpace(void) const { return strSpace; }
  const string &CommonDblSpace(void) const { return strDblSpace; }
  const string &CommonEllipsis(void) const { return strEllipsis; }
  const string &CommonFSlash(void) const { return strFSlash; }
  const string &CommonUnspec(void) const { return strUnspec; }
  const string &CommonNull(void) const { return strNull; }
  const string &CommonPeriod(void) const { return strPeriod; }
  const string &CommonTwoPeriod(void) const { return str2Period; }
  const string &CommonLuaName(void) const { return strLuaName; }
  const string &CommonPrivate(void) const { return strPrivate; }
  const string &CommonProtected(void) const { return strProtected; }
  const string &CommonEmpty(void) const { return strEmpty; }
  const string &CommonInvalid(void) const { return strInvalid; }
  const string &CommonAsterisk(void) const { return strAsterisk; }
  const string &CommonEnt(void) const { return strEnt; }
  const string &CommonNil(void) const { return strNil; }
  const string &CommonFs(void) const { return strFs; }
  const string &CommonDir(void) const { return strDir; }
  /* -- Default constructor ------------------------------------- */ protected:
  Common(void) :                       // No parameters
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
    cpBlank(strBlank.data()),          lLocaleCurrent{ strBlank }
    /* -- Set global pointer to static class ------------------------------- */
    { cCommon = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
