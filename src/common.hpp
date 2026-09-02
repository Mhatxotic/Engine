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
static const char caBlank[] = "";      // Blank C-String on the stack
/* -- Common class with common objects ------------------------------------- */
class Common;                          // Prototype
static Common *cCommon = nullptr;      // Global access to class
class Common                           // Common variables class
{/* -- Common strings ------------------------------------------------------ */
  const StdString strBlank;
  /* -- Common string views ------------------------------------------------ */
  const StdStringView ssv2Period, ssvAsterisk, ssvBlank, ssvColon, ssvCr,
    ssvCrLf, ssvCrLf2, ssvDblSpace, ssvDir, ssvEllipsis, ssvEmpty, ssvEnt,
    ssvEquals, ssvFalse, ssvFs, ssvFSlash, ssvHttp, ssvHttps, ssvInvalid,
    ssvLf, ssvLfCr, ssvLuaName, ssvN, ssvNil, ssvNull, ssvOne, ssvPeriod,
    ssvPipe, ssvQuote, ssvSpace, ssvTimeout, ssvTrue, ssvUnknown,
    ssvUnresolved, ssvUnspec, ssvY, ssvZero;
  /* -- StringStream manager class ----------------------------------------- */
  template<class StringStreamType>     // It sets up a permanent StringStream
    class StringStream                 // object for optimum speed.
  { /* -- Private variables ------------------------------------------------ */
    StdLocale       &lLocaleCurrent;   // Current locale
    StringStreamType sstSd,            // Blank string stream to copy defaults
                     sstSdi;           // " imbued
    /* -- Get a thread localised string stream ----------------------------- */
    // Note that although using thread_local still has an overhead, it's still
    // faster to use this then having to recreate stringstreams. Just remember
    // not to mix the calls with manual use of OStringStream << usage.
    static StringStreamType &Stream()
      { thread_local StringStreamType sstS; return sstS; }
    /* -- Get a thread localised imbued string stream ---------------------- */
    static StringStreamType &StreamImbued()
      { thread_local StringStreamType sstSi; return sstSi; }
    /* -- Partial init ----------------------------------------------------- */
    static StringStreamType &StreamPartialReInit(StringStreamType &sstDst,
      StringStreamType &sstDef)
    { sstDst.clear(); sstDst.copyfmt(sstDef); return sstDst; }
    /* -- Initialise the read-only imbued string stream ----------- -*/ public:
    void InitImbuedStream() { sstSdi.imbue(lLocaleCurrent); }
    /* -- Get a thread localised string stream that has been reset --------- */
    StringStreamType &StreamReset()
      { Stream().str(caBlank); return StreamPartialReInit(Stream(), sstSd); }
    /* -- Same as above but initialise with specified storage -------------- */
    StringStreamType &StreamReset(const StdString &strInit)
      { Stream().str(strInit); return StreamPartialReInit(Stream(), sstSd); }
    /* -- Get a thread localised imbued string stream that has been reset -- */
    StringStreamType &StreamImbuedReset()
      { StreamImbued().str(caBlank);
        return StreamPartialReInit(StreamImbued(), sstSdi); }
    /* -- Same as above but initialise with specified storage -------------- */
    StringStreamType &StreamImbuedReset(const StdString &strInit)
      { StreamImbued().str(strInit);
        return StreamPartialReInit(StreamImbued(), sstSdi); }
    /* -- Constructor ------------------------------------------------------ */
    explicit StringStream(StdLocale &lNLocaleCurrent) :
      /* -- Initialisers --------------------------------------------------- */
      lLocaleCurrent(lNLocaleCurrent)
      /* -- Initialise the localisation for the imbued stream -------------- */
      { InitImbuedStream(); }
  };/* -- Miscellaneous common variables --------------------------- */ public:
  StdLocale        lLocaleCurrent;     // Current locale
  StringStream<StdIStringStream> i;    // For reading strings
  StringStream<StdOStringStream> o;    // For writing strings
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
  /* -- Set a new locale --------------------------------------------------- */
  void CommonSetLocale(const StdString &strLocale)
  { // Overwrite the old locale
    lLocaleCurrent = StdLocale{ strLocale };
    // Re-initialise the output and input stringstreams
    o.InitImbuedStream();
    i.InitImbuedStream();
  }
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
    /* -- Locale and string stream initialisers ---------------------------- */
    lLocaleCurrent{ strBlank },        // Initialise locale
    i{ lLocaleCurrent },               // Initialise input string stream
    o{ lLocaleCurrent }                // Initialise output string stream
    /* -- Set global pointer to static class and init imbued stream -------- */
    { cCommon = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
