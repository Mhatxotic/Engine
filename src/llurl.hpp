/* == LLURL.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Url' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Url
/* ------------------------------------------------------------------------- */
// ! This class gives the guest parsing and validation of Uniform Resource
// ! Locators (or URLs).
/* ========================================================================= */
namespace LLUrl {                      // Url namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IUrl::P;               using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Url common helper classes                                           ## **
** ######################################################################### **
** -- Read Url class argument ---------------------------------------------- */
struct AgUrl : public ArClass<Url> {
  explicit AgUrl(lua_State*const lS, const int iArg) :
    ArClass{LuaUtilGetClassRef<Url>(lS, iArg, cUrls)}{} };
/* -- Create Url class argument -------------------------------------------- */
struct AcUrl : public ArClass<Url> {
  explicit AcUrl(lua_State*const lS) :
    ArClass{LuaUtilClassCreateRef<Url>(lS, cUrls)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Url:* methods                                                       ## **
** ######################################################################### **
** ========================================================================= */
// $ Url:Destroy
// ? Destroys the bin and frees all the memory associated with it. The object
// ? will no longer be useable after this call and an error will be generated
// ? if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Url>(lS, cUrls))
/* ========================================================================= */
// $ Url:GetBookmark
// < String:string=The bookmark part of the url.
// ? Returns the bookmark part of the parsed url.
/* ------------------------------------------------------------------------- */
LLFUNC(GetBookmark, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().UrlGetBookmark()))
/* ========================================================================= */
// $ Url:GetFinal
// < Scheme:string=The final url.
// ? Returns the final part of the Url object after the parse.
/* ------------------------------------------------------------------------- */
LLFUNC(GetFinal, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().UrlGetUrl()))
/* ========================================================================= */
// $ Url:GetHost
// < String:string=The hostname part of the url.
// ? Returns the hostname part of the parsed url.
/* ------------------------------------------------------------------------- */
LLFUNC(GetHost, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().UrlGetHost()))
/* ========================================================================= */
// $ Url:GetId
// < Id:integer=The id number of the Url object.
// ? Returns the unique id of the Url object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().Serial()))
/* ========================================================================= */
// $ Url:GetParams
// < Params:table=The parameters as a table
// ? Returns the parameters of the parsed url.
/* ------------------------------------------------------------------------- */
LLFUNC(GetParams, 1, LuaUtilToTableEx(lS, AgUrl{lS, 1}()))
/* ========================================================================= */
// $ Url:GetPassword
// < Password:string=The password part of the url.
// ? Returns the password part of the Url object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetPassword, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().UrlGetPassword()))
/* ========================================================================= */
// $ Url:GetPort
// < Port:integer=The port number of the url.
// ? Returns the port part of the parsed url.
/* ------------------------------------------------------------------------- */
LLFUNC(GetPort, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().UrlGetPort()))
/* ========================================================================= */
// $ Url:GetResource
// < Resource:string=The resource of the url.
// ? Returns the resource part of the Url object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetResource, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().UrlGetResource()))
/* ========================================================================= */
// $ Url:GetResult
// < Code:integer=The result of the last parse.
// ? Returns the hostname part of the parsed url.
/* ------------------------------------------------------------------------- */
LLFUNC(GetResult, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().UrlGetResult()))
/* ========================================================================= */
// $ Url:GetScheme
// < Scheme:string=The scheme of the url.
// ? Returns the scheme part of the Url object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetScheme, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().UrlGetScheme()))
/* ========================================================================= */
// $ Url:GetUsername
// < Username:string=The username part of the url.
// ? Returns the username part of the Url object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetUsername, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().UrlGetUsername()))
/* ========================================================================= */
// $ Url:IsDestroyed
// < Destroyed:boolean=If the Url class is destroyed
// ? Returns if the Url class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(IsDestroyed, 1, LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cUrls)))
/* ========================================================================= */
// $ Url:IsSecure
// < Secure:boolean=Does the url define a secure url?
// ? Returns if the scheme would use SSL.
/* ------------------------------------------------------------------------- */
LLFUNC(IsSecure, 1, LuaUtilPushVar(lS, AgUrl{lS, 1}().UrlGetSecure()))
/* ========================================================================= */
// $ Url:Parse
// > URL:string=The url to parse.
// ? Reparses the specified address.
/* ------------------------------------------------------------------------- */
LLFUNC(Parse, 0, AgUrl{lS ,1}().UrlParse(AgNeString{lS, 2}(), 0))
/* ========================================================================= */
// $ Url:ParseEx
// > URL:string=The url to parse.
// > Mode:integer=Post processing requirement.
// ? Reparses the specified address with the specified reprocessing mode. The
// ? 'Post' argument refers to one of the values in the Url.CreateEx function.
/* ------------------------------------------------------------------------- */
LLFUNC(ParseEx, 0,
  const AgUrl aUrl{lS, 1};
  const AgNeString aUrlAddr{lS, 2};
  const AgUIntLG aMode{lS, 3, 0, 2};
  aUrl().UrlParse(aUrlAddr(), aMode))
