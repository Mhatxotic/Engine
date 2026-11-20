/* == CVARS.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles the logic behind CVars which are settings that  ## **
** ## the user can change to manipulate the engine's functionality. Add   ## **
** ## new cvars in cvardef.hpp and cvarlib.hpp.                           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICVars {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace ICodec::P;
using namespace ICommon::P;            using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IDir::P;
using namespace IError::P;             using namespace IFillCon::P;
using namespace IHelper::P;            using namespace IJson::P;
using namespace ILog::P;               using namespace IMetadata::P;
using namespace IMutex::P;             using namespace IPSplit::P;
using namespace ISql::P;               using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace LJson;
using namespace LSql;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* ------------------------------------------------------------------------- */
MAPPACK_BUILD(CVar, const StdString, CVarItem) // Build actual cvar list types
/* ------------------------------------------------------------------------- */
enum CVarDefaults : unsigned           // Flags when loaded from DB
{ /* -- (Note: Don't ever change these around) ----------------------------- */
  DC_NONE,                             // Accept current configuration
  DC_OVERWRITE,                        // Overwrite core variables only
  DC_REFRESH                           // Wipe database completely
}; /* ---------------------------------------------------------------------- */
struct CVars;                          // Class prototype
static CVars *cCVars = nullptr;        // Pointer to global class
struct CVars :                         // Start of vars class
  /* -- Base classes ------------------------------------------------------- */
  private InitHelper                   // Initialisation helper
{ /* -- Settings ----------------------------------------------------------- */
  constexpr static const size_t        // Some internal settings
    stCVarConfigSizeMinimum = 2,       // Minimum config file size
    stCVarConfigSizeMaximum = 1048576, // Maximum config file size
    stCVarConfigMaxLevel    = 10,      // Maximum recursive level
    stCVarMinLength         = 5,       // Minimum length of a cvar name
    stCVarMaxLength         = 255;     // Maximum length of a cvar name
  /* -- Private typedefs ------------------------------------------ */ private:
  using ArrayVars = StdArray<CVarMapIt, CVAR_MAX>;
  /* -- Private variables -------------------------------------------------- */
  size_t           stMaxInactiveCount; // Maximum Initial CVars allowed
  ssize_t          sstReqActiveSet,    // Requested active config set override
                   sstActiveSet;       // Actual activated set override
  CVarMap          cvmPending,         // CVars inactive list
                   cvmActive;          // CVars active list
  ArrayVars        avInternal;         // Quick lookup to internal vars
  StdString        strCBError;         // Callback error message
  /* ----------------------------------------------------------------------- */
  struct CVarsMapNameStruct            // Join initial with cvars
  { /* --------------------------------------------------------------------- */
    CVarMap       &cvmMap;             // Reference to variable list
    const StdStringView strName;       // Name of list
    /* --------------------------------------------------------------------- */
    CVarsMapNameStruct(CVarMap &cvmNMap, const StdStringView &strNName) :
      cvmMap(cvmNMap), strName{ strNName } {}
  };/* -- Holds pending and active lists ----------------------------------- */
  using CVarsMapNameStructArray = StdArray<const CVarsMapNameStruct, 2>;
  using CVarsMapNameStructArrayIt = CVarsMapNameStructArray::iterator;
  const CVarsMapNameStructArray cvmnsaList;
  /* -- Reference to default cvars ----------------------------------------- */
  const CVarItemStaticList &cvislList;
  /* ----------------------------------------------------------------------- */
  bool CVarsSetInitialVar(CVarItem &cviRef, const StdStringView &ssvVal,
    const CVarFlagsConst cvfcFlags)
  { // If same value as before?
    if(cviRef.CVarGetValue() == ssvVal)
    { // Flags are the same too?
      if(cviRef.FlagIsSet(cvfcFlags))
      { // Log that we're not overriding
        cLog->LogWarningExSafe("CVars initial var '$' already set to '$'!",
          cviRef.CVarGetVar(), ssvVal, StdIOSHex, cvfcFlags.FlagGet());
        return false;
      } // Flags are not the same so merge the flags and log what we did
      const CVarFlagsConst cvfcOldFlags{ cviRef.FlagGet() };
      cviRef.FlagSet(cvfcFlags);
      cLog->LogDebugExSafe("CVars initial var '$' already set to '$' "
        "with new flags from $$ to $ with $.",
        cviRef.CVarGetVar(), ssvVal, cvfcOldFlags.FlagGet(), StdIOSHex,
        cvfcOldFlags.FlagGet(), cviRef.FlagGet(), cvfcFlags.FlagGet());
    } // Value is different?
    else
    { // Log that we're overriding the specified variable
      cLog->LogDebugExSafe(
        "CVars initial variable '$' value overridden to '$'.",
        cviRef.CVarGetVar(), ssvVal);
      // Now override
      cviRef.CVarSetValue(ssvVal);
      cviRef.FlagSet(cvfcFlags);
    } // Success
    return true;
  }
  /* -- Finds a variable and returns it as read only --------------- */ public:
  template<class StrType>
    requires StdIsString<StrType>
  CVarMapConstIt CVarsFindVarConst(const StrType &strVar) const
    { return cvmActive.find(strVar); }
  /* -- Finds a variable and returns it as writable ------------------------ */
  template<class StrType>
    requires StdIsString<StrType>
  CVarMapIt CVarsFindVar(const StrType &strVar)
    { return cvmActive.find(strVar); }
  /* -- Returns if the specified standby variable exists ------------------- */
  template<class StrType>
    requires StdIsString<StrType>
  bool CVarsIsInitialVarExist(const StrType &strVar) const
    { return cvmPending.contains(strVar); }
  /* -- Returns if the specified variable exists --------------------------- */
  template<class StrType>
    requires StdIsString<StrType>
  bool CVarsIsVarExist(const StrType &strVar) const
    { return cvmActive.contains(strVar); }
  /* -- Set value of the specified iterator -------------------------------- */
  template<class StrType>
    requires StdIsString<StrType>
  CVarSetEnums CVarsSetIt(const CVarMapIt &cvmiIt, const StrType &strValue,
    const CVarFlagsConst cvfcFlags,
    const CVarConditionFlagsConst cvcfcFlags = CCF_THROWONERROR)
  { return cvmiIt->second.CVarCheckAndSetValue(StdString{ strValue },
      cvfcFlags, cvcfcFlags, strCBError); }
  /* -- Set value of the specified variable name --------------------------- */
  template<class StrTypeVar, class StrTypeVal>
    requires StdIsString<StrTypeVar> && StdIsString<StrTypeVal>
  CVarSetEnums CVarsSet(const StrTypeVar &strVar, const StrTypeVal &strValue,
    const CVarFlagsConst cvfcFlags,
    const CVarConditionFlagsConst cvcfcFlags = CCF_THROWONERROR)
  { // Find item and if variable is found? Goto the next step
    const CVarMapIt cvmiIt{ CVarsFindVar(strVar) };
    if(cvmiIt != cvmActive.end())
      return CVarsSetIt(cvmiIt, strValue, cvfcFlags, cvcfcFlags);
    // Just return if missing else throw an error
    if(cvcfcFlags.FlagIsSet(CCF_IGNOREIFMISSING)) return CVS_NOTFOUND;
    XC("CVar not found!", "Variable", strVar, "Value", strValue);
  }
  /* -- Check that the variable name is valid ------------------------------ */
  template<class StrType>
    requires StdIsString<StrType>
  bool CVarsIsValidVarName(const StrType &strVar) const
  { // Return failure if length is under minimum allowed or length is over
    // maximum allowed or first character is not valid.
    if(strVar.size() < stCVarMinLength ||
       strVar.size() > stCVarMaxLength ||
       StdIsNotAlpha(strVar.front())) return false;
    // Find an invalid character in the string
    const typename StrType::const_iterator sciPos{
      StdFindIf(par, StdNext(strVar.cbegin()), strVar.cend(),
        [](const char cC) { return StdIsNotAlnum(cC) && cC != '_'; }) };
    // Success if at end of string
    if(sciPos == strVar.cend()) return true;
    // Failure if not an underscore
    if(*sciPos != '_') return false;
    // Search rest of string but underscore is no longer allowed
    return StdFindIf(par_unseq, StdNext(sciPos), strVar.cend(),
      [](const char cC) { return StdIsNotAlnum(cC); }) == strVar.cend();
  }
  /* -- Return the cvar id's value as a string ----------------------------- */
  const StdString &CVarsGetStrInternal(const CVarEnums cveId) const
  { // Get internal iterator and return value or empty string if invalid
    const CVarMapConstIt cvmciIt{ CVarsGetInternalList()[cveId] };
    return cvmciIt != cvmActive.cend() ? cvmciIt->second.CVarGetValue() :
      cCommon->CommonBlankStr();
  }
  /* -- Return the cvar id's value as a C-string --------------------------- */
  const char *CVarsGetCStrInternal(const CVarEnums cveId)
    { return CVarsGetStrInternal(cveId).data(); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    const IntType CVarsGetInternal(const CVarEnums cveId)
  { return StrToNum<IntType>(CVarsGetStrInternal(cveId)); }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums CVarsResetIt(const CVarMapIt &cvmiIt,
    const CVarFlagsConst cvfcFlags = PCONSOLE,
    const CVarConditionFlagsConst cvcfcFlags = CCF_THROWONERROR)
  { return cvmiIt->second.CvarCheckAndResetValue(cvfcFlags,
      cvcfcFlags, strCBError); }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums CVarsReset(const CVarEnums cveId,
    const CVarFlagsConst cvfcFlags = PCONSOLE,
    const CVarConditionFlagsConst cvcfcFlags = CCF_THROWONERROR)
  { // Get internal iterator and return value or empty string if invalid
    const CVarMapIt cvmiIt{ CVarsGetInternalList()[cveId] };
    return cvmiIt != cvmActive.cend() ?
      CVarsResetIt(cvmiIt, cvfcFlags, cvcfcFlags) : CVS_NOTFOUND;
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums CVarsSetInternal(const CVarEnums cveId,
    const StdString &strValue, const CVarFlagsConst cvfcFlags = PCONSOLE,
    const CVarConditionFlagsConst cvcfcFlags = CCF_THROWONERROR)
  { // Get iterator and set the value if valid except return invalid
    const CVarMapIt cvmiIt{ CVarsGetInternalList()[cveId] };
    return cvmiIt != cvmActive.cend() ?
      cvmiIt->second.CVarCheckAndSetValue(strValue,
        cvfcFlags, cvcfcFlags, strCBError) : CVS_NOTFOUND;
  }
  /* ----------------------------------------------------------------------- */
  template<typename AnyType>
    CVarSetEnums CVarsSetInternalNum(const CVarEnums cveId, const AnyType atV)
  { return CVarsSetInternal(cveId, StrFromNum(atV)); }
  /* -- -------------------------------------------------------------------- */
  const ArrayVars &CVarsGetInternalList() const { return avInternal; }
  /* -- -------------------------------------------------------------------- */
  size_t CVarsGetInternalListSize() const
    { return CVarsGetInternalList().size(); }
  /* -- Return the cvar name's value as a string --------------------------- */
  static const StdString &CVarsGetStrIt(const CVarMapConstIt &cvmciIt)
    { return cvmciIt->second.CVarGetValue(); }
  /* -- Return the cvar name's default value as a string ------------------- */
  static const StdString &CVarsGetDefStrIt(const CVarMapConstIt &cvmciIt)
    { return cvmciIt->second.CVarGetDefValue(); }
  /* -- Unregister variable by iterator ------------------------------------ */
  void CVarsUnregister(const CVarMapIt &cvmiIt)
  { // Get cvar data and throw exception if variable has been locked
    const CVarItem &cviRef = cvmiIt->second;
    if(cviRef.FlagIsSet(LOCKED))
      XC("CVar cannot unregister in callback!",
        "Variable", cviRef.CVarGetVar());
    // Grab the variable for logging
    const StdString strVar{ cviRef.CVarGetVar() };
    // If this cvar is marked as commit, force save or loaded from database?
    // Move back into the initial list so it can be saved.
    if(cviRef.FlagIsAnyOfSet(COMMIT|LOADED))
      cvmPending.emplace(StdMove(*cvmiIt));
    // Erase iterator from list
    cvmActive.erase(cvmiIt);
    // Variable unregistered
    cLog->LogDebugExSafe("CVars unregistered variable '$'.",  strVar);
  }
  /* -- Do register a new variable without any checks ---------------------- */
  CVarMapIt CVarsRegister(const StdStringView &ssvVar,
    const StdStringView &ssvValue, CbFunc cbTrigger,
    const CVarFlagsConst cvfcFlags,
    const CVarConditionFlagsConst cvcfcFlags = CCF_NOTHING)
  { // Calculate set permission based on source
    auto SourceToPermission = [](const CVarFlagsConst cvfcS)->CVarFlagsConst
    { // Convert the S* flag to a P* flag and return it (order is important)
      return cvfcS.FlagIsClear(SANY)   ? PANY     :
           ((cvfcS.FlagIsSet(SCMDLINE) ? PCMDLINE : NONE) |
            (cvfcS.FlagIsSet(SAPPCFG)  ? PAPPCFG  : NONE) |
            (cvfcS.FlagIsSet(SUDB)     ? PUDB     : NONE) |
            (cvfcS.FlagIsSet(SCONSOLE) ? PCONSOLE : NONE) |
            (cvfcS.FlagIsSet(SLUA)     ? PLUA     : NONE));
    }; // Find initial cvar in initial pending list and if not found?
    const CVarMapIt cvmiPendIt{ cvmPending.find(ssvVar) };
    if(cvmiPendIt == cvmPending.cend())
    { // Register cvar into cvar list and get iterator to it. Although putting
      // the default value in here does not matter since we are about to set it
      // anyway, the decryptfailquiet flag will need it if the decrypt fails.
      const CVarMapIt cvmiIt{ cvmActive.insert({ StdString{ ssvVar },
        CVarItem{ ssvVar, ssvValue, cbTrigger, cvfcFlags } }).first };
      // Capture exceptions as we need to remove the variable if the value
      // failed to set for a multitude of reasons.
      try
      { // Use the default value. Although we already set the default value
        // when we inserted it, we need to check if it is valid too.
        cvmiIt->second.CVarCheckAndSetValue(StdString{ ssvValue },
          // Permission required to set the value
          SourceToPermission(cvfcFlags),
          // Merge throw on error and new cvar flag to supplied flags
          cvcfcFlags|CCF_THROWONERROR|CCF_NEWCVAR, strCBError);
      } // Exception occured?
      catch(const StdException &)
      { // Unregister the variable that was created to not cause problems when
        // for example, resetting LUA.
        cvmActive.erase(cvmiIt);
        // Rethrow the error
        throw;
      } // Return iterator
      return cvmiIt;
    } // Persistant var exists? Move into cvar list, remove persist & get data.
    const CVarMapIt cvmiIt{ cvmActive.emplace(StdMove(*cvmiPendIt)).first };
    cvmPending.erase(cvmiPendIt);
    // Capture exceptions as we need to remove the variable if the value failed
    // to set for a multitude of reasons.
    try
    { // Get reference to pending cvar
      CVarItem &cviRef = cvmiIt->second;
      // If it wasn't initialised from app manifest, set requested value as
      // the default value, otherwise the default value was overridden.
      if(cviRef.FlagIsClear(SAPPCFG)) cviRef.CVarSetDefValue(ssvValue);
      // Update the function callback
      cviRef.CVarSetTrigger(cbTrigger);
      // Strip flags from pending cvar and XOR them with requested flags and
      // a flag to say the flag was moved from the pending list (LOADED).
      cviRef.FlagReset(cvfcFlags | LOADED | (cviRef & CVREGMASK));
      // Update value and send appropriate perm based on where it came from
      cviRef.CVarCheckAndSetValue(
        // Do not use the saved value if the cvar isn't from the command-line,
        // the application manifest or loaded from the database
        cviRef.FlagIsAnyOfSet(CSAVEABLE|SAPPCFG|SCMDLINE) ?
          cviRef.CVarGetValue() : StdString{ ssvValue },
        // Permission required to set the value
        SourceToPermission(cviRef),
        // Merge throw on error and new cvar flag to supplied flags
        cvcfcFlags|CCF_THROWONERROR|CCF_NEWCVAR,
        // Error call back destination string
        strCBError);
      // Return iterator
      return cvmiIt;
    } // exception occured so remove the added item and rethrow the exception
    catch(const StdException &) { cvmActive.erase(cvmiIt); throw; }
  }
  /* -- Try to set the variable even if it doesnt exist and return result -- */
  bool CVarsSetVarOrInitial(const StdStringView &ssvVar,
    const StdStringView &ssvVal, const CVarFlagsConst cvfcFlags,
    const CVarConditionFlagsConst cvcfcFlags)
  { // Try to set the variable and grab the result
    switch(CVarsSet(ssvVar, ssvVal, cvfcFlags, cvcfcFlags|CCF_IGNOREIFMISSING))
    { // Not found? We need to add it to a standby list
      case CVS_NOTFOUND: break;
      // No error or not changed? return success!
      case CVS_OKNOTCHANGED: case CVS_OK: return true;
      // Failed status code
      default: return false;
    } // Check that the variable name is valid.
    if(!CVarsIsValidVarName(ssvVar))
    { // Throw if theres an error and return failure instead
      if(cvcfcFlags.FlagIsSet(CCF_THROWONERROR))
        XC("CVar name is not valid! Only alphanumeric characters "
          "and underscores are acceptable!",
          "Name",  ssvVar,             "Value",     ssvVal,
          "Flags", cvfcFlags.FlagGet(), "Condition", cvcfcFlags.FlagGet());
      return false;
    } // Look if the initial var already exists and if we found it? Set it
    const CVarMapIt cvmiIt{ cvmPending.find(ssvVar) };
    if(cvmiIt != cvmPending.cend())
    { // Get reference to pending cvar
      CVarItem &cviRef = cvmiIt->second;
      // Ignore if lesser priority source then what is allowed. For example,
      // we won't override command-line supplied variables ever with udb loaded
      // variables or application manifest variables.
      if(cviRef.CVarIsLowerPriority(cvfcFlags))
      { // Was sourced from application manifest?
        if(cvfcFlags.FlagIsSet(SAPPCFG))
        { // Update the default value and flags.
          cviRef.CVarSetDefValue(ssvVal);
          cviRef.FlagSet(cvfcFlags);
          // Log that this action was taken and return success
          cLog->LogDebugExSafe("CVars overriding default of '$' with '$'!",
            ssvVar, ssvVal);
          return true;
        } // Log that this action was denied and return failure
        cLog->LogWarningExSafe("CVars ignored overriding '$' with '$'!",
          ssvVar, ssvVal);
        return false;
      } // Else do the set and return success
      return CVarsSetInitialVar(cviRef, ssvVal, cvfcFlags);
    }
    // Check that we can create another variable
    if(cvmPending.size() >= stMaxInactiveCount)
    { // Can we throw error?
      if(cvcfcFlags.FlagIsSet(CCF_THROWONERROR))
        XC("Initial CVar count upper threshold reached!",
          "Variable", ssvVar, "Maximum", stMaxInactiveCount);
      // Log that this action was denied and return failure
      cLog->LogWarningExSafe(
        "CVars not adding '$' because upper threshold of $ reached!",
        ssvVar, stMaxInactiveCount);
      return false;
    } // Check ok so insert
    cvmPending.insert({ StdString{ ssvVar },
      CVarItem{ ssvVar, ssvVal, NoOp, cvfcFlags } });
    // Success
    return true;
  }
  /* -- Get a writable version of the variables list ----------------------- */
  CVarMap &CVarsGetVarList() { return cvmActive; }
  /* -- Get end of variable list ------------------------------------------- */
  CVarMapIt CVarsGetVarListEnd() { return cvmActive.end(); }
  /* -- Get number of registered variables (lua.hpp) ----------------------- */
  size_t CVarsGetVarCount() const { return cvmActive.size(); }
  /* -- Get a read-only version of the initial var list (conlib.hpp) ------- */
  const CVarMap &CVarsGetInitialVarList() const { return cvmPending; }
  /* -- Set an initial variable that already exists (console.hpp) ---------- */
  bool CVarsSetExistingInitialVar(const StdString &strVar,
    const StdString &strVal, const CVarFlagsConst cvfcFlags = PCONSOLE)
  { // Find initial item and return failure if it doesn't exist
    const CVarMapIt cvmiIt{ cvmPending.find(strVar) };
    if(cvmiIt == cvmPending.end()) return false;
    // Set the value and return success
    return CVarsSetInitialVar(cvmiIt->second, strVal, cvfcFlags);
  }
  /* -- Return last error from callback (also moves it) -------------------- */
  StdString CVarsGetCBError() { return StdMove(strCBError); }
  /* -- Return human readable value of the specified iterator -------------- */
  static StdString CVarsGetReadableValue(const CVarMapConstIt &cvmciIt)
    { return cvmciIt->second.CVarGetValueSafe(); }
  /* ----------------------------------------------------------------------- */
  size_t CvarsMarkAllEncodedVarsAsCommit()
  { // Total number of commits
    AtomicSizeT astCommitted{0};
    // Enumerate the initial list and cvar list asyncronously
    StdForEach(par_unseq, cvmnsaList.cbegin(), cvmnsaList.cend(),
      [&astCommitted](const CVarsMapNameStruct &cvmmRef)
      { // Enumerate the cvars in each list asynchronously
        StdForEach(par_unseq, cvmmRef.cvmMap.begin(), cvmmRef.cvmMap.end(),
          [&astCommitted](CVarMapPair &cvmpRef)
            { if(cvmpRef.second.CVarMarkEncodedVarAsCommit())
                ++astCommitted; });
      });
    // Return commit count
    return astCommitted;
  }
  /* ----------------------------------------------------------------------- */
  size_t CVarsSaveToDatabase()
  { // If there is no database?
    if(!cSql->SqlIsOpened())
    { // Log error silently and return
      cLog->LogWarningSafe("CVars commit skipped because no database!");
      return StdNPos;
    } // Return if there is no table
    if(cSql->SqlCVarCreateTable() == CTR_FAIL) return StdNPos;
    // Begin transaction and if failed? Log error silently
    if(cSql->SqlBegin() == SQLITE_ERROR)
      cLog->LogWarningExSafe("CVars commit begin transaction failed ($)! $.",
        cSql->SqlGetError(), cSql->SqlGetErrorAsIdString());
    // Total number of commits attempted which may need to be read and
    // written by multiple threads.
    AtomicSizeT astCommitTotal{0}, astPurgeTotal{0};
    // Enumerate the lists asynchronously
    StdForEach(par_unseq, cvmnsaList.cbegin(), cvmnsaList.cend(),
      [&astCommitTotal, &astPurgeTotal](const CVarsMapNameStruct &cvmpIt)
    { // Total number of commits attempted which may need to be read and
      // written by multiple threads.
      AtomicSizeT astCommit{0}, astPurge{0};
      // Iterate through the initial list and try to commit the cvar. We might
      // as well try to do this as quick as possible
      StdForEach(par_unseq, cvmpIt.cvmMap.begin(), cvmpIt.cvmMap.end(),
        [&astCommit, &astPurge](CVarMapPair &cvmpRef)
          { cvmpRef.second.CVarSave(astCommit, astPurge); });
      // Log variables written
      if(astCommit || astPurge)
        cLog->LogInfoExSafe("CVars committed $ and purged $ from $ pool.",
          astCommit.load(), astPurge.load(), cvmpIt.strName);
      // Add to totals
      astCommitTotal += astCommit;
      astPurgeTotal += astPurge;
    });
    // End transaction and if failed? Log error silently
    if(cSql->SqlEnd() == SQLITE_ERROR)
      cLog->LogWarningExSafe("CVars commit end transaction failed ($)! $.",
        cSql->SqlGetError(), cSql->SqlGetErrorAsIdString());
    // Get total changes and show a message on total changes
    if(!astCommitTotal && !astPurgeTotal)
      cLog->LogDebugSafe("CVars commit made no database changes.");
    // Return number of records saved or updated
    return astCommitTotal + astPurgeTotal;
  }
  /* ----------------------------------------------------------------------- */
  StdString CVarsGetInitialVar(const StdString &strKey)
  { // Find var and return empty string or the var
    const CVarMapConstIt cvmciIt{ cvmPending.find(strKey) };
    return cvmciIt != cvmPending.cend() ?
      cvmciIt->second.CVarGetValue() : cCommon->CommonBlankStr();
  }
  /* ----------------------------------------------------------------------- */
  size_t CVarsClean()
  { // Get all key names in cvars, no values
    if(!cSql->SqlCVarReadKeys()) return 0;
    // Get vars list
    const SqlResult &srRef = cSql->SqlGetRecords();
    if(srRef.empty())
    { // Log message and return failed
      cLog->LogInfoSafe("CVars found no variables to clean up!");
      return 0;
    } // Say how many records we're probing
    cLog->LogDebugExSafe("CVars probing $ records...", srRef.size());
    // Begin transaction
    cSql->SqlBegin();
    // Transactions committed
    size_t astCommit = 0;
    // For each record returned.
    for(const SqlRecordsMap &srmRef : srRef)
    { // Get key and goto next record if not found, else assign cvar name
      const SqlRecordsMapConstIt srmciIt{ srmRef.find(cSqls->strCVKeyColumn) };
      if(srmciIt == srmRef.cend()) continue;
      const SqlData &sdRef = srmciIt->second;
      // If is not a string?
      if(sdRef.iType != SQLITE_TEXT)
      { // Delete it from database and add to counter if ok and goto next cvar
        if(cSql->SqlCVarPurgeData(sdRef.MemPtr<char>(),
             sdRef.MemSize()) == PR_OK)
          ++astCommit;
        continue;
      } // Get string and find cvar name in live cvar list, ignore if it exists
      const StdString strKey{ sdRef.MemToStringSafe() };
      if(CVarsIsVarExist(strKey)) continue;
      // Find item in initial/standby cvar list, delete it if found
      const CVarMapConstIt cvmciIt{ cvmPending.find(strKey) };
      if(cvmciIt != cvmPending.end()) cvmPending.erase(cvmciIt);
      // Delete from database and proceed to the next record
      if(cSql->SqlCVarPurge(strKey) != PR_OK) continue;
      // Number of transactions send to sqlite
      ++astCommit;
    } // End bulk transaction commital
    cSql->SqlEnd();
    // If we have items to delete, we can say how many we deleted
    cLog->LogInfoExSafe("CVars removed $ of $ orphan records.",
      astCommit, srRef.size());
    // Clean up downloaded records
    cSql->SqlReset();
    // Return number of rows affected
    return astCommit;
  }
  /* ----------------------------------------------------------------------- */
  size_t CVarsLoadFromDatabase()
  { // Return if table is not already created or not available
    switch(cSql->SqlCVarCreateTable())
    { // Table already exists?
      case CTR_OK_ALREADY:
        // Read all the variables and break if true
        if(cSql->SqlCVarReadAll()) break;
        // Fall through to drop table and recreate it
        [[fallthrough]];
      // Some problem occurred?
      default:
        // Drop the table and throw exception if failed
        if(cSql->SqlCVarDropTable() != CTR_OK)
          XC("Failed to drop problematic CVars table!",
            "Reason", cSql->SqlGetErrorStr());
        // Create the table and throw exception if failed
        if(cSql->SqlCVarCreateTable() != CTR_OK)
          XC("Failed to re-create problematic CVars table!",
            "Reason", cSql->SqlGetErrorStr());
        // There are no variables to load so return no variables loaded
        [[fallthrough]];
      // Table was created? Nothing to load
      case CTR_OK: return 0;
    } // Number of variables loaded
    AtomicSizeT astLoaded{ 0 };
    // Get vars list and if not empty
    const SqlResult &srRef = cSql->SqlGetRecords();
    if(!srRef.empty())
    { // Log number of vars being parsed
      cLog->LogDebugExSafe("CVars read $ persistent variables, parsing...",
        srRef.size());
      // Mutex for synchronisation to engine. (no-op mutex on MacOS)
      MutexAuto maMutex;
      // For each record returned. Set each keypair returned, these are user
      // variables. We're using multithreading for this to accellerate
      // decryption and decompression routines.
      StdForEach(par_unseq, srRef.cbegin(), srRef.cend(),
        [this, &astLoaded, &maMutex](const SqlRecordsMap &srmRef)
      { // Get key and goto next record if not found, else set the key string
        const SqlRecordsMapConstIt
          srmciKeyIt{ srmRef.find(cSqls->strCVKeyColumn) };
        if(srmciKeyIt == srmRef.cend()) return;
        const SqlData &sdKeyRef = srmciKeyIt->second;
        if(sdKeyRef.iType != SQLITE_TEXT) return;
        // Convert the variable to string and ignore if invalid
        const StdString strVar{ sdKeyRef.MemToStringSafe() };
        // Get flags and goto next record if not found, else set the key string
        const SqlRecordsMapConstIt
          srmciFlagsIt{ srmRef.find(cSqls->strCVFlagsColumn) };
        if(srmciFlagsIt == srmRef.cend()) return;
        const SqlData &sdFlagsRef = srmciFlagsIt->second;
        if(sdFlagsRef.iType != SQLITE_INTEGER) return;
        const SqlCVarDataFlagsConst
          scfFlags{ sdFlagsRef.MemReadInt<sqlite_int64>() };
        // Get value and goto next record if not found, else set the value
        const SqlRecordsMapConstIt
          srmciValueIt{ srmRef.find(cSqls->strCVValueColumn) };
        if(srmciValueIt == srmRef.cend()) return;
        const SqlData &sdValueRef = srmciValueIt->second;
        // If the data pointed at 'V' is not encrypted?
        if(scfFlags.FlagIsClear(SD_ENCRYPTED))
        { // Must be text
          if(sdValueRef.iType != SQLITE_TEXT)
          { // Show warning and ignore if not
            cLog->LogWarningExSafe("CVars expected SQLITE_TEXT<$> "
              "and not type $ for '$'!",
              SQLITE_TEXT, sdValueRef.iType, strVar);
            return;
          } // Store value directly with synchronisation
          maMutex.MutexCall([this, &strVar, &sdValueRef, &astLoaded](){
            if(CVarsSetVarOrInitial(strVar, sdValueRef.MemToStringSafe(),
              PUDB|SUDB, CCF_NOTDECRYPTED))
                ++astLoaded;
          }); // Go to next item
          return;
        } // New decrypted value to write into
        StdString strNewValue;
        // Must be binary
        if(sdValueRef.iType != SQLITE_BLOB) return;
        // Capture exceptions because MagicBlock does that on error
        try
        { // Decrypt the value and get the result, and if that call fails?
          strNewValue = Block<CoDecoder>{ sdValueRef }.MemToStringSafe();
        } // exception occured?
        catch(const StdException &eReason)
        { // Log failure and try to reset the initial var so this does not
          // happen again and goto next record
          return cLog->LogErrorExSafe(
            "CVars variable '$' decrypt exception: $", strVar, eReason);
        }
        // Set the variable or place it in the initials list if cvar not not
        // registered. The access will be user mode only and assignments
        // are not to be marked as changed. Do not throw on error because it
        // is not easy to change a sql database manually if we change the
        // rules on a cvar.
        maMutex.MutexCall([this, &strVar, &strNewValue, &astLoaded](){
          if(CVarsSetVarOrInitial(strVar, strNewValue, PUDB|SUDB,
               CCF_DECRYPTED))
            ++astLoaded;
        });
      });
      // If we loaded all the variables? Report that we loaded all the vars
      if(astLoaded == srRef.size())
        cLog->LogInfoExSafe("CVars parsed all $ persistent variables.",
          srRef.size());
      // We didn't load all the variables? Report that we could not load all
      // the variables
      else cLog->LogWarningExSafe(
        "CVars only parsed $ of $ persistent variables!",
        astLoaded.load(), srRef.size());
    } // No variables loaded
    else cLog->LogInfoSafe("CVars found no variables to parse.");
    // Clean up downloaded records
    cSql->SqlReset();
    // Return loaded variables
    return astLoaded;
  }
  /* ----------------------------------------------------------------------- */
  void CVarsDeInit()
  { // Ignore if not initialised
    if(IHNotDeInitialise()) return;
    // Save all variables
    CVarsSaveToDatabase();
    // Log result then dereg core variables, they don't need testing
    cLog->LogDebugExSafe("CVars unregistering core variables...",
      CVarsGetInternalListSize());
    // Enumerate all the internal variables in reverse order
    size_t stVars = 0;
    for(size_t stIndex = CVarsGetInternalListSize() - 1;
               stIndex != StdMaxSizeT;
             --stIndex)
    { // Get iterator and unregister it if it is registered
      const CVarMapIt cvmiIt{ CVarsGetInternalList()[stIndex] };
      if(cvmiIt == cvmActive.end()) continue;
      avInternal[stIndex] = cvmActive.end();
      CVarsUnregister(cvmiIt);
      ++stVars;
    } // This should be 100% impossible but we'll deal with it just incase
    if(!cvmActive.empty())
    { // Something lingers which should be impossible but just incase
      cLog->LogWarningExSafe("CVars unregistered $ engine variables "
        "with $ lingering core engine variables!",
          CVarsGetInternalListSize(), cvmActive.size());
      // Repeat unregistering each remaining variable...
      do { CVarsUnregister(cvmActive.rbegin().base()); }
      // ...until the vars list is empty
      while(!cvmActive.empty());
      // Log warning that we finished
      cLog->LogWarningSafe("CVars unregistered all remaining variables!");
    } // All the core variables were unregistered
    else cLog->LogDebugExSafe("CVars unregistered $ core engine variables.",
           stVars);
  }
  /* ----------------------------------------------------------------------- */
  void CVarsInit()
  { // Object initialised
    IHInitialise();
    // Register each cvar
    for(size_t stIndex = 0; stIndex < cvislList.size(); ++stIndex)
    { // Get ref to cvar info and register the variable if the guimode is valid
      const CVarItemStatic &cvisRef = cvislList[stIndex];
      avInternal[stIndex] =
        cMetadata->MetadataIsCoreFlagsHave(cvisRef.cfcRequired) ?
          CVarsRegister(cvisRef.ssvVar, cvisRef.ssvValue, cvisRef.cbTrigger,
            cvisRef.cFlags, CCF_NOTHING) : cvmActive.end();
    } // Finished
    cLog->LogInfoExSafe(
      "CVars registered $ of $ built-in variables for $<0x$$>.",
      cvmActive.size(), cvislList.size(),
      cMetadata->MetadataGetCoreFlagsString(),
      StdIOSHex, cMetadata->MetadataGetCoreFlags());
  }
  /* -- Default constructor ------------------------------------- */ protected:
  explicit CVars(const CVarItemStaticList &cvislDef) :
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Set function name for init helper
    stMaxInactiveCount(CVAR_MAX),      // Initially set to max cvar count
    sstReqActiveSet(-2),               // Initalised by cvar later
    sstActiveSet(-2),                  // Initalised by cvar later
    avInternal{ FillConGeneric<ArrayVars>(cvmActive.end()) },
    cvmnsaList{{                       // Set combined lists
      { cvmPending, "unregistered" },  // Inactive cvars list
      { cvmActive,  "registered" } }}, // Active cvars list
    cvislList{ cvislDef }              // Default engine cvars list
    /* -- Set global pointer to static class ------------------------------- */
    { cCVars = this; }
  /* -- Destructor ---------------------------------------------- */ protected:
  DTORHELPER(~CVars, CVarsDeInit())
  /* --------------------------------------------------------------- */ public:
  CVarReturn CVarsSetDefaults(const CVarDefaults cvdVal)
  { // Compare defaults setting
    switch(cvdVal)
    { // Use current configuration
      case DC_NONE: break;
      // Set defaults only? Overwrite engine variables with defaults
      case DC_OVERWRITE:
        // Overwrite engine variables with defaults
        cLog->LogDebugSafe("CVars forcing default engine settings...");
        cSql->SqlBegin();
        for(const CVarMapIt &cvmiIt : CVarsGetInternalList())
          if(cvmiIt != cvmActive.end()) cSql->SqlCVarPurge(cvmiIt->first);
        cSql->SqlEnd();
        cLog->LogWarningSafe("CVars finished setting defaults.");
        // Done
        break;
      // Completely clear SQL cvars table.
      case DC_REFRESH:
        // Completely clear SQL cvars table.
        cLog->LogDebugSafe("CVars erasing saved engine settings...");
        cSql->SqlCVarDropTable();
        cSql->SqlCVarCreateTable();
        cLog->LogWarningSafe("CVars finished erasing saved engine settings.");
        // Done
        break;
      // Invalid value
      default: return DENY;
    } // Ok
    return ACCEPT;
  }
  /* ----------------------------------------------------------------------- */
  CVarReturn CVarsSetDisplayFlags(const CVarShowFlagsType cvsftFlags)
  { // Failed if flags are not valid
    if(cvsftFlags != CSF_NONE && (cvsftFlags & ~CSF_MASK)) return DENY;
    // Set the new flags
    csfShowFlags.FlagReset(cvsftFlags);
    // Done
    return ACCEPT;
  }
  /* -- Load settings if needed -------------------------------------------- */
  CVarReturn CVarsLoadSettings(const bool bDoLoad)
    { if(bDoLoad) CVarsLoadFromDatabase(); return ACCEPT; }
  /* -- Set maximum offline cvar count ------------------------------------- */
  CVarReturn CVarsMaxICountModified(const size_t stCount)
    { return CVarSimpleSetInt(stMaxInactiveCount, stCount); }
  /* -- Set active configuration set --------------------------------------- */
  CVarReturn CVarsSetConfigActiveSet(const ssize_t sstId)
    { return CVarSimpleSetIntNLG(sstReqActiveSet, sstId,
        static_cast<ssize_t>(-2), StdLimits<ssize_t>::max()); }
  /* -- Set and execute default app configuration file --------------------- */
  CVarReturn CVarsExecAppConfig(const StdString &strFile, StdString &strVal)
  { // Convert whole file data to a string
    const Json jsManifest{ StrAppend(strFile, "." JSON_EXTENSION) };
    // Check version is correct
    const unsigned uVersionRequired = 1,
                   uVersion = jsManifest.GetInteger("Version");
    if(uVersion != uVersionRequired)
      XC("Invalid application manifest version!",
        "Manfiest", jsManifest.NameGet(), "Required", uVersionRequired,
        "Actual",   uVersion);
    // Look for constants and throw if there are none then report them in log
    using LJson::Value;
    const Value &rjvConstants = jsManifest.GetValue("Constants");
    if(!rjvConstants.IsObject())
      XC("Constants array not valid!", "Manfiest", jsManifest.NameGet());
    // Total variables parsed, good vars and bad vars.
    size_t stGood = 0, stBad = 0;
    // Precompute compulsory flags
    const CVarFlagsConst cvfcFlags{ PAPPCFG|SAPPCFG };
    const CVarConditionFlagsConst cvcfcFlags{
      CCF_IGNOREIFMISSING|CCF_THROWONERROR|CCF_NOMARKCOMMIT };
    // Add the characters the manifest file cares about
    StdForEach(par_unseq, rjvConstants.MemberBegin(), rjvConstants.MemberEnd(),
      [this, &stGood, &stBad, cvfcFlags, cvcfcFlags]
        (const Value::Member &rjvItem)
    { // Get alias to keyname and value and check its type
      const Value &rjvKey = rjvItem.name, &rjvValue = rjvItem.value;
      switch(rjvValue.GetType())
      { // Json entry is a number type?
        case LJson::kNumberType:
          if(CVarsSetVarOrInitial(rjvKey.GetString(),
             (rjvValue.IsUint() ? StrFromNum(rjvValue.GetUint()) :
             (rjvValue.IsInt() ? StrFromNum(rjvValue.GetInt()) :
             StrFromNum(rjvValue.GetDouble()))), cvfcFlags, cvcfcFlags))
            ++stGood; else ++stBad;
          break;
        // Json entry is a string type?
        case LJson::kStringType:
          if(CVarsSetVarOrInitial(rjvKey.GetString(), rjvValue.GetString(),
             cvfcFlags, cvcfcFlags))
            ++stGood; else ++stBad;
          break;
        // Json entry is a boolean type?
        case LJson::kTrueType:
          if(CVarsSetVarOrInitial(rjvKey.GetString(),
             cCommon->CommonOne(), cvfcFlags, cvcfcFlags))
            ++stGood; else ++stBad;
          break;
        case LJson::kFalseType:
          if(CVarsSetVarOrInitial(rjvKey.GetString(),
             cCommon->CommonZero(), cvfcFlags, cvcfcFlags))
            ++stGood; else ++stBad;
          break;
        // Everything else is unsupported
        default:
          cLog->LogWarningExSafe(
            "CVars constant value type '$' for '$' is invalid!",
            rjvValue.GetType(), rjvKey.GetString());
          break;
      }
    }); // Compare requested active set
    if(sstReqActiveSet == -2)
    { // Optionally look for a active set value and if it exists. This useful
      // so developers can switch between different configurations easier.
      const Value::ConstMemberIterator
        vcmiIt{ jsManifest.FindMember("ActiveSet") };
      if(vcmiIt != jsManifest.MemberEnd())
      { // Get value and if it is not an unsigned integer throw an exception
        const Value &rjvActiveSet = vcmiIt->value;
        if(!rjvActiveSet.IsUint())
          XC("Active set id invalid in app manifest!",
            "Name", jsManifest.NameGet());
        // Set active set id
        sstActiveSet = static_cast<ssize_t>(rjvActiveSet.GetUint());
      } // Active set will not be chosen
      else sstActiveSet = -2;
    } // Set user specified override set (-1 will skip the below condition)
    else sstActiveSet = static_cast<ssize_t>(sstReqActiveSet);
    // Select an active configuration set?
    if(sstActiveSet >= 0)
    { // Convert the integer value to unsigned
      const size_t stIndex = static_cast<size_t>(sstActiveSet);
      // Make sure the sets member exists
      const Value::ConstMemberIterator vcmiIt{ jsManifest.FindMember("Sets") };
      if(vcmiIt == jsManifest.MemberEnd())
        XC("Active set array member 'Sets' not found in app manifest!",
          "Name", jsManifest.NameGet(), "Set", stIndex);
      // Get the sets value and it must be an array
      const Value &rjvActiveSetArray = vcmiIt->value;
      if(!rjvActiveSetArray.IsArray())
        XC("Active set array member 'Sets' has invalid type in app manifest!",
          "Name", jsManifest.NameGet(), "Set", stIndex);
      // Throw an exception if the specified value is out of range
      if(stIndex >= rjvActiveSetArray.Size())
        XC("Active set id out of range in app manifest!",
          "Name",    jsManifest.NameGet(), "Set", stIndex,
          "Maximum", rjvActiveSetArray.Size());
      // Throw an exception if the specified value is not an object
      using LJson::SizeType;
      const Value &rjvObject =
        rjvActiveSetArray[static_cast<SizeType>(stIndex)];
      if(!rjvObject.IsObject())
        XC("Active set id not a valid object!",
          "Name", jsManifest.NameGet(), "Set", stIndex);
      // Add the characters the manifest file cares about
      StdForEach(par_unseq, rjvObject.MemberBegin(),
                            rjvObject.MemberEnd(),
        [this, stIndex, &stGood, &stBad, cvfcFlags, cvcfcFlags]
          (const Value::Member &rjvItem)
      { // Get alias to keyname and value and compare type
        const Value &rjvKey = rjvItem.name, &rjvValue = rjvItem.value;
        switch(rjvValue.GetType())
        { // Json entry is a number type?
          case LJson::kNumberType:
            if(CVarsSetVarOrInitial(rjvKey.GetString(),
                 (rjvValue.IsUint() ? StrFromNum(rjvValue.GetUint()) :
                 (rjvValue.IsInt() ? StrFromNum(rjvValue.GetInt()) :
                 StrFromNum(rjvValue.GetDouble()))), cvfcFlags, cvcfcFlags))
              ++stGood; else ++stBad;
            break;
          // Json entry is a string type?
          case LJson::kStringType:
            if(CVarsSetVarOrInitial(rjvKey.GetString(), rjvValue.GetString(),
                 cvfcFlags, cvcfcFlags))
              ++stGood; else ++stBad;
            break;
          // Json entry is a boolean type?
          case LJson::kTrueType:
            if(CVarsSetVarOrInitial(rjvKey.GetString(),
               cCommon->CommonOne(), cvfcFlags, cvcfcFlags))
              ++stGood; else ++stBad;
            break;
          case LJson::kFalseType:
            if(CVarsSetVarOrInitial(rjvKey.GetString(),
               cCommon->CommonZero(), cvfcFlags, cvcfcFlags))
              ++stGood; else ++stBad;
            break;
          // Everything else is unsupported
          default:
            cLog->LogWarningExSafe(
              "CVars value for '$' in set '$' is invalid!",
              rjvKey.GetType(), stIndex, rjvKey.GetString());
            break;
        }
      }); // Log total cvars processed and log the result
      cLog->LogInfoExSafe(
        "CVars parsed $ of $ ($ bad) constant and set $ variables.",
        stGood, stGood + stBad, stBad, stIndex);
    } // Log total cvars processed and log the result
    else cLog->LogInfoExSafe("CVars parsed $ of $ ($ bad) constant variables.",
      stGood, stGood + stBad, stBad);
    // We are manually updating the value with the correct filename
    strVal = StdMove(jsManifest.NameGet());
    // Carry on parsing cvars
    return ACCEPT_HANDLED;
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
