/* == PSPLIT.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a simple C++ wrapper for splitting components of a path     ## **
** ## name.                                                               ## **
** ######################################################################### **
** ## UNIX PATH LAYOUT                                                    ## **
** ######################################################################### **
** ## Path      ## MAX_PATH   #### /Directory/SubDirectory/File.Extension ## **
** ## Directory ## MAX_DIR    #### ^^^^^^^^^^^^^^^^^^^^^^^^-------------- ## **
** ## FileMap   ## MAX_FNAME  #### ------------------------^^^^---------- ## **
** ## Extension ## MAX_EXT    #### ----------------------------^^^^^^^^^^ ## **
** ######################################################################### **
** ## WIN32 PATH LAYOUT                                                   ## **
** ######################################################################### **
** ## Path      ## _MAX_PATH  ## C:\Directory\SubDirectory\File.Extension ## **
** ## Drive     ## _MAX_DRIVE ## ^^-------------------------------------- ## **
** ## Directory ## _MAX_DIR   ## --^^^^^^^^^^^^^^^^^^^^^^^^-------------- ## **
** ## FileMap   ## _MAX_FNAME ## --------------------------^^^^---------- ## **
** ## Extension ## _MAX_EXT   ## ------------------------------^^^^^^^^^^ ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IPSplit {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Convert back slashes to forward slashes ------------------------------ */
static string &PSplitBackToForwardSlashes(string &strText)
  { return StrReplace(strText, '\\', '/'); }
static string PSplitBackToForwardSlashes(const string &strIn)
  { string strOut{ strIn }; return PSplitBackToForwardSlashes(strOut); }
#if defined(WINDOWS)
static string PSplitBackToForwardSlashes(const wstring &wstrName)
  { return PSplitBackToForwardSlashes(WS16toUTF(wstrName)); }
