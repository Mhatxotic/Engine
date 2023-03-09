/* == LLFILE.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'File' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % File
/* ------------------------------------------------------------------------- */
// ! The file class allows the programmer to read and write files from the
// ! game directory.
/* ========================================================================= */
namespace LLFile {                     // File namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IDir::P;
using namespace IFile::P;              using namespace IFStream::P;
using namespace IPSplit::P;            using namespace IStd::P;
using namespace IMemory::P;            using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## File common helper classes                                          ## **
** ######################################################################### **
** -- Read File class argument --------------------------------------------- */
struct AgFile : public ArClass<File> {
  explicit AgFile(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<File>(lS, iArg, *cFiles)}{} };
/* -- Create File class argument ------------------------------------------- */
struct AcFile : public ArClass<File> {
  explicit AcFile(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<File>(lS, *cFiles)}{} };
/* -- Other types ---------------------------------------------------------- */
typedef AgInteger<int64_t> AgInt64;
/* ========================================================================= **
** ######################################################################### **
** ## File:* member functions                                             ## **
** ######################################################################### **
** ========================================================================= */
// $ File:Close
// < Result:boolean=If file was closed
// ? Closes the file
/* ------------------------------------------------------------------------- */
LLFUNC(Close, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().FStreamCloseSafe()))
/* ========================================================================= */
// $ File:Destroy
// ? Closes and destroys the file stream and frees all the memory associated
// ? with it. The object will no longer be useable after this call and an
// ? error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<File>(lS, 1, *cFiles))
/* ========================================================================= */
// $ File:End
// < State:boolean=File is at EOF?
// ? Calls feof() and returns if the file pointer is at EOF.
/* ------------------------------------------------------------------------- */
LLFUNC(End, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().FStreamIsEOFSafe()))
/* ========================================================================= */
// $ File:Error
// < ErrNo:integer=Error code returned
// ? Returns the last recorded errno.
/* ------------------------------------------------------------------------- */
LLFUNC(Error, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().FStreamGetErrNo()))
/* ========================================================================= */
// $ File:ErrorStr
// < Error:string=Error string returned
// ? Returns the last recorded errno as a human readable string.
/* ------------------------------------------------------------------------- */
LLFUNC(ErrorStr, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().FStreamGetErrStr()))
/* ========================================================================= */
// $ File:FError
// < State:boolean=Error in stream?
// ? Returns the file stream encounted an error.
/* ------------------------------------------------------------------------- */
LLFUNC(FError, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().FStreamFErrorSafe()))
/* ========================================================================= */
// $ File:Flush
// < State:boolean=Succeeded?
// ? By default writing to a file stream is buffered. This calls fflush() and
// ? makes sure that any unwritten data to disk is written straight away.
/* ------------------------------------------------------------------------- */
LLFUNC(Flush, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().FStreamFlushSafe()))
/* ========================================================================= */
// $ File:Id
// < Id:integer=The id number of the File object.
// ? Returns the unique id of the File object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ File:Name
// < Name:string=Name of the file.
// ? Returns the name of the loaded file.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ File:Opened
// < State:boolean=Error in stream?
// ? Returns if the file was successfully opened.
/* ------------------------------------------------------------------------- */
LLFUNC(Opened, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().FStreamOpened()))
/* ========================================================================= */
// $ File:Read
// < Data:array=The data read.
// > Bytes:integer=Maximum number of bytes to read.
// ? Uses fread() to read the specified number of bytes from the stream. The
// ? data is returned as an array class.
/* ------------------------------------------------------------------------- */
LLFUNC(Read, 1,
  const AgFile aFile{lS, 1};
  const AgSizeT aBytes{lS, 2};
  AcAsset{lS}().MemSwap({ aFile().FStreamReadBlockSafe(aBytes) }))
