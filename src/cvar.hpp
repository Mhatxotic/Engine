/* == CVAR.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles the logic behind a single cvar.                 ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICVarDef {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICodec::P;             using namespace ICommon::P;
using namespace IDir::P;               using namespace IError::P;
using namespace IFlags::P;             using namespace ILog::P;
using namespace IMemory::P;            using namespace ISql::P;
using namespace IStd::P;               using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(CVarShow,                  // CVar show flags
  /* ----------------------------------------------------------------------- */
  CSF_NONE                  {Flag(0)}, // Show no confidential cvars
  CSF_CONFIDENTIAL          {Flag(1)}, // Show private cvars
  CSF_PROTECTED             {Flag(2)}, // Show protected cvars
  /* ----------------------------------------------------------------------- */
  CSF_MASK { CSF_CONFIDENTIAL|CSF_PROTECTED }
);/* ----------------------------------------------------------------------- */
static CVarShowFlags csfShowFlags{CSF_NONE}; // Console cvar display flags
/* == Notes ================================================================ **
** ######################################################################### **
** ## We cannot define the CVar library now because we have not compiled  ## **
** ## the rest of the engine yet so we define this namespace so we can    ## **
** ## setup the ability for the CVars manager to access the CVars here    ## **
** ## and give us the ability to define the CVar library later on         ## **
** ######################################################################### **
** -- Default no-op for cvar lib callback functions ------------------------ */
static CVarReturn NoOp(CVarItem&, const StdString&) { return ACCEPT; }
/* ------------------------------------------------------------------------- */
enum CVarSetEnums : unsigned           // Cvar set return codes
{ /* ----------------------------------------------------------------------- */
  CVS_OK,                              // [00] Value set successful
  CVS_OKNOTCHANGED,                    // [01] Value set successful, no change
  CVS_NOTFOUND,                        // [02] Value not found
  CVS_NOTWRITABLE,                     // [03] Value set is not writable
  CVS_NOTINTEGER,                      // [04] Value set was not an integer
  CVS_NOTFLOAT,                        // [05] Value set was not a float
  CVS_NOTBOOLEAN,                      // [06] Value set was not a boolean
  CVS_NOTUNSIGNED,                     // [07] Value set was not >= 0
  CVS_NOTPOW2,                         // [08] Value set was not power of two
  CVS_NOTALPHA,                        // [09] Must contain only letters
  CVS_NOTNUMERIC,                      // [10] Must contain only digits
  CVS_NOTALPHANUMERIC,                 // [11] Must contain only alphanumerics
  CVS_NOTFILENAME,                     // [12] Must be a valid filename
  CVS_TRIGGERDENIED,                   // [13] Trigger callback denied change
  CVS_TRIGGEREXCEPTION,                // [14] Exception occured in trigger
  CVS_EMPTY,                           // [15] Value set was an empty string
  CVS_ZERO,                            // [16] Value not allowed to be zero
  CVS_NOTYPESET,                       // [17] No type is set
};/* ----------------------------------------------------------------------- */
BUILD_FLAGS(CVarCondition,             // For Set() functions
  /* ----------------------------------------------------------------------- */
  CCF_NOTHING               {Flag(0)}, // No flags specified?
  CCF_NOMARKCOMMIT          {Flag(1)}, // Do not mark as commit?
  CCF_IGNOREIFMISSING       {Flag(2)}, // Don't throw if var missing?
  CCF_THROWONERROR          {Flag(3)}, // Throw if there is an error?
  CCF_NEWCVAR               {Flag(4)}, // Variable was just registered?
  CCF_DECRYPTED             {Flag(5)}, // The variable was decrypted?
  CCF_NOTDECRYPTED          {Flag(6)}  // The variable was not encrypted?
);/* -- Cvar item class ---------------------------------------------------- */
class CVarItem :                       // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public CVarFlags                     // Cvar configuration settings
{ /* -- Private typedefs --------------------------------------------------- */
  using ValueIntType = int64_t;        // When handling integral values
  /* ----------------------------------------------------------------------- */
  enum CommitResult : unsigned         // Result to a commit request
  { /* --------------------------------------------------------------------- */
    CR_OK,                             // [00] Call commited the variable
    CR_FAIL,                           // [01] Call update cvar failed
    CR_OK_PURGE,                       // [02] Call succeeded with purge
    CR_OK_NOTHING_TO_DO,               // [03] Call succeeded with no changes
    CR_FAIL_PURGE,                     // [04] Call failed purge
    CR_FAIL_PURGE_NOT_CHANGED,         // [05] Call succeded but no changes
    CR_FAIL_PURGE_UNKNOWN_ERROR,       // [06] Unknown result from sql purge
    CR_FAIL_NOT_SAVEABLE,              // [07] CVar is not saveable
    CR_FAIL_LOADED_NOT_MODIFIED,       // [08] Cvar loaded but not modified
    CR_FAIL_ENCRYPT,                   // [09] Cvar could not be encrypted
    CR_FAIL_COMPRESS,                  // [10] Cvar could not be compress
  };/* -- Private variables ------------------------------------------------ */
  StdString        strVar,             // Variable name
                   strValue,           // Value name
                   strDefValue;        // Default value name
  CbFunc           cfTrigger;          // Callback trigger event
  /* -- Check and commit the cvar to database ------------------------------ */
  CommitResult CVarCheckAndCommit()
  { // Ignore if variable not modified, force saved or loaded
    if(FlagIsClear(COMMIT|COMMITNOCHECK)) return CR_OK_NOTHING_TO_DO;
    // If the value is the same as default value and nocheck not set?
    if(CVarIsValueUnchanged() && FlagIsClear(COMMITNOCHECK))
    { // Nothing to do if the flag wasn't loaded from the database
      if(FlagIsClear(LOADED)) return CR_OK_NOTHING_TO_DO;
      // Purge the cvar from the database
      switch(cSql->SqlCVarPurge(CVarGetVar()))
      { case PR_OK    : return CR_OK_PURGE;
        case PR_FAIL  : return CR_FAIL_PURGE;
        case PR_OK_NC : return CR_FAIL_PURGE_NOT_CHANGED;
        default       : return CR_FAIL_PURGE_UNKNOWN_ERROR;
      }
    } // Function to write the cvar to database
    auto CVarWrite = [this](const MemConst &mcSrc)->bool
      { return cSql->SqlCVarCommitBlob(CVarGetVar(), mcSrc); };
    // If we are to encrypt?
    if(FlagIsSet(CPROTECTED)) try
    { // Try encryption and/or compression and store result in database
      if(FlagIsSet(CDEFLATE)) CVarWrite(Block<AESZLIBEncoder>(CVarGetValue()));
      else CVarWrite(Block<AESEncoder>(CVarGetValue()));
    } // exception occured?
    catch(const StdException &eReason)
    { // Log exception
      cLog->LogErrorExSafe("CVars encrypt exception: $", eReason);
      // Capture exceptions again, try raw encoder and return string
      try { CVarWrite(Block<RAWEncoder>(CVarGetValue())); }
      // exception occured again?
      catch(const StdException &eReason2)
      { // Log exception and return failure
        cLog->LogErrorExSafe("CVars store exception: $", eReason2);
        return CR_FAIL_ENCRYPT;
      } // Success
    } // If we are to compress?
    else if(FlagIsSet(CDEFLATE)) try
    { // Try compression and commit the result to the database
      CVarWrite(Block<ZLIBEncoder>(CVarGetValue()));
    } // exception occured?
    catch(const StdException &eReason)
    { // Log exception
      cLog->LogErrorExSafe("CVars compress exception: $", eReason);
      // Capture exceptions again, try raw encoder and return string
      try { CVarWrite(Block<RAWEncoder>(CVarGetValue())); }
      // exception occured again?
      catch(const StdException &eReason2)
      { // Log exception and return failure
        cLog->LogErrorExSafe("CVars store exception: $", eReason2);
        return CR_FAIL_COMPRESS;
      } // Success
    } // Commit the unencrypted cvar and return if failed
    else if(!cSql->SqlCVarCommitString(CVarGetVar(), CVarGetValue()))
      return CR_FAIL;
    // Successfully saved so remove commit flag
    FlagClear(COMMIT);
    // Success
    return CR_OK;
  }
  /* -- Fired on exception when calling custom callback -------------------- */
  CVarSetEnums HandleCallbackException(const StdException &eReason,
    const CVarConditionFlagsConst ccfcFlags, const StdString &strNValue,
    StdString &strCBError)
  { // Throw exception if requested
    if(ccfcFlags.FlagIsSet(CCF_THROWONERROR))
      XC(eReason, "Variable", CVarGetVar(), "Value", strNValue);
    // Save the error string because we have no access to the console
    strCBError = StrAppend("CVar CB failed! > ", eReason);
    // Return error
    return CVS_TRIGGEREXCEPTION;
  }
  /* -- Set value post checking -------------------------------------------- */
  CVarSetEnums CVarPostSetValue(const StdString &strNValue,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst ccfcFlags,
    StdString &strCBError)
  { // If value is equal as current value then ignore it. We'll allow the
    // change when setting new vars because the triggers should trigger
    if(CVarGetValue() == strNValue && ccfcFlags.FlagIsClear(CCF_NEWCVAR))
      return CVS_OKNOTCHANGED;
    // Result storage
    CVarReturn cbrResult;
    // If it is a lua cvar?
    if(FlagIsSet(SLUA))
    { // Lock the cvar from being unregistered
      FlagSet(LOCKED);
      // Call the trigger and capture the result of the callback
      try { cbrResult = CVarGetTrigger()(*this, strNValue); }
      // exception occured?
      catch(const StdException &eReason)
      { // Remove the lock on the cvar
        FlagClear(LOCKED);
        // Handle exception and return error if no exception
        return HandleCallbackException(eReason, ccfcFlags, strNValue,
          strCBError);
      } // Remove the lock on the cvar
      FlagClear(LOCKED);
    } // Not a lua cvar? Call the trigger and capture the result of the cb
    else try { cbrResult = CVarGetTrigger()(*this, strNValue); }
    // exception occured
    catch(const StdException &eReason)
    { // Handle exception and return error if no exception
      return HandleCallbackException(eReason, ccfcFlags, strNValue,
        strCBError);
    } // If triggered denied to set the var? DON'T REARRANGE THESE!
    switch(cbrResult)
    { // The change was not allowed?
      case DENY:
        // Throw exception if requested otherwise return trigger denied code
        if(ccfcFlags.FlagIsSet(CCF_THROWONERROR))
          XC("CVar callback denied change!",
            "Variable", CVarGetVar(), "Value", strNValue);
        return CVS_TRIGGERDENIED;
        // The new value is acceptable? Set the new value
      case ACCEPT: CVarSetValue(strNValue); break;
      // The new value is acceptable, but the caller set the value? Ignore
      case ACCEPT_HANDLED: break;
      // Same as above but forcing commit? Set commit flag as requested
      case ACCEPT_HANDLED_FORCECOMMIT: FlagSet(COMMIT|COMMITNOCHECK); break;
      // Unknown return value?
      default:
        // Throw exception if requested otherwise return trigger denied code
        if(ccfcFlags.FlagIsSet(CCF_THROWONERROR))
          XC("CVar callback returned unknown value!",
            "Variable", CVarGetVar(), "Value", strNValue, "Result", cbrResult);
        return CVS_TRIGGEREXCEPTION;
    } // Free unused memory from cvar
    CVarGetModifyableValue().shrink_to_fit();
    // If decryption didn't fail and the no mark commit flag is set?
    if(ccfcFlags.FlagIsClear(CCF_NOTDECRYPTED|CCF_NOMARKCOMMIT))
    { // CVar is saveable?
      if(FlagIsSet(CSAVEABLE))
      { // Set commit flag if the cvar was not just registered or it was new
        // and initialised from the command line
        if(ccfcFlags.FlagIsClear(CCF_NEWCVAR) || FlagIsSet(SCMDLINE))
          FlagSet(COMMIT);
      } // Remove commit if set
      else if(FlagIsSet(COMMIT)) FlagClear(COMMIT);
    } // Function to return where the set came from
    auto fGetType = [cvfcFlags]()->const StdStringView&
    { // When you set a variable. We only specify one P* enum so the remaining
      // P* enums not specified are not set.
      switch(cvfcFlags.FlagGet())
      { // Return string where the set came from (order is important)
        case PCMDLINE : return cCommon->CommonFromCmdLine();
        case PAPPCFG  : return cCommon->CommonFromManifest();
        case PUDB     : return cCommon->CommonFromDatabase();
        case PCONSOLE : return cCommon->CommonFromConsole();
        // Engine set the variable
        default       : return cCommon->CommonBlank();
      } // Doesn't get here
    }; // Log the event
    cLog->LogDebugExSafe("CVars $ '$' to $$.",
      ccfcFlags.FlagIsSet(CCF_NEWCVAR) ?
        cCommon->CommonRegistered() : cCommon->CommonSet(),
      CVarGetVar(), CVarGetValueSafe(), fGetType());
    // Return success
    return CVS_OK;
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums CheckAndSetString(const StdString &strNValue,
    const CVarFlagsConst cvfcFlags, const CVarConditionFlagsConst ccfcFlags,
    StdString &strCBError)
  { // String cannot be empty and string is empty?
    if(FlagIsSet(CNOTEMPTY) && strNValue.empty())
    { // If we should not abort? Just return error else throw exception
      if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
        return CVS_EMPTY;
      XC("CVar specified cannot be empty!", "Variable", CVarGetVar());
    } // Check if valid untrusted pathname required
    if(FlagIsSet(CFILENAME))
    { // Check filename and get result
      switch(const ValidResult vrRes = DirValidName(strNValue))
      { // Break if ok or empty
        case VR_OK: case VR_EMPTY: break;
        // Show error otherwise
        default: if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
                   return CVS_NOTFILENAME;
                 XC("CVar untrusted path name is invalid!",
                   "Reason",   cDirBase->DirBaseVNRtoStr(vrRes),
                   "Result",   vrRes,
                   "Variable", CVarGetVar());
      }
    } // Check if valid trusted pathname required
    if(FlagIsSet(CTRUSTEDFN))
    { // Check filename and get result
      switch(const ValidResult vrRes =
        DirValidName(strNValue, VT_TRUSTED))
      { // Break if ok or empty
        case VR_OK: case VR_EMPTY: break;
        // Show error otherwise
        default : if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
                    return CVS_NOTFILENAME;
                  XC("CVar trusted path name is invalid!",
                    "Reason",   cDirBase->DirBaseVNRtoStr(vrRes),
                    "Result",   vrRes,
                    "Variable", CVarGetVar(),
                    "Path",     strNValue);
      }
    } // Alpha characters only?
    if(FlagIsSet(CALPHA))
    { // And numeric characters?
      if(FlagIsSet(CNUMERIC) && !StrIsAlphaNum(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
          return CVS_NOTALPHANUMERIC;
        XC("CVar specified must only contain alphanumeric characters!",
          "Variable", CVarGetVar());
      } // Only letters?
      if(!StrIsAlpha(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTALPHA;
        XC("CVar specified must only contain letters!",
          "Variable", CVarGetVar());
      }
    } // Must only contain numbers
    else if(FlagIsSet(CNUMERIC) && !StrIsInt(strNValue))
    { // If we should not abort? Just return error else throw exception
      if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTNUMERIC;
      XC("CVar specified must only contain numeric characters!",
        "Variable", CVarGetVar());
    } // Next step
    return CVarPostSetValue(strNValue, cvfcFlags, ccfcFlags, strCBError);
  }
  /* -- Returns the current trigger callback ----------------------- */ public:
  const CbFunc &CVarGetTrigger() const { return cfTrigger; }
  /* -- Sets a new custom trigger callback --------------------------------- */
  void CVarSetTrigger(const CbFunc &cfCb) { cfTrigger = cfCb; }
  /* -- Returns if a custom trigger callback is set ------------------------ */
  bool CVarIsTriggerSet() const { return CVarGetTrigger() != NoOp; }
  /* -- Get the read-only/writable version of the variable name ------------ */
  const StdString &CVarGetVar() const { return strVar; }
  StdString &CVarGetModifyableVar() { return strVar; }
  /* -- Get the read-only/writable version of the default value ------------ */
  const StdString &CVarGetDefValue() const { return strDefValue; }
  StdString &CVarGetModifyableDefValue() { return strDefValue; }
  /* -- Get current value -------------------------------------------------- */
  const StdString &CVarGetValue() const { return strValue; }
  StdString &CVarGetModifyableValue() { return strValue; }
  /* -- Get size and capacity of the default value ------------------------- */
  size_t CVarGetDefLength() const { return CVarGetDefValue().size(); }
  size_t CVarGetDefCapacity() const { return CVarGetDefValue().capacity(); }
  /* -- Return if cvar is lower priority than another cvar ----------------- */
  bool CVarIsLowerPriority(const CVarFlagsConst cvfcFlags) const
    { return (FlagGet() & SANY) < (cvfcFlags & SANY); }
  /* -- Get size and capacity of the value --------------------------------- */
  size_t CVarGetValueLength() const { return CVarGetValue().size(); }
  size_t CVarGetValueCapacity() const { return CVarGetValue().capacity(); }
  /* -- Return if the value is set or not ---------------------------------- */
  bool CVarIsValueUnset() const { return CVarGetValue().empty(); }
  bool CVarIsValueSet() const { return !CVarIsValueUnset(); }
  /* -- Return if the value has been changed or not ------------------------ */
  bool CVarIsValueChanged() const
    { return CVarGetValue() != CVarGetDefValue(); }
  bool CVarIsValueUnchanged() const { return !CVarIsValueChanged(); }
  /* -- Set current value to the default value ----------------------------- */
  void CVarSetDefault() { strValue = CVarGetDefValue(); }
  /* -- Set default value -------------------------------------------------- */
  void CVarSetDefValue(auto &&strV) { strDefValue = StdMove(strV); }
  /* -- Set current value -------------------------------------------------- */
  void CVarSetValue(auto &&strV) { strValue = StdMove(strV); }
  /* -- Returns a human readable version of the current value -------------- */
  StdString CVarGetValueSafe() const
  { // If confidential, return confidential
    if(FlagIsSet(CONFIDENTIAL) && csfShowFlags.FlagIsClear(CSF_CONFIDENTIAL))
      return "<Private>";
    // If protected, return protected
    if(FlagIsSet(CPROTECTED) && csfShowFlags.FlagIsClear(CSF_PROTECTED))
      return "<Protected>";
    // If value is empty, return as empty
    if(CVarIsValueUnset()) return StdString{ cCommon->CommonEmpty() };
    // If is a float, return value
    if(FlagIsSet(TFLOAT))
      return StrFromNum(StrToNum<double>(CVarGetValue()), 0, 12);
    // Is a boolean
    if(FlagIsSet(TBOOLEAN))
      return StdString{ StrFromBoolTF(StrToNum<bool>(CVarGetValue())) };
    // If is a integer, return number + hex
    if(FlagIsSet(TINTEGER))
    { // Get value as 64-bit integer
      const ValueIntType vitV = StrToNum<ValueIntType>(CVarGetValue());
      return StrFormat("$ [0x$$]", vitV, StdIOSHex, vitV);
    } // Unknown value or string. Return as-is.
    return StrFormat("\"$\"", CVarGetValue());
  }
  /* -- Marks the specified cvar as commit and write to database ----------- */
  bool CVarMarkEncodedVarAsCommit()
  { // Ignore if appropriate flag not set
    if(FlagIsClear(CPROTECTED|CDEFLATE)) return false;
    // Mark as commit
    FlagSet(COMMIT);
    // Commit the cvar and increment count if succeeded. 'this->'
    // needed to stop clang whining about 'this' not being used but it
    // is needed!
    return CVarCheckAndCommit() == CR_OK;
  }
  /* -- Save with counter increment ---------------------------------------- */
  template<typename IntType>void CVarSave(IntType &itCommit, IntType &itPurge)
  { // Try to commit the cvar. 'this->' needed to stop clang whining about
    // 'this' not being used but it is needed!
    switch(CVarCheckAndCommit())
    { // enum CommitResult
      case CR_OK       : itCommit = itCommit + 1; break; // Saved
      case CR_OK_PURGE : itPurge = itPurge + 1; break;   // Purged
      default          : break;                          // Error
    }
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums CVarCheckAndSetValue(const StdString &strNValue,
    const CVarFlagsConst &cvfcFlags, const CVarConditionFlagsConst ccfcFlags,
    StdString &strCBError)
  { // Failed if not writable
    if(FlagIsClear(cvfcFlags))
    { // If we should not abort? Just return error else throw exception
      if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTWRITABLE;
      XC("CVar is not writable from this scope!",
        "Variable", CVarGetVar(),        "Value", strNValue,
        "Scope",    cvfcFlags.FlagGet(), "Flags", FlagGet());
    } // If integer?
    if(FlagIsSet(TINTEGER))
    { // If number begins with '0x' to denote hexadecimal? Convert specified
      // value from hexadecimal to decimal and restart the call with the
      // converted value.
      if(strNValue.size() > 2 && strNValue[0] == '0' && strNValue[1] == 'x')
        return CVarPostSetValue(StrFromNum(StrHexToInt<ValueIntType>
          (strNValue.substr(2))), cvfcFlags, ccfcFlags, strCBError);
      // If specified value is not a valid integer?
      if(!StrIsInt(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTINTEGER;
        XC("CVar specified is not a valid integer!",
          "Variable", CVarGetVar(), "Value", strNValue);
      } // Deny negative values if unsigned only needed
      if(FlagIsSet(CUNSIGNED) && strNValue.front() == '-')
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTUNSIGNED;
        XC("CVar specified must be an unsigned integer!",
          "Variable", CVarGetVar(), "Value", strNValue);
      } // Deny non-power of two numbers but allow zero?
      if(FlagIsSet(CNOTEMPTY) && !StrToNum<uint64_t>(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_ZERO;
        XC("CVar specified must be non-zero!",
          "Variable", CVarGetVar(), "Value", strNValue);
      } // Deny non-power of two numbers?
      if(FlagIsSet(CPOW2) && !StrIsNumPOW2(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTPOW2;
        XC("CVar specified must be power of two!",
          "Variable", CVarGetVar(), "Value", strNValue);
      } // Next step
      return CVarPostSetValue(strNValue, cvfcFlags, ccfcFlags, strCBError);
    } // If float? Bail if not a floating point number
    if(FlagIsSet(TFLOAT))
    { // If specified value is not a valid floating point number?
      if(!StrIsFloat(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
          return CVS_NOTFLOAT;
        XC("CVar specified is not a valid number!",
          "Variable", CVarGetVar(), "Value", strNValue);
      } // Deny negative values if unsigned only needed
      if(FlagIsSet(CUNSIGNED) && strNValue.front() == '-')
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
          return CVS_NOTUNSIGNED;
        XC("CVar specified must be a non-negative float!",
          "Variable", CVarGetVar(), "Value", strNValue);
      } // Next step
      return CVarPostSetValue(strNValue, cvfcFlags, ccfcFlags, strCBError);
    } // Is a boolean?
    if(FlagIsSet(TBOOLEAN))
    { // Must be one byte, then test first character
      if(strNValue.size() == 1) switch(strNValue.front())
      { // Is zero or one? OK
        case '0': case '1':
          return CVarPostSetValue(strNValue, cvfcFlags, ccfcFlags, strCBError);
        // Invalid value
        default: break;
      } // True?
      else if(strNValue.size() == 4 &&
        StrToLowCase(strNValue) == cCommon->CommonTrue())
          return CVarPostSetValue(StdString{ cCommon->CommonOne() },
            cvfcFlags, ccfcFlags, strCBError);
      // False?
      else if(strNValue.size() == 5 &&
        StrToLowCase(strNValue) == cCommon->CommonFalse())
          return CVarPostSetValue(StdString{ cCommon->CommonZero() },
            cvfcFlags, ccfcFlags, strCBError);
      // If we should not abort? Just return error else throw exception
      if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
        return CVS_NOTBOOLEAN;
      XC("CVar specified is not a valid boolean!",
        "Variable", CVarGetVar(), "Value", strNValue);
    } // Is a string?
    if(FlagIsSet(TSTRING))
    { // If not modification flags are set? Just set to verify
      if(FlagIsClear(MMASK))
        return CheckAndSetString(strNValue, cvfcFlags, ccfcFlags, strCBError);
      // We're modifying the value
      StdString strModifiedValue{ strNValue };
      // Remove whitespaces?
      if(FlagIsSet(MTRIM)) StrTrimRef(strModifiedValue, ' ');
      // Set uppercase or lowercase?
      if(FlagIsSet(MUPPER)) StrToUpCaseRef(strModifiedValue);
      else if(FlagIsSet(MLOWER)) StrToLowCaseRef(strModifiedValue);
      // Verify and return  result
      return CheckAndSetString(strModifiedValue, cvfcFlags, ccfcFlags,
        strCBError);
    } // If we should not throw error? Just return code else throw exception
    if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTYPESET;
    XC("CVar type is not set!",
      "Variable", CVarGetVar(), "NewValue", strNValue,
      "OldValue", CVarGetValue(), "Flags",    FlagGet());
  }
  /* -- Set the value to default with checking ----------------------------- */
  CVarSetEnums CvarCheckAndResetValue(const CVarFlagsConst &cvfcFlags,
    const CVarConditionFlagsConst ccfcFlags, StdString &strCBError)
  { return CVarCheckAndSetValue(strDefValue,
      cvfcFlags, ccfcFlags, strCBError); }
  /* -- Move constructor --------------------------------------------------- */
  CVarItem(CVarItem &&ciOther) :       // Other item
    /* -- Initialisers ----------------------------------------------------- */
    CVarFlags{ ciOther },                                        // Copy flags
    strVar{ StdMove(ciOther.CVarGetModifyableVar()) },           // Move var
    strValue{ StdMove(ciOther.CVarGetModifyableValue()) },       // Move value
    strDefValue{ StdMove(ciOther.CVarGetModifyableDefValue()) }, // Move def
    cfTrigger{ ciOther.CVarGetTrigger() }                        // Copy trig
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor to initialise a new item ------------------------------- */
  template<class StrType>
    requires StdIsString<StrType>
  CVarItem(
    /* -- Required parameters ---------------------------------------------- */
    const StrType &strKey,             // Variable name
    const StrType &strVal,             // Value name
    const CbFunc &cfCb,                // Trigger function
    const CVarFlagsConst &cvfcF) :     // CVar flags
    /* -- Initialisers ----------------------------------------------------- */
    CVarFlags{ cvfcF },                // Initialise cvars flags
    strVar{ strKey },                  // Initialise variable name
    strValue{ strVal },                // Initialise new value
    strDefValue{ strVal },             // Initialise default value
    cfTrigger{ cfCb }                  // Initialise trigger function
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
