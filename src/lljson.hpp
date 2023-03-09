/* == LLJSON.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Json' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Json
/* ------------------------------------------------------------------------- */
// ! This allows the programmer to encode and decode Json objects.
/* ========================================================================= */
namespace LLJson {                     // Json namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IJson::P;              using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Json common helper classes                                          ## **
** ######################################################################### **
** -- Create Json class argument ------------------------------------------- */
struct AcJson : public ArClass<Json> {
  explicit AcJson(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Json>(lS, *cJsons)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Json:* member functions                                             ## **
** ######################################################################### **
** ========================================================================= */
// $ Json:Destroy
// ? Destroys the json object and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Json>(lS, 1, *cJsons))
/* ========================================================================= */
// $ Json:ToString
// < Data:string=Encoded JSON data
// ? Encodes the data inside the class to JSON string.
/* ------------------------------------------------------------------------- */
LLFUNC(ToString, 1,
  LuaUtilPushVar(lS, AgJson{lS, 1}().ToString<Json::RJCompactWriter>()))
/* ========================================================================= */
// $ Json:ToHRString
// < Data:string=Encoded JSON data
// ? Encodes the data inside the class to JSON string in human readable format.
/* ------------------------------------------------------------------------- */
LLFUNC(ToHRString, 1,
  LuaUtilPushVar(lS, AgJson{lS, 1}().ToString<Json::RJPrettyWriter>()))
/* ========================================================================= */
// $ Json:ToFile
// < Result:integer=Error number code returned (0 = success)
// > Filename:string=The filename to write to
// ? Dumps the entire json into the specified file.
/* ------------------------------------------------------------------------- */
LLFUNC(ToFile, 1,
  const AgJson aJson{lS, 1};
  const AgFilename aFilename{lS, 2};
  LuaUtilPushVar(lS, aJson().ToFile<Json::RJCompactWriter>(aFilename)))
/* ========================================================================= */
// $ Json:ToHRFile
// < Result:integer=Error number code returned (0 = success)
// > Filename:string=The filename to write to
// ? Dumps the entire json into the specified file in readable format.
/* ------------------------------------------------------------------------- */
LLFUNC(ToHRFile, 1,
  const AgJson aJson{lS, 1};
  const AgFilename aFilename{lS, 2};
  LuaUtilPushVar(lS, aJson().ToFile<Json::RJPrettyWriter>(aFilename)))
/* ========================================================================= */
// $ Json:ToTable
// < Result:table=The entire json scope converted to a lua table
// ? Dumps the entire contents of the JSON's current scope as a LUA table
/* ------------------------------------------------------------------------- */
LLFUNC(ToTable, 1, AgJson{lS, 1}().ToLuaTable(lS))
/* ========================================================================= */
// $ Json:Id
// < Id:integer=The id number of the Json object.
// ? Returns the unique id of the Json object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgJson{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Json:Name
// < Name:string=Name of the json.
// ? If this json was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgJson{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Json:Sort
// > Descending:bool=Descending (true) or ascending (false) flag.
// ? Sorts the entire json array.
/* ------------------------------------------------------------------------- */
LLFUNC(Sort, 1, AgJson{lS, 1}().Sort(AgBoolean{lS, 2}))
/* ========================================================================= **
** ######################################################################### **
** ## Json:* member functions structure                                   ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Json:* member functions begin
  LLRSFUNC(Destroy),    LLRSFUNC(Id),      LLRSFUNC(Name),
  LLRSFUNC(Sort),       LLRSFUNC(ToTable), LLRSFUNC(ToString),
  LLRSFUNC(ToHRString), LLRSFUNC(ToFile),  LLRSFUNC(ToHRFile),
LLRSEND                                // Json:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Json.* namespace functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Json.Count
// < Count:integer=Total number of jsons created.
// ? Returns the total number of json classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(Count, 1, LuaUtilPushVar(lS, cJsons->CollectorCount()))
/* ========================================================================= */
// $ Json.File
// > Filename:string=The filename of the json to load
// ? Decodes the specified string as JSON encoded. The level depth is limited
// ? to 255 due to limitations with LUA's hardcoded stack level.
/* ------------------------------------------------------------------------- */
LLFUNC(File, 1, const AgFilename aFilename{lS,1};
  AcJson{lS}().SyncInitFileSafe(aFilename))
/* ========================================================================= */
// $ Json.FileAsync
// > Filename:string=The filename of the json to load
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the JSON string is laoded
// ? Decodes the specified string as JSON encoded asynchronously.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, 0, AcJson{lS}().InitAsyncFile(lS))
/* ========================================================================= */
// $ Json.String
// > Code:string=The string of JSON encoded data to decode
// < Handle:Json=Handle to the Json object
// ? Decodes the specified string as JSON encoded. The level depth is limited
// ? to 255 due to limitations with LUA's hardcoded stack level.
/* ------------------------------------------------------------------------- */
LLFUNC(String, 1, AcJson{lS}().InitString(lS))
/* ========================================================================= */
// $ Json.StringAsync
// > Code:string=The string of JSON encoded data to decode
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the JSON string is laoded
// ? Decodes the specified string as JSON encoded asynchronously.
/* ------------------------------------------------------------------------- */
LLFUNC(StringAsync, 0, AcJson{lS}().InitAsyncString(lS))
/* ========================================================================= */
// $ Json.Table
// > Table:string=The string of JSON encoded data to decode
// < Handle:Json=Handle to the Json object
// ? Encodes the specified string as JSON encoded. The level depth is limited
// ? to 255 due to limitations with LUA's hardcoded stack level.
/* ------------------------------------------------------------------------- */
LLFUNC(Table, 1, AcJson{lS}().InitFromTable(lS))
/* ========================================================================= */
// $ Json.WaitAsync
// ? Halts main-thread execution until all json pcm events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 0, cJsons->WaitAsync())
/* ========================================================================= **
** ######################################################################### **
** ## Json.* namespace functions structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Json.* namespace functions begin
  LLRSFUNC(Count),     LLRSFUNC(File),        LLRSFUNC(FileAsync),
  LLRSFUNC(String),    LLRSFUNC(StringAsync), LLRSFUNC(Table),
  LLRSFUNC(WaitAsync),
LLRSEND                                // Json.* namespace functions end
/* ========================================================================= */
}                                      // End of Json namespace
/* == EoF =========================================================== EoF == */
