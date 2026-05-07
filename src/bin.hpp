/* == BIN.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles packing of 2D rectangles into a limited         ## **
** ## rectangular space.                                                  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IBin {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IDim::P;
using namespace IDimCoord::P;          using namespace ILockable::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace IName::P;
using namespace ISerial::P;            using namespace IStd::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<typename IntType = int,
         typename UIntType = StdMakeUnsigned<IntType>,
         class DimClass = Dimensions<IntType>>
  requires StdIsSigned<IntType>
class Pack :
  /* -- Initialisers ------------------------------------------------------- */
  public DimClass                      // Dimensions of bin in pixels
{ /* --------------------------------------------------------------- */ public:
  using Rect     = DimCoords<IntType>; // Rectangle of signed ints
  using RectList = StdVector<Rect>;    // list of rectangles
  constexpr static double dIdle = -0.0;// Uninitialised value
  /* -------------------------------------------------------------- */ private:
  RectList         rlUsed, rlFree;     // Used and free data
  /* -- Remove a rect (not that this can cause a realloc) ------------------ */
  void PruneFreeRect(const size_t stIndex)
    { rlFree.erase(StdNext(rlFree.cbegin(), static_cast<ssize_t>(stIndex))); }
  /* -- Add a rect (note this can cause a realloc) ------------------------- */
  void AddFreeRect(const IntType itX, const IntType itY, const IntType itW,
    const int itH) { rlFree.push_back({ itX, itY, itW, itH }); }
  /* ----------------------------------------------------------------------- */
  const Rect FindPositionForNewNodeBestShortSideFit(const IntType itW,
    const IntType itH, IntType &itBestShortSideFit, IntType &itBestLongSideFit)
      const
  { // The best node that will be returned
    itBestShortSideFit = itBestLongSideFit = StdLimits<IntType>::max();
    Rect rFound;
    for(const Rect &rNode : rlFree)
    { // Try to place the rectangle in upright (non-flipped) orientation.
      if(rNode.DimGetWidth() < itW || rNode.DimGetHeight() < itH) continue;
      const IntType
        itLeftOverHoriz = abs(rNode.DimGetWidth() - itW),
        itLeftOverVert = abs(rNode.DimGetHeight() - itH),
        iShortSideFit = UtilMinimum(itLeftOverHoriz, itLeftOverVert),
        iLongSideFit = UtilMaximum(itLeftOverHoriz, itLeftOverVert);
      if(iShortSideFit < itBestShortSideFit ||
        (iShortSideFit == itBestShortSideFit &&
         iLongSideFit < itBestLongSideFit))
      {
        itBestShortSideFit = iShortSideFit;
        itBestLongSideFit = iLongSideFit;
        rFound.DimCoSet(rNode.CoordGetX(), rNode.CoordGetY(), itW, itH);
      }
    } // Return what we found if we did
    return rFound;
  }
  /* -- Do try to split a free node and ------------------------------------ */
  void SplitFreeNodeUnsafe(const Rect &rFree, const Rect &rUsed,
    const IntType itFreeH, const IntType itUsedH, const IntType itFreeV,
    const IntType itUsedV)
  { // Check horizontal bounds
    if(rUsed.CoordGetX() < itFreeH && itUsedH > rFree.CoordGetX())
    { // New node at the top side of the used node.
      if(rUsed.CoordGetY() > rFree.CoordGetY() && rUsed.CoordGetY() < itFreeV)
        AddFreeRect(rFree.CoordGetX(), rFree.CoordGetY(),
          rFree.DimGetWidth(), rUsed.CoordGetY() - rFree.CoordGetY());
      // New node at the bottom side of the used node.
      if(itUsedV < itFreeV)
        AddFreeRect(rFree.CoordGetX(), itUsedV,
          rFree.DimGetWidth(), itFreeV - itUsedV);
    } // Check vertical bounds
    if(rUsed.CoordGetY() < itFreeV && itUsedV > rFree.CoordGetY())
    { // New node at the left side of the used node.
      if(rUsed.CoordGetX() > rFree.CoordGetX() && rUsed.CoordGetX() < itFreeH)
        AddFreeRect(rFree.CoordGetX(), rFree.CoordGetY(),
          rUsed.CoordGetX() - rFree.CoordGetX(), rFree.DimGetHeight());
      // New node at the right side of the used node.
      if(itUsedH < itFreeH)
        AddFreeRect(itUsedH, rFree.CoordGetY(),
          itFreeH - itUsedH, rFree.DimGetHeight());
    }
  }
  /* -- Try to split a free node and return true if succeeded -------------- */
  bool SplitFreeNode(const size_t stIndex, const Rect &rUsed)
  { // Get reference to free node
    const Rect &rFree = rlFree[stIndex];
    // Calculate maximum bounds of the free and used rslRects.
    const IntType itFreeH = rFree.CoordGetX() + rFree.DimGetWidth(),
              itUsedH = rUsed.CoordGetX() + rUsed.DimGetWidth(),
              itFreeV = rFree.CoordGetY() + rFree.DimGetHeight(),
              itUsedV = rUsed.CoordGetY() + rUsed.DimGetHeight();
    // Test with SAT if the rectangles even intersect.
    if(rUsed.CoordGetX() >= itFreeH || itUsedH <= rFree.CoordGetX() ||
       rUsed.CoordGetY() >= itFreeV || itUsedV <= rFree.CoordGetY())
      return false;
    // DANGER! There must be at least four reserved vector nodes or a
    // push_back() could trigger a realloc() being a STL vector and &rFree
    // would in succession point to freed memory and all havoc breaks lose in
    // the engine! So if there is not enough room for four more push_back()'s?
    if(rlFree.capacity() - rlFree.size() < 4)
    { // Not enough room so try to reserve up to four more items. Reserve()
      // will not grow exponentially so we should handle that instead.
      rlFree.reserve(UtilNearestPow2<size_t>(rlFree.size() + 4));
      // Do the split with a NEW evaluation of rlFree[stIndex] as again, the
      // address of rlFree[stIndex] may have changed!
      SplitFreeNodeUnsafe(rlFree[stIndex], rUsed,
        itFreeH, itUsedH, itFreeV, itUsedV);
    } // Enough room so we can just continue with the existing reference.
    else SplitFreeNodeUnsafe(rFree, rUsed, itFreeH, itUsedH, itFreeV, itUsedV);
    // Success!
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool IsContainedIn(const Rect &rSrc, const Rect &rDest)
  { // Return if the src rect bounds are contained in dest bounds
    return rSrc.CoordGetX() >= rDest.CoordGetX()
        && rSrc.CoordGetY() >= rDest.CoordGetY()
        && rSrc.CoordGetX() + rSrc.DimGetWidth() <=
           rDest.CoordGetX() + rDest.DimGetWidth()
        && rSrc.CoordGetY() + rSrc.DimGetHeight() <=
           rDest.CoordGetY() + rDest.DimGetHeight();
  }
  /* -- Goes through free rectangle list and removes any redundant rects --- */
  void PruneFreeList()
  { // Go through each free rectangle
    for(size_t stIndex = 0; stIndex < rlFree.size(); ++stIndex)
    { // Compare with all other rectangles
      for(size_t stSubIndex = stIndex + 1;
                 stSubIndex < rlFree.size();
               ++stSubIndex)
      { // If rectangle sub-rect is inside this rect
        if(IsContainedIn(rlFree[stIndex], rlFree[stSubIndex]))
        { // Remove it
          PruneFreeRect(stIndex);
          // Decrement index
          --stIndex;
          // We're done with the sub-rect
          break;
        } // Ignore if subrect is inside this rect
        if(!IsContainedIn(rlFree[stSubIndex], rlFree[stIndex])) continue;
        // Remove sub-rect
        PruneFreeRect(stSubIndex);
        // Decrement index
        --stSubIndex;
      }
    }
  }
  /* -- Swap --------------------------------------------------------------- */
  void BinSwap(Pack &pOther)
  { // Swap dimensions
    DimSwap(pOther);
    // Swap used and free lists
    rlUsed.swap(pOther.rlUsed);
    rlFree.swap(pOther.rlFree);
  }
  /* -- (Re)initialise bin to empty state -------------------------- */ public:
  void Init(const UIntType uitNWidth, const UIntType uitNHeight)
  { // Init itW and itH
    this->DimSet(static_cast<IntType>(uitNWidth),
      static_cast<IntType>(uitNHeight));
    // Clear current usage
    rlUsed.clear();
    rlFree.clear();
    // Full bounds initially available
    AddFreeRect(0, 0, this->DimGetWidth(), this->DimGetHeight());
  }
  /* -- Reserve memory ----------------------------------------------------- */
  void Reserve(const size_t stUsedReserve, const size_t stFreeReserve)
    { rlUsed.reserve(stUsedReserve); rlFree.reserve(stFreeReserve); }
  /* -- (Re)initialise bin to empty state ---------------------------------- */
  void Init(const UIntType uitNWidth, const UIntType uitNHeight,
     const size_t stUsedReserve, const size_t stFreeReserve)
  { // Initialise rect
    Init(uitNWidth, uitNHeight);
    // Reserve memory
    Reserve(stUsedReserve, stFreeReserve);
  }
  /* -- Englarge the bin --------------------------------------------------- */
  bool Resize(const UIntType uitNWidth, const UIntType uitNHeight)
  { // If there is nothing in the bin?
    if(rlUsed.empty())
    { // Just reinitialise it
      Init(uitNWidth, uitNHeight);
      // Success
      return true;
    } // Convert values to int
    const int iNHeight = static_cast<IntType>(uitNHeight);
    const int iNWidth = static_cast<IntType>(uitNWidth);
    // New width same as old?
    if(iNWidth <= this->DimGetWidth())
    { // Return if new height same as old
      if(iNHeight <= this->DimGetHeight()) return false;
    } // New width specified?
    else
    { // New width specified so add new free rect and update it
      AddFreeRect(this->DimGetWidth(), 0,
        iNWidth - this->DimGetWidth(), this->DimGetHeight());
      this->DimSetWidth(iNWidth);
      // New height not specified? We're done
      if(iNHeight <= this->DimGetHeight()) return true;
    } // New height specified so add new free rect and update it
    AddFreeRect(0, this->DimGetHeight(), iNWidth,
      iNHeight - this->DimGetHeight());
    this->DimSetHeight(iNHeight);
    // Englarge complete
    return true;
  }
  /* -- Test insert a single rectangle into the bin ------------------------ */
  const Rect Test(const UIntType uitW, const UIntType uitH) const
  { // Initialise two score values and init to maximum supported int value
    IntType itScore1 = StdLimits<IntType>::max(), itScore2 = itScore1;
    // Find position for new node and return if the size will fit
    return FindPositionForNewNodeBestShortSideFit(static_cast<IntType>(uitW),
      static_cast<IntType>(uitH), itScore1, itScore2);
  }
  /* -- Inserts a single rectangle into the bin ---------------------------- */
  const Rect Insert(const UIntType uitW, const UIntType uitH)
  { // Initialise two score values and init to maximum supported int value
    IntType itScore1 = StdLimits<IntType>::max(), itScore2 = itScore1;
    // Find position for new node and return zero (rNew={0,0}) if failed
    const Rect rNew{ FindPositionForNewNodeBestShortSideFit(
      static_cast<IntType>(uitW), static_cast<IntType>(uitH),
      itScore1, itScore2) };
    if(rNew.DimGetHeight() == 0) return rNew;
    // Get number of free rectangles and enumerate through them
    size_t stRectanglesToProcess = rlFree.size();
    for(size_t stIndex = 0; stIndex < stRectanglesToProcess; ++stIndex)
    { // Split the free node to accommodate new value and ignore if failed
      if(!SplitFreeNode(stIndex, rNew)) continue;
      // Remove from free list and update index and counter
      PruneFreeRect(stIndex);
      --stIndex;
      --stRectanglesToProcess;
    } // Remove redundant rectangles
    PruneFreeList();
    // Insert new rect into used list
    rlUsed.emplace_back(rNew);
    // Return size to caller
    return rNew;
  }
  /* -- Returns the % total filled ----------------------------------------- */
  double Occupancy() const
  { // Return the ratio of used surface area to the total bin area.
    return StdAccumulate(rlUsed.cbegin(), rlUsed.cend(), 0.0,
      [](double dUsed, const Rect &rNode)
        { return dUsed += rNode.template DimGetWidth<double>() *
                          rNode.DimGetHeight(); }) /
      (this->template DimGetWidth<double>() * this->DimGetHeight());
  }
  /* -- Return number of rectangles created in the used list --------------- */
  size_t Used() const { return rlUsed.size(); }
  /* -- Return number of rectangles created in the free list --------------- */
  size_t Free() const { return rlFree.size(); }
  /* -- Return number of total rectangles created -------------------------- */
  size_t Total() const { return rlFree.size() + rlUsed.size(); }
  /* -- Default constructor that instantiates an empty bin of size 0x0 ----- */
  Pack()
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Instantiates a bin of the given size with pre-reserved memory ------ */
  Pack(const UIntType uitNWidth,         // Set width of new pack
       const UIntType uitNHeight,        // Set height of new pack
       const size_t stUsedReserve,     // Used list initial size
       const size_t stFreeReserve) :   // Free list initial size
    /* -- Initialisers ----------------------------------------------------- */
    Pack{ uitNWidth, uitNHeight }          // Initialise pack
    /* -- Reserve list sizes ----------------------------------------------- */
    { Reserve(stUsedReserve, stFreeReserve); }
  /* -- Instantiates a bin of the given size ------------------------------- */
  Pack(const UIntType uitNWidth,         // Width of new pack
       const UIntType uitNHeight) :      // Height of new pack
    /* -- Initialisers ----------------------------------------------------- */
    DimClass{ static_cast<IntType>(uitNWidth),    // Set width of new pack
              static_cast<IntType>(uitNHeight) }, // Set height of new pack
    rlFree{{ 0, 0,                     // Initialise co-ordinates
      this->DimGetWidth(),             // Initialise specified width
      this->DimGetHeight() }}          // Initialise specified height
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == Bin object collector and member class ================================ */
CTOR_BEGIN_DUO(Bins, Bin, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua garbage collector instruction
  public Pack<int>                     // Base Pack class with GLint's
{ /* -- Default constructor ---------------------------------------- */ public:
  Bin() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperBin{ cBins, this },        // Register the object in collector
    SerialSlave{ cParent->Serial() }   // Initialise identification number
    /* --------------------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Bins, Bin, BIN)       // End of bin objects collector
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
