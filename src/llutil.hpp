/* == LLUTIL.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Util' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Util
/* ------------------------------------------------------------------------- */
// ! This contains miscellaneous utility functions such as crypto, string,
// ! number integer and time manipulation functions.
/* ========================================================================= */
namespace LLUtil {                     // Util namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArgs::P;              using namespace IAsset::P;
using namespace IClock::P;             using namespace ICrypt::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace IUrl::P;
using namespace IUtf::P;               using namespace IUtil::P;
using namespace IUuId::P;              using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Util common helper classes                                          ## **
** ######################################################################### **
** -- Get argument for more specific type ---------------------------------- */
typedef AgNumberL<double> AgDoubleL;
typedef AgInteger<uint16_t> AgUInt16;
typedef AgInteger<StdTimeT> AgTimeT;
/* ========================================================================= **
** ######################################################################### **
** ## Util.* namespace functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Util.AscNTime
// < Date:string=The timestamp converted to a string.
// ? Converts the current time into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNC(AscNTime, 1, LuaUtilPushVar(lS, cmSys.FormatTime()))
/* ========================================================================= */
// $ Util.AscNTimeUTC
// < Date:string=The timestamp converted to a string.
// ? Converts the current time into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNC(AscNTimeUTC, 1, LuaUtilPushVar(lS, cmSys.FormatTimeUTC()))
/* ========================================================================= */
// $ Util.AscTime
// > Timestamp:integer=The timestamp to convert to string
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNC(AscTime, 1, LuaUtilPushVar(lS, StrFromTimeTT(AgTimeT{lS, 1})))
/* ========================================================================= */
// $ Util.AscTimeUTC
// > Timestamp:integer=The timestamp to convert to string
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the format 'Www Mmm dd hh:mm:ss yyyy zzz'.
// ? There maybe a slight differences across different operating systems.
// ? Note that time will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNC(AscTimeUTC, 1, LuaUtilPushVar(lS, StrFromTimeTTUTC(AgTimeT{lS, 1})))
/* ========================================================================= */
// $ Util.B64D
// > Text:string=The string to decode.
// < Text:string=The base64 decoded string.
// ? Decodes the specified string from base64.
/* ------------------------------------------------------------------------- */
LLFUNC(B64D, 1, LuaUtilPushVar(lS, CryptB64toS(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.B64DA
// > Text:string=The string to decode.
// < Data:Asset=The base64 decoded data array.
// ? Decodes the specified string from base64 to a data array class. Useful
// ? if you're handling binary data and/or not sure if the data is binary/text
// ? and need to check.
/* ------------------------------------------------------------------------- */
LLFUNC(B64DA, 1,
  const AgString aString{lS, 1};
  AcAsset{lS}().MemSwap({ CryptB64toMB(aString) }))
/* ========================================================================= */
// $ Util.B64E
// > Data:string=The string to encode.
// < Hash:string=The return string encoded to base64.
// ? Encodes the specified string to base64.
/* ------------------------------------------------------------------------- */
LLFUNC(B64E, 1, LuaUtilPushVar(lS, CryptStoB64(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.B64EA
// > Data:Asset=The data to encode.
// < Hash:string=The return string encoded to bas64.
// ? Encodes the specified data array to base64.
/* ------------------------------------------------------------------------- */
LLFUNC(B64EA, 1, LuaUtilPushVar(lS, CryptMBtoB64(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Util.Bits
// > Bits:integer=The number to convert.
// > Precision:integer=The precision after the decimal point to use
// < Result:string=The resulting string.
// ? Converts the specified number of bits into a human readable string.
/* ------------------------------------------------------------------------- */
LLFUNC(Bits, 1,
  const AgUInt64 aBits{lS, 1};
  const AgInt aPrecision{lS, 2};
  LuaUtilPushVar(lS, StrToReadableBits(aBits(), aPrecision)))
/* ========================================================================= */
// $ Util.Blank
// ? Just a blank function that does nothing. Useful for passing to functions
// ? that require a callback. The function is only available for convenience as
// ? performing 'local function EmptyFunc() end' is generally faster than this.
/* ------------------------------------------------------------------------- */
LLFUNC(Blank, 0,);
/* ========================================================================= */
// $ Util.Bytes
// > Count:integer=The number to convert.
// > Precision:integer=The precision after the decimal point to use
// < Result:string=The resulting string.
// ? Converts the specified number of bytes into a human readable string.
/* ------------------------------------------------------------------------- */
LLFUNC(Bytes, 1,
  const AgUInt64 aBytes{lS, 1};
  const AgInt aPrecision{lS, 2};
  LuaUtilPushVar(lS, StrToReadableBytes(aBytes(), aPrecision)))
/* ========================================================================= */
// $ Util.CRC
// > Data:string=The string to hash.
// < Hash:integer=The calculated CRC hash.
// ? Calculates a CRC32 hash from a string. Insecure, only use for super fast
// ? hashing of insignificant strings. The benefit of CRC hashing though is
// ? that the function returns an integer.
/* ------------------------------------------------------------------------- */
LLFUNC(CRC, 1, LuaUtilPushVar(lS, CryptToCRC32(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.CRCA
// > Data:Asset=The data to hash.
// < Hash:integer=The calculated CRC hash.
// ? Calculates a CRC32 hash from an Asset class. Insecure, only use for super
// ? fast hashing of insignificant strings. The benefit of CRC hashing though
// ? is that the function returns an integer.
/* ------------------------------------------------------------------------- */
LLFUNC(CRCA, 1, LuaUtilPushVar(lS, CryptToCRC32(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Util.Capitalise
// > String:string=The string to capitalise
// < Result:string=The same string in memory capitalised
// ? Returns the same string capitalised
/* ------------------------------------------------------------------------- */
LLFUNC(Capitalise, 1, LuaUtilPushVar(lS, StrCapitalise(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.Chop
// > String:string=The string to modify
// < Result:string=The chopped string
// ? Removes carriage returns from the beginning and end of the string.
/* ------------------------------------------------------------------------- */
LLFUNC(Chop, 1,
  AgString aString{lS, 1};
  LuaUtilPushVar(lS, StrChop(const_cast<string&>(aString()))))
/* ========================================================================= */
// $ Util.Clamp
// > Value:number=The current number.
// > Minimum:number=The minimum number.
// > Maximum:number=The maximum number.
// < Result:number=The clamped number.
// ? Makes sure the specified number does not go under the minimum or maximum
// ? numbers specified.
/* ------------------------------------------------------------------------- */
LLFUNC(Clamp, 1, const AgLuaNumber aValue{lS, 1}, aMin{lS, 2}, aMax{lS, 3};
  LuaUtilPushVar(lS, UtilClamp(aValue(), aMin(), aMax())))
/* ========================================================================= */
// $ Util.ClampInt
// > Value:integer=The current number.
// > Minimum:integer=The minimum number.
// > Maximum:integer=The maximum number.
// < Result:integer=The clamped number.
// ? Makes sure the specified number does not go under the minimum or maximum
// ? numbers specified.
/* ------------------------------------------------------------------------- */
LLFUNC(ClampInt, 1,
  const AgLuaInteger aValue{lS, 1}, aMin{lS, 2}, aMax{lS, 3};
  LuaUtilPushVar(lS, UtilClamp(aValue(), aMin(), aMax())))
/* ========================================================================= */
// $ Util.Compact
// > Value:string=The string to compact.
// < Result:integer=The compacted string.
// ? Removes all leading, trailing and grouped whitespaces from string so that
// ? each word has only one whitespace in between them.
/* ------------------------------------------------------------------------- */
LLFUNC(Compact, 1, LuaUtilPushVar(lS, StrCompact(AgCStringChar{lS, 1})))
/* ========================================================================= */
// $ Util.CountOf
// > Source:string=Source string to search
// > What:string=String to search for
// < Count:integer=Number of occurences in the string
// ? A fast way of counting the number of occurences in a string.
/* ------------------------------------------------------------------------- */
LLFUNC(CountOf, 1, const AgString aSource{lS, 1}, aWhat{lS, 2};
  LuaUtilPushVar(lS, StrCountOccurences(aSource, aWhat)))
/* ========================================================================= */
// $ Util.Duration
// > Seconds:number=The number of seconds.
// > Precision:integer=The maximum number of floating point digits.
// < Result:string=The human readable duration string.
// ? Converts the specified number of seconds into a human readable duration
// ? string in the format of 000:00:00:00.
/* ------------------------------------------------------------------------- */
LLFUNC(Duration, 1,
  const AgDouble aSeconds{lS, 1};
  const AgInt aPrecision{lS, 2};
  LuaUtilPushVar(lS, StrShortFromDuration(aSeconds, aPrecision)))
/* ========================================================================= */
// $ Util.EntDecode
// > Text:string=The URL string to remove HTML entities from.
// < Text:string=The URL decoded string.
// ? Decodes HTML entities from the spcified string.
/* ------------------------------------------------------------------------- */
LLFUNC(EntDecode, 1, LuaUtilPushVar(lS, cCrypt->EntDecode(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.EntEncode
// > Text:string=The URL string to insert HTML entities into.
// < Text:string=The URL encoded string.
// ? Encodes HTML entities into the spcified string.
/* ------------------------------------------------------------------------- */
LLFUNC(EntEncode, 1, LuaUtilPushVar(lS, CryptEntEncode(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.Explode
// > Text:string=The text to tokenise.
// > Separator:string=The separator.
// < Table:table=The returned table.
// ? Splits the specified string into table entries on each matching interation
// ? of the specified separator.
/* ------------------------------------------------------------------------- */
LLFUNC(Explode, 1, LuaUtilExplode(lS))
/* ========================================================================= */
// $ Util.ExplodeEx
// > Text:string=The text to tokenise.
// > Separator:string=The separator.
// > Maximum:integer=Maximum tokens.
// < Table:table=The returned table.
// ? Splits the specified string into table entries on each matching interation
// ? of the specified separator. The maximum value is the maximum number of
// ? tokens allowed so further occurences of the separator are not tokenised
// ? and included in the final tokens string.
/* ------------------------------------------------------------------------- */
LLFUNC(ExplodeEx, 1, LuaUtilExplodeEx(lS))
/* ========================================================================= */
// $ Util.FormatNTime
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the current timestamp into the specified formatted string.
// ? There may be slight differences across different operating systems.
// ? Note that time will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNC(FormatNTime, 1,
  LuaUtilPushVar(lS, cmSys.FormatTime(AgCStringChar{lS, 1})))
/* ========================================================================= */
// $ Util.FormatNTimeUTC
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the current timestamp into the specified formatted string. There
// ? may be slight differences across different operating systems. Note that
// ? time will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNC(FormatNTimeUTC, 1,
  LuaUtilPushVar(lS, cmSys.FormatTimeUTC(AgCStringChar{lS, 1})))
/* ========================================================================= */
// $ Util.FormatNumber
// > Value:number=A number value, it will be converted to a string.
// > Precision:integer=Maximum number of floating point digits
// < Value:string=The formatted string.
// ? Uses the current OS locale settings to cleanly format a number with
// ? thousand separators.
/* ------------------------------------------------------------------------- */
LLFUNC(FormatNumber, 1,
  const AgDouble aValue{lS, 1};
  const AgInt aDigits{lS, 2};
  LuaUtilPushVar(lS, StrReadableFromNum(aValue(), aDigits)))
/* ========================================================================= */
// $ Util.FormatNumberI
// > Value:integer=A integer value, it will be converted to a string.
// > Precision:integer=Maximum number of floating point digits
// < Value:string=The formatted string.
// ? Uses the current OS locale settings to cleanly format a number with
// ? thousand separators.
/* ------------------------------------------------------------------------- */
LLFUNC(FormatNumberI, 1,
  LuaUtilPushVar(lS, StrReadableFromNum(AgUInt64{lS, 1}(), 0)))
/* ========================================================================= */
// $ Util.FormatTime
// > Timestamp:integer=The timestamp to convert to string
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the specified formatted string. There may be
// ? slight differences across different operating systems. Note that time
// ? will be in local time format.
/* ------------------------------------------------------------------------- */
LLFUNC(FormatTime, 1,
  const AgTimeT aTimestamp{lS, 1};
  const AgCStringChar aFormat{lS, 2};
  LuaUtilPushVar(lS, StrFromTimeTT(aTimestamp, aFormat)))
/* ========================================================================= */
// $ Util.FormatTimeUTC
// > Timestamp:integer=The timestamp to convert to string
// > Format:stringr=The strftime format string.
// < Date:string=The timestamp converted to a string.
// ? Converts the timestamp into the specified formatted string. There may be
// ? slight differences across different operating systems. Note that time
// ? will be in UTC time format.
/* ------------------------------------------------------------------------- */
LLFUNC(FormatTimeUTC, 1,
  const AgTimeT aTimestamp{lS, 1};
  const AgCStringChar aFormat{lS, 2};
  LuaUtilPushVar(lS, StrFromTimeTTUTC(aTimestamp, aFormat)))
/* ========================================================================= */
// $ Util.GetRatio
// > Width:integer=The width integer.
// > Height:integer=The height integer.
// < Result:string=The ratio between the two numbers.
// ? Caclulates the ratio between the two numbers and returns a string in the
// ? format of "n:n".
/* ------------------------------------------------------------------------- */
LLFUNC(GetRatio, 1, const AgLuaIntegerL aWidth{lS, 1, 1}, aHeight{lS, 2, 1};
  LuaUtilPushVar(lS, StrFromRatio(aWidth(), aHeight())))
/* ========================================================================= */
// $ Util.Grouped
// > Count:integer=The number to convert.
// > Precision:integer=The precision after the decimal point to use
// < Result:string=The resulting string.
// ? Converts the specified number into a human readable string.
/* ------------------------------------------------------------------------- */
LLFUNC(Grouped, 1,
  const AgUInt64 aValue{lS, 1};
  const AgInt aPrecision{lS, 2};
  LuaUtilPushVar(lS, StrToReadableGrouped(aValue(), aPrecision)))
/* ========================================================================= */
// $ Util.Hash
// > Salt:string|Asset=The salt string or raw data used to randomise the data
// > Data:string|Asset=The string or raw data to hash.
// < Hash:string|Asset=The string or raw data calculated hash.
// ? A collection of functions that will calculate a hash from a 'string' class
// ? or raw 'Asset' class with or without salting.
// ?
// ? Actual functions (Replace 'Hash' with one of the below). Syntax of the
// ? function is <TYP><STR><R><P...> where <TYP> is the hash type. <STR> is the
// ? strength of the hash, <R> is either 'A' returning an 'Asset' class or 'S'
// ? returning a string object and <P...> means one or more 'Asset' (A) or
// ? 'String' (S) parameters. (*) denotes do not use due to collision risks...
// ? - Hash:Asset = SHA1AA(Data:Asset) (*).
// ? - Hash:Asset = SHA1AAA(Salt:Asset, Data:Asset) (*).
// ? - Hash:Asset = SHA1AAS(Salt:Asset, Data:string) (*).
// ? - Hash:Asset = SHA1AS(Data:string) (*).
// ? - Hash:Asset = SHA1ASA(Salt:string, Data:Asset) (*).
// ? - Hash:Asset = SHA1ASS(Salt:string, Data:string) (*).
// ? - Hash:string = SHA1SA(Data:Asset) (*).
// ? - Hash:string = SHA1SAA(Salt:Asset, Data:Asset) (*).
// ? - Hash:string = SHA1SAS(Salt:Asset, Data:string) (*).
// ? - Hash:string = SHA1SS(Data:string) (*).
// ? - Hash:string = SHA1SSA(Salt:string, Data:Asset) (*).
// ? - Hash:string = SHA1SSS(Salt:string, Data:string) (*).
// ? - Hash:Asset = SHA224AA(Data:Asset).
// ? - Hash:Asset = SHA224AAA(Salt:Asset, Data:Asset).
// ? - Hash:Asset = SHA224AAS(Salt:Asset, Data:string).
// ? - Hash:Asset = SHA224AS(Data:string).
// ? - Hash:Asset = SHA224ASA(Salt:string, Data:Asset).
// ? - Hash:Asset = SHA224ASS(Salt:string, Data:string).
// ? - Hash:string = SHA224SA(Data:Asset).
// ? - Hash:string = SHA224SAA(Salt:Asset, Data:Asset).
// ? - Hash:string = SHA224SAS(Salt:Asset, Data:string).
// ? - Hash:string = SHA224SS(Data:string).
// ? - Hash:string = SHA224SSA(Salt:string, Data:Asset).
// ? - Hash:string = SHA224SSS(Salt:string, Data:string).
// ? - Hash:Asset = SHA256AA(Data:Asset).
// ? - Hash:Asset = SHA256AAA(Salt:Asset, Data:Asset).
// ? - Hash:Asset = SHA256AAS(Salt:Asset, Data:string).
// ? - Hash:Asset = SHA256AS(Data:string).
// ? - Hash:Asset = SHA256ASA(Salt:string, Data:Asset).
// ? - Hash:Asset = SHA256ASS(Salt:string, Data:string).
// ? - Hash:string = SHA256SA(Data:Asset).
// ? - Hash:string = SHA256SAA(Salt:Asset, Data:Asset).
// ? - Hash:string = SHA256SAS(Salt:Asset, Data:string).
// ? - Hash:string = SHA256SS(Data:string).
// ? - Hash:string = SHA256SSA(Salt:string, Data:Asset).
// ? - Hash:string = SHA256SSS(Salt:string, Data:string).
// ? - Hash:Asset = SHA384AA(Data:Asset).
// ? - Hash:Asset = SHA384AAA(Salt:Asset, Data:Asset).
// ? - Hash:Asset = SHA384AAS(Salt:Asset, Data:string).
// ? - Hash:Asset = SHA384AS(Data:string).
// ? - Hash:Asset = SHA384ASA(Salt:string, Data:Asset).
// ? - Hash:Asset = SHA384ASS(Salt:string, Data:string).
// ? - Hash:string = SHA384SA(Data:Asset).
// ? - Hash:string = SHA384SAA(Salt:Asset, Data:Asset).
// ? - Hash:string = SHA384SAS(Salt:Asset, Data:string).
// ? - Hash:string = SHA384SS(Data:string).
// ? - Hash:string = SHA384SSA(Salt:string, Data:Asset).
// ? - Hash:string = SHA384SSS(Salt:string, Data:string).
// ? - Hash:Asset = SHA512AA(Data:Asset).
// ? - Hash:Asset = SHA512AAA(Salt:Asset, Data:Asset).
// ? - Hash:Asset = SHA512AAS(Salt:Asset, Data:string).
// ? - Hash:Asset = SHA512AS(Data:string).
// ? - Hash:Asset = SHA512ASA(Salt:string, Data:Asset).
// ? - Hash:Asset = SHA512ASS(Salt:string, Data:string).
// ? - Hash:string = SHA512SA(Data:Asset).
// ? - Hash:string = SHA512SAA(Salt:Asset, Data:Asset).
// ? - Hash:string = SHA512SAS(Salt:Asset, Data:string).
// ? - Hash:string = SHA512SS(Data:string).
// ? - Hash:string = SHA512SSA(Salt:string, Data:Asset).
// ? - Hash:string = SHA512SSS(Salt:string, Data:string).
/* -- Macro to create a function that creates a string hash ---------------- */
#define HASH_FUNC_NOSALT_STR(pf, af, cf, ty) \
  LLFUNC(pf ## af, 1, LuaUtilPushVar(lS, pf ## functions::H ## cf(ty{lS, 1})))
/* -- Macro to create a function that creates a raw hash ------------------- */
#define HASH_FUNC_NOSALT_ASSET(pf, af, cf, ty) \
  LLFUNC(pf ## af, 1, \
    AcAsset{lS}().MemSwap(pf ## functions::H ## cf(ty{lS, 1})))
/* -- Macro to create a function that creates a salted string hash --------- */
#define HASH_FUNC_SALT_STR(pf, af, cf, t1, t2) \
  LLFUNC(pf ## af, 1, const t1 aKey{lS, 1}; const t2 aData{lS, 2}; \
    LuaUtilPushVar(lS, pf ## functions::H ## cf(aKey, aData)))
/* -- Macro to create a function that creates a salted raw hash ------------ */
#define HASH_FUNC_SALT_ASSET(pf, af, cf, t1, t2) \
  LLFUNC(pf ## af, 1, const t1 aKey{lS, 1}; const t2 aData{lS, 2}; \
    AcAsset{lS}().MemSwap(pf ## functions::H ## cf(aKey, aData)))
/* -- Macro to create a bunch of functions with different rets/params ------ */
#define HASH_FUNC_PACK(pf) \
  HASH_FUNC_NOSALT_ASSET(pf, AA, MM, AgAsset) \
  HASH_FUNC_NOSALT_ASSET(pf, AS, MS, AgString) \
  HASH_FUNC_NOSALT_STR(pf, SA, SM, AgAsset) \
  HASH_FUNC_NOSALT_STR(pf, SS, SS, AgString) \
  HASH_FUNC_SALT_ASSET(pf, AAA, MMM, AgAsset, AgAsset) \
  HASH_FUNC_SALT_ASSET(pf, AAS, MMS, AgAsset, AgString) \
  HASH_FUNC_SALT_ASSET(pf, ASA, MSM, AgString, AgAsset) \
  HASH_FUNC_SALT_ASSET(pf, ASS, MSS, AgString, AgString) \
  HASH_FUNC_SALT_STR(pf, SAA, SMM, AgAsset, AgAsset) \
  HASH_FUNC_SALT_STR(pf, SAS, SMS, AgAsset, AgString) \
  HASH_FUNC_SALT_STR(pf, SSA, SSM, AgString, AgAsset) \
  HASH_FUNC_SALT_STR(pf, SSS, SSS, AgString, AgString)
/* -- Create the actual function packs for different hash types ------------ */
HASH_FUNC_PACK(SHA1)                   // 128-bit function pack (insecure)
HASH_FUNC_PACK(SHA224)                 // 224-bit function pack
HASH_FUNC_PACK(SHA256)                 // 256-bit function pack
HASH_FUNC_PACK(SHA384)                 // 384-bit function pack
HASH_FUNC_PACK(SHA512)                 // 512-bit function pack
/* -- Done with these macros ----------------------------------------------- */
#undef HASH_FUNC_PACK
#undef HASH_FUNC_SALT_ASSET
#undef HASH_FUNC_SALT_STR
#undef HASH_FUNC_NOSALT_ASSET
#undef HASH_FUNC_NOSALT_STR
/* ========================================================================= */
// $ Util.Hex
// > Value:integer=Integer to be converted to hex
// < Value:string=Same number but in hexadecimal
// ? Convert integer to hexadecimal in uppercase
/* ------------------------------------------------------------------------- */
LLFUNC(Hex, 1, LuaUtilPushVar(lS, StrHexUFromInt(AgLuaInteger{lS, 1})))
/* ========================================================================= */
// $ Util.HexDecode
// > Text:string=The hex encoded string to decode.
// < Text:Asset=The text decoded string.
// ? Encodes the specified text to uppercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNC(HexDecode, 1, LuaUtilPushVar(lS, CryptHexDecodeStr(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.HexDecodeA
// > Text:string=The hex encoded string to decode.
// < Data:Asset=The decoded data array class.
// ? Decodes the specified hexadecimal string to binary data.
/* ------------------------------------------------------------------------- */
LLFUNC(HexDecodeA, 1,
  const AgString aString{lS, 1};
  AcAsset{lS}().MemSwap({ CryptHexDecodeA(aString) }))
/* ========================================================================= */
// $ Util.HexEncode
// > Text:string=The string.
// < Text:string=The hex encoded string.
// ? Encodes the specified text to uppercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNC(HexEncode, 1, LuaUtilPushVar(lS, CryptStoHEX(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.HexEncodeA
// > Data:Asset=The data array class to encode.
// < Text:string=The hex encoded string.
// ? Encodes the specified data array to uppercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNC(HexEncodeA, 1, LuaUtilPushVar(lS, CryptBin2Hex(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Util.HexEncodeL
// > Text:string=The string.
// < Text:string=The hex encoded string.
// ? Encodes the specified text to lowercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNC(HexEncodeL, 1, LuaUtilPushVar(lS, CryptStoHEXL(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.HexEncodeLA
// > Data:Asset=The data array class to encode.
// < Text:string=The hex encoded string.
// ? Encodes the specified data array to lowercase hexadecimal.
/* ------------------------------------------------------------------------- */
LLFUNC(HexEncodeLA, 1, LuaUtilPushVar(lS, CryptBin2HexL(AgAsset{lS, 1})))
/* ========================================================================= */
// $ Util.HexL
// > Value:integer=Integer to be converted to hex
// < Value:string=Same number but in hexadecimal
// ? Convert integer to hexadecimal in lowercase
/* ------------------------------------------------------------------------- */
LLFUNC(HexL, 1, LuaUtilPushVar(lS, StrHexFromInt(AgLuaInteger{lS, 1})))
/* ========================================================================= */
// $ Util.HighByte
// > Value:integer=The 16-bit ranged value to extract the highest 8-bit value
// < Result:integer=The resulting 8-bit ranged value
// ? Extracts the highest 8-bits from a 16-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNC(HighByte, 1, LuaUtilPushVar(lS, UtilHighByte(AgUInt16{lS, 1})))
/* ========================================================================= */
// $ Util.HighDWord
// > Value:integer=The 64-bit ranged value to extract the lowest 32-bit value
// < Result:integer=The resulting 32-bit ranged value
// ? Extracts the lowest 32-bits from a 64-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNC(HighDWord, 1, LuaUtilPushVar(lS, UtilHighDWord(AgUInt64{lS, 1})))
/* ========================================================================= */
// $ Util.HighWord
// > Value:integer=The 32-bit ranged value to extract the highest 16-bit value
// < Result:integer=The resulting 16-bit ranged value
// ? Extracts the highest 16-bits from a 32-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNC(HighWord, 1, LuaUtilPushVar(lS, UtilHighWord(AgUInt32{lS, 1})))
/* ========================================================================= */
// $ Util.Implode
// > Table:table=The table to convert to a string.
// > Separator:string=The separator.
// < String:string=The returned string.
// ? Appends each entry in the specified table to a string separated by the
// ? specified separator string.
/* ------------------------------------------------------------------------- */
LLFUNC(Implode, 1, LuaUtilImplode(lS))
/* ========================================================================= */
// $ Util.ImplodeEx
// > Table:table=The table to convert to a string.
// > Separator:string=The separator.
// > LastSep:string=The final separator (i.e. ' or' or ' and').
// < String:string=The returned string.
// ? Appends each entry in the specified table to a string separated by a
// ? 'Separator' and uses the specified 'LastSep' as the final separator to
// ? form a human reable string.
/* ------------------------------------------------------------------------- */
LLFUNC(ImplodeEx, 1, LuaUtilImplodeEx(lS))
/* ========================================================================= */
// $ Util.IfBlank
// > String:string=The string to test if empty.
// > Alternative:string=The string to use if blank.
// < Return:string=The returned string.
// ? If 'String' is blank or invalid, then 'StringAlt' is returned.
/* ------------------------------------------------------------------------- */
LLFUNC(IfBlank, 1, LuaUtilIfBlank(lS))
/* ========================================================================= */
// $ Util.IsASCII
// > String:string=The string to test if empty.
// < Result:boolean=String is all ASCII.
// ? Returns if all characters in string are all less than 0x80. This call is
// ? UTF8 compatible.
/* ------------------------------------------------------------------------- */
LLFUNC(IsASCII, 1,
  LuaUtilPushVar(lS, UtfDecoder{ AgCStringChar{lS, 1} }.IsASCII()))
/* ========================================================================= */
// $ Util.IsBoolean
// > Var:*=Any parameter.
// < Result:boolean=Is a valid boolean.
// ? Returns if specified parameter is a boolean.
/* ------------------------------------------------------------------------- */
LLFUNC(IsBoolean, 1, LuaUtilPushVar(lS, LuaUtilIsBoolean(lS, 1)))
/* ========================================================================= */
// $ Util.IsExtASCII
// > String:string=The string to test if empty.
// < Result:boolean=String is all extended ASCII.
// ? Returns if all characters in string are all less than 0xFF. This call is
// ? UTF8 compatible.
/* ------------------------------------------------------------------------- */
LLFUNC(IsExtASCII, 1,
  LuaUtilPushVar(lS, UtfDecoder{ AgCStringChar{lS, 1} }.IsExtASCII()))
/* ========================================================================= */
// $ Util.IsFunction
// > Var:*=Any parameter.
// < Result:boolean=Is a valid function.
// ? Returns if specified parameter is a function.
/* ------------------------------------------------------------------------- */
LLFUNC(IsFunction, 1, LuaUtilPushVar(lS, LuaUtilIsFunction(lS, 1)))
/* ========================================================================= */
// $ Util.IsInteger
// > Var:*=Any parameter.
// < Result:boolean=Is a valid integer.
// ? Returns if specified parameter is a integer.
/* ------------------------------------------------------------------------- */
LLFUNC(IsInteger, 1, LuaUtilPushVar(lS, LuaUtilIsInteger(lS, 1)))
/* ========================================================================= */
// $ Util.IsNumber
// > Var:*=Any parameter.
// < Result:boolean=Is a valid number.
// ? Returns if specified parameter is a number.
/* ------------------------------------------------------------------------- */
LLFUNC(IsNumber, 1, LuaUtilPushVar(lS, LuaUtilIsNumber(lS, 1)))
/* ========================================================================= */
// $ Util.IsString
// > Var:*=Any parameter.
// < Result:boolean=Is a valid string.
// ? Returns if specified parameter is a string.
/* ------------------------------------------------------------------------- */
LLFUNC(IsString, 1, LuaUtilPushVar(lS, LuaUtilIsString(lS, 1)))
/* ========================================================================= */
// $ Util.IsTable
// > Var:*=Any parameter.
// < Result:boolean=Is a valid table.
// ? Returns if specified parameter is a table.
/* ------------------------------------------------------------------------- */
LLFUNC(IsTable, 1, LuaUtilPushVar(lS, LuaUtilIsTable(lS, 1)))
/* ========================================================================= */
// $ Util.IsUserdata
// > Var:*=Any parameter.
// < Result:boolean=Is a valid userdata object.
// ? Returns if specified parameter is a userdata object.
/* ------------------------------------------------------------------------- */
LLFUNC(IsUserdata, 1, LuaUtilPushVar(lS, LuaUtilIsUserData(lS, 1)))
/* ========================================================================= */
// $ Util.LDuration
// > Seconds:integer=The number of seconds.
// < Result:string=The human readable duration string.
// ? Converts the specified number of seconds into a human readable duration
// ? string in the format of e.g. 0 years 0 weeks 0 days 0 hours 0 mins 0 secs.
/* ------------------------------------------------------------------------- */
LLFUNC(LDuration, 1, LuaUtilPushVar(lS, StrLongFromDuration(AgTimeT{lS, 1})))
/* ========================================================================= */
// $ Util.LDurationEx
// > Seconds:integer=The number of seconds.
// > Components:integer=The maximum number of components
// < Result:string=The human readable duration string.
// ? Converts the specified number of seconds into a human readable duration
// ? string in the format of e.g. 0 years 0 weeks 0 days 0 hours 0 mins 0 secs.
/* ------------------------------------------------------------------------- */
LLFUNC(LDurationEx, 1,
  const AgTimeT aTimestamp{lS, 1};
  const AgUInt aComponents{lS, 2};
  LuaUtilPushVar(lS, StrLongFromDuration(aTimestamp, aComponents)))
/* ========================================================================= */
// $ Util.LowByte
// > Value:integer=The 16-bit ranged value to extract the lowest 8-bit value
// < Result:integer=The resulting 8-bit ranged value
// ? Extracts the lowest 8-bits from a 16-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNC(LowByte, 1, LuaUtilPushVar(lS, UtilLowByte(AgUInt16{lS, 1})))
/* ========================================================================= */
// $ Util.LowDWord
// > Value:integer=The 64-bit ranged value to extract the lowest 32-bit value
// < Result:integer=The resulting 32-bit ranged value
// ? Extracts the lowest 32-bits from a 64-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNC(LowDWord, 1, LuaUtilPushVar(lS, UtilLowDWord(AgUInt64{lS, 1})))
/* ========================================================================= */
// $ Util.LowWord
// > Value:integer=The 32-bit ranged value to extract the lowest 16-bit value
// < Result:integer=The resulting 16-bit ranged value
// ? Extracts the lowest 16-bits from a 32-bit ranged integer.
/* ------------------------------------------------------------------------- */
LLFUNC(LowWord, 1, LuaUtilPushVar(lS, UtilLowWord(AgUInt32{lS, 1})))
/* ========================================================================= */
// $ Util.MakeDWord
// > High:integer=The high-order 16-bit range integer
// > Low:integer=The low-order 16-bit range integer
// < Result:integer=The resulting 32-bit range integer
// ? Joins two integers in the 16-bit range to return one integer in the 32-bit
// ? range.
/* ------------------------------------------------------------------------- */
LLFUNC(MakeDWord, 1, const AgUInt16 aHigh{lS, 1}, aLow{lS, 2};
  LuaUtilPushVar(lS, UtilMakeDWord(aHigh, aLow)))
/* ========================================================================= */
// $ Util.MakeWord
// > High:integer=The high-order 8-bit range integer
// > Low:integer=The low-order 8-bit range integer
// < Result:integer=The resulting 16-bit range integer
// ? Joins two integers in the 8-bit range to return one integer in the 16-bit
// ? range.
/* ------------------------------------------------------------------------- */
LLFUNC(MakeWord, 1, const AgUInt8 aHigh{lS, 1}, aLow{lS, 2};
  LuaUtilPushVar(lS, UtilMakeWord(aHigh, aLow)))
/* ========================================================================= */
// $ Util.MakeQWord
// > High:integer=The high-order 32-bit range integer
// > Low:integer=The low-order 32-bit range integer
// < Result:integer=The resulting 64-bit range integer
// ? Joins two integers in the 32-bit range to return one integer in the 64-bit
// ? range.
/* ------------------------------------------------------------------------- */
LLFUNC(MakeQWord, 1, const AgUInt32 aHigh{lS, 1}, aLow{lS, 2};
  LuaUtilPushVar(lS, UtilMakeQWord(aHigh, aLow)))
/* ========================================================================= */
// $ Util.ParseArgs
// > String:string=The arguments to parse.
// < Table:table=A table of arguments.
// ? Treats the specified string as a command line and returns a table entry
// ? for each argument.
/* ------------------------------------------------------------------------- */
LLFUNC(ParseArgs, 1, LuaUtilToTable(lS, ArgsBuildSafe(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.ParseTime
// > Format:string=The ISO 8601 formatted string.
// < Timestamp:integer=The timestamp converted from the string.
// ? Converts the specified ISO 8601 formatted string to a numerical timestamp.
// ? Only the format '2015-09-04T14:26:16Z' is supported at the moment.
/* ------------------------------------------------------------------------- */
LLFUNC(ParseTime, 1, LuaUtilPushVar(lS, StrParseTime(AgNeString{lS, 1})))
/* ========================================================================= */
// $ Util.ParseTime2
// > String:string=The specially formatted date string.
// < Timestamp:integer=The timestamp converted from the string.
// ? Converts the specified specially formatted string to a numerical
// ? timestamp. Only the format 'Mon May 04 00:05:00 +0000 2009' is supported
// ? at the moment.
/* ------------------------------------------------------------------------- */
LLFUNC(ParseTime2, 1, LuaUtilPushVar(lS, StrParseTime2(AgNeString{lS, 1})))
/* ========================================================================= */
// $ Util.ParseTimeEx
// > Timestamp:string=The specially formatted date string.
// > Format:string=The customised syntax of the formatted string.
// < Timestamp:integer=The timestamp converted from the string.
// ? Converts the specified specially formatted string to a numerical
// ? timestamp.
/* ------------------------------------------------------------------------- */
LLFUNC(ParseTimeEx, 1,
  const AgNeString aTimestamp{lS, 1};
  const AgCStringChar aFormat{lS, 2};
  LuaUtilPushVar(lS, StrParseTime(aTimestamp, aFormat)))
/* ========================================================================= */
// $ Util.ParseUrl
// > String:string=The url to parse
// > Post:integer=Encode, decode or none
// < Result:integer=The result code of the parse
// < Url:string=The canonicalised url
// < Scheme:string=The scheme string 'http' or 'https'
// < Secure:boolean=The conneciton would need to use SSL?
// < Host:string=The hostname to connect to
// < Port:integer=The port number to connect to
// < Resource:string=The resource to request
// < Bookmark:string=The bookmark part of the request
// < Username:string=The username part of the request
// < Password:string=The password part of the request
// < Params:table=The key/value table of parameters parsed
// ? Parses the specified url and returns all the information about it. The
// ? 'Post' argument refers to one of the following...
// ? [0] Don't modify parameters.
// ? [1] URL Encode parameters (URL passed is not properly URL encoded).
// ? [2] URL Decode parameters (URL passed is already properly encoded).
/* ------------------------------------------------------------------------- */
LLFUNC(ParseUrl, 11,
  const AgString aUrl{lS, 1};
  const AgUIntLG aEncode{lS, 2, 0, 2};
  const Url uParsed{aUrl, aEncode};
  LuaUtilPushVar(lS, uParsed.GetResult());
  if(uParsed.GetResult() != Url::R_GOOD) return 1;
  LuaUtilPushVar(lS, uParsed.GetUrl(), uParsed.GetScheme(),
    uParsed.GetSecure(), uParsed.GetHost(), uParsed.GetPort(),
    uParsed.GetResource(), uParsed.GetBookmark(), uParsed.GetUsername(),
    uParsed.GetPassword());
  LuaUtilToTableEx(lS, uParsed))
/* ========================================================================= */
// $ Util.Pluralise
// > Count:integer=The number to check
// > Singular:string=The word to suffix if the number is singular
// > Plural:string=The word to suffix if the number is plural
// < Result:string=The resulting number plus the suffixed text
// ? Checks the number and suffixes the appropriate word depending on if the
// ? supplied number is singular or plural.
/* ------------------------------------------------------------------------- */
LLFUNC(Pluralise, 1,
  const AgUInt64 aValue{lS, 1};
  const AgCStringChar aSingular{lS, 2}, aPlural{lS, 3};
  LuaUtilPushVar(lS, StrCPluraliseNum(aValue, aSingular, aPlural)))
/* ========================================================================= */
// $ Util.PluraliseEx
// > Count:integer=The number to check
// > Singular:string=The word to suffix if the number is singular
// > Plural:string=The word to suffix if the number is plural
// < Result:string=The resulting number plus the suffixed text
// ? Checks the number and suffixes the appropriate word depending on if the
// ? supplied number is singular or plural. Uses the systems local settings
// ? to make sure the number is formatted properly.
/* ------------------------------------------------------------------------- */
LLFUNC(PluraliseEx, 1,
  const AgUInt64 aValue{lS, 1};
  const AgCStringChar aSingular{lS, 2}, aPlural{lS, 3};
  LuaUtilPushVar(lS, StrCPluraliseNumEx(aValue, aSingular, aPlural)))
/* ========================================================================= */
// $ Util.PlusOrMinus
// > Count:number=The number to convert.
// < Result:string=The resulting string.
// ? Prefixes the specified number with + or -.
/* ------------------------------------------------------------------------- */
LLFUNC(PlusOrMinus, 1,
  const AgDouble aValue{lS, 1};
  const AgInt aPrecision{lS, 2};
  LuaUtilPushVar(lS, StrPrefixPosNeg(aValue, aPrecision)))
/* ========================================================================= */
// $ Util.PlusOrMinusEx
// > Count:number=The number to convert.
// < Result:string=The resulting string.
// ? Prefixes the specified number with + or - and formats the number.
/* ------------------------------------------------------------------------- */
LLFUNC(PlusOrMinusEx, 1,
  const AgDouble aValue{lS, 1};
  const AgInt aPrecision{lS, 2};
  LuaUtilPushVar(lS, StrPrefixPosNegReadable(aValue, aPrecision)))
/* ========================================================================= */
// $ Util.Position
// > Position:integer=The position to convert to human readable form.
// < Result:string=The resulting string
// ? Converts the specified number into a positional number. i.e. 1=1st,
// ? 2="2nd", 11="11th", 98="98th", etc.
/* ------------------------------------------------------------------------- */
LLFUNC(Position, 1, LuaUtilPushVar(lS, StrFromPosition(AgUInt64{lS, 1})))
/* ========================================================================= */
// $ Util.RelTime
// > Seconds:integer=The unix timestamp.
// < Result:string=The resulting string
// ? Subtracts the specified time with the current time and converts it to a
// ? human readable string showing the time duration between the two.
/* ------------------------------------------------------------------------- */
LLFUNC(RelTime, 1, LuaUtilPushVar(lS, cmSys.ToDurationRel(AgTimeT{lS, 1})))
/* ========================================================================= */
// $ Util.Random
// < Result:Integer=The random number returned.
// ? Uses OpenSSL to generate a random 64-bit number. This data is constantly
// ? salted so you always get unique data. This function is provided for
// ? security over performance, otherwise you should use 'math.random()'.
/* ------------------------------------------------------------------------- */
LLFUNC(Random, 1, LuaUtilPushVar(lS, CryptRandom<lua_Integer>()))
/* ========================================================================= */
// $ Util.RandomA
// > Size:integer=The size of the random data.
// < Result:Asset=The random data returned.
// ? Uses OpenSSL to generate random data. This data is constantly salted so
// ? you always get unique data.
/* ------------------------------------------------------------------------- */
LLFUNC(RandomA, 1, AcAsset{lS}().MemSwap(CryptRandomBlock(AgSizeT{lS, 1})))
/* ========================================================================= */
// $ Util.RelTimeEx
// > Seconds:integer=The unix timestamp.
// > Count:integer=The maximum number of components.
// < Result:string=The resulting string
// ? Subtracts the specified time with the current time and converts it to a
// ? human readable string showing the time duration between the two whilst
// ? giving the option to keep the string short and simple by limiting the
// ? specified number of components to show.
/* ------------------------------------------------------------------------- */
LLFUNC(RelTimeEx, 1,
  const AgTimeT aTimestamp{lS, 1};
  const AgUInt aComponents{lS, 2};
  LuaUtilPushVar(lS, cmSys.ToDurationRel(aTimestamp, aComponents)))
/* ========================================================================= */
// $ Util.Replace
// > String:string=The string to search from
// > Search:string=The string to find
// > Replace:string=The string to replace with
// < Result:string=The resulting string
// ? A simple search/replace text mechanism without having to worry about
// ? errors with escape sequences with string:gsub. Uses standard c++ string
// ? class functions for functionality.
/* ------------------------------------------------------------------------- */
LLFUNC(Replace, 1,
  const AgString aValue{lS, 1}, aSearch{lS, 2}, aReplace{lS, 3};
  LuaUtilPushVar(lS, StrReplace(aValue, aSearch, aReplace)))
/* ========================================================================= */
// $ Util.ReplaceEx
// > String:string=The string to search from
// > Data:table=Object with the replacement data
// < Result:string=The resulting string
// ? Same as Replace but takes an object table as argument which has key
// ? value pairs which are the text to replace (as key) and the text to replace
// ? to (as value).
/* ------------------------------------------------------------------------- */
LLFUNC(ReplaceEx, 1, LuaUtilReplaceMulti(lS))
/* ========================================================================= */
// $ Util.Round
// > Value:string=A number value
// > Precision:integer=Maximum number of floating point digits
// < Value:number=The rounded value
// ? Rounds the specified number to the specified number of floating point
// ? digits up or down to the nearest zero.
/* ------------------------------------------------------------------------- */
LLFUNC(Round, 1,
  const AgDouble aValue{lS, 1};
  const AgInt aPrecision{lS, 2};
  LuaUtilPushVar(lS, UtilRound<lua_Number>(aValue(), aPrecision)))
/* ========================================================================= */
// $ Util.RoundInt
// > Value:string=A number value
// > Precision:integer=Maximum number of floating point digits
// < Value:integer=The rounded value
// ? Rounds the specified number to the specified number of floating point
// ? digits up or down to the nearest zero.
/* ------------------------------------------------------------------------- */
LLFUNC(RoundInt, 1,
  LuaUtilPushVar(lS, UtilRound<lua_Integer>(AgDouble{lS, 1}(), 0)))
/* ========================================================================= */
// $ Util.RoundMul
// > Value:integer=The integer to round up to the nearest multiple.
// > Multiplier:integer=The nearest multiple in which to round up to.
// < Result:integer=The resulting rounded value.
// ? Rounds the specified integer up to the nearest multiple of the specified
// ? multiplier.
/* ------------------------------------------------------------------------- */
LLFUNC(RoundMul, 1, const AgLuaNumber aValue{lS, 1}, aMultiplier{lS, 2};
  LuaUtilPushVar(lS, UtilNearest(aValue(), aMultiplier())))
/* ========================================================================= */
// $ Util.RoundPow2
// > Value:integer=The integer to round up to the nearest power of two
// < Result:integer=The resulting rounded value
// ? Rounds the specified integer up to the nearest power of two.
/* ------------------------------------------------------------------------- */
LLFUNC(RoundPow2, 1,
  LuaUtilPushVar(lS, UtilNearestPow2<lua_Integer>(AgLuaInteger{lS, 1}())))
/* ========================================================================= */
// $ Util.Sanitise
// > Text:string=The string to sanitise.
// < Text:string=The modified string.
// ? Tries to remove repetitions in the specified string.
/* ------------------------------------------------------------------------- */
LLFUNC(Sanitise, 1, LuaUtilPushVar(lS, CryptSanitise(AgString{lS, 1}())))
/* ========================================================================= */
// $ Util.StretchInner
// > OuterWidth:number=Outer width of the rectangle to adjust
// > OuterHeight:number=Outer height of the rectangle to adjust
// > InnerWidth:number=Inner width of the rectangle to adjust
// > InnerHeight:number=Inner height of the rectangle to adjust
// < OuterWidth:number=Outer width of the rectangle to adjust
// < OuterHeight:number=Outer height of the rectangle to adjust
// < InnerWidth:number=Inner width of the rectangle to adjust
// < InnerHeight:number=Inner height of the rectangle to adjust
// ? Scales the specified 2D inner rectangle to the outer bounds while
// ? preserving aspect ratio.
/* ------------------------------------------------------------------------- */
LLFUNC(StretchInner, 4,
  double dOW = LuaUtilGetNum<decltype(dOW)>(lS, 1),
         dOH = LuaUtilGetNum<decltype(dOH)>(lS, 2),
         dIW = LuaUtilGetNum<decltype(dIW)>(lS, 3),
         dIH = LuaUtilGetNum<decltype(dIH)>(lS, 4);
  UtilStretchToInner(dOW, dOH, dIW, dIH);
  LuaUtilPushVar(lS, dOW, dOH, dIW, dIH))
/* ========================================================================= */
// $ Util.StretchOuter
// > OuterWidth:number=Outer width of the rectangle to adjust
// > OuterHeight:number=Outer height of the rectangle to adjust
// > InnerWidth:number=Inner width of the rectangle to adjust
// > InnerHeight:number=Inner height of the rectangle to adjust
// < OuterWidth:number=Outer width of the rectangle to adjust
// < OuterHeight:number=Outer height of the rectangle to adjust
// < InnerWidth:number=Inner width of the rectangle to adjust
// < InnerHeight:number=Inner height of the rectangle to adjust
// ? Scales the specified 2D inner rectangle to the outer bounds while
// ? preserving aspect ratio.
/* ------------------------------------------------------------------------- */
LLFUNC(StretchOuter, 4,
  double dOW = LuaUtilGetNum<decltype(dOW)>(lS, 1),
         dOH = LuaUtilGetNum<decltype(dOH)>(lS, 2),
         dIW = LuaUtilGetNum<decltype(dIW)>(lS, 3),
         dIH = LuaUtilGetNum<decltype(dIH)>(lS, 4);
  UtilStretchToOuter(dOW, dOH, dIW, dIH);
  LuaUtilPushVar(lS, dOW, dOH, dIW, dIH))
/* ========================================================================= */
// $ Util.TableSize
// > Table:table=A key/value pairs table
// < Count:integer=Number of items in a table
// ? This function is only provided for convenience and should not normally
// ? be used. It enumerates a key/value pairs table and returns the number of
// ? items in it. It will also subtract the number of actual 1-indexed items
// ? if there are any present. It is recommended to manually record the table
// ? size yourself whilst building the key/value pairs table instead of using
// ? this.
/* ------------------------------------------------------------------------- */
LLFUNC(TableSize, 1, LuaUtilPushVar(lS, LuaUtilGetKeyValTableSize(lS)))
/* ========================================================================= */
// $ Util.Trim
// > String:string=The string to modify
// > Search:integer=The ASCII character to remove
// < Result:string=The trimmed string
// ? Removes the specified character that are prefixed and suffixed to the
// ? specified string.
/* ------------------------------------------------------------------------- */
LLFUNC(Trim, 1,
  const AgString aString{lS, 1};
  const AgUInt8 aChar{lS, 2};
  LuaUtilPushVar(lS, StrTrim(aString, static_cast<char>(aChar))))
/* ========================================================================= */
// $ Util.UTF8Char
// > Value:integer=Value to cast to a string
// < ByteCode:string=The 'value' cast as a string.
// ? Casts the specified 24-bit integer directly to a string which is
// ? different from 'utf8.char' as you specify the integer with the actual
// ? utf8 encoded bytes in them.
/* ------------------------------------------------------------------------- */
LLFUNC(UTF8Char, 1, LuaUtilPushVar(lS, UtfDecodeNum(AgUInt32{lS, 1})))
/* ========================================================================= */
// $ Util.UUIDDecode
// > High64:integer=The high-order 128-bit integer.
// > Low64:integer=The low-order 128-bit integer.
// < Result:string=The decoded UUID.
// ? Decodes the specified UUID integers to a string.
/* ------------------------------------------------------------------------- */
LLFUNC(UUIDDecode, 1, const AgUInt64 aHigh{lS, 1}, aLow{lS, 2};
  LuaUtilPushVar(lS, UuId{ aHigh, aLow }.UuIdToString()))
/* ========================================================================= */
// $ Util.UUIDEncode
// > UUID:string=The UUID string to recode.
// < High64:integer=The high-order 128-bit integer
// < Low64:integer=The low-order 128-bit integer
// ? Encodes the specified UUID string to two 64-bit integers.
/* ------------------------------------------------------------------------- */
LLFUNC(UUIDEncode, 2, const UuId uuidData{ AgString{lS, 1} };
  LuaUtilPushVar(lS, uuidData.d.qwRandom[0], uuidData.d.qwRandom[1]))
/* ========================================================================= */
// $ Util.UUIDRandom
// < High64:integer=The high-order 128-bit integer
// < Low64:integer=The low-order 128-bit integer
// ? Generates a random UUIDv4
/* ------------------------------------------------------------------------- */
LLFUNC(UUIDRandom, 2, const UuId uuidData;
  LuaUtilPushVar(lS, uuidData.d.qwRandom[0], uuidData.d.qwRandom[1]))
/* ========================================================================= */
// $ Util.UrlDecode
// > Text:string=The URL string to decode.
// < Text:string=The URL decoded string.
// ? URL decodes the specified string.
/* ------------------------------------------------------------------------- */
LLFUNC(UrlDecode, 1, LuaUtilPushVar(lS, CryptURLDecode(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.UrlEncode
// > Text:string=The URL string to encode.
// < Text:string=The URL encoded string.
// ? URL encodes the specified string.
/* ------------------------------------------------------------------------- */
LLFUNC(UrlEncode, 1, LuaUtilPushVar(lS, CryptURLEncode(AgString{lS, 1})))
/* ========================================================================= */
// $ Util.WordWrap
// > Text:string=The text to word wrap
// > Maximum:integer=The maximum number of characters per line.
// > Indent:integer=The indentation to wrap at.
// < Lines:table=The resulting indexed table of lines.
// ? Word wraps the specified string into a table of lines.
/* ------------------------------------------------------------------------- */
LLFUNC(WordWrap, 1,
  const AgNeString aText{lS, 1};
  const AgSizeT aWidth{lS, 2}, aIndent{lS, 3};
  LuaUtilToTable(lS, UtfWordWrap(aText, aWidth, aIndent)))
/* ========================================================================= **
** ######################################################################### **
** ## Util.* namespace functions structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Util.* namespace functions begin
  LLRSFUNC(AscNTime),      LLRSFUNC(AscNTimeUTC),    LLRSFUNC(AscTime),
  LLRSFUNC(AscTimeUTC),    LLRSFUNC(B64D),           LLRSFUNC(B64DA),
  LLRSFUNC(B64E),          LLRSFUNC(B64EA),          LLRSFUNC(Bits),
  LLRSFUNC(Blank),         LLRSFUNC(Bytes),          LLRSFUNC(Capitalise),
  LLRSFUNC(Chop),          LLRSFUNC(Clamp),          LLRSFUNC(ClampInt),
  LLRSFUNC(Compact),       LLRSFUNC(CountOf),        LLRSFUNC(CRC),
  LLRSFUNC(CRCA),          LLRSFUNC(Duration),       LLRSFUNC(EntDecode),
  LLRSFUNC(EntEncode),     LLRSFUNC(Explode),        LLRSFUNC(ExplodeEx),
  LLRSFUNC(FormatNTime),   LLRSFUNC(FormatNTimeUTC), LLRSFUNC(FormatNumber),
  LLRSFUNC(FormatNumberI), LLRSFUNC(FormatTime),     LLRSFUNC(FormatTimeUTC),
  LLRSFUNC(GetRatio),      LLRSFUNC(Grouped),        LLRSFUNC(Hex),
  LLRSFUNC(HexDecode),     LLRSFUNC(HexDecodeA),     LLRSFUNC(HexEncode),
  LLRSFUNC(HexEncodeA),    LLRSFUNC(HexEncodeL),     LLRSFUNC(HexEncodeLA),
  LLRSFUNC(HexL),          LLRSFUNC(HighByte),       LLRSFUNC(HighDWord),
  LLRSFUNC(HighWord),      LLRSFUNC(IfBlank),        LLRSFUNC(Implode),
  LLRSFUNC(ImplodeEx),     LLRSFUNC(IsASCII),        LLRSFUNC(IsBoolean),
  LLRSFUNC(IsExtASCII),    LLRSFUNC(IsFunction),     LLRSFUNC(IsInteger),
  LLRSFUNC(IsNumber),      LLRSFUNC(IsString),       LLRSFUNC(IsTable),
  LLRSFUNC(IsUserdata),    LLRSFUNC(LDuration),      LLRSFUNC(LDurationEx),
  LLRSFUNC(LowByte),       LLRSFUNC(LowDWord),       LLRSFUNC(LowWord),
  LLRSFUNC(MakeDWord),     LLRSFUNC(MakeQWord),      LLRSFUNC(MakeWord),
  LLRSFUNC(ParseArgs),     LLRSFUNC(ParseTime),      LLRSFUNC(ParseTime2),
  LLRSFUNC(ParseTimeEx),   LLRSFUNC(ParseUrl),       LLRSFUNC(Pluralise),
  LLRSFUNC(PluraliseEx),   LLRSFUNC(PlusOrMinus),    LLRSFUNC(PlusOrMinusEx),
  LLRSFUNC(Position),      LLRSFUNC(Random),         LLRSFUNC(RandomA),
  LLRSFUNC(RelTime),       LLRSFUNC(RelTimeEx),     LLRSFUNC(Replace),
  LLRSFUNC(ReplaceEx),     LLRSFUNC(Round),         LLRSFUNC(RoundInt),
  LLRSFUNC(RoundMul),      LLRSFUNC(RoundPow2),     LLRSFUNC(SHA1AA),
  LLRSFUNC(SHA1AAA),       LLRSFUNC(SHA1AAS),       LLRSFUNC(SHA1AS),
  LLRSFUNC(SHA1ASA),       LLRSFUNC(SHA1ASS),       LLRSFUNC(SHA1SA),
  LLRSFUNC(SHA1SAA),       LLRSFUNC(SHA1SAS),       LLRSFUNC(SHA1SS),
  LLRSFUNC(SHA1SSA),       LLRSFUNC(SHA1SSS),       LLRSFUNC(SHA224AA),
  LLRSFUNC(SHA224AAA),     LLRSFUNC(SHA224AAS),     LLRSFUNC(SHA224AS),
  LLRSFUNC(SHA224ASA),     LLRSFUNC(SHA224ASS),     LLRSFUNC(SHA224SA),
  LLRSFUNC(SHA224SAA),     LLRSFUNC(SHA224SAS),     LLRSFUNC(SHA224SS),
  LLRSFUNC(SHA224SSA),     LLRSFUNC(SHA224SSS),     LLRSFUNC(SHA256AA),
  LLRSFUNC(SHA256AAA),     LLRSFUNC(SHA256AAS),     LLRSFUNC(SHA256AS),
  LLRSFUNC(SHA256ASA),     LLRSFUNC(SHA256ASS),     LLRSFUNC(SHA256SA),
  LLRSFUNC(SHA256SAA),     LLRSFUNC(SHA256SAS),     LLRSFUNC(SHA256SS),
  LLRSFUNC(SHA256SSA),     LLRSFUNC(SHA256SSS),     LLRSFUNC(SHA384AA),
  LLRSFUNC(SHA384AAA),     LLRSFUNC(SHA384AAS),     LLRSFUNC(SHA384AS),
  LLRSFUNC(SHA384ASA),     LLRSFUNC(SHA384ASS),     LLRSFUNC(SHA384SA),
  LLRSFUNC(SHA384SAA),     LLRSFUNC(SHA384SAS),     LLRSFUNC(SHA384SS),
  LLRSFUNC(SHA384SSA),     LLRSFUNC(SHA384SSS),     LLRSFUNC(SHA512AA),
  LLRSFUNC(SHA512AAA),     LLRSFUNC(SHA512AAS),     LLRSFUNC(SHA512AS),
  LLRSFUNC(SHA512ASA),     LLRSFUNC(SHA512ASS),     LLRSFUNC(SHA512SA),
  LLRSFUNC(SHA512SAA),     LLRSFUNC(SHA512SAS),     LLRSFUNC(SHA512SS),
  LLRSFUNC(SHA512SSA),     LLRSFUNC(SHA512SSS),     LLRSFUNC(Sanitise),
  LLRSFUNC(StretchInner),  LLRSFUNC(StretchOuter),  LLRSFUNC(TableSize),
  LLRSFUNC(Trim),          LLRSFUNC(UrlDecode),     LLRSFUNC(UrlEncode),
  LLRSFUNC(UTF8Char),      LLRSFUNC(UUIDDecode),    LLRSFUNC(UUIDEncode),
  LLRSFUNC(UUIDRandom),    LLRSFUNC(WordWrap),
LLRSEND                                // Util.* namespace functions end
/* ========================================================================= */
}                                      // End of Util namespace
/* == EoF =========================================================== EoF == */
