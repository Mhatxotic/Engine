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
using namespace ILog::P;               using namespace ILookupMap::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IToken::P;             using namespace IUtf::P;
using namespace Lib::OpenAL;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- GL error checking wrapper macros ------------------------------------- */
#define ALEX(EF,F,M,...)  do{ F; EF(M, ## __VA_ARGS__); }while(false) // :(
#define ALL(F,M,...)      ALEX(cOal->LogALError, F, M, ## __VA_ARGS__)
#define AL(F,M,...)       ALEX(cOal->CheckALError, F, M, ## __VA_ARGS__)
#define ALNF(M,...)       AL(static_cast<void>(0), M, ## __VA_ARGS__)
/* -- GL context error checking wrapper macros ----------------------------- */
#define ALCL(F,M,...)     ALEX(cOal->LogALCError, F, M, ## __VA_ARGS__)
#define ALCLNF(M,...)     ALCL(static_cast<void>(0), M, ## __VA_ARGS__)
#define ALC(F,M,...)      ALEX(cOal->CheckALCError, F, M, ## __VA_ARGS__)
#define ALCNF(M,...)      ALC(static_cast<void>(0), M, ## __VA_ARGS__)
/* -- Typedefs ------------------------------------------------------------- */
using ALUIntVector = StdVector<ALuint>; // A vector of ALuint's
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(Oal,                       // OpenAL flags
  /* -- OAL specific flags ------------------------------------------------- */
  AFL_NONE                  {Flag(0)}, // No flags
  AFL_INITDEVICE            {Flag(1)}, // Device has been initialised?
  AFL_INITCONTEXT           {Flag(2)}, // Context has been initialised?
  AFL_INFINITESOURCES       {Flag(3)}, // Have infinite sources?
  AFL_CONTEXTCURRENT        {Flag(4)}, // Context has been made current?
  AFL_INITIALISED           {Flag(5)}, // OpenAL fully initialised
  AFL_HAVE32FPPB            {Flag(6)}, // Can play 32-bit float audio?
  AFL_HAVEENUMEXT           {Flag(7)}, // Have ALC_ENUMERATE_ALL_EXT?
  AFL_HAVESEPBDDC           {Flag(8)}, // Playback change def device event?
  AFL_HAVESECADDC           {Flag(9)}, // Capture change default device event?
  AFL_HAVESEPBDA           {Flag(10)}, // Playback add device event?
  AFL_HAVESECADA           {Flag(11)}, // Capture add device event?
  AFL_HAVESEPBDR           {Flag(12)}, // Playback remove device event?
  AFL_HAVESECADR           {Flag(13)}, // Capture remove device event?
  AFL_REINIT               {Flag(14)}, // Audio system is resetting?
  AFL_HRTFREQ              {Flag(15)}, // HRTF is requested enabled?
  AFL_HRTFINIT             {Flag(16)}, // HRTF init was successful?
  /* -- Masks -------------------------------------------------------------- */
  AFL_VOLATILE{ AFL_INITDEVICE|AFL_INITCONTEXT|AFL_INFINITESOURCES|
                AFL_CONTEXTCURRENT|AFL_INITIALISED|AFL_HAVE32FPPB|
                AFL_HAVEENUMEXT }
);/* == Oal class ========================================================== */
class Oal;                             // Class prototype
static Oal *cOal = nullptr;            // Pointer to global class
class Oal :                            // Actual class body
  /* -- Base classes ------------------------------------------------------- */
  public OalFlags                      // OpenAL flags
{ /* -- Error macros for use inside the class only ------------------------- */
#define IAL(F,M,...)  ALEX(CheckALError, F, M, ## __VA_ARGS__)
#define IALNF(M,...)  IAL(static_cast<void>(0), M, ## __VA_ARGS__)
#define IALC(F,M,...) ALEX(CheckALCError, F, M, ## __VA_ARGS__)
#define IALCNF(M,...) IALC(static_cast<void>(0), M, ## __VA_ARGS__)
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
  StdStringView    strvPlayback;       // String playback device
  /* ----------------------------------------------------------------------- */
  ALCdevice       *alcDevice;          // OpenAL device
  ALCcontext      *alcContext;         // OpenAL context
  /* ----------------------------------------------------------------------- */
  bool             bHave32FPPB;        // Cached version of 32-bit float cap
  /* -- Public Variables --------------------------------------------------- */
  ALenum           aleQuery;           // Device query devices command
  /* -- AL generic context error logger ------------------------------------ */
  template<ALenum aleNoErr, typename FuncTypeErr, typename FuncTypeStr,
    typename StrType, typename ...VarArgs>
  void LogErrorGeneric(FuncTypeErr fteFunc, FuncTypeStr ftsFunc,
    const char*const cpPrefix, StrType &&strFormat, VarArgs &&...vaArgs) const
  { // Enumerate all queued OpenAL errors and print them
    for(ALenum aleError = fteFunc();
               aleError != aleNoErr;
               aleError = fteFunc())
      cLog->LogWarningExSafe("AL$ call failed: $ ($/0x$$).", cpPrefix,
        StrFormat(StdForward<StrType>(strFormat),
        StdForward<VarArgs>(vaArgs)...), ftsFunc(aleError),
        StdIOSHex, aleError);
  }
  /* -- AL generic exception handler --------------------------------------- */
  template<ALenum aleNoErr, typename FuncTypeErr, typename FuncTypeStr,
    typename ...VarArgs>
  void CheckErrorGeneric(FuncTypeErr fteFunc, FuncTypeStr ftsFunc,
    const char*const cpType, const char*const cpReason, VarArgs &&...vaArgs)
      const
  { // Get error and return if error is good
    const ALenum aleError = fteFunc();
    if(aleError == aleNoErr) [[likely]] return;
    // Build an exception object to show why the call failed
    XC(StrFormat("AL$ call failed: $", cpType, cpReason),
      StdForward<VarArgs>(vaArgs)...,
      "Code", aleError, "Reason", ftsFunc(aleError));
  }
  /* -- Destroy the set context and device --------------------------------- */
  void DestroyContext(void) { alcMakeContextCurrent(nullptr);
                              alcDestroyContext(alcContext); }
  void DestroyDevice(void) { alcCloseDevice(alcDevice); }
  /* -- Return error status ---------------------------------------- */ public:
  ALenum GetContextError() const { return alcGetError(alcDevice); }
  bool HaveContextError() const { return GetContextError() != ALC_NO_ERROR; }
  bool HaveNoContextError() const { return !HaveContextError(); }
  static ALenum GetError() { return alGetError(); }
  static bool HaveError() { return GetError() != AL_NO_ERROR; }
  static bool HaveNoError() { return !HaveError(); }
  /* -- Get query method --------------------------------------------------- */
  ALenum GetQueryMethod() const { return aleQuery; }
  /* -- Return version information ----------------------------------------- */
  const StdString &GetVersion() const { return strVersion; }
  /* -- AL context error logger -------------------------------------------- */
  template<typename StrType, typename ...VarArgs>
    void LogALCError(StrType &&strFormat, VarArgs &&...vaArgs) const
  { LogErrorGeneric<ALC_NO_ERROR>(
      [this]()->ALenum{ return GetContextError(); },
      [this](const ALenum aleError){ return GetALCErr(aleError); },
      " context", StdForward<StrType>(strFormat),
      StdForward<VarArgs>(vaArgs)...); }
  /* -- AL error logger ---------------------------------------------------- */
  template<typename StrType, typename ...VarArgs>
    void LogALError(StrType &&strFormat, VarArgs &&...vaArgs) const
  { LogErrorGeneric<AL_NO_ERROR>(
      []()->ALenum{ return GetError(); },
      [this](const ALenum aleError){ return GetALErr(aleError); },
      cCommon->CommonCBlank(), StdForward<StrType>(strFormat),
      StdForward<VarArgs>(vaArgs)...); }
  /* -- AL context error handler ------------------------------------------- */
  template<typename ...VarArgs>
    void CheckALCError(const char*const cpReason, VarArgs &&...vaArgs) const
  { CheckErrorGeneric<ALC_NO_ERROR>(
      [this]()->ALenum{ return GetContextError(); },
      [this](const ALenum aleError) { return GetALCErr(aleError); },
      " context", cpReason, StdForward<VarArgs>(vaArgs)...); }
  /* -- AL error handler --------------------------------------------------- */
  template<typename ...VarArgs>
    void CheckALError(const char*const cpReason, VarArgs &&...vaArgs) const
  { CheckErrorGeneric<AL_NO_ERROR>(
      []()->ALenum{ return GetError(); },
      [this](const ALenum aleError) { return GetALErr(aleError); },
      cCommon->CommonCBlank(), cpReason, StdForward<VarArgs>(vaArgs)...); }
  /* -- Upload data to audio device ---------------------------------------- */
  static void BufferData(const ALuint aluBuffer, const ALenum aleFormat,
    const ALvoid*const vpData, const ALsizei siSize,
    const ALsizei alsiFrequency)
  { alBufferData(aluBuffer, aleFormat, vpData, siSize, alsiFrequency); }
  /* -- Upload data to audio device ---------------------------------------- */
  static void BufferData(const ALuint aluBuffer, const ALenum aleFormat,
    const MemConst &mcSrc, const ALsizei alsiFrequency)
  { BufferData(aluBuffer, aleFormat, mcSrc.MemPtr<ALvoid>(),
      mcSrc.MemSize<ALsizei>(), alsiFrequency); }
  /* -- Queue specified buffer count into source --------------------------- */
  static void QueueBuffers(const ALuint aluSource, const ALsizei alsiCount,
    ALuint*const alupBuffer)
  { alSourceQueueBuffers(aluSource, alsiCount, alupBuffer); }
  /* -- Queue one buffer count into source --------------------------------- */
  static void QueueBuffer(const ALuint aluSource, ALuint aluBuffer)
    { QueueBuffers(aluSource, 1, &aluBuffer); }
  /* -- Unqueue specified buffer count from source and place into buffers -- */
  static void UnQueueBuffers(const ALuint aluSource,
    const ALsizei alsiCount, ALuint*const alupBuffer)
  { alSourceUnqueueBuffers(aluSource, alsiCount, alupBuffer); }
  /* -- Unqueue one buffer from source and place into buffers -------------- */
  static void UnQueueBuffer(const ALuint aluSource, ALuint &aluBuffer)
    { UnQueueBuffers(aluSource, 1, &aluBuffer); }
  /* -- Unqueue one buffer from source and return it ----------------------- */
  static ALuint UnQueueBuffer(const ALuint aluSource)
    { ALuint aluBuffer;
      UnQueueBuffer(aluSource, aluBuffer);
      return aluBuffer; }
  /* -- Set source value as float ------------------------------------------ */
  static void SetSourceFloat(const ALuint aluSource, const ALenum aleWhat,
    const ALfloat fValue)
  { alSourcef(aluSource, aleWhat, fValue); }
  /* -- Get source value as float ------------------------------------------ */
  static void GetSourceFloat(const ALuint aluSource, const ALenum aleWhat,
    ALfloat*const fpDestValue)
  { alGetSourcef(aluSource, aleWhat, fpDestValue); }
  /* -- Set source value as int -------------------------------------------- */
  static void SetSourceInt(const ALuint aluSource, const ALenum aleWhat,
    const ALint iValue)
  { alSourcei(aluSource, aleWhat, iValue); }
  /* -- Get source value as int -------------------------------------------- */
  static void GetSourceInt(const ALuint aluSource, const ALenum aleWhat,
    ALint*const alipDestValue)
  { alGetSourcei(aluSource, aleWhat, alipDestValue); }
  /* -- Get source value as a float vector --------------------------------- */
  static void GetSourceVector(const ALuint aluSource, const ALenum aleWhat,
    ALfloat*const alfpDX, ALfloat*const alfpDY, ALfloat*const alfpDZ)
  { alGetSource3f(aluSource, aleWhat, alfpDX, alfpDY, alfpDZ); }
  /* -- Set source value as a float vector --------------------------------- */
  static void SetSourceVector(const ALuint aluSource, const ALenum aleWhat,
    const ALfloat alfDX, const ALfloat alfDY, const ALfloat alfDZ)
      { alSource3f(aluSource, aleWhat, alfDX, alfDY, alfDZ); }
  /* -- Stop a source from playing ----------------------------------------- */
  static void StopSource(const ALuint aluSource) { alSourceStop(aluSource); }
  /* -- Play a source ------------------------------------------------------ */
  static void PlaySource(const ALuint aluSource) { alSourcePlay(aluSource); }
  /* -- Rewind a source ---------------------------------------------------- */
  static void RewindSource(const ALuint aluSource) {alSourceRewind(aluSource);}
  /* -- Pause a source ---------------------------------------------------- */
  static void PauseSource(const ALuint aluSource) { alSourcePause(aluSource); }
  /* -- Play more than one source simultaniously --------------------------- */
  template<class ListType>
    requires StdHasDataSize<ListType>
  static void PlaySources(const ListType &ltSources)
    { alSourcePlayv(static_cast<ALsizei>(ltSources.size()),
        ltSources.data()); }
  /* -- Create multiple sources -------------------------------------------- */
  static void CreateSources(const ALsizei alsiCount, ALuint*const alupSources)
    { alGenSources(alsiCount, alupSources); }
  /* -- Create one source and place it in the specified buffer ------------- */
  static void CreateSource(ALuint &aluSourceRef)
    { CreateSources(1, &aluSourceRef); }
  /* -- Create and return a source ----------------------------------------- */
  static ALuint CreateSource()
    { ALuint aluSource; CreateSource(aluSource); return aluSource; }
  /* -- Delete multiple sources -------------------------------------------- */
  static void DeleteSources(const ALsizei alsiCount,
    const ALuint*const alupSources)
  { alDeleteSources(alsiCount, alupSources); }
  /* -- Delete multiple sources -------------------------------------------- */
  template<class ListType>
    requires StdHasDataSize<ListType>
  static void DeleteSources(const ListType &ltSources)
    { DeleteSources(static_cast<ALsizei>(ltSources.size()),
        ltSources.data()); }
  /* -- Delete one source -------------------------------------------------- */
  static void DeleteSource(const ALuint &aluSourceRef)
    { DeleteSources(1, &aluSourceRef); }
  /* -- Create multiple buffers -------------------------------------------- */
  static void CreateBuffers(const ALsizei alsiCount, ALuint*const alupBuffer)
    { alGenBuffers(alsiCount, alupBuffer); }
  /* -- Create multiple buffers -------------------------------------------- */
  template<class ListType>
    requires StdHasDataSize<ListType>
  static void CreateBuffers(ListType &ltBuffers)
  { CreateBuffers(static_cast<ALsizei>(ltBuffers.size()), ltBuffers.data()); }
  /* -- Create one buffer and place it in the specified variable ----------- */
  static void CreateBuffer(ALuint &aluBuffer) { CreateBuffers(1, &aluBuffer); }
  /* -- Create and return a buffer ----------------------------------------- */
  static ALuint CreateBuffer()
    { ALuint aluBuffer; CreateBuffer(aluBuffer); return aluBuffer; }
  /* -- Delete multiple buffers -------------------------------------------- */
  static void DeleteBuffers(const ALsizei alsiCount,
    const ALuint*const alupBuffer)
  { alDeleteBuffers(alsiCount, alupBuffer); }
  /* -- Delete multiple sources -------------------------------------------- */
  template<class ListType>
    requires StdHasDataSize<ListType>
  static void DeleteBuffers(const ListType &ltBuffers)
    { DeleteBuffers(static_cast<ALsizei>(ltBuffers.size()),
        ltBuffers.data()); }
  /* -- Delete one buffer -------------------------------------------------- */
  static void DeleteBuffer(const ALuint &aluBufferRef)
    { DeleteBuffers(1, &aluBufferRef); }
  /* -- Checks if a buffer id is valid ------------------------------------- */
  static bool IsBuffer(const ALuint aluBuffer)
    { return alIsBuffer(aluBuffer) == AL_TRUE; }
  /* -- Get buffer parameter as integer ------------------------------------ */
  static void GetBufferInt(const ALuint aluBuffer, const ALenum aleId,
    ALint*const alipDest)
  { alGetBufferi(aluBuffer, aleId, alipDest); }
  /* -- Get buffer information --------------------------------------------- */
  template<typename IntType = ALint>
    requires StdIsIntegral<IntType>
  static IntType GetBufferInt(const ALuint aluBuffer, const ALenum aleId)
  { // Create storage, populate and retun it
    ALint aliV;
    GetBufferInt(aluBuffer, aleId, &aliV);
    return static_cast<IntType>(aliV);
  }
  /* -- Set distance model ------------------------------------------------- */
  static void SetDistanceModel(const ALenum aleModel)
    { alDistanceModel(aleModel); }
  /* -- Set listener vector ------------------------------------------------ */
  static void SetListenerVector(const ALenum aleParam, const ALfloat fX,
    const ALfloat fY, const ALfloat fZ)
      { alListener3f(aleParam, fX, fY, fZ); }
  /* -- Set listener position ---------------------------------------------- */
  static void SetListenerPosition(const ALfloat fX, const ALfloat fY,
    const ALfloat fZ)
      { SetListenerVector(AL_POSITION, fX, fY, fZ); }
  /* -- Set listener velocity ---------------------------------------------- */
  static void SetListenerVelocity(const ALfloat fX, const ALfloat fY,
    const ALfloat fZ)
      { SetListenerVector(AL_VELOCITY, fX, fY, fZ); }
  /* -- Set listener velocity ---------------------------------------------- */
  static void SetListenerVectors(const ALenum aleParam,
    const ALfloat*const fpVectors)
  { alListenerfv(aleParam, fpVectors); }
  /* -- Set listener orientation ------------------------------------------- */
  static void SetListenerOrientation(const ALfloat*const fpVectors)
    { SetListenerVectors(AL_ORIENTATION, fpVectors); }
  /* -- Is extension present ----------------------------------------------- */
  static bool HaveExtension(const char*const cpEnum)
    { return alIsExtensionPresent(cpEnum) != AL_FALSE; }
  /* ----------------------------------------------------------------------- */
  static bool HaveCExtension(const char*const cpEnum, ALCdevice*const alcDev)
    { return alcIsExtensionPresent(alcDev, cpEnum) != AL_FALSE; }
  /* ----------------------------------------------------------------------- */
  bool HaveCExtension(const char*const cpEnum) const
    { return HaveCExtension(cpEnum, alcDevice); }
  /* ----------------------------------------------------------------------- */
  static bool HaveNCExtension(const char*const cpEnum)
    { return HaveCExtension(cpEnum, nullptr); }
  /* -- Detect enumeration method ------------------------------------------ */
  void DetectEnumerationMethod()
  { // Get if we have ALC_ENUMERATE_ALL_EXT
    if(HaveNCExtension("ALC_ENUMERATE_ALL_EXT"))
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
  /* -- Report floating point playback to other classes -------------------- */
  bool Have32FPPB() const { return bHave32FPPB; } // Accessed in audio thread!!
  /* -- Get openAL string -------------------------------------------------- */
  template<typename IntType = ALchar>
    requires StdIsIntegral<IntType>
  const IntType *GetString(const ALenum aleId) const
  { // Get the checked variable and return it
    const ALchar*const ucpStr = alGetString(aleId);
    IALNF("Get string failed!", "Index", aleId);
    if(!UtfIsCStringValid(ucpStr))
      XC("Invalid string returned!", "Index", aleId, "String", ucpStr);
    return reinterpret_cast<const IntType*>(ucpStr);
  }
  /* -- Get context openAL string ------------------------------------------ */
  template<typename IntType = const ALCchar*>
    requires StdIsPointer<IntType>
  static IntType ContextGetString(ALCdevice*const alcDevice,
    const ALCenum alcEnum)
  { return reinterpret_cast<IntType>(alcGetString(alcDevice, alcEnum)); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType = const ALCchar*>
    requires StdIsPointer<IntType>
  static IntType GetCString(ALCdevice*const alcDev, const ALenum aleId)
    { return reinterpret_cast<IntType>(ContextGetString(alcDev, aleId)); }
  /* -- Get context openAL string ------------------------------------------ */
  template<typename StrType = const ALCchar*>
    requires StdIsPointer<StrType>
  StrType GetCString(const ALenum aleId) const
    { return GetCString<StrType>(alcDevice, aleId); }
  /* -- Get nullptr context openAL string ---------------------------------- */
  template<typename StrType = const ALCchar*>
    requires StdIsPointer<StrType>
  static StrType GetNCString(const ALenum aleId)
    { return reinterpret_cast<StrType>(ContextGetString(nullptr, aleId)); }
  /* -- Get openAL int array ----------------------------------------------- */
  template<size_t stCount, class IntType = StdArray<ALCint, stCount>>
    const IntType GetIntegerArray(const ALenum aleId) const
  { // Create array, check, populate and return it
    IntType aData;
    IALC(alcGetIntegerv(alcDevice, aleId, sizeof(IntType), aData.data()),
      "Get integer array failed!", "Index", aleId, "Count", stCount);
    return aData;
  }
  /* -- Get openAL int ----------------------------------------------------- */
  template<typename IntType = ALCint>
    requires StdIsIntegral<IntType>
  IntType GetInteger(const ALenum aleId) const
    { return static_cast<IntType>(GetIntegerArray<1>(aleId)[0]); }
  /* -- Convert PCM format identifier to short identifier string ----------- */
  const StdStringView &GetALFormat(const ALenum aleFormat) const
    { return alemFormatCodes.Get(aleFormat); }
  /* -- Get source counts -------------------------------------------------- */
  size_t GetMaxMonoSources() const { return stMaxMonoSources; }
  size_t GetMaxStereoSources() const { return stMaxStereoSources; }
  /* -- Get current playback device ---------------------------------------- */
  const StdStringView &GetPlaybackDevice() const { return strvPlayback; }
  /* -- Set system event callback ------------------------------------------ */
  void SetEventCallback(const ALCEVENTPROCTYPESOFT cbProc, void*const vpParam)
  { // Keeping Ubuntu 24.04 compatibility for now until supported
#if !defined(LINUX)
    return alcEventCallbackSOFT(cbProc, vpParam);
#endif
  }
  /* -- Enable or disable system events ------------------------------------ */
  ALenum SetEventState(const ALCenum alceEvent, const ALCboolean alcbEnabled)
  { // Keeping Ubuntu 24.04 compatibility for now until supported
#if defined(LINUX)
    return AL_FALSE;
#else
    return alcEventControlSOFT(1, &alceEvent, alcbEnabled);
#endif
  }
  /* -- Is system event supported ------------------------------------------ */
  ALenum IsEventSupported(const ALenum aleEventType,
    const ALenum aleDeviceType)
  { // Keeping Ubuntu 24.04 compatibility for now until supported
#if defined(LINUX)
    return ALC_EVENT_NOT_SUPPORTED_SOFT;
#else
    return alcEventIsSupportedSOFT(aleEventType, aleDeviceType);
#endif
  }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType>
  const StdStringView &GetALErr(const IntType tCode) const
    { return alemOALCodes.Get(static_cast<ALenum>(tCode)); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType>
  const StdStringView &GetALCErr(const IntType tCode) const
    { return alemOALCCodes.Get(static_cast<ALenum>(tCode)); }
  /* -- AL is initialised? ------------------------------------------------- */
  bool IsInitialised() const { return alcDevice && alcContext; }
  bool IsNotInitialised() const { return !IsInitialised(); }
  /* -- Update device ------------------------------------------------------ */
  void UpdateDevice(ALCdevice*const alcNDevice) { alcDevice = alcNDevice; }
  /* -- Update playback device name ---------------------------------------- */
  void UpdatePlaybackDeviceName()
    { strvPlayback = GetCString(FlagIsSet(AFL_HAVEENUMEXT) ?
        ALC_ALL_DEVICES_SPECIFIER : ALC_DEVICE_SPECIFIER); }
  /* -- Initialise HRTF override ------------------------------------------- */
  void InitHRTFOverride()
  { // Set HRTF setting and return if failed
    const StdArray<const ALCint,3> alciAttrs{
      ALC_HRTF_SOFT, FlagIsSet(AFL_HRTFREQ) ? AL_TRUE : AL_FALSE, 0 };
    FlagSetOrClear(AFL_HRTFINIT,
      alcResetDeviceSOFT(alcDevice, alciAttrs.data()) == AL_FALSE);
  }
  /* -- Initialise device -------------------------------------------------- */
  void InitDevice(const char*const cpDevice)
  { // Open the specified device and if successful?
    if(ALCdevice*const alcNDevice = alcOpenDevice(cpDevice)) [[likely]]
    { // Unload previous device if it is set (impossible)
      if(alcDevice) [[unlikely]] DeInitContext();
      // Device is now initialise
      else FlagSet(AFL_INITDEVICE);
      // Set the new active device
      UpdateDevice(alcNDevice);
    }
  }
  /* -- DeInitialise device ------------------------------------------------ */
  bool DeInitDevice()
  { // Bail if no context
    if(!alcDevice) return false;
    // Close device and nullify handle
    DestroyDevice();
    UpdateDevice(nullptr);
    FlagClear(AFL_INITDEVICE);
    // Succeeded
    return true;
  }
  /* -- Initialise context ------------------------------------------------- */
  void InitContext()
  { // Set the context for the specified device and if successful?
    if(ALCcontext*const alcNContext =
      alcCreateContext(alcDevice, nullptr)) [[likely]]
    { // Unload previous context if it is set (impossible)
      if(alcContext) [[unlikely]] DeInitContext();
      // Context is now initialise
      else FlagSet(AFL_INITCONTEXT);
      // Set the new active context
      alcContext = alcNContext;
    } // Do nothing else
    else return;
    // Get maximum number of sources (dynamic on Apple implementation).
    stMaxMonoSources =
      GetInteger<decltype(stMaxMonoSources)>(ALC_MONO_SOURCES);
    stMaxStereoSources =
      GetInteger<decltype(stMaxStereoSources)>(ALC_STEREO_SOURCES);
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
        "Device", strvPlayback);
    } // Zero stereo sources? Failed because no mono sources
    else if(!stMaxStereoSources)
      XC("No stereo source support on this device!", "Device", strvPlayback);
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
        IsEventSupported(eciItem.aleEventType, eciItem.aleDeviceType))
      { // Event is supported?
        case ALC_EVENT_SUPPORTED_SOFT:
          // Enable system event
          SetEventState(eciItem.aleEventType, AL_TRUE);
          // Set capability
          FlagSet(eciItem.ofcFlag);
          // Done
          break;
        // Event is not supported?
        case ALC_EVENT_NOT_SUPPORTED_SOFT:
          // Disable system event
          SetEventState(eciItem.aleEventType, AL_FALSE);
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
    } // Log context initialisation
    cLog->LogDebugExSafe(
      "Oal playback device initialised with capabilities 0x$$.",
      GetVersion(), StdIOSHex, FlagGet());
    // Return if debug logging not enabled
    if(cLog->LogNotHasLevel(LH_DEBUG)) return;
    // Build sorted list of extensions and log them all
    using Pair = StdPair<const StdStringView, const size_t>;
    using Map = StdMap<Pair::first_type, Pair::second_type>;
    Map mExts;
    // Build context extensions list
    size_t stCount = 0;
    Tokeniser<StdStringView>(GetCString(ALC_EXTENSIONS),
      cCommon->CommonSpaceV(), [&mExts, &stCount](const StdStringView &strvExt)
        { mExts.insert({ StdMove(strvExt), stCount++ }); });
    // Log context initialisation
    cLog->LogNLCDebugExSafe(
      "- Maximum mono sources: $.\n"
      "- Maximum stereo sources: $.\n"
      "- Playback events: !$ +$ -$.\n"
      "- Capture events: !$ +$ -$.\n"
      "- Context extensions count: $...",
      stMaxMonoSources, stMaxStereoSources,
      StrFromBoolTF(FlagIsSet(AFL_HAVESEPBDDC)),
      StrFromBoolTF(FlagIsSet(AFL_HAVESEPBDA)),
      StrFromBoolTF(FlagIsSet(AFL_HAVESEPBDR)),
      StrFromBoolTF(FlagIsSet(AFL_HAVESECADDC)),
      StrFromBoolTF(FlagIsSet(AFL_HAVESECADA)),
      StrFromBoolTF(FlagIsSet(AFL_HAVESECADR)),
      mExts.size());
    // Log extensions if debug is enabled
    for(const Pair &pExt : mExts)
      cLog->LogNLCDebugExSafe("- Have extension '$' (#$).",
        pExt.first, pExt.second);
  }
  /* -- DeInitialise context ----------------------------------------------- */
  bool DeInitContext()
  { // Bail if no context
    if(!alcContext) return false;
    // Clear context
    alcMakeContextCurrent(nullptr);
    FlagClear(AFL_CONTEXTCURRENT);
    // Destroy context and nullify handle
    DestroyContext();
    alcContext = nullptr;
    FlagClear(AFL_INITCONTEXT);
    strvPlayback = cCommon->CommonNull();
    // Succeeded
    return true;
  }
  /* -- Initialise after the context has been set -------------------------- */
  void Init()
  { // Make sure not initialised already
    if(FlagIsSet(AFL_INITIALISED)) XC("OpenAL was already initialised!");
    // Activate the context (We can use alGetError() from now on)
    IALC(alcMakeContextCurrent(alcContext),
      "Failed to make OpenAL context current!");
    FlagSet(AFL_CONTEXTCURRENT);
    // Update playback device name
    UpdatePlaybackDeviceName();
    // Set if we have 32-bit floating-point playback
    FlagSetOrClear(AFL_HAVE32FPPB, alIsExtensionPresent("AL_EXT_FLOAT32"));
    // This is a thread safe version of this bool since the audio thread will
    // need access to this processing Stream objects.
    bHave32FPPB = FlagIsSet(AFL_HAVE32FPPB);
    // Show change in state
    cLog->LogInfoExSafe("Oal using playback device '$'.", GetPlaybackDevice());
    // Set the flag
    FlagSet(AFL_INITIALISED);
    // Return if debug logging not enabled
    if(cLog->LogNotHasLevel(LH_DEBUG)) return;
    // Build sorted list of extensions and log them all
    using Pair = StdPair<const StdStringView, const size_t>;
    using Map = StdMap<Pair::first_type, Pair::second_type>;
    Map mExts;
    // Build extensions list
    size_t stCount = 0;
    Tokeniser<StdStringView>(GetString(AL_EXTENSIONS), cCommon->CommonSpaceV(),
      [&mExts, &stCount](const StdStringView &strvExt)
        { mExts.insert({ StdMove(strvExt), stCount++ }); });
    // Log device info and basic capabilities
    cLog->LogNLCDebugExSafe(
      "- Head related transfer function: $.\n"
      "- Floating-point playback: $.\n"
      "- Have ext.device enumerator: $.\n"
      "- Device extensions count: $...",
      StrFromBoolTF(FlagIsSet(AFL_HRTFREQ)),
      StrFromBoolTF(Have32FPPB()),
      StrFromBoolTF(FlagIsSet(AFL_HAVEENUMEXT)),
      mExts.size());
    // Log extensions if debug is enabled
    for(const Pair &pExt : mExts)
      cLog->LogNLCDebugExSafe("- Have extension '$' (#$).",
        pExt.first, pExt.second);
  }
  /* -- DeInitialise ------------------------------------------------------- */
  void DeInit()
  { // De-init context and device if they're not gone already. The audio
    // class should be responsible for this, but just incase.
    DeInitContext();
    DeInitDevice();
    // Clear only volatile flags
    FlagClear(AFL_VOLATILE);
    // Clear everything else
    aleQuery = AL_NONE;
    stMaxStereoSources = stMaxMonoSources = 0;
  }
  /* -- Constructor --------------------------------------------- */ protected:
  Oal() :
    /* -- Initialisers ----------------------------------------------------- */
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
    strvPlayback{                      // Blank playback device
      cCommon->CommonNull() },         // Initialise with "null" text
    alcDevice(nullptr),                // Device not initialised yet
    alcContext(nullptr),               // Context not initialised yet
    bHave32FPPB(false),                // 32bit float cap not initialised yet
    aleQuery(AL_NONE)                  // Query method not initialised yet
    /* --------------------------------------------------------------------- */
    { cOal = this; }
  /* -- Destructor that unloads context and device ------------------------- */
  DTORHELPER(~Oal, if(alcContext) DestroyContext();
                   if(alcDevice) DestroyDevice())
  /* -- Update hints ----------------------------------------------- */ public:
  CVarReturn OalSetHRTF(const bool bState)
    { FlagSetOrClear(AFL_HRTFREQ, bState); return ACCEPT; }
  /* -- Update hints ----------------------------------------------- */ public:
  CVarReturn OalSetVersion(const double dVersion)
  { // Prepare version information. Partially initialises OpenALSoft too.
    const ALuint
      aluVersionMajor =
        GetInteger<decltype(aluVersionMajor)>(ALC_MAJOR_VERSION),
      aluVersionMinor =
        GetInteger<decltype(aluVersionMinor)>(ALC_MINOR_VERSION);
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
      GetVersion(), StdIOSHex, FlagGet());
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
