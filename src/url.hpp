/* == URL.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## A parser for Uniform Resource Locator strings.                      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IUrl {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Class to break apart urls ============================================ */
struct Url                             // Members initially public
{ /* -- Public typedefs ---------------------------------------------------- */
  enum Result                          // Result codes
  {/* ---------------------------------------------------------------------- */
    R_GOOD,                            // Url is good
    R_TOOLONG,                         // Url is too long
    R_EMURL,                           // Empty URL specified
    R_EMSCHEME,                        // Empty scheme after processing
    R_EMHOSTUSERPASSPORT,              // Empty hostname/username/password/port
    R_EMUSERPASS,                      // Empty username and password
    R_EMUSER,                          // Empty username
    R_EMPASS,                          // Empty password
    R_EMHOSTPORT,                      // Empty hostname/port
    R_EMHOST,                          // Empty hostname
    R_EMPORT,                          // Empty port number
    R_NOSCHEME,                        // No scheme delimiter ':'
    R_INVSCHEME,                       // Invalid scheme (not ://)
    R_INVPORT,                         // Invalid port number (1-65535)
    R_UNKSCHEME,                       // Unknown scheme without port
    R_MAX                              // Maximum number of codes
  };/* -- Private variables --------------------------------------- */ private:
  Result           rResult;            // Result
  string           strScheme,          // 'http' or 'https'
                   strUsername,        // Username
                   strPassword,        // Password
                   strHost,            // Hostname
                   strResource,        // The request uri
                   strBookmark;        // Bookmark
  unsigned int     uiPort;             // Port number (1-65535)
  bool             bSecure;            // Connection would require SSL?
  /* -- Code setter and returner ------------------------------------------- */
  void SetCode(const Result rNResult) { rResult = rNResult; }
  /* -- Return data ------------------------------------------------ */ public:
  Result GetResult(void) const { return rResult; }
  const string &GetScheme(void) const { return strScheme; }
  const string &GetUsername(void) const { return strUsername; }
  const string &GetPassword(void) const { return strPassword; }
  const string &GetHost(void) const { return strHost; }
  const string &GetResource(void) const { return strResource; }
  const string &GetBookmark(void) const { return strBookmark; }
  unsigned int GetPort(void) const { return uiPort; }
  bool GetSecure(void) const { return bSecure; }
  /* -- Constructor -------------------------------------------------------- */
  explicit Url(const string &strUrl)
  { // Error if string is empty
    if(strUrl.empty()) { SetCode(R_EMURL); return; }
    // Error if URL is too long
    if(strUrl.size() > 2048) { SetCode(R_TOOLONG); return; }
    // Error if no scheme
    size_t stStart = 0, stEnd = strUrl.find(':');
    if(stEnd == StdNPos) { SetCode(R_NOSCHEME); return; }
    // Set scheme and error if empty
    strScheme = strUrl.substr(stStart, stEnd);
    if(strScheme.empty()) { SetCode(R_EMSCHEME); return; }
    // Error if scheme is invalid
    stStart = stEnd + 1;
    if(strUrl.substr(stStart, 2) != "//") { SetCode(R_INVSCHEME); return; }
    // Move past scheme and find the resource part
    stStart += 2;
    stEnd = strUrl.find('/', stStart);
    if(stEnd == StdNPos)
    { // Couldn't find it so the resource wasn't specified so assume the root
      stEnd = strUrl.size();
      strResource = "/";
    } // We got the resource
    else strResource = strUrl.substr(stEnd);
    // Extract entire part of authority, hostname and port
    string strAHP{ strUrl.substr(stStart, stEnd - stStart) };
    // Find authority delimiter and if we find it?
    size_t stAtPos = strAHP.find('@');
    if(stAtPos != StdNPos)
    { // Extract username and password
      const string strUserInfo{ strAHP.substr(0, stAtPos) };
      // Find username and password delimiter and if we find it?
      size_t stColonPos = strUserInfo.find(':');
      if(stColonPos != StdNPos)
      { // We have the username and password
        strUsername = strUserInfo.substr(0, stColonPos);
        strPassword = strUserInfo.substr(stColonPos + 1);
        // Error if password not specified
        if(strPassword.empty()) { SetCode(R_EMPASS); return; }
      } // Username and password delimiter not specified so just username
      else strUsername = strUserInfo;
      // Error if username not specified
      if(strUsername.empty()) { SetCode(R_EMUSER); return; }
      // We have the authority
      strAHP = strAHP.substr(stAtPos + 1);
    } // Find port delimiter in hostname and if we have it?
    size_t stColonPos = strAHP.find(':');
    if(stColonPos != StdNPos)
    { // We have the hostname
      strHost = strAHP.substr(0, stColonPos);
      // Extract the port number and error if empty
      const string strPort{ strAHP.substr(stColonPos + 1) };
      if(strPort.empty()) { SetCode(R_EMPORT); return; }
      // Convert to number and error if out of range
      uiPort = StrToNum<unsigned int>(strPort);
      if(uiPort == 0 || uiPort > 65535) { SetCode(R_INVPORT); return; }
    } // Port delimiter not found
    else
    { // We have the host
      strHost = strAHP;
      // But we need to guess the port
      if(strScheme == "http") { uiPort = 80; bSecure = false; }
      else if(strScheme == "https") { uiPort = 443; bSecure = true; }
      else { SetCode(R_UNKSCHEME); return; }
    } // Error if the host is not empty
    if(strHost.empty()) { SetCode(R_EMHOST); return; }
    // Find the bookmark delimiter and if we have it?
    size_t stHashPos = strResource.find('#');
    if(stHashPos != StdNPos)
    { // Extract the bookmark and truncate the resource string
      strBookmark = strResource.substr(stHashPos + 1);
      strResource.resize(stHashPos);
    } // Perfect
    SetCode(R_GOOD);
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
