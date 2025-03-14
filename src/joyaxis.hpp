/* == JOYAXIS.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines the axis classes for joystick access.           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IJoyAxis {                   // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IGlFWUtil::P;          using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class JoyAxisInfo                      // Axis class
{ /* -------------------------------------------------------------- */ private:
  const int        iId;                // Axis unique identifier
  float            fDeadZoneR,         // Reverse deadzone threshold
                   fDeadZoneF,         // Forward deadzone threshold
                   fUnbuffered;        // Current unbuffered axis press state
  int              iBuffered;          // Current buffered axis press state
  /* -- Get axis identifier ---------------------------------------- */ public:
  int AxisGetId(void) const { return iId; }
  /* -- Get reverse deadzone value ----------------------------------------- */
  float AxisGetReverseDeadZone(void) const { return fDeadZoneR; }
  /* -- Get forward deadzone value ----------------------------------------- */
  float AxisGetForwardDeadZone(void) const { return fDeadZoneF; }
  /* -- Set reverse deadzone value ----------------------------------------- */
  void AxisSetReverseDeadZone(const float fDZ) { fDeadZoneR = -fDZ; }
  /* -- Set forward deadzone value ----------------------------------------- */
  void AxisSetForwardDeadZone(const float fDZ) { fDeadZoneF = fDZ; }
  /* -- Get unbuffered state ----------------------------------------------- */
  float AxisGetUnbufferedState(void) const { return fUnbuffered; }
  /* -- Get buffered state ------------------------------------------------- */
  int AxisGetBufferedState(void) const { return iBuffered; }
  /* -- Clear button state ------------------------------------------------- */
  void AxisClearState(void) { fUnbuffered = 0.0f; iBuffered = GLFW_RELEASE; }
  /* -- Set axis state ----------------------------------------------------- */
  void AxisSetState(const float*const fpData)
  { // Get the axis reading and if it's moving negatively?
    fUnbuffered = fpData[AxisGetId()];
    if(fUnbuffered < fDeadZoneR)
    { // Released or already in reverse position?
      if(iBuffered <= GLFW_RELEASE)
      { // Set to -GLFW_REPEAT if currently -GLFW_PRESSED or
        //        -GLFW_PRESSED if currently GLFW_RELEASE
        if(iBuffered > -GLFW_REPEAT) --iBuffered;
        // State changed
        return;
      }
    } // Moving positively?
    else if(fUnbuffered > fDeadZoneF)
    { // Released or already in forward position?
      if(iBuffered >= GLFW_RELEASE)
      { // Set to GLFW_REPEAT if currentl GLFW_PRESSED or
        //        GLFW_PRESSED if currently GLFW_RELEASE
        if(iBuffered < GLFW_REPEAT) ++iBuffered;
        // State changed
        return;
      }
    } // Nothing pressed so reset buffered counter
    iBuffered = GLFW_RELEASE;
  }
  /* -- Constructor with unique id ----------------------------------------- */
  explicit JoyAxisInfo(const int iNId) :
    /* -- Initialisers ----------------------------------------------------- */
    iId(iNId),                         // Set unique id
    fDeadZoneR(-0.25f),                // Set default reverse deadzone
    fDeadZoneF(0.25f),                 // Set default forward deadzone
    fUnbuffered(0.0f),                 // Set default unbuffered state
    iBuffered(GLFW_RELEASE)            // Set default state
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(JoyAxisInfo)         // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
template<class Container = array<JoyAxisInfo, 8>,
         class Iterator = typename Container::const_iterator>
