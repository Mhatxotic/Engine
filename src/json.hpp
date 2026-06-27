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
using namespace ILog::P;               using namespace ILuaBase::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace IMemory::P;
using namespace IName::P;              using namespace ISerial::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace Lib::RapidJson;
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
{ /* -- Build a human readable string when we know the value is a string --- */
  Value ToStrIsStr(lua_State*const lS, const int iIndex)
  { // Get string and length from LUA
    size_t stStr;
    const char*const cpStr = LuaBaseToLStr(lS, iIndex, stStr);
    // Return as a json string. Unfortunately, ALL strings from LUA are
    // volatile so we need to copy the string.
    return Value{ cpStr, static_cast<SizeType>(stStr), GetAllocator() };
  }
  /* -- Build a human readable string when we know the value not a string -- */
  Value ToStrNotStr(lua_State*const lS, const int iIndex)
  { // Not a string so we need to convert it to human readable
    LuaBaseToLStrTS(lS, iIndex);
    const int iSIndex = LuaBaseGetTop(lS);
    // Get string and length from LUA
    size_t stStr;
    const char*const cpStr = LuaBaseToLStr(lS, iSIndex, stStr);
    // Return as a json string. Unfortunately, ALL strings from LUA are
    // volatile so we need to copy the string.
    Value vStr{ cpStr, static_cast<SizeType>(stStr), GetAllocator() };
    // Remove the value we just created
    LuaBaseRemove(lS, iSIndex);
    // Return the value
    return vStr;
  }
  /* -- Handle type value -------------------------------------------------- */
  template<typename Value>
    static void ProcessValueType(lua_State*const lS, const Value &vValue)
  { // What type is the value?
    switch(vValue.GetType())
    { // Json entry is a number type?
      case kNumberType:
      { // Actually an integer or a number type?
        if(vValue.IsInt()) LuaBasePushInt(lS, vValue.GetInt());
        else LuaBasePushNum(lS, vValue.GetDouble());
        break;
      } // Json entry is a string type?
      case kStringType:
      { // Push as string
        LuaBasePushLStr(lS, vValue.GetString(), vValue.GetStringLength());
        break;
      } // Json entry is a boolean type?
      case kTrueType:
      { // Push as boolean
        LuaUtilPushTrue(lS);
        break;
      } // Json entry is a boolean type?
      case kFalseType:
      { // Push as boolean
        LuaUtilPushFalse(lS);
        break;
      } // Json entry is an array[] type?
      case kArrayType:
      { // Convert array to table
        ToTableArray(lS, vValue);
        break;
      } // Json entry is an object{} type?
      case kObjectType:
      { // Convert object to table
        ToTableObject(lS, vValue);
        break;
      } // Json entry is a null type?
      case kNullType: [[fallthrough]];
      // Unknown type?
      default: LuaBasePushNil(lS); break;
    }
  }
  /* -- Sort entire json array --------------------------------------------- */
  template<class SortType>void SortArray(Value &vArray)
  { // For each table item...
    for(Value &vIndice : vArray.GetArray())
    { // Search for and sort all sub-tables
      switch(vIndice.GetType())
      { // Indexed array
        case kArrayType: SortArray<SortType>(vIndice); break;
        // Key/value object
        case kObjectType: SortObject<SortType>(vIndice); break;
        // Don't care about other types
        default: continue;
      } // Next item
    } // End of array
  }
  /* -- Sort entire json object -------------------------------------------- */
  template<class SortType>void SortObject(Value &vObject)
  { // For each table item
    for(Value::Member &vmMember : vObject.GetObject())
    { // Search for and sort all sub-tables
      switch(vmMember.value.GetType())
      { // Indexed array
        case kArrayType: SortArray<SortType>(vmMember.value); break;
        // Key/value object
        case kObjectType: SortObject<SortType>(vmMember.value); break;
        // Don't care about other types
        default: continue;
      } // Next item
    } // Do the sort
    StdSort(par_unseq, vObject.MemberBegin(), vObject.MemberEnd(), SortType());
  }
  /* -- Convert LUA table to rapidjson::Value ------------------------------ */
  Value ParseTable(lua_State*const lS, const int iTIndex)
  { // Get size of table and if we have length then we need to create an array
    if(const lua_Integer liLen =
      UtilIntOrMax<lua_Integer>(LuaBaseRawLen(lS, iTIndex)))
    {  // Set this value is array
      Value rjvRoot{ kArrayType };
      // We need one more free item on the stack, leave empty if not
      if(!LuaBaseCheckStack(lS, UtilIntOrMax<int>(liLen))) return rjvRoot;
      // Until end of table
      for(lua_Integer lI = 1; lI <= liLen; ++lI)
      { // Get the value at the specified indice
        LuaBaseRawGetI(lS, iTIndex, lI);
        const int iIndex = LuaBaseGetTop(lS);
        // This is the value to add
        Value vValue;
        // Append value if a string. Lua will convert any valid numbered
        // string to a number if this is not checked before integral checks.
        // Test with: lexec Console.Write(Json.Table({1,2,'3'}):ToHRString());
        switch(LuaBaseType(lS, iIndex))
        { // Variable is a number?
          case LUA_TNUMBER:
          { // Is an integer?
            if(LuaBaseIsInt(lS, iIndex))
            { // Get and set the number as quad word
              const lua_Integer liValue = LuaBaseToInt(lS, iIndex);
              vValue.SetInt64(liValue);
            } // Is a number?
            else
            { // Get and set the number as double
              const lua_Number lnValue = LuaBaseToNum(lS, iIndex);
              vValue.SetDouble(lnValue);
            } // Done
            break;
          } // Lua variable is a boolean?
          case LUA_TBOOLEAN:
          { // Get and set the value as boolean
            const bool bValue = LuaBaseToBool(lS, iIndex);
            vValue.SetBool(bValue);
            break;
          } // Lua variable is a table?
          case LUA_TTABLE:
          { // Parse the table and push it into the Json array
            vValue = ParseTable(lS, iIndex);
            break;
          } // Lua variable is a string?
          case LUA_TSTRING:
          { // Set the new string
            vValue = ToStrIsStr(lS, iIndex);
            break;
          } // Any other type
          default:
          { // Convert to human readable type and put it in the Json array
            vValue = ToStrNotStr(lS, iIndex);
            break;
          }
        } // Push the value into the Json array
        rjvRoot.PushBack(vValue, GetAllocator());
        // Remove the indice value
        LuaBaseRemove(lS, iIndex);
      } // Return new object
      return rjvRoot;
    } // Set this value as object
    Value rjvRoot{ kObjectType };
    // We need two more free item on the stack, leave empty if not
    if(!LuaBaseCheckStack(lS, 2)) return rjvRoot;
    // Key and value indexes. We do another stack size query because initially
    // there could be a JSon object on the stack when called by the guest but
    // subsequent calls to nested tables won't duplicate that value.
    const int iKIndex = LuaBaseGetTop(lS) + 1, iVIndex = iKIndex + 1;
    // Walk through all the object members
    for(LuaBasePushNil(lS);
        LuaBaseNext(lS, iTIndex);
        LuaBaseRemove(lS, iVIndex))
    { // Get keyname. It will be a number if array size was zero but still has
      // array elements. We need to keep the keyname as is for lua_next();
      Value vKey{ LuaBaseType(lS, iKIndex) == LUA_TSTRING ?
                  ToStrIsStr(lS, iKIndex) :
                  ToStrNotStr(lS, iKIndex) };
      // What is the value type?
      switch(LuaBaseType(lS, iVIndex))
      { // Variable is a number?
        case LUA_TNUMBER:
        { // Is it actually an integer?
          if(LuaBaseIsInt(lS, iVIndex))
          { // Get the integer and write it to the array
            const lua_Integer liValue = LuaBaseToInt(lS, iVIndex);
            rjvRoot.AddMember(vKey, Value().SetInt64(liValue), GetAllocator());
          } // Actually a number?
          else
          { // Get the number and write it to the array
            const lua_Number lnValue = LuaBaseToNum(lS, iVIndex);
            rjvRoot.AddMember(vKey, lnValue, GetAllocator());
          } // Done
          break;
        } // Variable is a boolean?
        case LUA_TBOOLEAN:
        { // Get the boolean and write it to the array
          const bool bValue = LuaBaseToBool(lS, iVIndex);
          rjvRoot.AddMember(vKey, bValue, GetAllocator());
          // Done
          break;
        } // Variable is a table?
        case LUA_TTABLE:
        { // Recurse into the table
          rjvRoot.AddMember(vKey, ParseTable(lS, iVIndex), GetAllocator());
          break;
        } // Is strictly a string?
        case LUA_TSTRING:
        { // Recurse into the table
          rjvRoot.AddMember(vKey, ToStrIsStr(lS, iVIndex), GetAllocator());
          break;
        } // Unknown type?
        default:
        { // Make it human readable
          rjvRoot.AddMember(vKey, ToStrNotStr(lS, iVIndex), GetAllocator());
          break;
        }
      } // Test: lexec return Json.Table({a=1,b={a=1},c={nil,1}}):ToHRString();
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
    LuaUtilPushObject(lS, vObject.MemberCount());
    // We need two more free item on the stack, leave empty if not
    if(!LuaBaseCheckStack(lS, 2)) return;
    // Get index of object
    const int iOIndex = LuaBaseGetTop(lS);
    // For each table item
    for(const Value::Member &vmMember : vObject.GetObject())
    { // What type is the value?
      ProcessValueType(lS, vmMember.value);
      // Push key name
      LuaBaseSetField(lS, iOIndex, vmMember.name.GetString());
    }
  }
  /* -- Convert json value to lua array table and put it on stack ---------- */
  static void ToTableArray(lua_State*const lS, const Value &vArray)
  { // Create the table, we're creating a indexed/value array
    LuaUtilPushArray(lS, vArray.Size());
    // We need two more free items on the stack, leave empty if not
    if(vArray.Empty() || !LuaBaseCheckStack(lS, 2)) return;
    // Get index of array
    const int iAIndex = LuaBaseGetTop(lS);
    // Index id
    lua_Integer liId = 0;
    // For each table item
    for(const Value &vIndice : vArray.GetArray())
    { // Table index
      LuaBasePushInt(lS, ++liId);
      // What type is the value?
      ProcessValueType(lS, vIndice);
      // Push key pair as integer table
      LuaBaseRawSet(lS, iAIndex);
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
