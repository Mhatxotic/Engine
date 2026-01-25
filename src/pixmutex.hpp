/* == PIXMUTEX.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This allows the engine to call POSIX functions to check for an      ## **
** ## already running instance of the engine and runs a specified         ## **
** ## callback so the caller can do more intimate operating system        ## **
** ## specific tasks which differ between Linux and MacOS.                ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
class SysMutex                         // Need this before of System init order
{ /* -- Shared memory ------------------------------------------------------ */
  Ident            idMutex;            // Mutex identifier
  const pid_t     &ptPId;              // Reference to process id
  pid_t           *pipProcessId;       // Process id (OS level memory)
  static constexpr size_t stPidSize = sizeof(pid_t); // Size of a pid
  typedef bool (CbFunc)(const pid_t,const pid_t); // Callback return type
  /* -- Initialise global mutex ------------------------------------ */ public:
  bool SysDoInitGlobalMutex(const string_view &strvTitle, CbFunc cbCb)
  { // Initialise mutex ident
    idMutex.IdentSet(strvTitle);
    // Shared memory file descriptor helper
    class Shm
    { /* -- Private variables ---------------------------------------------- */
      const string_view &strvTitle; // Filename of shm object
      int               iFd,        // The file descriptor of the shm object
                        iMode;      // Requested mode for the shm object
      /* -- Return mode -------------------------------------------- */ public:
      int Mode() const { return iMode; }
      /* -- Get file descriptor -------------------------------------------- */
      int Get() const { return iFd; }
      /* -- Resize file descriptor content --------------------------------- */
      bool Truncate(const size_t stSize) const
        { return !ftruncate(Get(), static_cast<off_t>(stSize)); }
      /* -- Close the file descriptor -------------------------------------- */
      bool Close()
      { // Return success if already closed
        if(Get() < 0) return true;
        // Do the close and return failure if failed
        if(close(Get())) return false;
        // Reset the file descriptor
        iFd = -1;
        // Success
        return true;
      }
      /* -- Open the file descriptor --------------------------------------- */
      bool Open(const int iNMode)
      { // Close previous file descriptor
        Close();
        // Set new mode
        iMode = iNMode;
        // Open and assign the new descriptor
        iFd = shm_open(strvTitle.data(), iMode, 0600);
        // Return if the open succeeded
        return Get() >= 0;
      }
      /* -- Destructor that closes the file descriptor --------------------- */
      ~Shm() { if(Get() >= 0) close(Get()); }
      /* -- Constructor that initialises variables ------------------------- */
      explicit Shm(const string_view &strvNTitle) :
        /* -- Initialisers ------------------------------------------------- */
        strvTitle(strvNTitle),         // Set reference to title
        iFd(-1),                       // File descriptor uninitialised
        iMode(0)                       // Mode uninitialised
        /* -- No code ------------------------------------------------------ */
        {}
      /* -- Initialise a single object that automatically cleans up -------- */
    } shmShm{ strvTitle };
    // Create the semaphore and if an error occurs?
    if(!shmShm.Open(O_CREAT | O_EXCL | O_RDWR))
    { // Report error if it isn't because the semaphore already exists
      if(StdIsNotError(EEXIST))
        XCL("Failed to setup shared memory object for exclusive writing!",
          "Name", strvTitle, "Mode", shmShm.Mode());
      // Try opening it again for reading with exclusivity
      if(!shmShm.Open(O_RDWR | O_EXCL))
        XCL("Failed to reopen setup shared memory object!",
          "Name", strvTitle, "Mode", shmShm.Mode());
      // Initialise memory
      pipProcessId = StdMMap<pid_t>
        (nullptr, stPidSize, PROT_READ, MAP_SHARED, shmShm.Get(), 0);
      if(pipProcessId == MAP_FAILED)
        XCL("Failed to setup shared memory for reading!",
          "Name", strvTitle, "ObjectFD", shmShm.Get(), "Size", stPidSize);
      // Test if the process exists and if it does, run the user callback
      const pid_t pPId = *pipProcessId;
      if(getpgid(pPId) >= 0)
      { // Put in log that another instance of this application is running and
        // return to caller that execution must cease.
        cLog->LogWarningExSafe("System detected another instance of this "
          "application running at pid $.", pPId);
        // Call the callback and return the result
        return cbCb(ptPId, pPId);
      } // Put in log that another instance of this application is running.
      cLog->LogWarningExSafe(
        "Previous pid of $ not valid so assuming no previous instance.", pPId);
      // Unmap previous shared memory
      if(munmap(pipProcessId, stPidSize))
        XCL("Failed to unmap shared memory from reading!", "Name", strvTitle);
      // Try reopening for exclusive writing again
      if(!shmShm.Open(O_RDWR | O_EXCL))
        XCL("Failed to setup shared memory object for exclusive writing!",
          "Name", strvTitle, "Mode", shmShm.Mode());
    } // Make sure it is the correct size
    if(!shmShm.Truncate(stPidSize) && StdIsNotError(EINVAL))
      XCL("Failed to truncate shared memory object!",
        "Name", strvTitle, "ObjectFD", shmShm.Get(), "Size", stPidSize);
    // Initialise memory
    pipProcessId = StdMMap<pid_t>(nullptr, stPidSize,
      PROT_READ | PROT_WRITE, MAP_SHARED, shmShm.Get(), 0);
    if(pipProcessId == MAP_FAILED)
      XCL("Failed to setup shared memory for writing!",
        "Name", strvTitle, "ObjectFD", shmShm.Get(), "Size", stPidSize);
    // Write the current PID to shared memory
    *pipProcessId = ptPId;
    // Execution can continue
    return true;
  }
  /* -- Constructor -------------------------------------------------------- */
  SysMutex(const pid_t &piNPId) :
    /* -- Initialisers ----------------------------------------------------- */
    ptPId(piNPId),                     // Set my process id
    pipProcessId(nullptr)              // Process id memory not available yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~SysMutex,
    // Unmap the memory containing the pid and reset the pid memory address
    if(pipProcessId && munmap(pipProcessId, stPidSize))
      cLog->LogWarningExSafe("System failed to unmap shared memory of size "
        "$ bytes! $", stPidSize, SysError());
    // Unlink the mutex and show warning in log if failed
    if(idMutex.IdentIsSet() && shm_unlink(idMutex.IdentGetData()))
      cLog->LogWarningExSafe("System could not delete old mutex '$': $",
        idMutex.IdentGet(), SysError());
  )
};/* ----------------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
