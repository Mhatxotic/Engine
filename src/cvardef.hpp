/* == CVARDEF.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines cvar names, default values and flags. Theres no ## **
** ## actual code, just definitions!                                      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICVarDef {                   // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IFlags;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum CVarReturn : unsigned int         // Callback return values
{ /* ----------------------------------------------------------------------- */
  DENY,                                // The new cvar value is not acceptable
  ACCEPT,                              // The new var value is acceptable
  ACCEPT_HANDLED,                      // The CBfunc changes cvarData.strValue
  ACCEPT_HANDLED_FORCECOMMIT,          // Same as above and mark to commit
};/* == Convert bool result cvar accept or deny ============================ */
static CVarReturn BoolToCVarReturn(const bool bState)
  { return bState ? ACCEPT : DENY; }
static bool CVarToBoolReturn(const CVarReturn cvState)
  { return cvState != DENY; }
/* -- Helper function to just set a value and return accept ---------------- */
template<typename AnyToType, typename AnyFromType>
  static CVarReturn CVarSimpleSetInt(AnyToType &atTo,
    const AnyFromType atFrom)
      { atTo = atFrom; return ACCEPT; }
template<typename AnyToType, typename AnyFromType, typename AnyRangeType>
  static CVarReturn CVarSimpleSetIntNL(AnyToType &atTo,
    const AnyFromType atFrom, const AnyRangeType artNL)
      { return atFrom < artNL ? DENY : CVarSimpleSetInt(atTo, atFrom); }
template<typename AnyToType, typename AnyFromType, typename AnyRangeType>
  static CVarReturn CVarSimpleSetIntNG(AnyToType &atTo,
    const AnyFromType atFrom, const AnyRangeType artNG)
      { return atFrom > artNG ? DENY : CVarSimpleSetInt(atTo, atFrom); }
template<typename AnyToType, typename AnyFromType, typename AnyRangeType>
  static CVarReturn CVarSimpleSetIntNGE(AnyToType &atTo,
    const AnyFromType atFrom, const AnyRangeType artNGE)
      { return atFrom >= artNGE ? DENY : CVarSimpleSetInt(atTo, atFrom); }
template<typename AnyToType, typename AnyFromType, typename AnyRangeType>
  static CVarReturn CVarSimpleSetIntNLG(AnyToType &atTo,
    const AnyFromType atFrom, const AnyRangeType artNL,
    const AnyRangeType artNG)
      { return atFrom < artNL || atFrom > artNG ?
          DENY : CVarSimpleSetInt(atTo, atFrom); }
template<typename AnyToType, typename AnyFromType, typename AnyRangeType>
  static CVarReturn CVarSimpleSetIntNLGE(AnyToType &atTo,
    const AnyFromType atFrom, const AnyRangeType artNL,
    const AnyRangeType artNGE)
      { return atFrom < artNL || atFrom >= artNGE ?
          DENY : CVarSimpleSetInt(atTo, atFrom); }
