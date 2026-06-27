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
** ## VarName  ## A user-defined identifier to the value.                 ## **
** ## VarValue ## The variable to translate into the resulting message.   ## **
** ## ...      ## An infinite amount more of variables can be specified.  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IError {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
using namespace IString::P;            using namespace IUtf::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Helper plugin for C runtime errno checking --------------------------- */
struct ErrorPluginStandard final
{ /* -- Exception plugin to expand and report errno ------------------------ */
#define XCL(r,...) throw Error<ErrorPluginStandard>(r, ## __VA_ARGS__)
  /* -- Constructor to add C runtime error code ---------------------------- */
  explicit ErrorPluginStandard(StdOStringStream &osS)
    { osS << "\n+ Reason<" << StdGetError() << "> = \""
          << StrFromErrNo() << "\"."; }
};/* -- Standard exception plugin that does nothing ------------------------ */
struct ErrorPluginGeneric final
  { explicit ErrorPluginGeneric(StdOStringStream&) {} };
/* ------------------------------------------------------------------------- */
template<class Plugin = ErrorPluginGeneric>
  requires StdIsClass<Plugin>
class Error final :
  /* -- Derivced classes --------------------------------------------------- */
  public StdException,                 // So we can capture as exception
  public StdString                     // String to store generated string
{ /* -- Private variables -------------------------------------------------- */
  StdOStringStream osS;                // Error message builder
  /* -- Write left part of var --------------------------------------------- */
  void Init(const StdStringView &strvName, const StdStringView &strvType)
    { osS << "\n+ " << strvName << '<' << strvType << "> = "; }
  /* -- Show integer ------------------------------------------------------- */
  template<typename IntType, typename UIntType = StdMakeUnsigned<IntType>>
    requires StdIsIntegral<IntType> && StdIsUnsigned<UIntType>
  void Int(const StdStringView &strvName, const StdStringView &strvType,
    const IntType itVal)
  { // Write value and type to stream and then the human readable value
    Init(strvName, strvType);
    osS << StdIOSDec << itVal << " (0x" << StdIOSHex
        << static_cast<UIntType>(itVal) << ").";
  }
  /* -- Show float --------------------------------------------------------- */
  template<typename FloatType>
    void Float(const StdStringView &strvName, const StdStringView &strvType,
      const FloatType ftValue)
  { Init(strvName, strvType); osS << StdIOSFixed << ftValue << '.'; }
  /* -- Show STL string ---------------------------------------------------- */
  template<class StrType>
    void Str(const StdStringView &strvName, const StdStringView &strvType,
      StrType &&strValue)
  { // Initialise start of string
    Init(strvName, strvType);
    // String is empty? Write empty string label
    if(strValue.empty()) { osS << cCommon->CommonEmpty() << '.'; return; }
    // String is not displayable?
    if(strValue.front() < 32)
      { osS << cCommon->CommonInvalid() << '.'; return; }
    // Write first part of string preview
    osS << '\"' << strValue << '\"' << StdIOSDec
        << " [" << strValue.size();
    // Is a string view? (has no capacity())
    if constexpr(!StdIsSame<StdDecay<StrType>, StdStringView>)
      osS << '/' << strValue.capacity();
    // End of string preview
    osS << "].";
  }
  /* -- Last parameter processed ------------------------------------------- */
  void Param()
  { // Process custom plugin on generated error message
    const Plugin pPlugin{ osS };
    // Finalise the error message into string
    assign(osS.str());
    // It was just a functor cppcheck!
    static_cast<void>(pPlugin);
  }
  /* -- Process STL string ------------------------------------------------- */
  template<size_t stA, typename AnyType, typename ...VarArgs>
    requires (stA > 0)
  void Param(const char (&caName)[stA], AnyType &&atValue, VarArgs &&...vaArgs)
  { // Make a string view of parameter since we have the size
    const StdStringView strvName{ caName, stA - 1 };
    // Remove trash from type and if is integral or enum type?
    using AnyTypeNoRef = StdRemoveConstVolRef<AnyType>;
    using AnyTypeDecayed = StdDecay<AnyType>;
    if constexpr(StdIsIntegral<AnyTypeDecayed> || StdIsEnum<AnyTypeDecayed>)
    { // Is a boolean type?
      if constexpr(StdIsSame<AnyTypeDecayed, bool>)
      { // Initialise key name with a boolean type and make it human readable
        Init(strvName, "Bool");
        osS << StrFromBoolTF(atValue) << '.';
      } // If the type is a readable unsigned character type?
      else if constexpr(StdIsSame<AnyTypeDecayed, unsigned char>)
      { // Initialise key name with a uchar type and make it human readable
        Int<unsigned>(strvName, "UChar", static_cast<unsigned>(atValue));
        if(atValue > 32) osS << " '" << static_cast<char>(atValue) << "'.";
      } // If the type is a readable character type?
      else if constexpr(StdIsSame<AnyTypeDecayed, char>)
      { // Initialise key name with a char type and make it human readable
        Int<signed, unsigned>(strvName, "Char", static_cast<int>(atValue));
        if(atValue > 32) osS << " '" << atValue << "'.";
      } // If it is a 64-bit integer type?
      else if constexpr(sizeof(AnyTypeDecayed) == sizeof(uint64_t))
      { // If it's signed or unsigned type?
        if constexpr(StdIsSigned<AnyTypeDecayed>)
          Int<int64_t, uint64_t>(strvName, "Int64", atValue);
        else Int<uint64_t>(strvName, "UInt64", atValue);
      } // If it is a 32-bit integer type?
      else if constexpr(sizeof(AnyTypeDecayed) == sizeof(uint32_t))
      { // If it's signed or unsigned type?
        if constexpr(StdIsSigned<AnyTypeDecayed>)
          Int<int32_t, uint32_t>(strvName, "Int32", atValue);
        else Int<uint32_t>(strvName, "UInt32", atValue);
      } // If it is a 16-bit integer type?
      else if constexpr(sizeof(AnyTypeDecayed) == sizeof(uint16_t))
      { // If it's signed or unsigned type?
        if constexpr(StdIsSigned<AnyTypeDecayed>)
          Int<int16_t, uint16_t>(strvName, "Int16", atValue);
        else Int<uint16_t>(strvName, "UInt16", atValue);
      } // If it is a 8-bit integer type?
      else if constexpr(sizeof(AnyTypeDecayed) == sizeof(uint8_t))
      { // If it's signed or unsigned type?
        if constexpr(StdIsSigned<AnyTypeDecayed>)
          Int<int64_t, uint64_t>(strvName, "Int64", atValue);
        else Int<uint64_t>(strvName, "UInt64", atValue);
      } // Impossible size but just incase.
      else static_assert(sizeof(atValue) == 0, "Unknown integral type!");
    } // Is a floating point type?
    else if constexpr(StdIsFloat<AnyTypeDecayed>)
    { // Is a 64-bit floating point number?
      if constexpr(sizeof(AnyTypeDecayed) == sizeof(double))
        Float(strvName, "Float64", atValue);
      // Is a 32-bit floating point number?
      else if constexpr(sizeof(AnyTypeDecayed) == sizeof(float))
        Float(strvName, "Float32", atValue);
      // Impossible size but just incase.
      else static_assert(sizeof(atValue) == 0, "Unknown float type!");
    } // Is a class type?
    else if constexpr(StdIsClass<AnyTypeDecayed>)
    { // Is 'StdString' type?
      if constexpr(StdIsSame<AnyTypeDecayed, StdString>)
        Str(strvName, "Str", StdForward<AnyType>(atValue));
      // Is 'StdStringView' type?
      else if constexpr(StdIsSame<AnyTypeDecayed, StdStringView>)
        Str(strvName, "StrV", StdForward<AnyType>(atValue));
      // Is 'StdWideString' type?
      else if constexpr(StdIsSame<AnyTypeDecayed, StdWideString>)
        Str(strvName, "WStr", StdForward<AnyType>(atValue));
      // Is 'StdWideStringView' type?
      else if constexpr(StdIsSame<AnyTypeDecayed, StdWideStringView>)
        Str(strvName, "WStrV", StdForward<AnyType>(atValue));
      // Is 'StdException' type?
      else if constexpr(StdIsSame<AnyTypeDecayed, StdException>)
        Str(strvName, "Ex", StdStringView{ atValue.what() });
      // We don't support any other types yet
      else static_assert(sizeof(atValue) == 0, "Unknown class type!");
    } // Is an array type?
    else if constexpr(StdIsArray<AnyTypeNoRef>)
    { // Get the size of the text array and throw error if no size
      constexpr size_t stN = StdExtent<AnyTypeNoRef>;
      static_assert(stN > 0);
      // Initialise start of string
      Init(strvName, "CStrArray");
      // Empty?
      if(!*atValue) osS << cCommon->CommonEmpty() << '.';
      // Displayable?
      else if(*atValue < 32) osS << cCommon->CommonInvalid() << '.';
      // Valid? Display and translation if neccesary
      else osS << '\"' << StdStringView{ atValue, stN - 1 } << "\" ["
               << stN << "].";
    } // Is a pointer type?
    else if constexpr(StdIsPointer<AnyTypeDecayed>)
    { // Completely strip
      using AnyTypeNoRefPtr =
        StdRemoveConstVolRef<
          StdRemovePointer<StdRemoveConstVolRef<AnyTypeDecayed>>>;
      if constexpr(StdIsSame<AnyTypeNoRefPtr, char>)
      { // Initialise start of string
        Init(strvName, "CStr");
        // Get variable as a C-string
        if(!atValue) osS << cCommon->CommonNull() << '.';
        // Empty?
        else if(!*atValue) osS << cCommon->CommonEmpty() << '.';
        // Displayable?
        else if(*atValue < 32) osS << cCommon->CommonInvalid() << '.';
        // Valid? Display and translation if neccesary
        else osS << '\"' << atValue << "\".";
      } // If it's a wide char pointer type?
      else if constexpr(StdIsSame<AnyTypeNoRefPtr, wchar_t>)
      { // Initialise start of string
        Init(strvName, "WCStr");
        // Get variable as a C-string
        if(!atValue) osS << cCommon->CommonNull() << '.';
        // Empty?
        else if(!*atValue) osS << cCommon->CommonEmpty() << '.';
        // Displayable?
        else if(*atValue < 32) osS << cCommon->CommonInvalid() << '.';
        // Valid? Display and translation if neccesary
        else osS << '\"' << UtfFromWide(StdWideStringView{ atValue }) << "\".";
      } // Anything else we can just print the address
      else
      { // Initialise name
        Init(strvName, "Ptr");
         // Get variable as a C-string
        if(!atValue) osS << cCommon->CommonNull();
        // Valid? Display and translation if neccesary
#if defined(WINDOWS)                   // Using Windows?
        else osS << "0x" << atValue;   // Windows doesn't put in 0x for us
#else                                  // Using anything but Windows?
        else osS << atValue;           // Already puts in 0x for us
#endif                                 // Windows check
        // Add full stop
        osS << '.';
      }
    } // Unknown type was sent in a exception constructor
    else static_assert(sizeof(AnyType) == 0, "Invalid type!");
    // Process more parameters
    Param(StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Get message ------------------------------------------------ */ public:
  virtual const char *what() const noexcept override { return data(); }
  /* -- Prepare error message constructor with C-string--------------------- */
  template<typename AnyType, typename ...VarArgs>
    Error(AnyType &&aErrMsg, VarArgs &&...vaArgs)
  { // If is an exception object? Push the string of it
    if constexpr(StdIsSame<StdDecay<AnyType>, StdException>)
      osS << aErrMsg.what();
    // Let ostringstream handle the value
    else osS << aErrMsg;
    // Process key, value components
    Param(StdForward<VarArgs>(vaArgs)...);
  }
};/* -- Helper macro to trigger exceptions --------------------------------- */
#define XC(r,...) throw Error<>(r, ## __VA_ARGS__)
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
