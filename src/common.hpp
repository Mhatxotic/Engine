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
/* ------------------------------------------------------------------------- */
const char caBlank[] = "";             // Blank C-String on the stack
/* -- Common class with common objects ------------------------------------- */
class Common;                          // Prototype
static Common *cCommon = nullptr;      // Global access to class
class Common                           // Common variables class
{ /* -- Common strings ----------------------------------------------------- */
  const StdString strBlank;
  /* -- Common string views ------------------------------------------------ */
  const StdStringView ssv2Period, ssvAsterisk, ssvBlank, ssvColon, ssvCr,
    ssvCrLf, ssvCrLf2, ssvDblSpace, ssvDir, ssvEllipsis, ssvEmpty, ssvEnt,
    ssvEquals, ssvFalse, ssvFs, ssvFSlash, ssvHttp, ssvHttps, ssvInvalid,
    ssvLf, ssvLfCr, ssvLuaName, ssvN, ssvNil, ssvNull, ssvOne, ssvPeriod,
    ssvPipe, ssvQuote, ssvSpace, ssvTimeout, ssvTrue, ssvUnknown,
    ssvUnresolved, ssvUnspec, ssvY, ssvZero;
  /* -- Miscellaneous common variables ------------------------------------- */
  StdLocale        lLocaleCurrent;     // Current locale
  /* -- Locale methods --------------------------------------------- */ public:
  const StdLocale &CommonLocale() const { return lLocaleCurrent; }
  void CommonSetLocale(const StdString &strLocale)
    { lLocaleCurrent = StdLocale{ strLocale }; }
  /* -- Return string view functions --------------------------------------- */
  const StdStringView &CommonAsterisk() const { return ssvAsterisk; }
  const StdStringView &CommonBlank() const { return ssvBlank; }
  const StdStringView &CommonColon() const { return ssvColon; }
  const StdStringView &CommonCrLf2() const { return ssvCrLf2; }
  const StdStringView &CommonCrLf() const { return ssvCrLf; }
  const StdStringView &CommonCr() const { return ssvCr; }
  const StdStringView &CommonDblSpace() const { return ssvDblSpace; }
  const StdStringView &CommonDir() const { return ssvDir; }
  const StdStringView &CommonEllipsis() const { return ssvEllipsis; }
  const StdStringView &CommonEmpty() const { return ssvEmpty; }
  const StdStringView &CommonEnt() const { return ssvEnt; }
  const StdStringView &CommonEquals() const { return ssvEquals; }
  const StdStringView &CommonFalse() const { return ssvFalse; }
  const StdStringView &CommonFSlash() const { return ssvFSlash; }
  const StdStringView &CommonFs() const { return ssvFs; }
  const StdStringView &CommonHttps() const { return ssvHttps; }
  const StdStringView &CommonHttp() const { return ssvHttp; }
  const StdStringView &CommonInvalid() const { return ssvInvalid; }
  const StdStringView &CommonLfCr() const { return ssvLfCr; }
  const StdStringView &CommonLf() const { return ssvLf; }
  const StdStringView &CommonLuaName() const { return ssvLuaName; }
  const StdStringView &CommonNil() const { return ssvNil; }
  const StdStringView &CommonNo() const { return ssvN; }
  const StdStringView &CommonNull() const { return ssvNull; }
  const StdStringView &CommonOne() const { return ssvOne; }
  const StdStringView &CommonPeriod() const { return ssvPeriod; }
  const StdStringView &CommonPipe() const { return ssvPipe; }
  const StdStringView &CommonQuote() const { return ssvQuote; }
  const StdStringView &CommonSpace() const { return ssvSpace; }
  const StdStringView &CommonTimeout() const { return ssvTimeout; }
  const StdStringView &CommonTrue() const { return ssvTrue; }
  const StdStringView &CommonTwoPeriod() const { return ssv2Period; }
  const StdStringView &CommonUnknown() const { return ssvUnknown; }
  const StdStringView &CommonUnresolved() const { return ssvUnresolved; }
  const StdStringView &CommonUnspec() const { return ssvUnspec; }
  const StdStringView &CommonYes() const { return ssvY; }
  const StdStringView &CommonZero() const { return ssvZero; }
  /* -- Return string functions -------------------------------------------- */
  const StdString &CommonBlankStr() const { return strBlank; }
  /* -- Default constructor ------------------------------------- */ protected:
  Common() :
    /* -- String view initialisers ----------------------------------------- */
    ssv2Period{ ".." },                ssvAsterisk{ "*" },
    ssvBlank{ caBlank },               ssvColon{ ":" },
    ssvCr{ "\r" },                     ssvCrLf{ "\r\n" },
    ssvCrLf2{ "\r\n\r\n" },            ssvDblSpace{ "  " },
    ssvDir{ "<DIR>" },                 ssvEllipsis{ "..." },
    ssvEmpty{ "<Empty>" },             ssvEnt{ "&#x" },
    ssvEquals{ "=" },                  ssvFalse{ "false" },
    ssvFs{ "<FS>" },                   ssvFSlash{ "/" },
    ssvHttp{ "http" },                 ssvHttps{ "https" },
    ssvInvalid{ "<Invalid>" },         ssvLf{ "\n" },
    ssvLfCr{ "\n\r" },                 ssvLuaName{ "__name" },
    ssvN{ "N" },                       ssvNil{ "nil" },
    ssvNull{ "<Null>" },               ssvOne{ "1" },
    ssvPeriod{ "." },                  ssvPipe{ "|" },
    ssvQuote{ "\"" },                  ssvSpace{ " " },
    ssvTimeout{ "Frame timeout!" },    ssvTrue{ "true" },
    ssvUnknown{ "Unknown" },           ssvUnresolved{ "<Unresolved>" },
    ssvUnspec{ "<Unspecified>" },      ssvY{ "Y" },
    ssvZero{ "0" },
    /* -- Other initialisers ----------------------------------------------- */
    lLocaleCurrent{ strBlank }
    /* -- Set global pointer to static class ------------------------------- */
    { cCommon = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
