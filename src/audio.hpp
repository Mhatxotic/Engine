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
using namespace IClock::P;             using namespace ICVar::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IHelper::P;            using namespace ILog::P;
using namespace ILuaFunc::P;           using namespace IOal::P;
using namespace ISample::P;            using namespace ISource::P;
using namespace IStd::P;               using namespace IStream::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IThread::P;            using namespace IVideo::P;
using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ========================================================================= */
static class Audio final :             // Audio manager class
  /* -- Base classes ------------------------------------------------------- */
  private InitHelper,                  // Initialisation helper class
  private Thread                       // Audio monitoring thread
{ /* -- Monitoring thread timers ---------------------------------- */ private:
  const EvtMainRegVec emrvEvents;      // Frequently used events
  ClkTimePoint        tpNextCheck;     // Next check for hardware changes
  SafeClkDuration     cdCheckRate,     // Check rate
                      cdThreadDelay;   // Thread sleep time
  const ClkDuration   cdDiscWait;      // Sleep time waiting for main thread
  const CbThFunc      ctfThSysEvts,    // Thread function to use
                      ctfThNoSysEvts;  // Thread function to use
  /* -- Devices ------------------------------------------------------------ */
  StrVector        dlPBDevices,        // List of playback devices
                   dlCTDevices;        // List of capture devices
  /* -- References ------------------------------------------------- */ public:
  LuaFunc          lfOnUpdate;         // Fire this when device updates
  /* -- Playback device list was updated ----------------------------------- */
  void OnPbkDeviceUpdated(const EvtMainEvent &emeEvent)
  { // Update the device context if supplied
    cOal->UpdateDevice(reinterpret_cast<ALCdevice*>(emeEvent.aArgs[0].vp));
    // Refresh device list and update the name
    EnumeratePlaybackDevices();
    cOal->UpdatePlaybackDeviceName();
    // Send lua event with isplaybackdevice set to true
    lfOnUpdate.LuaFuncDispatch(true);
  }
  /* -- Capture device list was updated ------------------------------------ */
  void OnCapDeviceUpdated(const EvtMainEvent&)
  { // Re-enumerate capture devices
    EnumerateCaptureDevices();
    // Send lua event with isplaybackdevice set to true
    lfOnUpdate.LuaFuncDispatch(false);
  }
  /* -- Init thread -------------------------------------------------------- */
  void InitThread(void)
  { // Log that the thread is starting
    cLog->LogDebugSafe("Audio monitoring thread initialising...");
    // Start the minimal thread
    ThreadInit(cOal->FlagIsSet(AFL_HAVESEPBDDC) ?
      ctfThSysEvts : ctfThNoSysEvts, this);
    // Log completion of thread startup
    cLog->LogDebugExSafe("Audio monitoring thread initialised (D:$;C:$)!",
      StrShortFromDuration(ClockDurationToDouble(cdThreadDelay)),
      StrShortFromDuration(ClockDurationToDouble(cdCheckRate)));
  }
  /* -- DeInit thread ------------------------------------------------------ */
  void DeInitThread(void)
  { // Stop and de-init the thread and log progress
    cLog->LogDebugSafe("Audio monitoring thread de-initialising...");
    ThreadDeInit();
    cLog->LogDebugSafe("Audio monitoring thread de-initialised.");
  }
  /* -- Reset timer interval ----------------------------------------------- */
  void ResetCheckTime(void)
    { tpNextCheck = cmHiRes.GetTime() + cdCheckRate.load(); }
  /* -- ReInit requested --------------------------------------------------- */
  void OnReInit(const EvtMainEvent&)
  { // Capture exceptions
    try
    { // Log status
      cLog->LogDebugSafe("Audio class re-initialising...");
      // De-Init thread
      DeInitThread();
      // Unload all buffers for streams and samples and destroy all sources
      VideoDeInit();
      StreamDeInit();
      SourceDeInit();
      SampleDeInit();
      // Deinit and reinit context
      DeInitContext();
      InitContext();
      // Re-initialise volume levels
      SetGlobalVolume(cSources->fGVolume);
      SampleSetVolume(cSources->fSVolume);
      StreamSetVolume(cSources->fMVolume);
      VideoSetVolume(cSources->fVVolume);
      // Re-create all buffers for streams and samples
      SampleReInit();
      SourceReInit();
      StreamReInit();
      VideoReInit();
      // Init monitoring thread
      InitThread();
      // Send lua event with the device type not set
      lfOnUpdate.LuaFuncDispatch();
      // Log status
      cLog->LogInfoSafe("Audio class re-initialised successfully.");
    } // We don't want LUA to hard break really.
    catch(const exception &eReason)
    { // Log the exception first
      cLog->LogErrorExSafe("Audio re-init exception: $", eReason);
      // Reset next thread check time
      ResetCheckTime();
    } // Remove re-initialisation flag
    cOal->FlagClear(AFL_REINIT);
  }
  /* -- Thread main function with system events support -------------------- */
  int AudioThreadMainSysEvents(Thread &) try
  { // Loop forever until thread exit signalled.
    while(ThreadShouldNotExit())
    { // Manage stream classes
      StreamManage();
      // Suspend thread for the user requested time
      StdSuspend(cdThreadDelay.load());
    } // Terminate thread
    return 1;
  } // exception occured in this thread
  catch(const exception &eReason)
  { // Report error
    cLog->LogErrorExSafe("(AUDIO SE THREAD EXCEPTION) $", eReason);
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
  /* -- Thread main function with no system events support ----------------- */
  int AudioThreadMainNoSysEvents(Thread &) try
  { // Enumerate...
    for(ResetCheckTime();                 // Reset device list check time
        ThreadShouldNotExit();            // Enum until thread exit signalled
        StdSuspend(cdThreadDelay.load())) // Suspend thread pecified time
    { // Manage all streams audio.
      StreamManage();
      // Verify the hardware setup and reset if there are any descreprencies.
      // If there were no descreprencies detected then loop again.
      if(Verify()) continue;
      // Put in infinite loop and wait for the reinit function to request
      // termination of this thread
      while(ThreadShouldNotExit()) StdSuspend(cdDiscWait);
      // Thread terminate request recieved, now break the loop.
      break;
    } // Terminate thread
    return 1;
  } // exception occured in this thread
  catch(const exception &eReason)
  { // Report error
    cLog->LogErrorExSafe("(AUDIO NSE THREAD EXCEPTION) $", eReason);
    // Restart the thread
    return 0;
  }
  /* -- Event function (switch to this call) ------------------------------- */
  static void OnEvent(const ALCenum eEventType, const ALCenum eDeviceType,
    ALCdevice*const alcNDevice, const ALCsizei stLength,
    const ALCchar*const cpMessage, void*const)
  { // Create string view of message and log the event
    const string_view strvMsg{ cpMessage, static_cast<size_t>(stLength) };
    // Log event text to say we processed the event successfully.
    cLog->LogDebugExSafe(
      "Audio received system event $<0x$$> with device type $$<0x$$>...\n"
      "- $.", eEventType, hex, eEventType, dec, eDeviceType, hex,
              eDeviceType, strvMsg);
    // Send event to process the event
    switch(eDeviceType)
    { // It was a playback device?
      case ALC_PLAYBACK_DEVICE_SOFT:
        // Send playback devices updated event and break
        cEvtMain->Add(EMC_AUD_PDEVICE_UPDATED,
          reinterpret_cast<void*>(alcNDevice));
        break;
      // It was a capture device?
      case ALC_CAPTURE_DEVICE_SOFT:
        // Send capture devices updated event and break
        cEvtMain->Add(EMC_AUD_CDEVICE_UPDATED,
          reinterpret_cast<void*>(alcNDevice));
        break;
      // Unknown device?
      default: break;
    }
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
    if(stDevice != StdNPos)
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
    cEvtMain->RegisterEx(emrvEvents);
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
    cEvtMain->UnregisterEx(emrvEvents);
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
    for(size_t stIndex = 0; stIndex < 3; ++stIndex, StdSuspend())
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
    cLog->LogDebugExSafe("Audio found $ capture devices.",
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
    for(size_t stIndex = 0; stIndex < 3; ++stIndex, StdSuspend())
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
        cLog->LogDebugExSafe("Audio found $ playback devices.",
          GetNumPlaybackDevices());
        return;
      } // Report that the attempt failed
      cLog->LogWarningExSafe("Audio attempt $ failed on playback device "
        "enumeration (0x$$), retrying...", stIndex+1, hex, cOal->GetError());
    } // Problems, problems
    cLog->LogErrorSafe("Audio couldn't access playback devices list.");
  }
  /* -- Set distance model ------------------------------------------------- */
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
    if(cOal->FlagIsSet(AFL_REINIT)) return false;
    // Set the signal to re-init (it will get unset when re-initialised)
    cOal->FlagSet(AFL_REINIT);
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
    // Unload all Video, Stream, Sample and Source classes. All these should
    // already by zero size at this point but they all rely on this class so we
    // should make sure they're all loaded just incase.
    cVideos->CollectorDestroyUnsafe();
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
    // Enable the audio device change event
    AL(cOal->SetEventCallback(reinterpret_cast<ALCEVENTPROCTYPESOFT>(OnEvent),
      this), "Failed to set system event callback!");
    // Start the monitoring thread
    InitThread();
    // Log status
    cLog->LogDebugSafe("Audio class started successfully.");
  }
  /* -- Default constructor ------------------------------------------------ */
  Audio(void) :                        // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Initialise class name
    Thread{ "audio", STP_AUDIO },      // Initialise high perf audio thread
    emrvEvents{
      { EMC_AUD_REINIT,          bind(&Audio::OnReInit,  this, _1) },
      { EMC_AUD_PDEVICE_UPDATED, bind(&Audio::OnPbkDeviceUpdated, this, _1) },
      { EMC_AUD_CDEVICE_UPDATED, bind(&Audio::OnCapDeviceUpdated, this, _1) }
    },
    cdDiscWait{ milliseconds{ 100 } }, // Initialise discrepency sleep time
    ctfThSysEvts{ bind(&Audio::AudioThreadMainSysEvents, this, _1) },
    ctfThNoSysEvts{ bind(&Audio::AudioThreadMainNoSysEvents, this, _1) },
    lfOnUpdate{ "OnUpdate" }           // On update event
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Audio, DeInit())         // Destructor helper
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
