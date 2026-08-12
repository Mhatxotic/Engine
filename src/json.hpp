/* == JSON.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines a class that allows JSON encoding or decoding   ## **
** ## to performed.                                                       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IJson {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IASync::P;
using namespace ICollector::P;         using namespace IError::P;
using namespace IEvtMain::P;           using namespace IFileMap::P;
using namespace IFStream::P;           using namespace ILockable::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IName::P;
using namespace ISerial::P;            using namespace IStd::P;
using namespace ISysUtil::P;           using namespace IUtil::P;
using namespace Lib::RapidJson;
/* ------------------------------------------------------------------------- */
using Lib::RapidJson::Value;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Json object collector and member class =============================== */
CTOR_BEGIN_ASYNC_DUO(Jsons, Json, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoaderJson,              // Asynchronous loading of Json object
  public Lockable,                     // Lua garbage collector instruction
  public Document                      // RapidJson document class
{ /* -- Build a json string from lua string ----------------------- */ private:
  Value ToStr(lua_State*const lS, const int iIndex)
  { // Get string and length from LUA
    size_t stStr; const char*const cpStr = LuaUtilToLString(lS, iIndex, stStr);
    // Return as a json string. Unfortunately, ALL strings from LUA are
    // volatile so we need to copy the string.
    return { cpStr, static_cast<SizeType>(stStr), GetAllocator() };
  }
  /* -- Handle type value -------------------------------------------------- */
  template<typename Value>
    static void ProcessValueType(lua_State*const lS, const Value &vValue)
  { // What type is the value?
    switch(vValue.GetType())
    { // Json entry is a number type?
      case kNumberType:
        // Actually an integer or a number type?
        if(vValue.IsInt()) LuaUtilPushInt(lS, vValue.GetInt());
        else LuaUtilPushNum(lS, vValue.GetDouble());
        break;
      // Json entry is a string type?
      case kStringType:
        LuaUtilPushLStr(lS, vValue.GetString(), vValue.GetStringLength());
        break;
      // Json entry is a boolean type?
      case kTrueType:
        LuaUtilPushBool(lS, true);
        break;
      case kFalseType:
        LuaUtilPushBool(lS, false);
        break;
      // Json entry is an array[] type?
      case kArrayType:
        ToTableArray(lS, vValue);
        break;
      // Json entry is an object{} type?
      case kObjectType:
        ToTableObject(lS, vValue);
        break;
      // Json entry is a null type?
      case kNullType: [[fallthrough]];
      // Unknown type?
      default:
        LuaUtilPushNil(lS);
        break;
    }
  }
  /* -- Sort entire json array --------------------------------------------- */
  template<class SortType>void SortArray(Value &vArray)
  { // For each table item, search for and sort all sub-tables
    for(Value &vIndice : vArray.GetArray())
      switch(vIndice.GetType())
      { // Indexed array
        case kArrayType: SortArray<SortType>(vIndice); break;
        // Key/value object
        case kObjectType: SortObject<SortType>(vIndice); break;
        // Don't care about other types
        default: continue;
      }
  }
  /* -- Sort entire json object -------------------------------------------- */
  template<class SortType>void SortObject(Value &vObject)
  { // For each table item, search for and sort all sub-tables
    for(Value::Member &vmMember : vObject.GetObject())
      switch(vmMember.value.GetType())
      { // Indexed array
        case kArrayType: SortArray<SortType>(vmMember.value); break;
        // Key/value object
        case kObjectType: SortObject<SortType>(vmMember.value); break;
        // Don't care about other types
        default: continue;
      }
    // Do the sort
    StdSort(par_unseq, vObject.MemberBegin(), vObject.MemberEnd(), SortType());
  }
  /* -- Convert LUA table to rapidjson::Value ------------------------------ */
  Value ParseTable(lua_State*const lS, const int iTIndex)
  { // Get size of table and if we have length then we need to create an array
    if(const lua_Integer liLen =
      UtilIntOrMax<lua_Integer>(LuaUtilGetSize(lS, iTIndex)))
    {  // Set this value is array
      Value rjvRoot{ kArrayType };
      // We need one more free item on the stack, leave empty if not
      if(!LuaUtilIsStackAvail(lS, UtilIntOrMax<int>(liLen))) return rjvRoot;
      // Until end of table
      for(lua_Integer lI = 1; lI <= liLen; ++lI)
      { // Get first item and the index of it
        LuaUtilGetRefEx(lS, iTIndex, lI);
        const int iIndex = LuaUtilStackSize(lS);
        // Append value if a string. Lua will convert any valid numbered
        // string to a number if this is not checked before integral checks.
        // Test with: lexec Console.Write(Json.Table({1,2,'3'}):ToHRString());
        switch(lua_type(lS, iIndex))
        { // Variable is a number
          case LUA_TNUMBER:
            if(LuaUtilIsInteger(lS, iIndex))
              rjvRoot.PushBack(Value().
                SetInt64(LuaUtilToInt(lS, iIndex)), GetAllocator());
            else rjvRoot.PushBack(LuaUtilToNum(lS, iIndex), GetAllocator());
            break;
          // Variable is a boolean
          case LUA_TBOOLEAN: rjvRoot.PushBack(LuaUtilToBool(lS, iIndex),
            GetAllocator()); break;
          // Variable is a table
          case LUA_TTABLE: rjvRoot.PushBack(ParseTable(lS, iIndex),
            GetAllocator()); break;
          // Unknown or a string, just add a string
          case LUA_TSTRING:
          default: rjvRoot.PushBack(ToStr(lS, iIndex), GetAllocator()); break;
        } // Remove the last item
        LuaUtilRmStack(lS);
      } // Return new object
      return rjvRoot;
    } // Set this value as object
    Value rjvRoot{ kObjectType };
    // We need two more free item on the stack, leave empty if not
    if(!LuaUtilIsStackAvail(lS, 2)) return rjvRoot;
    // Key and value indexes. We do another stack size query because initially
    // there could be a JSon object on the stack when called by the guest but
    // subsequent calls to nested tables won't duplicate that value.
    const int iKIndex = LuaUtilStackSize(lS) + 1, iVIndex = iKIndex + 1;
    // Walk through all the object members
    for(LuaUtilPushNil(lS);
        LuaUtilTableEnumerateKeyValues(lS, iTIndex);
        LuaUtilRmStack(lS))
    { // Get keyname
      Value vKey{ ToStr(lS, iKIndex) };
      // Set the key->value for the LUA variable
      switch(lua_type(lS, iVIndex))
      { // Variable is a number
        case LUA_TNUMBER:
          if(LuaUtilIsInteger(lS, iVIndex))
            rjvRoot.AddMember(vKey,
              Value().SetInt64(LuaUtilToInt(lS, iVIndex)),
              GetAllocator());
          else
            rjvRoot.AddMember(vKey, LuaUtilToNum(lS, iVIndex), GetAllocator());
          break;
        // Variable is a boolean
        case LUA_TBOOLEAN:
          rjvRoot.AddMember(vKey, LuaUtilToBool(lS, iVIndex), GetAllocator());
          break;
        // Variable is a table
        case LUA_TTABLE: rjvRoot.AddMember(vKey,
          ParseTable(lS, iVIndex), GetAllocator()); break;
        // Unknown or a string, just add as string
        case LUA_TSTRING:
        default: rjvRoot.AddMember(vKey, ToStr(lS, iVIndex), GetAllocator());
          break;
      } // Test: lexec return Json.Table({a=1,b={a=1}}):ToHRString();
    } // Return new object
    return rjvRoot;
  }
  /* -- When file data has loaded ---------------------------------- */ public:
  void AsyncReady(const FileMap &fmData)
  { // The memory isn't null-terminated so we have to do that and also this
    // could be a file map so we can't modify it so we'll load it all properly
    // and null-terminate it and place it into a RapidJson StringStream object
    // and then place that into a CSW object so we can track the source
    // information when a parse error occurs.
    const StdString strJson{ fmData.MemToString() };
    StringStream ssStream{ strJson.data() };
    CursorStreamWrapper<StringStream> cswStream{ ssStream };
    // Parse the text and if there is a parse error? Break execution
    if(ParseStream(cswStream).HasParseError())
      XC(GetParseError_En(GetParseError()),
        "Name",   fmData.NameGet(), "Line", cswStream.GetLine(),
        "Column", cswStream.GetColumn());
    // Write that we parsed this stream
    cLog->LogDebugExSafe("Json parsed $ bytes from '$' successfully.",
      fmData.MemSize(), fmData.NameGet());
  }
  /* -- Convert LUA table to rapidjson::Value and assign it as root key ---- */
  void ParseTableSafe(lua_State*const lS, const int iTIndex)
  { // Check table
    LuaUtilCheckTable(lS, iTIndex);
    // Create the new root key
    Value vRoot{ ParseTable(lS, iTIndex) };
    // Make this the actual new root key
    Swap(vRoot);
  }
  /* -- Convert json value to lua object table and put it on stack --------- */
  static void ToTableObject(lua_State*const lS, const Value &vObject)
  { // Create the table, we're creating non-indexed key/value pairs
    LuaUtilPushTable(lS, 0, vObject.MemberCount());
    // We need two more free item on the stack, leave empty if not
    if(!LuaUtilIsStackAvail(lS, 2)) return;
    // Get index of object
    const int iOIndex = LuaUtilStackSize(lS);
    // For each table item
    for(const Value::Member &vmMember : vObject.GetObject())
    { // What type is the value?
      ProcessValueType(lS, vmMember.value);
      // Push key name
      LuaUtilSetField(lS, iOIndex, vmMember.name.GetString());
    }
  }
  /* -- Convert json value to lua array table and put it on stack ---------- */
  static void ToTableArray(lua_State*const lS, const Value &vArray)
  { // Create the table, we're creating a indexed/value array
    LuaUtilPushTable(lS, vArray.Size());
    // We need two more free items on the stack, leave empty if not
    if(vArray.Empty() || !LuaUtilIsStackAvail(lS, 2)) return;
    // Get index of array
    const int iAIndex = LuaUtilStackSize(lS);
    // Index id
    lua_Integer liId = 0;
    // For each table item
    for(const Value &vIndice : vArray.GetArray())
    { // Table index
      LuaUtilPushInt(lS, ++liId);
      // What type is the value?
      ProcessValueType(lS, vIndice);
      // Push key pair as integer table
      LuaUtilSetRaw(lS, iAIndex);
    }
  }
  /* -- Convert json value to lua table and put it on stack ---------------- */
  void ToLuaTable(lua_State*const lS)
  { // Get root object
    const Value &vRoot =
      reinterpret_cast<const Value&>(static_cast<const Document&>(*this));
    // What type is the value?
    switch(vRoot.GetType())
    { // Indexed array
      case kArrayType: ToTableArray(lS, vRoot); break;
      // Key/value object
      case kObjectType: ToTableObject(lS, vRoot); break;
      // Unacceptable
      default: XC("Not array or object!",
        "Name", NameGet(), "Type", vRoot.GetType());
    }
  }
  /* -- Start sorting the entire array ------------------------------------- */
  void Sort(const bool bDescending)
  { // Get root object
    Value &rjvVal = reinterpret_cast<Value&>(static_cast<Document&>(*this));
    // Ascending sorting algorithm
    struct SortAscending {
      bool operator()(const Value::Member &vLhs,
                      const Value::Member &vRhs) const {
        return strcmp(vLhs.name.GetString(), vRhs.name.GetString()) < 0;
      }
    };
    // Descending sorting algorithm
    struct SortDescending {
      bool operator()(const Value::Member &vLhs,
                      const Value::Member &vRhs) const {
        return strcmp(vLhs.name.GetString(), vRhs.name.GetString()) > 0;
      }
    };
    // What type is the value?
    switch(rjvVal.GetType())
    { // Indexed array
      case kArrayType: bDescending ? SortArray<SortDescending>(rjvVal) :
                                     SortArray<SortAscending>(rjvVal); break;
      // Key/value object
      case kObjectType: bDescending ? SortObject<SortDescending>(rjvVal) :
                                      SortObject<SortAscending>(rjvVal); break;
      // Unacceptable
      default: XC("Not an array or object!",
        "Name", NameGet(), "Type", rjvVal.GetType());
    }
  }
  /* ----------------------------------------------------------------------- */
  const Value &GetValue(const char*const cpKey) const
  { // Check to see if the member exists
    const Value::ConstMemberIterator vcmiIt{ FindMember(cpKey) };
    if(vcmiIt == MemberEnd())
      XC("Member not found!", "Name", NameGet(), "Key", cpKey);
    // Return the value
    return vcmiIt->value;
  }
  /* ----------------------------------------------------------------------- */
  double GetNumber(const char*const cpKey) const
  { // Get and check the value
    const Value &rjvValue = GetValue(cpKey);
    if(!rjvValue.IsNumber())
      XC("Invalid integer type!", "Name", NameGet(), "Key", cpKey);
    // Return the integer
    return rjvValue.GetDouble();
  }
  /* ----------------------------------------------------------------------- */
  StdString GetString(const char*const cpKey) const
  { // Get and check the value
    const Value &rjvValue = GetValue(cpKey);
    if(!rjvValue.IsString())
      XC("Invalid string type!", "Name", NameGet(), "Key", cpKey);
    // Return the integer
    return rjvValue.GetString();
  }
  /* ----------------------------------------------------------------------- */
  bool GetBoolean(const char*const cpKey) const
  { // Get and check the value
    const Value &rjvValue = GetValue(cpKey);
    if(!rjvValue.IsBool())
      XC("Invalid boolean type!", "Name", NameGet(), "Key", cpKey);
    // Return the integer
    return rjvValue.GetBool();
  }
  /* ----------------------------------------------------------------------- */
  unsigned GetInteger(const char*const cpKey) const
  { // Get and check the value
    const Value &rjvValue = GetValue(cpKey);
    if(!rjvValue.IsUint())
      XC("Invalid number type!", "Name", NameGet(), "Key", cpKey);
    // Return the integer
    return rjvValue.GetUint();
  }
  /* ----------------------------------------------------------------------- */
  using RJCompactWriter = Writer<StringBuffer, UTF8<>, UTF8<>>;
  using RJPrettyWriter = PrettyWriter<StringBuffer, UTF8<>, UTF8<>>;
  /* ----------------------------------------------------------------------- */
  template<typename WriterType>const StdString ToString() const
  { // Output buffer
    StringBuffer rsbOut;
    WriterType rwWriter{ rsbOut };
    Accept(rwWriter);
    return { rsbOut.GetString(), rsbOut.GetSize() };
  }
  /* ----------------------------------------------------------------------- */
  template<typename WriterType>int ToFile(const StdStringView &ssvFile) const
    { return FStream{ ssvFile, FM_W_T }.
        FStreamWriteStringSafe(ToString<WriterType>()) ? 0 : StdGetError(); }
  /* -- Default constructor ------------------------------------------------ */
  Json() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperJson{ cJsons },            // Initialise collector
    SerialSlave{ cParent->Serial() },  // Initialise identification number
    AsyncLoaderJson{ this,             // Initialise async loader with this
      EMC_MP_JSON }                    // ...and the event id
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor from a filename ---------------------------------------- */
  explicit Json(const StdStringView &ssvFile) :
    /* -- Initialisers ----------------------------------------------------- */
    Json{}                             // Use default initialisers
    /* -- Initialise from file --------------------------------------------- */
    { SyncInitFileSafe(ssvFile); }
  /* -- Destructor that tries to recover on exception ---------------------- */
  DTORHELPER(~Json, AsyncCancel())
};/* -- End ---------------------------------------------------------------- */
CTOR_END_ASYNC_NOFUNCS(Jsons, Json, JSON, JSON) // Finish collector class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
