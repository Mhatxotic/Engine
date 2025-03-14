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
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Button class --------------------------------------------------------- */
class JoyButtonInfo
{ /* -- Private variables ----------------------------------------- */ private:
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
    switch(const unsigned int uiUnbuffered =
      static_cast<unsigned int>(ucpState[ButtonGetId()]))
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
  class JoyButtonList : private Container
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
    /* -- Initialise joystick buttons -------------------------------------- */
#define JBLX(x) JoyButtonInfo{ x }
#define JBL(n) JBLX(GLFW_GAMEPAD_BUTTON_ ## n)
    /* --------------------------------------------------------------------- */
    Container{{                        // Initialise joystick buttons ids
      /* ------------------------------------------------------------------- */
      JBL(A),           JBL(B),            JBL(X),           JBL(Y),
      JBL(LEFT_BUMPER), JBL(RIGHT_BUMPER), JBL(BACK),        JBL(START),
      JBL(GUIDE),       JBL(LEFT_THUMB),   JBL(RIGHT_THUMB), JBL(DPAD_UP),
      JBL(DPAD_RIGHT),  JBL(DPAD_DOWN),    JBL(DPAD_LEFT),
      JBLX( 15),JBLX( 16),JBLX( 17),JBLX( 18),JBLX( 19),JBLX( 20),JBLX( 21),
      JBLX( 22),JBLX( 23),JBLX( 24),JBLX( 25),JBLX( 26),JBLX( 27),JBLX( 28),
      JBLX( 29),JBLX( 30),JBLX( 31),JBLX( 32),JBLX( 33),JBLX( 34),JBLX( 35),
      JBLX( 36),JBLX( 37),JBLX( 38),JBLX( 39),JBLX( 40),JBLX( 41),JBLX( 42),
      JBLX( 43),JBLX( 44),JBLX( 45),JBLX( 46),JBLX( 47),JBLX( 48),JBLX( 49),
      JBLX( 50),JBLX( 51),JBLX( 52),JBLX( 53),JBLX( 54),JBLX( 55),JBLX( 56),
      JBLX( 57),JBLX( 58),JBLX( 59),JBLX( 60),JBLX( 61),JBLX( 62),JBLX( 63),
      JBLX( 64),JBLX( 65),JBLX( 66),JBLX( 67),JBLX( 68),JBLX( 69),JBLX( 70),
      JBLX( 71),JBLX( 72),JBLX( 73),JBLX( 74),JBLX( 75),JBLX( 76),JBLX( 77),
      JBLX( 78),JBLX( 79),JBLX( 80),JBLX( 81),JBLX( 82),JBLX( 83),JBLX( 84),
      JBLX( 85),JBLX( 86),JBLX( 87),JBLX( 88),JBLX( 89),JBLX( 90),JBLX( 91),
      JBLX( 92),JBLX( 93),JBLX( 94),JBLX( 95),JBLX( 96),JBLX( 97),JBLX( 98),
      JBLX( 99),JBLX(100),JBLX(101),JBLX(102),JBLX(103),JBLX(104),JBLX(105),
      JBLX(106),JBLX(107),JBLX(108),JBLX(109),JBLX(110),JBLX(111),JBLX(112),
      JBLX(113),JBLX(114),JBLX(115),JBLX(116),JBLX(117),JBLX(118),JBLX(119),
      JBLX(120),JBLX(121),JBLX(122),JBLX(123),JBLX(124),JBLX(125),JBLX(126),
      JBLX(127)
    /* ------------------------------------------------------------------- */
    }},                                // End of joystick button ids init
    /* ------------------------------------------------------------------- */
#undef JBL                             // Done with this macro
#undef JBX                             // Done with this macro
    /* --------------------------------------------------------------------- */
    iButtons(0)                        // Initialise button count
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(JoyButtonList)       // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
