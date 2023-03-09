/* == ERROR.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a very useful class to handle errors and display useful     ## **
** ## data to help debug the error. It uses the C++11 variadic templates  ## **
** ## Use the XC() macro as a helper to build an error message with your  ## **
** ## chosen parameters to debug...                                       ## **
** ######################################################################### **
** ## Usage: XC(Reason, [VarName, VarValue[, ...]])                       ## **
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
using namespace IString::P;
using namespace IUtf;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
struct ErrorPluginGeneric final
  { explicit ErrorPluginGeneric(ostringstream&) { } };
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
  void Param(void) { const Plugin pPlugin(osS); assign(osS.str()); }
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
  template<typename ...VarArgs>void Param(const char*const cpName,
    const long lVal, const VarArgs &...vaVars)
  { Int<long,unsigned long>(cpName, "Long", lVal); Param(vaVars...); }
  /* -- Process unsigned long integer -------------------------------------- */
  template<typename ...VarArgs>void Param(const char*const cpName,
    const unsigned long lVal, const VarArgs &...vaVars)
  { Int<unsigned long>(cpName, "ULong", lVal); Param(vaVars...); }
  /* ----------------------------------------------------------------------- */
#elif defined(MACOS)                   // Targeting Apple device?
  /* -- Process signed long ------------------------------------------------ */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const long lVal,
      const VarArgs &...vaVars)
  { Int<long>(cpName, "Long", lVal); Param(vaVars...); }
  /* -- Process unsigned long ---------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const unsigned long ulVal,
      const VarArgs &...vaVars)
  { Int<unsigned long>(cpName, "ULong", ulVal); Param(vaVars...); }
  /* ----------------------------------------------------------------------- */
