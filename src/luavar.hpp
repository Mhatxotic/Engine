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
using namespace ICollector::P;         using namespace ICVarDef::P;
using namespace ICVar::P;              using namespace ICVarLib::P;
using namespace IError::P;             using namespace IIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace ILuaFunc::P;           using namespace ILua::P;
using namespace IString::P;            using namespace IStat::P;
using namespace IStd::P;               using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
typedef IdMap<CVarFlagsType> IdMapCVarEnums;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Lua cvar list types -------------------------------------------------- */
typedef pair<LuaFunc, CVarMapIt> LuaCVarPair;
MAPPACK_BUILD(LuaCVar, const string, LuaCVarPair)
/* -- Variables ollector class for collector data and custom variables ----- */
CTOR_BEGIN(Variables, Variable, CLHelperSafe,
  /* ----------------------------------------------------------------------- */
  LuaCVarMap       lcvmMap;            // Lua cvar list
  /* -- Cvar flag type strings --------------------------------------------- */
  const IdMapCVarEnums imcveTypes;     // Types
  const IdMapCVarEnums imcveConditions;  // Conditional flags
  const IdMapCVarEnums imcvePermissions; // Permission flags
  const IdMapCVarEnums imcveSources;   // Load reason flags
  const IdMapCVarEnums imcveOther;,    // Misc flags
);/* -- Lua variables collector and member class --------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Variables, Variable, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable                      // Lua garbage collector instruction
{ /* -- Private variables -------------------------------------------------- */
  LuaCVarMapIt     lcvmiIt;            // Iterator to command Console gives us
  /* -- Returns the lua console command list ------------------------------- */
  LuaCVarMap &GetLuaVarList(void) { return cVariables->lcvmMap; }
  /* -- Returns the end of the lua console command list -------------------- */
  LuaCVarMapIt GetLuaVarListEnd(void) { return GetLuaVarList().end(); }
  /* == Cvar updated callback for Lua ============================== */ public:
  static CVarReturn LuaCallbackStatic(
    CVarItem &cviVar,               // cppcheck-suppress constParameterCallback
    const string &strVal)
  { // Find cvar and ignore if we don't have it yet! This can happen if the
    // variable is initialising for the first time. We haven't added the
    // variable to cvmActive yet and we don't want to until the CVARS system
    // has created the variable.
    const LuaCVarMapIt lcvmpIt{ cVariables->lcvmMap.find(cviVar.GetVar()) };
    if(lcvmpIt == cVariables->lcvmMap.cend()) return ACCEPT;
    // Call the Lua callback assigned. We're expecting one return value
    lcvmpIt->second.first.LuaFuncProtectedDispatch(1, strVal, cviVar.GetVar());
    // Get result of the callback which means a boolean HAS to be returned or
    // we will deny the cvar change by default.
    const CVarReturn cvResult = BoolToCVarReturn(
      LuaUtilIsBoolean(cLuaFuncs->LuaRefGetState(), -1) &&
      LuaUtilIsBoolean(cLuaFuncs->LuaRefGetState(), -1));
    // Remove whatever it was that was returned by the callback
    LuaUtilRmStack(cLuaFuncs->LuaRefGetState());
    // Return result to cvar set function
    return cvResult;
  }
  /* -- Unregister the console command from lua -------------------- */ public:
  const string &Name(void) const { return lcvmiIt->first; }
  /* -- Get current value as string ---------------------------------------- */
  const string Get(void) const
    { return cCVars->GetStr(lcvmiIt->second.second); }
  /* -- Get default value as string ---------------------------------------- */
  const string Default(void) const
    { return cCVars->GetDefStr(lcvmiIt->second.second); }
  /* -- Reset default value ------------------------------------------------ */
  void Reset(void) const { cCVars->Reset(lcvmiIt->second.second); }
  /* -- Returns if value is empty ------------------------------------------ */
  bool Empty(void) const { return Get().empty(); }
  /* -- Set value ---------------------------------------------------------- */
  CVarSetEnums Set(const string &strValue) const
    { return cCVars->Set(lcvmiIt->second.second, strValue); }
  /* -- Register user console command from lua ----------------------------- */
  void Init(lua_State*const lS)
  { // Must be running on the main thread
    cLua->StateAssert(lS);
    // Must have 5 parameters (including this class ptr that was just created)
    LuaUtilCheckParams(lS, 5);
    // Get and check the variable name
    const string strName{ LuaUtilGetCppStr(lS, 1) };
    // Check that the variable name is valid
    if(!cCVars->IsValidVariableName(strName))
      XC("CVar name is not valid!",
         "Variable", strName,
         "Minimum",  cCVars->stCVarMinLength,
         "Maximum",  cCVars->stCVarMaxLength);
    // Make sure cvar doesn't already exist
    if(cCVars->VarExists(strName))
      XC("CVar already registered!", "Variable", strName);
    // Get the value name
    const string strD{ LuaUtilGetCppStr(lS, 2) };
    // Get the flags and check that the flags are in range
    const CVarFlagsConst cvfcFlags{ LuaUtilGetFlags(lS, 3, CVMASK) };
    // Check that the var has at least one type
    if(!(cvfcFlags.FlagIsAnyOfSet(TSTRING|TINTEGER|TFLOAT|TBOOLEAN) &&
      // Check that types are not mixed
      cvfcFlags.FlagIsAnyOfSetAndClear(
        TSTRING,  /* <- Set? & Clear? -> */ TINTEGER|TFLOAT|TBOOLEAN,  /* Or */
        TINTEGER, /* <- Set? & Clear? -> */ TSTRING|TFLOAT|TBOOLEAN,   /* Or */
        TFLOAT,   /* <- Set? & Clear? -> */ TSTRING|TINTEGER|TBOOLEAN, /* Or */
        TBOOLEAN, /* <- Set? & Clear? -> */ TSTRING|TINTEGER|TFLOAT)))
      XC("CVar flags have none or mixed types!",
         "Variable", strName, "Flags", cvfcFlags.FlagGet());
    // Check that the fourth parameter is a function
    LuaUtilCheckFunc(lS, 4);
    // Since the userdata for this class object is at arg 5, we need to make
    // sure the callback function is ahead of it in arg 6 or the LuaFunc()
    // class which calls luaL_ref will fail as it ONLY reads position -1.
    LuaUtilCopyValue(lS, 4);
    // Save the function at the top of the stack used for the callback
    lcvmiIt = cVariables->lcvmMap.insert(GetLuaVarListEnd(), { strName,
      make_pair(LuaFunc{ StrAppend("CV:", strName), true },
        cCVars->GetVarListEnd())
    }); // Register the variable and set the iterator to the new cvar.
    lcvmiIt->second.second = cCVars->RegisterVar(strName, strD,
      LuaCallbackStatic, cvfcFlags|TLUA|PANY);
  }
  /* -- Destructor that unregisters the cvar ------------------------------- */
  ~Variable(void)
  { // Return if the iterator is invalid?
    if(lcvmiIt == GetLuaVarListEnd()) return;
    // Unregister the cvar
    if(lcvmiIt->second.second != cCVars->GetVarListEnd())
      cCVars->UnregisterVar(lcvmiIt->second.second);
    // Remove the lua var
    GetLuaVarList().erase(lcvmiIt);
  }
  /* -- Basic constructor with no init ------------------------------------- */
  Variable(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperVariable{                  // Initialise and register the object
      cVariables, this },
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    lcvmiIt{ GetLuaVarListEnd() }      // Initialise iterator to the last
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Variable)            // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
CTOR_END(Variables, Variable, VARIABLE,,,, // Finish off collector class
/* ------------------------------------------------------------------------- */
imcveTypes{{                           // Cvar types
  IDMAPSTR(TSTRING),                   IDMAPSTR(TINTEGER),
  IDMAPSTR(TFLOAT),                    IDMAPSTR(TBOOLEAN),
  IDMAPSTR(TLUA),
}, "NONE" },
/* ------------------------------------------------------------------------- */
imcveConditions{{                      // Conditional flags
  IDMAPSTR(CALPHA),                    IDMAPSTR(CNUMERIC),
  IDMAPSTR(CSAVEABLE),                 IDMAPSTR(CPROTECTED),
  IDMAPSTR(CDEFLATE),                  IDMAPSTR(CNOTEMPTY),
  IDMAPSTR(CUNSIGNED),                 IDMAPSTR(CPOW2),
  IDMAPSTR(CFILENAME),                 IDMAPSTR(CTRUSTEDFN),
}, "NONE" },
/* ------------------------------------------------------------------------- */
imcvePermissions{{                     // Permission flags
  IDMAPSTR(PBOOT),                     IDMAPSTR(PSYSTEM),
  IDMAPSTR(PUSR),
}, "NONE" },
/* ------------------------------------------------------------------------- */
imcveSources{{                         // Load sources
  IDMAPSTR(SENGINE),                   IDMAPSTR(SCMDLINE),
  IDMAPSTR(SAPPCFG),                   IDMAPSTR(SUDB),
}, "NONE" },
/* ------------------------------------------------------------------------- */
imcveOther{{                           // Misc flags
  IDMAPSTR(MTRIM),                     IDMAPSTR(OSAVEFORCE),
  IDMAPSTR(LOCKED),                    IDMAPSTR(COMMIT),
  IDMAPSTR(PURGE),                     IDMAPSTR(CONFIDENTIAL),
  IDMAPSTR(LOADED),
}, "NONE" }
);/* -- Return human readable string about CVar ---------------------------- */
static const string VariablesMakeInformation(const CVarItem &cviVar)
{ // Print data about the cvar
  return StrFormat("Status for '$'...\n"
    "- Callback: $.\n"               "- Flags: 0x$$$.\n"
    "- Types: $.\n"                  "- Conditions: $.\n"
    "- Permissions: $.\n"            "- Source: $.\n"
    "- Other: $.\n"                  "- Default: [$/$] \"$\".\n"
    "- Modified: $.\n"               "- Current: [$/$] \"$\".",
      cviVar.GetVar(),
      StrFromBoolTF(cviVar.IsTriggerSet()),
      hex, cviVar, dec,
      StrImplode(cVariables->imcveTypes.Test(cviVar), 0, ", "),
      StrImplode(cVariables->imcveConditions.Test(cviVar), 0, ", "),
      StrImplode(cVariables->imcvePermissions.Test(cviVar), 0, ", "),
      StrImplode(cVariables->imcveSources.Test(cviVar), 0, ", "),
      StrImplode(cVariables->imcveOther.Test(cviVar), 0, ", "),
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
    { true, cviVar.FlagIsSet(CFILENAME)  ? 'F' :
           (cviVar.FlagIsSet(CTRUSTEDFN) ? 'T' :
           (cviVar.FlagIsSet(TSTRING)    ? 'S' :
           (cviVar.FlagIsSet(TINTEGER)   ? 'I' :
           (cviVar.FlagIsSet(TFLOAT)     ? 'N' :
           (cviVar.FlagIsSet(TBOOLEAN)   ? 'B' :
                                           '?'))))) },
    // Permissions
    { cviVar.FlagIsSet(PBOOT),        '1' },
    { cviVar.FlagIsSet(PSYSTEM),      '2' },
    { cviVar.FlagIsSet(PUSR),         '3' },
    // Sources
    { cviVar.FlagIsSet(SENGINE),      '6' },
    { cviVar.FlagIsSet(SCMDLINE),     '7' },
    { cviVar.FlagIsSet(SAPPCFG),      '8' },
    { cviVar.FlagIsSet(SUDB) ,        '9' },
    // Conditions and operations
    { cviVar.FlagIsSet(CUNSIGNED),    'U' },
    { cviVar.FlagIsSet(TLUA),         'L' },
    { cviVar.FlagIsSet(CONFIDENTIAL), 'C' },
    { cviVar.FlagIsSet(CPROTECTED),   'P' },
    { cviVar.FlagIsSet(CDEFLATE),     'D' },
    { cviVar.FlagIsSet(COMMIT),       'M' },
    { cviVar.FlagIsSet(LOADED),       'O' },
    { cviVar.FlagIsSet(CSAVEABLE),    'V' },
    { cviVar.FlagIsSet(OSAVEFORCE),   'Z' },
    { cviVar.FlagIsSet(CPOW2),        'W' },
    { cviVar.FlagIsSet(CNOTEMPTY),    'Y' },
    { cviVar.FlagIsSet(MTRIM),        'R' },
    { cviVar.IsTriggerSet(),          'K' }
  // Name and value
  })).Data(cviVar.GetVar()).Data(cviVar.Protect());
}
/* -- Enumerate a list ----------------------------------------------------- */
template<class MapType>
  static const string VariablesMakeList(const MapType &mtMap,
    const string &strFilter)
{ // Get pending cvars list and ignore if empty
  if(mtMap.empty()) return "No cvars exist in this category!";
  // Try to find the cvar outright first (only make work when not in release)
  typedef typename MapType::const_iterator MapTypeConstIt;
  const MapTypeConstIt mtciExactIt{ mtMap.find(strFilter) };
  if(mtciExactIt != mtMap.cend())
  {  // Type could either be CVarMap?
    if constexpr(is_same_v<MapType, CVarMap>)
      return VariablesMakeInformation(mtciExactIt->second);
    // ..or the type could either be LuaCVarMap
    else if constexpr(is_same_v<MapType, LuaCVarMap>)
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
      const string &strKey = mtciIt->first;
      if(strKey.compare(0, strFilter.size(), strFilter)) continue;
      // Increment matched counter
      ++stMatched;
      // Type could either be CVarMap?
      if constexpr(is_same_v<MapType, CVarMap>)
        VariablesMakeInformationTokens(sTable, mtciIt->second);
      // ..or the type could either be LuaCVarMap
      else if constexpr(is_same_v<MapType, LuaCVarMap>)
        VariablesMakeInformationTokens(sTable, mtciIt->second.second->second);
    } // Until no more commands
    while(++mtciIt != mtMap.cend());
    // Print output if we matched commands
    if(stMatched) return StrFormat("$$ of $ matched.", sTable.Finish(),
        stMatched, StrCPluraliseNum(mtMap.size(), "cvar", "cvars"));
  } // No matches
  return StrFormat("No match from $.",
    StrCPluraliseNum(mtMap.size(), "cvar", "cvars"));
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
