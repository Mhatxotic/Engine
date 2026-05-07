/* == SERIAL.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Simple class to hold a unique serial number for each object.        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISerial {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- The slave class usually goes in a member class ----------------------- */
template<typename IntType = const uint64_t> // Default uint64_t
  requires StdIsIntegral<IntType>           // *or* must be integral
struct SerialSlave                          // Members initially public
{ /* -- Public functions --------------------------------------------------- */
  IntType Serial() const { return itSerial; }
  /* -- Protected variables ------------------------------------- */ protected:
  IntType          itSerial;           // IntTypehe counter
  /* -- Constructor that initialises counter ------------------------------- */
  explicit SerialSlave(const IntType itId) : itSerial(itId) {}
};/* -- The master class usually goes in the collector class --------------- */
template<typename IntType = uint64_t,      // Serial int type (non-const)
  class SlaveClass = SerialSlave<IntType>> // Slave class type
struct SerialMaster : public SlaveClass    // Members initially public
{ /* -- Reset counter (used at sanbox restart only) ------------------------ */
  void SerialReset(const IntType itId = 0) { this->itSerial = itId; }
  /* -- Return value before incrementing counter --------------------------- */
  IntType Serial() { return this->itSerial++; }
  /* -- Default constructor to initialise counter to zero --------*/ protected:
  SerialMaster() : SlaveClass{ static_cast<IntType>(0) } {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
