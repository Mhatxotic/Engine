/* == ENDIAN.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up functions for dealing with out of order bytes. There is a   ## **
** ## great deal of typecasting, but thats just to prevent potential      ## **
** ## warnings, but doesn't result in extra run-time code generation.     ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IEndian {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
#if defined(MSVC_VANILLA)              // Using vanilla Visual C++ compiler?
/* -- 16, 32 and 64-bit byte swapping functions ---------------------------- */
static uint16_t EndianSwap16(const uint16_t usVal)
  { return _byteswap_ushort(static_cast<unsigned short>(usVal)); }
static uint32_t EndianSwap32(const uint32_t ulVal)
  { return _byteswap_ulong(static_cast<unsigned long>(ulVal)); }
static uint64_t EndianSwap64(const uint64_t ullVal)
  { return _byteswap_uint64(static_cast<unsigned __int64>(ullVal)); }
/* ------------------------------------------------------------------------- */
#else                                  // MacOS or Linux?
/* -- 16, 32 and 64-bit byte swapping functions ---------------------------- */
static uint16_t EndianSwap16(const uint16_t usVal)
  { return __builtin_bswap16(static_cast<unsigned short>(usVal)); }
static uint32_t EndianSwap32(const uint32_t uiVal)
  { return __builtin_bswap32(static_cast<unsigned int>(uiVal)); }
static uint64_t EndianSwap64(const uint64_t ullVal)
  { return __builtin_bswap64(static_cast<unsigned long long>(ullVal)); }
/* ------------------------------------------------------------------------- */
#endif                                 // OS level functions setup
/* ------------------------------------------------------------------------- */
#if defined(LITTLEENDIAN)              // Little-endian (Intel/AMD/ARM/etc.)
/* -- Byte swapping so we have the correct byte order (16-bit) ------------- */
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint16_t))
static AnyType EndianTo16LE(const AnyType atVal) { return atVal; }
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint16_t))
static AnyType EndianTo16BE(const AnyType atVal)
  { return static_cast<AnyType>(EndianSwap16(static_cast<uint16_t>(atVal))); }
/* -- 32-bit --------------------------------------------------------------- */
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint32_t))
static AnyType EndianTo32LE(const AnyType atVal) { return atVal; }
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint32_t))
static AnyType EndianTo32BE(const AnyType atVal)
  { return static_cast<AnyType>(EndianSwap32(static_cast<uint32_t>(atVal))); }
/* -- 64-bit --------------------------------------------------------------- */
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint64_t))
static AnyType EndianTo64LE(const AnyType atVal) { return atVal; }
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint64_t))
static AnyType EndianTo64BE(const AnyType atVal)
  { return static_cast<AnyType>(EndianSwap64(static_cast<uint64_t>(atVal))); }
/* ------------------------------------------------------------------------- */
#elif defined(BIGENDIAN)               // Big-endian (???)
/* -- Byte swapping so we have the correct byte order (16-bit) ------------- */
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint16_t))
static AnyType EndianTo16LE(const AnyType atVal)
  { return static_cast<AnyType>(EndianSwap16(static_cast<uint16_t>(atVal))); }
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint16_t))
static AnyType EndianTo16BE(const AnyType atVal) { return atVal; }
/* -- 32-bit --------------------------------------------------------------- */
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint32_t))
static AnyType EndianTo32LE(const AnyType atVal)
  { return static_cast<AnyType>(EndianSwap32(static_cast<uint32_t>(atVal))); }
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint32_t))
static AnyType EndianTo32BE(const AnyType atVal) { return atVal; }
/* -- 64-bit --------------------------------------------------------------- */
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint64_t))
static AnyType EndianTo64LE(const AnyType atVal)
  { return static_cast<AnyType>(EndianSwap64(static_cast<uint64_t>(atVal))); }
template<typename AnyType>
  requires StdIsIntegral<AnyType> && (sizeof(AnyType) == sizeof(uint64_t))
static AnyType EndianTo64BE(const AnyType atVal) { return atVal; }
/* ------------------------------------------------------------------------- */
#endif
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
