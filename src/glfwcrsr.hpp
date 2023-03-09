/* == GLFWCRSR.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles creation of cursors using the Glfw subsystem.   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IGlFWCursor {                // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IGlFWUtil::P;          using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum GlFWCursorType : size_t           // Supported cursor types from GLFW
{ /* ----------------------------------------------------------------------- */
  CUR_ARROW,                           // [00] GLFW_ARROW_CURSOR
  CUR_CROSSHAIR,                       // [01] GLFW_CROSSHAIR_CURSOR
  CUR_HAND,                            // [02] GLFW_HAND_CURSOR
  CUR_HRESIZE,                         // [03] GLFW_HRESIZE_CURSOR
  CUR_IBEAM,                           // [04] GLFW_IBEAM_CURSOR
  CUR_NOT_ALLOWED,                     // [05] GLFW_NOT_ALLOWED_CURSOR
  CUR_RESIZE_ALL,                      // [06] GLFW_RESIZE_ALL_CURSOR
  CUR_RESIZE_EW,                       // [07] GLFW_RESIZE_EW_CURSOR
  CUR_RESIZE_NESW,                     // [08] GLFW_RESIZE_NESW_CURSOR
  CUR_RESIZE_NS,                       // [09] GLFW_RESIZE_NS_CURSOR
  CUR_RESIZE_NWSE,                     // [10] GLFW_RESIZE_NWSE_CURSOR
  CUR_VRESIZE,                         // [11] GLFW_VRESIZE_CURSOR
  /* ----------------------------------------------------------------------- */
  CUR_MAX                              // [12] Maximum supported cursors
};/* ----------------------------------------------------------------------- */
class GlFWCursor
{ /* ----------------------------------------------------------------------- */
  const int            iCursorId;      // GLFW specific cursor id
  GLFWcursor          *gcContext;      // Pointer to cursor
  /* -- Destroy the cursor ------------------------------------------------- */
  void CursorDoDeInit(void) { glfwDestroyCursor(gcContext); }
  /* -- Re-initialise the cursor ----------------------------------- */ public:
  void CursorInit(void) { gcContext = glfwCreateStandardCursor(iCursorId); }
  /* -- De-initialise the cursor ------------------------------------------- */
  void CursorDeInit(void) { CursorDoDeInit(); gcContext = nullptr; }
  /* -- Return the context ------------------------------------------------- */
  GLFWcursor *CursorGetContext(void) const { return gcContext; }
  /* -- Destruct that de-initialises the cursor ---------------------------- */
  ~GlFWCursor(void) { if(CursorGetContext()) CursorDoDeInit(); }
  /* -- Standby constructor ------------------------------------------------ */
  explicit GlFWCursor(const int iId) :
    /* -- Initialisers ----------------------------------------------------- */
    iCursorId(iId),                    // Assign cursor id for re-init
    gcContext(nullptr)                 // Create standard cursor...
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
