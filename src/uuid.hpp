/* == UUID.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class allows UUID v4's to be generated, encoded and decoded.   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IUuId {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICrypt::P;             using namespace IStd::P;
using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Universally unique identifier helper ================================= */
class UuId                             // Members initially public
{ /* -- Private typedefs --------------------------------------------------- */
  typedef StdArray<uint8_t, 6>  Sextuple;   // 6 bytes
  typedef StdArray<uint8_t, 16> Sexdecuple; // 16 bytes
  typedef StdArray<uint64_t, 2> Double;     // 2 quad words (16 bytes)
  /* -- Typedefs --------------------------------------------------- */ public:
  const union Struct                   // UUIDv4 structure
  { /* -- It's safe to use arrays in unions since C++11 -------------------- */
    struct Parts                       // Access to parts
    { /* -- Public variables ----------------------------------------------- */
      uint32_t     dwTimeLow;          // Low 32-bits of the current time
      uint16_t     wTimeMid,           // Middle 16-bits of the current time
                   wTimeHiAndVer;      // 4-bit "version" + 12-bits of the time
      uint8_t      ucClkSeqHiRes,      // 1-3 bit "variant" + 13-15 bit clock
                   ucClkSeqLow;        // As above
      Sextuple     sxtNode;            // Node id (48-bit)
      /* -- Default constructor -------------------------------------------- */
      Parts() :
        /* -- Initialisers ------------------------------------------------- */
        dwTimeLow(0),                  wTimeMid(0),
        wTimeHiAndVer(0),              ucClkSeqHiRes(0),
        ucClkSeqLow(0),                sxtNode{ 0, 0, 0, 0, 0, 0 }
        /* -- No code ------------------------------------------------------ */
        {}
      /* -- Initialiser constructor ---------------------------------------- */
      Parts(const uint32_t dwNTimeLow, const uint16_t wNTimeMid,
        const uint16_t wNTimeHiAndVer, const uint8_t ucNClkSeqHiRes,
        const uint8_t ucNClkSeqLow, const Sextuple &sxtNNode) :
        /* -- Initialisers ------------------------------------------------- */
        dwTimeLow(dwNTimeLow),         wTimeMid(wNTimeMid),
        wTimeHiAndVer(wNTimeHiAndVer), ucClkSeqHiRes(ucNClkSeqHiRes),
        ucClkSeqLow(ucNClkSeqLow),     sxtNode{ sxtNNode }
        /* -- No code ------------------------------------------------------ */
        {}
    } p; /* ---------------------------------------------------------------- */
    Sexdecuple     sdcRandom;        // As sixteen 8-bit integers (128-bit)
    Double         dbRandom;         // As two 64-bit integers    (128-bit)
    /* -- Default constructor ---------------------------------------------- */
    Struct() :
      /* -- Initialisers --------------------------------------------------- */
      p{}                            // Run default constructor (required)
      /* -- No code -------------------------------------------------------- */
      {}
    /* -- Initialise from two 64-bit values -------------------------------- */
    Struct(const uint64_t ull1, const uint64_t ull2) :
      /* -- Initialisers --------------------------------------------------- */
      dbRandom{ ull1, ull2 }         // Initialise the two shared 64-bit ints
      /* -- No code -------------------------------------------------------- */
      {}
    /* -- Initialise random data ------------------------------------------- */
    Struct(bool) :
      /* -- Initialisers --------------------------------------------------- */
      sdcRandom{ CryptRandom<Sexdecuple>() }
      /* -- Initialise version --------------------------------------------- */
      { // Set the UUID variant to RFC 4122 (bits 6-7 set to 10 binary / 0x80)
        p.ucClkSeqHiRes =
          static_cast<uint8_t>((p.ucClkSeqHiRes & 0x3F) | 0x80);
        // Set the UUID version to 4 - Randomly generated (bits 12-15 set to
        // 0100 binary / 0x4000)
        p.wTimeHiAndVer =
          static_cast<uint16_t>((p.wTimeHiAndVer & 0x0FFF) | 0x4000);
      }
    /* -- Initialiser constructor ------------------------------------------ */
    Struct(const uint32_t dwNTimeLow, const uint16_t wNTimeMid,
      const uint16_t wNTimeHiAndVer, const uint8_t ucNClkSeqHiRes,
      const uint8_t ucNClkSeqLow, const Sextuple &sxtNNode) :
      /* -- Initialisers --------------------------------------------------- */
      p{ dwNTimeLow, wNTimeMid, wNTimeHiAndVer, ucNClkSeqHiRes, ucNClkSeqLow,
         sxtNNode }                    // Initialise union
      /* -- No code -------------------------------------------------------- */
      {}
    /* --------------------------------------------------------------------- */
  } d;                                 // Member to hold uuid data
  /* -- Initialise UUID from a container (thick) ------------------ */ private:
  static Struct UuIdReadStringStr(auto &&aData)
  { // Check that the uuid is formatted properly
    if(aData.size() != 36 ||           // 00000000-0000-0000-000000000000 [36]
       aData[8]  != '-' ||             // 00000000{-}0000-0000-000000000000
       aData[13] != '-' ||             // 00000000-0000{-}0000-000000000000
       aData[18] != '-') [[unlikely]]  // 00000000-0000-0000{-}000000000000
      return { 0, 0, 0, 0, 0, {{ 0, 0, 0, 0, 0, 0 }} };
    // Helper function to modify each index with bits
    auto HDC = [&aData](const size_t stIndex, const int iBits)->int {
      return CryptHex2Char<0>(static_cast<unsigned char>(aData[stIndex])) *
        iBits; };
    // Return parsed values
    return {
      // Helper macro
      static_cast<uint32_t>(           // d.p.dwTimeLow
        HDC(0, 0x10000000) | HDC(1, 0x1000000) | HDC(2, 0x100000) |
        HDC(3, 0x10000)    | HDC(4, 0x1000)    | HDC(5, 0x100)    |
        HDC(6, 0x10)       | HDC(7, 0x1)),
      static_cast<uint16_t>(           // d.p.wTimeMid
        HDC(9, 0x1000) | HDC(10, 0x100) | HDC(11, 0x10) | HDC(12, 0x1)),
      static_cast<uint16_t>(           // d.p.wTimeHiAndVer
        HDC(14, 0x1000) | HDC(15, 0x100) | HDC(16, 0x10) | HDC(17, 0x1)),
      static_cast<uint8_t>(HDC(19, 0x10) | HDC(20, 0x1)), // d.p.ucClkSeqHiRes
      static_cast<uint8_t>(HDC(21, 0x10) | HDC(22, 0x1)), // d.p.ucClkSeqLow
      {{ // d.p.sxtNode
        static_cast<uint8_t>(HDC(24, 0x10) | HDC(25, 0x1)), // [0]
        static_cast<uint8_t>(HDC(26, 0x10) | HDC(27, 0x1)), // [1]
        static_cast<uint8_t>(HDC(28, 0x10) | HDC(29, 0x1)), // [2]
        static_cast<uint8_t>(HDC(30, 0x10) | HDC(31, 0x1)), // [3]
        static_cast<uint8_t>(HDC(32, 0x10) | HDC(33, 0x1)), // [4]
        static_cast<uint8_t>(HDC(34, 0x10) | HDC(35, 0x1))  // [5]
      }} // Done with helper macro
    };
  }
  /* -- Initialise UUID from C-string (thin) ------------------------------- */
  template<class StrType>static Struct UuIdReadString(StrType &&strStr)
    { return StdNormaliseString(StdForward<StrType>(strStr),
        [](auto &&aStr)->Struct{
          return UuIdReadStringStr(StdForward<decltype(aStr)>(aStr)); }); }
  /* -- Convert UUID to string ------------------------------------- */ public:
  StdString UuIdToString() const
  { // Return result
    return StrAppend(StdIOSHex, StdIOSSetFill('0'), StdIOSRight,
      StdIOSSetWidth(8), d.p.dwTimeLow,                         '-', // %08x-
      StdIOSSetWidth(4), d.p.wTimeMid,                          '-', // %04x-
      StdIOSSetWidth(4), d.p.wTimeHiAndVer,                     '-', // %04x-
      StdIOSSetWidth(2), static_cast<unsigned>(d.p.ucClkSeqHiRes),   // %02x
      StdIOSSetWidth(2), static_cast<unsigned>(d.p.ucClkSeqLow),'-', // %02x-
      StdIOSSetWidth(2), static_cast<unsigned>(d.p.sxtNode[0]),      // %02x
      StdIOSSetWidth(2), static_cast<unsigned>(d.p.sxtNode[1]),      // %02x
      StdIOSSetWidth(2), static_cast<unsigned>(d.p.sxtNode[2]),      // %02x
      StdIOSSetWidth(2), static_cast<unsigned>(d.p.sxtNode[3]),      // %02x
      StdIOSSetWidth(2), static_cast<unsigned>(d.p.sxtNode[4]),      // %02x
      StdIOSSetWidth(2), static_cast<unsigned>(d.p.sxtNode[5]));     // %02x
  }
  /* -- Constructor to init from string ------------------------------------ */
  explicit UuId(auto &&aStr) :
    /* -- Initialisers ----------------------------------------------------- */
    d{ UuIdReadString(aStr) }          // Read STL string and store result
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor to initialise random uuid ---------------------- */
  UuId() :
    /* -- Initialisers ----------------------------------------------------- */
    d{ true }                          // Generate random UUID and store result
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor to intiialise from two quads --------------------------- */
  UuId(const uint64_t ull1, const uint64_t ull2) :
    /* -- Initialisers ----------------------------------------------------- */
    d{ ull1, ull2 }                    // Read two integers and store result
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
