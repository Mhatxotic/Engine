/* == GLFW.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Allows the engine to talk to GLFW easily.                           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IGlFW {                      // Start of module namespace
/* ------------------------------------------------------------------------- */
using namespace ICommon::P;            using namespace IError::P;
using namespace IGlFWCursor::P;        using namespace IGlFWUtil::P;
using namespace IGlFWWindow::P;        using namespace IHelper::P;
using namespace ILog::P;               using namespace IStd::P;
using namespace IString::P;            using namespace IToken::P;
using namespace ISysUtil::P;           using namespace IUtil::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
typedef array<GlFWCursor, CUR_MAX> CursorStandard;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ========================================================================= */
class GlFW;                            // Class prototype
static GlFW *cGlFW = nullptr;          // Pointer to global class
class GlFW :                           // Root engine class
  /* -- Base classes ------------------------------------------------------- */
  public InitHelper,                   // Initialisation helper
  public GlFWWindow,                   // GLFW window class
  private CursorStandard               // Standard cursors list
{ /* -- Private variables and functions ------------------------------------ */
  unsigned int     uiErrorLevel;       // Ignore further glfw errors
  const string_view strvIntVersion;    // Internal (headers) version number
  string           strExtVersion;      // External (library) version number
  StrSet           ssFeatures;         // Features included
  bool             bRawMouseSupported; // Is raw mouse motion supported
  /* -- Custom allocator --------------------------------------------------- */
  static void *GlFWAlloc(size_t stSize, void*const)
    { return StdAlloc<void>(stSize); }
  /* -- Custom reallocator ------------------------------------------------- */
  static void *GlFWReAlloc(void*const vpPtr, size_t stSize, void*const)
    { return StdReAlloc(vpPtr, stSize); }
  /* -- Custom free -------------------------------------------------------- */
  static void GlFWFree(void*const vpPtr, void*const) { StdFree(vpPtr); }
  /* -- Error handler prototype (full body at bottom) ---------------------- */
  static void ErrorHandler(int, const char*const);
  /* -- Error handler converted to thiscall -------------------------------- */
  void HandleError(const int iCode, const char*const cpDesc)
  { // What's the error code?
    switch(iCode)
    { // Errors that are safe to ignore
      case GLFW_FORMAT_UNAVAILABLE: [[fallthrough]];
      case GLFW_FEATURE_UNIMPLEMENTED: [[fallthrough]];
      case GLFW_FEATURE_UNAVAILABLE:
        return cLog->LogDebugExSafe("GlFW ignored API error $: $!",
          iCode, cpDesc);
      // Anything else?
      default:
        // Send an exception for the first infringement
        if(!uiErrorLevel++) XC(cpDesc, "Code", iCode);
        // Put further errors in the log to prevent message box spam
        cLog->LogWarningExSafe("GlFW got API error $/$: $!",
          uiErrorLevel, iCode, cpDesc);
        // Done
        return;
    }
  }
  /* --------------------------------------------------------------- */ public:
  const string_view &GetInternalVersion(void) const { return strvIntVersion; }
  /* -- DeInitialiser ------------------------------------------------------ */
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Report de-initialisation attempt
    cLog->LogDebugSafe("GlFW subsystem de-initialising...");
    // Destroy window if created
    this->WinDeInit();
    // De-Initialise standard cursors
    DeInitCursors();
    // Terminate glfw
    glfwTerminate();
    // Delete error handler
    glfwSetErrorCallback(nullptr);
    // Report de-initialisation successful
    cLog->LogInfoSafe("GlFW subsystem de-initialised.");
  }
  /* -- Return if raw mouse motion is supported ---------------------------- */
  bool IsRawMouseMotionSupported(void) const { return bRawMouseSupported; }
  bool IsNotRawMouseMotionSupported(void) const
    { return !IsRawMouseMotionSupported(); }
  /* -- Set the specified cursor ------------------------------------------- */
  void SetCursor(const GlFWCursorType gctCursorId)
    { WinSetCursorGraphic(at(gctCursorId).CursorGetContext()); }
  /* -- DeInitialise all standard cursors ---------------------------------- */
  void DeInitCursors(void)
  { // Enumerate each created fbo and deinitialise it (NOT destroy it)
    cLog->LogDebugExSafe("GlFW de-initialising $ standard cursors...", size());
    for(GlFWCursor &gcObj : *this) gcObj.CursorDeInit();
    cLog->LogInfoExSafe("GlFW de-initialised $ standard cursors.", size());
  }
  /* -- Initialise all standard cursors ------------------------------------ */
  void InitCursors(void)
  { // Enumerate each created fbo and reinitialise it
    cLog->LogDebugExSafe("GlFW initialising $ standard cursors...", size());
    for(GlFWCursor &gcObj : *this) gcObj.CursorInit();
    cLog->LogInfoExSafe("GlFW initialised $ standard cursors.", size());
  }
  /* -- Reset error level -------------------------------------------------- */
  void ResetErrorLevel(void) { uiErrorLevel = 0; }
  /* -- Verify the GLFW library -------------------------------------------- */
  void VerifyVersion(void)
  { // Get GLFW's identity
    if(const char*const cpIdentity = glfwGetVersionString())
    { // Parse each token (0 is always the version), rest is the features
      if(const Token tIdentity{ cpIdentity, cCommon->CommonSpace() })
      { // Store library version and If first token which says the version
        // mismatches with our version? Write a log message. It's not really a
        // problem since GlFW's headers maintain compatibility across versions.
        strExtVersion = StdMove(tIdentity.front());
        if(strExtVersion != GetInternalVersion())
          cLog->LogInfoExSafe("GlFW compiled with version '$' headers.",
            GetInternalVersion());
        // Parse features into a list
        StdForEach(seq, tIdentity.cbegin()+1, tIdentity.cend(),
          [this](const string &strStr)
            { ssFeatures.emplace(StdMove(strStr)); });
        // Write the features
        cLog->LogDebugExSafe("GlFW library $ features $ ($).",
          strExtVersion, StrExplodeEx(ssFeatures, ", ", " and "),
          ssFeatures.size());
      } // Failed to parse tokens
      else cLog->LogWarningExSafe("GlFW identity parse '$' fail!", cpIdentity);
    } // Failed to get identity
    else cLog->LogWarningSafe("GlFW failed to retrieve identity!");
  }
  /* -- Initialiser -------------------------------------------------------- */
  void Init(void)
  { // Report initialisation attempt
    cLog->LogDebugSafe("GlFW subsystem initialising...");
    // Setup custom allocator (Ubuntu 23.10 does not have 3.4 yet)
#if !defined(LINUX)
    const GLFWallocator gaInfo{ GlFWAlloc, GlFWReAlloc, GlFWFree, this };
    glfwInitAllocator(&gaInfo);
#endif
    // Set error callback which just throws an exception and reset error level
    glfwSetErrorCallback(ErrorHandler);
    ResetErrorLevel();
    // Initialise GlFW and throw exception if failed
    if(!glfwInit()) XC("GlFW initialisation failed!");
    // Report internal library version
    VerifyVersion();
    // Class initialised
    IHInitialise();
    // Initialise standard built-in operating system cursors
    InitCursors();
    // Set if raw mouse motion supported
    bRawMouseSupported = GlFWIsRawMouseMotionSupported();
    cLog->LogDebugExSafe("GlFW raw mouse motion support is $.",
      IsRawMouseMotionSupported() ? "available" : "unavailable");
    // Report initialisation successful
    cLog->LogInfoSafe("GlFW subsystem initialised.");
  }
  /* -- Destructor ---------------------------------------------- */ protected:
  DTORHELPER(~GlFW, DeInit())
  /* -- Constructor -------------------------------------------------------- */
  GlFW(void) :                         // Default constructor (No arguments)
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Set class function name
    /* --------------------------------------------------------------------- */
