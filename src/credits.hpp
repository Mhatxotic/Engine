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
using namespace Lib::Ogg::Theora;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Credit library class ------------------------------------------------- */
class CreditLib :                      // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public MemConst                      // License data memory
{ /* ----------------------------------------------------------------------- */
  const size_t     stId;               // Unique identification umber
  const string_view strvName,          // Name of library
                   strvVersion,        // String version
                   strvAuthor;         // Author of library
  const bool       bCopyright;         // Is copyrighted library
  /* --------------------------------------------------------------- */ public:
  const size_t &GetID(void) const { return stId; }
  const string_view &GetName(void) const { return strvName; }
  const string_view &GetVersion(void) const { return strvVersion; }
  const string_view &GetAuthor(void) const { return strvAuthor; }
  bool IsCopyright(void) const { return bCopyright; }
  /* ----------------------------------------------------------------------- */
  CreditLib(const size_t stCreditId, const string_view &strvNName,
    const string_view &strvNVersion, const string_view &strvNAuthor,
    const bool bNCopyright, const void*const vpData, const size_t stSize) :
    /* -- Initialisers ----------------------------------------------------- */
    MemConst{ stSize, vpData },        // Init credit license data
    stId{ stCreditId },                // Init credit unique id
    strvName{ strvNName },             // Init credit name
    strvVersion{ strvNVersion },       // Init credit version
    strvAuthor{ strvNAuthor },         // Init credit author
    bCopyright{ bNCopyright }          // Init credit copyright status
    /* -- Increment credit id counter -------------------------------------- */
    { }
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
static const class Credits final       // Members initially private
{ /* -- License data ------------------------------------------------------- */
#define BEGINLICENSE(n,s) static constexpr const array<const uint8_t,s> l ## n{
#define ENDLICENSE };                  // Helper functions for licenses header
#include "license.hpp"                 // Load up compressed licenses
#undef ENDLICENSE                      // Done with this macro
#undef BEGINLICENSE                    // Done with this macro
  /* -- Variables ---------------------------------------------------------- */
  const string strTheoraVersion;       // The only string that needs generating
  const CreditLibList cllCredits;      // Credits list
  /* -- Get credits class ------------------------------------------ */ public:
  const CreditLibList &CreditGetLibList(void) const { return cllCredits; }
  /* -- Get credits count -------------------------------------------------- */
  size_t CreditGetItemCount(void) const { return CreditGetLibList().size(); }
  /* -- Get credit item ---------------------------------------------------- */
  const CreditLib &CreditGetItem(const CreditEnums ceIndex) const
    { return CreditGetLibList()[ceIndex]; }
  /* -- Decompress a credit item ------------------------------------------- */
  const string CreditGetItemText(const CreditLib &libItem) const try
  { // Using codec namespace
    using namespace ICodec;
    return Block<CoDecoder>{ libItem }.MemToStringSafe();
  } // exception occured?
  catch(const exception &eReason)
  { // Log failure and try to reset the initial var so this does not
    XC("Failed to decode license text!",
       "Name",   libItem.GetName(), "Reason", eReason,
       "Length", libItem.MemSize());
  }
  /* -- Decompress a credit ------------------------------------------------ */
  const string CreditGetItemText(const CreditEnums ceIndex) const
    { return CreditGetItemText(CreditGetItem(ceIndex)); }
  /* -- Dump credits to log ------------------------------------------------ */
  void CreditDumpList(void) const
  { // Iterate through each entry and send library information to log
    cLog->LogNLCInfoExSafe("Credits enumerating $ external libraries...",
      CreditGetItemCount());
    for(const CreditLib &lD : CreditGetLibList())
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
    strTheoraVersion{ StrFormat("$.$.$", // Generate Theora version string
      theora_version_number()       & 0x00FF,
      theora_version_number() >>  8 & 0x00FF,
      theora_version_number() >> 16 & 0xFFFF) },
    cllCredits{{                       // The library list
      // t = Title of dependency         v = Version of dependency
      // n = license variable name       c = is dependency copyrighted?
      // a = Author of dependency
#define LD(i,t,v,c,a,n) { CL_ ## i, t, v, c, a, l ## n.data(), l ## n.size() }
      // The credits data structure (Keep the engine credit as the first)
      LD(MSE, cSystem->ENGName(), cSystem->ENGVersion(), cSystem->ENGAuthor(),
        true, ENGINE),
      LD(FT, "FreeType", STR(FREETYPE_MAJOR) "." STR(FREETYPE_MINOR) "."
        STR(FREETYPE_PATCH), "The FreeType Project", true, FREETYPE),
      LD(GLFW, "GLFW", cGlFW->GetInternalVersion(),
        "Marcus Geelnard & Camilla Löwy", true, GLFW),
      LD(JPEG, "JPEGTurbo", STR(LIBJPEG_TURBO_VERSION),
        "IJG/Contributing authors", true, LIBJPEGTURBO),
      LD(GIF, "LibNSGif", "1.0.0", "Richard Wilson & Sean Fox", true,
        LIBNSGIF),
      LD(PNG, "LibPNG", PNG_LIBPNG_VER_STRING, "Contributing authors", true,
        LIBPNG),
      LD(LUA, "LUA", LUA_VDIR "." LUA_VERSION_RELEASE, "Lua.org, PUC-Rio",
        true, LUA),
      LD(LZMA, "LZMA", MY_VERSION, "Igor Pavlov", false, 7ZIP),
      LD(MP3, "MiniMP3", "25.3.12", "Lieff/Lion", false, MINIMP3),
#if !defined(WINDOWS)
      LD(NCURSES, "NCurses", NCURSES_VERSION, "Free Software Foundation", true,
        NCURSES),
#endif
      LD(OGG, "OggVorbis", Lib::Ogg::vorbis_version_string()+19, "Xiph.Org",
        false, OGGVORBISTHEORA),
      LD(AL, "OpenALSoft", "1.24.3", "Chris Robinson", false, OPENALSOFT),
      LD(SSL, "OpenSSL", STR(OPENSSL_VERSION_MAJOR) "."
        STR(OPENSSL_VERSION_MINOR) "." STR(OPENSSL_VERSION_PATCH),
        "OpenSSL Software Foundation", true, OPENSSL),
      LD(JSON, "RapidJson", RAPIDJSON_VERSION_STRING,
        "THL A29 Ltd., Tencent co. & Milo Yip", true, RAPIDJSON),
      LD(SQL, "SQLite", SQLITE_VERSION, "Contributing authors", false, SQLITE),
      LD(THEO, "Theora", strTheoraVersion, "Xiph.Org", false, OGGVORBISTHEORA),
      LD(ZLIB, "Z-Lib", STR(ZLIB_VER_MAJOR) "." STR(ZLIB_VER_MINOR) "."
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
