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
using namespace ICrypt::P;             using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Universally unique identifier helper ================================= */
struct UuId                            // Members initially public
{ /* -- Typedefs ----------------------------------------------------------- */
  const union Struct                   // UUIDv4 structure
  { /* --------------------------------------------------------------------- */
    struct Parts                       // Access to parts
    { /* ------------------------------------------------------------------- */
      uint32_t     dwTimeLow;          // Low 32-bits of the current time
      uint16_t     wTimeMid,           // Middle 16-bits of the current time
                   wTimeHiAndVer;      // 4-bit "version" + 12-bits of the time
      uint8_t      ucClkSeqHiRes,      // 1-3 bit "variant" + 13-15 bit clock
                   ucClkSeqLow,        // As above
                   ucNode[6];          // 48-bit node id
    } p; /* ---------------------------------------------------------------- */
    uint8_t        ucRandom[16];       // As sixteen 8-bit integers (128-bit)
    uint64_t       qwRandom[2];        // As two 64-bit integers    (128-bit)
    /* --------------------------------------------------------------------- */
  } d;                                 // Member to hold uuid data
  /* -- Initialise randomised UUID -------------------------------- */ private:
  const Struct UuIdRandom(void) const
  { // Initialise a random struct
    Struct uuidData;
    CryptRandomPtr(&uuidData.ucRandom, sizeof(uuidData.ucRandom));
    // https://tools.ietf.org/html/rfc4122#section-4.2
    uuidData.p.ucClkSeqHiRes =
      static_cast<uint8_t>((uuidData.p.ucClkSeqHiRes & 0x3F) | 0x80);
    uuidData.p.wTimeHiAndVer =
      static_cast<uint16_t>((uuidData.p.wTimeHiAndVer & 0x0FFF) | 0x4000);
    // Return UUID
    return uuidData;
  }
  /* -- Initialise UUID from two quads ------------------------------------- */
  const Struct UuIdReadTwoQuads(const uint64_t q1, const uint64_t q2) const
  { // Structure to return
    Struct uuidData;
    uuidData.qwRandom[0] = q1;
    uuidData.qwRandom[1] = q2;
    return uuidData;
  }
  /* -- Initialise UUID from C-string -------------------------------------- */
  const Struct UuIdReadString(const string &strUUID) const
  { // Check that the uuid is formatted properly
    if(strUUID.length() != 36  ||      // 00000000-0000-0000-000000000000 [36]
       strUUID[8]       != '-' ||      // 00000000{-}0000-0000-000000000000
       strUUID[13]      != '-' ||      // 00000000-0000{-}0000-000000000000
       strUUID[18]      != '-')        // 00000000-0000-0000{-}000000000000
      return { { 0, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 } } };
    // Return parsed values
    return { {
      // Helper macro
#define HDC(i,m) (CryptHex2Char(static_cast<uint8_t>(strUUID[i]))*m)
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
      { // d.p.ucNode
        static_cast<uint8_t>(HDC(24, 0x10) | HDC(25, 0x1)), // [0]
        static_cast<uint8_t>(HDC(26, 0x10) | HDC(27, 0x1)), // [1]
        static_cast<uint8_t>(HDC(28, 0x10) | HDC(29, 0x1)), // [2]
        static_cast<uint8_t>(HDC(30, 0x10) | HDC(31, 0x1)), // [3]
        static_cast<uint8_t>(HDC(32, 0x10) | HDC(33, 0x1)), // [4]
        static_cast<uint8_t>(HDC(34, 0x10) | HDC(35, 0x1))  // [5]
      } // Done with helper macro
#undef HDC
    } };
  }
  /* -- Convert UUID to string ------------------------------------- */ public:
  const string UuIdToString(void) const
  { // Return result
    return StrAppend(hex, setfill('0'), right,
      setw(8), d.p.dwTimeLow,                                '-', // %08x-
      setw(4), d.p.wTimeMid,                                 '-', // %04x-
      setw(4), d.p.wTimeHiAndVer,                            '-', // %04x-
      setw(2), static_cast<unsigned int>(d.p.ucClkSeqHiRes),      // %02x
      setw(2), static_cast<unsigned int>(d.p.ucClkSeqLow),   '-', // %02x-
      setw(2), static_cast<unsigned int>(d.p.ucNode[0]),          // %02x
      setw(2), static_cast<unsigned int>(d.p.ucNode[1]),          // %02x
      setw(2), static_cast<unsigned int>(d.p.ucNode[2]),          // %02x
      setw(2), static_cast<unsigned int>(d.p.ucNode[3]),          // %02x
      setw(2), static_cast<unsigned int>(d.p.ucNode[4]),          // %02x
      setw(2), static_cast<unsigned int>(d.p.ucNode[5]));         // %02x
  }
  /* -- Constructor to init from string ------------------------------------ */
  explicit UuId(const string &strUUID) :
    /* -- Initialisers ----------------------------------------------------- */
    d{ UuIdReadString(strUUID) }       // Read STL string and store result
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor to initialise random uuid ------------------------------ */
  UuId(void) :
    /* -- Initialisers ----------------------------------------------------- */
    d{ UuIdRandom() }                  // Generate random UUID and store result
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor to intiialise from two quads --------------------------- */
  UuId(const uint64_t q1, const uint64_t q2) :
    /* -- Initialisers ----------------------------------------------------- */
    d{ UuIdReadTwoQuads(q1, q2) }      // Read two integers and store result
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
