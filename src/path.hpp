/* == PATH.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Contains the full parts of a path name.                             ## **
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
namespace IPath {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Path class ----------------------------------------------------------- */
struct Path                            // Contains parts of a filename
{ /* -- Public variables --------------------------------------------------- */
  const StdString  strDrive,           // Drive part of path
                   strDir,             // Directory part of path
                   strFile,            // Filename part of path
                   strExt,             // Extension part of path
                   strFileExt,         // Filename plus extension
                   strFull,            // Full path name
                   strLoc;             // Drive+path name without file name
  /* -- Initialise constructor --------------------------------------------- */
  Path(StdString &&strDriveNew, StdString &&strDirNew, StdString &&strFileNew,
    StdString &&strExtNew, StdString &&strFullNew) :
    /* -- Initialisers ----------------------------------------------------- */
    strDrive{ StdMove(strDriveNew) },  // Move drive string
    strDir{ StdMove(strDirNew) },      // Move directory string
    strFile{ StdMove(strFileNew) },    // Move file part string
    strExt{ strExtNew },               // Move file extension string
    strFileExt{ strFile + strExt },    // Build file name string with extension
    strFull{ StdMove(strFullNew) },    // Move full path string
    strLoc{ strDrive + strDir }        // Build full drive and directory string
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Move constructor --------------------------------------------------- */
  Path(Path &&pOther) :
    /* -- Initialisers ----------------------------------------------------- */
    strDrive{ StdMove(pOther.strDrive) },     // Move other drive string
    strDir{ StdMove(pOther.strDir) },         // Move other directory string
    strFile{ StdMove(pOther.strFile) },       // Move other file string
    strExt{ StdMove(pOther.strExt) },         // Move other extension string
    strFileExt{ StdMove(pOther.strFileExt) }, // Move other filename string
    strFull{ StdMove(pOther.strFull) },       // Move other fullname string
    strLoc{ StdMove(pOther.strLoc) }          // Move other full dir string
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