#define CURSOR(x) GlFWCursor{ GLFW_ ## x ## _CURSOR }
    /* --------------------------------------------------------------------- */
    CursorStandard{{                   // Define standard cursors
      CURSOR(ARROW),                   // [00] GLFW_ARROW_CURSOR
      CURSOR(CROSSHAIR),               // [01] GLFW_CROSSHAIR_CURSOR
      CURSOR(HAND),                    // [02] GLFW_HAND_CURSOR
      CURSOR(HRESIZE),                 // [03] GLFW_HRESIZE_CURSOR
      CURSOR(IBEAM),                   // [04] GLFW_IBEAM_CURSOR
#if defined(LINUX)                     // Using Linux (Ubuntu not 3.4 yet)
      CURSOR(ARROW),                   // [05] GLFW_ARROW (Ubuntu not 3.4 yet)
      CURSOR(ARROW),                   // [06] GLFW_ARROW (Ubuntu not 3.4 yet)
      CURSOR(ARROW),                   // [07] GLFW_ARROW (Ubuntu not 3.4 yet)
      CURSOR(ARROW),                   // [08] GLFW_ARROW (Ubuntu not 3.4 yet)
      CURSOR(ARROW),                   // [09] GLFW_ARROW (Ubuntu not 3.4 yet)
      CURSOR(ARROW),                   // [10] GLFW_ARROW (Ubuntu not 3.4 yet)
#else                                  // Using Windows or MacOS?
      CURSOR(NOT_ALLOWED),             // [05] GLFW_NOT_ALLOWED_CURSOR
      CURSOR(RESIZE_ALL),              // [06] GLFW_RESIZE_ALL_CURSOR
      CURSOR(RESIZE_EW),               // [07] GLFW_RESIZE_EW_CURSOR
      CURSOR(RESIZE_NESW),             // [08] GLFW_RESIZE_NESW_CURSOR
      CURSOR(RESIZE_NS),               // [09] GLFW_RESIZE_NS_CURSOR
      CURSOR(RESIZE_NWSE),             // [10] GLFW_RESIZE_NWSE_CURSOR
#endif                                 // OS check
      CURSOR(VRESIZE)                  // [11] GLFW_VRESIZE_CURSOR
    }},                                // Should have specified CUR_MAX cursors
    /* --------------------------------------------------------------------- */
#undef CURSOR                          // Done with this macro
    /* --------------------------------------------------------------------- */
    uiErrorLevel(0),                   // No errors occured
    strvIntVersion{                    // Init internal version number
      STR(GLFW_VERSION_MAJOR) "."      // (?.x.x) Major
      STR(GLFW_VERSION_MINOR) "."      // (x.?.x) Minor
      STR(GLFW_VERSION_REVISION)       // (x.x.?) Revision
    },
    /* --------------------------------------------------------------------- */
    bRawMouseSupported(false)          // Raw mouse support is detected soon
    /* -- Set global pointer to static class ------------------------------- */
    { cGlFW = this; }
};/* ----------------------------------------------------------------------- */
/* == Process a glfw error ================================================= */
void GlFW::ErrorHandler(int iCode, const char*const cpDesc)
  { cGlFW->HandleError(iCode, cpDesc); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