#endif                                 // Target check
  /* -- Process 16-bit signed integer -------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const int16_t sVal,
      const VarArgs &...vaVars)
  { Int<int16_t,uint16_t>(cpName, "Int16", sVal); Param(vaVars...); }
  /* -- Process 16-bit unsigned integer ------------------------------------ */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const uint16_t usVal,
      const VarArgs &...vaVars)
  { Int<uint16_t>(cpName, "UInt16", usVal); Param(vaVars...); }
  /* -- Process 32-bit signed integer -------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const int32_t lVal,
      const VarArgs &...vaVars)
  { Int<int32_t,uint32_t>(cpName, "Int32", lVal); Param(vaVars...); }
  /* -- Process 32-bit unsigned integer ------------------------------------ */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const uint32_t ulVal,
      const VarArgs &...vaVars)
  { Int<uint32_t>(cpName, "UInt32", ulVal); Param(vaVars...); }
  /* -- Process 64-bit signed integer -------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const int64_t qVal,
      const VarArgs &...vaVars)
  { Int<int64_t,uint64_t>(cpName, "Int64", qVal); Param(vaVars...); }
  /* -- Process 64-bit unsigned integer ------------------------------------ */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const uint64_t uqVal,
      const VarArgs &...vaVars)
  { Int<uint64_t>(cpName, "UInt64", uqVal); Param(vaVars...); }
  /* -- Process 8-bit unsigned integer ------------------------------------- */
  template<typename ...VarArgs>void Param(const char*const cpName,
    const unsigned char ucByte, const VarArgs &...vaVars)
  { // First show as integer
    Int<unsigned int>(cpName, "UInt8", static_cast<unsigned int>(ucByte));
    // Display only if valid
    if(ucByte > 32) osS << " '" << static_cast<char>(ucByte) << "'.";
    // Process more parameters
    Param(vaVars...);
  }
  /* -- Process 8-bit signed integer --------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const char cByte,
      const VarArgs &...vaVars)
  { // First show as integer
    Int<int,unsigned int>(cpName, "Int8", static_cast<int>(cByte));
    // Display only if valid
    if(cByte > 32) osS << " '" << cByte << "'.";
    // Process more parameters
    Param(vaVars...);
  }
  /* -- Show float --------------------------------------------------------- */
  template<typename FloatType>void Float(const char*const cpName,
    const char*const cpType, const FloatType tVal)
  { Init(cpName, cpType); osS << fixed << tVal << '.'; }
  /* -- Process 64-bit double ---------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const double dVal,
      const VarArgs &...vaVars)
  { Float(cpName, "Float64", dVal); Param(vaVars...); }
  /* -- Process 32-bit float ----------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const float fVal,
      const VarArgs &...vaVars)
  { Float(cpName, "Float32", fVal); Param(vaVars...); }
  /* -- Process boolean ---------------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const bool bFlag,
      const VarArgs &...vaVars)
  { // Prepare parameter
    Init(cpName, "Bool");
    osS << StrFromBoolTF(bFlag) << '.';
    // Process more parameters
    Param(vaVars...);
  } /* -- Process pointer to address --------------------------------------- */
  template<typename ...VarArgs>void Param(const char*const cpName,
    const void*const vpPtr, const VarArgs &...vaVars)
  { // Get StringStream
    Init(cpName, "Ptr");
     // Get variable as a C-string
    if(!vpPtr) osS << "<Null>";
    // Valid? Display and translation if neccesary
#if defined(WINDOWS)                   // Using Windows?
    else osS << "0x" << vpPtr;         // Windows doesn't put in 0x for us
#else                                  // Using anything but Windows?
    else osS << vpPtr;                 // Already puts in 0x for us
#endif                                 // Windows check
    // Add full stop
    osS << '.';
    // Process more parameters
    Param(vaVars...);
  }
  /* -- Process C-String ------------------------------------------------- */
  template<typename ...VarArgs>void Param(const char*const cpName,
    const char*const cpStr, const VarArgs &...vaVars)
  { // Initialise start of string
    Init(cpName, "CStr");
    // Get variable as a C-string
    if(!cpStr) osS << "<Null>.";
    // Empty?
    else if(!*cpStr) osS << "<Empty>.";
    // Displayable?
    else if(*cpStr < 32) osS << "<Invalid>.";
    // Valid? Display and translation if neccesary
    else osS << '\"' << cpStr << "\".";
    // Process more parameters
    Param(vaVars...);
  }
  /* -- Process C-String ------------------------------------------------- */
  template<typename ...VarArgs>
    void Param[[maybe_unused]](const char*const cpName,
      const wchar_t*const wcpStr, const VarArgs &...vaVars)
  { // Initialise start of string
    Init(cpName, "WCStr");
    // Get variable as a C-string
    if(!wcpStr) osS << "<Null>.";
    // Empty?
    else if(!*wcpStr) osS << "<Empty>.";
    // Displayable?
    else if(*wcpStr < 32) osS << "<Invalid>.";
    // Valid? Display and translation if neccesary
    else osS << '\"' << UtfFromWide(wcpStr) << "\".";
    // Process more parameters
    Param(vaVars...);
  }
  /* -- Process exception object ------------------------------------------- */
  template<typename ...VarArgs>void Param(const char*const cpName,
    const exception &e, const VarArgs &...vaVars)
  { // Initialise start of string
    Init(cpName, "Ex");
    // Valid? Display and translation if neccesary
    osS << e.what() << '.';
    // Process more parameters
    Param(vaVars...);
  }
  /* -- Show STL string ---------------------------------------------------- */
  template<class StringType>void Str(const char*const cpName,
    const char*const cpType, const StringType &tString)
  { // Initialise start of string
    Init(cpName, cpType);
    // String is empty?
    if(tString.empty()) osS << "<Empty>.";
    // String is not displayable?
    else if(tString.front() < 32) osS << "<Invalid>.";
    // Valid? Is a string view? (has no capacity())
    else if constexpr(is_same_v<StringType, string_view>)
      osS << '\"' << tString << '\"' << dec
                  << " [" << tString.length() << "].";
    // Valid? Display string
    else osS << '\"' << tString << '\"' << dec
             << " [" << tString.length() << '/'
             << tString.capacity() << "].";
  }
  /* -- Process std::string lvalue ----------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const string &strV,
      const VarArgs &...vaVars)
        { Str(cpName, "Str", strV); Param(vaVars...); }
  /* -- Process std::wstring lvalue ---------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const wstring &wstrV,
      const VarArgs &...vaVars)
        { Str(cpName, "WStr", wstrV); Param(vaVars...); }
  /* -- Process std::string_view ------------------------------------------- */
  template<typename ...VarArgs>
    void Param(const char*const cpName, const string_view &strvV,
      const VarArgs &...vaVars)
        { Str(cpName, "StrV", strvV); Param(vaVars...); }
  /* -- Get message ------------------------------------------------ */ public:
  virtual const char *what(void) const noexcept override { return c_str(); }
  /* -- Prepare error message constructor with C-string--------------------- */
  template<typename ...VarArgs>
    Error(const char*const cpErr, const VarArgs &...vaVars)
      { osS << cpErr; Param(vaVars...); }
  /* -- Prepare error message constructor with STL string ------------------ */
  template<typename ...VarArgs>
    Error(const string &strErr, const VarArgs &...vaVars)
      { osS << strErr; Param(vaVars...); }
};/* -- Helper macro to trigger exceptions --------------------------------- */
#define XC(r,...) throw Error<>(r, ## __VA_ARGS__)
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
