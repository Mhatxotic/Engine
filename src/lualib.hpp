/* == LUALIB.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file defines the complete LUA API that interfaces with the     ## **
** ## game engine. The guest will be able to use this functions when      ## **
** ## writing scripts for their software.                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaLib {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICVarDef::P;           using namespace ILuaUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Macros to simplify creating lualib functions ------------------------- */
#define LLFUNCBEGIN(n)         static int Cb ## n(lua_State*const lS) {
#define LLFUNCBEGINTMPL(n)     template<typename IntType>LLFUNCBEGIN(n)
#define LLFUNCEND              LLFUNCENDEX(0)
#define LLFUNCENDEX(x)         static_cast<void>(lS); return x; }
#define LLFUNCTMPL(n,x,...)    LLFUNCBEGINTMPL(n) __VA_ARGS__; LLFUNCENDEX(x)
#define LLFUNC(n,x,...)        LLFUNCBEGIN(n) __VA_ARGS__; LLFUNCENDEX(x)
/* -- Macros to simplify definition of lualib reg func structures ---------- */
#define LLRSBEGIN              static const luaL_Reg llrFunctions[]{
#define LLRSCONST(n)           { #n, lkiConsts ## n, LLArrLen(lkiConsts ## n) }
#define LLRSCONSTBEGIN         static const LuaTable ltConsts[]{
#define LLRSCONSTEND           { nullptr, nullptr, 0 } };
#define LLRSEND                { nullptr, nullptr } };
#define LLRSFUNC(n)            { #n, Cb ## n }
#define LLRSFUNCEX(n,f)        { #n, Cb ## f }
#define LLRSKTBEGIN(n)         static const LuaKeyInt lkiConsts ## n[]{
#define LLRSKTEND              { nullptr, 0 } };
#define LLRSKTITEM(p,n)        LLRSKTITEMEX2(#n, p ## n)
#define LLRSKTITEMEX(p,n,u)    LLRSKTITEMEX2(#n, p ## n ## u)
#define LLRSKTITEMEX2(k,v)     { k, static_cast<lua_Integer>(v) }
#define LLRSMFBEGIN            static const luaL_Reg llrMethods[]{
/* -- Get length of an array ignoring the final NULL entry ----------------- */
template<typename AnyType, int itSize>
  constexpr int LLArrLen(AnyType (&)[itSize]) { return itSize - 1; };
