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
/* -- Public typedefs ------------------------------------------------------ */
enum CVarReturn : unsigned int         // Callback return values
{ /* ----------------------------------------------------------------------- */
  DENY,                                // [0] New cvar value is not acceptable
  ACCEPT,                              // [1] New var value is acceptable
  ACCEPT_HANDLED,                      // [2] CBfunc changes cvarData.strValue
  ACCEPT_HANDLED_FORCECOMMIT,          // [3] Same as above but mark to commit
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
/* ------------------------------------------------------------------------- */
BUILD_FLAGS(Core,                      // Engine mode flags
  /* ----------------------------------------------------------------------- */
  CFL_BASIC                 {Flag(0)}, // No flags?
  CFL_TEXT                  {Flag(1)}, // Want text mode console?
  CFL_AUDIO                 {Flag(2)}, // Want audio sub-system?
  CFL_VIDEO                 {Flag(3)}, // Want opengl window?
  CFL_TIMER                 {Flag(4)}, // Want frame limiter?
  /* ----------------------------------------------------------------------- */
  CFL_AUDIOVIDEO{ CFL_AUDIO|CFL_VIDEO },
  CFL_MASK{ CFL_TEXT|CFL_AUDIOVIDEO|CFL_TIMER }
);/* ----------------------------------------------------------------------- */
BUILD_FLAGS(CVar,                      // CVar flags
  /* -- Types (T) ---------------------------------------------------------- */
  TSTRING                   {Flag(8)}, // Variable is a string?
  TINTEGER                  {Flag(9)}, // Variable is a integer?
  TFLOAT                   {Flag(10)}, // Variable is a float?
  TBOOLEAN                 {Flag(11)}, // Variable is a boolean?
  /* -- Conditional (C) ---------------------------------------------------- */
  CALPHA                   {Flag(16)}, // Var string can contain letters?
  CNUMERIC                 {Flag(17)}, // Variable string can contain numerics?
  CSAVEABLE                {Flag(18)}, // Variable is written to database?
  CPROTECTED               {Flag(19)}, // Variable is protected?
  CDEFLATE                 {Flag(20)}, // Variable is compressed?
  CNOTEMPTY                {Flag(21)}, // Var can't be empty (str) or 0? (int)
  CUNSIGNED                {Flag(22)}, // Variable must be unsigned?
  CPOW2                    {Flag(23)}, // Variable integer must be power of 2?
  CFILENAME                {Flag(24)}, // Variable must be a valid filename?
  /* -- Manipulation (M) --------------------------------------------------- */
  MTRIM                    {Flag(32)}, // Variable string should be trimmed?
  /* -- Privates (Used internally) ----------------------------------------- */
  TLUA                     {Flag(48)}, // Variable was created by LUA?
  CTRUSTEDFN               {Flag(49)}, // Variable is a trusted filename?
  LOCKED                   {Flag(50)}, // Cannot unreg this in callback?
  COMMIT                   {Flag(51)}, // Variable should be committed to DB?
  PURGE                    {Flag(52)}, // Var should be purged from DB?
  CONFIDENTIAL             {Flag(53)}, // Variable not readable by lua?
  LOADED                   {Flag(54)}, // Variable value was loaded from db?
  COMMITNOCHECK            {Flag(55)}, // Commit even if default/value match?
  /* -- Sources (S) [Private] ---------------------------------------------- */
  SCMDLINE                 {Flag(57)}, // Set from command-line?
  SUDB                     {Flag(58)}, // Set from database?
  SAPPCFG                  {Flag(59)}, // Set from application manifest?
  /* -- Permissions (P) [Private] ------------------------------------------ */
  PCMDLINE                 {Flag(61)}, // Variable can be changed at boot?
  PAPPCFG                  {Flag(62)}, // Variable can be changed by system?
  PUDB                     {Flag(63)}, // Variable can be changed by udb?
  PCONSOLE                 {Flag(64)}, // Variable can be changed by user?
  /* -- Other -------------------------------------------------------------- */
  NONE                      {Flag(0)}, // No flags?
  PRIVATE                      {TLUA}, // Private vars begin after here?
  /* -- Collections -------------------------------------------------------- */
  TUINTEGER{ TINTEGER|CUNSIGNED },     // Shortcut to unsigned int type
  TUFLOAT{ TFLOAT|CUNSIGNED },         // Shortcut to 'unsigned float' type
  TINTEGERSAVE{ TINTEGER|CSAVEABLE },  // Shortcut to int + saveable
  TFLOATSAVE{ TFLOAT|CSAVEABLE },      // Shortcut to float + saveable
  TUINTEGERSAVE{ TUINTEGER|CSAVEABLE },// Shortcut to uint + saveable
  TUFLOATSAVE{ TUFLOAT|CSAVEABLE },    // Shortcut to unsigned float + saveable
  TSTRINGSAVE{ TSTRING|CSAVEABLE },    // Shortcut to string + saveable
  TBOOLEANSAVE{ TBOOLEAN|CSAVEABLE },  // Shortcut to boolean + saveable
  CALPHANUMERIC{ CALPHA|CNUMERIC },    // Only alphanumeric characeters
  SANY{ SCMDLINE|SAPPCFG|SUDB },       // Any source
  CVREGMASK{ COMMIT|SANY },            // Registration mask bits
  PANY{PCMDLINE|PAPPCFG|PUDB|PCONSOLE},// All perms granted to modify
  /* -- Allowed bits ------------------------------------------------------- */
  CVMASK{ TSTRING|TINTEGER|TFLOAT|TBOOLEAN|CALPHA|CNUMERIC|CSAVEABLE|
          CPROTECTED|CDEFLATE|CNOTEMPTY|CUNSIGNED|CPOW2|CFILENAME|MTRIM }
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
enum CVarEnums : size_t                // CVars list (cvarlib.hpp for bodies)
{ /* -- Critical cvars (order is critical!) -------------------------------- */
  APP_CMDLINE,      LOG_LEVEL,         AST_LZMABUFFER,      AST_PIPEBUFFER,
  AST_FSOVERRIDE,   AST_EXEBUNDLE,     AST_BASEDIR,         AST_BUNDLES,
  APP_CONFIG,       APP_AUTHOR,        APP_SHORTNAME,       AST_HOMEDIR,
  AST_MODBUNDLE,    AST_SAFETYMODE,    SQL_DB,              SQL_RETRYCOUNT,
  SQL_RETRYSUSPEND, SQL_ERASEEMPTY,    SQL_TEMPSTORE,       SQL_SYNCHRONOUS,
  SQL_JOURNALMODE,  SQL_AUTOVACUUM,    SQL_FOREIGNKEYS,     SQL_INCVACUUM,
  SQL_DEFAULTS,     SQL_LOADCONFIG,    APP_CFLAGS,          LOG_LINES,
  LOG_FILE,         APP_LONGNAME,      ERR_INSTANCE,
  /* -- Object cvars ------------------------------------------------------- */
  OBJ_ARCHIVEMAX,   OBJ_ASSETMAX,      OBJ_ATLASMAX,        OBJ_BINMAX,
  OBJ_CLIPMAX,      OBJ_CMDMAX,        OBJ_CVARIMAX,        OBJ_CVARMAX,
  OBJ_FBOMAX,       OBJ_FILEMAX,       OBJ_FONTMAX,         OBJ_FTFMAX,
  OBJ_FUNCMAX,      OBJ_IMGMAX,        OBJ_JSONMAX,         OBJ_MASKMAX,
  OBJ_PCMMAX,       OBJ_SAMPLEMAX,     OBJ_SHADERMAX,       OBJ_SOCKETMAX,
  OBJ_SOURCEMAX,    OBJ_SSHOTMAX,      OBJ_STATMAX,         OBJ_STREAMMAX,
  OBJ_TEXTUREMAX,   OBJ_THREADMAX,     OBJ_URLMAX,          OBJ_VIDEOMAX,
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
  CON_GCWTERM,      CON_INPREFRESH,    CON_INPUTMAX,        CON_OUTPUTMAX,
  CON_PAGELINES,    CON_TMCCOLS,       CON_TMCROWS,         CON_TMCREFRESH,
  CON_TMCNOCLOSE,   CON_TMCTFORMAT,
  /* -- Fmv cvars ---------------------------------------------------------- */
  FMV_ABUFFER,      FMV_IOBUFFER,      FMV_MAXDRIFT,
  /* -- Input cvars -------------------------------------------------------- */
  INP_CLAMPMOUSE,   INP_JOYDEFFDZ,     INP_JOYDEFRDZ,       INP_FSTOGGLER,
  INP_LOCKKEYMODS,  INP_RAWMOUSE,      INP_STICKYKEY,       INP_STICKYMOUSE,
  /* -- Network cvars ------------------------------------------------------ */
  NET_CBPFLAG1,     NET_CBPFLAG2,      NET_BUFFER,          NET_RTIMEOUT,
  NET_STIMEOUT,     NET_CIPHERTLSv1,   NET_CIPHERTLSv13,    NET_CASTORE,
  NET_OCSP,         NET_USERAGENT,
  /* -- Video cvars -------------------------------------------------------- */
  VID_API,          VID_CTXMAJOR,      VID_CTXMINOR,        VID_CLEAR,
  VID_CLEARCOLOUR,  VID_DBLBUFF,       VID_DEBUG,           VID_FBALPHA,
  VID_FBBLUE,       VID_FBGREEN,       VID_FBRED,           VID_FS,
  VID_FSAA,         VID_FORWARD,       VID_FSMODE,          VID_GAMMA,
  VID_GASWITCH,     VID_HIDPI,         VID_MONITOR,         VID_NOERRORS,
  VID_ORASPMAX,     VID_ORASPMIN,      VID_ORHEIGHT,        VID_ORWIDTH,
  VID_PROFILE,      VID_QCOMPRESS,     VID_QLINE,           VID_QPOLYGON,
  VID_QSHADER,      VID_RCMDS,         VID_RDFBO,           VID_RDTEX,
  VID_RELEASE,      VID_RFBO,          VID_RFLOATS,         VID_ROBUSTNESS,
  VID_SIMPLEMATRIX, VID_SRGB,          VID_SSTYPE,          VID_STEREO,
  VID_SUBPIXROUND,  VID_TEXFILTER,     VID_VSYNC,
  /* -- Window cvars ------------------------------------------------------- */
  WIN_ALPHA,        WIN_ASPECT,        WIN_BORDER,          WIN_CLOSEABLE,
  WIN_FLOATING,     WIN_FOCUSED,       WIN_HEIGHT,          WIN_HEIGHTMAX,
  WIN_HEIGHTMIN,    WIN_MAXIMISED,     WIN_MINIMISEAUTO,    WIN_POSX,
  WIN_POSY,         WIN_SIZABLE,       WIN_WIDTH,           WIN_WIDTHMAX,
  WIN_WIDTHMIN,
  /* -- Logging cvars ------------------------------------------------------ */
  LOG_CREDITS,      LOG_DYLIBS,
  /* -- Other cvars -------------------------------------------------------- */
  COM_FLAGS,
  /* -- Misc (do not (re)move) --------------------------------------------- */
  CVAR_MAX,                            // Maximum cvars
  CVAR_FIRST = APP_CMDLINE,            // The first cvar item
};/* ----------------------------------------------------------------------- */
struct CVarItemStatic                  // Start of CVar static struct
{ /* ----------------------------------------------------------------------- */
  const CoreFlagsConst cfcRequired;    // Required core flags
  const string_view    strvVar,        // Variable name from C-String
                       strvValue;      // Variable default value from C-String
  const CbFunc         cbTrigger;      // Callback trigger event
  const CVarFlagsConst cFlags;         // Variable flags
};/* ----------------------------------------------------------------------- */
typedef array<const CVarItemStatic, CVAR_MAX> CVarItemStaticList;
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
