/* == METADATA.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Contains guest metadata.                                             ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IMetadata {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICVarDef::P;           using namespace ILookupArray::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Metadata class ------------------------------------------------------- */
class Metadata;                        // Prototype
static Metadata *cMetadata = nullptr;  // Global access to class
class Metadata                         // Common variables class
{ /* -- Private variables -------------------------------------------------- */
  StdStringView    ssvTitle,           // Guest title
                   ssvShortTitle,      // Guest short title
                   ssvVersion,         // Guest version
                   ssvAuthor,          // Guest author
                   ssvCopyright,       // Guest copyright
                   ssvDescription,     // Guest description
                   ssvWebsite;         // Guest website
  /* ----------------------------------------------------------------------- */
  CoreFlags        cfMode;             // Requested core subsystem flags
  /* ----------------------------------------------------------------------- */
  using ModeList = LookupArray<16>;    // List of possible combinations
  const ModeList   mList;              // Modes list
   /* -- Get descriptor strings ------------------------------------ */ public:
  const StdStringView &MetadataGetTitle() const { return ssvTitle; }
  const StdStringView &MetadataGetShortTitle() const { return ssvShortTitle; }
  const StdStringView &MetadataGetVersion() const { return ssvVersion; }
  const StdStringView &MetadataGetAuthor() const { return ssvAuthor; }
  const StdStringView &MetadataGetCopyright() const { return ssvCopyright; }
  const StdStringView &MetadataGetDescription() const
    { return ssvDescription; }
  const StdStringView &MetadataGetWebsite() const { return ssvWebsite; }
  /* ----------------------------------------------------------------------- */
  const CoreFlagsConst MetadataGetCoreFlags() const { return cfMode; }
  bool MetadataIsCoreFlagsHave(const CoreFlagsConst cfFlags) const
    { return !cfFlags || MetadataGetCoreFlags().FlagIsSet(cfFlags); }
  bool MetadataIsCoreFlagsNotHave(const CoreFlagsConst cfFlags) const
    { return !MetadataIsCoreFlagsHave(cfFlags); }
  bool MetadataIsGraphicalMode() const
    { return MetadataGetCoreFlags().FlagIsSet(CFL_VIDEO); }
  bool MetadataIsNotGraphicalMode() const
    { return !MetadataIsGraphicalMode(); }
  bool MetadataIsTextMode() const
    { return MetadataGetCoreFlags().FlagIsSet(CFL_TEXT); }
  bool MetadataIsNotTextMode() const { return !MetadataIsTextMode(); }
  bool MetadataIsAudioMode() const
    { return MetadataGetCoreFlags().FlagIsSet(CFL_AUDIO); }
  bool MetadataIsNotAudioMode() const { return !MetadataIsAudioMode(); }
  bool MetadataIsTimerMode() const
    { return MetadataGetCoreFlags().FlagIsSet(CFL_TIMER); }
  bool MetadataIsNotTimerMode() const { return !MetadataIsTimerMode(); }
  /* ----------------------------------------------------------------------- */
  const StdStringView
    &MetadataGetCoreFlagsString(const CoreFlagsConst cfFlags) const
  { return mList.Get(cfFlags); }
  /* ----------------------------------------------------------------------- */
  const StdStringView &MetadataGetCoreFlagsString() const
    { return MetadataGetCoreFlagsString(MetadataGetCoreFlags()); }
  /* -- Default constructor ------------------------------------- */ protected:
  Metadata() :
    /* -- Initialisers ----------------------------------------------------- */
    cfMode{ CFL_MASK },                // Guimode initially set by cvars
    mList{{                            // Initialise mode strings list
      "stdout",                        // [00<      0>] (nothing)
      "text",                          // [01<      1>] (text)
      "audio",                         // [02<      2>] (audio)
      "text+audio",                    // [03<    1|2>] (text+audio)
      "video",                         // [04<      4>] (video)
      "text+video",                    // [05<    1|4>] (video+text)
      "audio+video",                   // [06<    2|4>] (video+audio)
      "text+audio+video",              // [07<  1|2|4>] (text+audio+video)
      "timer",                         // [08<      8>] (timer)
      "text+timer",                    // [09<    1|8>] (text+timer)
      "audio+timer",                   // [10<    2|8>] (audio+timer)
      "text+audio+timer",              // [11<  1|2|8>] (text+audio+timer)
      "video+timer",                   // [12<    4|8>] (video+timer)
      "text+video+timer",              // [13<  1|4|8>] (video+text+timer)
      "audio+video+timer",             // [14<  2|4|8>] (video+audio+timer)
      "text+audio+video+timer",        // [15<1|2|4|8>] (text+audio+video+timr)
    }}                                 // Mode strings list initialised
  /* -- Code --------------------------------------------------------------- */
  { cMetadata = this; }
  /* -- CVar callbacks to update guest descriptor strings ---------- */ public:
  CVarReturn MetadataSetTitle(const StdString&, const StdString &strV)
    { ssvTitle = strV; return ACCEPT; }
  CVarReturn MetadataSetShortTitle(const StdString&, const StdString &strV)
  { ssvShortTitle = strV; return ACCEPT; }
  CVarReturn MetadataSetVersion(const StdString&, const StdString &strV)
    { ssvVersion = strV; return ACCEPT; }
  CVarReturn MetadataSetAuthor(const StdString&, const StdString &strV)
    { ssvAuthor = strV; return ACCEPT; }
  CVarReturn MetadataSetCopyright(const StdString&, const StdString &strV)
    { ssvCopyright = strV; return ACCEPT; }
  CVarReturn MetadataSetDescription(const StdString&, const StdString &strV)
  { ssvDescription = strV; return ACCEPT; }
  CVarReturn MetadataSetWebsite(const StdString&, const StdString &strV)
    { ssvWebsite = strV; return ACCEPT; }
  /* -- Set GUI mode status ------------------------------------------------ */
  CVarReturn MetadataSetCoreFlags(const CoreFlagsType cftFlags)
  { // Failed if bad value
    if(cftFlags != CFL_BASIC && (cftFlags & ~CFL_MASK)) return DENY;
    // Set new value
    cfMode.FlagReset(cftFlags);
    // Accepted
    return ACCEPT;
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
