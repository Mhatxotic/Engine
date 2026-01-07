/* == MEMORY.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## These classes allow allocation and manipulation of memory. Always   ## **
** ## use the 'Memory' class as statically allocated so the memory will   ## **
** ## never be lost. The 'MemBase' class is used to help manipulate       ## **
** ## existing memory and may be used when allocation isn't needed.       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IMemory {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IError::P;             using namespace IIdent::P;
using namespace IStd::P;               using namespace IUtf::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Read only data class ================================================= */
class MemConst                         // Start of const MemBase Block Class
{ /* -- Private variables ----------------------------------------- */ private:
  char            *cpPtr;              // Pointer to data
  size_t           stSize;             // MemSize of data
  /* -- Read pointer -------------------------------------------- */ protected:
  template<typename Type=char>
    Type *MemDoRead(const size_t stPos) const
  { return reinterpret_cast<Type*>(cpPtr + stPos); }
  /* -- Clear parameters. Used by FileMap() -------------------------------- */
  void MemReset() { MemSetPtr(); MemSetSize(); }
  /* -- Swap members with another block ------------------------------------ */
  void MemConstSwap(MemConst &mcOther)
    { swap(stSize, mcOther.stSize); swap(cpPtr, mcOther.cpPtr); }
  /* -- Bit test error handler --------------------------------------------- */
  void MemCheckBit[[noreturn]](const char*const cpAddr,
    const size_t stPos) const
  { // Get absolute position and maximum bit position
    const size_t stAbsPos = UtilBitToByte(stPos),
                 stMax = UtilBitFromByte(stSize);
    // Throw the error
    XC(cpAddr,
      "BitPosition",  stPos,    "BitMaximum",  stMax,
      "BytePosition", stAbsPos, "ByteMaximum", stSize,
      "AddrPosition", MemDoRead<void*>(stAbsPos),
      "AddrStart",    MemPtr(), "AddrMaximum", MemPtrEnd());
  }
  /* -- Read pointer error handler------------------------------------------ */
  void MemErrorRead[[noreturn]](const char*const cpAddr, const size_t stPos,
    const size_t stBytes) const
  { XC(cpAddr,
      "Position",  stPos,    "Amount",  stBytes,
      "Maximum",   stSize,   "AddrPos", MemDoRead<void*>(stPos),
      "AddrStart", MemPtr(), "AddrMax", MemPtrEnd()); }
  /* -- Find a NULL character in the memory -------------------------------- */
  size_t MemFindNull() const
  { // If memory is not empty, find a null character and resize up to it
    if(const char*const cpLocPtr =
      StdFindChar(MemPtr<char>(), '\0', MemSize()))
        return static_cast<size_t>(cpLocPtr - MemPtr<char>());
    // Not found
    return StdMaxSizeT;
  }
  /* -- Set size ----------------------------------------------------------- */
  void MemSetPtr(char*const cpNPtr = nullptr) { cpPtr = cpNPtr; }
  void MemSetSize(const size_t stBytes = 0) { stSize = stBytes; }
  void MemSetPtrSize(char*const cpNPtr, const size_t stBytes)
    { MemSetPtr(cpNPtr); MemSetSize(stBytes); }
  /* -- Free the pointer --------------------------------------------------- */
  void MemFreePtr() { StdFree(MemPtr()); }
  void MemFreePtrIfSet() { if(MemIsPtrSet()) MemFreePtr(); }
  /* -- Return memory at the allocated address --------------------- */ public:
  template<typename Type=void>
    Type *MemPtr() const { return reinterpret_cast<Type*>(cpPtr); }
  /* -- Return size of allocated memory ------------------------------------ */
  template<typename Type=size_t>
    Type MemSize() const { return static_cast<Type>(stSize); }
  /* -- Return if memory is allocated -------------------------------------- */
  bool MemIsEmpty() const { return MemSize() == 0; }
  bool MemIsNotEmpty() const { return !MemIsEmpty(); }
  /* -- Returns if the pointer is valid ------------------------------------ */
  bool MemIsPtrSet() const { return !!MemPtr(); }
  bool MemIsPtrNotSet() const { return !MemPtr(); }
  /* -- Return ending address ---------------------------------------------- */
  template<typename Type=void>
    Type *MemPtrEnd() const { return MemDoRead<Type>(stSize); }
  /* -- Return if we can read this amount of data -------------------------- */
  bool MemCheckParam(const size_t stPos, const size_t stBytes) const
    { return stPos + stBytes <= MemSize(); }
  /* -- Same as MemCheckParam() with ptr check ----------------------------- */
  bool MemCheckPtr(const size_t stPos, const size_t stBytes,
    const void*const vpOther) const
      { return MemCheckParam(stPos, stBytes) && vpOther != nullptr; }
  /* -- Test a bit position ------------------------------------------------ */
  bool MemCheckPos(const size_t stPos) const
    { return UtilBitToByte(stPos) >= MemSize(); }
  /* -- Find specified string ---------------------------------------------- */
  size_t MemFind(const string &strWhat, size_t stPos=0) const
  { // Bail if parameters are invalid
    if(strWhat.empty() || MemIsEmpty() || stPos > MemSize())
      return StdMaxSizeT;
    // Position
    size_t stIndex;
    // Until end of string
    while(const char*const cpLoc =
      StdFindChar(MemDoRead(stPos), strWhat.front(), MemSize() - stPos))
    { // Calculate index
      stPos = static_cast<size_t>(MemPtr<char>() - cpLoc);
      // Walk data until one of three things happen
      // - End of match string
      // - Character mismatch
      // - End of memory block
      for(stIndex = 0;
          stIndex < strWhat.length() &&
          strWhat[stIndex] == cpPtr[stPos + stIndex] &&
          stPos+stIndex < stSize;
        ++stIndex);
      // If we read all of the string match then we succeeded
      if(stIndex >= strWhat.length()) return stPos;
      // Incrememnt position and try again
      stPos += stIndex;
    } // Failed
    return StdMaxSizeT;
  }
  /* -- Read with byte bound check ----------------------------------------- */
  template<typename Type=char>
    Type *MemRead(const size_t stPos, const size_t stBytes=0) const
  { // Bail if size bad
    if(!MemCheckParam(stPos, stBytes))
      MemErrorRead("Read error!", stPos, stBytes);
    // Return pointer
    return MemDoRead<Type>(stPos);
  }
  /* -- Read static variable ----------------------------------------------- */
  template<typename Type>Type MemReadInt(const size_t stPos=0) const
  { // We could just read the data directly unaligned like thus...
    // - return *MemRead<Type>(stPos, sizeof(Type));
    // But this there are consequences as reads could be unaligned so we need
    // to make sure we align it or ASAN debuggers will flag it as such. It
    // seems the following code gets properly optimised anyway.
    Type tDest;
    memcpy(&tDest, MemRead<void>(stPos, sizeof(Type)), sizeof(Type));
    return tDest;
  }
  /* ----------------------------------------------------------------------- */
  template<typename Type> requires is_integral_v<Type> && (sizeof(Type) > 1)
    Type ReadIntLE(const size_t stPos=0) const
  { return UtilToLittleEndian(MemReadInt<Type>(stPos)); }
  /* ----------------------------------------------------------------------- */
  template<typename Type> requires is_integral_v<Type> && (sizeof(Type) > 1)
    Type ReadIntBE(const size_t stPos=0) const
  { return UtilToBigEndian(MemReadInt<Type>(stPos)); }
  /* -- Test a bit --------------------------------------------------------- */
  bool MemBitTest(const size_t stPos) const
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Test error!", stPos);
    // Return the tested bit
    return UtilBitTest(MemPtr<char>(), stPos);
  }
  /* -- Stringview'ify the memory ------------------------------------------ */
  const string_view MemToStringViewSafe(const size_t stBytes) const
  { // Return empty string if no size
    if(MemIsEmpty()) return {};
    // Check position
    if(!MemCheckParam(0, stBytes))
      XC("Invalid size!",
        "Source", MemPtr(), "Bytes", stBytes, "Maximum", MemSize());
    // There is no null character so we have to limit the size
    return { MemPtr<char>(), stBytes };
  }
  /* -- Stringview'ify the memory with the current size -------------------- */
  const string_view MemToStringViewSafe() const
    { return MemToStringViewSafe(MemSize()); }
  /* -- Stringviewify the memory (already assumes last char is '\0') ------- */
  const string_view MemToStringView() const
    { return { MemPtr<char>(), MemSize() - 1 }; }
  /* -- Stringview'ify the memory ------------------------------------------ */
  const string MemToString() const
    { return { MemPtr<char>(), MemSize() }; }
  /* -- Stringify the memory ----------------------------------------------- */
  const string MemToStringSafe() const
  { // Return empty string if no memory
    if(MemIsEmpty()) return {};
    // Find the null character and if we find it?
    switch(const size_t stPos = MemFindNull())
    { // Not found? Return full string
      case StdMaxSizeT: return MemToString();
      // Anything else?
      default: return { MemPtr<char>(), stPos };
    }
  }
  /* -- Return if current size would overflow specified type --------------- */
  template<typename Type=size_t>bool MemIsSizeOverflow() const
    { return UtilIntWillOverflow<Type>(MemSize()); }
  /* -- Init from string (does not copy) ----------------------------------- */
  explicit MemConst(const string &strRef) :
    /* -- Initialisers ----------------------------------------------------- */
    MemConst{ strRef.length(),         // Copy string size and pointer over
      strRef.data() }                  // from specified string
    /* -- Copy pointer and size over from string --------------------------- */
    {}
  /* -- Assignment constructor (rvalue) ------------------------------------ */
  explicit MemConst(MemConst &&mbOther) :
    /* -- Initialisers ----------------------------------------------------- */
    cpPtr(mbOther.MemPtr<char>()),        // Copy pointer
    stSize(mbOther.MemSize())             // Copy size
    /* -- Code to clear other MemConst ------------------------------------ */
    { mbOther.MemReset(); }
  /* -- Uninitialised constructor -- pointer ------------------------------- */
  MemConst() :
    /* -- Initialisers ----------------------------------------------------- */
    cpPtr(nullptr),                    // Start with null pointer
    stSize(0)                          // No size allocated yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Inherit an already allocated pointer ------------------------------- */
  MemConst(const size_t stBytes, char*const cpSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    cpPtr(cpSrc),                      // Initialise memory
    stSize(stBytes)                    // Set pointer size
    /* -- No code ---------------------------------------------------------- */
    { if(MemIsPtrNotSet() && MemIsNotEmpty())
        XC("Null pointer with non-zero memory size requested!",
          "MemSize", MemSize()); }
  /* -- Cast void pointer to char pointer ---------------------------------- */
  MemConst(const size_t stBytes, void*const vpSrc) :
    MemConst(stBytes, reinterpret_cast<char*>(vpSrc)) {}
  /* -- Cast const char pointer to char pointer ---------------------------- */
  MemConst(const size_t stBytes, const char*const cpSrc) :
    MemConst(stBytes, const_cast<char*>(cpSrc)) {}
  /* -- Cast const void pointer to const char ------------------------------ */
  MemConst(const size_t stBytes, const void*const vpSrc) :
    MemConst(stBytes, const_cast<void*>(vpSrc)) {}
};/* ----------------------------------------------------------------------- */
/* == Read and write data class ============================================ */
class MemBase :
  /* -- Base classes ------------------------------------------------------- */
  public MemConst                      // Start of MemBase Block Class
{ /* -- Copy memory --------------------------------------------- */ protected:
  void MemDoWrite(const size_t stPos, const void*const vpSrc,
    const size_t stBytes) { memcpy(MemDoRead(stPos), vpSrc, stBytes); }
  /* -- Move memory (use when memory regions overlap) ---------------------- */
  void MemDoMove(const size_t stPos, const void*const vpSrc,
    const size_t stBytes) { memmove(MemDoRead(stPos), vpSrc, stBytes); }
  /* -- Fill with specified character at specifed position ----------------- */
  template<typename Type=unsigned char>void MemFill(const size_t stPos,
    const Type tVal, const size_t stBytes)
  { // Get address to start from and the address to end at
    char*const cpStart = MemDoRead(stPos),
        *const cpEnd = MemDoRead(stBytes);
    // Do the fill. This is supposedly faster than memset().
    StdFill(par_unseq, reinterpret_cast<Type*const>(cpStart),
      reinterpret_cast<Type*const>(cpEnd), tVal);
    // If we used non-byte integrals? Get remaining bytes and if there is a
    // remainder? Fill the remainder of the buffer.
    if constexpr(sizeof(Type) > 1)
      if(const size_t stRemainder = stBytes % sizeof(Type))
        StdFill(seq, reinterpret_cast<uint8_t*const>(cpEnd - stRemainder),
          reinterpret_cast<uint8_t*const>(cpEnd), static_cast<uint8_t>(tVal));
  }
  /* -- Fill with specified character at specifed position --------- */ public:
  template<typename Type=unsigned char>void MemFillEx(const size_t stPos,
    const Type tVal, const size_t stBytes)
  { // Get end position and make sure it wont overrun
    if(!MemCheckParam(stPos, stBytes))
      MemErrorRead("Fill error!", stPos, stBytes);
    // Do the fill
    MemFill<Type>(stPos, tVal, stBytes);
  }
  /* -- Fill with specified value ------------------------------------------ */
  template<typename Type=unsigned char>void MemFill(const Type tVal=0)
    { MemFill<Type>(0, tVal, MemSize()); }
  /* -- Fast fill with 64-bit ints ----------------------------------------- */
  void MemFill() { MemFill<uint64_t>(); }
  /* -- Move memory with checks ------------------------------------------ */
  void MemMove(const size_t stPos, const void*const vpSrc,
    const size_t stBytes)
  { // Check parameters are valid
    if(!MemCheckPtr(stPos, stBytes, vpSrc))
      XC("Write error!",
        "Destination", MemPtr(), "Source",   vpSrc,
        "Bytes",       stBytes,  "Position", stPos,
        "Maximum",     MemSize());
    // Do copy
    MemDoMove(stPos, vpSrc, stBytes);
  }
  /* -- Write memory with checks ------------------------------------------ */
  void MemWrite(const size_t stPos, const void*const vpSrc,
    const size_t stBytes)
  { // Check parameters are valid
    if(!MemCheckPtr(stPos, stBytes, vpSrc))
      XC("Write error!",
        "Destination", MemPtr(), "Source",   vpSrc,
        "Bytes",       stBytes,  "Position", stPos,
        "Maximum",     MemSize());
    // Do copy
    MemDoWrite(stPos, vpSrc, stBytes);
  }
  /* -- Swap bits ---------------------------------------------------------- */
  void MemSwap8(const size_t stPos)
    { MemWriteInt<uint8_t>(stPos, UtilBitSwap4(MemReadInt<uint8_t>(stPos))); }
  /* ----------------------------------------------------------------------- */
  void MemSwap16(const size_t stPos)
    { MemWriteInt<uint16_t>(stPos, SWAP_U16(MemReadInt<uint16_t>(stPos))); }
  /* ----------------------------------------------------------------------- */
  void MemSwap32(const size_t stPos)
    { MemWriteInt<uint32_t>(stPos, SWAP_U32(MemReadInt<uint32_t>(stPos))); }
  /* ----------------------------------------------------------------------- */
  void MemSwap64(const size_t stPos)
    { MemWriteInt<uint64_t>(stPos, SWAP_U64(MemReadInt<uint64_t>(stPos))); }
  /* -- Write specified variable as an integer ----------------------------- */
  template<typename Type> requires std::is_arithmetic_v<Type>
  void MemWriteInt(const size_t stPos, const Type tVar)
    { MemWrite(stPos, &tVar, sizeof(tVar)); }
  /* ----------------------------------------------------------------------- */
  template<typename Type> requires is_arithmetic_v<Type>
  void MemWriteInt(const Type tVar)
    { MemWriteInt<Type>(0, tVar); }
  /* ----------------------------------------------------------------------- */
  template<typename Type> requires is_arithmetic_v<Type>
    void MemWriteIntLE(const size_t stPos, const Type tVar)
  { MemWriteInt<Type>(stPos, UtilToLittleEndian(tVar)); }
  /* ----------------------------------------------------------------------- */
  template<typename Type> requires is_integral_v<Type> && (sizeof(Type) > 1)
    void MemWriteIntLE(const Type tVar)
  { MemWriteInt<Type>(UtilToLittleEndian(tVar)); }
  /* ----------------------------------------------------------------------- */
  template<typename Type> requires is_integral_v<Type> && (sizeof(Type) > 1)
    void MemWriteIntBE(const size_t stPos, const Type tVar)
  { MemWriteInt<Type>(stPos, UtilToBigEndian(tVar)); }
  /* ----------------------------------------------------------------------- */
  template<typename Type> requires is_integral_v<Type> && (sizeof(Type) > 1)
    void MemWriteIntBE(const Type tVar)
  { MemWriteInt<Type>(UtilToBigEndian(tVar)); }
  /* -- Write specified variable as an floating point number --------------- */
  void MemWriteFloatLE(const float fVar) { MemWriteFloatLE(0, fVar); }
  /* ----------------------------------------------------------------------- */
  void MemWriteFloatLE(const size_t stPos, const float fVar)
    { MemWriteInt<float>(stPos, UtilToF32LE(fVar)); }
  /* ----------------------------------------------------------------------- */
  void MemWriteFloatBE(const float fVar) { MemWriteFloatBE(0, fVar); }
  /* ----------------------------------------------------------------------- */
  void MemWriteFloatBE(const size_t stPos, const float fVar)
    { MemWriteInt<float>(stPos, UtilToF32BE(fVar)); }
  /* ----------------------------------------------------------------------- */
  void MemWriteDoubleLE(const double dVar) { MemWriteDoubleLE(0, dVar); }
  /* ----------------------------------------------------------------------- */
  void MemWriteDoubleLE(const size_t stPos, const double dVar)
    { MemWriteInt<double>(stPos, UtilToF64LE(dVar)); }
  /* ----------------------------------------------------------------------- */
  void MemWriteDoubleBE(const double dVar) { MemWriteDoubleBE(0, dVar); }
  /* ----------------------------------------------------------------------- */
  void MemWriteDoubleBE(const size_t stPos, const double dVar)
    { MemWriteInt<double>(stPos, UtilToF64BE(dVar)); }
  /* -- Write memory block at specified position --------------------------- */
  void MemWriteBlock(const size_t stPos, const MemConst &mcRef,
    const size_t stBytes) { MemWrite(stPos, mcRef.MemPtr<char>(), stBytes); }
  /* ----------------------------------------------------------------------- */
  void MemWriteBlock(const size_t stPos, const MemConst &mcRef)
    { MemWriteBlock(stPos, mcRef, mcRef.MemSize()); }
  /* -- Set a bit ---------------------------------------------------------- */
  void MemBitSet(const size_t stPos)
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Set error!", stPos);
    // Set the bit
    UtilBitSet(MemPtr<char>(), stPos);
  }
  /* -- Clear a bit -------------------------------------------------------- */
  void MemBitClear(const size_t stPos)
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Clear error!", stPos);
    // Clear the bit
    UtilBitClear(MemPtr<char>(), stPos);
  }
  /* -- Flip a bit --------------------------------------------------------- */
  void MemBitFlip(const size_t stPos)
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Flip error!", stPos);
    // Flip the bit
    UtilBitFlip(MemPtr<char>(), stPos);
  }
  /* -- Invert specific flags ---------------------------------------------- */
  template<typename Type=uint8_t>void MemInvert(const size_t stPos,
    const Type tFlags=numeric_limits<Type>::max())
  { // Bail if size bad
    if(!MemCheckParam(stPos, sizeof(Type)))
      MemErrorRead("Invert error!", stPos, sizeof(Type));
    // Do the invert
    *reinterpret_cast<Type*const>(MemDoRead(stPos)) ^= tFlags;
  }
  /* -- Assignment constructor (rvalue) ------------------------------------ */
  MemBase(MemBase &&mbOther) :
    /* -- Initialisers ----------------------------------------------------- */
    MemConst{ StdMove(mbOther) }          // Move other data object
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Take ownership of pointer (rvalue) --------------------------------- */
  explicit MemBase(MemConst &&mcRef) :
    /* -- Initialisers ----------------------------------------------------- */
    MemConst{ StdMove(mcRef) }         // Move other data const object
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Inherit an already allocated pointer ------------------------------- */
  MemBase(const size_t stBytes, char*const cpSrc) :
    MemConst{ stBytes, cpSrc } {}
  MemBase(const size_t stBytes, void*const vpSrc) :
    MemConst{ stBytes, vpSrc } {}
  MemBase(const size_t stBytes, const char*const cpSrc) :
    MemConst{ stBytes, cpSrc } {}
  MemBase(const size_t stBytes, const void*const vpSrc) :
    MemConst{ stBytes, vpSrc } {}
  /* -- Uninitialised constructor -- pointer ------------------------------- */
  MemBase() = default;
};/* ----------------------------------------------------------------------- */
/* == Read, write and allocation data class ================================ */
class Memory :
  /* -- Base classes ------------------------------------------------------- */
  public MemBase                       // Start of Memory Block Class
{ /* -- Resize and preserve allocated memory ---------------------- */ private:
  void MemDoResize(const size_t stBytes)
  { // Realloc new amount of memory and if succeeded? Set new block and size
    if(char*const cpNew =
      StdReAlloc(MemPtr<char>(), UtilMaximum(stBytes, static_cast<size_t>(1))))
        MemSetPtrSize(cpNew, stBytes);
    // Failed so throw error
    else XC("Re-alloc failed!", "OldSize", MemSize(), "NewSize", stBytes);
  }
  /* -- Swap memory with another memory block ---------------------- */ public:
  void MemSwap(Memory &mOther) { MemConstSwap(mOther); }
  /* -- This one help with one liners (temporary variables) ---------------- */
  void MemSwap(Memory &&mOther) { MemConstSwap(mOther); }
  /* -- Resize and preserve allocated memory ------------------------------- */
  void MemResize(const size_t stBytes)
    { if(stBytes != MemSize()) MemDoResize(stBytes); }
  /* -- Add allocated memory ----------------------------------------------- */
  void MemResizeAdd(const size_t stBytes) { MemDoResize(MemSize() + stBytes); }
  /* -- Resize memory upwards never downwards ------------------------------ */
  void MemResizeUp(const size_t stBytes)
    { if(stBytes > MemSize()) MemDoResize(stBytes); }
  /* -- Resize up to the NULL terminator ----------------------------------- */
  void MemResizeToNull()
  { // Return if no memory
    if(MemIsEmpty()) return;
    // Find the null character and if we find it?
    switch(const size_t stPos = MemFindNull())
    { // Not found? Nothing to do
      case StdMaxSizeT: break;
      // Anything else? Do the resize INCLUDING the null character
      default: MemResize(stPos + 1); break;
    }
  }
  /* -- Append the specified amount of memory ------------------------------ */
  void MemAppend(const void*const vpSrc, const size_t stBytes)
  { // Bail out if the pointer is invalid
    if(!vpSrc) XC("Source address to append invalid!");
    // Return if no bytes are to be copied
    if(!stBytes) return;
    // Position to write to
    const size_t stPos = MemSize();
    // Resize block to write memory
    MemDoResize(MemSize() + stBytes);
    // MemWrite data
    MemDoWrite(stPos, vpSrc, stBytes);
  }
  /* -- Resize a block from both ends -------------------------------------- */
  void MemCrop(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!MemCheckParam(stPos, stBytes))
      MemErrorRead("Crop error!", stPos, stBytes);
    // If position is from start? We just need to realloc
    if(!stPos) return MemDoResize(stBytes);
    // Transfer it to this one
    MemSwap({ stBytes - stPos, MemDoRead(stPos) });
  }
  /* -- Byte swap (Hi4<->Lo4 from 8-bit integer) --------------------------- */
  void MemByteSwap8(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!MemCheckParam(stPos, stBytes))
      MemErrorRead("8-bit swap error!", stPos, stBytes);
    // Reverse each byte
    for(uint8_t *ubpPtr = MemDoRead<uint8_t>(stPos),
                *ubpEnd = ubpPtr + stBytes;
                 ubpPtr < ubpEnd;
               ++ubpPtr)
      *ubpPtr = UtilReverseByte(*ubpPtr);
  }
  void MemByteSwap8(const size_t stBytes) { MemByteSwap8(0, stBytes); }
  void MemByteSwap8() { MemByteSwap8(MemSize()); }
  /* -- Byte swap (Hi8<->Lo8 from 16-bit integer) -------------------------- */
  void MemByteSwap16(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!MemCheckParam(stPos, stBytes) || stBytes % sizeof(uint16_t) > 0)
      MemErrorRead("16-bit swap error!", stPos, stBytes);
    // Reverse each word
    for(uint16_t *uspPtr = MemDoRead<uint16_t>(stPos),
                 *uspEnd = MemDoRead<uint16_t>(stPos + stBytes);
                  uspPtr < uspEnd;
                ++uspPtr)
      *uspPtr = SWAP_U16(*uspPtr);
  }
  void MemByteSwap16(const size_t stBytes) { MemByteSwap16(0, stBytes); }
  void MemByteSwap16() { MemByteSwap16(MemSize()); }
  /* -- Byte swap (Hi16<->Lo16 from 32-bit integer) ------------------------ */
  void MemByteSwap32(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!MemCheckParam(stPos, stBytes) || stBytes % sizeof(uint32_t) > 0)
      MemErrorRead("32-bit swap error!", stPos, stBytes);
    // Reverse each dword
    for(uint32_t *ulpPtr = MemDoRead<uint32_t>(stPos),
                 *ulpEnd = MemDoRead<uint32_t>(stPos + stBytes);
                  ulpPtr < ulpEnd;
                ++ulpPtr)
      *ulpPtr = SWAP_U32(*ulpPtr);
  }
  void MemByteSwap32(const size_t stBytes) { MemByteSwap32(0, stBytes); }
  void MemByteSwap32() { MemByteSwap32(MemSize()); }
  /* -- Byte swap (Hi32<->Lo32 from 64-bit integer) ------------------------ */
  void MemByteSwap64(const size_t stPos, const size_t stBytes)
  { // Bail if position + bytes exceeds size
    if(!MemCheckParam(stPos, stBytes) || stBytes % sizeof(uint64_t) > 0)
      MemErrorRead("64-bit swap error!", stPos, stBytes);
    // Reverse each quad
    for(uint64_t *ullpPtr = MemDoRead<uint64_t>(stPos),
                 *ullpEnd = MemDoRead<uint64_t>(stPos + stBytes);
                  ullpPtr < ullpEnd;
                ++ullpPtr)
      *ullpPtr = SWAP_U64(*ullpPtr);
  }
  void MemByteSwap64(const size_t stBytes) { MemByteSwap64(0, stBytes); }
  void MemByteSwap64() { MemByteSwap64(MemSize()); }
  /* -- Reverse at the position for number of bytes ------------------------ */
  void MemReverse(const size_t stPos, const size_t stBytes)
  { // Ignore if length not set and treat as if succeeded
    if(!stBytes) return;
    // Bail if size bad
    if(!MemCheckParam(stPos, stBytes))
      MemErrorRead("Reverse error!", stPos, stBytes);
    // Create empty memory block
    Memory mDst{ stBytes };
    // Copy each byte from the start of source, to the end of destination
    for(size_t stIndex = 0; stIndex < stBytes; ++stIndex)
      mDst.MemDoWrite(MemSize() - stIndex - 1, MemDoRead(stIndex), 1);
    // Assign new memory block
    MemSwap(mDst);
  }
  /* -- Reverse the specified number of bytes------------------------------- */
  void MemReverse(const size_t stBytes) { MemReverse(0, stBytes); }
  /* -- Reverse the entire memory block ------------------------------------ */
  void MemReverse() { MemReverse(MemSize()); }
  /* -- Free the allocated memory and reset members ------------------------ */
  void MemDeInit()
    { if(MemIsPtrSet()) { MemFreePtr(); MemSetPtr(); } MemSetSize(); }
  /* -- Allocate memory ---------------------------------------------------- */
  void MemInitBlank(const size_t stBytesRequested=0)
  { // If allocated memory already exists? Free it!
    MemFreePtrIfSet();
    // Allocate memory forcing zero bytes to 1 byte for compatibility.
    if(char*const cpNew =
      StdAlloc<char>(UtilMaximum(stBytesRequested, static_cast<size_t>(1))))
        return MemSetPtrSize(cpNew, stBytesRequested);
    // The memory was freed so this memory is no longer available.
    MemSetSize();
    // Failed so throw an exception.
    XC("Alloc failed!", "MemSize", stBytesRequested);
  }
  /* -- Allocate and copy from existing memory ----------------------------- */
  void MemInitData(const size_t stBytes, const void*const vpPtr)
    { MemInitBlank(stBytes);
      if(stBytes && vpPtr) MemWrite(0, vpPtr, stBytes); }
  /* -- Allocate and copy from existing string ----------------------------- */
  void MemInitString(const string &strRef)
    { MemInitData(strRef.length(),
        reinterpret_cast<const void*>(strRef.data())); }
  /* -- Allocate and copy from existing memory block ----------------------- */
  void MemInitCopy(const MemConst &mcRef)
    { MemInitData(mcRef.MemSize(), mcRef.MemPtr()); }
  /* -- Allocate and zero memory ------------------------------------------- */
  void MemInitSafe(const size_t stBytes) { MemInitBlank(stBytes); MemFill(); }
  /* -- Assignment operator (rvalue) ------------------------------------ -- */
  Memory &operator=(Memory &&mOther) { MemSwap(mOther); return *this; }
  /* -- Assignment constructor (rvalue) ------------------------------------ */
  Memory(Memory &&mOther) :
    /* -- Initialisers ----------------------------------------------------- */
    MemBase{ StdMove(mOther) }          // Move other memory object
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Take ownership of pointer (must originally be malloc'd) ------------ */
  explicit Memory(MemBase &&mbOther) :
    /* -- Initialisers ----------------------------------------------------- */
    MemBase{ StdMove(mbOther) }        // Move other data object
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Take ownership of pointer (must originally be malloc'd) ------------ */
  explicit Memory(MemConst &&mcOther) :
    /* -- Initialisers ----------------------------------------------------- */
    MemBase{ StdMove(mcOther) }        // Move other read only memory object
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init from string --------------------------------------------------- */
  explicit Memory(const string &strRef) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ strRef.length(),           // Allocate memory and copy the string
      strRef.data() }                  // over to our allocated memory
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Inherit an already allocated pointer ------------------------------- */
  Memory(const size_t stBytes, const void*const vpSrc, const bool) :
    /* -- Initialisers ----------------------------------------------------- */
    MemBase{ stBytes, vpSrc }          // Take ownership of pointer
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Alloc uninitialised ------------------------------------------------ */
  explicit Memory(const size_t stBytes) :
    /* -- Initialisers ----------------------------------------------------- */
    MemBase{ stBytes,                  // Initialise data base class
      StdAlloc<void>                   // Allocate memory (checked by CTOR)
        (UtilMaximum(stBytes,          // Allocate requested size
           static_cast<size_t>(1))) }  // Allocate even if zero so we get addr
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Alloc with fill ---------------------------------------------------- */
  Memory(const size_t stBytes, const bool) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ stBytes }                  // Allocate memory
    /* -- Full memory with zeros if there is data -------------------------- */
    { if(MemIsNotEmpty()) MemFill(); }
  /* -- Alloc with copy ---------------------------------------------------- */
  Memory(const size_t stBytes, const void*const vpSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ stBytes }                  // Allocate size of pointer
    /* -- Code to initialise pointer --------------------------------------- */
    { if(vpSrc) MemDoWrite(0, vpSrc, stBytes); }
  /* -- Standby constructor ------------------------------------------------ */
  Memory() = default;
  /* -- Destructor (just a free() needed) ---------------------------------- */
  ~Memory() { MemFreePtrIfSet(); }
};/* -- Useful types ------------------------------------------------------- */
typedef list<Memory> MemoryList;       // List of memory blocks
typedef vector<Memory> MemoryVector;   // A vector of memory classes
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
