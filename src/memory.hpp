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
/* -- Dependencies --------------------------------------------------------- */
using namespace IBit::P;               using namespace IEndian::P;
using namespace IError::P;             using namespace IName::P;
using namespace IStd::P;               using namespace IUtf::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
using SpanByte        = StdSpan<uint8_t>;   // The type we're using
using SpanByteIt      = SpanByte::iterator; // Iterator type
/* == Read only data class ================================================= */
class MemConst                         // Start of const MemBase Block Class
{ /* -- Private variables --------------------------------------- */ protected:
  SpanByte         sbData;             // Holds the pointer and the size
  /* -- Read pointer ------------------------------------------------------- */
  template<typename PtrType = uint8_t>
    requires (!StdIsPointer<PtrType>)
  PtrType *MemDoRead(const size_t stPos) const
    { return reinterpret_cast<PtrType*>(&*StdNext(sbData.begin(), stPos)); }
  /* -- Clear parameters. Used by FileMap() -------------------------------- */
  void MemReset() { MemSetPtr(); MemSetSize(); }
  /* -- Swap members with another block ------------------------------------ */
  void MemConstSwap(MemConst &mcOther) { StdSwap(sbData, mcOther.sbData); }
  /* -- Bit test error handler --------------------------------------------- */
  void MemCheckBit[[noreturn]](const char*const cpErrMsg,
    const size_t stPos) const
  { // Get absolute position and maximum bit position
    const size_t stAbsPos = BitToByte(stPos);
    // Throw the error
    XC(cpErrMsg,
      "BitPosition",  stPos,    "BitMaximum",  BitFromByte(MemSize()),
      "BytePosition", stAbsPos, "ByteMaximum", MemSize(),
      "AddrPosition", MemDoRead<void>(stAbsPos),
      "AddrStart",    MemPtr(), "AddrMaximum", MemPtrEnd());
  }
  /* -- Read pointer error handler------------------------------------------ */
  void MemErrorRead[[noreturn]](const char*const cpErrMsg, const size_t stPos,
    const size_t stBytes) const
  { XC(cpErrMsg,
      "Position",  stPos,     "Amount",  stBytes,
      "Maximum",   MemSize(), "AddrPos", MemDoRead<void>(stPos),
      "AddrStart", MemPtr(),  "AddrMax", MemPtrEnd()); }
  /* -- Find a NULL character in the memory -------------------------------- */
  size_t MemFindNull() const
  { // Find the null character and return maximum if not found
    const SpanByteIt sbiIt{ ::std::ranges::find(sbData, '\0') };
    return sbiIt != sbData.end() ?
      static_cast<size_t>(StdDistance(sbData.begin(), sbiIt)) :
      StdMaxSizeT;
  }
  /* -- Set size ----------------------------------------------------------- */
  void MemSetPtr(uint8_t*const ubpNPtr = nullptr)
    { sbData = { ubpNPtr, MemSize() }; }
  void MemSetSize(const size_t stBytes = 0)
    { sbData = { MemPtr<uint8_t>(), stBytes }; }
  void MemSetPtrSize(uint8_t*const ubpNPtr, const size_t stBytes)
    { MemSetPtr(ubpNPtr); MemSetSize(stBytes); }
  /* -- Free the pointer --------------------------------------------------- */
  void MemFreePtr() const              // cppcheck-suppress functionStatic
    { StdFree(MemPtr()); }
  void MemFreePtrIfSet() const { if(MemIsPtrSet()) MemFreePtr(); }
  /* -- Return memory at the allocated address --------------------- */ public:
  template<typename PtrType = void>
    requires (!StdIsPointer<PtrType>)
  PtrType *MemPtr() const
    { return StdToNonConstCast<PtrType*>(sbData.data()); }
  /* -- Return size of allocated memory ------------------------------------ */
  template<typename IntType = size_t>
    requires StdIsArithmatic<IntType>
  IntType MemSize() const { return static_cast<IntType>(sbData.size()); }
  /* -- Return if memory is allocated -------------------------------------- */
  bool MemIsEmpty() const { return MemSize() == 0; }
  bool MemIsNotEmpty() const { return !MemIsEmpty(); }
  /* -- Returns if the pointer is valid ------------------------------------ */
  bool MemIsPtrSet() const             // cppcheck-suppress functionStatic
    { return MemPtr() != nullptr; }
  bool MemIsPtrNotSet() const          // cppcheck-suppress functionStatic
    { return !MemPtr(); }
  /* -- Return ending address ---------------------------------------------- */
  template<typename PtrType = void>
    requires (!StdIsPointer<PtrType>)
  PtrType *MemPtrEnd() const { return MemDoRead<PtrType>(MemSize()); }
  /* -- Return if we can read this amount of data -------------------------- */
  bool MemCheckParam(const size_t stPos, const size_t stBytes) const
    { return stPos + stBytes <= MemSize(); }
  /* -- Same as MemCheckParam() with ptr check ----------------------------- */
  bool MemCheckPtr(const size_t stPos, const size_t stBytes,
    const void*const vpOther) const
  { return MemCheckParam(stPos, stBytes) && vpOther != nullptr; }
  /* -- Test a bit position ------------------------------------------------ */
  bool MemCheckPos(const size_t stPos) const
    { return BitToByte(stPos) >= MemSize(); }
  /* -- Find specified string ---------------------------------------------- */
  template<typename StrType>
    requires StdIsString<StrType>
  size_t MemFind(const StrType &strWhat, size_t stPos = 0) const
  { // Return if invalid position
    if(!MemCheckParam(stPos, strWhat.size()))
      MemErrorRead("Find error!", stPos, strWhat.size());
    // Find the string in our memory block and return position
    const SpanByteIt sbiIt{
      StdSearch(par_unseq, StdNext(sbData.begin(), stPos), sbData.end(),
        strWhat.cbegin(), strWhat.cend()) };
    return sbiIt != sbData.end() ?
      static_cast<size_t>(StdDistance(sbData.begin(), sbiIt)) :
      StdNPos;
  }
  /* -- Read with byte bound check ----------------------------------------- */
  template<typename PtrType = char>
    requires (!StdIsPointer<PtrType>)
  PtrType *MemRead(const size_t stPos, const size_t stBytes = 0) const
  { // Throw error if size bad else return the pointer
    if(!MemCheckParam(stPos, stBytes))
      MemErrorRead("Read error!", stPos, stBytes);
    return MemDoRead<PtrType>(stPos);
  }
  /* -- Read static variable ----------------------------------------------- */
  template<typename IntType>
    requires StdIsArithmatic<IntType>
  IntType MemReadInt(const size_t stPos = 0) const
  { // Copy the UNALIGNED data into the ALIGNED integer and return it.
    IntType itDest;
    StdMemCopy(&itDest, MemRead<void>(stPos, sizeof(IntType)),
      sizeof(IntType));
    return itDest;
  }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType> && (sizeof(IntType) > 1)
  IntType ReadIntLE(const size_t stPos = 0) const
    { return UtilToLittleEndian(MemReadInt<IntType>(stPos)); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType> && (sizeof(IntType) > 1)
  IntType ReadIntBE(const size_t stPos = 0) const
    { return UtilToBigEndian(MemReadInt<IntType>(stPos)); }
  /* -- Test a bit --------------------------------------------------------- */
  bool MemBitTest(const size_t stPos) const
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Test error!", stPos);
    // Return the tested bit
    return BitTest(MemPtr<char>(), stPos);
  }
  /* -- Stringview'ify the memory ------------------------------------------ */
  StdStringView MemToStringViewSafe(const size_t stBytes) const
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
  StdStringView MemToStringViewSafe() const
    { return MemToStringViewSafe(MemSize()); }
  /* -- Stringviewify the memory (already assumes last char is '\0') ------- */
  StdStringView MemToStringView() const
    { return { MemPtr<char>(), MemSize() - 1 }; }
  /* -- Stringview'ify the memory ------------------------------------------ */
  StdString MemToString() const { return { MemPtr<char>(), MemSize() }; }
  /* -- Stringify the memory ----------------------------------------------- */
  StdString MemToStringSafe() const
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
  template<typename IntType = size_t>
    requires StdIsIntegral<IntType>
  bool MemIsSizeOverflow() const
    { return UtilIntWillOverflow<IntType>(MemSize()); }
  /* -- Init from string (does not copy) ----------------------------------- */
  explicit MemConst(const StdString &strRef) :
    /* -- Initialisers ----------------------------------------------------- */
    MemConst{ strRef.size(),           // Copy string size and pointer over
      strRef.data() }                  // from specified string
    /* -- Copy pointer and size over from string --------------------------- */
    {}
  /* -- Assignment constructor (rvalue) ------------------------------------ */
  explicit MemConst(MemConst &&mbOther) :
    /* -- Initialisers ----------------------------------------------------- */
    sbData{ StdMove(mbOther.sbData) }    // Copy pointer and size
    /* -- Code to clear other MemConst ------------------------------------ */
    { mbOther.MemReset(); }
  /* -- Uninitialised constructor -- pointer ------------------------------- */
  MemConst() = default;
  /* -- Inherit an already allocated pointer ------------------------------- */
  MemConst(const size_t stBytes, uint8_t*const ubpNPtr) :
    /* -- Initialisers ----------------------------------------------------- */
    sbData{ ubpNPtr, stBytes }          // Set new pointer and size
  /* -- Check pointer and size --------------------------------------------- */
  { if(MemIsPtrNotSet() && MemIsNotEmpty())
      XC("Null pointer with non-zero memory size requested!",
        "MemSize", MemSize()); }
  /* -- Cast void pointer to char pointer ---------------------------------- */
  MemConst(const size_t stBytes, void*const vpSrc) :
    MemConst(stBytes, reinterpret_cast<uint8_t*>(vpSrc)) {}
  /* -- Cast const char pointer to char pointer ---------------------------- */
  MemConst(const size_t stBytes, const uint8_t*const ubpNPtr) :
    MemConst(stBytes, const_cast<uint8_t*>(ubpNPtr)) {}
  /* -- Cast const void pointer to const char ------------------------------ */
  MemConst(const size_t stBytes, const void*const vpSrc) :
    MemConst(stBytes, const_cast<void*>(vpSrc)) {}
};/* ----------------------------------------------------------------------- */
/* == Read and write data class ============================================ */
class MemBase :
  /* -- Base classes ------------------------------------------------------- */
  public MemConst                      // Start of MemBase Block Class
{ /* -- Copy memory --------------------------------------------- */ protected:
  void MemDoWrite(const size_t stPos,  // cppcheck-suppress functionStatic
                  const void*const vpSrc,
                  const size_t stBytes)
    { StdMemCopy(MemDoRead(stPos), vpSrc, stBytes); }
  /* -- Move memory (use when memory regions overlap) ---------------------- */
  void MemDoMove(const size_t stPos,   // cppcheck-suppress functionStatic
                 const void*const vpSrc,
                 const size_t stBytes)
    { StdMemMove(MemDoRead(stPos), vpSrc, stBytes); }
  /* -- Fill with specified character at specifed position ----------------- */
  template<typename IntType = uint8_t>
    void MemFill(const size_t stPos, const IntType itVal, const size_t stBytes)
  { // If the integer type is a byte size
    if constexpr(sizeof(IntType) == sizeof(uint8_t))
    { // Get starting and ending point
      const SpanByteIt sbiStart{ StdNext(sbData.begin(), stPos) };
      // Do a simple and fast byte fill
      StdFill(par_unseq, sbiStart, StdNext(sbiStart, stBytes), itVal);
    } // Integer type is not byte size?
    else
    { // Brute force the buffer into the specified type
      auto aPattern = StdBruteCast<StdArray<uint8_t, sizeof(IntType)>>(itVal);
      // Create a range for this
      auto aRange{ ::std::views::iota(stPos, stPos + stBytes) };
      ::std::for_each(aRange.begin(), aRange.end(),
        [this, &aPattern](const size_t stIndex) {
          sbData[stIndex] = aPattern[stIndex % aPattern.size()];
        });
    }
  }
  /* -- Fill with specified character at specifed position --------- */ public:
  template<typename IntType = uint8_t>
    requires StdIsArithmatic<IntType>
  void MemFillEx(const size_t stPos,
    const IntType itVal, const size_t stBytes)
  { // Get end position and make sure it wont overrun
    if(!MemCheckParam(stPos, stBytes))
      MemErrorRead("Fill error!", stPos, stBytes);
    // Do the fill
    MemFill<IntType>(stPos, itVal, stBytes);
  }
  /* -- Fill with specified value ------------------------------------------ */
  template<typename IntType = uint8_t>
    requires StdIsArithmatic<IntType>
  void MemFill(const IntType itVal = 0)
    { MemFill<IntType>(0, itVal, MemSize()); }
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
    { MemWriteInt<uint8_t>(stPos, BitSwap4(MemReadInt<uint8_t>(stPos))); }
  /* ----------------------------------------------------------------------- */
  void MemSwap16(const size_t stPos)
    { MemWriteInt<uint16_t>(stPos,
        EndianSwap16(MemReadInt<uint16_t>(stPos))); }
  /* ----------------------------------------------------------------------- */
  void MemSwap32(const size_t stPos)
    { MemWriteInt<uint32_t>(stPos,
        EndianSwap32(MemReadInt<uint32_t>(stPos))); }
  /* ----------------------------------------------------------------------- */
  void MemSwap64(const size_t stPos)
    { MemWriteInt<uint64_t>(stPos,
        EndianSwap64(MemReadInt<uint64_t>(stPos))); }
  /* -- Write specified variable as an integer ----------------------------- */
  template<typename IntType>
    requires StdIsArithmatic<IntType>
  void MemWriteInt(const size_t stPos, const IntType itVal)
    { MemWrite(stPos, &itVal, sizeof(itVal)); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsArithmatic<IntType>
  void MemWriteInt(const IntType itVal)
    { MemWriteInt<IntType>(0, itVal); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsArithmatic<IntType>
  void MemWriteIntLE(const size_t stPos, const IntType itVal)
    { MemWriteInt<IntType>(stPos, UtilToLittleEndian(itVal)); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType> && (sizeof(IntType) > 1)
  void MemWriteIntLE(const IntType itVal)
    { MemWriteInt<IntType>(UtilToLittleEndian(itVal)); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType> && (sizeof(IntType) > 1)
  void MemWriteIntBE(const size_t stPos, const IntType itVal)
    { MemWriteInt<IntType>(stPos, UtilToBigEndian(itVal)); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType> && (sizeof(IntType) > 1)
  void MemWriteIntBE(const IntType itVal)
    { MemWriteInt<IntType>(UtilToBigEndian(itVal)); }
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
    BitSet(MemPtr<char>(), stPos);
  }
  /* -- Clear a bit -------------------------------------------------------- */
  void MemBitClear(const size_t stPos)
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Clear error!", stPos);
    // Clear the bit
    BitClear(MemPtr<char>(), stPos);
  }
  /* -- Flip a bit --------------------------------------------------------- */
  void MemBitFlip(const size_t stPos)
  { // Throw error if invalid position
    if(!MemCheckPos(stPos)) MemCheckBit("Flip error!", stPos);
    // Flip the bit
    BitFlip(MemPtr<char>(), stPos);
  }
  /* -- Invert specific flags ---------------------------------------------- */
  template<typename IntType = uint8_t>
    requires StdIsIntegral<IntType>
  void MemInvert(const size_t stPos,
    const IntType itFlags = StdLimits<IntType>::max())
  { // Bail if size bad
    if(!MemCheckParam(stPos, sizeof(IntType)))
      MemErrorRead("Invert error!", stPos, sizeof(IntType));
    // Do the invert
    *reinterpret_cast<IntType*const>(MemDoRead(stPos)) ^= itFlags;
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
  MemBase(const size_t stBytes, char*const ubpNPtr) :
    MemConst{ stBytes, ubpNPtr } {}
  MemBase(const size_t stBytes, void*const vpSrc) :
    MemConst{ stBytes, vpSrc } {}
  MemBase(const size_t stBytes, const char*const ubpNPtr) :
    MemConst{ stBytes, ubpNPtr } {}
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
    if(uint8_t*const ubpNPtr =
      StdReAlloc(MemPtr<uint8_t>(),
        UtilMaximum(stBytes, static_cast<size_t>(1))))
          MemSetPtrSize(ubpNPtr, stBytes);
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
  /* -- Byte swap ---------------------------------------------------------- */
  template<typename IntType>
  static void MemDoByteSwap(Memory &mOther, const size_t stPos,
      const size_t stBytes, auto &&aFunc)
  { // Get the size of the integral
    constexpr const size_t stSize = sizeof(IntType);
    // Get number of pages to copy
    const size_t stPages = stBytes / stSize;
    // Prepare boundary to write to
    using SpanIntType = StdSpan<IntType>;
    SpanIntType sitData{ mOther.MemDoRead<IntType>(stPos), stPages };
    // Get starting position
    using SpanIntTypeIt = SpanIntType::iterator;
    const SpanIntTypeIt sitiIt{ StdNext(sitData.begin(), stPos / stSize) };
    // Do the transformation
    StdTransform(par_unseq, sitiIt, StdNext(sitiIt, stPages), sitiIt, aFunc);
  }
  /* -- Byte swap ---------------------------------------------------------- */
  template<typename IntType = uint8_t>
    void MemByteSwap(const size_t stPos, const size_t stBytes, auto &&aFunc)
  { // Check position and bytes and return if bad
    if(!MemCheckParam(stPos, stBytes))
       MemErrorRead("ByteSwap error!", stPos, stBytes);
    // If we don't have to care about alignment? Do the aligned byte swap
    constexpr const size_t stIntSize = sizeof(IntType);
    if constexpr(stIntSize == sizeof(uint8_t))
      return MemDoByteSwap<IntType>(*this, stPos, stBytes, aFunc);
      // If we do have care about unaligned position?
    else if constexpr(sizeof(IntType) >= sizeof(uint16_t) ||
                      sizeof(IntType) >= sizeof(uint32_t) ||
                      sizeof(IntType) >= sizeof(uint64_t))
    { // Position is aligned? Aligned so do the aligned byte swap.
      if((stPos & (stIntSize - 1))
          == 0)                    // cppcheck-suppress knownConditionTrueFalse
        return MemDoByteSwap<IntType>(*this, stPos, stBytes, aFunc);
      // Unaligned so copy memory to an ALIGNED buffer
      Memory mWork{ stBytes, MemDoRead(stPos) };
      // Data is now aligned so now we can swap it
      MemDoByteSwap<IntType>(mWork, 0, stBytes, aFunc);
      // Copy the memory back to the unaligned data
      MemDoWrite(stPos, mWork.MemPtr(), stBytes);
    } // Invalid size type
    else static_assert(sizeof(IntType) == 0);
  }
  /* -- Byte swap (Hi4<->Lo4 from 8-bit integer) --------------------------- */
  void MemByteSwap8(const size_t stPos, const size_t stBytes)
    { MemByteSwap<uint8_t>(stPos, stBytes, [](const uint8_t ubVal)->uint8_t
        { return BitReverseByte<uint8_t>(ubVal); }); }
  void MemByteSwap8(const size_t stBytes) { MemByteSwap8(0, stBytes); }
  void MemByteSwap8() { MemByteSwap8(MemSize()); }
  /* -- Byte swap (Hi8<->Lo8 from 16-bit integer) -------------------------- */
  void MemByteSwap16(const size_t stPos, const size_t stBytes)
    { MemByteSwap<uint16_t>(stPos, stBytes, [](const uint16_t usVal)->uint16_t
        { return EndianSwap16(usVal); }); }
  void MemByteSwap16(const size_t stBytes) { MemByteSwap16(0, stBytes); }
  void MemByteSwap16() { MemByteSwap16(MemSize()); }
  /* -- Byte swap (Hi16<->Lo16 from 32-bit integer) ------------------------ */
  void MemByteSwap32(const size_t stPos, const size_t stBytes)
    { MemByteSwap<uint32_t>(stPos, stBytes, [](const uint32_t ulVal)->uint32_t
        { return EndianSwap32(ulVal); }); }
  void MemByteSwap32(const size_t stBytes) { MemByteSwap32(0, stBytes); }
  void MemByteSwap32() { MemByteSwap32(MemSize()); }
  /* -- Byte swap (Hi32<->Lo32 from 64-bit integer) ------------------------ */
  void MemByteSwap64(const size_t stPos, const size_t stBytes)
    { MemByteSwap<uint64_t>(stPos, stBytes, [](const uint64_t ullVal)->uint64_t
        { return EndianSwap64(ullVal); }); }
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
  void MemInitBlank(const size_t stBytesRequested = 0)
  { // If allocated memory already exists? Free it!
    MemFreePtrIfSet();
    // Allocate memory forcing zero bytes to 1 byte for compatibility.
    if(uint8_t*const ubpNPtr =
      StdAlloc<uint8_t>(UtilMaximum(stBytesRequested, static_cast<size_t>(1))))
        return MemSetPtrSize(ubpNPtr, stBytesRequested);
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
  void MemInitString(const StdString &strRef)
    { MemInitData(strRef.size(),
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
    MemBase{ StdMove(mOther) }         // Move other memory object
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
  explicit Memory(const StdString &strRef) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ strRef.size(),             // Allocate memory and copy the string
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
using MemoryList   = StdList<Memory>;   // List of memory blocks
using MemoryVector = StdVector<Memory>; // A vector of memory classes
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
