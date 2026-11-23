/* == OAL.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Because OpenAL is LGPL, we're not really allowed to statically link ## **
** ## OpenAL libraries, however, there is a technical workaround that we  ## **
** ## can still use the static lib if we provide a facility to load if an ## **
** ## external version of OpenAL.dll is available. So thats why this      ## **
** ## class exists, so we can create a pointer to all the functions we    ## **
** ## will use in AL and switch between them.                             ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IOal {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace ICVarDef::P;
using namespace IError::P;             using namespace IFlags::P;
using namespace IGCHelper::P;          using namespace IHelper::P;
using namespace ILog::P;               using namespace ILookupMap::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IStdLib::P;            using namespace IString::P;
using namespace ISysUtil::P;           using namespace IToken::P;
using namespace LOal;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- GL error checking wrapper macros ------------------------------------- */
#define ALEX(EF,F,M,...)  do{ F; EF(M, ##__VA_ARGS__); }while(false) // :(
#define ALL(F,M,...)      ALEX(cOal->OalLogALError, F, M, ##__VA_ARGS__)
#define AL(F,M,...)       ALEX(cOal->OalCheckALError, F, M, ##__VA_ARGS__)
#define ALNF(M,...)       AL(static_cast<void>(0), M, ##__VA_ARGS__)
/* -- GL context error checking wrapper macros ----------------------------- */
#define ALCL(F,M,...)     ALEX(cOal->OalLogALCError, F, M, ##__VA_ARGS__)
#define ALCLNF(M,...)     ALCL(static_cast<void>(0), M, ##__VA_ARGS__)
#define ALC(F,M,...)      ALEX(cOal->OalCheckALCError, F, M, ##__VA_ARGS__)
#define ALCNF(M,...)      ALC(static_cast<void>(0), M, ##__VA_ARGS__)
/* -- Typedefs ------------------------------------------------------------- */
using ALUIntVector = StdVector<ALuint>; // A vector of ALuint's
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(unsigned, Oal,             // OpenAL flags
  /* -- OAL specific flags ------------------------------------------------- */
  AFL_NONE               {OalFlag[0]}, // No flags
  AFL_INITDEVICE         {OalFlag[1]}, // Device has been initialised?
  AFL_INITCONTEXT        {OalFlag[2]}, // Context has been initialised?
  AFL_INFINITESOURCES    {OalFlag[3]}, // Have infinite sources?
  AFL_CONTEXTCURRENT     {OalFlag[4]}, // Context has been made current?
  AFL_INITIALISED        {OalFlag[5]}, // OpenAL fully initialised
  AFL_HAVE32FPPB         {OalFlag[6]}, // Can play 32-bit float audio?
  AFL_HAVEENUMEXT        {OalFlag[7]}, // Have ALC_ENUMERATE_ALL_EXT?
  AFL_HAVESEPBDDC        {OalFlag[8]}, // Playback change def device event?
  AFL_HAVESECADDC        {OalFlag[9]}, // Capture change default device event?
  AFL_HAVESEPBDA        {OalFlag[10]}, // Playback add device event?
  AFL_HAVESECADA        {OalFlag[11]}, // Capture add device event?
  AFL_HAVESEPBDR        {OalFlag[12]}, // Playback remove device event?
  AFL_HAVESECADR        {OalFlag[13]}, // Capture remove device event?
  AFL_REINIT            {OalFlag[14]}, // Audio system is resetting?
  AFL_HRTFREQ           {OalFlag[15]}, // HRTF is requested enabled?
  AFL_HRTFINIT          {OalFlag[16]}  // HRTF init was successful?
);/* ----------------------------------------------------------------------- */
/* -- Class prototype and global ------------------------------------------- */
class Oal;                             // Class prototype
static Oal *cOal = nullptr;            // Pointer to global class
/* -- Class body ----------------------------------------------------------- */
class Oal :                            // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  GCHelper<Oal>,                       // Global pointer helper
  InitHelper,                          // Initialisation helper
  public OalFlags                      // OpenAL flags
{ /* -- Error macros for use inside the class only ------------------------- */
#define IAL(F,M,...)  ALEX(OalCheckALError, F, M, ##__VA_ARGS__)
#define IALNF(M,...)  IAL(static_cast<void>(0), M, ##__VA_ARGS__)
#define IALC(F,M,...) ALEX(OalCheckALCError, F, M, ##__VA_ARGS__)
#define IALCNF(M,...) IALC(static_cast<void>(0), M, ##__VA_ARGS__)
  /* ----------------------------------------------------------------------- */
  using ALenumMap = LookupMap<ALenum>; // A map of ALenum integers
  const ALenumMap  alemOALCodes,       // OpenAL standard error codes
                   alemOALCCodes,      // OpenAL context error codes
                   alemFormatCodes;    // OpenAL format codes
  /* ----------------------------------------------------------------------- */
  size_t           stMaxStereoSources, // Maximum number of stereo sources
                   stMaxMonoSources;   // Maximum number of mono sources
  /* ----------------------------------------------------------------------- */
  StdString        strVersion;         // String version of OpenAL
  StdStringView    ssvPlayback;        // String playback device
  /* ----------------------------------------------------------------------- */
  ALCdevice       *alcDevice;          // OpenAL device
  ALCcontext      *alcContext;         // OpenAL context
  /* ----------------------------------------------------------------------- */
  bool             bHave32FPPB;        // Cached version of 32-bit float cap
  /* -- Public Variables --------------------------------------------------- */
  ALenum           aleQuery;           // Device query devices command
  /* -- AL generic context error logger ------------------------------------ */
  template<typename ALEnumType, ALEnumType aletNoErr, typename FuncTypeErr,
    typename FuncTypeStr, typename StrType, typename ...VarArgs>
  void OalLogErrorGeneric(FuncTypeErr fteFunc, FuncTypeStr ftsFunc,
    const StdStringView &ssvPrefix, StrType &&strFormat,
    VarArgs &&...vaArgs) const
  { // Run the error function and store and compare the result
    switch(const ALEnumType aletError = fteFunc())
    { // No error so return
      case aletNoErr: return;
      // Anything else is an error
      default: cLog->LogWarningExSafe("AL$ call failed: $ ($/0x$$).",
        ssvPrefix, StrFormat(StdForward<StrType>(strFormat),
        StdForward<VarArgs>(vaArgs)...), ftsFunc(aletError),
        StdIOSHex, aletError);
    } // Executing never gets here. Note that al(c)GetError() isn't a queue
  }
  /* -- AL generic exception handler --------------------------------------- */
  template<typename ALEnumType, ALEnumType aletNoErr, typename FuncTypeErr,
    typename FuncTypeStr, typename ...VarArgs>
  void OalCheckErrorGeneric(FuncTypeErr fteFunc, FuncTypeStr ftsFunc,
    const char*const cpType, const char*const cpReason, VarArgs &&...vaArgs)
      const
  { // Run the error function and store and compare the result
    switch(const ALEnumType aletError = fteFunc())
    { // No error so return
      case aletNoErr: return;
      // Anything else build an exception object to show why the call failed
      default: XC(StrFormat("AL$ call failed: $", cpType, cpReason),
                    StdForward<VarArgs>(vaArgs)...,
                    "Code", aletError, "Reason", ftsFunc(aletError));
    } // Executing never gets here. Note that al(c)GetError() isn't a queue
  }
  /* -- Set context -------------------------------------------------------- */
  static void OalMakeContextCurrent(ALCcontext*const alcNContext = nullptr)
    { alcMakeContextCurrent(alcNContext); }
  /* -- Destroy the context ------------------------------------------------ */
  bool OalDestroyContext()
  { // If context is set?
    if(alcContext)
    { // Context is acquired
      if(FlagIsSet(AFL_CONTEXTCURRENT))
      { // Release the context
        cLog->LogDebugSafe("Oal releasing playback context...");
        OalMakeContextCurrent();
        OalLogALCError("Oal failed to release context with flags 0x$$!",
          StdIOSHex, FlagGet());
        cLog->LogDebugSafe("Oal released playback context.");
      } // Destroy the context
      cLog->LogDebugSafe("Oal deinitialising playback context...");
      alcDestroyContext(alcContext);
      OalLogALCError("Oal failed to deinitialise context with flags 0x$$!",
        StdIOSHex, FlagGet());
      cLog->LogDebugSafe("Oal deinitialising playback context.");
      // Success
      return true;
    } // Context is not set but init and acquire flag is set
    if(FlagIsAnyOfSet(AFL_CONTEXTCURRENT | AFL_INITCONTEXT))
    { // Clear the context flag and show warning
      cLog->LogWarningExSafe(
        "Oal cannot destroy a null context with flags 0x$$!",
        StdIOSHex, FlagGet());
      // Already unloaded but success anyway
      return true;
    } // Nothing unloaded
    return false;
  }
  /* -- Destroy the device ------------------------------------------------- */
  bool OalDestroyDevice()
  { // If device is set?
    if(alcDevice)
    { // Log that we're closing the device
      cLog->LogDebugSafe("Oal destroying playback device...");
      // Close the device
      alcCloseDevice(alcDevice);
      // Because alcDevice is now invalidated we can't use it anymore and
      // therefore we can't use the standed LogALCError() function
      switch(const ALCenum alceError = alcGetError(nullptr))
      { // Success
        case ALC_NO_ERROR: cLog->LogDebugSafe(
          "Oal destroyed playback device.");
           break;
        // Failed so report as warning in log
        default: cLog->LogWarningExSafe(
          "Oal destroy device failed with $[0x$$]!",
          OalGetALCErr(alceError), StdIOSHex, alceError);
          break;
      } // Success
      return true;
    } // Device is not set but init and acquire flag is set
    else if(FlagIsAnyOfSet(AFL_INITDEVICE))
    { // Clear the context flag and show warning
      cLog->LogWarningExSafe(
        "Oal cannot destroy a null device with flags 0x$$!",
        StdIOSHex, FlagGet());
      // Already unloaded but success anyway
      return true;
    } // Nothing unloaded
    return false;
  }
  /* -- Do deInitialise ---------------------------------------------------- */
  void OalDeInitNoReset()
  { // Log that we're deinitialising
    cLog->LogDebugSafe("Oal class deinitialising...");
    // Destroy the context before the device
    OalDestroyContext();
    OalDestroyDevice();
    // Finished so log that we did
    cLog->LogDebugSafe("Oal class deinitialised.");
  }
  /* -- Return current error for specified device -------------------------- */
  ALenum OalGetContextError() const { return alcGetError(alcDevice); }
  /* -- Enable or disable system events ------------------------------------ */
  ALenum OalSetEventState(const ALCenum alceEvent,
    const ALCboolean alcbEnabled)
  { // Keeping Ubuntu 26.04 compatibility for now until supported
#if defined(LINUX)
    return AL_FALSE;
#else
    return alcEventControlSOFT(1, &alceEvent, alcbEnabled);
#endif
  }
  /* -- Is system event supported ------------------------------------------ */
  ALenum OalIsEventSupported(const ALenum aleEventType,
    const ALenum aleDeviceType)
  { // Keeping Ubuntu 26.04 compatibility for now until supported
#if defined(LINUX)
    return ALC_EVENT_NOT_SUPPORTED_SOFT;
#else
    return alcEventIsSupportedSOFT(aleEventType, aleDeviceType);
#endif
  }
  /* -- Return error status ---------------------------------------- */ public:
  static ALenum OalGetError() { return alGetError(); }
  static bool OalHaveError() { return OalGetError() != AL_NO_ERROR; }
  /* -- Get query method --------------------------------------------------- */
  ALenum OalGetQueryMethod() const { return aleQuery; }
  /* -- Return version information ----------------------------------------- */
  const StdString &OalGetVersion() const { return strVersion; }
  /* -- AL context error logger -------------------------------------------- */
  template<typename StrType, typename ...VarArgs>
    void OalLogALCError(StrType &&strFormat, VarArgs &&...vaArgs) const
  { OalLogErrorGeneric<ALCenum, ALC_NO_ERROR>(
      [this]()->ALCenum{ return OalGetContextError(); },
      [this](const ALCenum alceError){ return OalGetALCErr(alceError); },
      " context", StdForward<StrType>(strFormat),
      StdForward<VarArgs>(vaArgs)...); }
  /* -- AL error logger ---------------------------------------------------- */
  template<typename StrType, typename ...VarArgs>
    void OalLogALError(StrType &&strFormat, VarArgs &&...vaArgs) const
  { OalLogErrorGeneric<ALenum, AL_NO_ERROR>(
      []()->ALenum{ return OalGetError(); },
      [this](const ALenum aleError){ return OalGetALErr(aleError); },
      cCommon->CommonBlank(), StdForward<StrType>(strFormat),
      StdForward<VarArgs>(vaArgs)...); }
  /* -- AL context error handler ------------------------------------------- */
  template<typename ...VarArgs>
    void OalCheckALCError(const char*const cpReason, VarArgs &&...vaArgs) const
  { OalCheckErrorGeneric<ALCenum, ALC_NO_ERROR>(
      [this]()->ALCenum{ return OalGetContextError(); },
      [this](const ALCenum alceError) { return OalGetALCErr(alceError); },
      " context", cpReason, StdForward<VarArgs>(vaArgs)...); }
  /* -- AL error handler --------------------------------------------------- */
  template<typename ...VarArgs>
    void OalCheckALError(const char*const cpReason, VarArgs &&...vaArgs) const
  { OalCheckErrorGeneric<ALenum, AL_NO_ERROR>(
      []()->ALenum{ return OalGetError(); },
      [this](const ALenum aleError) { return OalGetALErr(aleError); },
      caBlank, cpReason, StdForward<VarArgs>(vaArgs)...); }
  /* -- Upload data to audio device ---------------------------------------- */
  static void OalBufferData(const ALuint aluBuffer, const ALenum aleFormat,
    const ALvoid*const vpData, const ALsizei siSize,
    const ALsizei alsiFrequency)
  { alBufferData(aluBuffer, aleFormat, vpData, siSize, alsiFrequency); }
  /* -- Upload data to audio device ---------------------------------------- */
  static void OalBufferData(const ALuint aluBuffer, const ALenum aleFormat,
    const MemConst &mcSrc, const ALsizei alsiFrequency)
  { OalBufferData(aluBuffer, aleFormat, mcSrc.MemPtr<ALvoid>(),
      mcSrc.MemSize<ALsizei>(), alsiFrequency); }
  /* -- Queue specified buffer count into source --------------------------- */
  static void OalQueueBuffers(const ALuint aluSource, const ALsizei alsiCount,
    ALuint*const alupBuffer)
  { alSourceQueueBuffers(aluSource, alsiCount, alupBuffer); }
  /* -- Queue one buffer count into source --------------------------------- */
  static void OalQueueBuffer(const ALuint aluSource, ALuint aluBuffer)
    { OalQueueBuffers(aluSource, 1, &aluBuffer); }
  /* -- Unqueue specified buffer count from source and place into buffers -- */
  static void OalUnQueueBuffers(const ALuint aluSource,
    const ALsizei alsiCount, ALuint*const alupBuffer)
  { alSourceUnqueueBuffers(aluSource, alsiCount, alupBuffer); }
  /* -- Unqueue one buffer from source and place into buffers -------------- */
  static void OalUnQueueBuffer(const ALuint aluSource, ALuint &aluBuffer)
    { OalUnQueueBuffers(aluSource, 1, &aluBuffer); }
  /* -- Unqueue one buffer from source and return it ----------------------- */
  static ALuint OalUnQueueBuffer(const ALuint aluSource)
    { ALuint aluBuffer;
      OalUnQueueBuffer(aluSource, aluBuffer);
      return aluBuffer; }
  /* -- Set source value as float ------------------------------------------ */
  static void OalSetSourceFloat(const ALuint aluSource, const ALenum aleWhat,
    const ALfloat fValue)
  { alSourcef(aluSource, aleWhat, fValue); }
  /* -- Get source value as float ------------------------------------------ */
  static void OalGetSourceFloat(const ALuint aluSource, const ALenum aleWhat,
    ALfloat*const fpDestValue)
  { alGetSourcef(aluSource, aleWhat, fpDestValue); }
  /* -- Set source value as int -------------------------------------------- */
  static void OalSetSourceInt(const ALuint aluSource, const ALenum aleWhat,
    const ALint iValue)
  { alSourcei(aluSource, aleWhat, iValue); }
  /* -- Get source value as int -------------------------------------------- */
  static void OalGetSourceInt(const ALuint aluSource, const ALenum aleWhat,
    ALint*const alipDestValue)
  { alGetSourcei(aluSource, aleWhat, alipDestValue); }
  /* -- Get source value as a float vector --------------------------------- */
  static void OalGetSourceVector(const ALuint aluSource, const ALenum aleWhat,
    ALfloat*const alfpDX, ALfloat*const alfpDY, ALfloat*const alfpDZ)
  { alGetSource3f(aluSource, aleWhat, alfpDX, alfpDY, alfpDZ); }
  /* -- Set source value as a float vector --------------------------------- */
  static void OalSetSourceVector(const ALuint aluSource, const ALenum aleWhat,
    const ALfloat alfDX, const ALfloat alfDY, const ALfloat alfDZ)
      { alSource3f(aluSource, aleWhat, alfDX, alfDY, alfDZ); }
  /* -- Stop a source from playing ----------------------------------------- */
  static void OalStopSource(const ALuint aluSource)
    { alSourceStop(aluSource); }
  /* -- Play a source ------------------------------------------------------ */
  static void OalPlaySource(const ALuint aluSource)
    { alSourcePlay(aluSource); }
  /* -- Play more than one source simultaniously --------------------------- */
  template<class ListType>
    requires StdHasDataSize<ListType>
  static void OalPlaySources(const ListType &ltSources)
    { alSourcePlayv(static_cast<ALsizei>(ltSources.size()),
        ltSources.data()); }
  /* -- Rewind a source ---------------------------------------------------- */
  static void OalRewindSource(const ALuint aluSource)
    {alSourceRewind(aluSource);}
  /* -- Pause a source ---------------------------------------------------- */
  static void OalPauseSource(const ALuint aluSource)
    { alSourcePause(aluSource); }
  /* -- Create multiple sources -------------------------------------------- */
  static void OalCreateSources(const ALsizei alsiCount,
    ALuint*const alupSources)
  { alGenSources(alsiCount, alupSources); }
  /* -- Create one source and place it in the specified buffer ------------- */
  static void OalCreateSource(ALuint &aluSourceRef)
    { OalCreateSources(1, &aluSourceRef); }
  /* -- Create and return a source ----------------------------------------- */
  static ALuint OalCreateSource()
    { ALuint aluSource; OalCreateSource(aluSource); return aluSource; }
  /* -- Delete multiple sources -------------------------------------------- */
  static void OalDeleteSources(const ALsizei alsiCount,
    const ALuint*const alupSources)
  { alDeleteSources(alsiCount, alupSources); }
  /* -- Delete multiple sources -------------------------------------------- */
  template<class ListType>
    requires StdHasDataSize<ListType>
  static void OalDeleteSources(const ListType &ltSources)
    { OalDeleteSources(static_cast<ALsizei>(ltSources.size()),
        ltSources.data()); }
  /* -- Delete one source -------------------------------------------------- */
  static void OalDeleteSource(const ALuint &aluSourceRef)
    { OalDeleteSources(1, &aluSourceRef); }
  /* -- Create multiple buffers -------------------------------------------- */
  static void OalCreateBuffers(const ALsizei alsiCount,
    ALuint*const alupBuffer)
  { alGenBuffers(alsiCount, alupBuffer); }
  /* -- Create multiple buffers -------------------------------------------- */
  template<class ListType>
    requires StdHasDataSize<ListType>
  static void OalCreateBuffers(ListType &ltBuffers)
    { OalCreateBuffers(static_cast<ALsizei>(ltBuffers.size()),
        ltBuffers.data()); }
  /* -- Create one buffer and place it in the specified variable ----------- */
  static void OalCreateBuffer(ALuint &aluBuffer)
    { OalCreateBuffers(1, &aluBuffer); }
  /* -- Create and return a buffer ----------------------------------------- */
  static ALuint OalCreateBuffer()
    { ALuint aluBuffer; OalCreateBuffer(aluBuffer); return aluBuffer; }
  /* -- Delete multiple buffers -------------------------------------------- */
  static void OalDeleteBuffers(const ALsizei alsiCount,
    const ALuint*const alupBuffer)
  { alDeleteBuffers(alsiCount, alupBuffer); }
  /* -- Delete multiple sources -------------------------------------------- */
  template<class ListType>
    requires StdHasDataSize<ListType>
  static void OalDeleteBuffers(const ListType &ltBuffers)
    { OalDeleteBuffers(static_cast<ALsizei>(ltBuffers.size()),
        ltBuffers.data()); }
  /* -- Delete one buffer -------------------------------------------------- */
  static void OalDeleteBuffer(const ALuint &aluBufferRef)
    { OalDeleteBuffers(1, &aluBufferRef); }
  /* -- Checks if a buffer id is valid ------------------------------------- */
  static bool OalIsBuffer(const ALuint aluBuffer)
    { return alIsBuffer(aluBuffer) == AL_TRUE; }
  /* -- Get buffer parameter as integer ------------------------------------ */
  static void OalGetBufferInt(const ALuint aluBuffer, const ALenum aleId,
    ALint*const alipDest)
  { alGetBufferi(aluBuffer, aleId, alipDest); }
  /* -- Get buffer information --------------------------------------------- */
  template<typename IntType = ALint>
    requires StdIsIntegral<IntType>
  static IntType OalGetBufferInt(const ALuint aluBuffer, const ALenum aleId)
  { // Create storage, populate and retun it
    ALint aliV;
    OalGetBufferInt(aluBuffer, aleId, &aliV);
    return static_cast<IntType>(aliV);
  }
  /* -- Set distance model ------------------------------------------------- */
  static void OalSetDistanceModel(const ALenum aleModel)
    { alDistanceModel(aleModel); }
  /* -- Set listener vector ------------------------------------------------ */
  static void OalSetListenerVector(const ALenum aleParam, const ALfloat fX,
    const ALfloat fY, const ALfloat fZ)
  { alListener3f(aleParam, fX, fY, fZ); }
  /* -- Set listener position ---------------------------------------------- */
  static void OalSetListenerPosition(const ALfloat fX, const ALfloat fY,
    const ALfloat fZ)
  { OalSetListenerVector(AL_POSITION, fX, fY, fZ); }
  /* -- Set listener velocity ---------------------------------------------- */
  static void OalSetListenerVelocity(const ALfloat fX, const ALfloat fY,
    const ALfloat fZ)
  { OalSetListenerVector(AL_VELOCITY, fX, fY, fZ); }
  /* -- Set listener velocity ---------------------------------------------- */
  static void OalSetListenerVectors(const ALenum aleParam,
    const ALfloat*const fpVectors)
  { alListenerfv(aleParam, fpVectors); }
  /* -- Set listener orientation ------------------------------------------- */
  static void OalSetListenerOrientation(const ALfloat*const fpVectors)
    { OalSetListenerVectors(AL_ORIENTATION, fpVectors); }
  /* -- Is extension present ----------------------------------------------- */
  static bool OalHaveExtension(const char*const cpEnum)
    { return alIsExtensionPresent(cpEnum) != AL_FALSE; }
  /* ----------------------------------------------------------------------- */
  static bool OalHaveCExtension(const char*const cpEnum,
    ALCdevice*const alcDev)
  { return alcIsExtensionPresent(alcDev, cpEnum) != AL_FALSE; }
  /* ----------------------------------------------------------------------- */
  bool OalHaveCExtension(const char*const cpEnum) const
    { return OalHaveCExtension(cpEnum, alcDevice); }
  /* ----------------------------------------------------------------------- */
  static bool OalHaveNCExtension(const char*const cpEnum)
    { return OalHaveCExtension(cpEnum, nullptr); }
  /* -- Detect enumeration method ------------------------------------------ */
  void OalDetectEnumerationMethod()
  { // Get if we have ALC_ENUMERATE_ALL_EXT
    if(OalHaveNCExtension("ALC_ENUMERATE_ALL_EXT"))
    { // Set that we have the extension
      FlagSet(AFL_HAVEENUMEXT);
      // Set extended device query extension
      aleQuery = ALC_ALL_DEVICES_SPECIFIER;
    } // No extended query available? Set classic query
    else aleQuery = ALC_DEVICE_SPECIFIER;
  }
  /* == Convert bitrate and channels to an openal useful identifier ======== */
  static bool GetOALType(const ALuint aluChannels, const ALuint aluBits,
    ALenum &aleFormat, ALenum &aleSFormat)
  { // Compare channels
    switch(aluChannels)
    { // MONO: 1 channel
      case 1:
      { // Compare bit count
        switch(aluBits)
        { // 8-bits per sample (Integer)
          case  8: aleFormat = aleSFormat = AL_FORMAT_MONO8;
                   return true;
          // 16-bits per sample (Integer)
          case 16: aleFormat = aleSFormat = AL_FORMAT_MONO16;
                   return true;
          // 32-bits per sample (Float)
          case 32: aleFormat = aleSFormat = AL_FORMAT_MONO_FLOAT32;
                   return true;
          // Unknown
          default: return false;
        }
      } // STEREO: 2 channels
      case 2:
      { // Compare bit count
        switch(aluBits)
        { // 8-bits per sample (Integer)
          case  8: aleFormat = AL_FORMAT_STEREO8;
                   aleSFormat = AL_FORMAT_MONO8;
                   return true;
          // 16-bits per sample (Integer)
          case 16: aleFormat = AL_FORMAT_STEREO16;
                   aleSFormat = AL_FORMAT_MONO16;
                   return true;
          // 32-bits per sample (Float)
          case 32: aleFormat = AL_FORMAT_STEREO_FLOAT32;
                   aleSFormat = AL_FORMAT_MONO_FLOAT32;
                   return true;
          // Unknown
          default: return false;
        }
      } // Unknown
      default: return false;
    }
  }
  /* -- Report floating point playback capability (in audio thread!!) ------ */
  bool OalHave32FPPB() const { return bHave32FPPB; }
  /* -- Get openAL string from currently selected device and context ------- */
  template<typename PtrType = ALchar*>
    requires StdIsPointer<PtrType>
  const PtrType OalGetString(const ALenum aleId) const
  { // Get the checked variable and return it
    const ALchar*const alchStr = alGetString(aleId);
    IALNF("Get string failed!", "Index", aleId);
    if(StdIsCStringNotValid(alchStr))
      XC("Invalid string returned!", "Index", aleId, "String", alchStr);
    return StdToNonConstCast<const PtrType>(alchStr);
  }
  /* -- Get context openAL string ------------------------------------------ */
  template<typename PtrType = ALCchar*>
    requires StdIsPointer<PtrType>
  static const PtrType OalGetContextString(ALCdevice*const alcDev,
    const ALCenum alcId)
  { return StdToNonConstCast<const PtrType>(alcGetString(alcDev, alcId)); }
  /* -- Get context openAL string (current device) ------------------------- */
  template<typename PtrType = ALCchar*>
    requires StdIsPointer<PtrType>
  const PtrType OalGetContextString(const ALCenum alceId) const
    { return OalGetContextString<PtrType>(alcDevice, alceId); }
  /* -- Get nullptr context openAL string ---------------------------------- */
  template<typename PtrType = ALCchar*>
    requires StdIsPointer<PtrType>
  static const PtrType OalGetNullContextString(const ALCenum alceId)
    { return OalGetContextString<PtrType>(nullptr, alceId); }
  /* -- Get openAL int array ----------------------------------------------- */
  template<size_t stCount, class IntType = StdArray<ALCint, stCount>>
    const IntType OalGetContextIntArray(const ALCenum alceId) const
  { // Create array, check, populate and return it
    IntType itData;
    IALC(alcGetIntegerv(alcDevice, alceId, sizeof(IntType), itData.data()),
      "Get integer array failed!", "Index", alceId, "Count", stCount);
    return itData;
  }
  /* -- Get openAL int ----------------------------------------------------- */
  template<typename IntType = ALCint>
    requires StdIsIntegral<IntType>
  IntType OalGetContextInt(const ALCenum alceId) const
    { return static_cast<IntType>(OalGetContextIntArray<1>(alceId)[0]); }
  /* -- Convert PCM format identifier to short identifier string ----------- */
  const StdStringView &OalFormatToString(const ALenum aleFormat) const
    { return alemFormatCodes.Get(aleFormat); }
  /* -- Get source counts -------------------------------------------------- */
  size_t OalGetMaxMonoSources() const { return stMaxMonoSources; }
  size_t OalGetMaxStereoSources() const { return stMaxStereoSources; }
  /* -- Get current playback device ---------------------------------------- */
  const StdStringView &OalGetPlaybackDevice() const { return ssvPlayback; }
  /* -- Set system event callback ------------------------------------------ */
  void OalSetEventCallback(const ALCEVENTPROCTYPESOFT cbProc,
    void*const vpParam)
  { // Keeping Ubuntu 26.04 compatibility for now until supported
#if !defined(LINUX)
    return alcEventCallbackSOFT(cbProc, vpParam);
#endif
  }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType>
  const StdStringView &OalGetALErr(const IntType tCode) const
    { return alemOALCodes.Get(static_cast<ALenum>(tCode)); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType>
  const StdStringView &OalGetALCErr(const IntType tCode) const
    { return alemOALCCodes.Get(static_cast<ALenum>(tCode)); }
  /* -- AL is initialised? ------------------------------------------------- */
  bool OalIsInitialised() const { return alcDevice && alcContext; }
  bool OalIsNotInitialised() const { return !OalIsInitialised(); }
  /* -- DeInitialise context ----------------------------------------------- */
  void DeInitContext()
  { // Release and destroy context and if successful?
    if(OalDestroyContext())
    { // Clear the flags associated with context
      FlagClear(AFL_CONTEXTCURRENT | AFL_INITCONTEXT);
      ssvPlayback = cCommon->CommonNull();
      alcContext = nullptr;
    }
  }
  /* -- Initialise with specified device ----------------------------------- */
  void OalInit(const char*const cpDevice)
  { // Class initialised
    IHInitialise();
    // Log that we're loading the device
    cLog->LogDebugExSafe("Oal class initialising device '$'...",
      cpDevice ? cpDevice : "<Default>");
    // Open the specified device and if successful?
    if(ALCdevice*const alcNDevice = alcOpenDevice(cpDevice)) [[likely]]
    { // Unload previous device if it is set (impossible)
      if(alcDevice) [[unlikely]] DeInitContext();
      // Device is now initialise
      else FlagSet(AFL_INITDEVICE);
      // Set the new active device
      alcDevice = alcNDevice;
    } // Failed so retry with default device?
    else if(cpDevice != nullptr)
    { // Log that this failed
      cLog->LogDebugExSafe("Oal failed to initialise playback device '$', ",
        "retrying with default device...");
      // Re-initialise with default device
      return OalInit(nullptr);
    } // Hard fail with default device
    else if(!alcDevice) XC("Failed to initialise default OpenAL device!");
    // Update playback device name
    ssvPlayback = OalGetContextString(FlagIsSet(AFL_HAVEENUMEXT) ?
      ALC_ALL_DEVICES_SPECIFIER : ALC_DEVICE_SPECIFIER);
    // Log context initialisation
    cLog->LogInfoExSafe("Oal context initialised on device '$'.",
      OalGetPlaybackDevice());
    // Log that we're changing HRTF setting
    cLog->LogDebugExSafe(
      "Oal changing head related transfer function status to $...",
      StrFromBoolTF(cOal->FlagIsSet(AFL_HRTFREQ)));
    // Set HRTF setting and if succeeded?
    const StdArray<const ALCint, 3> alciAttrs{
      ALC_HRTF_SOFT, FlagIsSet(AFL_HRTFREQ) ? AL_TRUE : AL_FALSE, 0 };
    if(alcResetDeviceSOFT(alcDevice, alciAttrs.data()) == AL_TRUE)
    { // Log that we change the HRTF setting and set the flag
      cLog->LogDebugExSafe(
        "Oal set head related transfer function status to $.",
        StrFromBoolTF(cOal->FlagIsSet(AFL_HRTFREQ)));
      FlagSet(AFL_HRTFINIT);
    } // Setting HRTF did not work?
    else
    { // Log that we could not change the HRTF setting and clear the flag
      cLog->LogWarningExSafe(
        "Oal failed to set head related transfer function status to $.",
        StrFromBoolTF(cOal->FlagIsSet(AFL_HRTFREQ)));
      FlagClear(AFL_HRTFINIT);
    } // Set that we're intilaising the context now
    cLog->LogDebugExSafe("Oal creating context from device...",
      StrFromBoolTF(cOal->FlagIsSet(AFL_HRTFREQ)));
    // Set the context for the specified device and if successful?
    if(ALCcontext*const alcNContext =
      alcCreateContext(alcDevice, nullptr)) [[likely]]
    { // Unload previous context if it is set (impossible)
      if(alcContext) [[unlikely]] DeInitContext();
      // Context is now initialise
      else FlagSet(AFL_INITCONTEXT);
      // Set the new active context
      alcContext = alcNContext;
    } // We could not create the context
    else XC("Failed to initialise OpenAL context!");
    // Get maximum number of sources (dynamic on Apple implementation).
    stMaxMonoSources =
      OalGetContextInt<decltype(stMaxMonoSources)>(ALC_MONO_SOURCES);
    stMaxStereoSources =
      OalGetContextInt<decltype(stMaxStereoSources)>(ALC_STEREO_SOURCES);
    // Zero mono sources?
    if(!stMaxMonoSources)
    { // Zero stereo sources?
      if(!stMaxStereoSources)
      { // Set infinite sources flag
        FlagSet(AFL_INFINITESOURCES);
        // Set arbitrary amounts
        stMaxMonoSources = 255;
        stMaxStereoSources = 1;
      } // Failed because no stereo sources
      else XC("No mono source support on this device!",
        "Device", ssvPlayback);
    } // Zero stereo sources? Failed because no mono sources
    else if(!stMaxStereoSources)
      XC("No stereo source support on this device!", "Device", ssvPlayback);
    // Check playback system event capabilities
    struct EventCapItem { const ALenum aleEventType, aleDeviceType;
                          const OalFlagsConst &ofcFlag; };
    for(const EventCapItem &eciItem : StdArray<EventCapItem, 6>{{
      { ALC_EVENT_TYPE_DEFAULT_DEVICE_CHANGED_SOFT, ALC_PLAYBACK_DEVICE_SOFT,
        AFL_HAVESEPBDDC },
      { ALC_EVENT_TYPE_DEVICE_ADDED_SOFT,           ALC_PLAYBACK_DEVICE_SOFT,
        AFL_HAVESEPBDA },
      { ALC_EVENT_TYPE_DEVICE_REMOVED_SOFT,         ALC_PLAYBACK_DEVICE_SOFT,
        AFL_HAVESEPBDR },
      { ALC_EVENT_TYPE_DEFAULT_DEVICE_CHANGED_SOFT, ALC_CAPTURE_DEVICE_SOFT,
        AFL_HAVESECADDC },
      { ALC_EVENT_TYPE_DEVICE_ADDED_SOFT,           ALC_CAPTURE_DEVICE_SOFT,
        AFL_HAVESECADA },
      { ALC_EVENT_TYPE_DEVICE_REMOVED_SOFT,         ALC_CAPTURE_DEVICE_SOFT,
        AFL_HAVESECADR }
    }})
    { // Do the test and get result
      switch(const ALenum aleResult =
        OalIsEventSupported(eciItem.aleEventType, eciItem.aleDeviceType))
      { // Event is supported?
        case ALC_EVENT_SUPPORTED_SOFT:
          // Enable system event
          OalSetEventState(eciItem.aleEventType, AL_TRUE);
          // Set capability
          FlagSet(eciItem.ofcFlag);
          // Done
          break;
        // Event is not supported?
        case ALC_EVENT_NOT_SUPPORTED_SOFT:
          // Disable system event
          OalSetEventState(eciItem.aleEventType, AL_FALSE);
          // Clear capability
          FlagClear(eciItem.ofcFlag);
          // Done
          break;
        // Invalid result
        default: XC("Internal error: Invalid AL event query result!",
          "Flag",       eciItem.ofcFlag.FlagGet(),
          "EventType",  eciItem.aleEventType,
          "DeviceType", eciItem.aleDeviceType,
          "Result",     aleResult);
      }
    } // Extension logging types
    using ExtPair = StdPair<const StdStringView, const size_t>;
    using ExtMap = StdMap<ExtPair::first_type, ExtPair::second_type>;
    // Return if debug logging not enabled
    if(cLog->LogHasLevel(LH_DEBUG))
    { // Build sorted list of extensions and log them all
      ExtMap emExts;
      // Build context extensions list
      size_t stCount = 0;
      Tokeniser<StdStringView>(OalGetContextString(ALC_EXTENSIONS),
        cCommon->CommonSpace(),
        [&emExts, &stCount](const StdStringView &ssvExt)
          { emExts.insert({ StdMove(ssvExt), stCount++ }); });
      // Log context initialisation
      cLog->LogNLCDebugExSafe(
        "Oal context capabilities...\n"
        "- Maximum mono sources: $.\n"
        "- Maximum stereo sources: $.\n"
        "- Playback events: !$ +$ -$.\n"
        "- Capture events: !$ +$ -$.\n"
        "- Have ext.device enumerator: $.\n"
        "- Head related transfer function: $.\n"
        "- Context extensions count: $...",
        stMaxMonoSources, stMaxStereoSources,
        StrFromBoolTF(FlagIsSet(AFL_HAVESEPBDDC)),
        StrFromBoolTF(FlagIsSet(AFL_HAVESEPBDA)),
        StrFromBoolTF(FlagIsSet(AFL_HAVESEPBDR)),
        StrFromBoolTF(FlagIsSet(AFL_HAVESECADDC)),
        StrFromBoolTF(FlagIsSet(AFL_HAVESECADA)),
        StrFromBoolTF(FlagIsSet(AFL_HAVESECADR)),
        StrFromBoolTF(FlagIsSet(AFL_HAVEENUMEXT)),
        StrFromBoolTF(FlagIsSet(AFL_HRTFREQ)),
        emExts.size());
      // Log extensions if debug is enabled
      for(const ExtPair &epExt : emExts)
        cLog->LogNLCDebugExSafe("- Have extension '$' (#$).",
          epExt.first, epExt.second);
    } // Activate the context (We can use alGetError() from now on)
    cLog->LogDebugSafe("Oal acquiring playback context...");
    IALC(OalMakeContextCurrent(alcContext),
      "Failed to make OpenAL context current!");
    FlagSet(AFL_CONTEXTCURRENT);
    cLog->LogDebugSafe("Oal acquired playback context.");
    // Set if we have 32-bit floating-point playback
    FlagSetOrClear(AFL_HAVE32FPPB, alIsExtensionPresent("AL_EXT_FLOAT32"));
    // This is a thread safe version of this bool since the audio thread will
    // need access to this processing Stream objects.
    bHave32FPPB = FlagIsSet(AFL_HAVE32FPPB);
    // Return if debug logging not enabled
    if(cLog->LogHasLevel(LH_DEBUG))
    { // Build sorted list of extensions and log them all
      ExtMap emExts;
      // Build extensions list
      size_t stCount = 0;
      Tokeniser<StdStringView>(OalGetString(AL_EXTENSIONS),
        cCommon->CommonSpace(),
        [&emExts, &stCount](const StdStringView &ssvExt)
          { emExts.insert({ StdMove(ssvExt), stCount++ }); });
      // Log device info and basic capabilities
      cLog->LogNLCDebugExSafe(
        "Oal device capabilities...\n"
        "- Floating-point playback: $.\n"
        "- Device extensions count: $...",
        StrFromBoolTF(OalHave32FPPB()),
        emExts.size());
      // Log extensions if debug is enabled
      for(const ExtPair &epExt : emExts)
        cLog->LogNLCDebugExSafe("- Have extension '$' (#$).",
          epExt.first, epExt.second);
    } // Set the flag
    FlagSet(AFL_INITIALISED);
    // Log that we've completed initialisation
    cLog->LogDebugSafe("Oal class initialised.");
  }
  /* -- DeInitialise ------------------------------------------------------- */
  void OalDeInit()
  { // Return if not intiialised
    if(IHNotDeInitialise()) return;
    // Log that we're deinitialising
    cLog->LogDebugSafe("Oal class deinitialising...");
    // De-init context and device if they're not gone already. The audio
    // class should be responsible for this, but just incase.
    DeInitContext();
    // Destroy the device and if succeeded?
    if(OalDestroyDevice())
    { // Close device and nullify handle
      FlagClear(AFL_INITDEVICE);
      alcDevice = nullptr;
    } // Clear everything else
    aleQuery = AL_NONE;
    stMaxStereoSources = stMaxMonoSources = 0;
    // Clear only volatile flags
    FlagClear(AFL_INITDEVICE | AFL_INITCONTEXT | AFL_INFINITESOURCES |
              AFL_CONTEXTCURRENT | AFL_INITIALISED | AFL_HAVE32FPPB |
              AFL_HAVEENUMEXT);
    // Finished so log that we did
    cLog->LogDebugSafe("Oal class deinitialised.");
  }
  /* -- Constructor --------------------------------------------- */ protected:
  Oal() :
    /* -- Initialisers ----------------------------------------------------- */
    GCHelper{ this, &cOal },           // Initialise global pointer
    InitHelper{ __FUNCTION__ },        // Send name to InitHelper
    OalFlags{ AFL_NONE },              // Flags not initialised yet
    /* -- Const members ---------------------------------------------------- */
    alemOALCodes{{                     // Init OpenAL error codes
      IDMAPSTR(AL_NO_ERROR),           IDMAPSTR(AL_INVALID_NAME),
      IDMAPSTR(AL_INVALID_ENUM),       IDMAPSTR(AL_INVALID_VALUE),
      IDMAPSTR(AL_INVALID_OPERATION),  IDMAPSTR(AL_OUT_OF_MEMORY)
    }, "AL_UNKNOWN" },
    alemOALCCodes{{                    // Init OpenAL context error codes
      IDMAPSTR(ALC_NO_ERROR),          IDMAPSTR(ALC_INVALID_DEVICE),
      IDMAPSTR(ALC_INVALID_CONTEXT),   IDMAPSTR(ALC_INVALID_ENUM),
      IDMAPSTR(ALC_INVALID_VALUE),     IDMAPSTR(ALC_OUT_OF_MEMORY),
    }, "ALC_UNKNOWN" },
    alemFormatCodes{{                  // Init OpenAL format codes
      { AL_FORMAT_STEREO_FLOAT32, "SF32" }, { AL_FORMAT_MONO_FLOAT32, "MF32" },
      { AL_FORMAT_MONO16,         "MI16" }, { AL_FORMAT_STEREO16,     "SI16" },
      { AL_FORMAT_MONO8,          "MI08" }, { AL_FORMAT_STEREO8,      "SI08" }
    }, "????" },
    /* -- Initialisers ----------------------------------------------------- */
    stMaxStereoSources(0),             // Stereo sources initialised later
    stMaxMonoSources(0),               // Mono sources initialised later
    ssvPlayback{                       // Blank playback device
      cCommon->CommonNull() },         // Initialise with "null" text
    alcDevice(nullptr),                // Device not initialised yet
    alcContext(nullptr),               // Context not initialised yet
    bHave32FPPB(false),                // 32bit float cap not initialised yet
    aleQuery(AL_NONE)                  // Query method not initialised yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor that unloads context and device ------------------------- */
  DTORHELPER(~Oal, OalDeInitNoReset())
  /* -- Update hints ----------------------------------------------- */ public:
  CVarReturn OalSetHRTF(const bool bState)
    { FlagSetOrClear(AFL_HRTFREQ, bState); return ACCEPT; }
  /* -- Update hints ----------------------------------------------- */ public:
  CVarReturn OalSetVersion(const double dVersion)
  { // Prepare version information. Partially initialises OpenALSoft too.
    const ALuint
      aluVersionMajor =
        OalGetContextInt<decltype(aluVersionMajor)>(ALC_MAJOR_VERSION),
      aluVersionMinor =
        OalGetContextInt<decltype(aluVersionMinor)>(ALC_MINOR_VERSION);
    strVersion = StrAppend(aluVersionMajor, '.', aluVersionMinor);
    // Split version
    double dIntegral, dFractional = modf(dVersion, &dIntegral);
    const ALuint aluMajor = static_cast<ALuint>(dIntegral),
                 aluMinor = static_cast<ALuint>(dFractional * 10.0);
    // Need at least the specified version of OpenAL
    if(aluVersionMajor != aluMajor || aluVersionMinor != aluMinor)
      XC("Mismatch OpenAL version!",
        "MajorRequested", aluMajor,        "MinorRequested", aluMinor,
        "MajorActual",    aluVersionMajor, "MinorActual",    aluVersionMinor);
    // Log context initialisation
    cLog->LogDebugExSafe("Oal library version $ initialised.",
      OalGetVersion(), StdIOSHex, FlagGet());
    // Done
    return ACCEPT;
  }
  /* -- Undefines ---------------------------------------------------------- */
#undef IAL
#undef IALL
#undef IALLNF
#undef IALC
#undef IALCL
#undef IALCLNF
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