/* ========================================================================= */
// $ File:ReadStr
// < Bytes:integer=Maximum number of bytes to read.
// > String:string=The string read
// ? Uses fgets() to read the specified line from the stream. The line ends
// ? at the specified maximum number of bytes to read or until it reaches an
// ? end-of-line character.
/* ------------------------------------------------------------------------- */
LLFUNC(ReadStr, 1,
  const AgFile aFile{lS, 1};
  const AgSizeT aBytes{lS, 2};
  LuaUtilPushVar(lS, aFile().FStreamReadStringSafe(aBytes)))
/* ========================================================================= */
// $ File:Rewind
// ? Calls frewind() to sets the file stream pointer to position 0.
/* ------------------------------------------------------------------------- */
LLFUNC(Rewind, 0, AgFile{lS, 1}().FStreamRewindSafe())
/* ========================================================================= */
// $ File:Seek
// > Offset:integer=Offset.
// < State:boolean=Succeeded?
// ? Calls fseek() to set the file stream pointer to the specified position.
// ? from the start of the stream.
/* ------------------------------------------------------------------------- */
LLFUNC(Seek, 1,
  const AgFile aFile{lS, 1};
  const AgInt64 aOffset{lS, 2};
  LuaUtilPushVar(lS, aFile().FStreamSeekSafeSet(aOffset)))
/* ========================================================================= */
// $ File:SeekCur
// > Offset:integer=Offset.
// < State:boolean=Succeeded?
// ? Calls fseek() to adjust the file stream pointer from the current position
// ? of the stream.
/* ------------------------------------------------------------------------- */
LLFUNC(SeekCur, 1,
  const AgFile aFile{lS, 1};
  const AgInt64 aOffset{lS, 2};
  LuaUtilPushVar(lS, aFile().FStreamSeekSafeCur(aOffset)))
/* ========================================================================= */
// $ File:SeekEnd
// > Offset:integer=Offset.
// < State:boolean=Succeeded?
// ? Calls fseek() to set the file stream pointer to the specified position.
// ? from the end of the stream.
/* ------------------------------------------------------------------------- */
LLFUNC(SeekEnd, 1,
  const AgFile aFile{lS, 1};
  const AgInt64 aOffset{lS, 2};
  LuaUtilPushVar(lS, aFile().FStreamSeekSafeEnd(aOffset)))
/* ========================================================================= */
// $ File:Size
// < Size:integer=Size in bytes
// ? Returns the current sie of the file
/* ------------------------------------------------------------------------- */
LLFUNC(Size, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().FStreamSizeSafe()))
/* ========================================================================= */
// $ File:Tell
// < Position:integer=Position
// ? Returns the current position in file
/* ------------------------------------------------------------------------- */
LLFUNC(Tell, 1, LuaUtilPushVar(lS, AgFile{lS, 1}().FStreamTellSafe()))
/* ========================================================================= */
// $ File:Write
// > Data:Asset=The data to write.
// < Count:integer=Number of bytes written.
// ? Uses fwrite() to write the specified data array to the specified file.
/* ------------------------------------------------------------------------- */
LLFUNC(Write, 1,
  const AgFile aFile{lS, 1};
  const AgAsset aAsset{lS,2};
  LuaUtilPushVar(lS, aFile().FStreamWriteBlockSafe(aAsset)))
/* ========================================================================= */
// $ File:WriteStr
// > String:string=Text to write.
// < Count:integer=Number of bytes written
// ? Writes the specified string to the file.
/* ------------------------------------------------------------------------- */
LLFUNC(WriteStr, 1,
  const AgFile aFile{lS, 1};
  const AgLCString aString{lS, 2};
  LuaUtilPushVar(lS, aFile().FStreamWrite(aString.cpD, aString.stB)))