/* -- Gui mode flags ------------------------------------------------------- */
BUILD_FLAGS(Core,
  /* ----------------------------------------------------------------------- */
  // No flags?                         Want text mode console?
  CFL_NONE                  {Flag(0)}, CFL_TERMINAL              {Flag(1)},
  // Want audio sub-system?            Want opengl window?
  CFL_AUDIO                 {Flag(2)}, CFL_VIDEO                 {Flag(3)},
  /* ----------------------------------------------------------------------- */
  CFL_AUDIOVIDEO{ CFL_AUDIO|CFL_VIDEO },
  CFL_MASK{ CFL_TERMINAL|CFL_AUDIOVIDEO }
);/* -- CVar flags --------------------------------------------------------- */
BUILD_FLAGS(CVar,
  /* -- Types (T) ---------------------------------------------------------- */
  // Variable is a string?             Variable is a integer?
  TSTRING                   {Flag(8)}, TINTEGER                  {Flag(9)},
  // Variable is a float?              Variable is a boolean
  TFLOAT                   {Flag(10)}, TBOOLEAN                 {Flag(11)},
  /* -- Conditional (C) ---------------------------------------------------- */
  // Var string can contain letters?   Variable string can contain numerics?
  CALPHA                   {Flag(16)}, CNUMERIC                 {Flag(17)},
  // Variable is written to database?  Variable is protected?
  CSAVEABLE                {Flag(18)}, CPROTECTED               {Flag(19)},
  // Variable is compressed?           Var cannot be empty (str) or zero? (int)
  CDEFLATE                 {Flag(20)}, CNOTEMPTY                {Flag(21)},
  // Variable must be unsigned?        Variable integer must be power of two?
  CUNSIGNED                {Flag(22)}, CPOW2                    {Flag(23)},
  // Variable must be a valid filename?
  CFILENAME                {Flag(24)},
  /* -- Manipulation (M) --------------------------------------------------- */
  MTRIM                    {Flag(32)}, // Variable string should be trimmed
  /* -- Other (O) ---------------------------------------------------------- */
  OSAVEFORCE               {Flag(40)}, // Force variable to be saved
  /* -- Privates (Used internally) ----------------------------------------- */
  // Variable was created by LUA?      Variable is a trusted filename
  TLUA                     {Flag(48)}, CTRUSTEDFN               {Flag(49)},
  // Cannot unreg this in callback?    Variable should be committed to DB?
  LOCKED                   {Flag(50)}, COMMIT                   {Flag(51)},
  // Var should be purged from DB?     Variable not readable by lua?
  PURGE                    {Flag(52)}, CONFIDENTIAL             {Flag(53)},
  // Variable value was loaded from db
  LOADED                   {Flag(54)},
  /* -- Sources (S) [Private] ---------------------------------------------- */
  // Set from engine internally?       Set from command-line?
  SENGINE                  {Flag(56)}, SCMDLINE                 {Flag(57)},
  // Set from appconfig?               Set from database?
  SAPPCFG                  {Flag(58)}, SUDB                     {Flag(59)},
  /* -- Permissions (P) [Private] ------------------------------------------ */
  // Variable can be changed at boot?  Variable can be changed by system
  PBOOT                    {Flag(62)}, PSYSTEM                  {Flag(63)},
  // Variable can be changed by user?
  PUSR                     {Flag(64)},
  /* -- Other -------------------------------------------------------------- */
  // No flags                          Private vars begin after here
  NONE                      {Flag(0)}, PRIVATE                      {TLUA},
  /* -- Collections -------------------------------------------------------- */
  // Shortcut to unsigned int type     Shortcut to 'unsigned float' type
  TUINTEGER{ TINTEGER|CUNSIGNED },     TUFLOAT{ TFLOAT|CUNSIGNED },
  // Shortcut to int + saveable        Shortcut to float + saveable
  TINTEGERSAVE{ TINTEGER|CSAVEABLE },  TFLOATSAVE{ TFLOAT|CSAVEABLE },
  // Shortcut to uint + saveable       Shortcut to unsigned float + saveable
  TUINTEGERSAVE{ TUINTEGER|CSAVEABLE },TUFLOATSAVE{ TUFLOAT|CSAVEABLE },
  // Shortcut to string + saveable     Shortcut to boolean + saveable
  TSTRINGSAVE{ TSTRING|CSAVEABLE },    TBOOLEANSAVE{ TBOOLEAN|CSAVEABLE },
  // All perms granted to modify       // Any source
  PANY{ PBOOT|PSYSTEM|PUSR },          SANY{ SENGINE|SCMDLINE|SAPPCFG|SUDB },
  // Registration mask bits            Only alphanumeric characeters
  CVREGMASK{ COMMIT|SANY },            CALPHANUMERIC{ CALPHA|CNUMERIC },
  /* -- Allowed bits ------------------------------------------------------- */
  CVMASK{ TSTRING|TINTEGER|TFLOAT|TBOOLEAN|CALPHA|CNUMERIC|CSAVEABLE|
          CPROTECTED|CDEFLATE|CNOTEMPTY|CUNSIGNED|CPOW2|CFILENAME|MTRIM|
          OSAVEFORCE };
);/* ----------------------------------------------------------------------- */
class CVarItem;                        // (Prototype) Cvar callback data
typedef CVarReturn (*CbFunc)(CVarItem&, const string&); // Callback return type
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* ========================================================================= **
** ######################################################################### **
** ## BASE ENGINE CVARS LIST                                              ## **
** ######################################################################### **
** ## Please note that if you add more and/or modify this order then you  ## **
** ## need to update the 'cvKeyValueStaticList' list below this scope and ## **
** ## then the 'cvEngList' list in 'cvarlib.hpp'.                         ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
namespace ICVarLib {                   // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICVarDef::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum CVarEnums : size_t
{ /* -- Critical cvars (order is important!) ------------------------------- */
  APP_CMDLINE,      LOG_LEVEL,         AST_LZMABUFFER,      AST_PIPEBUFFER,
  AST_FSOVERRIDE,   AST_EXEBUNDLE,     AST_BASEDIR,         AST_BUNDLES,
  APP_CONFIG,       APP_AUTHOR,        APP_SHORTNAME,       AST_HOMEDIR,
  AST_MODBUNDLE,    SQL_DB,            SQL_RETRYCOUNT,      SQL_RETRYSUSPEND,
  SQL_ERASEEMPTY,   SQL_TEMPSTORE,     SQL_SYNCHRONOUS,     SQL_JOURNALMODE,
  SQL_AUTOVACUUM,   SQL_FOREIGNKEYS,   SQL_INCVACUUM,       SQL_DEFAULTS,
  SQL_LOADCONFIG,   APP_CFLAGS,        LOG_LINES,           LOG_FILE,
  APP_LONGNAME,     APP_CLEARMUTEX,    ERR_INSTANCE,
  /* -- Object cvars ------------------------------------------------------- */
  OBJ_CLIPMAX,      OBJ_CMDMAX,        OBJ_CVARMAX,         OBJ_CVARIMAX,
  OBJ_ARCHIVEMAX,   OBJ_ASSETMAX,      OBJ_BINMAX,          OBJ_FBOMAX,
  OBJ_FONTMAX,      OBJ_FILEMAX,       OBJ_FTFMAX,          OBJ_FUNCMAX,
  OBJ_IMGMAX,       OBJ_JSONMAX,       OBJ_MASKMAX,         OBJ_PCMMAX,
  OBJ_SAMPLEMAX,    OBJ_SHADERMAX,     OBJ_SOCKETMAX,       OBJ_SOURCEMAX,
  OBJ_STATMAX,      OBJ_STREAMMAX,     OBJ_TEXTUREMAX,      OBJ_THREADMAX,
  OBJ_VIDEOMAX,
  /* -- Base cvars --------------------------------------------------------- */
  APP_DESCRIPTION,  APP_VERSION,       APP_ICON,            APP_COPYRIGHT,
  APP_WEBSITE,      APP_TICKRATE,      APP_DELAY,           APP_TITLE,
  /* -- Error cvars -------------------------------------------------------- */
  ERR_ADMIN,        ERR_CHECKSUM,      ERR_DEBUGGER,        ERR_LUAMODE,
  ERR_LMRESETLIMIT, ERR_MINVRAM,       ERR_MINRAM,
  /* -- Lua cvars ---------------------------------------------------------- */
  LUA_CACHE,        LUA_DEBUGLOCALS,   LUA_GCPAUSE,         LUA_GCSTEPMUL,
  LUA_LASTVER,      LUA_RANDOMSEED,    LUA_SCRIPT,          LUA_SIZESTACK,
  LUA_TICKCHECK,    LUA_TICKTIMEOUT,
  /* -- Audio cvars -------------------------------------------------------- */
  AUD_DELAY,        AUD_VOL,           AUD_INTERFACE,       AUD_CHECK,
  AUD_NUMSOURCES,   AUD_SAMVOL,        AUD_STRBUFCOUNT,     AUD_STRBUFFER,
  AUD_STRVOL,       AUD_FMVVOL,        AUD_HRTF,
  /* -- Console cvars ------------------------------------------------------ */
  CON_KEYPRIMARY,   CON_KEYSECONDARY,  CON_AUTOCOMPLETE,    CON_AUTOSCROLL,
  CON_AUTOCOPYCVAR, CON_HEIGHT,        CON_BLOUTPUT,        CON_BLINPUT,
  CON_DISABLED,     CON_CVSHOWFLAGS,   CON_BGCOLOUR,        CON_BGTEXTURE,
  CON_FONT,         CON_FONTFLAGS,     CON_FONTCOLOUR,      CON_FONTHEIGHT,
  CON_FONTPADDING,  CON_FONTPCMIN,     CON_FONTPCMAX,       CON_FONTSCALE,
  CON_FONTSPACING,  CON_FONTLSPACING,  CON_FONTWIDTH,       CON_FONTTEXSIZE,
  CON_INPREFRESH,   CON_INPUTMAX,      CON_OUTPUTMAX,       CON_PAGELINES,
  CON_TMCCOLS,      CON_TMCROWS,       CON_TMCREFRESH,      CON_TMCNOCLOSE,
  CON_TMCTFORMAT,
  /* -- Fmv cvars ---------------------------------------------------------- */
  FMV_ABUFFER,      FMV_IOBUFFER,      FMV_MAXDRIFT,
  /* -- Input cvars -------------------------------------------------------- */
  INP_CLAMPMOUSE,   INP_JOYDEFFDZ,     INP_JOYDEFRDZ,       INP_FSTOGGLER,
  INP_RAWMOUSE,     INP_STICKYKEY,     INP_STICKYMOUSE,
  /* -- Network cvars ------------------------------------------------------ */
  NET_CBPFLAG1,     NET_CBPFLAG2,      NET_BUFFER,          NET_RTIMEOUT,
  NET_STIMEOUT,     NET_CIPHERTLSv1,   NET_CIPHERTLSv13,    NET_CASTORE,
  NET_OCSP,         NET_USERAGENT,
  /* -- Video cvars -------------------------------------------------------- */
  VID_API,          VID_AUXBUFFERS,    VID_CTXMAJOR,        VID_CTXMINOR,
  VID_CLEAR,        VID_CLEARCOLOUR,   VID_DBLBUFF,         VID_DEBUG,
  VID_FBALPHA,      VID_FBBLUE,        VID_FBGREEN,         VID_FBRED,
  VID_FS,           VID_FSAA,          VID_FORWARD,         VID_FSMODE,
  VID_GAMMA,        VID_GASWITCH,      VID_HIDPI,           VID_LOCK,
  VID_MONITOR,      VID_NOERRORS,      VID_ORASPMAX,        VID_ORASPMIN,
  VID_ORHEIGHT,     VID_ORWIDTH,       VID_PROFILE,         VID_QCOMPRESS,
  VID_QLINE,        VID_QPOLYGON,      VID_QSHADER,         VID_RCMDS,
  VID_RDFBO,        VID_RDTEX,         VID_RELEASE,         VID_RFBO,
  VID_RFLOATS,      VID_ROBUSTNESS,    VID_SIMPLEMATRIX,    VID_SRGB,
  VID_SSTYPE,       VID_STEREO,        VID_SUBPIXROUND,     VID_TEXFILTER,
  VID_VSYNC,
  /* -- Window cvars ------------------------------------------------------- */
  WIN_ALPHA,        WIN_ASPECT,        WIN_BORDER,          WIN_CLOSEABLE,
  WIN_FLOATING,     WIN_FOCUSED,       WIN_HEIGHT,          WIN_HEIGHTMAX,
  WIN_HEIGHTMIN,    WIN_MAXIMISED,     WIN_MINIMISEAUTO,    WIN_POSX,
  WIN_POSY,         WIN_SIZABLE,       WIN_WIDTH,           WIN_WIDTHMAX,
  WIN_WIDTHMIN,
  /* -- Logging cvars ------------------------------------------------------ */
  LOG_CREDITS,      LOG_DYLIBS,
  /* -- Misc (do not (re)move) --------------------------------------------- */
  CVAR_MAX,                            // Maximum cvars
  CVAR_FIRST = APP_CMDLINE,            // The first cvar item
};/* ----------------------------------------------------------------------- */
struct CVarItemStatic                  // Start of CVar static struct
{ /* ----------------------------------------------------------------------- */
  const CoreFlagsConst cfcRequired;    // Required core flags
  const string_view    &strvVar;       // Variable name from C-String
  const string_view    &strvValue;     // Variable default value from C-String
  const CbFunc         cbTrigger;      // Callback trigger event
  const CVarFlagsConst &cFlags;        // Variable flags
};/* ----------------------------------------------------------------------- */
typedef array<const CVarItemStatic, CVAR_MAX> CVarItemStaticList;
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
