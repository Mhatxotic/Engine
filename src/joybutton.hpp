/* == JOYBUTTON.HPP ======================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines button classes for joystick access.             ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IJoyButton {                 // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IFillCon::P;           using namespace IGlFWUtil::P;
using namespace IStd::P;               using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Button class --------------------------------------------------------- */
class JoyButtonInfo
{ /* -- Private variables -------------------------------------------------- */
  const int        iId;                // Button unique identifier
  unsigned         uState;             // Current buffered butn press state
  /* -- Get button id ---------------------------------------------- */ public:
  template<typename IntType = int>
    IntType ButtonGetId() const { return static_cast<IntType>(iId); }
  /* -- Get buffered state (GLFW_RELEASE, GLFW_PRESS or GLFW_REPEAT) ------- */
  unsigned ButtonGetState() const { return uState; }
  /* -- Clear button state ------------------------------------------------- */
  void ButtonClearState() { uState = GLFW_RELEASE; }
  /* -- Set button state --------------------------------------------------- */
  void ButtonSetState(const unsigned char ucValue)
  { // Save unbuffered state and compare it
    switch(ucValue)
    { // Button is released?
      case GLFW_RELEASE:
        // Reset button state if not released (0 = released)
        if(uState > GLFW_RELEASE) uState = GLFW_RELEASE;
        // Done
        return;
      // Button is pressed? Increase button state (1=pressed and 2=held)
      case GLFW_PRESS:
        // Set GLFW_PRESSED if GLFW_RELEASED or
        //     GLFW_REPEAT if GLFW_PRESSED.
        if(uState < GLFW_REPEAT) ++uState;
        // Done
        return;
      // Shouldn't get here
      default:
        // Reset to released
        uState = GLFW_RELEASE;
        // Done
        return;
    }
  }
  /* -- Constructor with unique id ----------------------------------------- */
  explicit JoyButtonInfo(const int iNId) :
    /* -- Initialisers ----------------------------------------------------- */
    iId(iNId),                         // Set unique id
    uState(GLFW_RELEASE)               // Set default buffered state
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- Button data list --------------------------------------------------- */
template<class Container = StdArray<JoyButtonInfo, 128>,
         class Iterator = typename Container::const_iterator>
class JoyButtonList :
  /* -- Base classes ------------------------------------------------------- */
  private Container                    // Button list values
{ /* ----------------------------------------------------------------------- */
  int              iButtons;           // Button count as int
  /* -- Return button array size ----------------------------------- */ public:
  int JoyButtonListSizeCountClamped() const { return iButtons & 0x7f; }
  /* -- Return button array size ------------------------------------------- */
  size_t JoyButtonListSize() const { return this->size(); }
  /* -- Return button count ------------------------------------------------ */
  size_t JoyButtonListCount() const
    { return static_cast<size_t>(iButtons); }
  /* -- Return button class ------------------------------------------------ */
  const JoyButtonInfo &JoyButtonListGet(const size_t stId) const
    { return (*this)[stId]; }
  /* -- Clear button data -------------------------------------------------- */
  void JoyButtonListClear()
    { StdForEach(par_unseq, this->begin(),
        StdNext(this->begin(), JoyButtonListSizeCountClamped()),
          [](JoyButtonInfo& jbiRef) { jbiRef.ButtonClearState(); }); }
  /* -- Refresh button data ------------------------------------------------ */
  void JoyButtonListRefresh(const int iJId)
  { // Get joystick buttons and if found?
    if(const unsigned char*const ucpData =
       GlFWGetJoystickButtons(iJId, iButtons))
    { // Put raw data into a protective barrier
      StdSpan<const unsigned char> ucaData
        { ucpData, static_cast<size_t>(iButtons) };
      // Refresh button data for each one
      StdForEach(seq, this->begin(),
        StdNext(this->begin(), JoyButtonListSizeCountClamped()),
      [&ucaData](JoyButtonInfo &jbiData)
        { jbiData.ButtonSetState(ucaData[jbiData.ButtonGetId<size_t>()]); });
    } // Polling buttons failed
    else iButtons = 0;
  }
  /* -- Iterators ---------------------------------------------------------- */
  Iterator JoyButtonListBegin() const
    { return this->cbegin(); }
  Iterator JoyButtonListEnd() const
    { return this->cbegin() + JoyButtonListSizeCountClamped(); }
  /* -- Default constructor ------------------------------------- */ protected:
  JoyButtonList() :
    /* -- Initialisers ----------------------------------------------------- */
    Container{ FillConClass<Container,int>() },
    iButtons(0)                        // Initialise button count
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