/* ========================================================================= **
** ######################################################################### **
** ## File:* member functions structure                                   ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // File:* member functions begin
  LLRSFUNC(Close),    LLRSFUNC(Destroy), LLRSFUNC(End),     LLRSFUNC(Error),
  LLRSFUNC(ErrorStr), LLRSFUNC(FError),  LLRSFUNC(Flush),   LLRSFUNC(Id),
  LLRSFUNC(Name),     LLRSFUNC(Opened),  LLRSFUNC(Read),    LLRSFUNC(ReadStr),
  LLRSFUNC(Rewind),   LLRSFUNC(Seek),    LLRSFUNC(SeekCur), LLRSFUNC(SeekEnd),
  LLRSFUNC(Size),     LLRSFUNC(Tell),    LLRSFUNC(Write),   LLRSFUNC(WriteStr),
LLRSEND                                // File:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## File:* namespace functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ File.Open
// > Filename:string=File to open or create.
// > Mode:integer=The mode id to use.
// < Handle:File=The handle to the file stream created.
// ? Uses fopen() to open or create a file stream for reading or writing. Note
// ? that this function will only allow access to files from the executable
// ? directory and it's children for security reasons.
/* ------------------------------------------------------------------------- */
LLFUNC(Open, 1,
  const AgFilename aFilename{lS, 1};
  const AgIntegerLGE<FStreamMode> aMode{lS, 2, FStreamMode(0), FM_MAX};
  AcFile{lS}().FStreamOpen(aFilename, aMode))
/* ========================================================================= */
// $ File.MkDir
// > Directory:string=Directory to create
// < Result:Integer=The errno return code from the C-Lib mkdir() function
// ? Creates the specified directory. Only allowed as a child of the working
// ? executable directory.
// * ----------------------------------------------------------------------- */
LLFUNC(MkDir, 1, LuaUtilPushVar(lS, DirMkDir(AgFilename{lS, 1}) ? 0 : errno))
/* ========================================================================= */
// $ File.MkDirEx
// > Directory:string=Directory to create
// < Result:Integer=The errno return code from the C-Lib mkdir() function
// ? Creates the specified directory. Only allowed as a child of the working
// ? executable directory. This creates interim directories that do not already
// ? exist. If the directory already exists than the function returns success.
// * ----------------------------------------------------------------------- */
LLFUNC(MkDirEx, 1,
  LuaUtilPushVar(lS, DirMkDirEx(AgFilename{lS, 1}) ? 0 : errno))
/* ========================================================================= */
// $ File.RmDir
// > Directory:string=Directory to create
// < Result:Integer=The errno return code from the C-Lib mkdir() function
// ? Creates the specified directory. Only allowed as a child of the working
// ? executable directory.
/* ------------------------------------------------------------------------- */
LLFUNC(RmDir, 1, LuaUtilPushVar(lS, DirRmDir(AgFilename{lS, 1}) ? 0 : errno))
/* ========================================================================= */
// $ File.RmDirEx
// > Directory:string=Directories to remove
// < Result:Integer=The errno return code from the C-Lib mkdir() function
// ? Removes the specified directory and interim directories. Only allowed as a
// ? child of the working executable directory. This removes interim
// ? directories that are empty.
/* ------------------------------------------------------------------------- */
LLFUNC(RmDirEx, 1,
  LuaUtilPushVar(lS, DirRmDirEx(AgFilename{lS, 1}) ? 0 : errno))
/* ========================================================================= */
// $ File.Unlink
// > File:string=File to delete
// < Result:Integer=The errno return code from the C-Lib unlink() function
// ? Deletes the specified file. Only allowed as a child of the working
// ? executable directory.
/* ------------------------------------------------------------------------- */
LLFUNC(Unlink, 1,
  LuaUtilPushVar(lS, DirFileUnlink(AgFilename{lS, 1}) ? 0 : errno))
/* ========================================================================= */
// $ File.Rename
// > Source:string=Source filename
// > Dest:string=Destination filename
// < Result:Integer=The errno return code from the C-Lib unlink() function
// ? Renames the specified file or directory. Only allowed as a child of the
// ? working executable directory for both parameters.
// * ----------------------------------------------------------------------- */
LLFUNC(Rename, 1,
  const AgFilename aSource{lS, 1};
  const AgFilename aDestination{lS,2};
  LuaUtilPushVar(lS, DirFileRename(aSource, aDestination)))
/* ========================================================================= */
// $ File.DirExists
// > Source:string=Directory
// < Result:Boolean=Directory exists?
// ? Returns if the specified directory exists and is actually a directory.
/* ------------------------------------------------------------------------- */
LLFUNC(DirExists, 1,
  LuaUtilPushVar(lS, DirLocalDirExists(AgFilename{lS, 1})))