class JoyAxisList :                    // Axis data list type
/* ------------------------------------------------------------------------- */
  private Container                    // Array class to store axis data
{ /* -------------------------------------------------------------- */ private:
  int              iAxes;              // Button count as int
  /* -- Return axes count clamped ------------------------------------------ */
  int JoyAxisListSizeCountClamped(void) const { return iAxes & 0x7f; }
  /* -- Return button class ------------------------------------------------ */
  JoyAxisInfo &JoyAxisListGetMod(const size_t stId) { return (*this)[stId]; }
  /* -- Return axes array size ------------------------------------- */ public:
  size_t JoyAxisListSize(void) const { return this->size(); }
  /* -- Return axes count -------------------------------------------------- */
  size_t JoyAxisListCount(void) const { return static_cast<size_t>(iAxes); }
  /* -- Return button class ------------------------------------------------ */
  const JoyAxisInfo &JoyAxisListGet(const size_t stId) const
    { return (*this)[stId]; }
  /* -- Clear button data -------------------------------------------------- */
  void JoyAxisListClear(void)
    { StdForEach(par_unseq, this->begin(),
        this->begin() + JoyAxisListSizeCountClamped(),
          [](JoyAxisInfo& jaiRef) { jaiRef.AxisClearState(); }); }
  /* -- Refresh button data ------------------------------------------------ */
  void JoyAxisListRefresh(const int iJId)
  { // Get joystick axes and if found? Refresh axes data for each one
    if(const float *fpData = GlFWGetJoystickAxes(iJId, iAxes))
      StdForEach(seq, this->begin(),
        this->begin() + JoyAxisListSizeCountClamped(),
      [fpData](JoyAxisInfo &jaiData){ jaiData.AxisSetState(fpData); });
    else iAxes = 0;
  }
  /* -- Get axis state ----------------------------------------------------- */
  void JoyAxisListSetForwardDeadZone(const size_t stAxisId, const float fDZ)
    { JoyAxisListGetMod(stAxisId).AxisSetForwardDeadZone(fDZ); }
  void JoyAxisListSetReverseDeadZone(const size_t stAxisId, const float fDZ)
    { JoyAxisListGetMod(stAxisId).AxisSetReverseDeadZone(fDZ); }
  void JoyAxisListSetDeadZones(const size_t stAxisId,
    const float fFDZ, const float fRDZ)
  { // Get axis data and set the new values
    JoyAxisInfo &jaiItem = JoyAxisListGetMod(stAxisId);
    jaiItem.AxisSetForwardDeadZone(fFDZ);
    jaiItem.AxisSetReverseDeadZone(fRDZ);
  }
  /* -- Set default positive deadzone -------------------------------------- */
  void JoyAxisListSetReverseDeadZone(const float fDZ)
    { StdForEach(par_unseq, this->begin(),
        this->end() + JoyAxisListSizeCountClamped(),
          [fDZ](JoyAxisInfo &jaiItem)
            { jaiItem.AxisSetReverseDeadZone(fDZ); }); }
  /* -- Set default positive deadzone -------------------------------------- */
  void JoyAxisListSetForwardDeadZone(const float fDZ)
    { StdForEach(par_unseq, this->begin(),
        this->end() + JoyAxisListSizeCountClamped(),
          [fDZ](JoyAxisInfo &jaiItem)
            { jaiItem.AxisSetForwardDeadZone(fDZ); }); }
  /* -- Iterators ---------------------------------------------------------- */
  Iterator JoyAxisListBegin(void) const
    { return this->cbegin(); }
  Iterator JoyAxisListEnd(void) const
    { return this->cbegin() + JoyAxisListSizeCountClamped(); }
  /* -- Default constructor ------------------------------------- */ protected:
  JoyAxisList(void) :
    /* -- Initialise joystick buttons -------------------------------------- */
#define JAIX(x) JoyAxisInfo{ x }
#define JAI(x) JAIX(GLFW_GAMEPAD_AXIS_ ## x)
    /* --------------------------------------------------------------------- */
    Container{{                        // Initialise joystick axes ids
      /* ------------------------------------------------------------------- */
      JAI(LEFT_X),       JAI(LEFT_Y),        JAI(RIGHT_X),   JAI(RIGHT_Y),
      JAI(LEFT_TRIGGER), JAI(RIGHT_TRIGGER), JAIX(6),        JAIX(7)
      /* ------------------------------------------------------------------- */
    }},                                // End of joystick axes ids init
    /* --------------------------------------------------------------------- */
#undef JAI                             // Done with this macro
#undef JAIX                            // Done with this macro
    /* --------------------------------------------------------------------- */
    iAxes(0)                           // Initialise axes count
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(JoyAxisList)         // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
