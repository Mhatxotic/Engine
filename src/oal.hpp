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
using namespace IError::P;             using namespace IFlags;
using namespace IIdent::P;             using namespace ILog::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IToken::P;             using namespace IUtf::P;
using namespace Lib::OpenAL;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- GL error checking wrapper macros ------------------------------------- */
#define ALEX(EF,F,M,...)  { F; EF(M, ## __VA_ARGS__); }
#define ALL(F,M,...)      ALEX(cOal->LogALError, F, M, ## __VA_ARGS__)
#define AL(F,M,...)       ALEX(cOal->CheckALError, F, M, ## __VA_ARGS__)
#define ALNF(M,...)       AL(, M, ## __VA_ARGS__)
/* -- GL context error checking wrapper macros ----------------------------- */
#define ALCL(F,M,...)     ALEX(cOal->LogALCError, F, M, ## __VA_ARGS__)
#define ALCLNF(M,...)     ALCL(, M, ## __VA_ARGS__)
#define ALC(F,M,...)      ALEX(cOal->CheckALCError, F, M, ## __VA_ARGS__)
#define ALCNF(M,...)      ALC(, M, ## __VA_ARGS__)
/* -- Typedefs ------------------------------------------------------------- */
typedef vector<ALuint> ALUIntVector; // A vector of ALuint's
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
#define IALNF(M,...)  IAL(, M, ## __VA_ARGS__)
#define IALC(F,M,...) ALEX(CheckALCError, F, M, ## __VA_ARGS__)
#define IALCNF(M,...) IALC(, M, ## __VA_ARGS__)
  /* ----------------------------------------------------------------------- */
  typedef IdMap<ALenum> ALenumMap;     // A map of ALenum integers
  const ALenumMap  almOALCodes,        // OpenAL standard error codes
                   almOALCCodes,       // OpenAL context error codes
                   almFormatCodes;     // OpenAL format codes
  /* ----------------------------------------------------------------------- */
  size_t           stMaxStereoSources, // Maximum number of stereo sources
                   stMaxMonoSources;   // Maximum number of mono sources
  /* ----------------------------------------------------------------------- */
  string           strVersion;         // String version of OpenAL
  string_view      strvPlayback;       // String playback device
  /* ----------------------------------------------------------------------- */
  ALCdevice       *alcDevice;          // OpenAL device
  ALCcontext      *alcContext;         // OpenAL context
  /* ----------------------------------------------------------------------- */
  bool             bHave32FPPB;        // Cached version of 32-bit float cap
  /* -- Public Variables --------------------------------------------------- */
  ALenum           eQuery;             // Device query extension
  /* -- AL generic context error logger ------------------------------------ */
  template<ALenum alNoErr, typename FuncTypeErr,
           typename FuncTypeStr, typename ...VarArgs>
  void LogErrorGeneric(FuncTypeErr fteFunc, FuncTypeStr ftsFunc,
    const char*const cpPrefix, const char*const cpFormat,
    VarArgs &&...vaArgs) const
  { // Enumerate all queued OpenAL errors and print them
    for(ALenum alError = fteFunc(); alError != alNoErr; alError = fteFunc())
      cLog->LogWarningExSafe("AL$ call failed: $ ($/0x$$).", cpPrefix,
        StrFormat(cpFormat, StdForward<VarArgs>(vaArgs)...), ftsFunc(alError),
        hex, alError);
  }
  /* -- AL generic exception handler --------------------------------------- */
  template<ALenum alNoErr, typename FuncTypeErr, typename FuncTypeStr,
    typename ...VarArgs>
  void CheckErrorGeneric(FuncTypeErr fteFunc, FuncTypeStr ftsFunc,
    const char*const cpType, const char*const cpReason, VarArgs &&...vaArgs)
      const
  { // Get error and return if error is good
    const ALenum alError = fteFunc();
    if(alError == alNoErr) [[likely]] return;
    // Build an exception object to show why the call failed
    XC(StrFormat("AL$ call failed: $", cpType, cpReason),
      StdForward<VarArgs>(vaArgs)...,
      "Code", alError, "Reason", ftsFunc(alError));
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
  ALenum GetQueryMethod() const { return eQuery; }
  /* -- AL context error logger -------------------------------------------- */
  template<typename ...VarArgs>
    void LogALCError(const char*const cpFormat, VarArgs &&...vaArgs) const
  { LogErrorGeneric<ALC_NO_ERROR>(
      [this]()->ALenum{ return GetContextError(); },
      [this](const ALenum alError){ return GetALCErr(alError); },
      " context", cpFormat, StdForward<decltype(vaArgs)>(vaArgs)...); }
  /* -- AL error logger ---------------------------------------------------- */
  template<typename ...VarArgs>
    void LogALError(const char*const cpFormat, VarArgs &&...vaArgs) const
  { LogErrorGeneric<AL_NO_ERROR>(
      []()->ALenum{ return GetError(); },
      [this](const ALenum alError){ return GetALErr(alError); },
      cCommon->CommonCBlank(), cpFormat,
      StdForward<decltype(vaArgs)>(vaArgs)...); }
  /* -- AL context error handler ------------------------------------------- */
  template<typename ...VarArgs>
    void CheckALCError(const char*const cpReason, VarArgs &&...vaArgs) const
  { CheckErrorGeneric<ALC_NO_ERROR>(
      [this]()->ALenum{ return GetContextError(); },
      [this](const ALenum alError) { return GetALCErr(alError); },
      " context", cpReason, StdForward<VarArgs>(vaArgs)...); }
  /* -- AL error handler --------------------------------------------------- */
  template<typename ...VarArgs>
    void CheckALError(const char*const cpReason, VarArgs &&...vaArgs) const
  { CheckErrorGeneric<AL_NO_ERROR>(
      []()->ALenum{ return GetError(); },
      [this](const ALenum alError) { return GetALErr(alError); },
      cCommon->CommonCBlank(), cpReason, StdForward<VarArgs>(vaArgs)...); }
  /* -- Upload data to audio device ---------------------------------------- */
  static void BufferData(const ALuint uiBuffer, const ALenum eFormat,
    const ALvoid*const vpData, const ALsizei siSize, const ALsizei siFrequency)
  { alBufferData(uiBuffer, eFormat, vpData, siSize, siFrequency); }
  /* -- Upload data to audio device ---------------------------------------- */
  static void BufferData(const ALuint uiBuffer, const ALenum eFormat,
    const MemConst &mcSrc, const ALsizei siFrequency)
  { BufferData(uiBuffer, eFormat, mcSrc.MemPtr<ALvoid>(),
      mcSrc.MemSize<ALsizei>(), siFrequency); }
  /* -- Queue specified buffer count into source --------------------------- */
  static void QueueBuffers(const ALuint uiSource,
    const ALsizei siCount, ALuint*const uipBuffer)
  { alSourceQueueBuffers(uiSource, siCount, uipBuffer); }
  /* -- Queue one buffer count into source --------------------------------- */
  static void QueueBuffer(const ALuint uiSource, ALuint uiBuffer)
    { QueueBuffers(uiSource, 1, &uiBuffer); }
  /* -- Unqueue specified buffer count from source and place into buffers -- */
  static void UnQueueBuffers(const ALuint uiSource,
    const ALsizei siCount, ALuint*const uipBuffer)
  { alSourceUnqueueBuffers(uiSource, siCount, uipBuffer); }
  /* -- Unqueue one buffer from source and place into buffers -------------- */
  static void UnQueueBuffer(const ALuint uiSource, ALuint &uiBuffer)
    { UnQueueBuffers(uiSource, 1, &uiBuffer); }
  /* -- Unqueue one buffer from source and return it ----------------------- */
  static ALuint UnQueueBuffer(const ALuint uiSource)
    { ALuint uiBuffer; UnQueueBuffer(uiSource, uiBuffer); return uiBuffer; }
  /* -- Set source value as float ------------------------------------------ */
  static void SetSourceFloat(const ALuint uiSource, const ALenum eWhat,
    const ALfloat fValue)
  { alSourcef(uiSource, eWhat, fValue); }
  /* -- Get source value as float ------------------------------------------ */
  static void GetSourceFloat(const ALuint uiSource, const ALenum eWhat,
    ALfloat*const fpDestValue)
  { alGetSourcef(uiSource, eWhat, fpDestValue); }
  /* -- Set source value as int -------------------------------------------- */
  static void SetSourceInt(const ALuint uiSource, const ALenum eWhat,
    const ALint iValue)
  { alSourcei(uiSource, eWhat, iValue); }
  /* -- Get source value as int -------------------------------------------- */
  static void GetSourceInt(const ALuint uiSource, const ALenum eWhat,
    ALint*const ipDestValue)
  { alGetSourcei(uiSource, eWhat, ipDestValue); }
  /* -- Get source value as a float vector --------------------------------- */
  static void GetSourceVector(const ALuint uiSource, const ALenum eWhat,
    ALfloat*const fpDX, ALfloat*const fpDY, ALfloat*const fpDZ)
  { alGetSource3f(uiSource, eWhat, fpDX, fpDY, fpDZ); }
  /* -- Set source value as a float vector --------------------------------- */
  static void SetSourceVector(const ALuint uiSource, const ALenum eWhat,
    const ALfloat fDX, const ALfloat fDY, const ALfloat fDZ)
      { alSource3f(uiSource, eWhat, fDX, fDY, fDZ); }
  /* -- Stop a source from playing ----------------------------------------- */
  static void StopSource(const ALuint uiSource) { alSourceStop(uiSource); }
  /* -- Play a source ------------------------------------------------------ */
  static void PlaySource(const ALuint uiSource) { alSourcePlay(uiSource); }
  /* -- Rewind a source ---------------------------------------------------- */
  static void RewindSource(const ALuint uiSource) { alSourceRewind(uiSource); }
  /* -- Pause a source ---------------------------------------------------- */
  static void PauseSource(const ALuint uiSource) { alSourcePause(uiSource); }
  /* -- Play more than one source simultaniously --------------------------- */
  template<class ArrayType>static void PlaySources(const ArrayType &atArray)
    { alSourcePlayv(static_cast<ALsizei>(atArray.size()), atArray.data()); }
  /* -- Create multiple sources -------------------------------------------- */
  static void CreateSources(const ALsizei siCount, ALuint*const uipSource)
    { alGenSources(siCount, uipSource); }
  /* -- Create one source and place it in the specified buffer ------------- */
  static void CreateSource(ALuint &uiSourceRef)
    { CreateSources(1, &uiSourceRef); }
  /* -- Create and return a source ----------------------------------------- */
  static ALuint CreateSource()
    { ALuint uiSource; CreateSource(uiSource); return uiSource; }
  /* -- Delete multiple sources -------------------------------------------- */
  static void DeleteSources(const ALsizei siCount,
    const ALuint*const uipSource)
  { alDeleteSources(siCount, uipSource); }
  /* -- Delete multiple sources -------------------------------------------- */
  template<class List>static void DeleteSources(const List &lList)
    { DeleteSources(static_cast<ALsizei>(lList.size()), lList.data()); }
  /* -- Delete one source -------------------------------------------------- */
  static void DeleteSource(const ALuint &uiSourceRef)
    { DeleteSources(1, &uiSourceRef); }
  /* -- Create multiple buffers -------------------------------------------- */
  static void CreateBuffers(const ALsizei siCount, ALuint*const uipBuffer)
    { alGenBuffers(siCount, uipBuffer); }
  /* -- Create multiple buffers -------------------------------------------- */
  template<class List>static void CreateBuffers(List &lList)
    { CreateBuffers(static_cast<ALsizei>(lList.size()), lList.data()); }
  /* -- Create one buffer and place it in the specified variable ----------- */
  static void CreateBuffer(ALuint &uiBuffer) { CreateBuffers(1, &uiBuffer); }
  /* -- Create and return a buffer ----------------------------------------- */
  static ALuint CreateBuffer()
    { ALuint uiBuffer; CreateBuffer(uiBuffer); return uiBuffer; }
  /* -- Delete multiple buffers -------------------------------------------- */
  static void DeleteBuffers(const ALsizei siCount,
    const ALuint*const uipBuffer)
  { alDeleteBuffers(siCount, uipBuffer); }
  /* -- Delete multiple sources -------------------------------------------- */
  template<class List>static void DeleteBuffers(const List &lList)
    { DeleteBuffers(static_cast<ALsizei>(lList.size()), lList.data()); }
  /* -- Delete one buffer -------------------------------------------------- */
  static void DeleteBuffer(const ALuint &uiBufferRef)
    { DeleteBuffers(1, &uiBufferRef); }
  /* -- Get buffer parameter as integer ------------------------------------ */
  static void GetBufferInt(const ALuint uiBId, const ALenum eId,
    ALint*const ipDest)
      { alGetBufferi(uiBId, eId, ipDest); }
  /* -- Get buffer information --------------------------------------------- */
  template<typename IntType=ALint>
    static IntType GetBufferInt(const ALuint uiBId, const ALenum eId)
  { ALint iV; GetBufferInt(uiBId, eId, &iV); return static_cast<IntType>(iV); }
  /* -- Set distance model ------------------------------------------------- */
  static void SetDistanceModel(const ALenum eModel)
    { alDistanceModel(eModel); }
  /* -- Set listener vector ------------------------------------------------ */
  static void SetListenerVector(const ALenum eParam, const ALfloat fX,
    const ALfloat fY, const ALfloat fZ)
      { alListener3f(eParam, fX, fY, fZ); }
  /* -- Set listener position ---------------------------------------------- */
  static void SetListenerPosition(const ALfloat fX, const ALfloat fY,
    const ALfloat fZ)
      { SetListenerVector(AL_POSITION, fX, fY, fZ); }
  /* -- Set listener velocity ---------------------------------------------- */
  static void SetListenerVelocity(const ALfloat fX, const ALfloat fY,
    const ALfloat fZ)
      { SetListenerVector(AL_VELOCITY, fX, fY, fZ); }
  /* -- Set listener velocity ---------------------------------------------- */
  static void SetListenerVectors(const ALenum eParam,
    const ALfloat*const fpVectors)
  { alListenerfv(eParam, fpVectors); }
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
      eQuery = ALC_ALL_DEVICES_SPECIFIER;
    } // Set standarded device query extension
    else eQuery = ALC_DEVICE_SPECIFIER;
  }
  /* == Convert bitrate and channels to an openal useful identifier ======== */
  static bool GetOALType(const ALuint uiChannels, const ALuint uiBits,
    ALenum &eFormat, ALenum &eSFormat)
  { // Compare channels
    switch(uiChannels)
    { // MONO: 1 channel
      case 1:
      { // Compare bit count
        switch(uiBits)
        { // 8-bits per sample (Integer)
          case  8: eFormat = eSFormat = AL_FORMAT_MONO8; return true;
          // 16-bits per sample (Integer)
          case 16: eFormat = eSFormat = AL_FORMAT_MONO16; return true;
          // 32-bits per sample (Float)
          case 32: eFormat = eSFormat = AL_FORMAT_MONO_FLOAT32; return true;
          // Unknown
          default: return false;
        }
      } // STEREO: 2 channels
      case 2:
      { // Compare bit count
        switch(uiBits)
        { // 8-bits per sample (Integer)
          case  8: eFormat = AL_FORMAT_STEREO8;
                   eSFormat = AL_FORMAT_MONO8;
                   return true;
          // 16-bits per sample (Integer)
          case 16: eFormat = AL_FORMAT_STEREO16;
                   eSFormat = AL_FORMAT_MONO16;
                   return true;
          // 32-bits per sample (Float)
          case 32: eFormat = AL_FORMAT_STEREO_FLOAT32;
                   eSFormat = AL_FORMAT_MONO_FLOAT32;
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
  template<typename CStrType=ALchar>
    const CStrType *GetString(const ALenum eId) const
  { // Get the variable and throw error if occured
    const ALchar*const ucpStr = alGetString(eId);
    IALNF("Get string failed!", "Index", eId);
    // Sanity check actual string
    if(!UtfIsCStringValid(ucpStr))
      XC("Invalid string returned!", "Index", eId, "String", ucpStr);
    // Return result
    return reinterpret_cast<const CStrType*>(ucpStr);
  }
  /* -- Get context openAL string ------------------------------------------ */
  template<typename StrType=const ALCchar*>
    static StrType ContextGetString(ALCdevice*const alcDevice,
      const ALCenum alcEnum)
  { return reinterpret_cast<StrType>(alcGetString(alcDevice, alcEnum)); }
  /* ----------------------------------------------------------------------- */
  template<typename StrType=const ALCchar*>
    StrType GetCString(ALCdevice*const alcDev, const ALenum eId) const
  { return reinterpret_cast<StrType>(ContextGetString(alcDev, eId)); }
  /* -- Get context openAL string ------------------------------------------ */
  template<typename StrType=const ALCchar*>
    StrType GetCString(const ALenum eId) const
  { return GetCString<StrType>(alcDevice, eId); }
  /* -- Get nullptr context openAL string ---------------------------------- */
  template<typename StrType=const ALCchar*>
    static StrType GetNCString(const ALenum eId)
  { return reinterpret_cast<StrType>(ContextGetString(nullptr, eId)); }
  /* -- Get openAL int array ----------------------------------------------- */
  template<size_t stCount, class A=array<ALCint,stCount>>
    const A GetIntegerArray(const ALenum eId) const
  { // Create array to return
    A aData;
    // Get specified value for enum and store it
    IALC(alcGetIntegerv(alcDevice, eId, sizeof(A), aData.data()),
      "Get integer array failed!", "Index", eId, "Count", stCount);
    // Return array
    return aData;
  }
  /* -- Get openAL int ----------------------------------------------------- */
  template<typename T=ALCint>T GetInteger(const ALenum eId) const
    { return static_cast<T>(GetIntegerArray<1>(eId)[0]); }
  /* -- Convert PCM format identifier to short identifier string ----------- */
  const string_view &GetALFormat(const ALenum eFormat) const
    { return almFormatCodes.Get(eFormat); }
  /* -- Get source counts -------------------------------------------------- */
  size_t GetMaxMonoSources() const { return stMaxMonoSources; }
  size_t GetMaxStereoSources() const { return stMaxStereoSources; }
  /* -- Get current playback device ---------------------------------------- */
  const string_view &GetPlaybackDevice() const { return strvPlayback; }
  /* -- Return version information ----------------------------------------- */
  const string &GetVersion() const { return strVersion; }
  /* -- Set system event callback ------------------------------------------ */
  void SetEventCallback(const ALCEVENTPROCTYPESOFT cbProc, void*const vpParam)
  { // Keeping Ubuntu 24.04 compatibility for now until supported
#if !defined(LINUX)
    return alcEventCallbackSOFT(cbProc, vpParam);
#endif
  }
  /* -- Enable or disable system events ------------------------------------ */
  ALenum SetEventState(const ALCenum eEvent, const ALCboolean bEnabled)
  { // Keeping Ubuntu 24.04 compatibility for now until supported
#if defined(LINUX)
    return AL_FALSE;
#else
    return alcEventControlSOFT(1, &eEvent, bEnabled);
#endif
  }
  /* -- Is system event supported ------------------------------------------ */
  ALenum IsEventSupported(const ALenum eEventType, const ALenum eDeviceType)
  { // Keeping Ubuntu 24.04 compatibility for now until supported
#if defined(LINUX)
    return ALC_EVENT_NOT_SUPPORTED_SOFT;
#else
    return alcEventIsSupportedSOFT(eEventType, eDeviceType);
#endif
  }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    const string_view &GetALErr(const IntType itCode) const
      { return almOALCodes.Get(static_cast<ALenum>(itCode)); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    const string_view &GetALCErr(const IntType itCode) const
      { return almOALCCodes.Get(static_cast<ALenum>(itCode)); }
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
    const array<const ALCint,3> alciAttrs{
      ALC_HRTF_SOFT, FlagIsSet(AFL_HRTFREQ) ? AL_TRUE : AL_FALSE, 0 };
    FlagSetOrClear(AFL_HRTFINIT,
      alcResetDeviceSOFT(alcDevice, alciAttrs.data()) == AL_FALSE);
  }
  /* -- Initialise device -------------------------------------------------- */
  void InitDevice(const char *cpDevice)
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
    }
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
    // Prepare version information
    const ALuint
      uiVersionMajor = GetInteger<decltype(uiVersionMajor)>(ALC_MAJOR_VERSION),
      uiVersionMinor = GetInteger<decltype(uiVersionMinor)>(ALC_MINOR_VERSION);
    strVersion = StrAppend(uiVersionMajor, '.', uiVersionMinor);
    // Need at least version 1.1 of OpenAL
    if(uiVersionMajor < 1 || (uiVersionMajor == 1 && uiVersionMinor < 1))
      XC("OpenAL version 1.1 is required!",
        "Major", uiVersionMajor, "Minor", uiVersionMinor);
    // Set if we have 32-bit floating-point playback
    FlagSetOrClear(AFL_HAVE32FPPB, alIsExtensionPresent("AL_EXT_FLOAT32"));
    // This is a thread safe version of this bool since the audio thread will
    // need access to this processing Stream objects.
    bHave32FPPB = FlagIsSet(AFL_HAVE32FPPB);
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
    struct EventCapItem { const ALenum eEventType, eDeviceType;
                          const OalFlagsConst &ofcFlag; };
    for(const EventCapItem &eciItem : array<EventCapItem, 6>{{
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
      switch(const ALenum eResult =
        IsEventSupported(eciItem.eEventType, eciItem.eDeviceType))
      { // Event is supported?
        case ALC_EVENT_SUPPORTED_SOFT:
          // Enable system event
          SetEventState(eciItem.eEventType, AL_TRUE);
          // Set capability
          FlagSet(eciItem.ofcFlag);
          // Done
          break;
        // Event is not supported?
        case ALC_EVENT_NOT_SUPPORTED_SOFT:
          // Disable system event
          SetEventState(eciItem.eEventType, AL_FALSE);
          // Clear capability
          FlagClear(eciItem.ofcFlag);
          // Done
          break;
        // Invalid result
        default: XC("Internal error: Invalid AL event query result!",
          "Flag",       eciItem.ofcFlag,     "EventType", eciItem.eEventType,
          "DeviceType", eciItem.eDeviceType, "Result",    eResult);
      }
    } // Show change in state
    cLog->LogInfoExSafe(
      "OAL version $ initialised with capabilities 0x$$...\n"
      "- Device: $.",
      GetVersion(), hex, FlagGet(), GetPlaybackDevice());
    // Set the flag
    FlagSet(AFL_INITIALISED);
    // Return if debug logging not enabled
    if(cLog->LogNotHasLevel(LH_DEBUG)) return;
    // Build sorted list of extensions and log them all
    typedef pair<const string_view, const size_t> Pair;
    typedef map<Pair::first_type, Pair::second_type> Map;
    Map mExts;
    // Build extensions list
    size_t stCount = 0;
    Tokeniser<string_view>(GetString(AL_EXTENSIONS), cCommon->CommonSpace(),
      [&mExts, &stCount](const string_view &strvExt){
        mExts.insert({ StdMove(strvExt), stCount++ });
      }
    );
    // Log device info and basic capabilities
    cLog->LogNLCDebugExSafe(
      "- Head related transfer function: $.\n"
      "- Floating-point playback: $.\n"
      "- Maximum mono sources: $.\n"
      "- Maximum stereo sources: $.\n"
      "- Have ext.device enumerator: $.\n"
      "- Playback events: !$ +$ -$.\n"
      "- Capture events: !$ +$ -$.\n"
      "- Extensions count: $.",
      StrFromBoolTF(FlagIsSet(AFL_HRTFREQ)),
      StrFromBoolTF(Have32FPPB()),
      stMaxMonoSources, stMaxStereoSources,
      StrFromBoolTF(FlagIsSet(AFL_HAVEENUMEXT)),
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
  /* -- DeInitialise ------------------------------------------------------- */
  void DeInit()
  { // De-init context and device if they're not gone already. The audio
    // class should be responsible for this, but just incase.
    DeInitContext();
    DeInitDevice();
    // Clear only volatile flags
    FlagClear(AFL_VOLATILE);
    // Clear everything else
    eQuery = AL_NONE;
    stMaxStereoSources = stMaxMonoSources = 0;
  }
  /* -- Constructor --------------------------------------------- */ protected:
  Oal() :
    /* -- Initialisers ----------------------------------------------------- */
    OalFlags{ AFL_NONE },              // Flags not initialised yet
    /* -- Const members ---------------------------------------------------- */
    almOALCodes{{                      // Init OpenAL error codes
      IDMAPSTR(AL_NO_ERROR),           IDMAPSTR(AL_INVALID_NAME),
      IDMAPSTR(AL_INVALID_ENUM),       IDMAPSTR(AL_INVALID_VALUE),
      IDMAPSTR(AL_INVALID_OPERATION),  IDMAPSTR(AL_OUT_OF_MEMORY)
    }, "AL_UNKNOWN" },
    almOALCCodes{{                     // Init OpenAL context error codes
      IDMAPSTR(ALC_NO_ERROR),          IDMAPSTR(ALC_INVALID_DEVICE),
      IDMAPSTR(ALC_INVALID_CONTEXT),   IDMAPSTR(ALC_INVALID_ENUM),
      IDMAPSTR(ALC_INVALID_VALUE),     IDMAPSTR(ALC_OUT_OF_MEMORY),
    }, "ALC_UNKNOWN" },
    almFormatCodes{{                   // Init OpenAL format codes
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
    eQuery(AL_NONE)                    // Query method not initialised yet
    /* -- Set global pointer to static class ------------------------------- */
    { cOal = this; }
  /* -- Destructor that unloads context and device ------------------------- */
  DTORHELPER(~Oal, if(alcContext) DestroyContext();
                   if(alcDevice) DestroyDevice())
  /* -- Update hints ----------------------------------------------- */ public:
  CVarReturn OalSetHRTF(const bool bState)
    { FlagSetOrClear(AFL_HRTFREQ, bState); return ACCEPT; }
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
