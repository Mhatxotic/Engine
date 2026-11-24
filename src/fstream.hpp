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
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IError::P;
using namespace IName::P;              using namespace IStd::P;
using namespace IStdLib::P;            using namespace IString::P;
using namespace IMemory::P;            using namespace IUtil::P;
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
  public virtual NameStr               // Contains filename string
{ /* -- Private variables -------------------------------------------------- */
  FILE            *fStream;            // Stream handle
  StdErrNo         enLast;             // Stored error number
  /* -- Accept a file stream from DoOpen() --------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  StdErrNo FStreamDoAccept(StrType &&strFile, FILE*const fPtr)
  { // Close original file if opened and set the new handle and file name
    FStreamCloseSafe();
    FStreamSetHandle(fPtr);
    NameSet(StdForward<StrType>(strFile));
    // Success!
    return 0;
  }
  /* -- Error number wrapper handle ---------------------------------------- */
  template<typename AnyType>
    AnyType FStreamSetError(AnyType atVal) noexcept
  { enLast = StdGetError(); return atVal; }
  /* -- Convert mode enum to text ------------------------------------------ */
  static auto FStreamModeIdToString(const FStreamMode fsmMode)
  { // Obviously Windows has to be different from everyone else!
#if defined(WINDOWS)                   // Using windows?
    // The mode supported (in unicode)
    static const StdArray<const wchar_t*const, FM_MAX> cplModes{
      L"rt", L"wt", L"at", L"r+t", L"w+t", L"a+t",
      L"rb", L"wb", L"ab", L"r+b", L"w+b", L"a+b"
    };
#else                                  // Using linux or MacOS?
    // The mode supported (in ansi string)
    static const StdArray<const char*const, FM_MAX> cplModes{
      "rt", "wt", "at", "r+t", "w+t", "a+t",
      "rb", "wb", "ab", "r+b", "w+b", "a+b"
    };
#endif                                 // Operating system check
    // Send it to the posix file open call and accept it if successful
    return cplModes[fsmMode];
  }
  /* -- Open a file and return its handle ---------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  static FILE *FStreamDoOpenDirect(StrType &&strFile,
    const FStreamMode fsmMode)
  { // Return the below file handle depending on operating system
    return
    // Obviously Windows has to be different from everyone else!
#if defined(WINDOWS)                   // Using windows?
      // Send it to the Windows file open call and accept it if successful
      _wfsopen(UTFtoS16(strFile).data(), FStreamModeIdToString(fsmMode),
        _SH_DENYWR);
#else                                  // Using linux or MacOS?
      // Send it to the posix file open call and accept it if successful
      fopen(StdGetStrOrCStr(strFile), FStreamModeIdToString(fsmMode));
#endif                                 // Operating system check
  }
  /* -- Closes the file and doesn't care about the result ------------------ */
  void FStreamDoClose() noexcept { fclose(FStreamGetCtx()); }
  /* -- Clear file handle -------------------------------------------------- */
  void FStreamClearHandle() noexcept { FStreamSetHandle(nullptr); }
  /* -- Set handle ---------------------------------------------- */ protected:
  void FStreamSetHandle(FILE*const fpS) noexcept { fStream = fpS; }
  /* -- Return file stream handle ---------------------------------- */ public:
  FILE *FStreamGetCtx() const noexcept { return fStream; }
  /* -- Returns true if handle is a known pipe ----------------------------- */
  bool FStreamIsHandleStd() const noexcept
    { return FStreamGetCtx() == stdin ||
             FStreamGetCtx() == stdout ||
             FStreamGetCtx() == stderr; }
  /* -- Returns true if handle is not a known pipe ------------------------- */
  bool FStreamIsNotHandleStd() const noexcept { return !FStreamIsHandleStd(); }
  /* -- Get file stream descriptor ----------------------------------------- */
  int FStreamGetFd() const noexcept { return StdFileNo(FStreamGetCtx()); }
  /* -- Get file stream descriptor with check ------------------------------ */
  int FStreamGetFdSafe() const noexcept
    { return FStreamOpened() ? FStreamGetFd() : EOF; }
  /* -- Swap stream handle ------------------------------------------------- */
  void FStreamSwap(FStreamBase &fsOther) noexcept
    { StdSwap(fStream, fsOther.fStream); }
  /* -- File is opened? ---------------------------------------------------- */
  bool FStreamOpened() const noexcept { return FStreamGetCtx() != nullptr; }
  /* -- File is closed? ---------------------------------------------------- */
  bool FStreamClosed() const noexcept { return !FStreamOpened(); }
  /* -- Return last error number ------------------------------------------- */
  StdErrNo FStreamGetErrNo() const noexcept { return enLast; }
  /* -- Return last error string ------------------------------------------- */
  StdString FStreamGetErrStr() const { return StrFromErrNo(enLast); }
  /* -- Return handle to stream -------------------------------------------- */
  int FStreamGetID() const noexcept { return StdFileNo(FStreamGetCtx()); }
  /* -- Return handle to stream with check --------------------------------- */
  int FStreamGetIDSafe() const noexcept
    { return FStreamOpened() ? FStreamGetID() : 0; }
  /* -- Set file position without check ------------------------------------ */
  bool FStreamSeek(const int64_t llPos, const int iMode) noexcept
    { return !FStreamSetError(StdFSeek(FStreamGetCtx(), llPos, iMode)); }
  /* -- Set file position with check --------------------------------------- */
  bool FStreamSeekSafe(const int64_t llPos, const int iMode) noexcept
    { return FStreamOpened() ? FStreamSeek(llPos, iMode) : false; }
  /* -- Offset file position ----------------------------------------------- */
  bool FStreamSeekCur(const int64_t llPos) noexcept
    { return FStreamSeek(llPos, SEEK_CUR); }
  /* -- Offset file position with check ------------------------------------ */
  bool FStreamSeekSafeCur(const int64_t llPos) noexcept
    { return FStreamSeekSafe(llPos, SEEK_CUR); }
  /* -- Set current file position ------------------------------------------ */
  bool FStreamSeekSet(const int64_t llPos) noexcept
    { return FStreamSeek(llPos, SEEK_SET); }
  /* -- Set current file position with check ------------------------------- */
  bool FStreamSeekSafeSet(const int64_t llPos) noexcept
    { return FStreamSeekSafe(llPos, SEEK_SET); }
  /* -- Set file position from end ----------------------------------------- */
  bool FStreamSeekEnd(const int64_t llPos) noexcept
    { return FStreamSeek(llPos, SEEK_END); }
  /* -- Set file position from end with check ------------------------------ */
  bool FStreamSeekSafeEnd(const int64_t llPos) noexcept
    { return FStreamSeekSafe(llPos, SEEK_END); }
  /* -- Return current file position --------------------------------------- */
  int64_t FStreamTell() noexcept
    { return FStreamSetError(StdFTell(FStreamGetCtx())); }
  /* -- Return current file position with check ---------------------------- */
  int64_t FStreamTellSafe() noexcept
    { return FStreamOpened() ? FStreamTell() : EOF; }
  /* -- Flush buffered file data to disk ----------------------------------- */
  bool FStreamFlush() const noexcept { return !fflush(FStreamGetCtx()); }
  /* -- Flush buffered file data to disk with check? ----------------------- */
  bool FStreamFlushSafe() const noexcept
    { return FStreamOpened() ? FStreamFlush() : false; }
  /* -- File stream is set to error status? -------------------------------- */
  bool FStreamFError() const noexcept { return ferror(FStreamGetCtx()) != 0; }
  /* -- File stream is set to error status with check? --------------------- */
  bool FStreamFErrorSafe() const noexcept
    { return FStreamOpened() ? FStreamFError() : true; }
  /* -- File stream is ready to be read ------------------------------------ */
  bool FStreamIsReadyRead() const noexcept { return !FStreamFErrorSafe(); }
  /* -- File stream is not ready to be read -------------------------------- */
  bool FStreamIsNotReadyRead() const noexcept { return !FStreamIsReadyRead(); }
  /* -- File stream is ready to be written --------------------------------- */
  bool FStreamIsReadyWrite() const noexcept
    { return FStreamIsReadyRead() && FStreamIsNotEOF(); }
  /* -- File stream is not ready to be written ----------------------------- */
  bool FStreamIsNotReadyWrite() const noexcept
    { return !FStreamIsReadyWrite(); }
  /* -- Is position at end of the file? ------------------------------------ */
  bool FStreamIsEOF() const noexcept { return feof(FStreamGetCtx()); }
  /* -- File is closed or is position at end of the file? ------------------ */
  bool FStreamIsEOFSafe() const noexcept
    { return FStreamClosed() || FStreamIsEOF(); }
  /* -- Is position not at end of the file? -------------------------------- */
  bool FStreamIsNotEOF() const noexcept { return !FStreamIsEOF(); }
  /* -- File is not closed or end of the file? ----------------------------- */
  bool FStreamIsNotEOFSafe() const noexcept { return !FStreamIsEOFSafe(); }
  /* -- Read file information ---------------------------------------------- */
  bool FStreamStat(StdFStatStruct &sData) noexcept
    { return !FStreamSetError(
        StdHStat(StdFileNo(FStreamGetCtx()), &sData)); }
  /* -- Read file information with check ----------------------------------- */
  bool FStreamStatSafe(StdFStatStruct &sData) noexcept
    { return FStreamOpened() ? FStreamStat(sData) : false; }
  /* -- Read data from file ------------------------------------------------ */
  size_t FStreamRead(void*const vpBuffer, const size_t stBytes) noexcept
    { return FStreamSetError(fread(vpBuffer,
        1, stBytes, FStreamGetCtx())); }
  /* -- Read data from file with check ------------------------------------- */
  size_t FStreamReadSafe(void*const vpBuffer, const size_t stBytes) noexcept
    { return FStreamIsReadyRead() ? FStreamRead(vpBuffer, stBytes) : 0; }
  /* -- Read specified number of bytes into a string object ---------------- */
  StdString FStreamReadString(const size_t stBytes) noexcept
  { // Setup string for buffer and read into it
    StdResized<StdString> strRead{ stBytes };
    strRead.resize(
      FStreamRead(const_cast<char*>(strRead.data()), strRead.size()));
    // Return string
    return strRead;
  }
  /* -- Read specified number of bytes into a string object with check ----- */
  StdString FStreamReadStringSafe(const size_t stBytes) noexcept
    { return FStreamIsReadyRead() && stBytes ?
        FStreamReadString(stBytes) : cCommon->CommonBlankStr(); }
  /* -- Read remaining number of bytes into a string object with check ----- */
  StdString FStreamReadStringSafe() noexcept
  { // Read if ready to read and there are remaining characters
    if(FStreamIsReadyRead())
      if(const size_t stBytesRemaining = FStreamRemain())
        return FStreamReadString(stBytesRemaining);
    // Nothing to read
    return {};
  }
  /* -- Write data to file without checking -------------------------------- */
  size_t FStreamWrite(const void*const vpPtr, const size_t stBytes) noexcept
    { return
        FStreamSetError(fwrite(vpPtr, 1, stBytes, FStreamGetCtx())); }
  /* -- Write data to file with checking ----------------------------------- */
  size_t FStreamWriteSafe(const void*const vpPtr, const size_t stBytes)
    noexcept
  { return FStreamIsReadyWrite() ? FStreamWrite(vpPtr, stBytes) : 0; }
  /* -- Write memory block to file without checking ------------------------ */
  size_t FStreamWriteBlock(const MemConst &mcSrc) noexcept
    { return FStreamWrite(mcSrc.MemPtr(), mcSrc.MemSize()); }
  /* -- Write memory block to file with checking --------------------------- */
  size_t FStreamWriteBlockSafe(const MemConst &mcSrc) noexcept
    { return FStreamIsReadyWrite() && mcSrc.MemIsNotEmpty() ?
        FStreamWrite(mcSrc.MemPtr(), mcSrc.MemSize()) : 0; }
  /* -- Write a string to the file without checking ------------------------ */
  template<class StrType>
    requires StdIsString<StrType>
  size_t FStreamWriteString(const StrType &strString) noexcept
    { return FStreamWrite(strString.data(), strString.size()); }
  /* -- Write a C-string array to the file without checking ---------------- */
  template<size_t stSize>
    requires (stSize > 0)
  size_t FStreamWriteString(const char (&caData)[stSize]) noexcept
    { return FStreamWrite(caData, stSize - 1); }
  /* -- Write a string to the file with checking --------------------------- */
  template<class StrType>
    requires StdIsString<StrType>
  size_t FStreamWriteStringSafe(const StrType &strString) noexcept
    { return FStreamIsReadyWrite() && !strString.empty() ?
        FStreamWriteString(strString) : 0; }
  /* -- Formatted writing a string to the file without checking ------------ */
  template<class StrType, typename ...VarArgs>
    requires StdIsStrOrCStr<StrType>
  size_t FStreamWriteStringEx(StrType &&strFormat, VarArgs &&...vaArgs)
    { return FStreamWriteString(StrFormat(StdForward<StrType>(strFormat),
                                          StdForward<VarArgs>(vaArgs)...)); }
  /* -- Read entire file without knowing the size of the file -------------- */
  StdString FStreamReadStringChunked(const size_t stBytes = 4096)
  { // Stream to write strings to
    StdOStringStream &osS = cCommon->o.StreamReset();
    // Loop point
    ContinueReadingStrings:
    // Read the chunk and break if at end of file or error
    const StdString strChunk{ FStreamReadStringSafe(stBytes) };
    if(strChunk.empty()) return osS.str();
    // Accumulate the size
    osS << strChunk;
    // Read another string
    goto ContinueReadingStrings;
  }
  /* -- Read entire file without knowing the size of the file -------------- */
  StdString FStreamReadStringChunkedSafe(const size_t stBytes = 4096)
    { return FStreamIsReadyRead() && stBytes ?
        FStreamReadStringChunked(stBytes) : cCommon->CommonBlankStr(); }
  /* -- Read specified bytes of data and return memory block --------------- */
  Memory FStreamReadBlock(const size_t stBytes)
  { // Allocate initial memory of expected bytes to read
    Memory mOut{ stBytes };
    // Read file data into buffer and resize buffer to the amount acteally read
    mOut.MemResize(FStreamRead(mOut.MemPtr(), stBytes));
    // Return the new size of the buffer and the number of bytes read
    return mOut;
  }
  /* -- Read specified bytes of data with checking and return memory block - */
  Memory FStreamReadBlockSafe(const size_t stBytes)
    { return FStreamIsReadyRead() && stBytes ?
        FStreamReadBlock(stBytes) : Memory{}; }
  /* -- Read remaining data with checking and return memory block ---------- */
  Memory FStreamReadBlockSafe()
  { // Read into memory block if stream is ready and there are bytes to read
    if(FStreamIsReadyRead())
      if(const size_t stBytesRemain = FStreamRemain())
        return FStreamReadBlock(stBytesRemain);
    // Nothing to read
    return {};
  }
  /* -- Write an static value to file -------------------------------------- */
  template<typename IntType>
    requires StdIsArithmatic<IntType>
  size_t FSWriteInt(const IntType itVar)
    { return FStreamWrite(&itVar, sizeof(itVar)); }
  /* -- Write an static value to file with check --------------------------- */
  template<typename IntType>
    requires StdIsArithmatic<IntType>
  size_t FSWriteIntSafe(const IntType itVar)
    { return FStreamOpened() ? FStreamWriteInt(itVar) : 0; }
  /* -- Return file position to the beginning ------------------------------ */
  bool FStreamRewind() noexcept { return FStreamSeekSet(0); }
  /* -- Return file position to the beginning with check ------------------- */
  bool FStreamRewindSafe() noexcept
    { return FStreamOpened() ? FStreamRewind() : false; }
  /* -- Return size of file ------------------------------------------------ */
  int64_t FStreamSize() noexcept
  { // Store current position, return if failed and reset to start
    const int64_t llCurrent = FStreamTell();
    if(llCurrent == EOF) return EOF;
    if(!FStreamSeekEnd(0)) return 0;
    // Store current position (size) and restore old position
    const int64_t llSize = FStreamTell();
    FStreamSeek(llCurrent, SEEK_SET);
    // Return size
    return llSize;
  }
  /* -- Return remaining bytes left to read in stream ---------------------- */
  size_t FStreamRemain() noexcept
    { return UtilIntOrMax<size_t>(FStreamSize() - FStreamTell()); }
  /* -- Return size of file ------------------------------------------------ */
  int64_t FStreamSizeSafe() noexcept
    { return FStreamClosed() ? 0 : FStreamSize(); }
  /* -- Open a file without filename validation ---------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  StdErrNo FStreamOpen(StrType &&strFile, const FStreamMode fsmMode)
  { // Try to open the file on disk and if succeeded? Return the result
    if(FILE*const fPtr =
         FStreamSetError(FStreamDoOpenDirect(strFile, fsmMode)))
      return FStreamDoAccept(StdForward<StrType>(strFile), fPtr);
    // Return original error if there is a home directory?
    using namespace ICmdLine::P;
    if(cCmdLine->CmdLineIsHome())
    { // Build new filename and return the new open result
      StdString strFilePersist{ cCmdLine->CmdLineGetHome(strFile) };
      if(FILE*const fPtr =
           FStreamDoOpenDirect(strFilePersist, fsmMode))
        return FStreamDoAccept(StdMove(strFilePersist), fPtr);
    } // Failed so return error number
    return enLast;
  }
  /* -- Close file --------------------------------------------------------- */
  bool FStreamClose() noexcept
  { // If file stream is not a standard pipe? Close it and set error number
    if(FStreamIsNotHandleStd()) { FStreamDoClose(); enLast = StdGetError(); }
    // File stream is a stdpipe? We don't close pipes so indicate success
    else errno = enLast = 0;
    // Stream is now invalid so clear handle
    FStreamClearHandle();
    // Returm original close result
    return (enLast == 0);
  }
  /* -- Close file with check ---------------------------------------------- */
  bool FStreamCloseSafe() noexcept
    { return FStreamOpened() ? FStreamClose() : true; }
  /* -- Default constructor with no init ----------------------------------- */
  FStreamBase() :
    /* -- Initialisers ----------------------------------------------------- */
    fStream(nullptr),                  // File context not initialised yet
    enLast(0)                          // Error number not initialised yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor with direct open (copy filename) ----------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  FStreamBase(StrType &&strF, const FStreamMode fsmMode) :
    /* -- Initialisers ----------------------------------------------------- */
    Name{ StdForward<StrType>(strF) }, // Copy or move filename
    fStream(FStreamDoOpenDirect(       // Open file...
      NameGet(), fsmMode)),            // ...sending name and mode
    enLast(StdGetError())              // Set the error number
    /* -- Open verification ------------------------------------------------ */
    { // Return if the file is opened successfully
      if(FStreamOpened()) return;
      // Return if there is no home directory
      using namespace ICmdLine::P;
      if(cCmdLine->CmdLineIsNoHome()) return;
      // Try opened again from persist directory
      NameSet(cCmdLine->CmdLineGetHome(strF));
      FStreamSetHandle(FStreamDoOpenDirect(NameGet(), fsmMode));
    }
  /* -- Constructor with rvalue name init, no open ------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  explicit FStreamBase(StrType &&strF) :
    /* -- Initialisers ----------------------------------------------------- */
    Name{ StdForward<StrType>(strF) }, // Copy or move filename
    fStream(nullptr),                  // File context not initialised yet
    enLast(0)                          // Error number not initialised yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- MOVE assignment constructor ---------------------------------------- */
  FStreamBase(FStreamBase &&fsOther) : // Other FStream object
    /* -- Initialisers ----------------------------------------------------- */
    Name{ StdMove(fsOther) },          // Move filename across from other
    fStream(fsOther.FStreamGetCtx()),  // Copy stream context over
    enLast(fsOther.FStreamGetErrNo())  // Copy error number over
    /* -- Clear other handle ----------------------------------------------- */
    { fsOther.FStreamClearHandle(); }
  /* -- Destructor to close the file if opened ----------------------------- */
  ~FStreamBase()
    { if(FStreamOpened() && FStreamIsNotHandleStd()) FStreamDoClose(); }
};/* ----------------------------------------------------------------------- */
struct FStream :                       // Main file stream class
  /* -- Base classes ------------------------------------------------------- */
  public FStreamBase                   // Contains fstream base class
{ /* -- Direct access using class variable name which returns opened ------- */
  operator bool() const { return FStreamOpened(); }
  /* -- Constructor with optional checking --------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  FStream(StrType &&strF, const FStreamMode fsmMode) :
    /* -- Initialisers ----------------------------------------------------- */
    Name{ StdForward<StrType>(strF) }, // Initialise identifier (virtual)
    FStreamBase{ NameGet(), fsmMode }  // Initialise other members
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- MOVE assignment constructor ---------------------------------------- */
  FStream(FStream &&fsOther) :
    /* -- Initialisers ----------------------------------------------------- */
    Name{ StdMove(fsOther) },          // Initialise identifier (virtual)
    FStreamBase{ StdMove(fsOther) }    // Initialise other members
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor ------------------------------------------------ */
  FStream() = default;
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
