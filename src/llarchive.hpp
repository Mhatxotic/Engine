/* == LLARCHIVE.HPP ======================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Archive' namespace and methods for the guest to use in ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Archive
/* ------------------------------------------------------------------------- */
// ! The archive class allows the programmer to load 7-zip archives into the
// ! engine in which any file loaded by the engine may exist in these archives
// ! as well. Although those files that exist on disk will always override
// ! any archive based file which is configured by the 'ast_override' cvar.
// !
// ! Archives with the extension specified in the cvar 'ast_bundles' in the
// ! current working directory will automatically be loaded at startup which
// ! is ".adb" by default (not .7z).
// !
// ! The executable file on Win32 systems is automatically loaded at startup
// ! in addition so that a programmer can store all assets inside the
// ! executable for portable distribution.
// !
// ! Of course the LZMA library handles the opening of such files so any such
// ! format of .7z file will work fine. Although it is recommended not to use
// ! solid archives as reading these archives will be extremely slow. Also note
// ! that encrypted or split archives are not supported yet as these features
// ! are not handled by the LZMA library.
/* ========================================================================= */
namespace LLArchive {                  // Archive namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArchive::P;           using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Archive common helper classes                                       ## **
** ######################################################################### **
** -- Read Archive class argument ------------------------------------------ */
struct AgArchive : public ArClass<Archive> {
  explicit AgArchive(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Archive>(lS, iArg, *cArchives)}{} };
/* -- Create Archive class argument ---------------------------------------- */
struct AcArchive : public ArClass<Archive> {
  explicit AcArchive(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Archive>(lS, *cArchives)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Archive:* member functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Archive:Destroy
// ? Destroys the archive and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Archive>(lS, 1, *cArchives))
/* ========================================================================= */
// $ Archive:Dir
// > Id:integer=Zero-index id of the directory
// < Name:string=The filename of the directory inside the archive
// < UId:integer=The unique id of the directory inside the archive
// ? Returns the name and unique id of the directory in the archive
/* ------------------------------------------------------------------------- */
LLFUNC(Dir, 2,
  const AgArchive aArchive{lS, 1};
  const AgSizeTLGE aIndex{lS, 2, 0, aArchive().GetDirList().size()};
  const StrUIntMapConstIt &suimciDir = aArchive().GetDir(aIndex);
  LuaUtilPushVar(lS, suimciDir->first, suimciDir->second))
/* ========================================================================= */
// $ Archive:Dirs
// < Directories:integer=Total number of directories inside the archive
// ? Returns total number of the directories inside the archive.
/* ------------------------------------------------------------------------- */
LLFUNC(Dirs, 1, LuaUtilPushVar(lS, AgArchive{lS, 1}().GetDirList().size()))
/* ========================================================================= */
// $ Archive:DirList
// < Directories:Table=A list of directories inside the archive
// ? Returns all the directories inside the archive.
/* ------------------------------------------------------------------------- */
LLFUNC(DirList, 1, LuaUtilToTable(lS, AgArchive{lS, 1}().GetDirList()))
/* ========================================================================= */
// $ Archive:File
// < Total:integer=Zero-index id of the file
// > Name:string=The filename of the file inside the archive
// > Id:integer=The unique id of the file inside the archive
// ? Returns the name and unique id of the file in the archive
/* ------------------------------------------------------------------------- */
LLFUNC(File, 2,
  const AgArchive aArchive{lS, 1};
  const AgSizeTLGE aIndex{lS, 2, 0, aArchive().GetFileList().size()};
  const StrUIntMapConstIt &suimciFile = aArchive().GetFile(aIndex);
  LuaUtilPushVar(lS, suimciFile->first, suimciFile->second))
/* ========================================================================= */
// $ Archive:Files
// < Files:integer=Total number of files inside the archive
// ? Returns total number of the files inside the archive.
/* ------------------------------------------------------------------------- */
LLFUNC(Files, 1, LuaUtilPushVar(lS, AgArchive{lS, 1}().GetFileList().size()))
/* ========================================================================= */
// $ Archive:FileList
// < Directories:Table=A list of files inside the archive
// ? Returns all the files inside the archive.
/* ------------------------------------------------------------------------- */
LLFUNC(FileList, 1, LuaUtilToTable(lS, AgArchive{lS, 1}().GetFileList()))
/* ========================================================================= */
// $ Archive:Id
// < Id:integer=The id number of the Archive object.
// ? Returns the unique id of the Archive object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgArchive{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Archive:Name
// < Name:string=Name of the archive.
// ? Returns the name of the archive which was loaded.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgArchive{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Archive:Size
// > Id:integer=The zero-index unique-id of the file/dir in archive
// < Size:integer=The uncompressed size of the file/dir in the archive
// ? Returns the uncompressed size of the file
/* ------------------------------------------------------------------------- */
LLFUNC(Size, 1,
  const AgArchive aArchive{lS, 1};
  const AgSizeTLGE aIndex{lS, 2, 0, aArchive().GetTotal()};
  LuaUtilPushVar(lS, aArchive().GetSize(aIndex)))
/* ========================================================================= */
// $ Archive:Total
// < Total:integer=Total number of files and directories in archive
// ? Returns the total number of files and directories in archive
/* ------------------------------------------------------------------------- */
LLFUNC(Total, 1, LuaUtilPushVar(lS, AgArchive{lS, 1}().GetTotal()))
/* ========================================================================= **
** ######################################################################### **
** ## Archive:* member functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Archive:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Dir),   LLRSFUNC(Dirs),     LLRSFUNC(DirList),
  LLRSFUNC(File),    LLRSFUNC(Files), LLRSFUNC(FileList), LLRSFUNC(Id),
  LLRSFUNC(Name),    LLRSFUNC(Size),  LLRSFUNC(Total),
LLRSEND                                // Archive:* member functions end
/* ========================================================================= */
// $ Archive.Load
// > Filename:string=The filename of the archive to load
// < Handle:Archive=A handle to the loaded archive
// ? Loads the specified 7-zip archive. All subsequent top-level assets
// ? operations will operate with this archive included as well and
// ? all files in this archive will override any previously loaded archives.
// ? Files on disk will always override any archived files. Please note that
// ? you should NOT use solid archives or there could be extreme performance
// ? issues while extracting files from it, this is by LZMA design.
/* ------------------------------------------------------------------------- */
LLFUNC(Load, 1, const AgFilename aFilename{lS, 1};
  AcArchive{lS}().SyncInitFileSafe(aFilename))
/* ========================================================================= */
// $ Archive.LoadAsync
// > Filename:string=The filename of the archive to load
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the file is laoded
// ? Loads a 7-zip archive ogg file off the main thread. The callback functions
// ? send an argument to the archive object that was created. See
// ? Asset.Archive' for more notes about loading 7-zip files.
/* ------------------------------------------------------------------------- */
LLFUNC(LoadAsync, 0,
  LuaUtilCheckParams(lS, 4);
  const AgFilename aFilename{lS,1};
  LuaUtilCheckFunc(lS, 2, 3, 4);
  AcArchive{lS}().AsyncInitNone(lS, aFilename, "archivefile"))
/* ========================================================================= */
// $ Archive.WaitAsync
// ? Halts main-thread execution until all async archive events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 0, cArchives->WaitAsync())
/* ========================================================================= **
** ######################################################################### **
** ## Archive.* namespace functions structure                             ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Archive.* namespace functions begin
  LLRSFUNC(Load), LLRSFUNC(LoadAsync), LLRSFUNC(WaitAsync),
LLRSEND                                // Archive.* namespace functions end
/* ========================================================================= */
}                                      // End of Archive namespace
/* == EoF =========================================================== EoF == */