/* ========================================================================= */
// $ File.FileExists
// > Source:string=Filename
// < Result:Boolean=File exists?
// ? Returns if the specified directory exists and is actually a directory.
/* ------------------------------------------------------------------------- */
LLFUNC(FileExists, 1,
  LuaUtilPushVar(lS, DirLocalFileExists(AgFilename{lS, 1})))
/* ========================================================================= */
// $ File.Executable
// > Source:string=Filename
// < Result:Boolean=File executable?
// ? Returns if the file on disk is executable. Only allowed as a child of the
// ? working executable directory. Should always return 'false' on Windows.
/* ------------------------------------------------------------------------- */
LLFUNC(Executable, 1,
  LuaUtilPushVar(lS, DirIsFileExecutable(AgFilename{lS, 1})))
/* ========================================================================= */
// $ File.Exists
// > Source:string=File name or directory name
// < Result:Boolean=File or directory exists?
// ? Returns if the specified file or directory exists on disk.
/* ------------------------------------------------------------------------- */
LLFUNC(Exists, 1,
  LuaUtilPushVar(lS, DirLocalResourceExists(AgFilename{lS, 1})))
/* ========================================================================= */
// $ File.Readable
// > Source:string=Filename
// < Result:Boolean=File readable?
// ? Returns if the file on disk is readable. Only allowed as a child of the
// ? working executable directory.
/* ------------------------------------------------------------------------- */
LLFUNC(Readable, 1, LuaUtilPushVar(lS, DirIsFileReadable(AgFilename{lS, 1})))
/* ========================================================================= */
// $ File.Writable
// > Source:string=Filename
// < Result:Boolean=File writable?
// ? Returns if the file on disk is writable. Only allowed as a child of the
// ? working executable directory.
/* ------------------------------------------------------------------------- */
LLFUNC(Writable, 1, LuaUtilPushVar(lS, DirIsFileWritable(AgFilename{lS, 1})))
/* ========================================================================= */
// $ File.ReadWritable
// > Source:string=Filename
// < Result:Boolean=File readable and writable?
// ? Returns if the file on disk is readable and writable. Only allowed as a
// ? child of the working executable directory.
/* ------------------------------------------------------------------------- */
LLFUNC(ReadWritable, 1,
  LuaUtilPushVar(lS, DirIsFileReadWriteable(AgFilename{lS, 1})))
/* ========================================================================= */
// $ File.Info
// > Source:string=Filename
// < Result:boolean=Error number result
// < Size:integer=File size
// < Mode:integer=File attributes
// < Timestamp:integer=File creation time
// < Timestamp:integer=File modification time
// < Timestamp:integer=File access time
// < Links:integer=Number of hard links in the file
// < UserId:integer=User id number
// < GroupId:integer=Group id number
// < Device:integer=Device id
// < RDevice:integer=Device id of special file
// < INode:integer=Inode id
// ? Returns information about a file without opening it.
/* ------------------------------------------------------------------------- */
LLFUNC(Info, 12,
  StdFStatStruct stData;
  LuaUtilPushVar(lS, DirFileSize(AgFilename{lS, 1}, stData), stData.st_size,
    stData.st_mode, stData.st_ctime, stData.st_mtime, stData.st_atime,
    stData.st_nlink, stData.st_uid, stData.st_gid, stData.st_dev,
    stData.st_rdev, stData.st_ino))
/* ========================================================================= */
// $ File.Parts
// > Source:string=Filename to break apart
// < Directory:string=The directory part
// < File:string=The filename part
// < Extension:string=The filename extension
// < Full:string=The fully qualified filename to the file
// ? Splits apart a filename and return its parts.
/* ------------------------------------------------------------------------- */
LLFUNC(Parts, 5,
  const PathSplit psData{ AgFilename{lS, 1} };
  LuaUtilPushVar(lS, psData.strDir, psData.strFile, psData.strExt,
    psData.strFileExt, psData.strFull))