/* -- Dependencies --------------------------------------------------------- */
#include "llcommon.hpp"                // Common helper classes (always first)
/* -- API includes --------------------------------------------------------- */
#include "llarchive.hpp"               // Archive namespace functions
#include "llasset.hpp"                 // Asset namespace functions
#include "llatlas.hpp"                 // Atlas namespace functions
#include "llaudio.hpp"                 // Audio namespace functions
#include "llbin.hpp"                   // Bin namespace functions
#include "llclip.hpp"                  // Clipboard namespace functions
#include "llcmd.hpp"                   // Command namespace functions
#include "llcore.hpp"                  // Core namespace functions
#include "lldisplay.hpp"               // Display namespace functions
#include "llfbo.hpp"                   // Fbo namespace functions
#include "llfile.hpp"                  // File namespace functions
#include "llfont.hpp"                  // Font namespace functions
#include "llftf.hpp"                   // Ftf namespace functions
#include "llimage.hpp"                 // Image namespace functions
#include "llinput.hpp"                 // Input namespace functions
#include "lljson.hpp"                  // Json namespace functions
#include "llmask.hpp"                  // Mask namespace functions
#include "llpcm.hpp"                   // Pcm namespace functions
#include "llsource.hpp"                // Source namespace functions
#include "llsample.hpp"                // Sample namespace functions
#include "llsocket.hpp"                // Socket namespace functions
#include "llsql.hpp"                   // Sql namespace functions
#include "llsshot.hpp"                 // SShot namespace functions
#include "llstat.hpp"                  // Stat namespace functions
#include "llstream.hpp"                // Stream namespace functions
#include "lltexture.hpp"               // Texture namespace functions
#include "llpalette.hpp"               // Palette namespace functions
#include "llutil.hpp"                  // Util namespace functions
#include "llvar.hpp"                   // Variable namespace functions
#include "llvideo.hpp"                 // Video namespace functions
/* -- Done with these macros ----------------------------------------------- */
#undef LLRSMFBEGIN
#undef LLRSKTITEMEX2
#undef LLRSKTITEMEX
#undef LLRSKTITEM
#undef LLRSKTEND
#undef LLRSKTBEGIN
#undef LLRSFUNCEX
#undef LLRSFUNC
#undef LLRSEND
#undef LLRSCONSTEND
#undef LLRSCONSTBEGIN
#undef LLRSCONST
#undef LLRSBEGIN
#undef LLFUNCTMPL
#undef LLFUNCENDEX
#undef LLFUNCEND
#undef LLFUNCBEGINTMPL
#undef LLFUNCBEGIN
#undef LLFUNC
/* ========================================================================= **
** ######################################################################### **
** ## LUA GLOBAL API ARRAY STRUCTURE                                      ## **
** ######################################################################### **
** -- Namespace name builder helper macro ---------------------------------- */
#define LLNS(n,t)  LL ## n::l ## t
/* -- Items in a table helper macro ---------------------------------------- */
#define LLAL(n,t)  LLArrLen(LLNS(n, t))
/* -- Destructor function builder helper macro ----------------------------- */
#define LLDF(n)    static_cast<lua_CFunction>(LL ## n::CbDestroy)
/* -- Macro to actually build an item in the main API array ---------------- */
#define LLITEM(lmt,                    /* LMT_* identifier (luadef.hpp)     */\
               ns,                     /* Name of namespace or class        */\
               cfl,                    /* CFL_* mode flags (cvardef.hpp)    */\
               sa,                     /* Class namespace functions array   */\
               sc,                     /* " items of above array            */\
               ma,                     /* Class method functions array      */\
               mc,                     /* " items in above array            */\
               mf,                     /* " garbage collector function      */\
               ca,                     /* Class const variables array       */\
               cc)                     /* " items in above array            */\
  { LMT_ ## lmt, #ns, CFL_ ## cfl, sa, sc, ma, mc, mf, ca, cc, sc+mc+cc }
/* -- Helper macros to use when actually defining the main API array ------- */
#define LLFxx(n,f) \
  LLITEM(CLASSES, n, f,                              /* Just a namespace    */\
    LLNS(n, lrFunctions), LLAL(n, lrFunctions),      /* Have functions      */\
    nullptr, 0, nullptr,                             /* No methods          */\
    nullptr, 0)                                      /* No statics           */
/* ------------------------------------------------------------------------- */
#define LLFMx(n,f,t) \
  LLITEM(t, n, f,                                    /* Is a class          */\
    LLNS(n, lrFunctions), LLAL(n, lrFunctions),      /* Have functions      */\
    LLNS(n, lrMethods), LLAL(n, lrMethods), LLDF(n), /* Have methods        */\
    nullptr, 0)                                      /* No statics           */
/* ------------------------------------------------------------------------- */
#define LLFxC(n,f) \
  LLITEM(CLASSES, n, f,                              /* Just a namespace    */\
    LLNS(n, lrFunctions), LLAL(n, lrFunctions),      /* Have functions      */\
    nullptr, 0, nullptr,                             /* No methods          */\
    LLNS(n, tConsts), LLAL(n, tConsts))              /* Have statics         */
/* ------------------------------------------------------------------------- */
#define LLFMC(n,f,t) \
  LLITEM(t, n, f,                                    /* Is a class          */\
    LLNS(n, lrFunctions), LLAL(n, lrFunctions),      /* Have functions      */\
    LLNS(n, lrMethods), LLAL(n, lrMethods), LLDF(n), /* Have methods        */\
    LLNS(n, tConsts), LLAL(n, tConsts))              /* Have statics         */
/* -- Define the engine api list loaded at startup ------------------------- */
const LuaLibStaticArray llsaAPI{{
  /* -- Make sure to follow the order of 'LuaClassId' from 'luadef.hpp' ---- */
  LLFMx(Archive, BASIC, ARCHIVE),      LLFMC(Asset, BASIC, ASSET),
  LLFxx(Atlas, VIDEO),                 LLFxx(Audio, AUDIO),
  LLFMx(Bin, BASIC, BIN),              LLFMx(Clip, VIDEO, CLIP),
  LLFxC(Core, BASIC),                  LLFMx(Command, BASIC, COMMAND),
  LLFxC(Display, VIDEO),               LLFMC(Fbo, VIDEO, FBO),
  LLFMC(File, BASIC, FILE),            LLFMC(Font, VIDEO, FONT),
  LLFMx(Ftf, BASIC, FTF),              LLFMC(Image, BASIC, IMAGE),
  LLFxC(Input, VIDEO),                 LLFMx(Json, BASIC, JSON),
  LLFMx(Mask, BASIC, MASK),            LLFMx(Palette, VIDEO, PALETTE),
  LLFMC(Pcm, BASIC, PCM),              LLFMx(Sample, AUDIO, SAMPLE),
  LLFMx(SShot, VIDEO, SSHOT),          LLFMx(Stat, BASIC, STAT),
  LLFMC(Socket, BASIC, SOCKET),        LLFMx(Source, AUDIO, SOURCE),
  LLFxC(Sql, BASIC),                   LLFMC(Stream, AUDIO, STREAM),
  LLFMx(Texture, VIDEO, TEXTURE),      LLFxx(Util, BASIC),
  LLFMC(Variable, BASIC, VARIABLE),    LLFMC(Video, AUDIOVIDEO, VIDEO),
}};/* -- Done with these macros -------------------------------------------- */
#undef LLFMC
#undef LLFxC
#undef LLFMx
#undef LLFxx
#undef LLITEM
#undef LLDF
#undef LLAL
#undef LLNS
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
