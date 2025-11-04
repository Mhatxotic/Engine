/* == MASK.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## 1-bit bitmask collision system.                                     ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IMask {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IDim::P;
using namespace IDir::P;               using namespace IError::P;
using namespace IIdent::P;             using namespace IImage::P;
using namespace IImageDef::P;          using namespace ILockable::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace IMemory::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace ITexDef::P;            using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Mask collector and member class ====================================== */
CTOR_BEGIN_DUO(Masks, Mask, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public MemoryVector,                 // Slots for each mask
  public Lockable,                     // Lua garbage collector instruction
  public Ident,                        // Name of mask object
  public DimInt                        // Size of mask image
{ /* -- Variables ---------------------------------------------------------- */
  size_t           stAlloc;            // Size of all mask bitmaps in array
  /* -- Two masks overlap? ----------------------------------------- */ public:
  bool IsCollide(const size_t stSourceId, const int iSrcX, const int iSrcY,
    const Mask &mCdest, const size_t stDestId, const int iDestX,
    const int iDestY) const
  { // Caculate distances and lengths between both bounds
    const int
      iXMax1 = iSrcX + DimGetWidth(),
      iYMax1 = iSrcY + DimGetHeight(),
      iXMax2 = iDestX + mCdest.DimGetWidth(),
      iYMax2 = iDestY + mCdest.DimGetHeight(),
      iXMin  = UtilMaximum(iSrcX, iDestX),
      iYMin  = UtilMaximum(iSrcY, iDestY),
      iXMax  = UtilMinimum(iXMax1, iXMax2),
      iYMax  = UtilMinimum(iYMax1, iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return false;
    // Get bitmask surfaces for both masks
    const unsigned char*const cpS = at(stSourceId).MemPtr<unsigned char>(),
                       *const cpD = mCdest[stDestId].MemPtr<unsigned char>();
    // Walk through the pixels of the intersection and check the bits and
    // return if we found a match, else try next pixel
    for(int iY = iYMin; iY < iYMax; ++iY)
    { // Pre-calculate source and destination Y
      const int iSrcYPos = (iY - iSrcY) * DimGetWidth(),
                iDestYPos = (iY - iDestY) * mCdest.DimGetWidth();
      // Enumerate X-axis
      for(int iX = iXMin; iX < iXMax; ++iX)
        if(UtilBitTest(cpS, iSrcYPos + (iX - iSrcX)) &&
           UtilBitTest(cpD, iDestYPos + (iX - iDestX)))
          return true;
    }
    // No collision
    return false;
  }
  /* -- Two masks overlap with raycasting? --------------------------------- */
  bool Raycast(const size_t stSourceId, const int iSrcX, const int iSrcY,
    const Mask &mCdest, const size_t stDestId, const int iFromX,
    const int iFromY, int &iToX, int &iToY) const
  { // Make calculations
    int         iDX = abs(iToX - iFromX), iDY = abs(iToY - iFromY),
                 iX = iFromX,              iY = iFromY,
                iLX = iFromX,             iLY = iFromY,
                 iN = 1 + iDX + iDY,   iError = iDX - iDY;
    const int iXinc = iToX > iFromX ? 1 : iToX < iFromX ? -1 : 0,
              iYinc = iToY > iFromY ? 1 : iToY < iFromY ? -1 : 0;
    // Make further calculations
    iDX *= 2;
    iDY *= 2;
    // Iterations
    for(;iN > 0; --iN)
    { // Check collision and return if found? Set last good position
      if(IsCollide(stSourceId, iSrcX, iSrcY, mCdest, stDestId, iX, iY))
        { iToX = iLX; iToY = iLY; return true; }
      // Check for errors
      else if(iError > 0) { iLX = iX; iX += iXinc; iError -= iDY; }
      else                { iLY = iY; iY += iYinc; iError += iDX; }
    }
    // No collision
    return false;
  }
  /* -- Copy specified mask into this one ---------------------------------- */
  void Copy(const size_t stDestId, const Mask &mCsrc,
    const size_t stSourceId, const int iDestX, const int iDestY)
  { // Caculate distances and lengths between both bounds
    const int
      iXMax2 = iDestX + mCsrc.DimGetWidth(),
      iYMax2 = iDestY + mCsrc.DimGetHeight(),
      iXMin  = UtilMaximum(iDestX, 0),
      iYMin  = UtilMaximum(iDestY, 0),
      iXMax  = UtilMinimum(DimGetWidth(), iXMax2),
      iYMax  = UtilMinimum(DimGetHeight(), iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
    const unsigned char*const cpS = mCsrc[stSourceId].MemPtr<unsigned char>();
          unsigned char*const cpD = at(stDestId).MemPtr<unsigned char>();
    // Walk through the pixels of the intersection
    for(int iY = iYMin; iY < iYMax; ++iY)
    { // Pre-calculate Y-axis positions
      const int iDestYPos = iY * DimGetWidth(),
                iSrcYPos = (iY - iDestY) * mCsrc.DimGetWidth();
      // Enumerate X-axis
      for(int iX = iXMin; iX < iXMax; ++iX)
      { // Calculate destination position
        const int iDestPos = iDestYPos + iX;
        // Clear the bits first and copy the bits
        UtilBitClear(cpD, iDestPos);
        UtilBitSet2(cpD, iDestPos, cpS, iSrcYPos + (iX - iDestX));
      }
    }
  }
  /* -- Merge specified mask into this one --------------------------------- */
  void Merge(const Mask &mCsrc, const size_t stSourceId,
    const int iDestX, const int iDestY)
  { // Caculate distances and lengths between both bounds
    const int
      iXMax2 = iDestX + mCsrc.DimGetWidth(),
      iYMax2 = iDestY + mCsrc.DimGetHeight(),
      iXMin  = UtilMaximum(iDestX, 0),
      iYMin  = UtilMaximum(iDestY, 0),
      iXMax  = UtilMinimum(DimGetWidth(), iXMax2),
      iYMax  = UtilMinimum(DimGetHeight(), iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
          unsigned char*const cpD = at(0).MemPtr<unsigned char>();
    const unsigned char*const cpS = mCsrc[stSourceId].MemPtr<unsigned char>();
    // Walk through the Y-axis of the intersection
    for(int iY = iYMin; iY < iYMax; ++iY)
    { // Pre-calculate Y-axis positions
      const int iDestYPos = iY * DimGetWidth(),
                iSrcYPos = (iY - iDestY) * mCsrc.DimGetWidth();
      // Enumerate X-axis and merge the specified bits
      for(int iX = iXMin; iX < iXMax; ++iX)
        UtilBitSet2(cpD, iDestYPos + iX, cpS, iSrcYPos + (iX - iDestX));
    }
  }
  /* -- Erase specified mask into this one --------------------------------- */
  void Erase(const size_t stDestId)
  { // Bail if out of bounds
    if(DimGetWidth() <= 0 || DimGetHeight() <= 0) return;
    // Get bit mask surfaces for both masks
    unsigned char*const cpD = at(stDestId).MemPtr<unsigned char>();
    // Enumerate Y-axis positions
    for(int iY = 0; iY < DimGetHeight(); ++iY)
    { // Calculate destination position
      const int iDestYPos = iY * DimGetWidth();
      // Enumerate X-axis positions and clear the bits
      for(int iX = 0; iX < DimGetWidth(); ++iX)
        UtilBitClear(cpD, iDestYPos + iX);
    }
  }
  /* -- Fill specified mask ------------------------------------------------ */
  void Fill(const int iDX, const int iDY, const int iW, const int iH)
  { // Calculate distances and lengths between both bounds
    const int
      iXMax2 = iDX + iW,                iYMax2 = iDY + iH,
      iXMin  = UtilMaximum(iDX, 0),     iYMin  = UtilMaximum(iDY, 0),
      iXMax  = UtilMinimum(DimGetWidth(), iXMax2),
      iYMax  = UtilMinimum(DimGetHeight(), iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
    unsigned char*const cpD = at(0).MemPtr<unsigned char>();
    // Walk through the pixels of the intersection and set each bit
    for(int iY = iYMin; iY < iYMax; ++iY)
    { // Calculate destination position
      const int iDestYPos = iY * DimGetWidth();
      // Enumerate X-axis positions and fill the bits
      for(int iX = iXMin; iX < iXMax; ++iX)
        UtilBitSet(cpD, iDestYPos + iX);
    }
  }
  /* -- Clear specified mask ----------------------------------------------- */
  void Clear(const int iDX, const int iDY, const int iW, const int iH)
  { // Caculate distances and lengths between both bounds
    const int
      iXMax2 = iDX + iW,               iYMax2 = iDY + iH,
      iXMin  = UtilMaximum(iDX, 0),    iYMin  = UtilMaximum(iDY, 0),
      iXMax  = UtilMinimum(DimGetWidth(), iXMax2),
      iYMax  = UtilMaximum(DimGetHeight(), iYMax2);
    // Bail if out of bounds
    if(iXMax <= iXMin || iYMax <= iYMin) return;
    // Get bitmask surfaces for both masks
    unsigned char*const cpD = at(0).MemPtr<unsigned char>();
    // Walk through the pixels of the intersection and set each bit
    for(int iY = iYMin; iY < iYMax; ++iY)
    { // Calculate destination position
      const int iDestYPos = iY * DimGetWidth();
      // Enumerate X-axis positions and fill the bits
      for(int iX = iXMin; iX < iXMax; ++iX)
        UtilBitClear(cpD, iDestYPos + iX);
    }
  }
  /* -- Init --------------------------------------------------------------- */
  void InitBlank(const string &strName, const unsigned int uiWidth,
    const unsigned int uiHeight)
  { // Check dimension parameters
    if(!uiWidth || !uiHeight ||
       UtilIntWillOverflow<int>(uiWidth) || UtilIntWillOverflow<int>(uiHeight))
      XC("Mask dimensions are invalid!",
         "Identifier", strName, "Width", uiWidth, "Height", uiHeight);
    // Calculate space required, push it into mask list and increment size
    const size_t stLen = (uiWidth * uiHeight) / 8;
    emplace_back(Memory{ stLen });
    stAlloc += stLen;
    // Set name of mask
    IdentSet(StdMove(strName));
    // Set width and height
    DimSet(static_cast<int>(uiWidth), static_cast<int>(uiHeight));
  }
  /* -- Init filled mask --------------------------------------------------- */
  void InitOne(const string &strName, const unsigned int uiWidth,
    const unsigned int uiHeight)
  { // Initialise new mask memory
    InitBlank(strName, uiWidth, uiHeight);
    // Now fill it with 1's
    back().MemFill<uint64_t>(0xFFFFFFFFFFFFFFFF);
  }
  /* -- Init cleared mask -------------------------------------------------- */
  void InitZero(const string &strName, const unsigned int uiWidth,
    const unsigned int uiHeight)
  { // Initialise new mask memory
    InitBlank(strName, uiWidth, uiHeight);
    // Now fill it with zero's
    back().MemFill();
  }
  /* -- Dump a tile to disk ------------------------------------------------ */
  void Dump(const size_t stId, const string &strFile) const
  { // Get source slot
    const MemConst &mcSrc = (*this)[stId];
    // Copy the slot because the image init moves it
    Memory mDst{ mcSrc.MemSize(), mcSrc.MemPtr() };
    // Byte swap it
    mDst.MemByteSwap8();
    // Setup raw image
    const Image imOut{ strFile, StdMove(mDst), DimGetWidth<unsigned int>(),
      DimGetHeight<unsigned int>(), BD_BINARY };
    // Capture exceptions
    try
    { // Save bitmap to PNG
      imOut.SaveFile(imOut.IdentGet(), 0, IFMT_PNG);
    } // exception occured?
    catch(const exception&)
    { // Close the file and delete it
      DirFileUnlink(imOut.IdentGet());
      // Throw original error
      throw;
    }
  }
  /* -- Initialise from an Image class ------------------------------------- */
  void InitFromImage(Image &imC, const unsigned int uiTileWidth,
    const unsigned int uiTileHeight)
  { // Set texture name
    IdentSet(imC);
    // Must have slots
    if(imC.IsNoSlots())
      XC("No data in image object!", "Identifier", IdentGet());
    // Check dimensions. We're also working with ints for sizes so we have
    // to limit the size to signed int range so check for that too.
    if(!uiTileWidth || !uiTileHeight ||
      UtilIntWillOverflow<int>(uiTileWidth) ||
      UtilIntWillOverflow<int>(uiTileHeight))
        XC("Invalid tile dimensions!",
           "Identifier", IdentGet(), "Width", uiTileWidth,
           "Height",     uiTileHeight);
    // Get first image slot and show error as we are not reversing this.
    ImageSlot &bData = imC.GetSlots().front();
    // Check bit depth
    if(imC.GetBitsPerPixel() != 1)
      XC("Image is not monochrome!",
         "Identifier",   IdentGet(),
         "Width",        bData.DimGetWidth(),
         "Height",       bData.DimGetHeight(),
         "BitsPerPixel", imC.GetBitsPerPixel());
    // Check image dimensions too. Again we're dealing with ints!
    if(!bData.DimIsSet() ||
      UtilIntWillOverflow<int>(bData.DimGetWidth()) ||
      UtilIntWillOverflow<int>(bData.DimGetHeight()))
        XC("Invalid image dimensions!",
           "Identifier", IdentGet(), "Width", bData.DimGetWidth(),
           "Height",     bData.DimGetHeight());
    // Image is divisible by 8?
    if(!UtilIsDivisible(bData.DimGetWidth<double>() / 8) ||
       !UtilIsDivisible(bData.DimGetHeight<double>() / 8))
      XC("Image dimensions are not divisible by eight!",
         "Identifier", IdentGet(), "Width", bData.DimGetWidth(),
         "Height",     bData.DimGetHeight());
    // Get reference to the image memory and if no tiling needed?
    if(bData.DimGetWidth() == uiTileWidth &&
       bData.DimGetHeight() == uiTileHeight)
    { // We can just add the full size texture.
      emplace_back(StdMove(bData));
      return;
    } // We're dealing with memory now so we need everything as size_t
    const size_t
      // Tile dimensions
      stTWidth = static_cast<size_t>(uiTileWidth),
      stTHeight = static_cast<size_t>(uiTileHeight),
      // Bitmap dimensions
      stWidth = bData.DimGetWidth<size_t>(),
      stHeight = bData.DimGetHeight<size_t>(),
      stHeightM1 = stHeight - 1,
      // Calculate how many tiles we can fit in the image
      stTotalX = stWidth / stTWidth,
      stTotalY = stHeight / stTHeight,
      // Properly clamped tile count value
      stTotalXWhole = stTotalX * stTWidth,
      stTotalYWhole = stTotalY * stTHeight,
      // Calculate size of the bitmask required
      stBytes = stTWidth * stTHeight / CHAR_BIT;
    // Reserve memory for all the tiles
    reserve(stTotalX * stTotalY);
    // Get source buffer
    const unsigned char*const ucpS = bData.MemPtr<unsigned char>();
    // If bitmap is reversed?
    if(imC.IsReversed())
    { // Get height and width minus one.
      const size_t stTHeightM1 = stTHeight - 1;
      // Start iterating rows from the bottom
      for(size_t stY = stTotalYWhole - stTHeight;
                 stY < stTotalYWhole;
                 stY -= stTHeight)
      { // Iterate columns from the left
        for(size_t stX = 0; stX < stTotalXWhole; stX += stTWidth)
        { // Create cleared mask buffer, insert it into list and get ptr to
          // the memory.
          unsigned char*const ucpD = emplace(cend(),
            Memory{ stBytes, true })->MemPtr<unsigned char>();
          // Copy source to buffer
          for(size_t stTY = stTHeightM1; stTY < stTHeight; --stTY)
          { // Pre-calculate Y position
            const size_t stDestYPos = (stTHeightM1 - stTY) * stTWidth,
                         stSrcYPos = (stHeightM1 - (stY + stTY)) * stWidth;
            // Enumerate X-axis. Note that bits are reversed too.
            for(size_t stTX = 0; stTX < stTWidth; ++stTX)
              UtilBitSet2R(ucpD, stDestYPos + stTX,
                ucpS, stSrcYPos + (stX + stTX));
          }
        }
      }
    } // Iterate each row from the top
    else for(size_t stY = 0; stY < stTotalYWhole; stY += stTHeight)
    { // Iterate each column from the left
      for(size_t stX = 0; stX < stTotalXWhole; stX += stTWidth)
      { // Create cleared mask buffer, insert it into list and get ptr to
        // the memory.
        unsigned char*const ucpD = emplace(cend(),
          Memory{ stBytes, true })->MemPtr<unsigned char>();
        // Copy source to buffer
        for(size_t stTY = 0; stTY < stTHeight; ++stTY)
        { // Pre-calculate Y position
          const size_t stDestYPos = stTY * stTWidth,
                       stSrcYPos = (stHeightM1 - (stY + stTY)) * stWidth;
          // Enumerate X-axis. Note that bits are reversed too.
          for(size_t stTX = 0; stTX < stTWidth; ++stTX)
            UtilBitSet2R(ucpD, stDestYPos + stTX,
              ucpS, stSrcYPos + (stX + stTX));
        }
      }
    } // Set allocated size
    stAlloc = stWidth * stHeight / CHAR_BIT;
    // The mask passed in the arguments is usually still allocated by LUA and
    // will still be registered, sp lets put a note in the mask to show that
    // this function has nicked the mask.
    imC.IdentSetEx("!MAS!$!", imC.IdentGet());
    // Tell log what we did
    cLog->LogInfoExSafe("Mask created $ ($x$) tiles from a $x$ $ bitmask.",
      size(), stTWidth, stTHeight, stWidth, stHeight,
      imC.IsReversed() ? "reversed" : "non-reversed");
    // Set new size and tile size
    DimSet(static_cast<int>(stTWidth), static_cast<int>(stTHeight));
  }
  /* -- Get size of all masks ---------------------------------------------- */
  size_t GetAlloc() const { return stAlloc; }
  /* -- Constructor -------------------------------------------------------- */
  Mask() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperMask{ cMasks, this },      // Register this object in collector
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    stAlloc(0)                         // Uninitialised allocated size
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Masks, Mask, MASK)    // Finish collector class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
