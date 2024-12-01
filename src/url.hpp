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
  { // Error if url is empty
    if(strUrl.empty()) { rResult = R_EMURL; return; }
    // Error if url is too long
    if(strUrl.size() > 2048) { rResult = R_TOOLONG; return; }
    // Find scheme and throw if error
    const size_t stSchemePos = strUrl.find(':');
    if(stSchemePos == string::npos) { rResult = R_NOSCHEME; return; }
    for(size_t stPos = stSchemePos + 1, stPosEnd = stPos + 1;
               stPos < stPosEnd;
             ++stPos)
      if(strUrl[stPos] != cCommon->CFSlash())
        { rResult = R_INVSCHEME; return; }
    // Copy the scheme part and throw error if empty string
    strScheme = strUrl.substr(0, stSchemePos);
    if(strScheme.empty()) { rResult = R_EMSCHEME; return; }
    // Find hostname and if we couldn't find it?
    const size_t stLocStartPos = stSchemePos + 3,
                 stHostPos = strUrl.find(cCommon->CFSlash(), stLocStartPos);
    if(stHostPos == string::npos)
    { // Set default resource to root
      strResource = cCommon->CFSlash();
      // Finalise hostname
      strHost = strUrl.substr(stLocStartPos);
    } // Request not found
    else
    { // Copy request part of url
      strResource = strUrl.substr(stHostPos);
      // Copy host part of url
      strHost = strUrl.substr(stLocStartPos, stHostPos-stLocStartPos);
    } // Check hostname not empty
    if(strHost.empty()) { rResult = R_EMHOSTUSERPASSPORT; return; }
    // Find username and password in hostname
    const size_t stUserPos = strHost.find('@');
    if(stUserPos != string::npos)
    { // Get username and password and error if empty
      strUsername = strHost.substr(0, stUserPos);
      if(strUsername.empty()) { rResult = R_EMUSERPASS; return; }
      // Look for password delimiter and if specified?
      const size_t stPassPos = strUsername.find(':');
      if(stPassPos != string::npos)
      { // Extract password
        strPassword = strUsername.substr(stPassPos + 1);
        if(strPassword.empty()) { rResult = R_EMPASS; return; }
        // Truncate password from username
        strUsername.resize(stPassPos);
      } // Check that username is valid
      if(strUsername.empty()) { rResult = R_EMUSER; return; }
      // Truncate username and password from hostname and make sure not empty
      strHost.erase(strHost.begin(), strHost.begin() +
        static_cast<ssize_t>(stUserPos) + 1);
      if(strHost.empty()) { rResult = R_EMHOSTPORT; return; }
    } // Find port in hostname
    const size_t stPortPos = strHost.find(':');
    if(stPortPos != string::npos)
    { // Get port part of string and throw error if empty string
      const string strPort{ strHost.substr(stPortPos + 1) };
      if(strPort.empty()) { rResult = R_EMPORT; return; }
      // Convert port to number and throw error if invalid
      uiPort = StrToNum<unsigned int>(strPort);
      if(!uiPort || uiPort > 65535) { rResult = R_INVPORT; return; }
      // We have to find the port so set insecure port 80 if scheme is http
      if(strScheme == "http") bSecure = false;
      // Set 443 if scheme is secure https
      else if(strScheme == "https") bSecure = true;
      // We don't know what the port is
      else { rResult = R_UNKSCHEME; return; }
      // Copy host part without port
      strHost.resize(stPortPos);
      if(strHost.empty()) { rResult = R_EMHOST; return; }
    } // Port number not found?
    else
    { // We have to find the port so set insecure port 80 if scheme is http
      if(strScheme == "http") { uiPort = 80; bSecure = false; }
      // Set 443 if scheme is secure https
      else if(strScheme == "https") { uiPort = 443; bSecure = true; }
      // We don't know what the port is
      else { rResult = R_UNKSCHEME; return; }
    } // Find bookmark and if there is one?
    const size_t stBookmarkPos = strResource.find('#');
    if(stBookmarkPos != string::npos)
    { // Copy in bookmark string
      strBookmark = strResource.substr(stBookmarkPos + 1);
      // Trim resource
      strResource.resize(stBookmarkPos);
    } // Good result
    rResult = R_GOOD;
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
