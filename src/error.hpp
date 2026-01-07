/* == ERROR.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a very useful class to handle errors and display useful     ## **
** ## data to help debug the error. It uses the C++11 variadic templates  ## **
** ## Use the XC() macro as a helper to build an error message with your  ## **
** ## chosen parameters to debug...                                       ## **
** ######################################################################### **
** ## Usage: XC(Reason, ["VarName", VarValue[, ...]])                     ## **
** ######################################################################### **
** ## []       ## Denotes an optional argument.                           ## **
** ## Reason   ## The reason for the error.                               ## **
** ## VarName  ## A user-defined idenifier to the value.                  ## **
** ## VarValue ## The variable to translate into the resulting message.   ## **
** ## ...      ## An infinite amount more of variables can be specified.  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IError {                     // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
using namespace IString::P;            using namespace IUtf::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
struct ErrorPluginGeneric final
  { explicit ErrorPluginGeneric(ostringstream&) {} };
/* ------------------------------------------------------------------------- */
template<class Plugin=ErrorPluginGeneric>class Error final :
  /* -- Derivced classes --------------------------------------------------- */
  public exception,                    // So we can capture as exception
  public string                        // String to store generated string
{ /* -- Private variables -------------------------------------------------- */
  ostringstream osS;                   // Error message builder
  /* -- Write left part of var --------------------------------------------- */
  void Init(const char*const cpName, const char*const cpType)
    { osS << "\n+ " << cpName << '<' << cpType << "> = "; }
  /* -- Last parameter processed ------------------------------------------- */
  void Param()
  { // Process custom plugin on generated error message
    const Plugin pPlugin{ osS };
    // Finalise the error message into string
    assign(osS.str());
    // It was just a functor cppcheck!
    static_cast<void>(pPlugin);
  }
  /* -- Show integer ------------------------------------------------------- */
  template<typename Type,typename UnsignedType=Type>
    void Int(const char*const cpName, const char*const cpType, const Type tVal)
  { // Write integer to stream
    Init(cpName, cpType);
    // Write value
    osS << dec << tVal << " (0x" << hex
        << static_cast<UnsignedType>(tVal) << ").";
  }
  /* ----------------------------------------------------------------------- */
#if defined(WINDOWS)                   // Using windows?
  /* -- Process long integer ----------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const long lVal, VarArgs &&...vaArgs)
  { Int<long,unsigned long>(cpName, "Long", lVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process unsigned long integer -------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName,
      const unsigned long lVal, VarArgs &&...vaArgs)
  { Int<unsigned long>(cpName, "ULong", lVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* ----------------------------------------------------------------------- */
#elif defined(LINUX)                   // Targeting Linux?
  /* -- Process long long int ---------------------------------------------- */
  template<typename ...VarArgs>        // (Lua_Integer)
    void Param(const char*const cpName, const long long int lliVal,
      VarArgs &&...vaArgs)
  { Int<long long int>(cpName, "LongLongInt", lliVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process unsigned long long int ------------------------------------- */
  template<typename ...VarArgs>        // (Lua_Integer)
    void Param(const char*const cpName, const unsigned long long int ulliVal,
      VarArgs &&...vaArgs)
  { Int<unsigned long long int>(cpName, "ULongLongInt", ulliVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* ----------------------------------------------------------------------- */
#elif defined(MACOS)                   // Targeting Apple device?
  /* -- Process signed long ------------------------------------------------ */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const long lVal, VarArgs &&...vaArgs)
  { Int<long>(cpName, "Long", lVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process unsigned long ---------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const unsigned long ulVal,
      VarArgs &&...vaArgs)
  { Int<unsigned long>(cpName, "ULong", ulVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* ----------------------------------------------------------------------- */
#endif                                 // Target check
  /* -- Process 16-bit signed integer -------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const int16_t sVal,
      VarArgs &&...vaArgs)
  { Int<int16_t,uint16_t>(cpName, "Int16", sVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process 16-bit unsigned integer ------------------------------------ */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const uint16_t usVal,
      VarArgs &&...vaArgs)
  { Int<uint16_t>(cpName, "UInt16", usVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process 32-bit signed integer -------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const int32_t lVal,
      VarArgs &&...vaArgs)
  { Int<int32_t,uint32_t>(cpName, "Int32", lVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process 32-bit unsigned integer ------------------------------------ */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const uint32_t ulVal,
      VarArgs &&...vaArgs)
  { Int<uint32_t>(cpName, "UInt32", ulVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process 64-bit signed integer -------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const int64_t llVal,
      VarArgs &&...vaArgs)
  { Int<int64_t,uint64_t>(cpName, "Int64", llVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process 64-bit unsigned integer ------------------------------------ */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const uint64_t ullVal,
      VarArgs &&...vaArgs)
  { Int<uint64_t>(cpName, "UInt64", ullVal);
    Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process 8-bit unsigned integer ------------------------------------- */
  template<typename ...VarArgs>void Param(const char*const cpName,
    const unsigned char ucByte, VarArgs &&...vaArgs)
  { // First show as integer
    Int<unsigned int>(cpName, "UInt8", static_cast<unsigned int>(ucByte));
    // Display only if valid
    if(ucByte > 32) osS << " '" << static_cast<char>(ucByte) << "'.";
    // Process more parameters
    Param(StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Process 8-bit signed integer --------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const char cByte, VarArgs &&...vaArgs)
  { // First show as integer
    Int<int,unsigned int>(cpName, "Int8", static_cast<int>(cByte));
    // Display only if valid
    if(cByte > 32) osS << " '" << cByte << "'.";
    // Process more parameters
    Param(StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Show float --------------------------------------------------------- */
  template<typename FloatType>void Float(const char*const cpName,
    const char*const cpType, const FloatType tVal)
  { Init(cpName, cpType); osS << fixed << tVal << '.'; }
  /* -- Process 64-bit double ---------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const double dVal, VarArgs &&...vaArgs)
  { Float(cpName, "Float64", dVal); Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process 32-bit float ----------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const float fVal, VarArgs &&...vaArgs)
  { Float(cpName, "Float32", fVal); Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process boolean ---------------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const bool bFlag, VarArgs &&...vaArgs)
  { // Prepare parameter
    Init(cpName, "Bool");
    osS << StrFromBoolTF(bFlag) << '.';
    // Process more parameters
    Param(StdForward<VarArgs>(vaArgs)...);
  } /* -- Process pointer to address --------------------------------------- */
  template<typename ...VarArgs>void Param(const char*const cpName,
    const void*const vpPtr, VarArgs &&...vaArgs)
  { // Get StringStream
    Init(cpName, "Ptr");
     // Get variable as a C-string
    if(!vpPtr) osS << cCommon->CommonNull();
    // Valid? Display and translation if neccesary
#if defined(WINDOWS)                   // Using Windows?
    else osS << "0x" << vpPtr;         // Windows doesn't put in 0x for us
#else                                  // Using anything but Windows?
    else osS << vpPtr;                 // Already puts in 0x for us
#endif                                 // Windows check
    // Add full stop
    osS << '.';
    // Process more parameters
    Param(StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Process C-String ------------------------------------------------- */
  template<typename ...VarArgs>void Param(const char*const cpName,
    const char*const cpStr, VarArgs &&...vaArgs)
  { // Initialise start of string
    Init(cpName, "CStr");
    // Get variable as a C-string
    if(!cpStr) osS << cCommon->CommonNull() << '.';
    // Empty?
    else if(!*cpStr) osS << cCommon->CommonEmpty() << '.';
    // Displayable?
    else if(*cpStr < 32) osS << cCommon->CommonInvalid() << '.';
    // Valid? Display and translation if neccesary
    else osS << '\"' << cpStr << "\".";
    // Process more parameters
    Param(StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Process C-String ------------------------------------------------- */
  template<typename ...VarArgs>
    void Param[[maybe_unused]](const char*const cpName,
      const wchar_t*const wcpStr, VarArgs &&...vaArgs)
  { // Initialise start of string
    Init(cpName, "WCStr");
    // Get variable as a C-string
    if(!wcpStr) osS << cCommon->CommonNull() << '.';
    // Empty?
    else if(!*wcpStr) osS << cCommon->CommonEmpty() << '.';
    // Displayable?
    else if(*wcpStr < 32) osS << cCommon->CommonInvalid() << '.';
    // Valid? Display and translation if neccesary
    else osS << '\"' << UtfFromWide(wcpStr) << "\".";
    // Process more parameters
    Param(StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Process exception object ------------------------------------------- */
  template<typename ...VarArgs>void Param(const char*const cpName,
    const exception &e, VarArgs &&...vaArgs)
  { // Initialise start of string
    Init(cpName, "Ex");
    // Valid? Display and translation if neccesary
    osS << e.what() << '.';
    // Process more parameters
    Param(StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Show STL string ---------------------------------------------------- */
  template<class StringType>void Str(const char*const cpName,
    const char*const cpType, const StringType &tString)
  { // Initialise start of string
    Init(cpName, cpType);
    // String is empty?
    if(tString.empty()) osS << cCommon->CommonEmpty() << '.';
    // String is not displayable?
    else if(tString.front() < 32) osS << cCommon->CommonInvalid() << '.';
    // Valid? Is a string view? (has no capacity())
    else if constexpr(is_same_v<StringType, string_view>)
      osS << '\"' << tString << '\"' << dec << " [" << tString.length()
          << "].";
    // Valid? Display string
    else osS << '\"' << tString << '\"' << dec << " [" << tString.length()
             << '/' << tString.capacity() << "].";
  }
  /* -- Process STL string lvalue ------------------------------------------ */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const string &strV,
      VarArgs &&...vaArgs)
  { Str(cpName, "Str", strV); Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process STL wstring lvalue ----------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const wstring &wstrV,
      VarArgs &&...vaArgs)
  { Str(cpName, "WStr", wstrV); Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Process STL string_view -------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const string_view &strvV,
      VarArgs &&...vaArgs)
  { Str(cpName, "StrV", strvV); Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Get message ------------------------------------------------ */ public:
  virtual const char *what() const noexcept override { return data(); }
  /* -- Prepare error message constructor with C-string--------------------- */
  template<typename ...VarArgs>
    Error(const char*const cpErr, VarArgs &&...vaArgs)
  { osS << cpErr; Param(StdForward<VarArgs>(vaArgs)...); }
  /* -- Prepare error message constructor with exception object ------------ */
  template<typename ...VarArgs>
    Error(const exception &eReason, VarArgs &&...vaArgs) :
      Error(eReason.what(), StdForward<VarArgs>(vaArgs)...) {}
  /* -- Prepare error message constructor with STL string ------------------ */
  template<typename ...VarArgs>
    Error(const string &strErr, VarArgs &&...vaArgs)
  { osS << strErr; Param(StdForward<VarArgs>(vaArgs)...); }
};/* -- Helper macro to trigger exceptions --------------------------------- */
#define XC(r,...) throw Error<>(r, ## __VA_ARGS__)
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
