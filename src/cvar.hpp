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
using namespace IFlags;                using namespace ILog::P;
using namespace IMemory::P;            using namespace ISql::P;
using namespace IStd::P;               using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(CVarShow,
  /* ----------------------------------------------------------------------- */
  // Show no confidential cvars        Show private cvars
  CSF_NONE                  {Flag(0)}, CSF_CONFIDENTIAL              {Flag(1)},
  // Show protected cvars
  CSF_PROTECTED             {Flag(2)},
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
static CVarReturn NoOp(CVarItem&, const string&) { return ACCEPT; }
/* ------------------------------------------------------------------------- */
enum CVarSetEnums : unsigned int       // Cvar set return codes
{ /* ----------------------------------------------------------------------- */
  CVS_OK,                              // Parameter set successful
  CVS_OKNOTCHANGED,                    // Parameter set successful, no change
  CVS_NOTFOUND,                        // Parameter not found
  CVS_NOTWRITABLE,                     // Parameter set is not writable
  CVS_NOTINTEGER,                      // Parameter set was not an integer
  CVS_NOTFLOAT,                        // Parameter set was not a float
  CVS_NOTBOOLEAN,                      // Parameter set was not a boolean
  CVS_NOTUNSIGNED,                     // Parameter set was not >= 0
  CVS_NOTPOW2,                         // Parameter set was not power of two
  CVS_NOTALPHA,                        // Must contain only letters
  CVS_NOTNUMERIC,                      // Must contain only digits
  CVS_NOTALPHANUMERIC,                 // Must contain only alphanumerics
  CVS_NOTFILENAME,                     // Must be a valid filename
  CVS_TRIGGERDENIED,                   // Trigger callback denied change
  CVS_TRIGGEREXCEPTION,                // exception occured in trigger
  CVS_EMPTY,                           // Parameter set was an empty string
  CVS_ZERO,                            // Parameter not allowed to be zero
  CVS_NOTYPESET,                       // No type is set
};/* ----------------------------------------------------------------------- */
BUILD_FLAGS(CVarCondition,             // For Set() functions
  /* ----------------------------------------------------------------------- */
  // No flags specified?               Do not mark as commit?
  CCF_NOTHING               {Flag(0)}, CCF_NOMARKCOMMIT          {Flag(1)},
  // Don't throw if var missing?       Throw if there is an error?
  CCF_IGNOREIFMISSING       {Flag(2)}, CCF_THROWONERROR          {Flag(3)},
  // Variable was just registered?     The variable was decrypted?
  CCF_NEWCVAR               {Flag(4)}, CCF_DECRYPTED             {Flag(5)},
  // The variable was not encrypted?
  CCF_NOTDECRYPTED          {Flag(6)}
);/* -- Cvar item class ---------------------------------------------------- */
class CVarItem :                       // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public CVarFlags                     // Cvar configuration settings
{ /* -- Private typedefs --------------------------------------------------- */
  typedef int64_t ValueIntType;        // When handling integral values
  /* ----------------------------------------------------------------------- */
  enum CommitResult : unsigned int     // Result to a commit request
  { /* --------------------------------------------------------------------- */
    CR_OK,                             // Sql call commited the variable
    CR_FAIL,                           // Sql call update cvar failed
    CR_OK_PURGE,                       // Sql call succeeded with purge
    CR_OK_NOTHING_TO_DO,               // Sql call succeeded with nothing to do
    CR_FAIL_PURGE,                     // Sql call failed purge
    CR_FAIL_PURGE_NOT_CHANGED,         // Sql call succeded but no changes
    CR_FAIL_PURGE_UNKNOWN_ERROR,       // Unknown result from sql purge call
    CR_FAIL_NOT_SAVEABLE,              // CVar is not saveable
    CR_FAIL_LOADED_NOT_MODIFIED,       // Cvar loaded but not modified
    CR_FAIL_ENCRYPT,                   // Cvar could not be encrypted
    CR_FAIL_COMPRESS,                  // Cvar could not be compress
  };/* -- Private variables ------------------------------------------------ */
  const string   strVar;               // Variable name
  string         strValue,             // Value name
                 strDefValue;          // Default value name
  CbFunc         cfTrigger;            // Callback trigger event
  /* --------------------------------------------------------------- */ public:
  const CbFunc &GetTrigger(void) const { return cfTrigger; }
  /* ----------------------------------------------------------------------- */
  void SetTrigger(const CbFunc &cfCb) { cfTrigger = cfCb; }
  /* ----------------------------------------------------------------------- */
  bool IsTriggerSet(void) const { return GetTrigger() == NoOp; }
  /* ----------------------------------------------------------------------- */
  const string &GetVar(void) const { return strVar; }
  /* ----------------------------------------------------------------------- */
  const string &GetDefValue(void) const { return strDefValue; }
  /* ----------------------------------------------------------------------- */
  size_t GetDefLength(void) const { return GetDefValue().length(); }
  /* ----------------------------------------------------------------------- */
  size_t GetDefCapacity(void) const { return GetDefValue().capacity(); }
  /* ----------------------------------------------------------------------- */
  string &GetModifyableValue(void) { return strValue; }
  /* ----------------------------------------------------------------------- */
  bool IsLowerPriority(const CVarFlagsConst cvfcFlags) const
    { return (FlagGet() & SANY) < (cvfcFlags & SANY); }
  /* ----------------------------------------------------------------------- */
  const string &GetValue(void) const { return strValue; }
  /* ----------------------------------------------------------------------- */
  size_t GetValueLength(void) const { return GetValue().length(); }
  /* ----------------------------------------------------------------------- */
  size_t GetValueCapacity(void) const { return GetValue().capacity(); }
  /* ----------------------------------------------------------------------- */
  void PruneValue(void) { GetModifyableValue().shrink_to_fit(); }
  /* ----------------------------------------------------------------------- */
  bool IsValueUnset(void) const { return GetValue().empty(); }
  /* ----------------------------------------------------------------------- */
  bool IsValueSet(void) const { return !IsValueUnset(); }
  /* ----------------------------------------------------------------------- */
  bool IsValueChanged(void) const { return GetValue() != GetDefValue(); }
  /* ----------------------------------------------------------------------- */
  bool IsValueUnchanged(void) const { return !IsValueChanged(); }
  /* ----------------------------------------------------------------------- */
  void SetDefault(void) { strValue = GetDefValue(); }
  /* ----------------------------------------------------------------------- */
  void SetDefValue(const string &strN) { strDefValue = strN; }
  /* ----------------------------------------------------------------------- */
  void SetDefValue(string &&strN) { strDefValue = StdMove(strN); }
  /* ----------------------------------------------------------------------- */
  void SetValue(const string &strV) { strValue = strV; }
  /* ----------------------------------------------------------------------- */
  void SetValue(string &&strV) { strValue = StdMove(strV); }
  /* ----------------------------------------------------------------------- */
  const string GetValueSafe(void) const
  { // If confidential, return confidential
    if(FlagIsSet(CONFIDENTIAL) && csfShowFlags.FlagIsClear(CSF_CONFIDENTIAL))
      return cCommon->CommonPrivate();
    // If protected, return protected
    if(FlagIsSet(CPROTECTED) && csfShowFlags.FlagIsClear(CSF_PROTECTED))
      return cCommon->CommonProtected();
    // If value is empty, return as empty
    if(IsValueUnset()) return cCommon->CommonEmpty();
    // If is a float, return value
    if(FlagIsSet(TFLOAT))
      return StrFromNum(StrToNum<double>(GetValue()), 0, 12);
    // Is a boolean
    if(FlagIsSet(TBOOLEAN))
      return StrFromNum(StrFromBoolTF(StrToNum<bool>(GetValue())));
    // If is a integer, return number + hex
    if(FlagIsSet(TINTEGER))
    { // Get value as 64-bit integer
      const ValueIntType vitV = StrToNum<ValueIntType>(GetValue());
      return StrFormat("$ [0x$$]", vitV, hex, vitV);
    } // Unknown value or string. Return as-is.
    return StrFormat("\"$\"", GetValue());
  }
  /* ----------------------------------------------------------------------- */
  bool MarkEncodedVarAsCommit(void)
  { // Ignore if appropriate flag not set
    if(FlagIsClear(CPROTECTED|CDEFLATE)) return false;
    // Mark as commit
    FlagSet(COMMIT);
    // Commit the cvar and increment count if succeeded. 'this->'
    // needed to stop clang whining about 'this' not being used but it
    // is needed!
    return Commit() == CR_OK;
  }
  /* ----------------------------------------------------------------------- */
  bool Commit(const MemConst &mcSrc)
    { return cSql->CVarCommitBlob(GetVar(), mcSrc); }
  /* ----------------------------------------------------------------------- */
  CommitResult Commit(void)
  { // Ignore if variable not modified, force saved or loaded
    if(FlagIsClear(COMMIT|COMMITNOCHECK)) return CR_OK_NOTHING_TO_DO;
    // If the value is the same as default value and nocheck not set?
    if(IsValueUnchanged() && FlagIsClear(COMMITNOCHECK))
    { // Nothing to do if the flag wasn't loaded from the database
      if(FlagIsClear(LOADED)) return CR_OK_NOTHING_TO_DO;
      // Purge the cvar from the database
      switch(cSql->CVarPurge(GetVar()))
      { case Sql::PR_OK    : return CR_OK_PURGE;
        case Sql::PR_FAIL  : return CR_FAIL_PURGE;
        case Sql::PR_OK_NC : return CR_FAIL_PURGE_NOT_CHANGED;
        default            : return CR_FAIL_PURGE_UNKNOWN_ERROR;
      }
    } // If we are to encrypt?
    if(FlagIsSet(CPROTECTED)) try
    { // Try encryption and/or compression and store result in database
      if(FlagIsSet(CDEFLATE)) Commit(Block<AESZLIBEncoder>(GetValue()));
      else Commit(Block<AESEncoder>(GetValue()));
    } // exception occured?
    catch(const exception &eReason)
    { // Log exception
      cLog->LogErrorExSafe("CVars encrypt exception: $", eReason);
      // Capture exceptions again, try raw encoder and return string
      try { Commit(Block<RAWEncoder>(GetValue())); }
      // exception occured again?
      catch(const exception &eReason2)
      { // Log exception and return failure
        cLog->LogErrorExSafe("CVars store exception: $", eReason2);
        return CR_FAIL_ENCRYPT;
      } // Success
    } // If we are to compress?
    else if(FlagIsSet(CDEFLATE)) try
    { // Try compression and commit the result to the database
      Commit(Block<ZLIBEncoder>(GetValue()));
    } // exception occured?
    catch(const exception &eReason)
    { // Log exception
      cLog->LogErrorExSafe("CVars compress exception: $", eReason);
      // Capture exceptions again, try raw encoder and return string
      try { Commit(Block<RAWEncoder>(GetValue())); }
      // exception occured again?
      catch(const exception &eReason2)
      { // Log exception and return failure
        cLog->LogErrorExSafe("CVars store exception: $", eReason2);
        return CR_FAIL_COMPRESS;
      } // Success
    } // Commit the unencrypted cvar and return if failed
    else if(!cSql->CVarCommitString(GetVar(), GetValue())) return CR_FAIL;
    // Successfully saved so remove commit flag
    FlagClear(COMMIT);
    // Success
    return CR_OK;
  }
  /* -- Save with counter increment ---------------------------------------- */
  template<typename IntType>void Save(IntType &itCommit, IntType &itPurge)
  { // Try to commit the cvar. 'this->' needed to stop clang whining about
    // 'this' not being used but it is needed!
    switch(Commit())
    { // enum CommitResult
      case CR_OK       : itCommit = itCommit + 1; break; // Saved
      case CR_OK_PURGE : itPurge = itPurge + 1; break;   // Purged
      default          : break;                          // Error
    }
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums HandleCallbackException(const exception &eReason,
    const CVarConditionFlagsConst &ccfcFlags, const string &strNValue,
    string &strCBError)
  { // Throw exception if requested
    if(ccfcFlags.FlagIsSet(CCF_THROWONERROR))
      XC(eReason, "Variable", GetVar(), "Value", strNValue);
    // Save the error string because we have no access to the console
    strCBError = StrAppend("CVar CB failed! > ", eReason);
    // Return error
    return CVS_TRIGGEREXCEPTION;
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums SetValue(const string &strNValue,
    const CVarConditionFlagsConst &ccfcFlags, string &strCBError)
  { // If value is equal as current value then ignore it. We'll allow the
    // change when setting new vars because the triggers should trigger
    if(strNValue == GetValue() && ccfcFlags.FlagIsClear(CCF_NEWCVAR))
      return CVS_OKNOTCHANGED;
    // Result storage
    CVarReturn cbrResult;
    // If it is a lua cvar?
    if(FlagIsSet(TLUA))
    { // Lock the cvar from being unregistered
      FlagSet(LOCKED);
      // Call the trigger and capture the result of the callback
      try { cbrResult = GetTrigger()(*this, strNValue); }
      // exception occured?
      catch(const exception &eReason)
      { // Remove the lock on the cvar
        FlagClear(LOCKED);
        // Handle exception and return error if no exception
        return HandleCallbackException(eReason, ccfcFlags, strNValue,
          strCBError);
      } // Remove the lock on the cvar
      FlagClear(LOCKED);
    } // Not a lua cvar? Call the trigger and capture the result of the cb
    else try { cbrResult = GetTrigger()(*this, strNValue); }
    // exception occured
    catch(const exception &eReason)
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
             "Variable", GetVar(), "Value", strNValue);
        return CVS_TRIGGERDENIED;
        // The new value is acceptable? Set the new value
      case ACCEPT: SetValue(strNValue); break;
      // The new value is acceptable, but the caller set the value? Ignore
      case ACCEPT_HANDLED: break;
      // Same as above but forcing commit? Set commit flag as requested
      case ACCEPT_HANDLED_FORCECOMMIT: FlagSet(COMMIT|COMMITNOCHECK); break;
      // Unknown return value?
      default:
        // Throw exception if requested otherwise return trigger denied code
        if(ccfcFlags.FlagIsSet(CCF_THROWONERROR))
          XC("CVar callback returned unknown value!",
             "Variable", GetVar(), "Value", strNValue, "Result", cbrResult);
        return CVS_TRIGGEREXCEPTION;
    } // Free unused memory from cvar
    PruneValue();
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
    } // Log progress and return success
    cLog->LogDebugExSafe("CVars $ '$' to $.",
      ccfcFlags.FlagIsSet(CCF_NEWCVAR) ? "registered" : "set",
      GetVar(), GetValueSafe());
    // Return success
    return CVS_OK;
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums SetValue(const string &strNValue,
    const CVarFlagsConst &cvfcFlags, const CVarConditionFlagsConst &ccfcFlags,
    string &strCBError)
  { // Failed if not writable
    if(FlagIsClear(cvfcFlags))
    { // If we should not abort? Just return error else throw exception
      if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTWRITABLE;
      XC("CVar is not writable from this scope!",
         "Variable", GetVar(),            "Value", strNValue,
         "Scope",    cvfcFlags.FlagGet(), "Flags", FlagGet());
    } // If integer?
    if(FlagIsSet(TINTEGER))
    { // If number begins with '0x' to denote hexadecimal? Convert specified
      // value from hexadecimal to decimal and restart the call with the
      // converted value.
      if(strNValue.length() > 2 && strNValue[0] == '0' && strNValue[1] == 'x')
        return SetValue(StrFromNum(StrHexToInt<ValueIntType>
          (strNValue.substr(2))), ccfcFlags, strCBError);
      // If specified value is not a valid integer?
      if(!StrIsInt(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTINTEGER;
        XC("CVar specified is not a valid integer!",
           "Variable", GetVar(), "Value", strNValue);
      } // Deny negative values if unsigned only needed
      if(FlagIsSet(CUNSIGNED) && strNValue.front() == '-')
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTUNSIGNED;
        XC("CVar specified must be an unsigned integer!",
           "Variable", GetVar(), "Value", strNValue);
      } // Deny non-power of two numbers but allow zero?
      if(FlagIsSet(CNOTEMPTY) && !StrToNum<uint64_t>(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_ZERO;
        XC("CVar specified must be non-zero!",
           "Variable", GetVar(), "Value", strNValue);
      } // Deny non-power of two numbers?
      if(FlagIsSet(CPOW2) && !StrIsNumPOW2(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTPOW2;
        XC("CVar specified must be power of two!",
           "Variable", GetVar(), "Value", strNValue);
      } // Next step
      return SetValue(strNValue, ccfcFlags, strCBError);
    } // If float? Bail if not a floating point number
    if(FlagIsSet(TFLOAT))
    { // If specified value is not a valid floating point number?
      if(!StrIsFloat(strNValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
          return CVS_NOTFLOAT;
        XC("CVar specified is not a valid number!",
           "Variable", GetVar(), "Value", strNValue);
      } // Deny negative values if unsigned only needed
      if(FlagIsSet(CUNSIGNED) && strNValue.front() == '-')
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
          return CVS_NOTUNSIGNED;
        XC("CVar specified must be a non-negative float!",
           "Variable", GetVar(), "Value", strNValue);
      } // Next step
      return SetValue(strNValue, ccfcFlags, strCBError);
    } // Is a boolean?
    if(FlagIsSet(TBOOLEAN))
    { // Must be one byte, then test first character
      if(strNValue.size() == 1) switch(strNValue.front())
      { // Is zero or one? OK
        case '0': case '1':
          return SetValue(strNValue, ccfcFlags, strCBError);
        // Invalid value
        default: break;
      } // True?
      else if(strNValue.size() == 4 &&
        StrToLowCase(strNValue) == cCommon->CommonTrue())
          return SetValue(cCommon->CommonOne(), ccfcFlags, strCBError);
      // False?
      else if(strNValue.size() == 5 &&
        StrToLowCase(strNValue) == cCommon->CommonFalse())
          return SetValue(cCommon->CommonZero(), ccfcFlags, strCBError);
      // If we should not abort? Just return error else throw exception
      if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
        return CVS_NOTBOOLEAN;
      XC("CVar specified is not a valid boolean!",
         "Variable", GetVar(), "Value", strNValue);
    } // Is a string?
    if(FlagIsSet(TSTRING))
    { // Trim string if setting requests it and get new result
      const string &strNewValue =
        FlagIsSet(MTRIM) ? StrTrim(strNValue, ' ') : strNValue;
      // String cannot be empty and string is empty?
      if(FlagIsSet(CNOTEMPTY) && strNewValue.empty())
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
          return CVS_EMPTY;
        XC("CVar specified cannot be empty!", "Variable", GetVar());
      } // Check if valid untrusted pathname required
      if(FlagIsSet(CFILENAME))
      { // Check filename and get result
        switch(const ValidResult vrRes = DirValidName(strNewValue))
        { // Break if ok or empty
          case VR_OK: case VR_EMPTY: break;
          // Show error otherwise
          default: if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
                     return CVS_NOTFILENAME;
                   XC("CVar untrusted path name is invalid!",
                      "Reason",   cDirBase->DirBaseVNRtoStr(vrRes),
                      "Result",   vrRes,
                      "Variable", GetVar());
        }
      } // Check if valid trusted pathname required
      if(FlagIsSet(CTRUSTEDFN))
      { // Check filename and get result
        switch(const ValidResult vrRes = DirValidName(strNewValue, VT_TRUSTED))
        { // Break if ok or empty
          case VR_OK: case VR_EMPTY: break;
          // Show error otherwise
          default : if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
                      return CVS_NOTFILENAME;
                    XC("CVar trusted path name is invalid!",
                       "Reason",   cDirBase->DirBaseVNRtoStr(vrRes),
                       "Result",   vrRes,
                       "Variable", GetVar(),
                       "Path",     strNewValue);
        }
      } // Alpha characters only?
      if(FlagIsSet(CALPHA))
      { // And numeric characters?
        if(FlagIsSet(CNUMERIC) && !StrIsAlphaNum(strNewValue))
        { // If we should not abort? Just return error else throw exception
          if(ccfcFlags.FlagIsClear(CCF_THROWONERROR))
            return CVS_NOTALPHANUMERIC;
          XC("CVar specified must only contain alphanumeric characters!",
             "Variable", GetVar());
        } // Only letters?
        if(!StrIsAlpha(strNewValue))
        { // If we should not abort? Just return error else throw exception
          if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTALPHA;
          XC("CVar specified must only contain letters!",
             "Variable", GetVar());
        }
      } // Must only contain numbers
      else if(FlagIsSet(CNUMERIC) && !StrIsInt(strNewValue))
      { // If we should not abort? Just return error else throw exception
        if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTNUMERIC;
        XC("CVar specified must only contain numeric characters!",
           "Variable", GetVar());
      } // Next step
      return SetValue(strNewValue, ccfcFlags, strCBError);
    } // If we should not throw error? Just return code else throw exception
    if(ccfcFlags.FlagIsClear(CCF_THROWONERROR)) return CVS_NOTYPESET;
    XC("CVar type is not set!",
       "Variable", GetVar(),   "NewValue", strNValue,
       "OldValue", GetValue(), "Flags",    FlagGet());
  }
  /* ----------------------------------------------------------------------- */
  CVarSetEnums ResetValue(const CVarFlagsConst &cvfcFlags,
    const CVarConditionFlagsConst &ccfcFlags, string &strCBError)
      { return SetValue(strDefValue, cvfcFlags, ccfcFlags, strCBError); }
  /* -- Move constructor --------------------------------------------------- */
  CVarItem(CVarItem &&ciOther) :       // Other item
    /* -- Initialisers ----------------------------------------------------- */
    CVarFlags{ ciOther },              // Copy flags over
    strVar{                            // Variable
      StdMove(ciOther.GetVar()) },     // Move variable
    strValue{                          // Value
      StdMove(ciOther.GetValue()) },   // Move value
    strDefValue{                       // Default value
      StdMove(ciOther.GetDefValue()) },// Move default value
    cfTrigger{                         // Trigger
      StdMove(ciOther.GetTrigger()) }  // Move trigger
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor -------------------------------------------------------- */
  CVarItem(
    /* -- Required parameters ---------------------------------------------- */
    const string &strKey,              // Variable name
    const string &strVal,              // Value name
    const CbFunc &cfCb,                // Trigger function
    const CVarFlagsConst &cvfcF) :     // CVar flags
    /* -- Initialisers ----------------------------------------------------- */
    CVarFlags{ cvfcF },                // Initialise cvars flags
    strVar{ strKey },                  // Initialise variable name
    strValue{ strVal },                // Initialise new value
    strDefValue{ strVal },             // Initialise default value
    cfTrigger{ cfCb }                  // Initialise trigger function
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
