/* == FILEMAP.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module closely emulates a FILE* stream but instead maps a file ## **
** ## on disk into memory. This interface is used only by the 'Assets'    ## **
** ## interface when loading files from archives or disk. ONLY supply     ## **
** ## already malloc()'d data to the class if re-using existing data.     ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFileMap {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IMemory::P;
using namespace IStd::P;               using namespace ISystem::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == FileMap Class Definition ============================================= */
class FileMap :
  /* -- Derivced classes --------------------------------------------------- */
  public SysMap,                       // File mapping
  public MemConst                      // Read only memory block
{ /* -- Private variables ----------------------------------------- */ private:
  size_t           stPosition;         // Current position
  /* -- Read from a certain position without checking ---------------------- */
  template<typename PtrType=char>
    PtrType *FileMapDoReadPtrFrom(const size_t stPos, const size_t stBytes=0)
  { // Read address. Memory() will handle all the error checking for us
    PtrType*const ptAddr = MemDoRead<PtrType>(stPos);
    // Set new position clamping to size of file
    stPosition = UtilMinimum(MemSize(), stPos + stBytes);
    // Return address
    return ptAddr;
  }
  /* -- Return current position ------------------------------------ */ public:
  size_t FileMapTell(void) const { return stPosition; }
  /* -- Return bytes remaining --------------------------------------------- */
  size_t FileMapRemain(void) const { return MemSize() - FileMapTell(); }
  /* -- Is end of file ----------------------------------------------------- */
  bool FileMapIsEOF(void) const { return FileMapTell() >= MemSize(); }
  bool FileMapIsNotEOF(void) const { return !FileMapIsEOF(); }
  /* -- Return file times -------------------------------------------------- */
  StdTimeT FileMapModifiedTime(void) { return SysMapGetModified(); }
  StdTimeT FileMapCreationTime(void) { return SysMapGetCreation(); }
  /* -- Return if file is opened ------------------------------------------- */
  bool FileMapOpened(void) const { return !!MemPtr(); }
  bool FileMapClosed(void) const { return !FileMapOpened(); }
  /* -- Read with byte bound check ----------------------------------------- */
  template<typename PtrType=char>
    PtrType *FileMapReadPtrFrom(const size_t stPos, const size_t stBytes=0)
  { // Check parameters.
    if(!MemCheckParam(stPos, stBytes))
      XC("Read error!",
         "Identifier", IdentGet(), "Destination", MemPtr<void>(),
         "Bytes",      stBytes,    "Position",    stPos,
         "Maximum",    SysMapGetSize());
    // Return address. This also sets the new position
    return FileMapDoReadPtrFrom<PtrType>(stPos, stBytes);
  }
  /* -- Read from current position ----------------------------------------- */
  template<typename PtrType=char>
    PtrType *FileMapReadPtr(const size_t stBytes=0)
      { return FileMapReadPtrFrom<PtrType>(FileMapTell(), stBytes); }
  /* -- Read variable from specified position ------------------------------ */
  template<typename IntType=char>
    const IntType FileMapReadVarFrom(const size_t stPos)
      { return *FileMapReadPtrFrom<const IntType>(stPos, sizeof(IntType)); }
  /* -- Read specified variable from current pos --------------------------- */
  template<typename IntType=char>const IntType FileMapReadVar(void)
    { return FileMapReadVarFrom<IntType>(FileMapTell()); }
  /* -- Read specified variable from specified position -------------------- */
  uint16_t FileMapReadVar16LEFrom(const size_t stFrom)
    { return UtilToI16LE(FileMapReadVarFrom<uint16_t>(stFrom)); }
  uint16_t FileMapReadVar16BEFrom(const size_t stFrom)
    { return UtilToI16BE(FileMapReadVarFrom<uint16_t>(stFrom)); }
  uint32_t FileMapReadVar32LEFrom(const size_t stFrom)
    { return UtilToI32LE(FileMapReadVarFrom<uint32_t>(stFrom)); }
  uint32_t FileMapReadVar32BEFrom(const size_t stFrom)
    { return UtilToI32BE(FileMapReadVarFrom<uint32_t>(stFrom)); }
  uint64_t FileMapReadVar64LEFrom(const size_t stFrom)
    { return UtilToI64LE(FileMapReadVarFrom<uint64_t>(stFrom)); }
  uint64_t FileMapReadVar64BEFrom(const size_t stFrom)
    { return UtilToI64BE(FileMapReadVarFrom<uint64_t>(stFrom)); }
  /* -- Read specified variable from current pos --------------------------- */
  uint16_t FileMapReadVar16LE(void)
    { return FileMapReadVar16LEFrom(FileMapTell()); }
  uint16_t FileMapReadVar16BE(void)
    { return FileMapReadVar16BEFrom(FileMapTell()); }
  uint32_t FileMapReadVar32LE(void)
    { return FileMapReadVar32LEFrom(FileMapTell()); }
  uint32_t FileMapReadVar32BE(void)
    { return FileMapReadVar32BEFrom(FileMapTell()); }
  uint64_t FileMapReadVar64LE(void)
    { return FileMapReadVar64LEFrom(FileMapTell()); }
  uint64_t FileMapReadVar64BE(void)
    { return FileMapReadVar64BEFrom(FileMapTell()); }
  /* -- Read data into new array class ------------------------------------- */
  const Memory FileMapReadBlock(const size_t stBytes)
  { // Allocate requested size
    Memory mOut{ stBytes };
    // Read data and shrink block to fit actual bytes read then return it
    mOut.MemResize(FileMapReadToAddr(mOut.MemPtr(), stBytes));
    // Return bytes read
    return mOut;
  }
  /* --Seek from start ----------------------------------------------------- */
  bool FileMapSeekSet(const size_t stPos)
  { // Not at position already?
    if(stPos != FileMapTell())
    { // Failed if out of range
      if(stPos > MemSize()) return false;
      // Update position
      stPosition = stPos;
    } // Success
    return true;
  }
  /* -- Seek from current position ----------------------------------------- */
  bool FileMapSeekCur(const size_t stPos)
    { return FileMapSeekSet(FileMapTell() + stPos); }
  /* -- Seek from end position --------------------------------------------- */
  bool FileMapSeekEnd(const size_t stPos)
    { return FileMapSeekSet(MemSize() + stPos); }
  /* -- Read data from specified position in file to array ----------------- */
  const Memory FileMapReadBlockFrom(const size_t stPos, const size_t stBytes)
  { // Do the seek and return a blank array if failed
    if(!FileMapSeekSet(stPos)) return { };
    // Do the read and return the array
    return FileMapReadBlock(stBytes);
  }
  /* -- FileMap::Seek position---------------------------------------------- */
  bool FileMapSeek(const size_t stPos, const int iMode)
  { // Which was set
    switch(iMode)
    { // Seek from start?
      case SEEK_SET: return FileMapSeekSet(stPos);
      // Seek from current position?
      case SEEK_CUR: return FileMapSeekCur(stPos);
      // Seek from eof
      case SEEK_END: return FileMapSeekEnd(stPos);
      // Anything else is a failure
      default: return false;
    } // Doesn't reach here
  }
  /* -- FileMap::Read data from specified position in file into memory ----- */
  size_t FileMapReadFrom(const size_t stPos, void*const vpDst,
    const size_t stBytes)
  { // Do the seek and return a blank array if failed
    if(!FileMapSeekSet(stPos)) return 0;
    // Do the read and return the array
    return FileMapReadToAddr(vpDst, stBytes);
  }
  /* -- Return full memory of file ----------------------------------------- */
  Memory FileMapDecouple(void)
  { // If memory is not mapped? Just move the current memory across so the
    // returned Memory block takes ownership and frees the memory
    if(SysMapIsNotAvailable())
      return Memory{ StdMove(static_cast<MemConst&>(*this)) };
    // We need to read mapped memory into a new memory block. The map class
    // disallows files greater than size_t(-1) so this is safe
    Memory mOut{ UtilIntOrMax<size_t>(SysMapGetSize()), SysMapGetMemory() };
    // De-initialise the map, no point keeping it anymore
    SysMapDeInit();
    // Clear memory block members
    MemReset();
    // Return memory
    return mOut;
  }
  /* -- FileMap::Read data into existing memory area ----------------------- */
  template<typename T=void>
    size_t FileMapReadToAddr(T*const vpDst, const size_t stBytes)
  { // Bail if no more data
    if(FileMapIsEOF()) return 0;
    // Calculate actual bytes to read
    const size_t stToRead = (FileMapTell() + stBytes > MemSize()) ?
      MemSize() - FileMapTell() : stBytes;
    // Copy memory to destination
    memcpy(reinterpret_cast<void*>(vpDst),
      MemDoRead<void>(FileMapTell()), stToRead);
    // Seek forward
    stPosition += stToRead;
    // Return bytes
    return stToRead;
  }
  /* -- Rewind to start ---------------------------------------------------- */
  void FileMapRewind(void) { stPosition = 0; }
  /* -- Assignment operator ------------------------------------------------ */
  void FileMapSwap(FileMap &fmOther)
  { // Swap memory block and map
    MemConstSwap(fmOther);
    SysMapSwap(fmOther);
    // Swap position
    swap(stPosition, fmOther.stPosition);
  }
  /* -- Direct access using class variable name which returns opened ------- */
  operator bool(void) const { return FileMapOpened(); }
  /* -- Open a file on disk and map it to memory --------------------------- */
  explicit FileMap(const string &strF) :
    /* -- Initialisers ----------------------------------------------------- */
    SysMap{ strF },
    MemConst{ UtilIntOrMax<size_t>(SysMapGetSize()), SysMapGetMemory() },
    stPosition(0)
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Take ownership of another memory block ----------------------------- */
  FileMap(const string &strF, MemConst &&mcSrc, const StdTimeT ttC,
    const StdTimeT ttM) :
    /* -- Initialisers ----------------------------------------------------- */
    SysMap{ strF, ttC, ttM },          // Reuse system map variables
    MemConst{ StdMove(mcSrc) },        // Init read-only memory block
    stPosition(0)                      // Initialise position
    /* --------------------------------------------------------------------- */
    { }                                // Don't do anything else
  /* -- Take ownership of another memory block ----------------------------- */
  FileMap(const string &strF, MemConst &&mcSrc, const StdTimeT ttC) :
    /* -- Initialisers ----------------------------------------------------- */
    FileMap{ strF, StdMove(mcSrc), ttC, ttC }
    /* --------------------------------------------------------------------- */
    { }                                // Don't do anything else
  /* -- Move filemap constructor ------------------------------------------- */
  FileMap(FileMap &&fmOther) :
    /* -- Initialisers ----------------------------------------------------- */
    SysMap{ StdMove(fmOther) },        // Just moves SysMap members
    MemConst{ StdMove(fmOther) },      // Just moves MemConst members
    stPosition(fmOther.FileMapTell())  // Copy other current position
    /* --------------------------------------------------------------------- */
    { fmOther.FileMapRewind(); }       // Reset other position
  /* -- No-init constructor ------------------------------------------------ */
  FileMap(void) :
    /* -- Initialisers ----------------------------------------------------- */
    stPosition(0)                      // No position
    /* --------------------------------------------------------------------- */
    { }                                // Don't do anything else
  /* -- Free memory if we allocated it and it's not a map ------------------ */
  ~FileMap(void) { if(MemIsPtrSet() && MemPtr() != SysMapGetMemory())
                     MemFreePtr(); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
