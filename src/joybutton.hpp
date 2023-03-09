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
using namespace IGlFWUtil::P;          using namespace IStd::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Button class --------------------------------------------------------- */
class JoyButtonInfo
{ /* -- Private variables -------------------------------------------------- */
  const int        iId;                // Button unique identifier
  unsigned int     uiState;            // Current buffered butn press state
  /* -- Get button id ---------------------------------------------- */ public:
  int ButtonGetId(void) const { return iId; }
  /* -- Get buffered state (GLFW_RELEASE, GLFW_PRESS or GLFW_REPEAT) ------- */
  unsigned int ButtonGetState(void) const { return uiState; }
  /* -- Clear button state ------------------------------------------------- */
  void ButtonClearState(void) { uiState = GLFW_RELEASE; }
  /* -- Set button state --------------------------------------------------- */
  void ButtonSetState(const unsigned char*const ucpState)
  { // Save unbuffered state and compare it
    switch(ucpState[ButtonGetId()])
    { // Button is released?
      case GLFW_RELEASE:
        // Reset button state if not released (0 = released)
        if(uiState > GLFW_RELEASE) uiState = GLFW_RELEASE;
        // Done
        return;
      // Button is pressed? Increase button state (1=pressed and 2=held)
      case GLFW_PRESS:
        // Set GLFW_PRESSED if GLFW_RELEASED or
        //     GLFW_REPEAT if GLFW_PRESSED.
        if(uiState < GLFW_REPEAT) ++uiState;
        // Done
        return;
      // Shouldn't get here
      default:
        // Reset to released
        uiState = GLFW_RELEASE;
        // Done
        return;
    }
  }
  /* -- Constructor with unique id ----------------------------------------- */
  explicit JoyButtonInfo(const int iNId) :
    /* -- Initialisers ----------------------------------------------------- */
    iId(iNId),                         // Set unique id
    uiState(GLFW_RELEASE)              // Set default buffered state
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- Button data list --------------------------------------------------- */
template<class Container = array<JoyButtonInfo, 128>,
         class Iterator = typename Container::const_iterator>
class JoyButtonList :
  /* -- Base classes ------------------------------------------------------- */
  private Container                    // Button list values
{ /* ----------------------------------------------------------------------- */
  int              iButtons;           // Button count as int
  /* -- Return button array size ----------------------------------- */ public:
  int JoyButtonListSizeCountClamped(void) const { return iButtons & 0x7f; }
  /* -- Return button array size ------------------------------------------- */
  size_t JoyButtonListSize(void) const { return this->size(); }
  /* -- Return button count ------------------------------------------------ */
  size_t JoyButtonListCount(void) const
    { return static_cast<size_t>(iButtons); }
  /* -- Return button class ------------------------------------------------ */
  const JoyButtonInfo &JoyButtonListGet(const size_t stId) const
    { return (*this)[stId]; }
  /* -- Clear button data -------------------------------------------------- */
  void JoyButtonListClear(void)
    { StdForEach(par_unseq, this->begin(),
        this->begin() + JoyButtonListSizeCountClamped(),
          [](JoyButtonInfo& jbiRef) { jbiRef.ButtonClearState(); }); }
  /* -- Refresh button data ------------------------------------------------ */
  void JoyButtonListRefresh(const int iJId)
  { // Get joystick buttons and if found? Refresh button data for each one
    if(const unsigned char*const ucpData =
       GlFWGetJoystickButtons(iJId, iButtons))
      StdForEach(seq, this->begin(),
        this->begin() + JoyButtonListSizeCountClamped(),
      [ucpData](JoyButtonInfo &jbiData){ jbiData.ButtonSetState(ucpData); });
    else iButtons = 0;
  }
  /* -- Iterators ---------------------------------------------------------- */
  Iterator JoyButtonListBegin(void) const
    { return this->cbegin(); }
  Iterator JoyButtonListEnd(void) const
    { return this->cbegin() + JoyButtonListSizeCountClamped(); }
  /* -- Default constructor ------------------------------------- */ protected:
  JoyButtonList(void) :
    /* -- Initialisers ----------------------------------------------------- */
    Container{ UtilMkFilledClassContainer<Container,int>() },
    iButtons(0)                        // Initialise button count
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
