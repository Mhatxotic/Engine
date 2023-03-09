/* == GLFWMON.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Neatly structures glfw's monitor and video enumeration information. ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IGlFWMonitor {               // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IDim;                  using namespace IError::P;
using namespace IGlFWUtil::P;          using namespace ILog::P;
using namespace IStd::P;               using namespace IString::P;
using namespace IUtil::P;              using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class GlFWRes                          // Members initially private
{ /* -- Private variables -------------------------------------------------- */
  const int      iIndex;               // Video mode index
  const GLFWvidmode vmData;            // Video mode data (C struct)
  const int      iDepth;               // Total bits
  /* -- Get video mode data ---------------------------------------- */ public:
  const GLFWvidmode &Data(void) const { return vmData; }
  /* -- Get resolution id -------------------------------------------------- */
  int Index(void) const { return iIndex; }
  /* -- Get resolution width/height in pixels ------------------------------ */
  int Width(void) const { return vmData.width; }
  int Height(void) const { return vmData.height; }
  /* -- Helpful checks ----------------------------------------------------- */
  bool IsWidth(const int iWidth) const { return Width() == iWidth; }
  bool IsHeight(const int iHeight) const { return Height() == iHeight; }
  bool IsDim(const int iWidth, const int iHeight) const
    { return IsWidth(iWidth) && IsHeight(iHeight); }
  /* -- Get resolution depth r/g/b component bits -------------------------- */
  int Red(void) const { return vmData.redBits; }
  int Green(void) const { return vmData.greenBits; }
  int Blue(void) const { return vmData.blueBits; }
  /* -- Get resolution depth total component bits -------------------------- */
  int Depth(void) const { return iDepth; }
  /* -- Get resolution refresh rate ---------------------------------------- */
  int Refresh(void) const { return vmData.refreshRate; }
  /* -- Are two resolutions the same? -------------------------------------- */
  bool Same(const GLFWvidmode &vmOther) const
    { return !memcmp(&vmData, &vmOther, sizeof(vmData)); }
  /* -- Constructor -------------------------------------------------------- */
  GlFWRes(const int iId, const GLFWvidmode &vmD) :
    /* -- Initialisers ----------------------------------------------------- */
    iIndex(iId),                       // Initialise video mode index
    vmData(vmD),                       // Initialise video mode data
    iDepth(Red() + Green() + Blue())   // Initialise video mode total bits
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
typedef vector<GlFWRes> GlFWResList;   // Vector of resolution classes
/* ------------------------------------------------------------------------- */
class GlFWMonitor :                    // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public GlFWResList,                  // Resolutions list
  public DimCoInt                      // Position and physical size
{ /* -- Private typedefs --------------------------------------------------- */
  typedef Dimensions<double> DimDouble;
  /* -- Private variables -------------------------------------------------- */
  const int      iIndex;               // Monitor index
  GLFWmonitor   *mContext;             // Monitor context
  const GlFWRes *rPrimary;             // Monitor primary resolution
  const DimDouble ddInches;            // Monitor size in inches
  const double   dDiagonal,            // Diagonal millimetre length
                 dDiagonalInches;      // Diagonal inches length
  const string   strName;              // Monitor name
  /* -- Get monitor position ----------------------------------------------- */
  DimCoInt Dim(GLFWmonitor*const mC) const
  { // Get monitor position and physical dimensions and return them
    int iX, iY; glfwGetMonitorPos(mC, &iX, &iY);
    int iW, iH; glfwGetMonitorPhysicalSize(mC, &iW, &iH);
    return { iX, iY, iW, iH };
  }
  /* -- Get monitor name as string ----------------------------------------- */
  string InitName(GLFWmonitor*const mC) const
  { // Get monitor name and if it's not null?
    if(const char*const cpName = GlFWGetMonitorName(mC))
    { // If monitor name is blank return blank name
      if(*cpName) return cpName;
      // Return blank name
      return cCommon->Unspec();
    } // Return null name
    return cCommon->Null();
  }
  /* -- Get monitor context ---------------------------------------- */ public:
  GLFWmonitor *Context(void) const { return mContext; }
  /* -- Get glfw monitor id ------------------------------------------------ */
  int Index(void) const { return iIndex; }
  /* -- Get primary monitor ------------------------------------------------ */
  const GlFWRes *Primary(void) const { return rPrimary; }
  /* -- Get monitor name --------------------------------------------------- */
  const string &Name(void) const { return strName; }
  /* -- Return diagonal size ----------------------------------------------- */
  double Diagonal(void) const { return dDiagonal; }
  double DiagonalInch(void) const { return dDiagonalInches; }
  /* -- Return size -------------------------------------------------------- */
  double WidthInch(void) const { return ddInches.DimGetWidth(); }
  double HeightInch(void) const { return ddInches.DimGetHeight(); }
  /* -- Constructor -------------------------------------------------------- */
  GlFWMonitor(const int iId, GLFWmonitor*const mC) :
    /* -- Initialisers ----------------------------------------------------- */
    DimCoInt{ Dim(mC) },               // Initialise physical position+size
    iIndex{ iId },                     // Initialise glfw index
    mContext(mC),                      // Initialise glfw context
    rPrimary(nullptr),                 // Initialise primary resolution
    ddInches{                          // Initialise physical size as inches
      UtilMillimetresToInches(DimGetWidth()),
      UtilMillimetresToInches(DimGetHeight()) },
    dDiagonal(UtilGetDiagLength(       // Initialise physical diagonal length
      DimGetWidth(),
      DimGetHeight())),
    dDiagonalInches(                   // Initialise diagonal length in inches
      UtilMillimetresToInches(dDiagonal)),
    strName{ StdMove(InitName(mC)) }   // Initialise monitor name
  /* -- No code ------------------------------------------------------------ */
  { // Get primary video mode information. Note that if a monitor is
    // just connecting, the width, height and refreshRate properties can be
    // zero which means we cannot detect the current resolution right now.
    if(const GLFWvidmode*const vPrimary = glfwGetVideoMode(mC))
    { // Get monitor, position, physical size and video modes
      int iModes;
      if(const GLFWvidmode*const vModes = glfwGetVideoModes(mC, &iModes))
      { // Modes count set?
        if(iModes)
        { // Video modes list to push
          reserve(static_cast<size_t>(iModes));
          // Enumerate video modes
          for(int iMode = 0; iMode < iModes; ++iMode)
          { // Monitor data to add. Do not use insert() here as it causes
            // a realloc and our pointers will be invalidated.
            const GlFWRes rData{ iMode, vModes[iMode] };
            emplace_back(StdMove(rData));
            // Ignore if this is not the active resolution for this display
            if(!back().Same(*vPrimary)) continue;
            // Set active resolution
            rPrimary = &back();
            // Add all the rest
            while(++iMode < iModes) push_back({ iMode, vModes[iMode] });
            // Done
            return;
          } // Could not detect primary resolution
          cLog->LogWarningExSafe("GlFW could not detect primary resolution "
            "for '$' so guessing with the last available resolution.",
            strName);
          // Set the primary resolution to the last one
          rPrimary = &back();
          // Done
          return;
        } // Impossible but log it just incase
        else cLog->LogWarningExSafe("GlFW returned valid video modes "
          "structure but zero video modes for '$' so falling back to primary "
          "resolution information.", strName);
      } // Log that enumeration was null
      else cLog->LogWarningExSafe("GlFW returned no video modes available "
        "for '$' so falling back to primary resolution information.",
        strName);
      // No video modes for this monitor so push back primary resolution
      push_back({ 0, *vPrimary });
    } // No primary resolution for this monitor
    else XC("Could not detect primary video mode!", "Monitor", strName);
  }
};/* ----------------------------------------------------------------------- */
typedef vector<GlFWMonitor> GlFWMonitorList; // Vector of monitor classes
/* ------------------------------------------------------------------------- */
struct GlFWMonitors :
  /* -- Base classes ------------------------------------------------------- */
  public GlFWMonitorList               // Monitors list
{ /* ----------------------------------------------------------------------- */
  const GlFWMonitor *moPrimary;        // Primary monitor
  /* -- Find a match from specified glfw monitor context ------------------- */
  const GlFWMonitor *Find(GLFWmonitor*const moCptr)
  { // Find the GLFW context in our structured classes
    typedef GlFWMonitorList::const_iterator GlFWMonitorListConstIt;
    const GlFWMonitorListConstIt gwmlciIt{
      StdFindIf(par_unseq, cbegin(), cend(), [moCptr](const GlFWMonitor &moIt)
        { return moIt.Context() == moCptr; }) };
    // Return the pointer to it or NULL if not found
    return gwmlciIt != cend() ? &(*gwmlciIt) : nullptr;
  }
  /* ----------------------------------------------------------------------- */
  void Refresh(void)
  { // Reset primary monitor
    moPrimary = nullptr;
    // Clear monitor list and free memory because we're using pointers to
    // static structures so we need to not ever trigger a realloc.
    clear();
    shrink_to_fit();
    // Get primary monitor information, throw exception if invalid
    if(GLFWmonitor*const mPrimary = glfwGetPrimaryMonitor())
    { // Fall back to this dummy monitor list if enumeration fails
      GLFWmonitor*const mFallbackMonitors[1]{ mPrimary };
      // Get monitors list and if we got them?
      int iMonitors;
      if(GLFWmonitor*const*mMonitors = glfwGetMonitors(&iMonitors))
      { // Have no monitors?
        if(!iMonitors)
        { // Log a warning
          cLog->LogWarningSafe("GlFW could not enumerate any monitors so "
            "falling back to primary monitor!");
          // Try using just the primary monitor
          mMonitors = mFallbackMonitors;
          // One monitor
          iMonitors = 1;
        } // Reserve space in monitors list
        reserve(static_cast<size_t>(iMonitors));
        // Display information about each new monitor
        for(int iMonitor = 0; iMonitor < iMonitors; ++iMonitor)
        { // Monitor data to add. Do not use const or insert() here as it
          // causes a realloc and our pointers will be invalidated.
          GlFWMonitor mData{ iMonitor, mMonitors[iMonitor] };
          emplace_back(StdMove(mData));
          // Set primary monitor handle
          if(back().Context() == mPrimary) moPrimary = &back();
        } // Check that we detected the primary resolution and monitor
        if(!moPrimary) XC("Could not detect primary monitor id!");
      } // Failed to detect monitor list
      else XC("Could not detect any monitors!");
    } // Could not detect primary monitor
    else XC("Could not detect primary monitor!");
  }
  /* -- Get primary monitor ------------------------------------------------ */
  const GlFWMonitor *Primary(void) const { return moPrimary; }
  /* ----------------------------------------------------------------------- */
  GlFWMonitors(void) : moPrimary(nullptr) { }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
