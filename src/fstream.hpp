/* == FSTREAM.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module is a simple C++ wrapper for C file stream functions.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFStream {                   // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICommon::P;            using namespace IError::P;
using namespace IIdent::P;             using namespace IStd::P;
using namespace IString::P;            using namespace IMemory::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Typedefs ------------------------------------------------------------- */
enum FStreamMode : size_t              // Open types allowed
{ /* ----------------------------------------------------------------------- */
  FM_R_T,                              // [00] "rt"  Read|Exists|Text
  FM_W_T,                              // [01] "wt"  Write|New|Trunc|Text
  FM_A_T,                              // [02] "at"  Write|Append|New|Text
  FM_R_P_T,                            // [03] "r+t" Read|Write|Exists|Text
  FM_W_P_T,                            // [04] "w+t" Read|Write|New|Trunc|Text
  FM_A_P_T,                            // [05] "a+t" Read|Write|Append|New|Text
  FM_R_B,                              // [06] "rb"  Read|Exists|Binary
  FM_W_B,                              // [07] "wb"  Write|New|Trunc|Binary
  FM_A_B,                              // [08] "ab"  Write|Append|New|Binary
  FM_R_P_B,                            // [09] "r+b" Read|Write|Exists|Binary
  FM_W_P_B,                            // [10] "w+b" Read|Write|New|Trunc|Binar
  FM_A_P_B,                            // [11] "a+b" Read|Append|New|Binary
  /* ----------------------------------------------------------------------- */
  FM_MAX                               // [12] Maximum number of modes
};/* -- FStream base class ------------------------------------------------- */
class FStreamBase :                    // File stream base class
  /* -- Base classes ------------------------------------------------------- */
  public Ident                         // Contains filename string
{ /* -- Private variables -------------------------------------------------- */
  FILE            *fStream;            // Stream handle
  int              iErrNo;             // Stored error number
  /* -- Accept a file stream from DoOpen() --------------------------------- */
  int FStreamDoAccept(const string &strFile, FILE*const fPtr)
  { // Close original file if opened
    FStreamCloseSafe();
    // Set the new handle and file name
    FStreamSetHandle(fPtr);
    IdentSet(strFile);
    // Success!
    return 0;
  }
  /* -- Error number wrapper handle ---------------------------------------- */
  template<typename AnyType>AnyType FStreamErrNoWrapper(AnyType atVal)
    { iErrNo = errno; return atVal; }
  /* -- Open a file and return its handle ---------------------------------- */
  FILE *FStreamDoOpenDirect(const string &strFile, const FStreamMode fsmMode)
  { // Obviously Windows has to be different from everyone else!
#if defined(WINDOWS)                   // Using windows?
    // The mode supported (in unicode)
    static const array<const wchar_t*const,FM_MAX>cplModes{
      L"rt", L"wt", L"at", L"r+t", L"w+t", L"a+t",
      L"rb", L"wb", L"ab", L"r+b", L"w+b", L"a+b"
    };
    // Send it to the Windows file open call and accept it if successful
    return _wfsopen(UTFtoS16(strFile).data(), cplModes[fsmMode], _SH_DENYWR);
#else                                  // Using linux or MacOS?
    // The mode supported (in ansi string)
    static const array<const char*const,FM_MAX>cplModes{
      "rt", "wt", "at", "r+t", "w+t", "a+t",
      "rb", "wb", "ab", "r+b", "w+b", "a+b"
    };
    // Send it to the posix file open call and accept it if successful
    return fopen(strFile.data(), cplModes[fsmMode]);
#endif                                 // Operating system check
  }
  /* -- Check that already set members are valid --------------------------- */
  void FStreamDoCheckOpenDirect(const string &strF, const FStreamMode fsmMode)
  { // Return if the file is opened successfully
    if(FStreamOpened()) return;
    // Return if there is no home directory
    using namespace ICmdLine::P;
    if(cCmdLine->CmdLineIsNoHome()) return;
    // Try opened again from persist directory
    IdentSet(cCmdLine->CmdLineGetHome(strF));
    FStreamSetHandle(FStreamDoOpenDirect(IdentGet(), fsmMode));
  }
  /* -- Retrun true if internal stream or stream closed successfully ------- */
  bool FStreamDoClose()
    { return !FStreamErrNoWrapper(fclose(FStreamGetCtx())); }
  /* -- Clear file handle -------------------------------------------------- */
  void FStreamClearHandle() { FStreamSetHandle(nullptr); }
  /* -- Set handle ---------------------------------------------- */ protected:
  void FStreamSetHandle(FILE*const fpS) { fStream = fpS; }
  /* -- Return file stream handle ---------------------------------- */ public:
  FILE *FStreamGetCtx() const { return fStream; }
  /* -- Returns true if handle is internal --------------------------------- */
  bool FStreamIsHandleStd() const { return FStreamGetCtx() == stdin ||
                                           FStreamGetCtx() == stdout ||
                                           FStreamGetCtx() == stderr; }
  bool FStreamIsHandleNotStd() const { return !FStreamIsHandleStd(); }
  /* -- Get file stream descriptor ----------------------------------------- */
  int FStreamGetFd() const { return StdFileNo(FStreamGetCtx()); }
  int FStreamGetFdSafe() const
    { return FStreamOpened() ? FStreamGetFd() : EOF; }
  /* -- Swap stream handle and filename ------------------------------------ */
  void FStreamSwap(FStreamBase &fsOther)
    { swap(fStream, fsOther.fStream); IdentSwap(fsOther); }
  /* -- File is opened or closed?  ----------------------------------------- */
  bool FStreamOpened() const { return !!FStreamGetCtx(); }
  bool FStreamClosed() const { return !FStreamOpened(); }
  /* -- Return last error nuumber ------------------------------------------ */
  int FStreamGetErrNo() const { return iErrNo; }
  const string FStreamGetErrStr() const { return StrFromErrNo(iErrNo); }
  /* -- Return handle to stream -------------------------------------------- */
  int FStreamGetID() const { return StdFileNo(FStreamGetCtx()); }
  int FStreamGetIDSafe() const { return FStreamOpened() ? FStreamGetID() : 0; }
  /* -- Set current file position without check ---------------------------- */
  bool FStreamSeek(const int64_t qwPos, const int iMode)
    { return !FStreamErrNoWrapper(StdFSeek(FStreamGetCtx(), qwPos, iMode)); }
  bool FStreamSeekCur(const int64_t qwPos)
    { return FStreamSeek(qwPos, SEEK_CUR); }
  bool FStreamSeekSet(const int64_t qwPos)
    { return FStreamSeek(qwPos, SEEK_SET); }
  bool FStreamSeekEnd(const int64_t qwPos)
    { return FStreamSeek(qwPos, SEEK_END); }
  /* -- Set current file position with check ------------------------------- */
  bool FStreamSeekSafe(const int64_t qwPos, const int iMode)
    { return FStreamOpened() ? FStreamSeek(qwPos, iMode) : false; }
  bool FStreamSeekSafeCur(const int64_t qwPos)
    { return FStreamSeekSafe(qwPos, SEEK_CUR); }
  bool FStreamSeekSafeSet(const int64_t qwPos)
    { return FStreamSeekSafe(qwPos, SEEK_SET); }
  bool FStreamSeekSafeEnd(const int64_t qwPos)
    { return FStreamSeekSafe(qwPos, SEEK_END); }
  /* -- Return current file position --------------------------------------- */
  int64_t FStreamTell()
    { return FStreamErrNoWrapper(StdFTell(FStreamGetCtx())); }
  int64_t FStreamTellSafe() { return FStreamOpened() ? FStreamTell() : EOF; }
  /* -- Flush buffered file data to disk ----------------------------------- */
  bool FStreamFlush() const { return !fflush(FStreamGetCtx()); }
  bool FStreamFlushSafe() const
    { return FStreamOpened() ? FStreamFlush() : false; }
  /* -- File stream is set to error status? -------------------------------- */
  bool FStreamFError() const { return !!ferror(FStreamGetCtx()); }
  bool FStreamFErrorSafe() const
    { return FStreamOpened() ? FStreamFError() : true; }
  /* -- File stream is ready to be read or written ------------------------- */
  bool FStreamIsReadyRead() const { return !FStreamFErrorSafe(); }
  bool FStreamIsNotReadyRead() const { return !FStreamIsReadyRead(); }
  bool FStreamIsReadyWrite() const
    { return FStreamIsReadyRead() && FStreamIsNotEOF(); }
  bool FStreamIsNotReadyWrite() const { return !FStreamIsReadyWrite(); }
  /* -- Is position at end of the file? ------------------------------------ */
  bool FStreamIsEOF() const { return feof(FStreamGetCtx()); }
  bool FStreamIsEOFSafe() const { return FStreamClosed() || FStreamIsEOF(); }
  bool FStreamIsNotEOF() const { return !FStreamIsEOF(); }
  bool FStreamIsNotEOFSafe() const { return !FStreamIsEOFSafe(); }
  /* -- Read file information ---------------------------------------------- */
  bool FStreamStat(StdFStatStruct &sData)
    { return !FStreamErrNoWrapper(
        StdHStat(StdFileNo(FStreamGetCtx()), &sData)); }
  bool FStreamStatSafe(StdFStatStruct &sData)
    { return FStreamOpened() ? FStreamStat(sData) : false; }
  /* -- Read data from file ------------------------------------------------ */
  size_t FStreamRead(void*const vpBuffer, const size_t stBytes)
    { return FStreamErrNoWrapper(fread(vpBuffer,
        1, stBytes, FStreamGetCtx())); }
  size_t FStreamReadSafe(void*const vpBuffer, const size_t stBytes)
    { return FStreamIsReadyRead() ? FStreamRead(vpBuffer, stBytes) : 0; }
  /* -- Read specified number of bytes into a string object ---------------- */
  const string FStreamReadString(const size_t stBytes)
  { // Setup string for buffer and read into it
    string strRead;
    strRead.resize(stBytes);
    strRead.resize(
      FStreamRead(const_cast<char*>(strRead.data()), strRead.length()));
    // Return string
    return strRead;
  }
  const string FStreamReadStringSafe(const size_t stBytes)
    { return FStreamIsReadyRead() && stBytes ?
        FStreamReadString(stBytes) : cCommon->CommonBlank(); }
  const string FStreamReadStringSafe()
  { // Read if ready to read and there are remaining characters
    if(FStreamIsReadyRead())
      if(const size_t stBytesRemaining = FStreamRemain())
        return FStreamReadString(stBytesRemaining);
    // Nothing to read
    return {};
  }
  /* -- Write data to file ------------------------------------------------- */
  size_t FStreamWrite(const void*const vpPtr, const size_t stBytes)
    { return FStreamErrNoWrapper(fwrite(vpPtr,
        1, stBytes, FStreamGetCtx())); }
  size_t FStreamWriteSafe(const void*const vpPtr, const size_t stBytes)
    { return FStreamIsReadyWrite() ? FStreamWrite(vpPtr, stBytes) : 0; }
  /* -- Write memory block to file ----------------------------------------- */
  size_t FStreamWriteBlock(const MemConst &mcSrc)
    { return FStreamWrite(mcSrc.MemPtr(), mcSrc.MemSize()); }
  size_t FStreamWriteBlockSafe(const MemConst &mcSrc)
    { return FStreamIsReadyWrite() && mcSrc.MemIsNotEmpty() ?
        FStreamWrite(mcSrc.MemPtr(), mcSrc.MemSize()) : 0; }
  /* -- Write a string to the file ----------------------------------------- */
  size_t FStreamWriteString(const string &strString)
    { return FStreamWrite(strString.data(), strString.length()); }
  size_t FStreamWriteStringSafe(const string &strString)
    { return FStreamIsReadyWrite() && !strString.empty() ?
        FStreamWrite(strString.data(), strString.length()) : 0; }
  /* -- Formatted writing a string to the file without checking ------------ */
  template<typename ...VarArgs>
    size_t FStreamWriteStringEx(const char*const cpFormat,
      VarArgs &&...vaArgs)
  { return FStreamWriteString(StrFormat(cpFormat,
      StdForward<VarArgs>(vaArgs)...)); }
  template<typename ...VarArgs>
    size_t FStreamWriteStringExSafe(const char*const cpFormat,
      VarArgs &&...vaArgs)
  { return FStreamWriteStringSafe(StrFormat(cpFormat,
      StdForward<VarArgs>(vaArgs)...)); }
  /* -- Read entire file without knowing the size of the file -------------- */
  const string FStreamReadStringChunked(const size_t stBytes=4096)
  { // Stream to write strings to
    ostringstream osS;
    // Loop point
    ContinueReadingStrings:
    // Read the chunk and break if at end of file or error
    const string strChunk{ FStreamReadStringSafe(stBytes) };
    if(strChunk.empty()) return osS.str();
    // Accumulate the size
    osS << StdMove(strChunk);
    // Read another string
    goto ContinueReadingStrings;
  }
  /* -- Read entire file without knowing the size of the file -------------- */
  const string FStreamReadStringChunkedSafe(const size_t stBytes=4096)
    { return FStreamIsReadyRead() && stBytes ?
        FStreamReadStringChunked(stBytes) : cCommon->CommonBlank(); }
  /* -- Read data and return memory block ---------------------------------- */
  Memory FStreamReadBlock(const size_t stBytes)
  { // Allocate initial memory of expected bytes to read
    Memory mOut{ stBytes };
    // Read file data into buffer and resize buffer to the amount acteally read
    mOut.MemResize(FStreamRead(mOut.MemPtr(), stBytes));
    // Return the new size of the buffer and the number of bytes read
    return mOut;
  }
  Memory FStreamReadBlockSafe(const size_t stBytes)
    { return FStreamIsReadyRead() && stBytes ?
        FStreamReadBlock(stBytes) : Memory{}; }
  Memory FStreamReadBlockSafe()
  { // Read into memory block if stream is ready and there are bytes to read
    if(FStreamIsReadyRead())
      if(const size_t stBytesRemain = FStreamRemain())
        return FStreamReadBlock(stBytesRemain);
    // Nothing to read
    return {};
  }
  /* -- Write an static value to file -------------------------------------- */
  template<typename T>size_t FSWriteInt(const T tVar)
    { return FStreamWrite(&tVar, sizeof(tVar)); }
  template<typename T>size_t FSWriteIntSafe(const T tVar)
    { return FStreamOpened() ? FStreamWrite<T>(tVar) : 0; }
  /* -- Return file position to the beginning ------------------------------ */
  bool FStreamRewind() { return FStreamSeekSet(0); }
  bool FStreamRewindSafe()
    { return FStreamOpened() ? FStreamRewind() : false; }
  /* -- Return size of file ------------------------------------------------ */
  int64_t FStreamSize()
  { // Store current position, return if failed and reset to start
    const int64_t qCurrent = FStreamTell();
    if(qCurrent == EOF) return EOF;
    if(!FStreamSeekEnd(0)) return 0;
    // Store current position (size) and restore old position
    const int64_t qSize = FStreamTell();
    FStreamSeek(qCurrent, SEEK_SET);
    // Return size
    return qSize;
  }
  /* -- Return remaining bytes left to read in stream ---------------------- */
  size_t FStreamRemain()
    { return UtilIntOrMax<size_t>(FStreamSize() - FStreamTell()); }
  /* -- Return size of file ------------------------------------------------ */
  int64_t FStreamSizeSafe() { return FStreamClosed() ? 0 : FStreamSize(); }
  /* -- Open a file without filename validation ---------------------------- */
  int FStreamOpen(const string &strFile, const FStreamMode fsmMode)
  { // Try to open the file on disk and if succeeded? Return the result
    if(FILE*const fPtr =
      FStreamErrNoWrapper(FStreamDoOpenDirect(strFile, fsmMode)))
        return FStreamDoAccept(strFile, fPtr);
    // Return original error if there is a home directory?
    using namespace ICmdLine::P;
    if(cCmdLine->CmdLineIsHome())
    { // Build new filename and return the new open result
      string strFilePersist{ cCmdLine->CmdLineGetHome(strFile) };
      if(FILE*const fPtr = FStreamDoOpenDirect(strFilePersist, fsmMode))
        return FStreamDoAccept(strFilePersist, fPtr);
    } // Failed so return error number
    return iErrNo;
  }
  /* -- Close file --------------------------------------------------------- */
  bool FStreamClose()
  { // Return success if is a std handle else close stream handle
    const bool bResult = FStreamIsHandleStd() ? true : FStreamDoClose();
    // Stream is now invalid so clear handle
    FStreamClearHandle();
    // Returm original close result
    return bResult;
  }
  bool FStreamCloseSafe()
    { return FStreamOpened() ? FStreamClose() : true; }
  /* -- Default constructor with no init ----------------------------------- */
  FStreamBase() :
    /* -- Initialisers ----------------------------------------------------- */
    fStream(nullptr),                  // File context not initialised yet
    iErrNo(0)                          // Error number not initialised yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor with direct open (copy filename) ----------------------- */
  FStreamBase(const string &strF, const FStreamMode fsmMode) :
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ strF },                     // Copy filename
    fStream(FStreamDoOpenDirect(       // Open a stream
      IdentGet(),                      // - with the specified filename
      fsmMode)),                       // - with the specified mode
    iErrNo(errno)                      // Set the error number
    /* -- Check that the file was opened and try persist dir if failed ----- */
    { FStreamDoCheckOpenDirect(IdentGet(), fsmMode); }
  /* -- Constructor with direct open (move filename) ----------------------- */
  FStreamBase(string &&strF, const FStreamMode fsmMode) :
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ StdMove(strF) },            // Move filename
    fStream(FStreamDoOpenDirect(       // Open a stream
      IdentGet(),                      // - with the specified filename
      fsmMode)),                       // - with the specified mode
    iErrNo(errno)                      // Set the error number
    /* -- Check that the file was opened and try persist dir if failed ----- */
    { FStreamDoCheckOpenDirect(IdentGet(), fsmMode); }
  /* -- Constructor with rvalue name init, no open ------------------------- */
  explicit FStreamBase(string &&strF) :    // Movable filename string
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ StdMove(strF) },            // Move filename across
    fStream(nullptr),                  // File context not initialised yet
    iErrNo(0)                          // Error number not initialised yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor with lvalue name init, no open ------------------------- */
  explicit FStreamBase(const string &strF) : // Filename to set
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ strF },                     // Move filename across
    fStream(nullptr),                  // File context not initialised yet
    iErrNo(0)                          // Error number not initialised yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- MOVE assignment constructor ---------------------------------------- */
  FStreamBase(FStreamBase &&fsOther) : // Other FStream object
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ StdMove(fsOther) },         // Move filename across from other
    fStream(fsOther.FStreamGetCtx()),  // Copy stream context over
    iErrNo(fsOther.FStreamGetErrNo())  // Copy error number over
    /* -- Clear other handle ----------------------------------------------- */
    { fsOther.FStreamClearHandle(); }
  /* -- Destructor (Close the file if opened) ------------------------------ */
  ~FStreamBase()
    { if(FStreamOpened() && FStreamIsHandleNotStd()) FStreamDoClose(); }
};/* ----------------------------------------------------------------------- */
class FStream :                        // Main file stream class
  /* -- Base classes ------------------------------------------------------- */
  public FStreamBase                   // Contains fstream base class
{ /* -- Direct access using class variable name which returns opened */ public:
  operator bool() const { return FStreamOpened(); }
  /* -- Constructor with optional checking --------------------------------- */
  FStream(const string &strF, const FStreamMode fsmMode) :
    FStreamBase(strF, fsmMode) {}
  /* -- Constructor with rvalue name init, no open ------------------------- */
  explicit FStream(string &&strF) : FStreamBase{ StdMove(strF) } {}
  /* -- Constructor with lvalue name init, no open ------------------------- */
  explicit FStream(const string &strF) : FStreamBase{ strF } {}
  /* -- MOVE assignment constructor ---------------------------------------- */
  FStream(FStream &&fsOther) : FStreamBase{ StdMove(fsOther) } {}
  /* -- Basic constructor with no init ------------------------------------- */
  FStream() = default;
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
