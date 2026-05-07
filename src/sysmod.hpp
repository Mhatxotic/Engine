/* == SYSMOD.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the common header for storing information about a shared    ## **
** ## object.                                                             ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysMod {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace ICVarDef::P;
using namespace IError::P;             using namespace ILog::P;
using namespace IPSplit::P;            using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == System module data =================================================== **
** ######################################################################### **
** ## Information about a module.                                         ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class SysModuleData :                  // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private PathSplit                    // Path parts to mod name
{ /* -- Variables ---------------------------------------------------------- */
  const unsigned   uMajor, uMinor,     // Major and minor version of module
                   uBuild, uRevision;  // Build and revision version of module
  const StdString  strVendor,          // Vendor of module
                   strDesc,            // Description of module
                   strComments,        // Comments of module
                   strVersion;         // Version as string
  /* --------------------------------------------------------------- */ public:
  const StdString &GetDrive() const { return strDrive; }
  const StdString &GetDir() const { return strDir; }
  const StdString &GetFile() const { return strFile; }
  const StdString &GetExt() const { return strExt; }
  const StdString &GetFileExt() const { return strFileExt; }
  const StdString &GetFull() const { return strFull; }
  const StdString &GetLoc() const { return strLoc; }
  unsigned GetMajor() const { return uMajor; }
  unsigned GetMinor() const { return uMinor; }
  unsigned GetBuild() const { return uBuild; }
  unsigned GetRevision() const { return uRevision; }
  const StdString &GetVendor() const { return strVendor; }
  const StdString &GetDesc() const { return strDesc; }
  const StdString &GetComments() const { return strComments; }
  const StdString &GetVersion() const { return strVersion; }
  /* -- Move constructor --------------------------------------------------- */
  SysModuleData(SysModuleData &&smdO) :
    /* -- Initialisers ----------------------------------------------------- */
    PathSplit{ StdMove(smdO) },                 // Copy filename
    uMajor(smdO.GetMajor()),                    // Copy major version
    uMinor(smdO.GetMinor()),                    // Copy minor version
    uBuild(smdO.GetBuild()),                    // Copy build version
    uRevision(smdO.GetRevision()),              // Copy revision version
    strVendor{ StdMove(smdO.GetVendor()) },     // Move vendor string
    strDesc{ StdMove(smdO.GetDesc()) },         // Move description string
    strComments{ StdMove(smdO.GetComments()) }, // Move comments string
    strVersion{ StdMove(smdO.GetVersion()) }    // Move version string
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialise all members contructor ---------------------------------- */
  explicit SysModuleData(const StdString &strF, const unsigned uMa,
    const unsigned uMi, const unsigned uBu, const unsigned uRe,
    StdString &&strVen, StdString &&strDe, StdString &&strCo,
    StdString &&strVer) :
    /* -- Initialisers ----------------------------------------------------- */
    PathSplit{ strF },                 // Copy filename
    uMajor(uMa),                     // Copy major version
    uMinor(uMi),                     // Copy minor version
    uBuild(uBu),                     // Copy build version
    uRevision(uRe),                  // Copy revision version
    strVendor{ StdMove(strVen) },      // Move vendor string
    strDesc{ StdMove(strDe) },         // Move description string
    strComments{ StdMove(strCo) },     // Move comments string
    strVersion{ StdMove(strVer) }      // Move version string
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialise filename only constructor ------------------------------- */
  explicit SysModuleData(const StdString &strF) :
    /* -- Initialisers ----------------------------------------------------- */
    PathSplit{ strF },                 // Initialise path parts
    uMajor(0),                        // Major version not initialised yet
    uMinor(0),                        // Minor version not initialised yet
    uBuild(0),                        // Build version not initialised yet
    uRevision(0)                      // Revision not initialised yet
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == System modules ======================================================= **
** ######################################################################### **
** ## Storage for all the modules loaded to this executable.              ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
MAPPACK_BUILD(SysMod, const size_t,const SysModuleData)
/* ------------------------------------------------------------------------- */
struct SysModules :
  /* -- Base classes ------------------------------------------------------- */
  public SysModMap                     // System module list
{ /* -- Move constructor ---------------------------------------- */ protected:
  SysModules(SysModules &&smOther) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModMap{ StdMove(smOther) }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init from SysModMap ----------------------------------------------- */
  explicit SysModules(SysModMap &&smlOther) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModMap{ StdMove(smlOther) }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Dump module list ------------------------------------------- */ public:
  CVarReturn DumpModuleList(const unsigned uShow)
  { // No modules? Return okay
    if(!uShow || empty()) return ACCEPT;
    // Print how many modules we are enumerating
    cLog->LogNLCInfoExSafe("System enumerating $ modules...", size());
    // For each shared module, print the data for it to log
    for(const SysModMapPair &smmpPair : *this)
    { // Get mod data and pathsplit data
      const SysModuleData &smdData = smmpPair.second;
      // Log the module data
      cLog->LogNLCInfoExSafe("- $ <$> '$' by '$' from '$'.",
        smdData.GetFileExt(), smdData.GetVersion(),
        smdData.GetDesc().empty() ?
          cCommon->CommonUnspec() : smdData.GetDesc(),
        smdData.GetVendor().empty() ?
          cCommon->CommonUnspec() : smdData.GetVendor(),
        smdData.GetLoc());
    } // Done
    return ACCEPT;
  }
};/* ----------------------------------------------------------------------- */
/* == System version ======================================================= **
** ######################################################################### **
** ## Information about the engine executable and modules loaded.         ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class SysVersion :
  /* -- Base classes ------------------------------------------------------- */
  public SysModules                    // System modules
{ /* ----------------------------------------------------------------------- */
  const SysModuleData &smdEng;         // Engine executable information
  /* ----------------------------------------------------------------------- */
  const StdStringView strvBuildType,   // Build type
                      strvCompVer,     // Compiler version
                      strvCompiled,    // Compilation date
                      strvCompiler,    // Compiler name
                      strvBuildTarget; // Target architechture
  /* -- Access to engine version data ------------------------------ */ public:
  const StdStringView &ENGBuildType() const { return strvBuildType; }
  const StdStringView &ENGCompVer() const { return strvCompVer; }
  const StdStringView &ENGCompiled() const { return strvCompiled; }
  const StdStringView &ENGCompiler() const { return strvCompiler; }
  const StdStringView &ENGTarget() const { return strvBuildTarget; }
  const StdString &ENGAuthor() const { return smdEng.GetVendor(); }
  const StdString &ENGComments() const { return smdEng.GetComments(); }
  const StdString &ENGDir() const { return smdEng.GetDir(); }
  const StdString &ENGDrive() const { return smdEng.GetDrive(); }
  const StdString &ENGExt() const { return smdEng.GetExt(); }
  const StdString &ENGFile() const { return smdEng.GetFile(); }
  const StdString &ENGFileExt() const { return smdEng.GetFileExt(); }
  const StdString &ENGFull() const { return smdEng.GetFull(); }
  const StdString &ENGLoc() const { return smdEng.GetLoc(); }
  const StdString &ENGName() const { return smdEng.GetDesc(); }
  const StdString &ENGVersion() const { return smdEng.GetVersion(); }
  unsigned ENGBits() const { return sizeof(void*)<<3; }
  unsigned ENGBuild() const { return smdEng.GetBuild(); }
  unsigned ENGMajor() const { return smdEng.GetMajor(); }
  unsigned ENGMinor() const { return smdEng.GetMinor(); }
  unsigned ENGRevision() const { return smdEng.GetRevision(); }
  /* -- Find executable module info and return reference to it -- */ protected:
  const SysModuleData &FindBaseModuleInfo(const size_t stId) const
  { // Find the module, we stored it as a zero, if not found?
    const SysModMap &smmMap = *this;
    const SysModMapConstIt smmciIt{ smmMap.find(stId) };
    if(smmciIt == smmMap.cend())
      XC("Failed to locate executable information!",
        "Length", smmMap.size(), "Instance", stId);
    // Return version data
    return smmciIt->second;
  }
  /* -- Init from SysModMap ----------------------------------------------- */
  SysVersion(SysModMap &&smlOther, const size_t stI) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModules{ StdMove(smlOther) },   // Move system modules list
    smdEng{ FindBaseModuleInfo(stI) }, // Move engine executable info
    strvBuildType{ BUILD_TYPE_LABEL }, // Build type
    strvCompVer{ COMPILER_VERSION },   // Compiler version
    strvCompiled{ VER_DATE },          // Compilation date
    strvCompiler{ COMPILER_NAME },     // Compiler name
    strvBuildTarget{ BUILD_TARGET }    // Target architechture
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
