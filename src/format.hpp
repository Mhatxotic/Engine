/* == FORMAT.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the header where we are outputting UTF data in OpenGL or    ## **
** ## the terminal. We handle parsing the control codes here. These are   ## **
** ## the following modules that currently use this interface.            ## **
** ######################################################################### **
** ## Console.HPP  ## app_cflags = CFL_BASIC ## Basic console output.     ## **
** ## FontBlit.HPP ## app_cflags & CFL_VIDEO ## GL font rendering output. ## **
** ## PixCon.HPP   ## app_cflags & CFL_TEXT  ## POSIX NCurses output.     ## **
** ## WinCon.HPP   ## app_cflags & CFL_TEXT  ## WIN32 console output.     ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFormat {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IUtf::P;
/* -- Control levels ------------------------------------------------------- */
static unsigned uBgLevel = 0,          // Background colour change level
                uFgLevel = 0;          // Foreground colour change level
constexpr static const size_t stRequiredChars = 8; // Req chars to scan for
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Finished handling ---------------------------------------------------- */
static void FormatHandleFinish(auto &&fcbPopFaceFunc, auto &&fcbPopOutlineFunc)
{ // Background level still set? reset it
  if(uFgLevel) { fcbPopFaceFunc(); uFgLevel = 0; }
  if(uBgLevel) { fcbPopOutlineFunc(); uBgLevel = 0; }
}
/* -- Handle print control character --------------------------------------- */
static void FormatPrintControl(UtfDecoder &udRef, const bool bSimulation,
  auto &&fcbSetFaceFunc, auto &&fcbPushFaceFunc, auto &&fcbPopFaceFunc,
  auto &&fcbSetOutlineFunc, auto &&fcbPushOutlineFunc,
  auto &&fcbResetOutlineFunc, auto &&fcbSetTextureFunc)
{ // Get next character
  switch(udRef.UtfNext())
  { // Colour selection
    case 'c':
    { // Scan for the hexadecimal value and if we found it?
      unsigned uCol;
      if(udRef.UtfScanValue(uCol) == stRequiredChars && !bSimulation &&
           !uFgLevel++) { fcbPushFaceFunc(); fcbSetFaceFunc(uCol); }
      break;
    } // Outline colour selection
    case 'o':
    { // Scan for the hexadecimal value and if we found it?
      unsigned uCol;
      if(udRef.UtfScanValue(uCol) == stRequiredChars && !bSimulation &&
           !uBgLevel++) { fcbPushOutlineFunc(); fcbSetOutlineFunc(uCol); }
      break;
    } // Reset colour request
    case 'r': if(bSimulation) break;
      // Decrement FG and BG colour counters, reset if zero.
      if(uFgLevel && !--uFgLevel) fcbPopFaceFunc();
      if(uBgLevel && !--uBgLevel) fcbResetOutlineFunc();
      break;
    // Print glyph
    case 't':
    { // Scan for the hexadecimal value and if we found it and we have
      // a glyphs texture assigned and is a valid glyph value?
      unsigned uGlyph;
      if(udRef.UtfScanValue(uGlyph) == stRequiredChars)
        fcbSetTextureFunc(uGlyph);
      break;
    } // Invalid control character.
    default: break;
  }
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