#endif
/* -- FileParts class ------------------------------------------------------ */
class FileParts                        // Contains parts of a filename
{ /* -- Variables -------------------------------------------------- */ public:
  const string     strDrive,           // Drive part of path
                   strDir,             // Directory part of path
                   strFile,            // Filename part of path
                   strExt,             // Extension part of path
                   strFileExt,         // Filename plus extension
                   strFull,            // Full path name
                   strLoc;             // Drive+path name without file name
  /* -- Initialise constructor --------------------------------------------- */
  FileParts(string &&strDriveNew, string &&strDirNew, string &&strFileNew,
    string &&strExtNew, string &&strFullNew) :
    /* -- Initialisers ----------------------------------------------------- */
    strDrive{ StdMove(strDriveNew) },
    strDir{ StdMove(strDirNew) },
    strFile{ StdMove(strFileNew) },
    strExt{ strExtNew },
    strFileExt{ strFile+strExt },
    strFull{ StdMove(strFullNew) },
    strLoc{ strDrive+strDir }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- MOVE assign constructor on class creation -------------------------- */
  FileParts(FileParts &&pspOther) :
    /* -- Initialisers ----------------------------------------------------- */
    strDrive{ StdMove(pspOther.strDrive) },
    strDir{ StdMove(pspOther.strDir) },
    strFile{ StdMove(pspOther.strFile) },
    strExt{ StdMove(pspOther.strExt) },
    strFileExt{ StdMove(pspOther.strFileExt) },
    strFull{ StdMove(pspOther.strFull) },
    strLoc{ StdMove(pspOther.strLoc) }
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- PathSplit class ---------------------------------------------------- */
class PathSplit :
  /* -- Base classes ------------------------------------------------------- */
  public FileParts                     // File parts
{ /* -- Constructors ---------------------------------------------- */ private:
  FileParts Init(const string &strSrc, const bool bUseFullPath) const
  { // Windows?
#if defined(WINDOWS)
    // Convert UTF8 string to UNICODE string
    wstring wstrSrc{ UTFtoS16(strSrc) };
    // Build full path name and use requested pathname if not wanted or failed?
    StdResized<wstring> wstrFull{ _MAX_PATH };
    if(!bUseFullPath || !_wfullpath(const_cast<wchar_t*>(wstrFull.data()),
         wstrSrc.data(), _MAX_PATH + 1))
      wstrFull.assign(wstrSrc);
    // Succeeded? Resize the string
    else wstrFull.resize(wcslen(wstrFull.data()));
    // This is the final full path string so compact it
    wstrFull.shrink_to_fit();
    // Buffers for the path parts
    StdResized<wstring> wstrDrive{ _MAX_DRIVE }, wstrDir{ _MAX_DIR },
      wstrFile{ _MAX_FNAME }, wstrExt{ _MAX_EXT };
    // Split the executable path name into bits and if failed?
    _wsplitpath_s(const_cast<wchar_t*>(wstrFull.data()),
                  const_cast<wchar_t*>(wstrDrive.data()), _MAX_DRIVE + 1,
                  const_cast<wchar_t*>(wstrDir.data()), _MAX_DIR + 1,
                  const_cast<wchar_t*>(wstrFile.data()), _MAX_FNAME + 1,
                  const_cast<wchar_t*>(wstrExt.data()), _MAX_EXT + 1);
    // Finalise the strings because they're still set to the maximum size
    wstrDrive.resize(wcslen(wstrDrive.data()));
    wstrDrive.shrink_to_fit();
    wstrDir.resize(wcslen(wstrDir.data()));
    wstrDir.shrink_to_fit();
    wstrFile.resize(wcslen(wstrFile.data()));
    wstrFile.shrink_to_fit();
    wstrExt.resize(wcslen(wstrExt.data()));
    wstrExt.shrink_to_fit();
    // Finalise strings and replace backslashes with forward slashes
    return { StdMove(PSplitBackToForwardSlashes(wstrDrive)),
             StdMove(PSplitBackToForwardSlashes(wstrDir)),
             StdMove(PSplitBackToForwardSlashes(wstrFile)),
             StdMove(PSplitBackToForwardSlashes(wstrExt)),
             StdMove(PSplitBackToForwardSlashes(wstrFull)) };
    // Unix?
#else
    // If a full path name build is re4quested? Set original string
    StdResized<string> strFull{ _MAX_PATH };
    if(!bUseFullPath || !realpath(const_cast<char*>(strSrc.data()),
      const_cast<char*>(strFull.data())))
        strFull.assign(strSrc);
    // Succeeded? Resize the string
    else strFull.resize(strlen(strFull.data()));
    // This is the final full path string so compact it
    strFull.shrink_to_fit();
    // Copy string and grab the directory part because linux version of
    // dirname() MODIFIES the original argument. We use memcpy because we
    // Don't want to resize the string. Also let us be careful of how many
    // bytes we should copy. Copy the lowest allocated string
    StdResized<string> strDir{ _MAX_DIR };
    strlcpy(const_cast<char*>(strDir.data()), strFull.data(), _MAX_DIR);
    if(const char*const cpDir = dirname(const_cast<char*>(strDir.data())))
    { // If the directory is not just a dot (current dir)?
      if(cpDir[0] != '.' || cpDir[1] != '\0')
      { // If the pointer is not the same as our string? Copy it.
        if(cpDir != strDir.data()) strDir.assign(cpDir);
        // Finalise the size of the original directory name
        else strDir.resize(strlen(strDir.data()));
        // Finalise directory and append slash if there is not one to match how
        // Win32's splitpath works which is better really.
        if(strDir.back() != '/')
          strDir.append(cCommon->CommonFSlash());
      } // We're not interested in pointlessly prepending the current dir
      else strDir.clear();
    } // Fiailed so clear the directory name
    else strDir.clear();
    // This is the final directory string so compact it
    strDir.shrink_to_fit();
    // Prepare filename. Again basename() can modify the argument on linux.
    StdResized<string> strFile{ _MAX_FNAME };
    strlcpy(const_cast<char*>(strFile.data()), strFull.data(), _MAX_FNAME);
    if(const char*const cpFile = basename(const_cast<char*>(strFull.data())))
    { // If the pointer is not the same as our string? Copy it.
      if(cpFile != strFile.data()) strFile.assign(cpFile);
      // Finalise the size of the original file name
      else strFile.resize(strlen(strFile.data()));
    } // Failed so clear the filename
    else strFile.clear();
    // Prepare extension and save extension and if found?
    const size_t stSlashPos = strFile.find_last_of('/'),
    stDotPos = StrFindCharBackwards(strFile, strFile.length() - 1,
      stSlashPos != StdNPos ? (stSlashPos + 1) : 0, '.');
    StdResized<string> strExt{ _MAX_EXT };
    if(stDotPos != StdNPos)
    { // Update filename
      strExt.assign(strFile.substr(stDotPos));
      strFile.assign(strFile.substr(stSlashPos != StdNPos ?
        stSlashPos : 0, stDotPos));
    } // No extension so clear memory
    else strExt.clear();
    // This is the final file and extension string so compact them
    strFile.shrink_to_fit();
    strExt.shrink_to_fit();
    // Return parts. There is no drive on unix systems
    return { {}, StdMove(strDir), StdMove(strFile),
             StdMove(strExt), StdMove(strFull) };
#endif
  }
  /* -- Constructors with initialisation --------------------------- */ public:
  explicit PathSplit(const string &strSrc, const bool bUseFullPath=false) :
    /* -- Initialisers ----------------------------------------------------- */
    FileParts{ Init(strSrc, bUseFullPath) }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- MOVE assign constructor on class creation -------------------------- */
  PathSplit(PathSplit &&psOther) :
    /* -- Initialisers ----------------------------------------------------- */
    FileParts{ StdMove(psOther) }
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
