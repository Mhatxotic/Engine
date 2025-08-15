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
namespace ICVar {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace ICodec::P;
using namespace ICommon::P;            using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IDir::P;
using namespace IError::P;             using namespace IHelper::P;
using namespace IJson::P;              using namespace ILog::P;
using namespace IPSplit::P;            using namespace ISql::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace Lib::Sqlite;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* ------------------------------------------------------------------------- */
MAPPACK_BUILD(CVar, const string, CVarItem) // Build actual cvar list types
/* ------------------------------------------------------------------------- */
enum CVarDefaults : unsigned int       // Flags when loaded from DB
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
  typedef array<CVarMapIt, CVAR_MAX> ArrayVars;
  /* -- Private variables -------------------------------------------------- */
  size_t           stMaxInactiveCount; // Maximum Initial CVars allowed
  CVarMap          cvmPending,         // CVars inactive list
                   cvmActive;          // CVars active list
  ArrayVars        avInternal;         // Quick lookup to internal vars
  string           strCBError;         // Callback error message
  /* ----------------------------------------------------------------------- */
  struct CVarMapNameStruct             // Join initial with cvars
  { /* --------------------------------------------------------------------- */
    CVarMap       &cvmMap;             // Reference to variable list
    const string_view strName;         // Name of list
  };/* --------------------------------------------------------------------- */
  typedef array<const CVarMapNameStruct,2> CVarMapNameStructArray;
  typedef CVarMapNameStructArray::iterator CVarMapNameStructArrayIt;
  const CVarMapNameStructArray cvmnsaList; // Holds pending and active lists
  /* ----------------------------------------------------------------------- */
  const CVarItemStaticList &cvislList; // Reference to default cvars
  /* --------------------------------------------------------------- */ public:
  bool InitialVarExists(const string &strVar) const
    { return cvmPending.contains(strVar); }
  /* ----------------------------------------------------------------------- */
  const CVarMapConstIt FindVariableConst(const string &strVar) const
    { return cvmActive.find(strVar); }
  /* ----------------------------------------------------------------------- */
  CVarMapIt FindVariable(const string &strVar)
    { return cvmActive.find(strVar); }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums Set(const CVarMapIt &cvmiIt, const string &strValue,
    const CVarFlagsConst cvfcFlags=PCONSOLE,
    const CVarConditionFlagsConst cvcfcFlags=CCF_THROWONERROR)
      { return cvmiIt->second.SetValue(strValue,
          cvfcFlags, cvcfcFlags, strCBError); }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums Set(const string &strVar, const string &strValue,
    const CVarFlagsConst cvfcFlags=PCONSOLE,
    const CVarConditionFlagsConst cvcfcFlags=CCF_THROWONERROR)
  { // Find item and if variable is found? Goto the next step
    const CVarMapIt cvmiIt{ FindVariable(strVar) };
    if(cvmiIt != cvmActive.end())
      return Set(cvmiIt, strValue, cvfcFlags, cvcfcFlags);
    // Just return if missing else throw an error
    if(cvcfcFlags.FlagIsSet(CCF_IGNOREIFMISSING)) return CVS_NOTFOUND;
    XC("CVar not found!", "Variable", strVar, "Value", strValue);
  }
  /* -- Check that the variable name is valid ------------------------------ */
  bool IsValidVariableName(const string &strVar)
  { // Check minimum name length
    if(strVar.length() < stCVarMinLength || strVar.length() > stCVarMaxLength)
      return false;
    // Get address of string. The first character must be a letter
    const unsigned char *ucpPtr =
      reinterpret_cast<const unsigned char*>(strVar.c_str());
    if(StdIsNotAlpha(*ucpPtr)) return false;
    // For each character in cvar name until end of string...
    for(const unsigned char*const ucpPtrEnd = ucpPtr + strVar.length();
                                   ++ucpPtr < ucpPtrEnd;)
    { // If it is an underscore?
      if(*ucpPtr == '_')
      { // Next character must be a letter. This could also catch a nullptr
        // character if at the end of string but thats okay too!
        if(StdIsNotAlpha(*(++ucpPtr))) return false;
        // Skip underscore and keep comparing with new conditions. The
        // underscore is now allowed normally.
        while(++ucpPtr < ucpPtrEnd)
          if(StdIsNotAlnum(*ucpPtr) && *ucpPtr != '_')
            return false;
        // Success!
        return true;
      } // Fail if not a letter
      else if(StdIsNotAlpha(*ucpPtr)) break;
    } // An underscore was not specified or invalid character
    return false;
  }
  /* ----------------------------------------------------------------------- */
  bool SetInitialVar(const string &strVar, const string &strVal,
    const CVarFlagsConst cvfcFlags=PCONSOLE,
    const CVarConditionFlagsConst cvcfcFlags=CCF_THROWONERROR)
  { // Check that the variable name is valid.
    if(!IsValidVariableName(strVar))
    { // Throw if theres an error and return failure instead
      if(cvcfcFlags.FlagIsSet(CCF_THROWONERROR))
        XC("CVar name is not valid! Only alphanumeric characters "
           "and underscores are acceptable!",
           "Name",  strVar,              "Value",     strVal,
           "Flags", cvfcFlags.FlagGet(), "Condition", cvcfcFlags.FlagGet());
      return false;
    } // Look if the initial var already exists and if we found it? Set it
    const CVarMapIt cvmiIt{ cvmPending.find(strVar) };
    if(cvmiIt != cvmPending.cend())
    { // Get reference to pending cvar
      CVarItem &cviRef = cvmiIt->second;
      // Ignore if lesser priority source then what is allowed. For example,
      // we won't override command-line supplied variables ever with udb loaded
      // variables or application manifest variables.
      if(cviRef.IsLowerPriority(cvfcFlags))
      { // Was sourced from application manifest?
        if(cvfcFlags & SAPPCFG)
        { // Update the default value and flags.
          cviRef.SetDefValue(strVal);
          cviRef.FlagSet(cvfcFlags);
          // Log that this action was taken
          cLog->LogDebugExSafe("CVars overriding default of '$' with '$'!",
            strVar, strVal);
        } // Wasn't sourced from the manifest?
        else
        { // Log that this action was denied and return failure
          cLog->LogWarningExSafe("CVars ignored overriding '$' with '$'!",
            strVar, strVal);
          return false;
        }
      } // Else do the set
      else SetInitialVar(cviRef, strVal, cvfcFlags);
    } // Insert into initial list
    else
    { // Check that we can create another variable
      if(cvmPending.size() >= stMaxInactiveCount)
      { // Can we throw error?
        if(cvcfcFlags.FlagIsSet(CCF_THROWONERROR))
          XC("Initial CVar count upper threshold reached!",
             "Variable", strVar, "Maximum", stMaxInactiveCount);
        // Log that this action was denied and return failure
        cLog->LogWarningExSafe(
          "CVars not adding '$' because upper threshold of $ reached!",
          strVar, stMaxInactiveCount);
        return false;
      } // Check ok so insert
      cvmPending.insert({ strVar,
        CVarItem{ strVar, strVal, NoOp, cvfcFlags } });
    } // Success
    return true;
  }
  /* -- Return the cvar id's value as a string ----------------------------- */
  const string &GetStrInternal(const CVarEnums cveId)
  { // Get internal iterator and return value or empty string if invalid
    const CVarMapConstIt cvmciIt{ GetInternalList()[cveId] };
    return cvmciIt != cvmActive.cend() ? cvmciIt->second.GetValue() :
      cCommon->CommonBlank();
  }
  /* -- Return the cvar id's value as a string ----------------------------- */
  const char *GetCStrInternal(const CVarEnums cveId)
    { return GetStrInternal(cveId).c_str(); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>const IntType GetInternal(const CVarEnums cveId)
    { return StdMove(StrToNum<IntType>(GetStrInternal(cveId))); }
  /* ----------------------------------------------------------------------- */
  void SetInitialVar(CVarItem &cviRef, const string &strVal,
    const CVarFlagsConst cvfcFlags)
  { // Ignore if same value
    if(cviRef.GetValue() == strVal)
    { // Flags are the same too?
      if(cviRef.FlagIsSet(cvfcFlags))
      { // Log that we're not overriding
        cLog->LogWarningExSafe("CVars initial var '$' already set to '$'/$$!",
          cviRef.GetVar(), strVal, hex, cvfcFlags.FlagGet());
      } // Flags need updating
      else
      { // Update flags
        cviRef.FlagSet(cvfcFlags);
        // Log that we're overriding flags
        cLog->LogWarningExSafe(
          "CVars initial var '$' flags overridden to $$ from $!",
            cviRef.GetVar(), hex, cvfcFlags.FlagGet(), cviRef.FlagGet());
      } // Done
      return;
    } // Log that we're overriding if priority
    cLog->LogWarningExSafe(
      "CVars initial var '$' overridden with '$'[$$] from '$'[$!]",
        cviRef.GetVar(), strVal, hex, cvfcFlags.FlagGet(), cviRef.GetValue(),
        cviRef.FlagGet());
    // Now override
    cviRef.SetValue(strVal);
    cviRef.FlagSet(cvfcFlags);
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums Reset(const CVarMapIt &cvmiIt,
    const CVarFlagsConst cvfcFlags=PCONSOLE,
    const CVarConditionFlagsConst cvcfcFlags=CCF_THROWONERROR)
      { return cvmiIt->second.ResetValue(cvfcFlags, cvcfcFlags, strCBError); }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums Reset(const CVarEnums cveId,
    const CVarFlagsConst cvfcFlags=PCONSOLE,
    const CVarConditionFlagsConst cvcfcFlags=CCF_THROWONERROR)
  { // Get internal iterator and return value or empty string if invalid
    const CVarMapIt cvmiIt{ GetInternalList()[cveId] };
    return cvmiIt != cvmActive.cend() ? Reset(cvmiIt, cvfcFlags, cvcfcFlags) :
      CVS_NOTFOUND;
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums SetInternal(const CVarEnums cveId, const string &strValue,
    const CVarFlagsConst cvfcFlags=PCONSOLE,
    const CVarConditionFlagsConst cvcfcFlags=CCF_THROWONERROR)
  { // Get iterator and set the value if valid except return invalid
    const CVarMapIt cvmiIt{ GetInternalList()[cveId] };
    return cvmiIt != cvmActive.cend() ?
      cvmiIt->second.SetValue(strValue,
        cvfcFlags, cvcfcFlags, strCBError) : CVS_NOTFOUND;
  }
  /* ----------------------------------------------------------------------- */
  template<typename AnyType>
    CVarSetEnums SetInternal(const CVarEnums cveId, const AnyType atV)
      { return SetInternal(cveId, StrFromNum(atV)); }
  /* ----------------------------------------------------------------------- */
  void RefreshSettings(void)
  { // Completely clear SQL cvars table.
    cLog->LogDebugSafe("CVars erasing saved engine settings...");
    cSql->CVarDropTable();
    cSql->CVarCreateTable();
    cLog->LogWarningSafe("CVars finished erasing saved engine settings.");
  }
  /* ----------------------------------------------------------------------- */
  void OverwriteExistingSettings(void)
  { // Overwrite engine variables with defaults
    cLog->LogDebugSafe("CVars forcing default engine settings...");
    cSql->Begin();
    for(const CVarMapIt &cvmiIt : GetInternalList())
      if(cvmiIt != cvmActive.end()) cSql->CVarPurge(cvmiIt->first);
    cSql->End();
    cLog->LogWarningSafe("CVars finished setting defaults.");
  }
  /* -- -------------------------------------------------------------------- */
  ArrayVars &GetInternalList(void) { return avInternal; }
  /* ----------------------------------------------------------------------- */
  bool VarExists(const string &strVar) const
    { return cvmActive.contains(strVar); }
  /* -- Return the cvar name's value as a string --------------------------- */
  const string &GetStr(const CVarMapConstIt &cvmciIt) const
    { return cvmciIt->second.GetValue(); }
  /* -- Return the cvar name's default value as a string ------------------- */
  const string &GetDefStr(const CVarMapConstIt &cvmciIt) const
    { return cvmciIt->second.GetDefValue(); }
  /* -- Unregister variable by iterator ------------------------------------ */
  void UnregisterVar(const CVarMapIt &cvmiIt)
  { // Get cvar data and throw exception if variable has been locked
    const CVarItem &cviRef = cvmiIt->second;
    if(cviRef.FlagIsSet(LOCKED))
      XC("CVar cannot unregister in callback!", "Variable", cviRef.GetVar());
    // Grab the variable for logging
    const string strVar{ cviRef.GetVar() };
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
  const CVarMapIt RegisterVar(const string &strVar, const string &strValue,
    CbFunc cbTrigger, const CVarFlagsConst cvfcFlags,
    const CVarConditionFlagsConst cvcfcFlags=CCF_NOTHING)
  { // Find initial cvar in initial pending list and if not found?
    const CVarMapIt cvmiPendIt{ cvmPending.find(strVar) };
    if(cvmiPendIt == cvmPending.cend())
    { // Register cvar into cvar list and get iterator to it. Although putting
      // the default value in here does not matter since we are about to set it
      // anyway, the decryptfailquiet flag will need it if the decrypt fails.
      const CVarMapIt cvmiIt{ cvmActive.insert({ strVar,
        CVarItem{ strVar, strValue, cbTrigger, cvfcFlags } }).first };
      // Capture exceptions as we need to remove the variable if the value
      // failed to set for a multitude of reasons.
      try
      { // Merge throw on error and new cvar flag to supplied flags
        const CVarConditionFlagsConst cvcfcSetFlags{
          cvcfcFlags|CCF_THROWONERROR|CCF_NEWCVAR };
        // Use the default value. Although we already set the default value
        // when we inserted it, we need to check if it is valid too.
        cvmiIt->second.SetValue(strValue, PANY, cvcfcSetFlags, strCBError);
      } // Exception occured?
      catch(const exception&)
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
      // the default value, otherwise the default value was overriden.
      if(cviRef.FlagIsClear(SAPPCFG)) cviRef.SetDefValue(strValue);
      // Update the function callback
      cviRef.SetTrigger(cbTrigger);
      // Strip flags from pending cvar and XOR them with requested flags and
      // a flag to say the flag was moved from the pending list (LOADED).
      cviRef.FlagReset(cvfcFlags | LOADED | (cviRef & CVREGMASK));
      // Do not use the saved value if the cvar isn't from the command-line,
      // the application manifest or loaded from the database
      const string &strNewValue =
        cviRef.FlagIsAnyOfSet(CSAVEABLE|SAPPCFG|SCMDLINE) ?
          cviRef.GetValue() : strValue;
      // Calculate permissions to use based on where the initial var came from
      const CVarFlagsConst cvfcPermissions{
        cviRef.FlagIsSet(SCMDLINE) ? PCMDLINE :
       (cviRef.FlagIsSet(SAPPCFG) ? PAPPCFG :
       (cviRef.FlagIsSet(SUDB) ? PUDB : PANY)) };
      // Merge throw on error and new cvar flag to supplied flags
      const CVarConditionFlagsConst cvcfcSetFlags{
        cvcfcFlags|CCF_THROWONERROR|CCF_NEWCVAR };
      // Update value and send appropriate perm based on where it came from
      cviRef.SetValue(strNewValue, cvfcPermissions, cvcfcSetFlags, strCBError);
      // Return iterator
      return cvmiIt;
    } // exception occured so remove the added item and rethrow the exception
    catch(const exception&) { cvmActive.erase(cvmiIt); throw; }
  }
  /* -- Try to set the variable even if it doesnt exist and return result -- */
  bool SetVarOrInitial(const string &strVar, const string &strVal,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst cvcfcFlags)
  { // Try to set the variable and grab the result
    switch(Set(strVar, strVal, cvfcFlags, cvcfcFlags|CCF_IGNOREIFMISSING))
    { // Not found? Set the variable in the initial list and return result
      case CVS_NOTFOUND:
        return SetInitialVar(strVar, strVal, cvfcFlags, cvcfcFlags);
      // No error or not changed? return success!
      case CVS_OKNOTCHANGED: case CVS_OK: return true;
      // Failed status code
      default: return false;
    }
  }
  /* ----------------------------------------------------------------------- */
  const CVarMap &GetVarList(void) { return cvmActive; }
  CVarMapIt GetVarListEnd(void) { return cvmActive.end(); }
  size_t GetVarCount(void) { return cvmActive.size(); }
  const CVarMap &GetInitialVarList(void) { return cvmPending; }
  /* ----------------------------------------------------------------------- */
  bool SetExistingInitialVar(const string &strVar, const string &strVal,
    const CVarFlagsConst cvfcFlags=PCONSOLE)
  { // Find initial item and return failure if it doesn't exist
    const CVarMapIt cvmiIt{ cvmPending.find(strVar) };
    if(cvmiIt == cvmPending.end()) return false;
    // Set the value
    SetInitialVar(cvmiIt->second, strVal, cvfcFlags);
    // Success
    return true;
  }
  /* -- Return last error from callback (also moves it) -------------------- */
  const string GetCBError(void) { return StdMove(strCBError); }
  /* ----------------------------------------------------------------------- */
  const string GetValueSafe(const CVarMapConstIt &cvmciIt) const
    { return cvmciIt->second.GetValueSafe(); }
  /* ----------------------------------------------------------------------- */
  const string GetValueSafe(const string &strVar) const
  { // Find item and return invalid if not found
    const CVarMapConstIt cvmciIt{ FindVariableConst(strVar) };
    return cvmciIt == cvmActive.cend() ?
      cCommon->CommonInvalid() : GetValueSafe(cvmciIt);
  }
  /* ----------------------------------------------------------------------- */
  size_t MarkAllEncodedVarsAsCommit(void)
  { // Total number of commits
    SafeSizeT stCommitted{0};
    // Enumerate the initial list and cvar list asyncronously
    StdForEach(par_unseq, cvmnsaList.cbegin(), cvmnsaList.cend(),
      [&stCommitted](const CVarMapNameStruct &cvmmRef)
      { // Enumerate the cvars in each list asynchronously
        StdForEach(par_unseq, cvmmRef.cvmMap.begin(), cvmmRef.cvmMap.end(),
          [&stCommitted](CVarMapPair &cvmpRef)
            { if(cvmpRef.second.MarkEncodedVarAsCommit()) ++stCommitted; });
      });
    // Return commit count
    return stCommitted;
  }
  /* ----------------------------------------------------------------------- */
  size_t Save(void)
  { // Done if sqlite database is not opened or vars table is not availabe
    if(!cSql->IsOpened() || cSql->CVarCreateTable() == Sql::CTR_FAIL)
      return StdNPos;
    // Begin transaction
    cSql->Begin();
    // Total number of commits attempted which may need to be read and
    // written by multiple threads.
    SafeSizeT stCommitTotal{0}, stPurgeTotal{0};
    // Enumerate the lists asyncronously
    StdForEach(par_unseq, cvmnsaList.cbegin(), cvmnsaList.cend(),
      [&stCommitTotal, &stPurgeTotal](const CVarMapNameStruct &cvmpIt)
    { // Total number of commits attempted which may need to be read and
      // written by multiple threads.
      SafeSizeT stCommit{0}, stPurge{0};
      // Iterate through the initial list and try to commit the cvar. We might
      // as well try to do this as quick as possible
      StdForEach(par_unseq, cvmpIt.cvmMap.begin(), cvmpIt.cvmMap.end(),
        [&stCommit, &stPurge](CVarMapPair &cvmpRef)
          { cvmpRef.second.Save(stCommit, stPurge); });
      // Log variables written
      if(stCommit || stPurge)
        cLog->LogInfoExSafe("CVars commited $ and purged $ from $ pool.",
          stCommit.load(), stPurge.load(), cvmpIt.strName);
      // Add to totals
      stCommitTotal += stCommit;
      stPurgeTotal += stPurge;
    });
    // End transaction
    cSql->End();
    // Return number of records saved or updated
    return stCommitTotal + stPurgeTotal;
  }
  /* ----------------------------------------------------------------------- */
  const string GetInitialVar(const string &strKey)
  { // Find var and return empty string or the var
    const CVarMapConstIt cvmciIt{ cvmPending.find(strKey) };
    return cvmciIt != cvmPending.cend() ?
      cvmciIt->second.GetValue() : cCommon->CommonBlank();
  }
  /* ----------------------------------------------------------------------- */
  size_t Clean(void)
  { // Get all key names in cvars, no values
    if(!cSql->CVarReadKeys()) return 0;
    // Get vars list
    const SqlResult &srRef = cSql->GetRecords();
    if(srRef.empty())
    { // Log message and return failed
      cLog->LogInfoSafe("CVars found no variables to clean up!");
      return 0;
    } // Say how many records we're probing
    cLog->LogDebugExSafe("CVars probing $ records...", srRef.size());
    // Begin transaction
    cSql->Begin();
    // Transactions committed
    size_t stCommit = 0;
    // For each record returned.
    for(const SqlRecordsMap &srmRef : srRef)
    { // Get key and goto next record if not found, else assign cvar name
      const SqlRecordsMapConstIt srmciIt{ srmRef.find(cSql->strCVKeyColumn) };
      if(srmciIt == srmRef.cend()) continue;
      const SqlData &sdRef = srmciIt->second;
      // If is not a string?
      if(sdRef.iType != SQLITE_TEXT)
      { // Delete it from database and add to counter if ok and goto next cvar
        if(cSql->CVarPurgeData(sdRef.MemPtr<char>(), sdRef.MemSize()) ==
             Sql::PR_OK)
          ++stCommit;
        continue;
      } // Get string and find cvar name in live cvar list, ignore if it exists
      const string strKey{ sdRef.MemToStringSafe() };
      if(VarExists(strKey)) continue;
      // Find item in initial/standby cvar list, delete it if found
      const CVarMapConstIt cvmciIt{ cvmPending.find(strKey) };
      if(cvmciIt != cvmPending.end()) cvmPending.erase(cvmciIt);
      // Delete from database and proceed to the next record
      if(cSql->CVarPurge(strKey) != Sql::PR_OK) continue;
      // Number of transactions send to sqlite
      ++stCommit;
    } // End bulk transaction commital
    cSql->End();
    // If we have items to delete, we can say how many we deleted
    cLog->LogInfoExSafe("CVars removed $ of $ orphan records.",
      stCommit, srRef.size());
    // Clean up downloaded records
    cSql->Reset();
    // Return number of rows affected
    return stCommit;
  }
  /* ----------------------------------------------------------------------- */
  size_t LoadFromDatabase(void)
  { // Return if table is not already created or not available
    switch(cSql->CVarCreateTable())
    { // Table already exists?
      case Sql::CTR_OK_ALREADY:
        // Read all the variables and break if true
        if(cSql->CVarReadAll()) break;
        // Fall through to drop table and recreate it
        [[fallthrough]];
      // Some problem occurred?
      default:
        // Drop the table and throw exception if failed
        if(cSql->CVarDropTable() != Sql::CTR_OK)
          XC("Failed to drop problematic CVars table!",
             "Reason", cSql->GetErrorStr());
        // Create the table and throw exception if failed
        if(cSql->CVarCreateTable() != Sql::CTR_OK)
          XC("Failed to re-create problematic CVars table!",
             "Reason", cSql->GetErrorStr());
        // There are no variables to load so return no variables loaded
        [[fallthrough]];
      // Table was created? Nothing to load
      case Sql::CTR_OK: return 0;
    } // Number of variables loaded
    SafeSizeT stLoaded{ 0 };
    // Get vars list and if not empty
    const SqlResult &srRef = cSql->GetRecords();
    if(!srRef.empty())
    { // Log number of vars being parsed
      cLog->LogDebugExSafe("CVars read $ persistent variables, parsing...",
        srRef.size());
      // For each record returned. Set each keypair returned, these are user
      // variables. We're using multithreading for this to accellerate
      // decryption and decompression routines.
      StdForEach(par_unseq, srRef.cbegin(), srRef.cend(),
        [this, &stLoaded](const SqlRecordsMap &srmRef)
      { // Get key and goto next record if not found, else set the key string
        const SqlRecordsMapConstIt
          srmciKeyIt{ srmRef.find(cSql->strCVKeyColumn) };
        if(srmciKeyIt == srmRef.cend()) return;
        const SqlData &sdKeyRef = srmciKeyIt->second;
        if(sdKeyRef.iType != SQLITE_TEXT) return;
        // Convert the variable to string and ignore if invalid
        const string strVar{ sdKeyRef.MemToStringSafe() };
        // Get flags and goto next record if not found, else set the key string
        const SqlRecordsMapConstIt
          srmciFlagsIt{ srmRef.find(cSql->strCVFlagsColumn) };
        if(srmciFlagsIt == srmRef.cend()) return;
        const SqlData &sdFlagsRef = srmciFlagsIt->second;
        if(sdFlagsRef.iType != SQLITE_INTEGER) return;
        const SqlCVarDataFlagsConst
          scfFlags{ sdFlagsRef.MemReadInt<sqlite_int64>() };
        // Get value and goto next record if not found, else set the value
        const SqlRecordsMapConstIt
          srmciValueIt{ srmRef.find(cSql->strCVValueColumn) };
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
          } // Store value directly with synchronisation and goto next
          if(SetVarOrInitial(strVar, sdValueRef.MemToStringSafe(),
            PUDB|SUDB, CCF_NOTDECRYPTED))
              ++stLoaded;
          return;
        } // New decrypted value to write into
        string strNewValue;
        // Must be binary
        if(sdValueRef.iType != SQLITE_BLOB) return;
        // Capture exceptions because MagicBlock does that on error
        try
        { // Decrypt the value and get the result, and if that call fails?
          strNewValue = Block<CoDecoder>{ sdValueRef }.MemToStringSafe();
        } // exception occured?
        catch(const exception &eReason)
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
        if(SetVarOrInitial(strVar, strNewValue, PUDB|SUDB, CCF_DECRYPTED))
          ++stLoaded;
      });
      // If we loaded all the variables? Report that we loaded all the vars
      if(stLoaded == srRef.size())
        cLog->LogInfoExSafe("CVars parsed all $ persistent variables.",
          srRef.size());
      // We didn't load all the variables? Report that we could not load all
      // the variables
      else cLog->LogWarningExSafe(
        "CVars only parsed $ of $ persistent variables!",
        stLoaded.load(), srRef.size());
    } // No variables loaded
    else cLog->LogInfoSafe("CVars found no variables to parse.");
    // Clean up downloaded records
    cSql->Reset();
    // Return loaded variables
    return stLoaded;
  }
  /* ----------------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if not initialised
    if(IHNotDeInitialise()) return;
    // Save all variables
    Save();
    // Log result then dereg core variables, they don't need testing
    cLog->LogDebugExSafe("CVars unregistering core variables...",
      GetInternalList().size());
    // Enumerate all the internal variables in reverse order
    size_t stVars = 0;
    for(size_t stIndex = GetInternalList().size()-1;
               stIndex != StdMaxSizeT;
             --stIndex)
    { // Get iterator and unregister it if it is registered
      const CVarMapIt cvmiIt{ GetInternalList()[stIndex] };
      if(cvmiIt == cvmActive.end()) continue;
      GetInternalList()[stIndex] = cvmActive.end();
      UnregisterVar(cvmiIt);
      ++stVars;
    } // This should be 100% impossible but we'll deal with it just incase
    if(!cvmActive.empty())
    { // Something lingers which should be impossible but just incase
      cLog->LogWarningExSafe("CVars unregistered $ engine variables "
        "with $ lingering core engine variables!",
          GetInternalList().size(), cvmActive.size());
      // Repeat unregistering each remaining variable...
      do { UnregisterVar(cvmActive.rbegin().base()); }
      // ...until the vars list is empty
      while(!cvmActive.empty());
      // Log warning that we finished
      cLog->LogWarningSafe("CVars unregistered all remaining variables!");
    } // All the core variables were unregistered
    else cLog->LogDebugExSafe("CVars unregistered $ core engine variables.",
           stVars);
  }
  /* ----------------------------------------------------------------------- */
  void Init(void)
  { // Object initialised
    IHInitialise();
    // Register each cvar
    for(size_t stIndex = 0; stIndex < cvislList.size(); ++stIndex)
    { // Get ref to cvar info and register the variable if the guimode is valid
      const CVarItemStatic &cvisRef = cvislList[stIndex];
      GetInternalList()[stIndex] =
        cSystem->IsCoreFlagsHave(cvisRef.cfcRequired) ?
          RegisterVar(string(cvisRef.strvVar), string(cvisRef.strvValue),
            cvisRef.cbTrigger, cvisRef.cFlags, CCF_NOTHING) : cvmActive.end();
    } // Finished
    cLog->LogInfoExSafe(
      "CVars registered $ of $ built-in variables for $<0x$$>.",
      cvmActive.size(), cvislList.size(), cSystem->GetCoreFlagsString(),
      hex, cSystem->GetCoreFlags());
  }
  /* -- Destructor ---------------------------------------------- */ protected:
  DTORHELPER(~CVars, DeInit())
  /* -- Default constructor ------------------------------------------------ */
  explicit CVars(const CVarItemStaticList &cvislDef) :
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Set function name for init helper
    stMaxInactiveCount(CVAR_MAX),      // Initially set to max cvar count
    avInternal{ UtilMkFilledContainer<ArrayVars>(cvmActive.end()) },
    cvmnsaList{{                       // Set combined lists
      { cvmPending, "unregistered" },  // Inactive cvars list
      { cvmActive,  "registered" } }}, // Active cvars list
    cvislList{ cvislDef }              // Default engine cvars list
    /* -- Set global pointer to static class ------------------------------- */
    { cCVars = this; }
  /* --------------------------------------------------------------- */ public:
  CVarReturn SetDefaults(const CVarDefaults cvdVal)
  { // Compare defaults setting
    switch(cvdVal)
    { // Use current configuration
      case DC_NONE: break;
      // Set defaults only? Overwrite engine variables with defaults
      case DC_OVERWRITE: OverwriteExistingSettings(); break;
      // Completely clear SQL cvars table.
      case DC_REFRESH: RefreshSettings(); break;
      // Invalid value
      default: return DENY;
    } // Ok
    return ACCEPT;
  }
  /* ----------------------------------------------------------------------- */
  CVarReturn SetDisplayFlags(const CVarShowFlagsType cvsftFlags)
  { // Failed if flags are not valid
    if(cvsftFlags != CSF_NONE && (cvsftFlags & ~CSF_MASK)) return DENY;
    // Set the new flags
    csfShowFlags.FlagReset(cvsftFlags);
    // Done
    return ACCEPT;
  }
  /* ----------------------------------------------------------------------- */
  CVarReturn LoadSettings(const bool bDoLoad)
  { // Load settings if needed
    if(bDoLoad) LoadFromDatabase();
    // Same
    return ACCEPT;
  }
  /* -- Set maximum offline cvar count ------------------------------------- */
  CVarReturn MaxICountModified(const size_t stCount)
    { return CVarSimpleSetInt(stMaxInactiveCount, stCount); }
  /* -- Set and execute default app configuration file --------------------- */
  CVarReturn ExecuteAppConfig(const string &strFile, string &strVal)
  { // Convert whole file data to a string
    const Json jsManifest{ StrAppend(strFile, "." JSON_EXTENSION) };
    // Check version is correct
    const unsigned int uiVersionRequired = 1;
    const unsigned int uiVersion = jsManifest.GetInteger("Version");
    if(uiVersion != uiVersionRequired)
      XC("Invalid application manifest version!",
         "Manfiest", jsManifest.IdentGet(), "Required", uiVersionRequired,
         "Actual",   uiVersion);
    // Look for constants and throw if there are none then report them in log
    using Lib::RapidJson::Value;
    const Value &rjvConstants = jsManifest.GetValue("Constants");
    if(!rjvConstants.IsObject())
      XC("Constants array not valid!", "Manfiest", jsManifest.IdentGet());
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
        case Lib::RapidJson::kNumberType:
          if(SetVarOrInitial(rjvKey.GetString(),
             (rjvValue.IsUint() ? StrFromNum(rjvValue.GetUint()) :
             (rjvValue.IsInt() ? StrFromNum(rjvValue.GetInt()) :
             StrFromNum(rjvValue.GetDouble()))), cvfcFlags, cvcfcFlags))
            ++stGood; else ++stBad;
          break;
        // Json entry is a string type?
        case Lib::RapidJson::kStringType:
          if(SetVarOrInitial(rjvKey.GetString(), rjvValue.GetString(),
             cvfcFlags, cvcfcFlags))
            ++stGood; else ++stBad;
          break;
        // Json entry is a boolean type?
        case Lib::RapidJson::kTrueType:
          if(SetVarOrInitial(rjvKey.GetString(),
             cCommon->CommonOne(), cvfcFlags, cvcfcFlags))
            ++stGood; else ++stBad;
          break;
        case Lib::RapidJson::kFalseType:
          if(SetVarOrInitial(rjvKey.GetString(),
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
    });
    // Optionally look for a active set value and if it exists. This useful so
    // developers can switch between different configurations easier.
    const Value::ConstMemberIterator
      vcmiIt{ jsManifest.FindMember("ActiveSet") };
    if(vcmiIt != jsManifest.MemberEnd())
    { // Get value and if it is not an unsigned integer throw an exception
      const Value &rjvActiveSet = vcmiIt->value;
      if(!rjvActiveSet.IsUint())
        XC("Active set id invalid in app manifest!",
           "Identifier", jsManifest.IdentGet());
      // Get the sets value and it must be an array
      const Value &rjvActiveSetArray = jsManifest.GetValue("Sets");
      if(!rjvActiveSetArray.IsArray())
        XC("Active set array invalid in app manifest!",
           "Identifier", jsManifest.IdentGet(),
           "Set",        rjvActiveSet.GetUint());
      // Throw an exception if the specified value is out of range
      if(rjvActiveSet.GetUint() >= rjvActiveSetArray.Size())
        XC("Active set id out of range in app manifest!",
           "Identifier", jsManifest.IdentGet(),
           "Set",        rjvActiveSet.GetUint(),
           "Maximum",    rjvActiveSetArray.Size());
      // Throw an exception if the specified value is not an object
      const Value &rjvObject = rjvActiveSetArray[rjvActiveSet.GetUint()];
      if(!rjvObject.IsObject())
        XC("Active set id not a valid object!",
           "Identifier", jsManifest.IdentGet(),
           "Set",        rjvActiveSet.GetUint());
      // Add the characters the manifest file cares about
      StdForEach(par_unseq, rjvObject.MemberBegin(),
                            rjvObject.MemberEnd(),
        [this, &rjvActiveSet, &stGood, &stBad, cvfcFlags, cvcfcFlags]
          (const Value::Member &rjvItem)
      { // Get alias to keyname and value and compare type
        const Value &rjvKey = rjvItem.name, &rjvValue = rjvItem.value;
        switch(rjvValue.GetType())
        { // Json entry is a number type?
          case Lib::RapidJson::kNumberType:
            if(SetVarOrInitial(rjvKey.GetString(),
               (rjvValue.IsUint() ? StrFromNum(rjvValue.GetUint()) :
               (rjvValue.IsInt() ? StrFromNum(rjvValue.GetInt()) :
               StrFromNum(rjvValue.GetDouble()))), cvfcFlags, cvcfcFlags))
              ++stGood; else ++stBad;
            break;
          // Json entry is a string type?
          case Lib::RapidJson::kStringType:
            if(SetVarOrInitial(rjvKey.GetString(), rjvValue.GetString(),
               cvfcFlags, cvcfcFlags))
              ++stGood; else ++stBad;
            break;
          // Json entry is a boolean type?
          case Lib::RapidJson::kTrueType:
            if(SetVarOrInitial(rjvKey.GetString(),
               cCommon->CommonOne(), cvfcFlags, cvcfcFlags))
              ++stGood; else ++stBad;
            break;
          case Lib::RapidJson::kFalseType:
            if(SetVarOrInitial(rjvKey.GetString(),
               cCommon->CommonZero(), cvfcFlags, cvcfcFlags))
              ++stGood; else ++stBad;
            break;
          // Everything else is unsupported
          default:
            cLog->LogWarningExSafe(
              "CVars value for '$' in set '$' is invalid!",
              rjvKey.GetType(), rjvActiveSet.GetUint(), rjvKey.GetString());
            break;
        }
      }); // Log total cvars processed and log the result
      cLog->LogInfoExSafe(
        "CVars parsed $ of $ ($ bad) constant and set $ variables.",
        stGood, stGood + stBad, stBad, rjvActiveSet.GetUint());
    } // Log total cvars processed and log the result
    else cLog->LogInfoExSafe("CVars parsed $ of $ ($ bad) constant variables.",
      stGood, stGood + stBad, stBad);
    // We are manually updating the value with the correct filename
    strVal = StdMove(jsManifest.IdentGet());
    // Carry on parsing cvars
    return ACCEPT_HANDLED;
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