/* ========================================================================= */
// $ File.Enumerate
// > Source:string=Directory to enumerate
// < Files:table=File list in a indexed table
// < Directories:table=Directory list in a indexed table
// ? Dumps all the files in the specified directory to two tables
/* ------------------------------------------------------------------------- */
LLFUNC(Enumerate, 2,
  const Dir dData{ AgFilename{lS, 1} };
  LuaUtilToTable(lS, dData.GetFiles());
  LuaUtilToTable(lS, dData.GetDirs()))
/* ========================================================================= */
// $ File.EnumerateEx
// > Source:string=Directory to enumerate
// > Extension:string=Extension to filter
// < Files:table=File list in a indexed table
// < Directories:table=Directory list in a indexed table
// ? Dumps all the files ending in the specified extension in the specified
// ? directory to two tables.
/* ------------------------------------------------------------------------- */
LLFUNC(EnumerateEx, 2,
  const AgFilename aDirectory{lS, 1};
  const AgFilename aExtension{lS,2};
  const Dir dData{ aDirectory, aExtension };
  LuaUtilToTable(lS, dData.GetFiles());
  LuaUtilToTable(lS, dData.GetDirs()))
/* ========================================================================= */
// $ File.AppendOneStr
// > Source:string=Filename to append string to
// > String:string=The string to append
// < Result:multi=Result of operation
// < Reason:string=(Conditional) Error reason
// ? Appends the specified string to the specified file. Returns 'false' if
// ? failed or bytes written if succeeded. If 'Result' is false then the second
// ? argument 'Reason' returned is the error reason string.
/* ------------------------------------------------------------------------- */
LLFUNC(AppendOneStr, 2, FileAppendString(lS))
/* ========================================================================= */
// $ File.AppendOne
// > Source:string=Filename to append asset to
// > Data:Asset=The asset to write to the file
// < Result:multi=Result of operation
// < Reason:string=(Conditional) Error reason
// ? Appends the specified data to the specified file. Returns 'false' if
// ? failed or bytes written if succeeded. If 'Result' is false then the second
// ? argument 'Reason' returned is the error reason string.
/* ------------------------------------------------------------------------- */
LLFUNC(AppendOne, 2, FileAppendBlock(lS))
/* ========================================================================= */
// $ File.ReadOneStr
// > Source:string=Filename to read string from
// < Result:multi=Result of operation
// < Reason:string=(Conditional) Error reason
// ? Reads the specified string from the the file in text-mode only. Please use
// ? Asset.String(Async) to read a file in binary form. Returns 'false' if
// ? failed or the string that was read if succeeded. If 'Result' is false then
// ? the second argument 'Reason' returned is the error reason string.
/* ------------------------------------------------------------------------- */
LLFUNC(ReadOneStr, 2, FileReadString(lS))
/* ========================================================================= */
// $ File.WriteOne
// > Source:string=Filename to append asset to
// > Data:Asset=The binary data from memory to write
// < Result:multi=Result of operation
// < Reason:string=(Conditional) Error reason
// ? Create or open existing file, and write the specified asset and closes
// ? the file. Returns the number of bytes written (integer) if succeeded or
// ? 'false' (boolean) if fails, and sets 'Reason' as the error reason.
// ------------------------------------------------------------------------- */
LLFUNC(WriteOne, 2, FileWriteBlock(lS))
/* ========================================================================= */
// $ File.WriteOneStr
// > Source:string=Filename to write string to
// > String:string=The string to write.
// < Result:multi=Result of operation
// < Reason:string=(Conditional) Error reason
// ? Opens/Creates the specified file, writes the specified string and closes
// ? the file. Returns the number of bytes written (integer) if succeeded or
// ? 'false' (boolean) if fails, and sets 'Reason' as the error reason.
/* ------------------------------------------------------------------------- */
LLFUNC(WriteOneStr, 2, FileWriteString(lS))
/* ========================================================================= */
// $ File.ValidName
// > Source:string=Filename to check
// < Result:integer=The result of the check
// ? Returns 0 if the filename is valid, non-zero if not.
/* ------------------------------------------------------------------------- */
LLFUNC(ValidName, 1, LuaUtilPushVar(lS, DirValidName(AgString{lS, 1})))
/* ========================================================================= **
** ######################################################################### **
** ## File.* namespace functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // File.* namespace functions begin
  LLRSFUNC(AppendOne),    LLRSFUNC(AppendOneStr), LLRSFUNC(DirExists),
  LLRSFUNC(Enumerate),    LLRSFUNC(EnumerateEx),  LLRSFUNC(Executable),
  LLRSFUNC(Exists),       LLRSFUNC(FileExists),   LLRSFUNC(Info),
  LLRSFUNC(MkDir),        LLRSFUNC(MkDirEx),      LLRSFUNC(Open),
  LLRSFUNC(Parts),        LLRSFUNC(Readable),     LLRSFUNC(ReadOneStr),
  LLRSFUNC(ReadWritable), LLRSFUNC(Rename),       LLRSFUNC(RmDir),
  LLRSFUNC(RmDirEx),      LLRSFUNC(Unlink),       LLRSFUNC(ValidName),
  LLRSFUNC(Writable),     LLRSFUNC(WriteOne),     LLRSFUNC(WriteOneStr),
LLRSEND                                // File.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## File.* namespace constants                                          ## **
** ######################################################################### **
** ========================================================================= */
// @ File.Flags
// < Codes:table=The table of key/value pairs of available open flags
// ? A table containing all the possible file open modes available...
// ? ID    Keyname  FOpen  Operation
// ? ====  =======  =====  ====================================================
// ? [00]  R_T      "rt"   Read  + Exists + Text
// ? [01]  W_T      "wt"   Write + New    + Truncate + Text
// ? [02]  A_T      "at"   Write + Append + New      + Text
// ? [03]  R_P_T    "r+t"  Read  + Write  + Exists   + Text
// ? [04]  W_P_T    "w+t"  Read  + Write  + New      + Truncate + Text
// ? [05]  A_P_T    "a+t"  Read  + Write  + Append   + New      + Text
// ? [06]  R_B      "rb"   Read  + Exists + Binary
// ? [07]  W_B      "wb"   Write + New    + Truncate + Binary
// ? [08]  A_B      "ab"   Write + Append + New      + Binary
// ? [09]  R_P_B    "r+b"  Read  + Write  + Exists   + Binary
// ? [10]  W_P_B    "w+b"  Read  + Write  + New      + Truncate + Binary
// ? [11]  A_P_B    "a+b"  Read  + Append + New      + Binary
// ? ====  =======  =====  ====================================================
// ? Read     = File must have permission to read.
// ? Write    = File must have permission to write.
// ? Exists   = File must exist.
// ? New      = Create file if it does not exist.
// ? Truncate = Truncate size to zero.
// ? Append   = Place the file pointer at the end of file.
// ? Binary   = Open in binary mode with no line ending conversions.
// ? Text     = Open in text mode and convert to OS specific line endings.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of open mode flags
  LLRSKTITEM(FM_,R_T),   LLRSKTITEM(FM_,W_T),   LLRSKTITEM(FM_,A_T),
  LLRSKTITEM(FM_,R_P_T), LLRSKTITEM(FM_,W_P_T), LLRSKTITEM(FM_,A_P_T),
  LLRSKTITEM(FM_,R_B),   LLRSKTITEM(FM_,W_B),   LLRSKTITEM(FM_,A_B),
  LLRSKTITEM(FM_,R_P_B), LLRSKTITEM(FM_,W_P_B), LLRSKTITEM(FM_,A_P_B),
  LLRSKTITEM(FM_,MAX),
LLRSKTEND                              // End of open mode flags
/* ========================================================================= **
** ######################################################################### **
** ## File.* namespace constants structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // File.* namespace consts begin
  LLRSCONST(Flags),
LLRSCONSTEND                           // File.* namespace consts end
/* ========================================================================= */
}                                      // End of File namespace
/* == EoF =========================================================== EoF == */
