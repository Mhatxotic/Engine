/* == AUDIO.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This the file handles audio management of sources and streams       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IAudio {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICollector::P;
using namespace ICVar::P;              using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IError::P;
using namespace IEvtMain::P;           using namespace IFlags;
using namespace ILog::P;               using namespace IOal::P;
using namespace ISample::P;            using namespace ISource::P;
using namespace IStd::P;               using namespace IStream::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IThread::P;            using namespace ITimer::P;
using namespace IVideo::P;             using namespace Lib::OpenAL;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Typedefs ============================================================= */
BUILD_FLAGS(Audio,                     // Audio flags classes
  /* ----------------------------------------------------------------------- */
  // No settings?                      Audio system is resetting?
  AF_NONE                   {Flag[0]}, AF_REINIT                 {Flag[1]}
);/* ======================================================================= */
static class Audio final :             // Audio manager class
  /* -- Base classes ------------------------------------------------------- */
  private IHelper,                     // Initialisation helper class
  public AudioFlags,                   // Audio flags
  private Thread                       // Audio monitoring thread
{ /* -- Monitoring thread timers ---------------------------------- */ private:
  ClkTimePoint     tpNextCheck;        // Next check for hardware changes
  SafeClkDuration  cdCheckRate,        // Check rate
                   cdThreadDelay;      // Thread sleep time
  const ClkDuration cdDiscWait;        // Sleep time waiting for main thread
  /* -- Devices ------------------------------------------------------------ */
  StrVector        dlPBDevices,        // list of playback devices
                   dlCTDevices;        // list of capture devices
  /* -- ReInit requested --------------------------------------------------- */
  void OnReInit(const EvtMainEvent&)
  { // Capture exceptions
    try
    { // Log status
      cLog->LogDebugSafe("Audio class re-initialising...");
      // De-Init thread
      DeInitThread();
      // Unload all buffers for streams and samples and destroy all sources
      StreamDeInit();
      SampleDeInit();
      VideoDeInit();
      cSources->CollectorDestroyUnsafe();
      // Deinit and reinit context
      DeInitContext();
      InitContext();
      // Re-initialise volume levels
      SetGlobalVolume(cSources->fGVolume);
      SampleSetVolume(cSources->fSVolume);
      StreamSetVolume(cSources->fMVolume);
      VideoSetVolume(cSources->fVVolume);
      // Re-create all buffers for streams and samples
      VideoReInit();
      SampleReInit();
      StreamReInit();
      // Init monitoring thread
      InitThread();
      // Log status
      cLog->LogInfoSafe("Audio class re-initialised successfully.");
    } // We don't want LUA to hard break really.
    catch(const exception &E)
    { // Log the exception first
      cLog->LogErrorExSafe("Audio re-init exception: $", E.what());
      // Reset next thread check time
      ResetCheckTime();
    } // Remove re-initialisation flag
    FlagClear(AF_REINIT);
  }
  /* -- Reset timer interval ----------------------------------------------- */
  void ResetCheckTime(void)
    { tpNextCheck = cmHiRes.GetTime() + cdCheckRate.load(); }
  /* -- Thread main function ----------------------------------------------- */
  int AudioThreadMain(Thread &) try
  { // Enumerate...
    for(ResetCheckTime();              // Reset device list check time
        ThreadShouldNotExit();         // Enumerate until thread exit signalled
        cTimer->TimerSuspend(cdThreadDelay)) // Suspend thread pecified time
    { // Manage all streams audio.
      StreamManage();
      // Verify the hardware setup and reset if there are any descreprencies.
      // If there were no descreprencies detected then loop again.
      if(Verify()) continue;
      // Put in infinite loop and wait for the reinit function to request
      // termination of this thread
      while(ThreadShouldNotExit()) cTimer->TimerSuspend(cdDiscWait);
      // Thread terminate request recieved, now break the loop.
      break;
    } // Terminate thread
    return 1;
  } // exception occured in this thread
  catch(const exception &E)
  { // Report error
    cLog->LogErrorExSafe("(AUDIO THREAD EXCEPTION) $", E.what());
    // Restart the thread
    return 0;
  }
  /* -- Verification of context -------------------------------------------- */
  bool Verify(void)
  { // Ignore if next check time not met
    if(cmHiRes.GetTime() < tpNextCheck) return true;
    // Number of discrepancies found. If there is no such device in the current
    // device list, or the current device item does not equal to the newly
    // detected item, then that is a discrepancy.
    size_t stDiscrepancies = 0;
    // Grab list of playback devices and if we found them?
    if(const char *cpList = cOal->GetNCString(cOal->eQuery))
    { // Number of new items detected
      size_t stIndex = 0;
      // For each playback device, is the first character valid?
      while(*cpList)
      { // Is index valid?
        if(stIndex >= dlPBDevices.size())
        { // Log warning and add to discrepancy list
          ++stDiscrepancies;
          cLog->LogDebugExSafe("Audio thread discrepancy $: "
            "Device index $ over limit of $!",
              stDiscrepancies, stIndex, dlPBDevices.size());
        } // Is the device name the same
        else if(dlPBDevices[stIndex] != cpList)
        { // Log warning and add to discreprency list
          ++stDiscrepancies;
          cLog->LogDebugExSafe("Audio thread discrepancy $: "
            "Expected device '$' at $, not '$'!",
              stDiscrepancies, dlPBDevices[stIndex], stIndex, cpList);
        } // Jump to next item
        cpList += strlen(cpList) + 1;
        // increment device index
        ++stIndex;
      }
      // If the number of items in the newly detected list doesn't match
      // the number of items in the current list then that is another
      // discrepancy.
      if(stIndex != dlPBDevices.size())
      { // Log warning and add to discrepancy list
        ++stDiscrepancies;
        cLog->LogDebugExSafe("Audio thread discrepancy $: "
          "Detected $ devices and had $ before!",
            stDiscrepancies, stIndex, dlPBDevices.size());
      }
    } // No newly detected list and have items in current list? Fail!
    else if(dlPBDevices.size())
    { // Log warning and add to discrepancy list
      ++stDiscrepancies;
      cLog->LogDebugExSafe("Audio thread discrepancy $: "
        "$ new devices detected!", stDiscrepancies, dlPBDevices.size());
    } // If there are discrepancies, then we need to restart the subsystem.
    if(stDiscrepancies)
    { // Send message to log
      cLog->LogWarningExSafe(
        "Audio thread restarting on $ device discrepancies!", stDiscrepancies);
      // Send event to reset audio
      cEvtMain->Add(EMC_AUD_REINIT);
      // Terminate this thread
      return false;
    } // Check again in a few seconds
    ResetCheckTime();
    // Audio context still valid, continue monitoring
    return true;
  }
  /* -- DeInit thread ------------------------------------------------------ */
  void DeInitThread(void)
  { // Stop and de-init the thread and log progress
    cLog->LogDebugSafe("Audio monitoring thread de-initialising...");
    ThreadDeInit();
    cLog->LogDebugSafe("Audio monitoring thread de-initialised.");
  }
  /* -- Init thread -------------------------------------------------------- */
  void InitThread(void)
  { // Initialise and start thread and log progress
    cLog->LogDebugSafe("Audio monitoring thread initialising...");
    ThreadStart(this);
    cLog->LogDebugExSafe("Audio monitoring thread initialised (D:$;C:$)!",
      StrShortFromDuration(ClockDurationToDouble(cdThreadDelay)),
      StrShortFromDuration(ClockDurationToDouble(cdCheckRate)));
  }
  /* -- Init context ------------------------------------------------------- */
  void InitContext(void)
  { // Log that we are initialising
    cLog->LogDebugSafe("Audio subsystem initialising...");
    // Enumerate devices
    EnumeratePlaybackDevices();
    EnumerateCaptureDevices();
    // Set device and override if requested
    size_t stDevice = cCVars->GetInternal<size_t>(AUD_INTERFACE);
    // Holding current device name
    string strDevice;
    // If -1 is not set (use specific device)
    if(stDevice != string::npos)
    { // Invalid device? Use default device!
      if(stDevice >= dlPBDevices.size())
      { // Log that the device id is invalid
        cLog->LogWarningExSafe("Audio device #$ invalid so using default!",
          stDevice);
        // Set to default device
        stDevice = 0;
      } // Set device
      strDevice = GetPlaybackDeviceById(stDevice).c_str();
    } // Device id setup complete
    { // Get pointer to device name
      const char*const cpDevice =
        strDevice.empty() ? nullptr : strDevice.c_str();
      // Say device being used
      cLog->LogDebugExSafe("Audio trying to initialise device '$'...",
        cpDevice ? cpDevice : "<Default>");
      // Open the device
      if(!cOal->InitDevice(cpDevice))
        XC("Failed to open al device!",
           "Identifier", strDevice, "Index", stDevice);
    } // Reopen the device to disable HRTF
    if(!cOal->SetHRTF(cCVars->GetInternal<bool>(AUD_HRTF)))
      XC("Audio failed to reconfigure al device!",
         "Identifier", strDevice, "Index", stDevice);
    // Create the context
    if(!cOal->InitContext())
      XC("Failed to create al context!",
         "Identifier", strDevice, "Index", stDevice);
    // Have the context
    cOal->Init();
    // Allocate sources data
    SourceAlloc(cCVars->GetInternal<ALuint>(AUD_NUMSOURCES));
    // Register engine events
    cEvtMain->Register(EMC_AUD_REINIT, bind(&Audio::OnReInit, this, _1));
    // Set parameters and check for errors
    SetDistanceModel(AL_NONE);
    SetPosition(0, 0, 0);
    SetVelocity(0, 0, 0);
    SetOrientation(0, 0, 1, 0, -1, 0);
  }
  /* -- DeInitContext ------------------------------------------------------ */
  void DeInitContext(void)
  { // Clear openAL context
    cOal->DeInit();
    // Unregister engine events
    cEvtMain->Unregister(EMC_AUD_REINIT);
  }
  /* -- Enumerate capture devices ------------------------------------------ */
  void EnumerateCaptureDevices(void)
  { // Log enumerations
    cLog->LogDebugSafe("Audio enumerating capture devices...");
    // Storage for list of devices
    const char *cpList = nullptr;
    // Clear recording devices list
    dlCTDevices.clear();
    // Same rule applies, try 3 times...
    for(size_t stIndex = 0; stIndex < 3; ++stIndex, cTimer->TimerSuspend())
    { // Grab list of capture devices and break if succeeded
      cpList = cOal->GetNCString(ALC_CAPTURE_DEVICE_SPECIFIER);
      if(cpList) break;
      // Report that the attempt failed
      cLog->LogWarningExSafe("Audio attempt $ failed on capture device "
        "enumeration (0x$$), retrying...", stIndex+1, hex, cOal->GetError());
    } // Capture devices found?
    if(cpList)
    { // For each capture device
      while(*cpList)
      { // Print device
        cLog->LogDebugExSafe("- $: $.", GetNumCaptureDevices(), cpList);
        // Push to list
        dlCTDevices.emplace_back(cpList);
        // Jump to next item
        cpList += strlen(cpList) + 1;
      } // Until no more devices
    } // Problems, problems
    else cLog->LogErrorExSafe(
      "Audio couldn't access capture devices list (0x$$).",
      hex, cOal->GetError());
    // Log enumerations
    cLog->LogInfoExSafe("Audio found $ capture devices.",
      GetNumCaptureDevices());
  }
  /* -- Enumerate playback devices ----------------------------------------- */
  void EnumeratePlaybackDevices(void)
  { // Log enumerations
    cLog->LogDebugSafe("Audio enumerating playback devices...");
    // Clear playback devices list
    dlPBDevices.clear();
    // Detect if we have extended or standard query
    cOal->DetectEnumerationMethod();
    // For some reason, this call is sometimes failing, probably because
    // another process is enumerating the list, so we'll do a retry loop.
    // Try to grab list of playback devices three times before failing.
    for(size_t stIndex = 0; stIndex < 3; ++stIndex, cTimer->TimerSuspend())
    { // Get list of devices and break if succeeded
      if(const char *cpList = cOal->GetNCString(cOal->eQuery))
      { // For each playback device
        while(*cpList)
        { // Print device
          cLog->LogDebugExSafe("- $: $.", GetNumPlaybackDevices(), cpList);
          // Push to list
          dlPBDevices.emplace_back(cpList);
          // Jump to next item
          cpList += strlen(cpList) + 1;
          // Until no more devices
        } // Log enumerations and return
        cLog->LogInfoExSafe("Audio found $ playback devices.",
          GetNumPlaybackDevices());
        return;
      } // Report that the attempt failed
      cLog->LogWarningExSafe("Audio attempt $ failed on playback device "
        "enumeration (0x$$), retrying...", stIndex+1, hex, cOal->GetError());
    } // Problems, problems
    cLog->LogErrorSafe("Audio couldn't access playback devices list.");
  }
  /* -- Set distance model ----------------------------------------- */ public:
  void SetDistanceModel(const ALenum eModel) const
    { AL(cOal->SetDistanceModel(eModel),
        "Failed to set audio distance model!", "Model", eModel); }
  /* -- Set listener position ---------------------------------------------- */
  void SetPosition(const ALfloat fX, const ALfloat fY, const ALfloat fZ) const
  { // Set position
    AL(cOal->SetListenerPosition(fX, fY, fZ),
      "Failed to set audio listener position!", "X", fX, "Y", fY, "Z", fZ);
  }
  /* -- Set listener velocity ---------------------------------------------- */
  void SetVelocity(const ALfloat fX, const ALfloat fY, const ALfloat fZ) const
  { // Set position
    AL(cOal->SetListenerVelocity(fX, fY, fZ),
      "Failed to set audio velocity!", "X", fX, "Y", fY, "Z", fZ);
  }
  /* -- Set listener orientation ------------------------------------------- */
  void SetOrientation(const ALfloat fX1, const ALfloat fY1,
    const ALfloat fZ1, const ALfloat fX2, const ALfloat fY2,
    const ALfloat fZ2) const
  { // Build array
    const array<const ALfloat, 6> aOr{ fX1, fY1, fZ1, fX2, fY2, fZ2 };
    // Set orientation
    AL(cOal->SetListenerVectors(AL_ORIENTATION, aOr.data()),
      "Failed to set audio orientation!",
      "X1", fX1, "Y1", fY1, "Z1", fZ1, "X2", fX2, "Y2", fY2, "Z2", fZ2);
  }
  /* -- Get current playback device ---------------------------------------- */
  const string GetPlaybackDeviceById(const size_t stId) const
  { // bail if value out of range
    if(stId >= GetNumPlaybackDevices())
      XC("Specified audio playback device id out of range!",
        "Index", stId, "Count", GetNumPlaybackDevices());
    // Return device
    return dlPBDevices[stId];
  }
  /* -- Return devices ----------------------------------------------------- */
  const StrVector &GetPBDevices(void) { return dlPBDevices; }
  const StrVector &GetCTDevices(void) { return dlCTDevices; }
  /* -- Get device information --------------------------------------------- */
  size_t GetNumPlaybackDevices(void) const { return dlPBDevices.size(); }
  /* -- Get device information --------------------------------------------- */
  const string GetCaptureDevice(const size_t stId) const
  { // bail if value out of range
    if(stId >= GetNumCaptureDevices())
      XC("Specified audio capture device id out of range!",
        "Index", stId, "Count", GetNumCaptureDevices());
    // Return device
    return dlCTDevices[stId];
  }
  /* -- Get device information --------------------------------------------- */
  size_t GetNumCaptureDevices(void) const { return dlCTDevices.size(); }
  /* -- Send init signal --------------------------------------------------- */
  bool ReInit(void)
  { // Return if signal already set to re-initialise
    if(FlagIsSet(AF_REINIT)) return false;
    // Set the signal to re-init (it will get unset when re-initialised)
    FlagSet(AF_REINIT);
    // Send event to re-initialise audio
    cEvtMain->Add(EMC_AUD_REINIT);
    // Event sent
    return true;
  }
  /* -- Stop all sounds ---------------------------------------------------- */
  void Stop(void) { VideoStop(); StreamStop(); SampleStop(); }
  /* -- Init --------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if class already de-initialised
    if(IHNotDeInitialise()) return;
    // Log subsystem
    cLog->LogDebugSafe("Audio class shutting down...");
    // DeInit thread
    DeInitThread();
    // Unload all Stream, Sample and Source classes
    cStreams->CollectorDestroyUnsafe();
    cSamples->CollectorDestroyUnsafe();
    cSources->CollectorDestroyUnsafe();
    // Unload handles
    DeInitContext();
    // Re-Init members
    tpNextCheck = {};
    dlPBDevices.clear();
    dlCTDevices.clear();
    // Report error code
    cLog->LogDebugSafe("Audio class shutdown finished.");
  }
  /* -- Init --------------------------------------------------------------- */
  void Init(void)
  { // Class initialised
    IHInitialise();
    // Log subsystem
    cLog->LogDebugSafe("Audio class starting up...");
    // Init context and thread
    InitContext();
    InitThread();
    // Log status
    cLog->LogDebugSafe("Audio class started successfully.");
  }
  /* -- Default constructor ------------------------------------------------ */
  Audio(void) :                        // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    IHelper{ __FUNCTION__ },           // Initialise class name
    AudioFlags{ AF_NONE },             // Initialise no audio flags
    Thread{ "audio", STP_AUDIO,        // Initialise high perf audio thread
      bind(&Audio::AudioThreadMain,    // " with reference to callback
        this, _1) },                   // " function
    cdCheckRate{ seconds{ 0 } },       // Initialise thread check time
    cdDiscWait{ milliseconds{ 100 } }  // Initialise discrepency sleep time
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Audio, DeInit())         // Destructor helper
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Audio)               // Suppress default functions for safety
  /* ----------------------------------------------------------------------- */
  CVarReturn SetAudCheckRate(const unsigned int uiTime)
    { return CVarSimpleSetIntNLG(cdCheckRate,
        milliseconds{ uiTime }, milliseconds{ 100 }, milliseconds{ 60000 }); }
  /* ----------------------------------------------------------------------- */
  CVarReturn SetAudThreadDelay(const ALuint uiTime)
    { return CVarSimpleSetIntNG(cdThreadDelay,
        milliseconds{ uiTime }, milliseconds{ 1000 }); }
  /* -- Set global volume -------------------------------------------------- */
  CVarReturn SetGlobalVolume(const ALfloat fVolume)
  { // Ignore if invalid value
    if(fVolume < 0.0f || fVolume > 1.0f) return DENY;
    // Store volume
    cSources->fGVolume = fVolume;
    // Update volumes on streams and videos
    StreamCommitVolume();
    VideoCommitVolume();
    SampleUpdateVolume();
    // Done
    return ACCEPT;
  }
  /* -- End ---------------------------------------------------------------- */
} *cAudio = nullptr;                   // Pointer to static class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
