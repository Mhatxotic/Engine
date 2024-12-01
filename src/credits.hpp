/* == CREDITS.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file is a consolidation of libraries information used in the   ## **
** ## engine which is logged at startup and can be read by the guest and  ## **
** ## the end-user.                                                       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICredit {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICodec::P;             using namespace ICVarDef::P;
using namespace IGlFW::P;              using namespace IError::P;
using namespace ILog::P;               using namespace IMemory::P;
using namespace IString::P;            using namespace ISystem::P;
/* ------------------------------------------------------------------------- */
static size_t stCreditId = 0;          // For setting 'stId' in CreditLib
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Credit library class ------------------------------------------------- */
class CreditLib :                      // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public MemConst                      // License data memory
{ /* ----------------------------------------------------------------------- */
  const size_t     stId;               // Unique identification umber
  const string     strName,            // Name of library
                   strVersion,         // String version
                   strAuthor;          // Author of library
  const bool       bCopyright;         // Is copyrighted library
  /* --------------------------------------------------------------- */ public:
  const size_t &GetID(void) const { return stId; }
  const string &GetName(void) const { return strName; }
  const string &GetVersion(void) const { return strVersion; }
  const string &GetAuthor(void) const { return strAuthor; }
  bool IsCopyright(void) const { return bCopyright; }
  /* ----------------------------------------------------------------------- */
  CreditLib(const string &strNName,
    const string &strNVersion, const string &strNAuthor,
    const bool bNCopyright, const void*const vpData, const size_t stSize) :
    /* -- Initialisers ----------------------------------------------------- */
    MemConst{ stSize, vpData },        // Init credit license data
    stId{ stCreditId },                // Init credit unique id
    strName{ strNName },               // Init credit name
    strVersion{ strNVersion },         // Init credit version
    strAuthor{ strNAuthor },           // Init credit author
    bCopyright{ bNCopyright }          // Init credit copyright status
    /* -- Increment credit id counter -------------------------------------- */
    { ++stCreditId; }
};/* ----------------------------------------------------------------------- */
/* -- Credits list lookup table -------------------------------------------- */
enum CreditEnums : size_t              // Credit ids
{ /* ----------------------------------------------------------------------- */
  CL_FIRST,                            // The first item (see llcredit.hpp)
  /* ----------------------------------------------------------------------- */
  CL_MSE=CL_FIRST,                     // Id for engine data (always first)
  CL_FT,                               // Id for FreeType credit data
  CL_GLFW,                             // Id for GLFW credit data
  CL_JPEG,                             // Id for LibJPEGTurbo credit data
  CL_GIF,                              // Id for LibNSGif credit data
  CL_PNG,                              // Id for LibPNG credit data
  CL_LUA,                              // Id for Lua credit data
  CL_LZMA,                             // Id for 7-Zip/LZMA credit data
  CL_MP3,                              // Id for MiniMP3 credit data
#if !defined(WINDOWS)                  // Not using Windows?
  CL_NCURSES,                          // Id for NCurses credit data
#endif                                 // Not using windows
  CL_OGG,                              // Id for LibOgg+LibVorbis credit data
  CL_AL,                               // Id for OpenALSoft credit data
  CL_SSL,                              // Id for OpenSSL credit data
  CL_JSON,                             // Id for RapidJson credit data
  CL_SQL,                              // Id for SqLite credit data
  CL_THEO,                             // Id for Theora credit data
  CL_ZLIB,                             // Id for ZLib credit data
  /* ----------------------------------------------------------------------- */
  CL_MAX                               // Item count. Don't remove
};/* ----------------------------------------------------------------------- */
typedef array<const CreditLib,CL_MAX> CreditLibList; // Library list typedef
typedef CreditLibList::const_iterator CreditLibListConstIt; // Iterator
/* ------------------------------------------------------------------------- */
static const class Credits final :     // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public CreditLibList                 // Credits list
{ /* -- License data ------------------------------------------------------- */
#define BEGINLICENSE(n,s) static constexpr const array<const uint8_t,s> l ## n{
#define ENDLICENSE };                  // Helper functions for licenses header
#include "license.hpp"                 // Load up compressed licenses
#undef ENDLICENSE                      // Done with this macro
#undef BEGINLICENSE                    // Done with this macro
  /* -- Get credits count ------------------------------------------ */ public:
  size_t CreditGetItemCount(void) const { return size(); }
  /* -- Get credit item ---------------------------------------------------- */
  const CreditLib &CreditGetItem(const CreditEnums ceIndex) const
    { return (*this)[ceIndex]; }
  /* -- Decompress a credit item ------------------------------------------- */
  const string CreditGetItemText(const CreditLib &libItem) const try
  { // Using codec namespace
    using namespace ICodec;
    return Block<CoDecoder>{ libItem }.MemToStringSafe();
  } // exception occured?
  catch(const exception &e)
  { // Log failure and try to reset the initial var so this does not
    XC("Failed to decode license text!",
       "Name", libItem.GetName(), "Reason", e, "Length", libItem.MemSize());
  }
  /* -- Decompress a credit ------------------------------------------------ */
  const string CreditGetItemText(const CreditEnums ceIndex) const
    { return CreditGetItemText(CreditGetItem(ceIndex)); }
  /* -- Dump credits to log ------------------------------------------------ */
  void CreditDumpList(void) const
  { // Iterate through each entry and send library information to log
    cLog->LogNLCInfoExSafe("Credits enumerating $ external libraries...",
        CreditGetItemCount());
    for(const CreditLib &lD : *this)
      cLog->LogNLCInfoExSafe("- Using $ (v$) $$", lD.GetName(),
        lD.GetVersion(), lD.IsCopyright() ? "\xC2\xA9 " : cCommon->Blank(),
        lD.GetAuthor());
  }
  /* -- Dump credits to log (cvar version) --------------------------------- */
  CVarReturn CreditDumpList(const bool bDoIt) const
    { if(bDoIt) CreditDumpList(); return ACCEPT; }
  /* -- Default constructor ------------------------------------------------ */
  Credits(void) :                      // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    CreditLibList{{                    // The library list
      // t = Title of dependency         v = Version of dependency
      // n = license variable name       c = is dependency copyrighted?
      // a = Author of dependency
#define LD(t,v,c,a,n) { t, v, c, a, l ## n.data(), l ## n.size() }
      // The credits data structure (Keep the engine credit as the first)
      LD(cSystem->ENGName(), cSystem->ENGVersion(), cSystem->ENGAuthor(), true,
        ENGINE),
      LD("FreeType", STR(FREETYPE_MAJOR) "." STR(FREETYPE_MINOR) "."
        STR(FREETYPE_PATCH), "The FreeType Project", true, FREETYPE),
      LD("GLFW", cGlFW->GetInternalVersion(),
        "Marcus Geelnard & Camilla Löwy", true, GLFW),
      LD("JPEGTurbo", STR(LIBJPEG_TURBO_VERSION), "IJG/Contributing authors",
        true, LIBJPEGTURBO),
      LD("LibNSGif", "1.0.0", "Richard Wilson & Sean Fox", true,
        LIBNSGIF),
      LD("LibPNG", PNG_LIBPNG_VER_STRING, "Contributing authors", true,
        LIBPNG),
      LD("LUA", LUA_VDIR "." LUA_VERSION_RELEASE, "Lua.org, PUC-Rio", true,
        LUA),
      LD("LZMA", MY_VERSION, "Igor Pavlov", false, 7ZIP),
      LD("MiniMP3", "1.0", "Martin Fiedler", false, MINIMP3),
#if !defined(WINDOWS)
      LD("NCurses", NCURSES_VERSION, "Free Software Foundation", true,
        NCURSES),
#endif
      LD("OggVorbis", Lib::Ogg::vorbis_version_string()+19, "Xiph.Org",
        false, OGGVORBISTHEORA),
      LD("OpenALSoft", "1.24.1", "Chris Robinson", false, OPENALSOFT),
      LD("OpenSSL", STR(OPENSSL_VERSION_MAJOR) "." STR(OPENSSL_VERSION_MINOR)
        "." STR(OPENSSL_VERSION_PATCH), "OpenSSL Software Foundation", true,
        OPENSSL),
      LD("RapidJson", RAPIDJSON_VERSION_STRING,
        "THL A29 Ltd., Tencent co. & Milo Yip", true, RAPIDJSON),
      LD("SQLite", SQLITE_VERSION, "Contributing authors", false, SQLITE),
      LD("Theora",
        StrFormat("$.$.$",
          Lib::Ogg::Theora::theora_version_number() >> 16 & 0xFFFF,
          Lib::Ogg::Theora::theora_version_number() >>  8 & 0x00FF,
          Lib::Ogg::Theora::theora_version_number()       & 0x00FF),
        "Xiph.Org", false, OGGVORBISTHEORA),
      LD("Z-Lib", STR(ZLIB_VER_MAJOR) "." STR(ZLIB_VER_MINOR) "."
        STR(ZLIB_VER_REVISION) "." STR(ZLIB_VER_SUBREVISION),
        "Jean-loup Gailly & Mark Adler", true, ZLIB),
      // End of credits data structure
#undef LD                              // Done with this macro
    } }                                // End of credits list
  /* -- No code ------------------------------------------------------------ */
  { }
} /* ----------------------------------------------------------------------- */
*cCredits = nullptr;                   // Pointer to static class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
