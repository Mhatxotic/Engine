/* == LUAVAR.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles Lua CVars.                                                  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaVariable {               // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace ICommon::P;
using namespace IConsole::P;           using namespace ICVarDef::P;
using namespace ICVar::P;              using namespace ICVarLib::P;
using namespace IError::P;             using namespace ILockable::P;
using namespace ILog::P;               using namespace ILookupMap::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace ILuaFunc::P;
using namespace ISerial::P;            using namespace IString::P;
using namespace IStat::P;              using namespace IStd::P;
/* ------------------------------------------------------------------------- */
using LumCvEnums = LookupMap<CVarFlagsType>;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Lua cvar list types -------------------------------------------------- */
using LuaCVarPair = StdPair<LuaFunc, CVarMapIt>;
MAPPACK_BUILD(LuaCVar, const StdString, LuaCVarPair)
/* -- Variables ollector class for collector data and custom variables ----- */
CTOR_BEGIN(Variables, Variable, CLHelperSafe,
  /* ----------------------------------------------------------------------- */
  LuaCVarMap       lcvmMap;            // Lua cvar list
  /* -- Cvar flag type strings --------------------------------------------- */
  const LumCvEnums lceTypes;           // Types
  const LumCvEnums lceConditions;      // Conditional flags
  const LumCvEnums lcePermissions;     // Permission flags
  const LumCvEnums lceSources;         // Load reason flags
  const LumCvEnums lceOther;,          // Misc flags
);/* -- Lua variables collector and member class --------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Variables, Variable, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable                      // Lua garbage collector instruction
{ /* -- Private variables -------------------------------------------------- */
  LuaCVarMapIt     lcvmiIt;            // Iterator to command Console gives us
  /* -- Returns the lua console command list ------------------------------- */
  static LuaCVarMap &GetLuaVarList() { return cVariables->lcvmMap; }
  /* -- Returns the end of the lua console command list -------------------- */
  static LuaCVarMapIt GetLuaVarListEnd() { return GetLuaVarList().end(); }
  /* == Cvar updated callback for Lua ============================== */ public:
  static CVarReturn LuaCallbackStatic(CVarItem &cviVar,
    const StdString &strVal)
  { // Find cvar and ignore if we don't have it yet! This can happen if the
    // variable is initialising for the first time. We haven't added the
    // variable to cvmActive yet and we don't want to until the CVARS system
    // has created the variable.
    const LuaCVarMapIt lcvmpIt{ cVariables->lcvmMap.find(cviVar.GetVar()) };
    if(lcvmpIt == cVariables->lcvmMap.cend()) return ACCEPT;
    // Save stack position and restore it on scope exit
    const LuaStackSaver lSS{ cLuaFuncs->LuaRefGetState() };
    // Call the Lua callback assigned. We're expecting one or two return values
    lcvmpIt->second.first.LuaFuncProtectedDispatch(2, strVal, cviVar.GetVar());
    // Get result of the callback which means a boolean HAS to be returned
    const bool bResult = LuaUtilGetBool(cLuaFuncs->LuaRefGetState(), -2);
    // Theres also an optional second string parameter. Return standard result
    // of 'ACCEPT' if true or 'DENY' if false.
    if(!LuaUtilIsString(cLuaFuncs->LuaRefGetState(), -1))
      return BoolToCVarReturn(bResult);
    // Replace the current value with the guest author specified value.
    cviVar.GetModifyableValue() =
      LuaUtilToCppString(cLuaFuncs->LuaRefGetState(), -1);
    // Return result of how to handle the returned string
    return bResult ? ACCEPT_HANDLED_FORCECOMMIT : ACCEPT_HANDLED;
  }
  /* -- Unregister the console command from lua -------------------- */ public:
  const StdString &Name() const { return lcvmiIt->first; }
  /* -- Get current value as string ---------------------------------------- */
  StdString Get() const { return cCVars->GetStr(lcvmiIt->second.second); }
  /* -- Get default value as string ---------------------------------------- */
  StdString Default() const
    { return cCVars->GetDefStr(lcvmiIt->second.second); }
  /* -- Reset default value ------------------------------------------------ */
  void Reset() const { cCVars->Reset(lcvmiIt->second.second); }
  /* -- Returns if value is empty ------------------------------------------ */
  bool Empty() const { return Get().empty(); }
  bool NotEmpty() const { return !Empty(); }
  /* -- Set value from different types ------------------------------------- */
  CVarSetEnums SetString(const StdStringView &strvValue) const
    { return cCVars->Set(lcvmiIt->second.second, strvValue); }
  CVarSetEnums Clear() const
    { return SetString(cCommon->CommonBlank()); }
  CVarSetEnums SetBoolean(const bool bState) const
    { return SetString(bState ?
        cCommon->CommonOneV() : cCommon->CommonZeroV()); }
  CVarSetEnums SetInteger(const lua_Integer liValue) const
    { return SetString(StrFromNum(liValue)); }
  CVarSetEnums SetNumber(const lua_Number lnValue) const
    { return SetString(StrFromNum(lnValue, 0, 15)); }
  /* -- Register user console command from lua ----------------------------- */
  void Init(lua_State*const lS, const StdStringView &strvName,
    const StdStringView &strvDefault, const CVarFlagsConst cvfcFlags)
  { // Check that the variable name is valid
    if(!cCVars->IsValidVariableName(strvName))
      XC("CVar name is not valid!",
        "Variable", strvName, "Minimum", cCVars->stCVarMinLength,
        "Maximum",  cCVars->stCVarMaxLength);
    // Make sure cvar doesn't already exist
    if(cCVars->VarExists(strvName))
      XC("CVar already registered!", "Variable", strvName);
    // Get all the flags in the types mask
    switch(cvfcFlags.FlagAnd(TMASK))
    { // Only types specified on their own are valid
      case TSTRING: case TINTEGER: case TFLOAT: case TBOOLEAN: break;
      // Anything else?
      default: XC("CVar flags have none or mixed types!",
        "Variable", strvName, "Flags", cvfcFlags.FlagGet());
    }
    // Since the userdata for this class object is at arg 5, we need to make
    // sure the callback function is ahead of it in arg 6 or the LuaFunc()
    // class which calls luaL_ref will fail as it ONLY reads position -1.
    LuaUtilCopyValue(lS, 4);
    // Save the function at the top of the stack used for the callback
    lcvmiIt = cVariables->lcvmMap.insert(GetLuaVarListEnd(), {
      StdString{ strvName },
      make_pair(LuaFunc{ StrAppend("CV:", strvName), true },
        cCVars->GetVarListEnd())
    }); // Register the variable and set the iterator to the new cvar.
    lcvmiIt->second.second = cCVars->RegisterVar(strvName, strvDefault,
      LuaCallbackStatic, cvfcFlags|TLUA|PANY);
  }
  /* -- Register existing internal engine variable as a Lua variable ------- */
  void InitInternal(const CVarMapIt &cvmiIt)
  { // Get cvar name
    const StdString &strvName = cvmiIt->first;
    // Insert a new variable
    lcvmiIt = cVariables->lcvmMap.insert(GetLuaVarListEnd(), { strvName,
      make_pair(LuaFunc{ strvName, false }, cCVars->GetVarListEnd()) });
    // Register the variable and set the iterator to the new cvar.
    lcvmiIt->second.second = cvmiIt;
  }
  /* -- Basic constructor with no init ------------------------------------- */
  Variable() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperVariable{                  // Initialise and register the object
      cVariables, this },
    SerialSlave{ cParent->Serial() },  // Initialise identification number
    lcvmiIt{ GetLuaVarListEnd() }      // Initialise iterator to the last
    /* --------------------------------------------------------------------- */
    {}
  /* -- Destructor that unregisters the cvar ------------------------------- */
  DTORHELPER(~Variable,
    // Return if the iterator is invalid?
    if(lcvmiIt == GetLuaVarListEnd()) return;
    // Unregister the cvar if valid and registered by Lua
    const CVarMapIt &cvmiIt = lcvmiIt->second.second;
    if(cvmiIt != cCVars->GetVarListEnd() && cvmiIt->second.FlagIsSet(TLUA))
      cCVars->UnregisterVar(cvmiIt);
    // Remove the lua var
    GetLuaVarList().erase(lcvmiIt);
  )
};/* ----------------------------------------------------------------------- */
CTOR_END(Variables, Variable, VARIABLE,,,, // Finish off collector class
/* ------------------------------------------------------------------------- */
lceTypes{{                             // Cvar types
  IDMAPSTR(TSTRING),                   IDMAPSTR(TINTEGER),
  IDMAPSTR(TFLOAT),                    IDMAPSTR(TBOOLEAN),
  IDMAPSTR(TLUA),
}, "NONE" },
/* ------------------------------------------------------------------------- */
lceConditions{{                        // Conditional flags
  IDMAPSTR(CALPHA),                    IDMAPSTR(CNUMERIC),
  IDMAPSTR(CSAVEABLE),                 IDMAPSTR(CPROTECTED),
  IDMAPSTR(CDEFLATE),                  IDMAPSTR(CNOTEMPTY),
  IDMAPSTR(CUNSIGNED),                 IDMAPSTR(CPOW2),
  IDMAPSTR(CFILENAME),                 IDMAPSTR(CTRUSTEDFN),
}, "NONE" },
/* ------------------------------------------------------------------------- */
lcePermissions{{                       // Permission flags
  IDMAPSTR(PCMDLINE),                  IDMAPSTR(PAPPCFG),
  IDMAPSTR(PUDB),                      IDMAPSTR(PCONSOLE),
}, "NONE" },
/* ------------------------------------------------------------------------- */
lceSources{{                           // Load sources
  IDMAPSTR(SCMDLINE),                  IDMAPSTR(SAPPCFG),
  IDMAPSTR(SUDB),
}, "NONE" },
/* ------------------------------------------------------------------------- */
lceOther{{                             // Misc flags
  IDMAPSTR(MTRIM),                     IDMAPSTR(LOCKED),
  IDMAPSTR(COMMIT),                    IDMAPSTR(COMMITNOCHECK),
  IDMAPSTR(PURGE),                     IDMAPSTR(CONFIDENTIAL),
  IDMAPSTR(LOADED),
}, "NONE" }
);/* -- Return human readable string about CVar ---------------------------- */
static StdString VariablesMakeInformation(const CVarItem &cviVar)
{ // Print data about the cvar
  return StrFormat("Status for '$'...\n"
    "- Callback: $.\n"               "- Flags: 0x$$$.\n"
    "- Types: $.\n"                  "- Conditions: $.\n"
    "- Permissions: $.\n"            "- Source: $.\n"
    "- Other: $.\n"                  "- Default: [$/$] \"$\".\n"
    "- Modified: $.\n"               "- Current: [$/$] \"$\".",
      cviVar.GetVar(),
      StrFromBoolTF(cviVar.IsTriggerSet()),
      StdIOSHex, cviVar, StdIOSDec,
      StrImplode(cVariables->lceTypes.Test(cviVar), ", "),
      StrImplode(cVariables->lceConditions.Test(cviVar), ", "),
      StrImplode(cVariables->lcePermissions.Test(cviVar), ", "),
      StrImplode(cVariables->lceSources.Test(cviVar), ", "),
      StrImplode(cVariables->lceOther.Test(cviVar), ", "),
      cviVar.GetDefLength(), cviVar.GetDefCapacity(), cviVar.GetDefValue(),
      StrFromBoolTF(cviVar.IsValueChanged()),
      cviVar.GetValueLength(), cviVar.GetValueCapacity(), cviVar.GetValue());
}
/* -- Return human readable tokenised string about CVar -------------==----- */
static void VariablesMakeInformationTokens(Statistic &sTable,
  const CVarItem &cviVar)
{ // Compare flags and return a character for each flag
  sTable.Data(StrFromEvalTokens({
    // Types
    { true, cviVar.FlagIsSet(TBOOLEAN)   ? 'B' :
           (cviVar.FlagIsSet(CFILENAME)  ? 'F' :
           (cviVar.FlagIsSet(TINTEGER)   ? 'I' :
           (cviVar.FlagIsSet(TFLOAT)     ? 'N' :
           (cviVar.FlagIsSet(TSTRING)    ? 'S' :
           (cviVar.FlagIsSet(CTRUSTEDFN) ? 'T' :
                                           '?'))))) },
    // Permissions
    { cviVar.FlagIsSet(PCMDLINE),      '1' },
    { cviVar.FlagIsSet(PAPPCFG),       '2' },
    { cviVar.FlagIsSet(PUDB),          '3' },
    { cviVar.FlagIsSet(PCONSOLE),      '4' },
    // Sources
    { cviVar.FlagIsSet(SCMDLINE),      '6' },
    { cviVar.FlagIsSet(SAPPCFG),       '7' },
    { cviVar.FlagIsSet(SUDB) ,         '8' },
    // Conditions and operations
    { cviVar.FlagIsSet(CONFIDENTIAL),  'C' },
    { cviVar.FlagIsSet(CDEFLATE),      'D' },
    { cviVar.FlagIsSet(COMMITNOCHECK), 'H' },
    { cviVar.IsTriggerSet(),           'K' },
    { cviVar.FlagIsSet(TLUA),          'L' },
    { cviVar.FlagIsSet(COMMIT),        'M' },
    { cviVar.FlagIsSet(LOADED),        'O' },
    { cviVar.FlagIsSet(CPROTECTED),    'P' },
    { cviVar.FlagIsSet(MTRIM),         'R' },
    { cviVar.FlagIsSet(CUNSIGNED),     'U' },
    { cviVar.FlagIsSet(CSAVEABLE),     'V' },
    { cviVar.FlagIsSet(CPOW2),         'W' },
    { cviVar.FlagIsSet(CNOTEMPTY),     'Y' }
  // Name and value
  })).Data(cviVar.GetVar()).Data(cviVar.GetValueSafe());
}
/* -- Enumerate a list ----------------------------------------------------- */
template<class MapType>
  static StdString VariablesMakeList(const MapType &mtMap,
    const StdString &strFilter)
{ // Get pending cvars list and ignore if empty
  if(mtMap.empty()) return "No cvars exist in this category!";
  // Try to find the cvar outright first (only make work when not in release)
  using MapTypeConstIt = MapType::const_iterator;
  const MapTypeConstIt mtciExactIt{ mtMap.find(strFilter) };
  if(mtciExactIt != mtMap.cend())
  { // Type could either be CVarMap?
    if constexpr(StdIsSame<MapType, CVarMap>)
      return VariablesMakeInformation(mtciExactIt->second);
    // ..or the type could either be LuaCVarMap
    else if constexpr(StdIsSame<MapType, LuaCVarMap>)
      return VariablesMakeInformation(mtciExactIt->second.second->second);
  } // Try as a lower bound (partial) check?
  MapTypeConstIt mtciIt{ mtMap.lower_bound(strFilter) };
  if(mtciIt != mtMap.cend())
  { // Formatted output. Can assume all variables will be printed
    Statistic sTable;
    sTable.Header("FLAGS").Header("NAME", false).Header("VALUE", false)
          .Reserve(mtMap.size());
    // Number of variables matched and tokens mask
    size_t stMatched = 0;
    // Build output string
    do
    { // If no match found? return original string
      const StdString &strKey = mtciIt->first;
      if(strKey.compare(0, strFilter.size(), strFilter)) continue;
      // Increment matched counter
      ++stMatched;
      // Type could either be CVarMap?
      if constexpr(StdIsSame<MapType, CVarMap>)
        VariablesMakeInformationTokens(sTable, mtciIt->second);
      // ..or the type could either be LuaCVarMap
      else if constexpr(StdIsSame<MapType, LuaCVarMap>)
        VariablesMakeInformationTokens(sTable, mtciIt->second.second->second);
    } // Until no more commands
    while(++mtciIt != mtMap.cend());
    // Print output if we matched commands
    if(stMatched) return StrFormat("$$ of $ matched.", sTable.Finish(),
        stMatched, StrPluraliseNum(mtMap.size(), "cvar", "cvars"));
  } // No matches
  return StrFormat("No match from $.",
    StrPluraliseNum(mtMap.size(), "cvar", "cvars"));
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
