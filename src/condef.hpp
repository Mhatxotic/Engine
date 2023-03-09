/* == CONDEF.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines console definitions before the console is       ## **
** ## created.                                                            ## **
** ######################################################################### */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IConDef {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Colour palette ------------------------------------------------------- */
enum Colour : unsigned int             // Try to match with SysCon colours
{ /* ----------------------------------------------------------------------- */
  COLOUR_BLACK   /* 00 */, COLOUR_BLUE     /* 01 */, COLOUR_GREEN  /* 02 */,
  COLOUR_CYAN    /* 03 */, COLOUR_RED      /* 04 */, COLOUR_GRAY   /* 05 */,
  COLOUR_MAGENTA /* 06 */, COLOUR_BROWN    /* 07 */, COLOUR_LGRAY  /* 08 */,
  COLOUR_LBLUE   /* 09 */, COLOUR_LGREEN   /* 10 */, COLOUR_LCYAN  /* 11 */,
  COLOUR_LRED    /* 12 */, COLOUR_LMAGENTA /* 13 */, COLOUR_YELLOW /* 14 */,
  COLOUR_WHITE   /* 15 */, COLOUR_MAX      /* 16 */
};/* ----------------------------------------------------------------------- */
/* -- Colour palette to RGB lookup ----------------------------------------- */
const unsigned int uiNDXtoRGB[COLOUR_MAX] =
{ /* ----------------------------------------------------------------------- */
  0x00010101 /* 00=COLOUR_BLACK   */, 0x0000008b /* 01=COLOUR_BLUE     */,
  0x00008b00 /* 02=COLOUR_GREEN   */, 0x00a0b0b0 /* 03=COLOUR_CYAN     */,
  0x008b0000 /* 04=COLOUR_RED     */, 0x00808080 /* 05=COLOUR_GRAY     */,
  0x008b008b /* 06=COLOUR_MAGENTA */, 0x00a52a2a /* 07=COLOUR_BROWN    */,
  0x01c0c0c0 /* 08=COLOUR_LGRAY   */, 0x018da8c6 /* 09=COLOUR_LBLUE    */,
  0x0190ee90 /* 10=COLOUR_LGREEN  */, 0x01c0dddd /* 11=COLOUR_LCYAN    */,
  0x01ff4b4b /* 12=COLOUR_LRED    */, 0x01e78be7 /* 13=COLOUR_LMAGENTA */,
  0x01ffff00 /* 14=COLOUR_YELLOW  */, 0x01ffffff /* 15=COLOUR_WHITE    */
};/* ----------------------------------------------------------------------- */
/* -- Console stuff, no where else to put it really ------------------------ */
struct ConLine                         // Console line data structure
{ /* ----------------------------------------------------------------------- */
  const double     dTime;              // Line time
  const Colour     cColour;            // Line colour index
  const string     strLine;            // Line data
};/* ----------------------------------------------------------------------- */
typedef list<ConLine>                    ConLines;           // Con lines data
typedef ConLines::const_iterator         ConLinesConstIt;    // " fwd const it
typedef ConLines::reverse_iterator       ConLinesRevIt;      // " reverse it
typedef ConLines::const_reverse_iterator ConLinesConstRevIt; // " const
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* ========================================================================= */
namespace IConLib {                    // Console library namespace
/* ------------------------------------------------------------------------- */
using namespace IArgs;                 using namespace IConDef::P;
using namespace ICVarDef::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ========================================================================= **
** ######################################################################### **
** ## BASE ENGINE COMMANDS DEFINES                                        ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
enum ConCmdEnums : unsigned int
{ /* ----------------------------------------------------------------------- */
  CC_ARCHIVES,  CC_ARESET,  CC_ASSETS,   CC_AUDINS,    CC_AUDIO,   CC_AUDOUTS,
  CC_BINS,      CC_CERTS,   CC_CLA,      CC_CLH,       CC_CLS,     CC_CMDS,
  CC_CON,       CC_CONLOG,  CC_CPU,      CC_CRASH,     CC_CREDITS, CC_CVARS,
  CC_CVCLR,     CC_CVLOAD,  CC_CVNPK,    CC_CVPEND,    CC_CVSAVE,  CC_DIR,
  CC_ENV,       CC_EVENTS,  CC_FBOS,     CC_FILES,     CC_FIND,    CC_FONTS,
  CC_FTFS,      CC_GPU,     CC_IMAGES,   CC_IMGFMTS,   CC_INPUT,   CC_JSONS,
  CC_LCALC,     CC_LCMDS,   CC_LEND,     CC_LEXEC,     CC_LFUNCS,  CC_LG,
  CC_LGC,       CC_LOG,     CC_LOGCLR,   CC_LPAUSE,    CC_LRESET,  CC_LRESUME,
  CC_LSTACK,    CC_LVARS,   CC_MASKS,    CC_MEM,       CC_MLIST,   CC_MODS,
  CC_OBJS,      CC_OGLEXT,  CC_OGLFUNC,  CC_PALETTES,  CC_PCMFMTS, CC_PCMS,
  CC_QUIT,      CC_RESTART, CC_SAMPLES,  CC_SHADERS,   CC_SHOT,    CC_SOCKETS,
  CC_SOCKRESET, CC_SOURCES, CC_SQLCHECK, CC_SQLDEFRAG, CC_SQLEND,  CC_SQLEXEC,
  CC_STOPALL,   CC_STREAMS, CC_SYSTEM,   CC_TEXTURES,  CC_THREADS, CC_TIME,
  CC_VERSION,   CC_VIDEOS,  CC_VMLIST,   CC_VRESET,    CC_WRESET,
  /* ----------------------------------------------------------------------- */
  MAX_CONCMD                           // Maximum console commands
};/* ======================================================================= */
/* ######################################################################### **
** ## BASE ENGINE COMMANDS TYPEDEFS                                       ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
typedef void (*ConCbFunc)(const Args &); // Cmd callback
/* ------------------------------------------------------------------------- */
struct ConLib                          // Console library command structure
{ /* ----------------------------------------------------------------------- */
  const string         strName;        // Function name
  const unsigned int   uiMinimum,      // Minimum parameters
                       uiMaximum;      // Maximum parameters
  const CoreFlagsConst cfcRequired;    // Required core flags
  const ConCbFunc      ccbFunc;        // Callback function
};/* ----------------------------------------------------------------------- */
struct ConLibStatic                    // For static engine commands list
{ /* ----------------------------------------------------------------------- */
  const string_view   &strvName;       // Function name
  const unsigned int   uiMinimum,      // Minimum parameters
                       uiMaximum;      // Maximum parameters
  const CoreFlagsConst cfcRequired;    // Required core flags
  const ConCbFunc      ccbFunc;        // Callback function
};/* ----------------------------------------------------------------------- */
typedef array<const ConLibStatic, MAX_CONCMD> ConCmdStaticList;
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