/* ========================================================================= **
** ######################################################################### **
** ## Url:* methods structure                                             ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Url:* methods begin
  LLRSFUNC(Destroy),     LLRSFUNC(GetBookmark), LLRSFUNC(GetFinal),
  LLRSFUNC(GetHost),     LLRSFUNC(GetId),       LLRSFUNC(GetParams),
  LLRSFUNC(GetPassword), LLRSFUNC(GetPort),     LLRSFUNC(GetResource),
  LLRSFUNC(GetResult),   LLRSFUNC(GetScheme),   LLRSFUNC(GetUsername),
  LLRSFUNC(IsDestroyed), LLRSFUNC(IsSecure),    LLRSFUNC(Parse),
  LLRSFUNC(ParseEx),
LLRSEND                                // Url:* methods end
/* ========================================================================= **
** ######################################################################### **
** ## Url.* non-member functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Url.Blank
// < Count:Url=The unparsed url class.
// ? Returns an empty Url class which you will populate with the 'Url:Parse*'
// ? methods.
/* ------------------------------------------------------------------------- */
LLFUNC(Blank, 1, AcUrl{lS});           // cppcheck-suppress unusedScopedObject
/* ========================================================================= */
// $ Url.GetClasses
// < Count:integer=Total number of bins created.
// ? Returns the total number of bin classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(GetClasses, 1, LuaUtilPushVar(lS, cUrls->CollectorCount()))
/* ========================================================================= */
// $ Url.IsHost
// > Hostname:string=The hostname to check.
// < State:boolean=The hostname is valid or not.
// ? Returns if the specified hostname or IP address is valid.
/* ------------------------------------------------------------------------- */
LLFUNC(IsHost, 1, LuaUtilPushVar(lS, LuaUtilValidHostname(lS, 1)))
/* ========================================================================= */
// $ Url.Url
// > URL:string=The url to parse.
// < Count:Url=The parsed url class.
// ? Return an Url class and parses the address.
/* ------------------------------------------------------------------------- */
LLFUNC(Url, 1, AcUrl{lS}().UrlParse(AgNeString{lS, 1}(), 0))
/* ========================================================================= */
// $ Url.UrlEx
// > URL:string=The url to parse.
// > Mode:integer=Post processing requirement.
// < Count:Url=The parsed url class.
// ? Parses the specified url and returns all the information about it. The
// ? 'Post' argument refers to one of the following...
// ? [0] Don't modify parameters.
// ? [1] URL Encode parameters (URL passed is not properly URL encoded).
// ? [2] URL Decode parameters (URL passed is already properly encoded).
/* ------------------------------------------------------------------------- */
LLFUNC(UrlEx, 1,
  const AgNeString aUrl{lS, 1};
  const AgUIntLG aEncode{lS, 2, 0, 2};
  AcUrl{lS}().UrlParse(aUrl(), aEncode))
/* ========================================================================= **
** ######################################################################### **
** ## Url.* non-member functions structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Url.* non-member functions begin
  LLRSFUNC(Blank), LLRSFUNC(GetClasses), LLRSFUNC(IsHost),
  LLRSFUNC(Url),   LLRSFUNC(UrlEx),
LLRSEND                                // Url.* non-member functions end
/* ========================================================================= **
** ######################################################################### **
** ## File.* namespace constants                                          ## **
** ######################################################################### **
** ========================================================================= */
// @ Url.Codes
// < Codes:table=The result codes of the Parse operation.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Codes)                     // Beginning of parse result flags
  LLRSKTITEM(R_,GOOD),               LLRSKTITEM(R_,TOOLONG),
  LLRSKTITEM(R_,EMURL),              LLRSKTITEM(R_,EMSCHEME),
  LLRSKTITEM(R_,EMHOSTUSERPASSPORT), LLRSKTITEM(R_,EMUSERPASS),
  LLRSKTITEM(R_,EMUSER),             LLRSKTITEM(R_,EMPASS),
  LLRSKTITEM(R_,EMHOSTPORT),         LLRSKTITEM(R_,EMHOST),
  LLRSKTITEM(R_,EMPORT),             LLRSKTITEM(R_,NOSCHEME),
  LLRSKTITEM(R_,INVSCHEME),          LLRSKTITEM(R_,INVPORT),
  LLRSKTITEM(R_,UNKSCHEME),          LLRSKTITEM(R_,EMPARAMS),
  LLRSKTITEM(R_,STANDBY),
LLRSKTEND                              // End of parse result flags
/* ========================================================================= **
** ######################################################################### **
** ## Url.* namespace constants structure                                 ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Url.* namespace consts begin
  LLRSCONST(Codes),
LLRSCONSTEND                           // Url.* namespace consts end
/* ========================================================================= */
}                                      // End of Url namespace
/* == EoF =========================================================== EoF == */
