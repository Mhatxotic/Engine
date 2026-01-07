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
#define LLFUNCBEGIN(n)       static int Cb ## n(lua_State*const lS) {
#define LLFUNCBEGINTMPL(n)   template<typename IntType>LLFUNCBEGIN(n)
#define LLFUNCEND            LLFUNCENDEX(0)
#define LLFUNCENDEX(x)       static_cast<void>(lS); return x; }
#define LLFUNCTMPL(n,x,...)  LLFUNCBEGINTMPL(n) __VA_ARGS__; LLFUNCENDEX(x)
#define LLFUNC(n,x,...)      LLFUNCBEGIN(n) __VA_ARGS__; LLFUNCENDEX(x)
/* -- Macros to simplify definition of lualib reg func structures ---------- */
#define LLRSBEGIN            static const luaL_Reg llrFunctions[]{
#define LLRSCONST(n)         { #n, lkiConsts ## n, LLArrLen(lkiConsts ## n) }
#define LLRSCONSTBEGIN       static const LuaTable ltConsts[]{
#define LLRSCONSTEND         ltLast };
#define LLRSEND              llrLast };
#define LLRSFUNC(n)          { #n, LuaUtilCallback<&Cb ## n> }
#define LLRSFUNCEX(n,f)      { #n, LuaUtilCallback<&Cb ## f> }
#define LLRSKTBEGIN(n)       static const LuaKeyInt lkiConsts ## n[]{
#define LLRSKTEND            lkiLast };
#define LLRSKTITEM(p,n)      LLRSKTITEMEX2(#n, p ## n)
#define LLRSKTITEMEX(p,n,u)  LLRSKTITEMEX2(#n, p ## n ## u)
#define LLRSKTITEMEX2(k,v)   { k, static_cast<lua_Integer>(v) }
#define LLRSMFBEGIN          static const luaL_Reg llrMethods[]{
/* -- Get length of an array ignoring the final NULL entry ----------------- */
template<typename AnyType, int itSize>
  constexpr static int LLArrLen(AnyType (&)[itSize])
    { return itSize - 1; };
/* -- Empty last entries --------------------------------------------------- */
constexpr static const LuaTable ltLast{ nullptr, nullptr, 0 };
constexpr static const LuaKeyInt lkiLast{ nullptr, 0 };
constexpr static const luaL_Reg llrLast{ nullptr, nullptr };
/* ------------------------------------------------------------------------- **
** ######################################################################### **
** ## LUA library API includes                                            ## **
** ######################################################################### **
** ## The following files are also parsed by the engine Project           ## **
** ## Management Utility to help create a HTML documentation. New         ## **
** ## namespaces start with '// % ' with each line describing the         ## **
** ## namespace starting with '// ! '. Each new function starts with      ## **
** ## '// $ ' with 'Class:Method' or 'Namespace.Function'. Each parameter ## **
** ## to that function starts with '// > ' with 'Name:type=Description'   ## **
** ## and returning result as '// < ' with the same format as             ## **
** ## 'Name:type=Description'. Each line describing the function will     ## **
** ## begin with '// ? '. If one of these lines ends with a period then   ## **
** ## the following line will not be a new paragraph.                     ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#include "llcommon.hpp"                // Common helper classes (always first)
#include "llarchive.hpp"               // Archive members and methods
#include "llasset.hpp"                 // Asset members and methods
#include "llatlas.hpp"                 // Atlas members and methods
#include "llaudio.hpp"                 // Audio members and methods
#include "llbin.hpp"                   // Bin members and methods
#include "llclip.hpp"                  // Clipboard members and methods
#include "llcmd.hpp"                   // Command members and methods
#include "llcore.hpp"                  // Core members and methods
#include "lldisplay.hpp"               // Display members and methods
#include "llfbo.hpp"                   // Fbo members and methods
#include "llfile.hpp"                  // File members and methods
#include "llfont.hpp"                  // Font members and methods
#include "llftf.hpp"                   // Ftf members and methods
#include "llimage.hpp"                 // Image members and methods
#include "llinput.hpp"                 // Input members and methods
#include "lljson.hpp"                  // Json members and methods
#include "llmask.hpp"                  // Mask members and methods
#include "llpcm.hpp"                   // Pcm members and methods
#include "llsource.hpp"                // Source members and methods
#include "llsample.hpp"                // Sample members and methods
#include "llsocket.hpp"                // Socket members and methods
#include "llsql.hpp"                   // Sql members and methods
#include "llsshot.hpp"                 // SShot members and methods
#include "llstat.hpp"                  // Stat members and methods
#include "llstream.hpp"                // Stream members and methods
#include "lltexture.hpp"               // Texture members and methods
#include "llpalette.hpp"               // Palette members and methods
#include "llurl.hpp"                   // Url members and methods
#include "llutil.hpp"                  // Util members and methods
#include "llvar.hpp"                   // Variable members and methods
#include "llvideo.hpp"                 // Video members and methods
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
#define LLDF(n)    LuaUtilCallback<&LL ## n::CbDestroy>
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
#define LLFxx(n,f)                                   /* Function only       */\
  LLITEM(CLASSES, n, f,                              /* Just a namespace    */\
    LLNS(n, lrFunctions), LLAL(n, lrFunctions),      /* Have functions      */\
    nullptr, 0, nullptr,                             /* No methods          */\
    nullptr, 0)                                      /* No statics           */
/* ------------------------------------------------------------------------- */
#define LLFMx(n,f,t)                                 /* Function + method   */\
  LLITEM(t, n, f,                                    /* Is a class          */\
    LLNS(n, lrFunctions), LLAL(n, lrFunctions),      /* Have functions      */\
    LLNS(n, lrMethods), LLAL(n, lrMethods), LLDF(n), /* Have methods        */\
    nullptr, 0)                                      /* No statics           */
/* ------------------------------------------------------------------------- */
#define LLFxC(n,f)                                   /* Function + const    */\
  LLITEM(CLASSES, n, f,                              /* Just a namespace    */\
    LLNS(n, lrFunctions), LLAL(n, lrFunctions),      /* Have functions      */\
    nullptr, 0, nullptr,                             /* No methods          */\
    LLNS(n, tConsts), LLAL(n, tConsts))              /* Have statics         */
/* ------------------------------------------------------------------------- */
#define LLFMC(n,f,t)                                 /* Func + meth + const */\
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
  LLFMx(Texture, VIDEO, TEXTURE),      LLFMC(Url, BASIC, URL),
  LLFxx(Util, BASIC),                  LLFMC(Variable, BASIC, VARIABLE),
  LLFMC(Video, AUDIOVIDEO, VIDEO),
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
