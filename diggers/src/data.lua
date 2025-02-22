-- DATA.LUA =============================================================== --
-- ooooooo.--ooooooo--.ooooo.-----.ooooo.--oooooooooo-oooooooo.----.ooooo..o --
-- 888'-`Y8b--`888'--d8P'-`Y8b---d8P'-`Y8b-`888'---`8-`888--`Y88.-d8P'---`Y8 --
-- 888----888--888--888---------888---------888--------888--.d88'-Y88bo.---- --
-- 888----888--888--888---------888---------888oo8-----888oo88P'---`"Y888o.- --
-- 888----888--888--888----oOOo-888----oOOo-888--"-----888`8b.--------`"Y88b --
-- 888---d88'--888--`88.---.88'-`88.---.88'-888-----o--888-`88b.--oo----.d8P --
-- 888bd8P'--oo888oo-`Y8bod8P'---`Y8bod8P'-o888ooood8-o888o-o888o-8""8888P'- --
-- ========================================================================= --
-- (c) Mhatxotic Design, 2025          (c) Millennium Interactive Ltd., 1994 --
-- ========================================================================= --
-- Menu data types array --------------------------------------------------- --
local MNU<const> = {                   -- Menu ids
  -- No menu selected                  -- Main digger menu
  NONE   = 0x01,                       MAIN   = 0x02,
  -- Digger movement menu              -- Digger digging menu
  MOVE   = 0x03,                       DIG    = 0x04,
  -- Digger drop item menu             -- Small and large tunneller menu
  DROP   = 0x05,                       TUNNEL = 0x06,
  -- Corkscrew menu                    -- Explosives menu
  CORK   = 0x07,                       TNT    = 0x08,
  -- Map menu                          -- Train for rails menu
  MAP    = 0x09,                       TRAIN  = 0x0A,
  -- Train drop menu                   -- Floating device movement menu
  TRDROP = 0x0B,                       FLOAT  = 0x0C,
  -- Flood gate menu                   -- Object deployment menu
  GATE   = 0x0D,                       DEPLOY = 0x0E,
  -- Lift control menu
  LIFT   = 0x0F,
};
-- Menu flags array -------------------------------------------------------- --
local MFL<const> = {    -- Menu flags array
  BUSY   = 0x01; -- Block action if object is busy
};
-- Actions ----------------------------------------------------------------- --
local ACT<const> = {    -- Object actions array
  HIDE    = 0x01, -- Object is invinsible
  STOP    = 0x02, -- Object is standing still
  CREEP   = 0x03, -- Object is creeping
  WALK    = 0x04, -- Object is walking
  RUN     = 0x05, -- Object is running
  DIG     = 0x06, -- Object is digging
  PHASE   = 0x07, -- Object is teleporting
  DEATH   = 0x08, -- Object is dead
  FIGHT   = 0x09, -- Object is fighting
  EATEN   = 0x0A, -- Object is eaten by an alien egg
  DYING   = 0x0B, -- Object is dying?
  KEEP    = 0x0C, -- Object should preserve the current action
  JUMP    = 0x0D, -- Object should jump
  GRAB    = 0x0E, -- Object should grab the nearest object
  DROP    = 0x0F, -- Object should drop the selected object
  PREV    = 0x10, -- Object should cycle to the previous held item
  NEXT    = 0x11, -- Object should cycle to the next held item
  REST    = 0x12, -- Object should rest at the trade-centre
  DEPLOY  = 0x13, -- Object should be deployed
  OPEN    = 0x14, -- Object should open (flood gate)
  CLOSE   = 0x15, -- Object should close (flood gate)
  MAP     = 0x16, -- Object should show TNT map
};
-- Jobs -------------------------------------------------------------------- --
local JOB<const> = {     -- Job data array
  NONE     = 0x1, -- No job
  BOUNCE   = 0x2, -- The object bounces in the opposite direction when blocked
  DIG      = 0x3, -- The object digs when it is blocked
  DIGDOWN  = 0x4, -- The object digs down when in centre of tile
  HOME     = 0x5, -- The object moves towards the home point
  INDANGER = 0x6, -- The object is in danger
  PHASE    = 0x7, -- The object is to teleport
  SEARCH   = 0x8, -- The object walks around and picks up treasure
  SPAWN    = 0x9, -- The object is spawning not teleporting (uses ACT_PHASE)
  KEEP     = 0xA, -- Preserve the current job (SETACTION() Command)
  REST     = 0xB, -- Object should rest
  KNDD     = 0xC, -- Preserve current job, but disallow JOB.DIGDOWN
};
-- Object types array ------------------------------------------------------ --
-- DO NOT MODIFY the order of these variables as it will screw up the level
-- data object data files.
-- ------------------------------------------------------------------------- --
local TYP<const> = {
  FTARG     = 0x00, -- F'Targ race digger
  HABBISH   = 0x01, -- Habbish race digger
  GRABLIN   = 0x02, -- Grablin race digger
  QUARRIOR  = 0x03, -- Quarrior race digger
  JENNITE   = 0x04, -- The most valuable treasure
  DIAMOND   = 0x05, -- The near-most valuable treasure
  GOLD      = 0x06, -- The un-common treasure
  EMERALD   = 0x07, -- The not so un-common treasure
  RUBY      = 0x08, -- The most common treasure
  PHANTOM   = 0x09, -- A fast moving random directional spinning monster
  SKELETON  = 0x0A, -- A slow moving skeleton that homes in on any digger
  ZOMBIE    = 0x0B, -- A fast moving skeleton that homes in on any digger
  GHOST     = 0x0C, -- A slow moving ghost that homes in on any digger
  ZIPPER    = 0x0D, -- A slow moving wormhole that transports diggers anywhere
  SWRLYPRT  = 0x0E, -- A fast moving wormhole that transports diggers anywhere
  PIRANA    = 0x0F, -- A plant that attacks diggers if they get near enough
  FUNGUS    = 0x10, -- A root plant that attacks diggers
  ALIEN     = 0x11, -- A mutated alien from a digger
  EGG       = 0x12, -- If digger touches this then   ALIEN is born
  BIRD      = 0x13, -- Critter. Just flies left and right
  FISH      = 0x14, -- Critter. Just swims left and right
  RAPTOR    = 0x15, -- A fast moving dinosour
  ROTARY    = 0x16, -- A slow moving dinosour
  STEGO     = 0x17, -- Main part of a slow moving stegosaurus
  STEGOB    = 0x18, -- Attachment part of TYP.STEGMAIN
  TURTLE    = 0x19, -- Turtle. Just swims left and right (Unused)
  BIGFOOT   = 0x1A, -- A fast moving = 0x intelligent monster (Unused)
  STUNNEL   = 0x1B, -- Small tunneler
  LTUNNEL   = 0x1C, -- Large tunneler
  LTUNNELB  = 0x1D, -- Attachment for large tunneler
  CORK      = 0x1E, -- Corkscrew (Vertical tunneler)
  TELEPOLE  = 0x1F, -- Telepole
  TNT       = 0x20, -- Explosives
  FIRSTAID  = 0x21, -- First aid kit
  MAP       = 0x22, -- Map that shows everything in the level
  TRACK     = 0x23, -- Track for train cart
  TRAIN     = 0x24, -- Train for track
  BRIDGE    = 0x25, -- Bridge piece
  BOAT      = 0x26, -- Inflatable boat
  GATE      = 0x27, -- Flood gate
  GATEB     = 0x28, -- Deployed flood gate
  LIFT      = 0x29, -- Elevator
  LIFTB     = 0x2A, -- Deployed elevator
  LIFTC     = 0x2B, -- Deployed elevator attachment
  CAMPFIRE  = 0x2C, -- Campfire
  MAX       = 0x2D, -- Maximum
  DIGRANDOM = 0xFF  -- For LoadLevel(). Select a random race
};
-- Races available list ---------------------------------------------------- --
local aRacesData<const> =
  { TYP.FTARG, TYP.GRABLIN, TYP.HABBISH, TYP.QUARRIOR };
-- Race data --------------------------------------------------------------- --
local aRaceStatData<const> = {
  -- Object id -- STR STA DSP PAT ATP TEL SPC ------------------------------ --
  { TYP.FTARG,    25, 50, 37, 32, 35, 42,  0 }, -- These are just values for
  { TYP.HABBISH,  42, 45, 27, 25, 42, 50,  1 }, -- drawing. The actual values
  { TYP.GRABLIN,  37, 35, 50, 35, 27, 38, -1 }, -- are in the actual object
  { TYP.QUARRIOR, 50, 27, 20, 50, 48, 25, -1 }, -- data structures.
};
-- Shop data statics ------------------------------------------------------- --
local aShopData<const> = {
  TYP.GATE, TYP.TRAIN, TYP.LIFT, TYP.TNT, TYP.LTUNNEL, TYP.CORK, TYP.MAP,
  TYP.BOAT, TYP.BRIDGE, TYP.STUNNEL, TYP.TRACK, TYP.TELEPOLE, TYP.FIRSTAID
};
-- Cursors ----------------------------------------------------------------- --
local CID<const> = {
  -- Arrow cursor                      Cursor with OK text
  ARROW     = 0x1,                     OK        = 0x2,
  -- Cursor with EXIT text             Cursor with LEFT scroll arrow
  EXIT      = 0x3,                     LEFT      = 0x4,
  -- Cursor with TOP scroll arrow      Cursor with RIGHT scroll arrow
  TOP       = 0x5,                     RIGHT     = 0x6,
  -- Cursor with BOTTOM scroll arrow   Cursor over selectable element
  BOTTOM    = 0x7,                     SELECT    = 0x8,
  -- Cursor loading                    No cursor
  WAIT      = 0x9,                     NONE      = 0xA,
  -- Maximum number of cursors
  MAX       = 0xB
};
-- Cursor data ------------------------------------------------------------- --
local aCursorData<const> = {
  -- CID ------- S - C - X  Y ---- CID ------- S - C - X  Y ---
  [CID.ARROW] ={ 992,  1,-3, -3}, [CID.OK]    ={1005,  4, -7,-8},
  [CID.EXIT]  ={1001,  4,-7, -8}, [CID.LEFT]  ={ 997,  1, -2,-7},
  [CID.TOP]   ={ 998,  1,-7, -2}, [CID.RIGHT] ={ 999,  1,-13,-7},
  [CID.BOTTOM]={1000,  1,-7,-13}, [CID.SELECT]={ 993,  4, -7,-7},
  [CID.WAIT]  ={ 960,  8,-7, -7}, [CID.NONE]  ={1009,  1,  0, 0},
  -- CID ------- S - C - X  Y ---- CID ------- S - C - X  Y ---
};
-- Object flags ------------------------------------------------------------ --
local OFL<const> = {          -- Max 64-bits
  -- Generic object flags -------------------------------------------------- --
  NONE         = 0x000000000, -- Object has no flags
  RESPAWN      = 0x000000001, -- Object respawns where it was created
  BUSY         = 0x000000002, -- Object is busy and commands disabled
  FALL         = 0x000000004, -- Object should fall
  LIVING       = 0x000000008, -- Object is a living object
  ENEMY        = 0x000000010, -- Object is an enemy
  DELICATE     = 0x000000020, -- Object is delicate (takes more damage)
  INWATER      = 0x000000040, -- Object is in water
  PHASETARGET  = 0x000000080, -- Object is a valid random phase target
  SOUNDLOOP    = 0x000000100, -- Object sound looped when sprite anim is reset
  NOANIMLOOP   = 0x000000200, -- Object is not allowed to loop its animation
  DIGGER       = 0x000000400, -- Object is a digger
  REGENERATE   = 0x000000800, -- Object can regenerate health?
  TPMASTER     = 0x000001000, -- Object is master at teleporting
  IMPATIENT    = 0x000002000, -- Object is a digger and becoming impatient
  JUMPFALL     = 0x000004000, -- Object is falling (while jumping)
  JUMPRISE     = 0x000008000, -- Object is jumping
  EXPLODE      = 0x000010000, -- Object explodes on death
  FLOAT        = 0x000020000, -- Object floats in water
  FLOATING     = 0x000040000, -- Object is floating right now
  HURTDIGGER   = 0x000080000, -- Object hurts diggers
  PHASEDIGGER  = 0x000100000, -- Object teleports diggers anywhere
  PICKUP       = 0x000200000, -- Object can be picked up
  PURSUEDIGGER = 0x000400000, -- Object follows a digger when colliding
  RNGSPRITE    = 0x000800000, -- Object selects a random sprite in animation.
  SELLABLE     = 0x001000000, -- Object is sellable to shop
  STATIONARY   = 0x002000000, -- Object does not move and is stationary
  TRACK        = 0x004000000, -- Object can only move on tracks
  TREASURE     = 0x008000000, -- Object is treasure
  DANGEROUS    = 0x010000000, -- Object is dangerous and diggers run away
  WATERBASED   = 0x020000000, -- Object is water based
  AQUALUNG     = 0x040000000, -- Object can breathe in water
  BLOCK        = 0x080000000, -- Object is a platform for diggers
  DEVICE       = 0x100000000, -- Object is a device
  HEALNEARBY   = 0x200000000, -- Object heals nearby Diggers
  CONSUME      = 0x400000000, -- Object consumes another object
  NOHOME       = 0x800000000, -- Object cannot enter home
};
OFL.JUMPMASK = OFL.JUMPRISE|OFL.JUMPFALL;
-- Jumping ----------------------------------------------------------------- --
local aJumpRiseData<const> =
  { -2, -2, -2, -1, -1, -1, -1,    -1, -1, -1,  0, -1,  0,  0, -1,  0,
     0,  0, -1,  0,  0,  0,  0 };
local aJumpFallData<const> =
  {  0,  0,  0,  0,  0,  0,  1,     0,  0,  1,  0,  1,  0,  1,  1,  1,
     1,  1,  1,  1,  2,  2,  2 };
-- Directions -------------------------------------------------------------- --
local DIR<const> = {
  UL       = 0x01, -- Move left and dig up-left diagonally
  U        = 0x02, -- Up direction but not used
  UR       = 0x03, -- Move right and dig up-right diagonally
  L        = 0x04, -- Move left and dig left
  NONE     = 0x05, -- No direction
  R        = 0x06, -- Move right and dig right
  DL       = 0x07, -- Move left and dig down-left diagnoally
  D        = 0x08, -- Dig down
  DR       = 0x09, -- Move right and dig down-right diagonally
  LR       = 0x0A, -- Go left or right
  UD       = 0x0B, -- Go up or down
  OPPOSITE = 0x0C, -- Move in the opposite direction (SETACTION() Command)
  KEEP     = 0x0D, -- Preserve the current direction (SETACTION() Command)
  KEEPMOVE = 0x0E, -- Keep direction if moving or move in random direction
  HOME     = 0x0F, -- Direction to the player's home point (SETACTION() Cmd)
  TCTR     = 0x10, -- Move into the centre of the tile (SETACTION() Command)
  RNG      = 0x100 -- Randomise the sprite tile (FLAG!)
};
-- Actions when blocked ---------------------------------------------------- --
local aDigBlockData<const> =
{ -- Job             Action    Job         Direction
  [JOB.NONE]     = { ACT.STOP, JOB.NONE,   DIR.NONE     }, -- No job
  [JOB.BOUNCE]   = { ACT.KEEP, JOB.BOUNCE, DIR.OPPOSITE }, -- Bounce
  [JOB.DIG]      = { ACT.DIG,  JOB.DIG,    DIR.KEEP     }, -- Digging
  [JOB.DIGDOWN]  = { ACT.STOP, JOB.NONE,   DIR.NONE     }, -- Digging down
  [JOB.HOME]     = { ACT.STOP, JOB.NONE,   DIR.NONE     }, -- Going home
  [JOB.INDANGER] = { ACT.WALK, JOB.BOUNCE, DIR.OPPOSITE }, -- Danger
  [JOB.PHASE]    = { ACT.STOP, JOB.NONE,   DIR.NONE     }, -- Other jobs
  [JOB.SEARCH]   = { ACT.KEEP, JOB.SEARCH, DIR.OPPOSITE }, -- Searching
  [JOB.SPAWN]    = { ACT.STOP, JOB.NONE,   DIR.NONE     }, -- Spawning
  [JOB.KEEP]     = { ACT.STOP, JOB.NONE,   DIR.NONE     }, -- Keeping job
  [JOB.REST]     = { ACT.STOP, JOB.NONE,   DIR.NONE     }, -- Resting
  [JOB.KNDD]     = { ACT.STOP, JOB.NONE,   DIR.NONE     }, -- Preserve
};
-- Treasure spawning table ------------------------------------------------- --
local aDigTileData<const> = { TYP.EMERALD, TYP.RUBY, TYP.DIAMOND, TYP.GOLD };
-- Treasure to UI status symbol tile ids ----------------------------------- --
local aObjToUIData<const> = {
  [TYP.RUBY]    = 846, [TYP.EMERALD] = 845, [TYP.GOLD]    = 844,
  [TYP.DIAMOND] = 843, [TYP.JENNITE] = 842
};
-- AI Types ---------------------------------------------------------------- --
local AI<const> = {
  NONE        = 0x00, -- Object has no AI
  DIGGER      = 0x01, -- Object is a digger and does many things
  PATIENCE    = 0x02, -- Object is a digger controlled by the player
  CRITTER     = 0x03, -- Object does nothing but go left and right
  CRITTERSLOW = 0x04, -- Object does nothing but go left and right slowly
  FIND        = 0x05, -- Object speedily homes in on a digger
  FINDSLOW    = 0x06, -- Object slowly homes in on a digger
  RANDOM      = 0x07, -- Object moves in 4 directions finding a digger
  BIGFOOT     = 0x08, -- Object moves around like a digger and steals items
  TUNNELER    = 0x09, -- Object moves around and tunnels.
  CORKSCREW   = 0x0A, -- Object moves left and right and digs down.
  EXPLODER    = 0x0B, -- Object explodes when the enemy comes near.
  TRAIN       = 0x0C, -- Object moves left/right and picks up treasure.
  LIFT        = 0x0D, -- Object moves up/down
  BOAT        = 0x0E, -- Object moves left and right slowly on water.
  DEPLOY      = 0x0F, -- Object should deploy at a random time
  GATE        = 0x10, -- Object should automatically open and close
};
-- AI decisions data for SetRandomJob() ------------------------------------ --
local aAIChoicesData<const> = {
  -- Performs the specified action, job and direction with SetAction() if the
  -- AI Diggers' digging was blocked in the following directions...
  { DIR.L,  { ACT.WALK, JOB.DIGDOWN, DIR.TCTR },   -- [1] = Failed direction?
            { ACT.WALK, JOB.DIG,     DIR.L    } }, -- [2] = Blocked action
  { DIR.R,  { ACT.WALK, JOB.DIGDOWN, DIR.TCTR },   -- [3] = Unblocked action
            { ACT.WALK, JOB.DIG,     DIR.R    } },
  { DIR.UL, { ACT.WALK, JOB.DIG,     DIR.L    },
            { ACT.WALK, JOB.DIG,     DIR.UL   } },
  { DIR.UR, { ACT.WALK, JOB.DIG,     DIR.R    },
            { ACT.WALK, JOB.DIG,     DIR.UR   } },
  { DIR.DL, { ACT.WALK, JOB.DIG,     DIR.L    },
            { ACT.WALK, JOB.DIG,     DIR.DL   } },
  { DIR.DR, { ACT.WALK, JOB.DIG,     DIR.R    },
            { ACT.WALK, JOB.DIG,     DIR.DR   } },
  { DIR.L,  { ACT.WALK, JOB.DIGDOWN, DIR.TCTR },
            { ACT.WALK, JOB.SEARCH,  DIR.L    } },
  { DIR.R,  { ACT.WALK, JOB.DIGDOWN, DIR.TCTR },
            { ACT.WALK, JOB.SEARCH,  DIR.R    } },
  { DIR.D,  { ACT.WALK, JOB.DIG,     DIR.LR   },
            { ACT.WALK, JOB.DIGDOWN, DIR.TCTR } },
};
-- Timers and animation consts --------------------------------------------- --
local aTimerData<const> = {
  ANIMTERRAIN   =     10, -- Game ticks to wait before animating next terrain
  ANIMNORMAL    =      8, -- Delay before anim next sprite for normal objects
  ANIMFAST      =      2, -- Delay before anim next sprite for fast objects
  PICKUPDELAY   =     10, -- Game ticks to wait before checking for pickups
  DEADWAIT      =    600, -- Wait time before removing ACT_DEATH objects
  MUTATEWAIT    =   3600, -- Wait time before full digger mutation occurs
  DANGERTIMEOUT =    600, -- Timeout before removing the danger flag
  TARGETTIME    =   1800, -- Timeout before selecting another target (AI)
  GEMCHANCE     =  0.025, -- Chance of getting a gem after each dig. (2.5%)
  GEMDEPTHEXTRA =   1024, -- Chance to double chance based on this depth
  ROAMDIRCHANGE = 0.0025, -- Chance of roaming AI switching direction
};
-- Sound id's array -------------------------------------------------------- --
local aSfxData<const> = {
  -- Click sound                       F'Targ death sound
  CLICK = 0x01,                        DIEFTAR = 0x02,
  -- Grablin death sound               Habbish death sound
  DIEGRAB = 0x03,                      DIEHABB = 0x04,
  -- Quarrior death sound              Digging sound
  DIEQUAR = 0x05,                      DIG = 0x06,
  -- Mouse click command failed        TNT explosion sound
  ERROR = 0x07,                        EXPLODE = 0x08,
  -- Treasure found                    Gate close
  FIND = 0x09,                         GCLOSE = 0x0A,
  -- Gate open                         Jump sound
  GOPEN = 0x0B,                        JUMP = 0x0C,
  -- Kick sound                        Phase teleport sound
  KICK = 0x0D,                         PHASE = 0x0E,
  -- Punch sound                       Select digger sound
  PUNCH = 0x0F,                        SELECT = 0x10,
  -- Trade complete sound              Shop open sound
  TRADE = 0x11,                        SOPEN = 0x12,
  -- Holoemitter hum                   Shop item select
  HOLOHUM = 0x13,                      SSELECT = 0x14,
  -- Maximum number of sound effects
  MAX = 0x15
};
-- Shroud data ------------------------------------------------------------- --
local iSH<const>, iCL<const> = 1022, 953;
-- Top row tiles (Left, Middle1, Middle2, Right)
local iTL<const>, iTA<const>, iTB<const>, iTR<const> = 892, 893, 894, 895;
-- Middle row tiles (Left1, Left2, Right1, Right2)
local iLA<const>, iLB<const>, iRA<const>, iRB<const> = 924, 925, 926, 927;
-- Bottom row tiles (Left, Middle1, Middle2, Right)
local iBL<const>, iBA<const>, iBB<const>, iBR<const> = 956, 957, 958, 959;
-- Edges (Top-left, top-right, bottom-left, bottom-right)
local iETL<const>, iETR<const>, iEBL<const>, iEBR<const> = 922, 923, 891, 890;
-- Diagonal fillers
local iDLR<const>,iULR<const> = 954, 955;
-- Transparency exposed edges flags
local ixTL<const>, ixTR<const> = 0x1,       0x2;
local ixBL<const>, ixBR<const> = 0x4,       0x8;
local ixL<const>,  ixT<const>  = ixTL|ixBL, ixTL|ixTR;
local ixR<const>,  ixB<const>  = ixTR|ixBR, ixBL|ixBR;
local isTL<const>, isTR<const> = ixTR|ixB,  ixTL|ixB;
local isBL<const>, isBR<const> = ixT|ixBR,  ixT|ixBL;
local ixA<const> = ixTL|ixTR|ixBL|ixBR;
-- Lookup table for tile values -------------------------------------------- --
local aShroudTileLookup<const> = {
  {iSH},     --[[ 00-0-01 Shrouded  --]] {iBR},     -- 01-1-02 ixTL
  {iBL},     --[[ 02-2-03 ixTR      --]] {iBA,iBB}, -- 03-3-04 ixT
  {iTR},     --[[ 04-4-05 ixBL      --]] {iRA,iRB}, -- 05-5-06 ixL
  {iULR},    --[[ 06-6-07 ixTR|ixBL --]] {iEBR},    -- 07-7-08 isBR
  {iTL},     --[[ 08-8-09 ixBR      --]] {iDLR},    -- 09-9-10 ixTL|ixBR
  {iLA,iLB}, --[[ 10-A-11 ixR       --]] {iEBL},    -- 11-B-12 isBL
  {iTA,iTB}, --[[ 12-C-13 ixB       --]] {iETL},    -- 13-D-14 isTL
  {iETR},    --[[ 14-E-15 isTR      --]] {iCL}      -- 15-F-16 ixA
};
-- Shroud circle template for removal around selected Digger --------------- --
local aShroudCircle<const> = {
  -- { XAdjust, YAdjust, ShroudFlags }
  {-2,-5,ixBR},{-1,-5,ixB },{ 0,-5,ixB },{ 1,-5,ixB },{ 2,-5,ixBL},
  {-4,-4,ixBR},{-3,-4,ixB },{-2,-4,isTL},{-1,-4,ixA },{ 0,-4,ixA },
  { 1,-4,ixA },{ 2,-4,isTR},{ 3,-4,ixB },{ 4,-4,ixBL},{-5,-3,ixBR},
  {-4,-3,isTL},{-3,-3,ixA },{-2,-3,ixA },{-1,-3,ixA },{ 0,-3,ixA },
  { 1,-3,ixA },{ 2,-3,ixA },{ 3,-3,ixA },{ 4,-3,isTR},{ 5,-3,ixBL},
  {-5,-2,ixR },{-4,-2,ixA },{-3,-2,ixA },{-2,-2,ixA },{-1,-2,ixA },
  { 0,-2,ixA },{ 1,-2,ixA },{ 2,-2,ixA },{ 3,-2,ixA },{ 4,-2,ixA },
  { 5,-2,ixL },{-5,-1,ixR },{-4,-1,ixA },{-3,-1,ixA },{-2,-1,ixA },
  {-1,-1,ixA },{ 0,-1,ixA },{ 1,-1,ixA },{ 2,-1,ixA },{ 3,-1,ixA },
  { 4,-1,ixA },{ 5,-1,ixL },{-5, 0,ixR },{-4, 0,ixA },{-3, 0,ixA },
  {-2, 0,ixA },{-1, 0,ixA },{ 0, 0,ixA },{ 1, 0,ixA },{ 2,-0,ixA },
  { 3, 0,ixA },{ 4, 0,ixA },{ 5, 0,ixL },{-5, 1,ixR },{-4, 1,ixA },
  {-3, 1,ixA },{-2, 1,ixA },{-1, 1,ixA },{ 0, 1,ixA },{ 1, 1,ixA },
  { 2, 1,ixA },{ 3, 1,ixA },{ 4, 1,ixA },{ 5, 1,ixL },{-5, 2,ixR },
  {-4, 2,ixA },{-3, 2,ixA },{-2, 2,ixA },{-1, 2,ixA },{ 0, 2,ixA },
  { 1, 2,ixA },{ 2, 2,ixA },{ 3, 2,ixA },{ 4, 2,ixA },{ 5, 2,ixL },
  {-5, 3,ixTR},{-4, 3,isBL},{-3, 3,ixA },{-2, 3,ixA },{-1, 3,ixA },
  { 0, 3,ixA },{ 1, 3,ixA },{ 2, 3,ixA },{ 3, 3,ixA },{ 4, 3,isBR},
  { 5, 3,ixTL},{-4, 4,ixTR},{-3, 4,ixT },{-2, 4,isBL},{-1, 4,ixA },
  { 0, 4,ixA },{ 1, 4,ixA },{ 2, 4,isBR},{ 3, 4,ixT },{ 4, 4,ixTL},
  {-2, 5,ixTR},{-1, 5,ixT },{ 0, 5,ixT },{ 1, 5,ixT },{ 2, 5,ixTL}
};
-- Level data types -------------------------------------------------------- --
local aLevelTypeDesert<const>   =
  -- Type  Filename    Name             Shroud colour (r[1],g[2],b[3])
  { i=0, f="desert",   n="DESERTOUS",   s={ 170/0xFF, 102/0xFF,  81/0xFF } };
local aLevelTypeGrass<const>    =
  { i=1, f="grass",    n="TEMPERATE",   s={ 128/0xFF,  67/0xFF,  49/0xFF } };
local aLevelTypeIslands<const>  =
  { i=2, f="islands",  n="COASTAL",     s={ 188/0xFF,  87/0xFF,   0/0xFF } };
local aLevelTypeJungle<const>   =
  { i=3, f="jungle",   n="TROPICAL",    s={  41/0xFF,   6/0xFF,   0/0xFF } };
local aLevelTypeMountain<const> =
  { i=4, f="mountain", n="MOUNTAINOUS", s={ 204/0xFF, 102/0xFF, 102/0xFF } };
local aLevelTypeRock<const>     =
  { i=5, f="rock",     n="BARRENOUS",   s={ 116/0xFF,  52/0xFF,  35/0xFF } };
local aLevelTypeWinter<const>   =
  { i=6, f="snow",     n="WINTEROUS",   s={ 102/0xFF, 102/0xFF, 153/0xFF } };
-- Level data types array -------------------------------------------------- --
local aLevelTypesData<const> = {
  aLevelTypeDesert,   aLevelTypeGrass, aLevelTypeIslands, aLevelTypeJungle,
  aLevelTypeMountain, aLevelTypeRock,  aLevelTypeWinter,
};
-- Skill levels ------------------------------------------------------------ --
local aSkillLevels<const> = {
  { r= 600, n="VERY EASY" }, { r=1000, n="EASY" }, { r=1300, n="AVERAGE" },
  { r=1500, n="HARD" }, { r=2100, n="VERY HARD" }
};
local aSkillEasiest<const>, aSkillEasy<const>,
      aSkillMedium<const>,  aSkillHard<const>,
      aSkillHardest<const> =
        aSkillLevels[1], aSkillLevels[2],
        aSkillLevels[3], aSkillLevels[4],
        aSkillLevels[5];
-- Level data types array -------------------------------------------------- --
local aLevelsData<const> =
{ -- Name --------- WinReq - Filename ----- Terrain type ------------------- --
  { n="AZERG",     w=aSkillEasiest,  f="azerg",     t=aLevelTypeRock     },--01
  { n="DHOBBS",    w=aSkillEasiest,  f="dhobbs",    t=aLevelTypeGrass    },--02
  { n="ELEEVATE",  w=aSkillEasiest,  f="eleevate",  t=aLevelTypeGrass    },--03
  { n="DEENA",     w=aSkillEasiest,  f="deena",     t=aLevelTypeGrass    },--04
  { n="JUHSTYN",   w=aSkillEasiest,  f="juhstyn",   t=aLevelTypeIslands  },--05
  { n="FUJALE",    w=aSkillEasy,     f="fujale",    t=aLevelTypeGrass    },--06
  { n="HAEWOULD",  w=aSkillEasiest,  f="haewould",  t=aLevelTypeGrass    },--07
  { n="SAIRRUHR",  w=aSkillEasy,     f="sairruhr",  t=aLevelTypeJungle   },--08
  { n="TRARGHE",   w=aSkillEasiest,  f="trarghe",   t=aLevelTypeJungle   },--09
  { n="KURVELYNN", w=aSkillMedium,   f="kurvelynn", t=aLevelTypeMountain },--10
  { n="SQUEEK",    w=aSkillMedium,   f="squeek",    t=aLevelTypeJungle   },--11
  { n="MYKEBORL",  w=aSkillMedium,   f="mykeborl",  t=aLevelTypeDesert   },--12
  { n="ZORLYACK",  w=aSkillMedium,   f="zorlyack",  t=aLevelTypeRock     },--13
  { n="FTARGUS",   w=aSkillMedium,   f="ftargus",   t=aLevelTypeRock     },--14
  { n="TRAISA",    w=aSkillMedium,   f="traisa",    t=aLevelTypeDesert   },--15
  { n="KLINDYKE",  w=aSkillMedium,   f="klindyke",  t=aLevelTypeMountain },--16
  { n="EEANZONE",  w=aSkillMedium,   f="eeanzone",  t=aLevelTypeGrass    },--17
  { n="CHYEISHIR", w=aSkillMedium,   f="chyeishir", t=aLevelTypeGrass    },--18
  { n="DJENNEEE",  w=aSkillMedium,   f="djenneee",  t=aLevelTypeWinter   },--19
  { n="DWINDERA",  w=aSkillMedium,   f="dwindera",  t=aLevelTypeWinter   },--20
  { n="TWANG",     w=aSkillMedium,   f="twang",     t=aLevelTypeJungle   },--21
  { n="HABBARD",   w=aSkillMedium,   f="habbard",   t=aLevelTypeMountain },--22
  { n="BENJAR",    w=aSkillMedium,   f="benjar",    t=aLevelTypeDesert   },--23
  { n="SHRUBREE",  w=aSkillHard,     f="shrubree",  t=aLevelTypeJungle   },--24
  { n="BAROK",     w=aSkillHard,     f="barok",     t=aLevelTypeIslands  },--25
  { n="MUHLAHRD",  w=aSkillHard,     f="muhlahrd",  t=aLevelTypeDesert   },--26
  { n="CHONSKEE",  w=aSkillHard,     f="chonskee",  t=aLevelTypeRock     },--27
  { n="PURTH",     w=aSkillHard,     f="purth",     t=aLevelTypeMountain },--28
  { n="ANKH",      w=aSkillHard,     f="ankh",      t=aLevelTypeWinter   },--29
  { n="ZELIOS",    w=aSkillHardest,  f="zelios",    t=aLevelTypeWinter   },--30
  { n="FRUER",     w=aSkillHardest,  f="fruer",     t=aLevelTypeIslands  },--31
  { n="KLARSH",    w=aSkillHardest,  f="klarsh",    t=aLevelTypeJungle   },--32
  { n="SUHMNER",   w=aSkillHardest,  f="suhmner",   t=aLevelTypeIslands  },--33
  { n="SIMTOB",    w=aSkillHardest,  f="simtob",    t=aLevelTypeJungle   },--34
  -- Name --------- WinReq ----- Map name ------------ Map objects name --- --
};
-- Map data lookup table -------------------------------------------------- --
local aZoneData<const> = {
  -- Legend --------------------------------------------------------------- --
  -- L = Left bound on zone map.       T = Top bound on zone map.
  -- R = Right bound on zone map.      B = Bottom bound on zone map.
  -- DEPENDENCY = Table of zone ids that unlock when any are completed.
  -- L -- T -- R -- B -- DEPENDENCY -------------------- ZONE ------- (1-10) --
  { 188,  50, 245,  78, {                        } }, -- [01] Azerg
  { 106,  63, 172,  90, {                        } }, -- [02] Dhobbs
  {  48, 137,  92, 167, {  2,  4,  5             } }, -- [03] Eleevate
  {  47, 192,  95, 245, {  3,  5,  9             } }, -- [04] Deena
  {  96, 154, 138, 185, {  2,  3,  4,  6,  8,  9 } }, -- [05] Justyn
  { 145, 133, 221, 148, {  5,  7,  8, 11         } }, -- [06] Fujale
  { 164,  88, 230, 112, {  1,  2,  6, 12         } }, -- [07] Haeward
  { 148, 167, 208, 189, {  5,  6,  9, 10, 11,    } }, -- [08] Sairruhr
  { 110, 194, 155, 238, {  4,  5,  8             } }, -- [09] Traaghe
  { 200, 206, 251, 236, {  8,  9, 11, 16, 17, 18 } }, -- [10] Kurvelynn
  -- L -- T -- R -- B -- DEPENDENCY -------------------- ZONE ------ (11-20) --
  { 225, 137, 264, 171, {  6,  8, 10, 12, 16     } }, -- [11] Squeek
  { 247, 110, 315, 132, {  7, 11, 13, 15, 16     } }, -- [12] Mykeborl
  { 286,  75, 336, 104, { 12, 14, 15             } }, -- [13] Zorlyack
  { 348,  63, 397,  93, { 13, 15, 23, 24         } }, -- [14] Ftargus
  { 329, 118, 371, 157, { 12, 13, 14, 21, 22, 23 } }, -- [15] Traisa
  { 271, 152, 306, 196, { 10, 11, 12, 13, 17, 21 } }, -- [16] Klindyke
  { 259, 210, 319, 236, { 10, 16, 19, 21         } }, -- [17] Eeanzone
  { 197, 250, 248, 283, { 10, 19                 } }, -- [18] Chysshir
  { 255, 251, 320, 279, { 17, 18, 20             } }, -- [19] Djennee
  { 337, 236, 391, 290, { 19, 21, 22, 28         } }, -- [20] Dwindera
  -- L -- T -- R -- B -- DEPENDENCY -------------------- ZONE ------ (21-30) --
  { 317, 165, 355, 200, { 15, 16, 17, 20, 22     } }, -- [21] Twang
  { 362, 168, 396, 216, { 15, 21, 20, 23, 27     } }, -- [22] Habbard
  { 381, 105, 419, 153, { 14, 15, 22, 24, 26     } }, -- [23] Benjarr
  { 412,  57, 467,  94, { 14, 23, 25, 26         } }, -- [24] Shrubree
  { 478,  56, 553,  94, { 24, 26, 33             } }, -- [25] Barok
  { 431, 129, 491, 163, { 23, 24, 25, 27, 32, 33 } }, -- [26] Muhlahrd
  { 414, 175, 459, 206, { 22, 26, 28, 31, 32     } }, -- [27] Chonskee
  { 406, 218, 453, 259, { 20, 27, 29, 31         } }, -- [28] Purth
  { 440, 266, 509, 286, { 28, 30, 31             } }, -- [29] Ankh
  { 518, 256, 578, 297, { 29, 31                 } }, -- [30] Zelios
  -- L -- T -- R -- B -- DEPENDENCY -------------------- ZONE ------ (31-40) --
  { 465, 214, 550, 243, { 27, 28, 29, 30, 32     } }, -- [31] Fruer
  { 491, 176, 549, 200, { 27, 31, 33             } }, -- [32] Klarsh
  { 516, 109, 569, 163, { 25, 26, 32             } }, -- [33] Suhmner
  { 564,  43, 589,  65, { 25                     } }, -- [34] Simtob
};-- L -- T -- R -- B -- DEPENDENCY -------------------- ZONE -------------- --
-- ------------------------------------------------------------------------- --
-- The following tables are used in the 'aObjectData' table below and specify
-- that these actions, jobs and directions are accepted for that particular
-- object when pressing certain keyboard keys.
-- Left and right direction key supported ---------------------------------- --
local aObjectLeftRight<const> = { [DIR.L] = true, [DIR.R] = true };
-- No job but moving left and right key supported -------------------------- --
local aObjectMove<const> = { [JOB.NONE] = aObjectLeftRight };
-- No job an no direction key supported ------------------------------------ --
local aObjectStop<const> = { [JOB.NONE] = { [DIR.NONE] = true } };
-- Keep job and direction keys supported ----------------------------------- --
local aObjectJobDirKeep<const> = { [JOB.KEEP] = { [DIR.KEEP] = true } };
-- Search for treasure keys supported -------------------------------------- --
local aObjectSearch<const> = { [DIR.LR] = true };
-- Digger walk or run movement keys supported ------------------------------ --
local aDiggerMovement<const> = {
  [JOB.NONE]    = aObjectLeftRight,
  [JOB.DIG]     = { [DIR.UL] = true, [DIR.UR] = true, [DIR.L] = true,
                    [DIR.R] = true, [DIR.DL] = true, [DIR.DR] = true },
  [JOB.HOME]    = { [DIR.HOME] = true },
  [JOB.SEARCH]  = aObjectSearch,
  [JOB.DIGDOWN] = { [DIR.TCTR] = true }
};
-- Digger keep job and direction key operations supported ----------------- --
local aDiggerKeys<const> = {
  [ACT.DROP]  = aObjectJobDirKeep,
  [ACT.GRAB]  = aObjectJobDirKeep,
  [ACT.JUMP]  = aObjectJobDirKeep,
  [ACT.PHASE] = { [JOB.PHASE] = { [DIR.U] = true } },
  [ACT.RUN]   = aDiggerMovement,
  [ACT.STOP]  = aObjectStop,
  [ACT.WALK]  = aDiggerMovement,
}
-- Small and large tuneller accepted keys ---------------------------------- --
local aTunnellerKeys<const> = {
  [ACT.STOP] = aObjectStop,
  [ACT.WALK] = { [JOB.NONE] = aObjectLeftRight,
                 [JOB.DIG] = aObjectLeftRight }
};
-- Deploy device ----------------------------------------------------------- --
local aDeployDevice<const> = { [ACT.DEPLOY] = aObjectStop };
-- ------------------------------------------------------------------------- --
local aObjectData<const> = {           -- Objects data
-- ------------------------------------------------------------------------- --
-- [TYP.*] = {                         Object ID (see above)
--   [ACT.*] = {                       Action ID (see above)
--     [DIR.*] = {                     Direction ID (see above)
--       SF,  = <integer>              Starting frame from sprite tileset.
--       EF,  = <integer>              Ending frame from sprite tileset.
--       OFX, = <integer> (optional)   Drawing offset X position.
--       OFY  = <integer> (optional)   Drawing offset Y position.
--     },                              End of specific direction data.
--     FLAGS   = OFL.*[|OFL.*],        Set obj flags on direction (see above).
--     SOUND   = aSfxData.*,           Play specified sound on first frame.
--     SOUNDRP = aSfxData.*            Repeat specified sound on first frame.
--   },                                End of specific action data.
--   ACTION       = ACT.*,             Initial action ID (see above).
--   AITYPE       = AI.*,              AI routine ID (see above).
--   ANIMTIMER    = aTimerData.*,      Animation delay in frames.
--   ATTACHMENT   = TYP.*,             Dummy object to attach to this one.
--   DESC         = "<string>",        Description of object for shop.
--   DIGDELAY     = <integer>,         Frame delay before completing dig.
--   DIRECTION    = DIR.*,             Initial direction ID (see above).
--   FLAGS        = OFL.*[|OFL.*],     XOR'd object flags always active.
--   INTELLIGENCE = <0.0-1.0>,         Chance to not ignore dangerous drops.
--   JOB          = JOB.*,             Initial job ID (see above).
--   KEYS = {                          Actions, jobs and dirs allowed for keys.
--     [ACT.*] = {                     Actions allowed for keyboard press
--       [JOB.*] = {                   Jobs allowed for keyboard press
--         [DIR.*] = true,             Directions allowed for keyboard press
--       },                            End of allowed directions list
--     },                              End of allowed jobs list
--   },                                End of actions, jobs and dirs allowed.
--   LONGNAME     = "<string>",        Name of object for shop/post mortem.
--   LUNGS        = <integer>,         Frame delay before reducing HP in water.
--   MENU         = MNU.*,             Right click menu to use (see above).
--   NAME         = "<string>",        Short name of object.
--   PATIENCE     = <integer>,         Frames before object is impatient.
--   STAMINA      = <integer>,         Frame delay before adding HP.
--   STRENGTH     = <integer>,         Strength when fighting and max carry.
--   TELEDELAY    = <integer>,         Delay before completing teleport.
--   VALUE        = <integer>,         Cost of item to purchase (sell 1/2).
--   WEIGHT       = <integer>          Required weight in order to carry.
-- };                                  End of specific type data.
-- ------------------------------------------------------------------------- --
[TYP.FTARG] = {
 [ACT.HIDE] = {
  [DIR.UL] = {  95,  95 }, [DIR.U]    = {  95,  95 }, [DIR.UR] = {  95,  95 },
  [DIR.L]  = {  95,  95 }, [DIR.NONE] = {  95,  95 }, [DIR.R]  = {  95,  95 },
  [DIR.DL] = {  95,  95 }, [DIR.D]    = {  95,  95 }, [DIR.DR] = {  95,  95 },
  FLAGS    = OFL.BUSY
 }, [ACT.STOP] = {
  [DIR.UL] = { 138, 140 }, [DIR.U]    = { 138, 140 }, [DIR.UR] = { 138, 140 },
  [DIR.L]  = { 138, 140 }, [DIR.NONE] = { 138, 140 }, [DIR.R]  = { 138, 140 },
  [DIR.DL] = { 138, 140 }, [DIR.D]    = { 138, 140 }, [DIR.DR] = { 138, 140 },
  FLAGS    = OFL.FALL|OFL.REGENERATE|OFL.PHASETARGET
 }, [ACT.WALK] = {
  [DIR.UL] = {  12,  15 }, [DIR.U]    = {  12,  15 }, [DIR.UR] = {   8,  11 },
  [DIR.L]  = {  12,  15 }, [DIR.NONE] = {  12,  15 }, [DIR.R]  = {   8,  11 },
  [DIR.DL] = {  12,  15 }, [DIR.D]    = {  12,  15 }, [DIR.DR] = {   8,  11 },
  FLAGS    = OFL.FALL|OFL.PHASETARGET
 }, [ACT.RUN] = {
  [DIR.UL] = {  20,  23 }, [DIR.U]    = {  20,  23 }, [DIR.UR] = {  16,  19 },
  [DIR.L]  = {  20,  23 }, [DIR.NONE] = {  20,  23 }, [DIR.R]  = {  16,  19 },
  [DIR.DL] = {  20,  23 }, [DIR.D]    = {  20,  23 }, [DIR.DR] = {  16,  19 },
  FLAGS    = OFL.FALL|OFL.PHASETARGET
 }, [ACT.DIG] = {
  [DIR.UL] = {  63,  65 }, [DIR.U]    = {  63,  65 }, [DIR.UR] = {  60,  62 },
  [DIR.L]  = {  63,  65 }, [DIR.NONE] = {  63,  65 }, [DIR.R]  = {  60,  62 },
  [DIR.DL] = {  63,  65 }, [DIR.D]    = {  86,  88 }, [DIR.DR] = {  60,  62 },
  FLAGS    = OFL.FALL|OFL.SOUNDLOOP|OFL.PHASETARGET,
  SOUNDRP  = aSfxData.DIG
 }, [ACT.PHASE] = {
  [DIR.UL] = { 106, 109 }, [DIR.U]    = { 106, 109 }, [DIR.UR] = { 106, 109 },
  [DIR.L]  = { 106, 109 }, [DIR.NONE] = { 106, 109 }, [DIR.R]  = { 106, 109 },
  [DIR.DL] = { 106, 109 }, [DIR.D]    = { 106, 109 }, [DIR.DR] = { 106, 109 },
  FLAGS    = OFL.BUSY,
  SOUND    = aSfxData.PHASE
 }, [ACT.FIGHT] = {
  [DIR.UL] = { 245, 249 }, [DIR.U]    = { 240, 244 }, [DIR.UR] = { 240, 244 },
  [DIR.L]  = { 245, 249 }, [DIR.NONE] = { 240, 244 }, [DIR.R]  = { 240, 244 },
  [DIR.DL] = { 245, 249 }, [DIR.D]    = { 240, 244 }, [DIR.DR] = { 240, 244 },
  FLAGS    = OFL.FALL|OFL.RNGSPRITE|OFL.PHASETARGET
 }, [ACT.EATEN] = {
  [DIR.UL] = {  77,  79 }, [DIR.U]    = {  74,  76 }, [DIR.UR] = {  74,  76 },
  [DIR.L]  = {  77,  79 }, [DIR.NONE] = {  74,  76 }, [DIR.R]  = {  74,  76 },
  [DIR.DL] = {  77,  79 }, [DIR.D]    = {  74,  76 }, [DIR.DR] = {  74,  76 },
  FLAGS    = OFL.FALL|OFL.NOANIMLOOP|OFL.BUSY|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY,
  SOUND      = aSfxData.DIEFTAR
 },
 ACTION       = ACT.STOP,              AITYPE       = AI.DIGGER,
 ANIMTIMER    = aTimerData.ANIMNORMAL, DIGDELAY     = 60,
 DIRECTION    = DIR.NONE,
 FLAGS        = OFL.DIGGER|OFL.DELICATE|OFL.LIVING,
 INTELLIGENCE = 0.7,                   JOB          = JOB.NONE,
 KEYS         = aDiggerKeys,           LONGNAME     = "F'TARG",
 LUNGS        = 4,                     MENU         = MNU.MAIN,
 NAME         = "FTARG",               PATIENCE     = 9600,
 STAMINA      = 60,                    STRENGTH     = 3,
 TELEDELAY    = 120,                   VALUE        = 1000,
 WEIGHT       = 0
-- ------------------------------------------------------------------------- --
}, [TYP.HABBISH] = {
 [ACT.HIDE] = {
  [DIR.UL] = {  95,  95 }, [DIR.U]    = {  95,  95 }, [DIR.UR] = {  95,  95 },
  [DIR.L]  = {  95,  95 }, [DIR.NONE] = {  95,  95 }, [DIR.R]  = {  95,  95 },
  [DIR.DL] = {  95,  95 }, [DIR.D]    = {  95,  95 }, [DIR.DR] = {  95,  95 },
  FLAGS    = OFL.BUSY
 }, [ACT.STOP] = {
  [DIR.UL] = { 135, 137 }, [DIR.U]    = { 135, 137 }, [DIR.UR] = { 135, 137 },
  [DIR.L]  = { 135, 137 }, [DIR.NONE] = { 135, 137 }, [DIR.R]  = { 135, 137 },
  [DIR.DL] = { 135, 137 }, [DIR.D]    = { 135, 137 }, [DIR.DR] = { 135, 137 },
  FLAGS    = OFL.FALL|OFL.REGENERATE|OFL.PHASETARGET
 }, [ACT.WALK] = {
  [DIR.UL] = { 120, 123 }, [DIR.U]    = { 120, 123 }, [DIR.UR] = { 116, 119 },
  [DIR.L]  = { 120, 123 }, [DIR.NONE] = { 120, 123 }, [DIR.R]  = { 116, 119 },
  [DIR.DL] = { 120, 123 }, [DIR.D]    = { 120, 123 }, [DIR.DR] = { 116, 119 },
  FLAGS    = OFL.FALL|OFL.PHASETARGET
 }, [ACT.RUN] = {
  [DIR.UL] = { 128, 131 }, [DIR.U]    = { 128, 131 }, [DIR.UR] = { 124, 127 },
  [DIR.L]  = { 128, 131 }, [DIR.NONE] = { 128, 131 }, [DIR.R]  = { 124, 127 },
  [DIR.DL] = { 128, 131 }, [DIR.D]    = { 128, 131 }, [DIR.DR] = { 124, 127 },
  FLAGS    = OFL.FALL|OFL.PHASETARGET
 }, [ACT.DIG] = {
  [DIR.UL] = { 228, 230 }, [DIR.U]    = { 228, 230 }, [DIR.UR] = { 225, 227 },
  [DIR.L]  = { 228, 230 }, [DIR.NONE] = { 228, 230 }, [DIR.R]  = { 225, 227 },
  [DIR.DL] = { 228, 230 }, [DIR.D]    = { 237, 239 }, [DIR.DR] = { 225, 227 },
  FLAGS    = OFL.FALL|OFL.SOUNDLOOP|OFL.PHASETARGET,
  SOUNDRP  = aSfxData.DIG
 }, [ACT.PHASE] = {
  [DIR.UL] = { 106, 109 }, [DIR.U]    = { 106, 109 }, [DIR.UR] = { 106, 109 },
  [DIR.L]  = { 106, 109 }, [DIR.NONE] = { 106, 109 }, [DIR.R]  = { 106, 109 },
  [DIR.DL] = { 106, 109 }, [DIR.D]    = { 106, 109 }, [DIR.DR] = { 106, 109 },
  FLAGS    = OFL.BUSY,
  SOUND    = aSfxData.PHASE
 }, [ACT.FIGHT] = {
  [DIR.UL] = { 255, 259 }, [DIR.U]    = { 250, 254 }, [DIR.UR] = { 250, 254 },
  [DIR.L]  = { 255, 259 }, [DIR.NONE] = { 250, 254 }, [DIR.R]  = { 250, 254 },
  [DIR.DL] = { 255, 259 }, [DIR.D]    = { 250, 254 }, [DIR.DR] = { 250, 254 },
  FLAGS    = OFL.FALL|OFL.RNGSPRITE|OFL.PHASETARGET,
 }, [ACT.EATEN] = {
  [DIR.UL] = { 151, 153 }, [DIR.U]    = { 141, 143 }, [DIR.UR] = { 141, 143 },
  [DIR.L]  = { 151, 153 }, [DIR.NONE] = { 141, 143 }, [DIR.R]  = { 141, 143 },
  [DIR.DL] = { 151, 153 }, [DIR.D]    = { 141, 143 }, [DIR.DR] = { 141, 143 },
  FLAGS    = OFL.FALL|OFL.NOANIMLOOP|OFL.BUSY|OFL.PHASETARGET,
 }, [ACT.REST] = {
  [DIR.NONE] = { 132, 132 },
  FLAGS      = OFL.BUSY|OFL.IMPATIENT|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY,
  SOUND      = aSfxData.DIEHABB
 },
 ACTION       = ACT.STOP,              AITYPE       = AI.DIGGER,
 ANIMTIMER    = aTimerData.ANIMNORMAL, DIGDELAY     = 70,
 DIRECTION    = DIR.NONE,
 FLAGS        = OFL.DIGGER|OFL.DELICATE|OFL.TPMASTER|OFL.LIVING,
 INTELLIGENCE = 0.9,                   JOB          = JOB.NONE,
 KEYS         = aDiggerKeys,           LONGNAME     = "HABBISH",
 LUNGS        = 12,                    MENU         = MNU.MAIN,
 NAME         = "HABBISH",             PATIENCE     = 7500,
 STAMINA      = 120,                   STRENGTH     = 5,
 TELEDELAY    = 60,                    VALUE        = 1000,
 WEIGHT       = 0
-- ------------------------------------------------------------------------- --
}, [TYP.GRABLIN] = {
 [ACT.HIDE] = {
  [DIR.UL] = {  95,  95 }, [DIR.U]    = {  95,  95 }, [DIR.UR] = {  95,  95 },
  [DIR.L]  = {  95,  95 }, [DIR.NONE] = {  95,  95 }, [DIR.R]  = {  95,  95 },
  [DIR.DL] = {  95,  95 }, [DIR.D]    = {  95,  95 }, [DIR.DR] = {  95,  95 },
  FLAGS    = OFL.BUSY
 }, [ACT.STOP] = {
  [DIR.UL] = { 222, 224 }, [DIR.U]    = { 222, 224 }, [DIR.UR] = { 222, 224 },
  [DIR.L]  = { 222, 224 }, [DIR.NONE] = { 222, 224 }, [DIR.R]  = { 222, 224 },
  [DIR.DL] = { 222, 224 }, [DIR.D]    = { 222, 224 }, [DIR.DR] = { 222, 224 },
  FLAGS    = OFL.FALL|OFL.REGENERATE|OFL.PHASETARGET
 }, [ACT.WALK] = {
  [DIR.UL] = { 204, 207 }, [DIR.U]    = { 204, 207 }, [DIR.UR] = { 200, 203 },
  [DIR.L]  = { 204, 207 }, [DIR.NONE] = { 204, 207 }, [DIR.R]  = { 200, 203 },
  [DIR.DL] = { 204, 207 }, [DIR.D]    = { 204, 207 }, [DIR.DR] = { 200, 203 },
  FLAGS    = OFL.FALL|OFL.PHASETARGET
 }, [ACT.RUN] = {
  [DIR.UL] = { 212, 215 }, [DIR.U]    = { 212, 215 }, [DIR.UR] = { 208, 211 },
  [DIR.L]  = { 212, 215 }, [DIR.NONE] = { 212, 215 }, [DIR.R]  = { 208, 211 },
  [DIR.DL] = { 212, 215 }, [DIR.D]    = { 212, 215 }, [DIR.DR] = { 208, 211 },
  FLAGS    = OFL.FALL|OFL.PHASETARGET
 }, [ACT.DIG] = {
  [DIR.UL] = {  83,  85 }, [DIR.U]    = {  83,  85 }, [DIR.UR] = {  80,  82 },
  [DIR.L]  = {  83,  85 }, [DIR.NONE] = {  83,  85 }, [DIR.R]  = {  80,  82 },
  [DIR.DL] = {  83,  85 }, [DIR.D]    = {  89,  91 }, [DIR.DR] = {  80,  82 },
  FLAGS    = OFL.FALL|OFL.SOUNDLOOP|OFL.PHASETARGET,
  SOUNDRP  = aSfxData.DIG
 }, [ACT.PHASE] = {
  [DIR.UL] = { 106, 109 }, [DIR.U]    = { 106, 109 }, [DIR.UR] = { 106, 109 },
  [DIR.L]  = { 106, 109 }, [DIR.NONE] = { 106, 109 }, [DIR.R]  = { 106, 109 },
  [DIR.DL] = { 106, 109 }, [DIR.D]    = { 106, 109 }, [DIR.DR] = { 106, 109 },
  FLAGS    = OFL.BUSY,
  SOUND    = aSfxData.PHASE
 }, [ACT.FIGHT] = {
  [DIR.UL] = { 275, 279 }, [DIR.U]    = { 270, 274 }, [DIR.UR] = { 270, 274 },
  [DIR.L]  = { 275, 279 }, [DIR.NONE] = { 270, 274 }, [DIR.R]  = { 270, 274 },
  [DIR.DL] = { 275, 279 }, [DIR.D]    = { 270, 274 }, [DIR.DR] = { 270, 274 },
  FLAGS    = OFL.FALL|OFL.RNGSPRITE|OFL.PHASETARGET
 }, [ACT.EATEN] = {
  [DIR.UL] = { 219, 221 }, [DIR.U]    = { 216, 218 }, [DIR.UR] = { 216, 218 },
  [DIR.L]  = { 219, 221 }, [DIR.NONE] = { 216, 218 }, [DIR.R]  = { 216, 218 },
  [DIR.DL] = { 219, 221 }, [DIR.D]    = { 216, 218 }, [DIR.DR] = { 216, 218 },
  FLAGS    = OFL.FALL|OFL.NOANIMLOOP|OFL.BUSY|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  SOUND      = aSfxData.DIEGRAB,
  FLAGS      = OFL.BUSY
 },
 ACTION       = ACT.STOP,              AITYPE       = AI.DIGGER,
 ANIMTIMER    = aTimerData.ANIMNORMAL, DIGDELAY     = 50,
 DIRECTION    = DIR.NONE,
 FLAGS        = OFL.DIGGER|OFL.DELICATE|OFL.LIVING,
 INTELLIGENCE = 0.8,                   JOB          = JOB.NONE,
 KEYS         = aDiggerKeys,           LONGNAME     = "GRABLIN",
 LUNGS        = 8,                     MENU         = MNU.MAIN,
 NAME         = "GRABLIN",             PATIENCE     = 10500,
 STAMINA      = 120,                   STRENGTH     = 4,
 TELEDELAY    = 120,                   VALUE        = 1000,
 WEIGHT       = 0,
-- ------------------------------------------------------------------------- --
}, [TYP.QUARRIOR] = {
 [ACT.HIDE] = {
  [DIR.UL] = {  95,  95 }, [DIR.U]    = {  95,  95 }, [DIR.UR] = {  95,  95 },
  [DIR.L]  = {  95,  95 }, [DIR.NONE] = {  95,  95 }, [DIR.R]  = {  95,  95 },
  [DIR.DL] = {  95,  95 }, [DIR.D]    = {  95,  95 }, [DIR.DR] = {  95,  95 },
  FLAGS    = OFL.BUSY
 }, [ACT.STOP] = {
  [DIR.UL] = { 178, 180 }, [DIR.U]    = { 178, 180 }, [DIR.UR] = { 178, 180 },
  [DIR.L]  = { 178, 180 }, [DIR.NONE] = { 178, 180 }, [DIR.R]  = { 178, 180 },
  [DIR.DL] = { 178, 180 }, [DIR.D]    = { 178, 180 }, [DIR.DR] = { 178, 180 },
  FLAGS    = OFL.FALL|OFL.REGENERATE|OFL.PHASETARGET
 }, [ACT.WALK] = {
  [DIR.UL] = { 160, 163 }, [DIR.U]    = { 160, 163 }, [DIR.UR] = { 156, 159 },
  [DIR.L]  = { 160, 163 }, [DIR.NONE] = { 160, 163 }, [DIR.R]  = { 156, 159 },
  [DIR.DL] = { 160, 163 }, [DIR.D]    = { 160, 163 }, [DIR.DR] = { 156, 159 },
  FLAGS    = OFL.FALL|OFL.PHASETARGET
 }, [ACT.RUN] = {
  [DIR.UL] = { 168, 171 }, [DIR.U]    = { 168, 171 }, [DIR.UR] = { 164, 167 },
  [DIR.L]  = { 168, 171 }, [DIR.NONE] = { 168, 171 }, [DIR.R]  = { 164, 167 },
  [DIR.DL] = { 168, 171 }, [DIR.D]    = { 168, 171 }, [DIR.DR] = { 164, 167 },
  FLAGS    = OFL.FALL|OFL.PHASETARGET
 }, [ACT.DIG] = {
  [DIR.UL] = { 234, 236 }, [DIR.U]    = { 234, 236 }, [DIR.UR] = { 231, 233 },
  [DIR.L]  = { 234, 236 }, [DIR.NONE] = { 234, 236 }, [DIR.R]  = { 231, 233 },
  [DIR.DL] = { 234, 236 }, [DIR.D]    = {  92,  94 }, [DIR.DR] = { 231, 233 },
  FLAGS    = OFL.FALL|OFL.SOUNDLOOP|OFL.PHASETARGET,
  SOUNDRP  = aSfxData.DIG
 }, [ACT.PHASE] = {
  [DIR.UL] = { 106, 109 }, [DIR.U]    = { 106, 109 }, [DIR.UR] = { 106, 109 },
  [DIR.L]  = { 106, 109 }, [DIR.NONE] = { 106, 109 }, [DIR.R]  = { 106, 109 },
  [DIR.DL] = { 106, 109 }, [DIR.D]    = { 106, 109 }, [DIR.DR] = { 106, 109 },
  FLAGS    = OFL.BUSY,
  SOUND    = aSfxData.PHASE
 }, [ACT.FIGHT] = {
  [DIR.UL] = { 265, 269 }, [DIR.U]    = { 260, 264 }, [DIR.UR] = { 260, 264 },
  [DIR.L]  = { 265, 269 }, [DIR.NONE] = { 260, 264 }, [DIR.R]  = { 260, 264 },
  [DIR.DL] = { 265, 269 }, [DIR.D]    = { 260, 264 }, [DIR.DR] = { 260, 264 },
  FLAGS    = OFL.FALL|OFL.RNGSPRITE|OFL.PHASETARGET
 }, [ACT.EATEN] = {
  [DIR.UL] = { 175, 177 }, [DIR.U]    = { 172, 174 }, [DIR.UR] = { 172, 174 },
  [DIR.L]  = { 175, 177 }, [DIR.NONE] = { 172, 174 }, [DIR.R]  = { 172, 174 },
  [DIR.DL] = { 175, 177 }, [DIR.D]    = { 172, 174 }, [DIR.DR] = { 172, 174 },
  FLAGS    = OFL.FALL|OFL.NOANIMLOOP|OFL.BUSY|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY,
  SOUND      = aSfxData.DIEQUAR
 },
 ACTION       = ACT.STOP,              AITYPE       = AI.DIGGER,
 ANIMTIMER    = aTimerData.ANIMNORMAL, DIGDELAY     = 80,
 DIRECTION    = DIR.NONE,              FLAGS        = OFL.DIGGER|OFL.LIVING,
 INTELLIGENCE = 0.6,                   JOB          = JOB.NONE,
 KEYS         = aDiggerKeys,           LONGNAME     = "QUARRIOR",
 LUNGS        = 16,                    MENU         = MNU.MAIN,
 NAME         = "QUARRIOR",            PATIENCE     = 15000,
 STAMINA      = 120,                   STRENGTH     = 6,
 TELEDELAY    = 180,                   VALUE        = 1000,
 WEIGHT       = 0
-- ------------------------------------------------------------------------- --
}, [TYP.JENNITE] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 315, 318 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.PHASE] = {
  [DIR.NONE] = { 106, 109, 0, 7 },
  FLAGS      = OFL.FALL|OFL.PICKUP,
  SOUND      = aSfxData.FIND
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.PHASE,                AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.TREASURE|OFL.AQUALUNG,
 JOB       = JOB.SPAWN,                LONGNAME  = "JENNITE",
 NAME      = "JENNITE",                STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 280,                      WEIGHT    = 1,
-- ------------------------------------------------------------------------- --
}, [TYP.DIAMOND] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 428, 431 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.PHASE] = {
  [DIR.NONE] = { 106, 109, 0, 7 },
  FLAGS      = OFL.FALL|OFL.PICKUP,
  SOUND      = aSfxData.FIND
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.PHASE,                AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.TREASURE|OFL.AQUALUNG,
 JOB       = JOB.SPAWN,                LONGNAME  = "DIAMOND",
 NAME      = "DIAMOND",                STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 100,                      WEIGHT    = 1
-- ------------------------------------------------------------------------- --
}, [TYP.GOLD] = {
 [ACT.STOP] = {
  [DIR.NONE] = {  96,  99 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.PHASE] = {
  [DIR.NONE] = { 106, 109, 0, 7 },
  FLAGS      = OFL.FALL|OFL.PICKUP,
  SOUND      = aSfxData.FIND
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.PHASE,                AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.TREASURE|OFL.AQUALUNG,
 JOB       = JOB.SPAWN,                LONGNAME  = "GOLD",
 NAME      = "GOLD",                   STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 80,                       WEIGHT    = 1
-- ------------------------------------------------------------------------- --
}, [TYP.EMERALD] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 432, 435 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.PHASE] = {
  [DIR.NONE] = { 106, 109, 0, 7 },
  FLAGS      = OFL.FALL|OFL.PICKUP,
  SOUND      = aSfxData.FIND
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.PHASE,                AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.TREASURE|OFL.AQUALUNG,
 JOB       = JOB.SPAWN,                LONGNAME  = "EMERALD",
 NAME      = "EMERALD",                STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 60,                       WEIGHT    = 1
-- ------------------------------------------------------------------------- --
}, [TYP.RUBY] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 436, 439 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.PHASE] = {
  [DIR.NONE] = { 106, 109, 0, 7 },
  FLAGS      = OFL.FALL|OFL.PICKUP,
  SOUND      = aSfxData.FIND
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.PHASE,                AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.TREASURE|OFL.AQUALUNG,
 JOB       = JOB.SPAWN,                LONGNAME  = "RUBY",
 NAME      = "RUBY",                   STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 40,                       WEIGHT    = 1,
-- ------------------------------------------------------------------------- --
}, [TYP.PHANTOM] = {
 [ACT.STOP] = {
                          [DIR.U]    = { 442, 445 },
  [DIR.L] = { 442, 445 }, [DIR.NONE] = { 446, 449 }, [DIR.R] = { 446, 449 },
                          [DIR.D]    = { 446, 449 },
  FLAGS   = OFL.HURTDIGGER
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.RANDOM,
 ANIMTIMER = aTimerData.ANIMFAST,      DIRECTION = DIR.NONE,
 FLAGS     = OFL.AQUALUNG|OFL.ENEMY,   JOB       = JOB.NONE,
 LONGNAME  = "PHANTOM",                NAME      = "PHANTOM",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 0,
 WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.SKELETON] = {
 [ACT.STOP] = {
  [DIR.UL] = { 409, 412 }, [DIR.U]    = { 413, 416 }, [DIR.UR] = { 413, 416 },
  [DIR.L]  = { 409, 412 }, [DIR.NONE] = { 413, 416 }, [DIR.R]  = { 413, 416 },
  [DIR.DL] = { 409, 412 }, [DIR.D]    = { 413, 416 }, [DIR.DR] = { 413, 416 },
  FLAGS    = OFL.HURTDIGGER
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.FINDSLOW,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.AQUALUNG|OFL.ENEMY,   JOB       = JOB.NONE,
 LONGNAME  = "SKELETON",               NAME      = "SKELETON",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 0,
 WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.ZOMBIE] = {
 [ACT.STOP] = {
  [DIR.UL] = { 147, 148 }, [DIR.U]    = { 149, 150 }, [DIR.UR] = { 149, 150 },
  [DIR.L]  = { 147, 148 }, [DIR.NONE] = { 149, 150 }, [DIR.R]  = { 149, 150 },
  [DIR.DL] = { 147, 148 }, [DIR.D]    = { 149, 150 }, [DIR.DR] = { 149, 150 },
  FLAGS    = OFL.HURTDIGGER
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.FIND,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.AQUALUNG|OFL.ENEMY,   JOB       = JOB.NONE,
 LONGNAME  = "ZOMBIE",                 NAME      = "ZOMBIE",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 0,
 WEIGHT    = 0,
-- ------------------------------------------------------------------------- --
}, [TYP.GHOST] = {
 [ACT.STOP] = {
  [DIR.UL] = { 360, 360 }, [DIR.U]    = { 361, 361 }, [DIR.UR] = { 361, 361 },
  [DIR.L]  = { 360, 360 }, [DIR.NONE] = { 361, 361 }, [DIR.R]  = { 361, 361 },
  [DIR.DL] = { 360, 360 }, [DIR.D]    = { 361, 361 }, [DIR.DR] = { 361, 361 },
  FLAGS    = OFL.HURTDIGGER
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451,454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.FINDSLOW,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.AQUALUNG|OFL.ENEMY,   JOB       = JOB.NONE,
 LONGNAME  = "GHOST",                  NAME      = "GHOST",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 0,
 WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.ZIPPER] = {
 [ACT.STOP] = {
  [DIR.UL] = { 371, 374 }, [DIR.U]    = { 371, 374 }, [DIR.UR] = { 371, 374 },
  [DIR.L]  = { 371, 374 }, [DIR.NONE] = { 371, 374 }, [DIR.R]  = { 371, 374 },
  [DIR.DL] = { 371, 374 }, [DIR.D]    = { 371, 374 }, [DIR.DR] = { 371, 374 },
  FLAGS    = OFL.PHASEDIGGER
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.FINDSLOW,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.AQUALUNG|OFL.ENEMY,   JOB       = JOB.NONE,
 LONGNAME  = "ZIPPER",                 NAME      = "ZIPPER",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 0,
 WEIGHT    = 0,
-- ------------------------------------------------------------------------- --
}, [TYP.SWRLYPRT] = {
 [ACT.STOP] = {
  [DIR.UL] = { 417, 427 }, [DIR.U]    = { 417, 427 }, [DIR.UR] = { 417, 427 },
  [DIR.L]  = { 417, 427 }, [DIR.NONE] = { 417, 427 }, [DIR.R]  = { 417, 427 },
  [DIR.DL] = { 417, 427 }, [DIR.D]    = { 417, 427 }, [DIR.DR] = { 417, 427 },
  FLAGS    = OFL.PHASEDIGGER
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.FIND,
 ANIMTIMER = aTimerData.ANIMFAST,      DIRECTION = DIR.NONE,
 FLAGS     = OFL.AQUALUNG|OFL.ENEMY,   JOB       = JOB.NONE,
 LONGNAME  = "SWIRLYPORT",             NAME      = "SWRLYPRT",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 0,
 WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.PIRANA] = {
 [ACT.STOP] = {
  [DIR.L] = { 388, 388 }, [DIR.NONE] = { 393, 393 }, [DIR.R] = { 393, 393 },
  FLAGS   = OFL.FALL|OFL.HURTDIGGER|OFL.PHASETARGET
 }, [ACT.FIGHT] = {
  [DIR.L] = { 389, 392 }, [DIR.R] = { 394, 397 },
  FLAGS   = OFL.FALL|OFL.HURTDIGGER|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.STATIONARY|OFL.LIVING,JOB       = JOB.NONE,
 LONGNAME  = "PIRANA PLANT",           LUNGS     = 128,
 NAME      = "PIRANA",                 STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 0,                        WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.FUNGUS] = {
 [ACT.STOP] = {
  [DIR.L] = { 398, 401 }, [DIR.NONE] = { 398, 401 }, [DIR.R] = { 398, 401 },
  FLAGS   = OFL.FALL|OFL.HURTDIGGER|OFL.PHASETARGET
 }, [ACT.FIGHT] = {
  [DIR.L] = { 402, 408 }, [DIR.NONE] = { 402, 408 }, [DIR.R] = { 402, 408 },
  FLAGS   = OFL.FALL|OFL.HURTDIGGER|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.STATIONARY|OFL.AQUALUNG|OFL.LIVING,
 JOB       = JOB.NONE,                 LONGNAME  = "FUNGUS",
 NAME      = "FUNGUS",                 STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 0,                        WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.ALIEN] = {
 [ACT.RUN] = {
  [DIR.L] = { 102, 105 }, [DIR.NONE] = { 112, 115 }, [DIR.R] = { 112, 115 },
  FLAGS   = OFL.FALL|OFL.HURTDIGGER|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.RUN,                  AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.LR,
 FLAGS     = OFL.ENEMY,                JOB       = JOB.BOUNCE,
 LONGNAME  = "ALIEN",                  LUNGS     = 32,
 NAME      = "ALIEN",                  STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 20,
 VALUE     = 0,                        WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.EGG] = {
 [ACT.STOP]  = {
  [DIR.NONE] = { 71, 71 },
  FLAGS      = OFL.FALL|OFL.CONSUME|OFL.PHASETARGET
 }, [ACT.PHASE] = {
  [DIR.NONE] = { 68, 68 },
  FLAGS      = OFL.FALL|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 375, 378 },
  FLAGS      = OFL.FALL|OFL.NOANIMLOOP|OFL.BUSY
 },
 ACTION    = ACT.PHASE,                AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.ENEMY,                JOB       = JOB.SPAWN,
 LONGNAME  = "MYSTERIOUS EGG",         LUNGS     = 128,
 NAME      = "EGG",                    STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 3600,
 VALUE     = 0,                        WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.BIRD] = {
 [ACT.STOP] = {
  [DIR.L] = { 297, 301 }, [DIR.R] = { 302, 306 }
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 }, FLAGS = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.CRITTER,
 ANIMTIMER = aTimerData.ANIMNORMAL ,   DIRECTION = DIR.LR,
 FLAGS     = OFL.LIVING,               JOB       = JOB.BOUNCE,
 LONGNAME  = "BIRD",                   LUNGS     = 2,
 NAME      = "BIRD",                   STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 0,                        WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.FISH] = {
 [ACT.STOP] = {
  [DIR.L] = { 58, 59 }, [DIR.R] = { 56, 57 }
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.CRITTERSLOW,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.LR,
 FLAGS     = OFL.AQUALUNG|OFL.WATERBASED|OFL.LIVING,
 JOB       = JOB.BOUNCE,               LONGNAME  = "GOLDFISH",
 NAME      = "FISH",                   STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 0,                        WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.RAPTOR] = {
 [ACT.RUN] = {
  [DIR.L] = { 362, 365 }, [DIR.R] = { 366, 369 },
  FLAGS   = OFL.FALL|OFL.HURTDIGGER|OFL.PURSUEDIGGER|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 }, FLAGS = OFL.BUSY
 },
 ACTION    = ACT.RUN,                  AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.LR,
 FLAGS     = OFL.LIVING,               JOB       = JOB.BOUNCE,
 LONGNAME  = "VELOCIRAPTOR",           LUNGS     = 16,
 NAME      = "VRAPTOR",                STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 0,                        WEIGHT    = 0,
-- ------------------------------------------------------------------------- --
}, [TYP.ROTARY] = {
 [ACT.WALK] = {
  [DIR.L] = { 380, 383 }, [DIR.R] = { 384, 387 },
  FLAGS   = OFL.FALL|OFL.HURTDIGGER|OFL.PURSUEDIGGER|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.WALK,                 AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.LR,
 FLAGS     = OFL.LIVING,               JOB       = JOB.BOUNCE,
 LONGNAME  = "ROTARYSAURUS",           LUNGS     = 16,
 NAME      = "RTRYSRUS",               STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 0,                        WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.STEGO] = {
 [ACT.CREEP] = {
  [DIR.L] = { 29, 32, 3, 0 }, [DIR.R] = { 39, 42, -3, 0 },
  FLAGS   = OFL.FALL|OFL.HURTDIGGER|OFL.PURSUEDIGGER|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION     = ACT.CREEP,               AITYPE     = AI.NONE,
 ANIMTIMER  = aTimerData.ANIMNORMAL,   ATTACHMENT = TYP.STEGOB,
 DIRECTION  = DIR.LR,                  FLAGS      = OFL.LIVING,
 JOB        = JOB.BOUNCE,              LONGNAME   = "STEGOSAURUS",
 LUNGS      = 16,                      NAME       = "STEGSAUR",
 STAMINA    = -1,                      STRENGTH   = 0,
 TELEDELAY  = 200,                     VALUE      = 0,
 WEIGHT     = 0,
-- ------------------------------------------------------------------------- --
}, [TYP.STEGOB] = {
 [ACT.CREEP] = {
  [DIR.L] = { 25, 28, -16, 0 }, [DIR.R] = { 43, 46, 16, 0 }
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454, -16, 0 },
  FLAGS      = OFL.BUSY
 }
-- ------------------------------------------------------------------------- --
}, [TYP.TURTLE] = {
 [ACT.STOP] = {
  [DIR.L] = { 307, 310 }, [DIR.R] = { 311, 314 }
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.CRITTER,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.LR,
 FLAGS     = OFL.AQUALUNG|OFL.WATERBASED|OFL.LIVING,
 JOB       = JOB.BOUNCE,               LONGNAME  = "TURTLE",
 NAME      = "TURTLE",                 STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 0,                        WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.BIGFOOT] = {
 [ACT.HIDE] = {
  [DIR.NONE] = { 95, 95 },
  FLAGS      = OFL.BUSY
 }, [ACT.STOP] = {
  [DIR.UL] = { 329, 331 }, [DIR.U]    = { 329, 331 }, [DIR.UR] = { 329, 331 },
  [DIR.L]  = { 329, 331 }, [DIR.NONE] = { 329, 331 }, [DIR.R]  = { 329, 331 },
  [DIR.DL] = { 329, 331 }, [DIR.D]    = { 329, 331 }, [DIR.DR] = { 329, 331 },
  FLAGS    = OFL.FALL|OFL.REGENERATE|OFL.PHASETARGET
 }, [ACT.WALK] = {
  [DIR.UL] = { 321, 324 }, [DIR.U]    = { 321, 324 }, [DIR.UR] = { 325, 328 },
  [DIR.L]  = { 321, 324 }, [DIR.NONE] = { 321, 324 }, [DIR.R]  = { 325, 328 },
  [DIR.DL] = { 321, 324 }, [DIR.D]    = { 321, 324 }, [DIR.DR] = { 325, 328 },
  FLAGS    = OFL.FALL|OFL.REGENERATE|OFL.PHASETARGET
 }, [ACT.PHASE] = {
  [DIR.NONE] = { 106, 109 }, [DIR.D] = { 106, 109 },
  FLAGS      = OFL.BUSY,
  SOUND      = aSfxData.PHASE
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.BIGFOOT,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.LR,
 FLAGS     = OFL.AQUALUNG|OFL.LIVING,  JOB       = JOB.BOUNCE,
 LONGNAME  = "SKINWALKER",             NAME      = "BIGFOOT",
 STAMINA   = -1,                       STRENGTH  = 100,
 TELEDELAY = 100,                      VALUE     = 0,
 WEIGHT    = 100
-- ------------------------------------------------------------------------- --
}, [TYP.STUNNEL] = {
 [ACT.STOP] = {
  [DIR.L] = { 284, 284 }, [DIR.NONE] = { 280, 280 }, [DIR.R] = { 280, 280 },
  FLAGS   = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.WALK] = {
  [DIR.L] = { 284, 287 }, [DIR.NONE] = { 280, 283 }, [DIR.R] = { 280, 283 },
  FLAGS   = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.DIG] = {
  [DIR.L] = { 284, 287 }, [DIR.NONE] = { 280, 283 }, [DIR.R] = { 280, 283 },
  FLAGS   = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET,
  SOUNDRP = aSfxData.DIG
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.TUNNELER,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "A MECHANICAL DIGGER\nTHAT DIGS HORIZONTALLY",
 DIGDELAY  = 30,                       DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.DEVICE|OFL.EXPLODE,
 KEYS      = aTunnellerKeys,           JOB       = JOB.NONE,
 LONGNAME  = "SMALL TUNNELER",         LUNGS     = 1,
 MENU      = MNU.TUNNEL,               NAME      = "SMALLTUN",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 150,
 WEIGHT    = 2,
-- ------------------------------------------------------------------------- --
}, [TYP.LTUNNEL] = {
 [ACT.STOP] = {
  [DIR.L] = { 185, 185 }, [DIR.NONE] = { 189, 189 }, [DIR.R] = { 189, 189 },
  FLAGS   = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.WALK] = {
  [DIR.L] = { 185, 188 }, [DIR.R] = { 189, 192 },
  FLAGS   = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.DIG] = {
  [DIR.L] = { 185, 188 }, [DIR.R] = { 189, 192 },
  FLAGS   = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET,
  SOUNDRP = aSfxData.DIG
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION     = ACT.STOP,                AITYPE     = AI.TUNNELER,
 ANIMTIMER  = aTimerData.ANIMNORMAL,   ATTACHMENT = TYP.LTUNNELB,
 DESC       = "A FAST MECHANICAL DIGGER\nTHAT DIGS HORIZONTALLY",
 DIGDELAY   = 10,                      DIRECTION  = DIR.NONE,
 FLAGS      = OFL.SELLABLE|OFL.DEVICE|OFL.EXPLODE,
 JOB        = JOB.NONE,                KEYS       = aTunnellerKeys,
 LONGNAME   = "LARGE TUNNELER",        LUNGS      = 1,
 MENU       = MNU.TUNNEL,              NAME       = "LARGETUN",
 STAMINA    = -1,                      STRENGTH   = 0,
 TELEDELAY  = 200,                     VALUE      = 230,
 WEIGHT     = 3
-- ------------------------------------------------------------------------- --
}, [TYP.LTUNNELB] = {
 [ACT.STOP] = {
  [DIR.L] = { 181, 181, -16, 0 }, [DIR.NONE] = { 193, 193, 16, 0 },
  [DIR.R] = { 193, 193,  16, 0 }
 }, [ACT.WALK] = {
  [DIR.L] = { 181, 184, -16, 0 }, [DIR.NONE] = { 193, 196, 16, 0 },
  [DIR.R] = { 193, 196,  16, 0 }
 }, [ACT.DIG] = {
  [DIR.L] = { 181, 184, -16, 0 }, [DIR.NONE] = { 193, 196, 16, 0 },
  [DIR.R] = { 193, 196,  16, 0 }
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454, 16, 0 },
  FLAGS      = OFL.BUSY
 },
-- ------------------------------------------------------------------------- --
}, [TYP.CORK] = {
 [ACT.STOP] = {
  [DIR.L] = { 288, 288 }, [DIR.NONE] = { 288, 288 }, [DIR.R] = { 288, 288 },
  [DIR.D] = { 288, 288 },
  FLAGS   = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.CREEP] = {
  [DIR.L] = { 288, 290 }, [DIR.R] = { 288, 290 }, [DIR.D] = { 288, 290 },
  FLAGS   = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.DIG] = {
  [DIR.D] = { 288, 290 },
  FLAGS   = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET,
  SOUNDRP = aSfxData.DIG
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 }, KEYS = {
  [ACT.STOP] = aObjectStop,
  [ACT.CREEP] = {
   [JOB.NONE]    = aObjectLeftRight,
   [JOB.DIGDOWN] = { [DIR.TCTR] = true }
  }
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.CORKSCREW,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "A MECHANICAL DIGGER\nTHAT DIGS VERTICALLY DOWN",
 DIGDELAY  = 20,                       DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.DEVICE|OFL.EXPLODE,
 JOB       = JOB.NONE,                 LONGNAME  = "CORKSCREW",
 LUNGS     = 1,                        MENU      = MNU.CORK,
 NAME      = "CORKSCRW",               STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 170,                      WEIGHT    = 3
-- ------------------------------------------------------------------------- --
}, [TYP.TELEPOLE] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 66, 67 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454},
  FLAGS = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "SERVES AS A TELEPORT\nPOINT FOR YOUR DIGGERS",
 DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.DEVICE|OFL.EXPLODE,
 JOB       = JOB.NONE,                 LONGNAME  = "TELEPOLE",
 LUNGS     = 1,                        NAME      = "TELEPOLE",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 260,
 WEIGHT    = 2
-- ------------------------------------------------------------------------- --
}, [TYP.TNT] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 24, 24 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.DYING] = {
  [DIR.NONE] = { 53, 55 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.DANGEROUS|OFL.BUSY|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 291, 296 },
  FLAGS      = OFL.BUSY,
  SOUND      = aSfxData.EXPLODE,
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.EXPLODER,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "QUICK DESTRUCTION\nOF TERRAIN",
 DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.DEVICE|OFL.AQUALUNG|OFL.EXPLODE,
 JOB       = JOB.NONE,
 KEYS      = { [ACT.DYING] = aObjectStop },
 LONGNAME  = "EXPLOSIVES",             MENU      = MNU.TNT,
 NAME      = "TNT",                    STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 600,
 VALUE     = 20,                       WEIGHT    = 1
-- ------------------------------------------------------------------------- --
}, [TYP.FIRSTAID] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 450, 450 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "COMPULSORY ACCESSORY\nFOR ALL DIGGERS",
 DIRECTION = DIR.NONE,                 FLAGS     = OFL.SELLABLE|OFL.DEVICE,
 JOB       = JOB.NONE,                 LONGNAME  = "FIRST AID KIT",
 LUNGS     = 2,                        NAME      = "FIRSTAID",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 60,
 WEIGHT    = 2,
-- ------------------------------------------------------------------------- --
}, [TYP.MAP] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 370, 370 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "VIEW THE ENTIRE ZONE\nLAYOUT WITH THIS MAP",
 DIRECTION = DIR.NONE,                 FLAGS     = OFL.SELLABLE|OFL.DEVICE,
 JOB       = JOB.NONE,                 KEYS      = { [ACT.MAP] = aObjectStop },
 LONGNAME  = "TNT MAP",                LUNGS     = 32,
 MENU      = MNU.MAP,                  NAME      = "MAP",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 300,
 WEIGHT    = 3,
-- ------------------------------------------------------------------------- --
}, [TYP.TRACK] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 441, 441 },
  FLAGS      = OFL.FALL|OFL.PICKUP|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.DEPLOY,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "CARRYS A TRAIN\nWITH VALUABLE CARGO",
 DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.DEVICE|OFL.AQUALUNG,
 JOB       = JOB.NONE,                 KEYS      = aDeployDevice,
 LONGNAME  = "TRACK FOR TRAIN",        MENU      = MNU.DEPLOY,
 NAME      = "TRACK",                  STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 10,                       WEIGHT    = 1,
-- ------------------------------------------------------------------------- --
}, [TYP.TRAIN] = {
 [ACT.STOP] = {
  [DIR.L] = { 4, 4 }, [DIR.NONE] = { 4, 4 }, [DIR.R] = { 4, 4 },
  FLAGS   = OFL.PICKUP|OFL.FALL|OFL.PHASETARGET
 }, [ACT.WALK] = {
  [DIR.L] = { 4, 7 }, [DIR.NONE] = { 4, 7 }, [DIR.R] = { 4, 7 },
  FLAGS   = OFL.PICKUP|OFL.FALL|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 }, KEYS = {
  [ACT.STOP] = aObjectStop,
  [ACT.WALK] = {
    [JOB.SEARCH] = aObjectSearch,
    [JOB.NONE] = aObjectLeftRight,
  },
  [ACT.DROP] = aObjectJobDirKeep,
  [ACT.GRAB] = aObjectJobDirKeep,
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.TRAIN,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "CARRY OBJECTS ALONG A MINE\nNEEDS TRAIN TRACKS",
 DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.TRACK|OFL.DEVICE|OFL.EXPLODE,
 JOB       = JOB.NONE,                 LONGNAME  = "TRAIN FOR RAILS",
 LUNGS     = 1,                        MENU      = MNU.TRAIN,
 NAME      = "TRAIN",                  STAMINA   = -1,
 STRENGTH  = 255,                      TELEDELAY = 200,
 VALUE     = 100,                      WEIGHT    = 3
-- ------------------------------------------------------------------------- --
}, [TYP.BRIDGE] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 146, 146 },
  FLAGS      = OFL.PICKUP|OFL.FALL|OFL.FLOAT|OFL.BLOCK|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "A PLATFORM FOR GAPS\nFOR HARD TO REACH PLACES",
 DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.DEVICE|OFL.AQUALUNG,
 JOB       = JOB.NONE,                 LONGNAME  = "BRIDGE PIECE",
 NAME      = "BRIDGE",                 STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 25,                       WEIGHT    = 1
-- ------------------------------------------------------------------------- --
}, [TYP.BOAT] = {
 [ACT.STOP] = {
  [DIR.L] = { 154, 155 }, [DIR.NONE] = { 154, 155 }, [DIR.R] = { 154, 155 },
  FLAGS   = OFL.PICKUP|OFL.FALL|OFL.FLOAT|OFL.BLOCK|OFL.PHASETARGET
 }, [ACT.CREEP] = {
  [DIR.L] = { 154, 155 }, [DIR.NONE] = { 154, 155 }, [DIR.R] = { 154, 155 },
  FLAGS   = OFL.PICKUP|OFL.FALL|OFL.FLOAT|OFL.BLOCK|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454},
  FLAGS      = OFL.BUSY
 }, KEYS = {
  [ACT.STOP] = aObjectStop,
  [ACT.CREEP] = aObjectMove,
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.BOAT,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "TRAVEL OVER WATERS\nTO DISTANT LOCATIONS",
 DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.DEVICE|OFL.AQUALUNG,
 JOB       = JOB.NONE,                 LONGNAME  = "INFLATABLE BOAT",
 MENU      = MNU.FLOAT,                NAME      = "BOAT",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 60,
 WEIGHT    = 2
-- ------------------------------------------------------------------------- --
}, [TYP.GATE] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 440, 440 },
  FLAGS      = OFL.PICKUP|OFL.FALL|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.DEPLOY,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "PROTECT FROM DANGERS\nAND FLOODING",
 DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.DEVICE|OFL.AQUALUNG,
 JOB       = JOB.NONE,                 KEYS      = aDeployDevice,
 LONGNAME  = "FLOOD GATE",             MENU      = MNU.DEPLOY,
 NAME      = "GATE",                   STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 80,                       WEIGHT    = 2
-- ------------------------------------------------------------------------- --
}, [TYP.GATEB] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 475, 475 }
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 }, KEYS = {
  [ACT.OPEN]  = aObjectStop,
  [ACT.CLOSE] = aObjectStop,
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.GATE,
 ANIMTIMER = aTimerData.ANIMNORMAL,    DIRECTION = DIR.NONE,
 FLAGS     = OFL.DEVICE|OFL.AQUALUNG,  JOB       = JOB.NONE,
 MENU      = MNU.GATE,                 NAME      = "GATE",
 STAMINA   = -1,                       STRENGTH  = 0,
 TELEDELAY = 200,                      VALUE     = 0,
 WEIGHT    = 0
-- ------------------------------------------------------------------------- --
}, [TYP.LIFT] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 320, 320 },
  FLAGS      = OFL.PICKUP|OFL.FALL
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.DEPLOY,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "A VERTICAL MOVING PLATFORM\nCARRIES YOUR DIGGERS",
 DIRECTION = DIR.NONE,
 FLAGS     = OFL.SELLABLE|OFL.DEVICE|OFL.EXPLODE|OFL.AQUALUNG,
 JOB       = JOB.NONE,                 KEYS      = aDeployDevice,
 LONGNAME  = "LIFT",                   MENU      = MNU.DEPLOY,
 NAME      = "LIFT",                   STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 220,                      WEIGHT    = 3
-- ------------------------------------------------------------------------- --
}, [TYP.LIFTB] = {
 [ACT.STOP] = {
  [DIR.U] = { 0, 0 }, [DIR.NONE] = { 0, 0 }, [DIR.D] = { 0, 0 },
  FLAGS   = OFL.BLOCK|OFL.PHASETARGET
 }, [ACT.CREEP] = {
  [DIR.U] = { 0, 0 }, [DIR.NONE] = { 0, 0 }, [DIR.D] = { 0, 0 },
  FLAGS   = OFL.BLOCK|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 }, KEYS = {
   [ACT.STOP] = aObjectStop,
   [ACT.CREEP] = { [JOB.NONE] = { [DIR.U] = true, [DIR.D] = true } }
 },
 ACTION     = ACT.STOP,                AITYPE     = AI.LIFT,
 ANIMTIMER  = aTimerData.ANIMNORMAL,   ATTACHMENT = TYP.LIFTC,
 DIRECTION  = DIR.D,
 FLAGS      = OFL.DEVICE|OFL.EXPLODE|OFL.AQUALUNG,
 JOB        = JOB.NONE,                LONGNAME   = "ELEVATOR",
 MENU       = MNU.LIFT,                NAME       = "ELEVATOR",
 STAMINA    = -1,                      STRENGTH   = 0,
 TELEDELAY  = 0,                       VALUE      = 0,
 WEIGHT     = 0
-- ------------------------------------------------------------------------- --
}, [TYP.LIFTC] = {
 [ACT.STOP] = {
  [DIR.U] = { 1, 1, 0, -16 }, [DIR.NONE] = { 1, 1, 0, -16 },
  [DIR.D] = { 1, 1, 0, -16 }
 }, [ACT.CREEP] = {
  [DIR.U] = { 1, 1, 0, -16}, [DIR.NONE] = { 1, 1, 0, -16 },
  [DIR.D] = { 1, 1, 0, -16}
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 451, 454 },
  FLAGS      = OFL.BUSY
 },
 ACTION     = ACT.STOP,                ANIMTIMER  = aTimerData.ANIMNORMAL,
 ATTACHMENT = TYP.LIFT,                DIRECTION  = DIR.NONE,
 FLAGS      = OFL.DEVICE|OFL.EXPLODE|OFL.AQUALUNG,
 JOB        = JOB.NONE,                LONGNAME   = "ELEVATOR",
 MENU       = MNU.LIFT,                NAME       = "LIFTC",
 STAMINA    = -1,                      STRENGTH   = 0,
 TELEDELAY  = 0,                       VALUE      = 0,
 WEIGHT     = 0
-- ------------------------------------------------------------------------- --
}, [TYP.CAMPFIRE] = {
 [ACT.STOP] = {
  [DIR.NONE] = { 133, 134 },
  FLAGS      = OFL.BUSY|OFL.HEALNEARBY|OFL.PHASETARGET
 }, [ACT.DEATH] = {
  [DIR.NONE] = { 133, 134 },
  FLAGS      = OFL.BUSY
 },
 ACTION    = ACT.STOP,                 AITYPE    = AI.NONE,
 ANIMTIMER = aTimerData.ANIMNORMAL,
 DESC      = "HEALS YOUR DIGGERS FASTER WHEN NEARBY",
 DIRECTION = DIR.NONE,                 FLAGS     = OFL.DEVICE|OFL.EXPLODE,
 JOB       = JOB.NONE,                 LONGNAME  = "CAMPFIRE",
 NAME      = "CAMPFIRE",               STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 0,
 VALUE     = 110,                      WEIGHT    = 0
-- ------------------------------------------------------------------------- --
} };
-- Digging tile flags ------------------------------------------------------ --
local DF<const> = {
  -- Match tile flags ------------------------------------------------------ --
  MO = 0x0001, -- Match over tile or dig failed (aDigData)
  MA = 0x0002, -- Match above tile or dig failed
  MB = 0x0004, -- Match below tile or dig failed
  MC = 0x0008, -- Match centre of over tile or dig failed
  -- Set tile flags -------------------------------------------------------- --
  SO = 0x0010, -- Set over tile if successful match
  SA = 0x0020, -- Set above tile if successful match
  SB = 0x0040, -- Set below tile if successful match
  -- On success flags ------------------------------------------------------ --
  OB = 0x0080, -- Set object to busy if successful dig
  OI = 0x0100, -- Remove busy flag if dig successful
  OG = 0x0200, -- Spin the wheel of fortune if dig successful
  OX = 0x0400  -- Bonus. Not in original game.
  -- ----------------------------------------------------------------------- --
};
-- Digging tile data ------------------------------------------------------- --
local aDigData<const> = {              -- Note that tile ids are 0-indexed here
-- ------------------------------------------------------------------------- --
-- FO    (FromOver)    DF.MO*. Tile to match from object's over tile
-- FA    (FromAbove)   DF.MA*. Tile to match from object's above tile
-- FB    (FromBelow)   DF.MB*. Tile to match from object's below tile
-- FC    (FromCentre)  DF.MC*. Tile to match from object's centre (over) tile
-- TO    (ToOver)      DF.SO*. Set over tile to this tile on successful match
-- TA    (ToAbove)     DF.SA*. Set above tile to this tile on successful match
-- TB    (ToBelow)     DF.SB*. Set below tile to this tile on successful match
-- FLAGS (Flags)       Flags see DF.M*, DF.S* and DF.O* #defines
-- ------------------------------------------------------------------------- --
[DIR.UL]={                           -- Digging upper-left tile data
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
{  3,  3,  0, 12,208,207,  0,DF.MO|DF.MA|DF.MC|DF.SO|DF.SA|DF.OB},
{  3,  3,  0,  0, 44,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3,  7,  0, 12,208,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3,  7,  0,  0, 44,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3, 95,  0, 12,208,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3, 96,  0,  0, 44,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3,150,  0, 12,208,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3,150,  0,  0, 44,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3,170,  0, 12,208,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3,170,  0,  0, 44,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3,171,  0, 12,208,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3,171,  0,  0, 44,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3,172,  0, 12,208,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3,172,  0,  0, 44,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{ 44,  3,  0,  0,203, 45,  0,DF.MO|DF.MA      |DF.SO|DF.SA|DF.OB},
{ 44,  0,  0,  0,203,  0,  0,DF.MO            |DF.SO      |DF.OB},
{203, 45,  0,  0, 12, 10,  0,DF.MO|DF.MA      |DF.SO|DF.SA|DF.OB},
{203,  0,  0,  0, 12,  0,  0,DF.MO            |DF.SO      |DF.OB},
{208,207,  0,  0, 11,209,  0,DF.MO|DF.MA      |DF.SO|DF.SA|DF.OB},
{208,  0,  0,  0, 11,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 11,  0,  0, 11,  0,  9,  0,DF.MO|DF.MC      |DF.SA      |DF.OI|DF.OG},
{ 52,  0,  0,  0,236,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OX},
{ 53,  0,  0,  0,236,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OX},
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
},[DIR.UR]={                          -- Digging upper-right tile data
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
{  3,  3,  0,  6, 33, 32,  0,DF.MO|DF.MA|DF.MC|DF.SO|DF.SA|DF.OB},
{  3,  3,  0,  0, 26,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3,  7,  0,  6, 33,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3,  7,  0,  0, 26,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3, 95,  0,  6, 33,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3, 96,  0,  0, 26,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3,150,  0,  6, 33,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3,150,  0,  0, 26,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3,170,  0,  6, 33,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3,170,  0,  0, 26,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3,171,  0,  6, 33,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3,171,  0,  0, 26,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{  3,172,  0,  6, 33,  0,  0,DF.MO|DF.MA|DF.MC|DF.SO      |DF.OB},
{  3,172,  0,  0, 26,  0,  0,DF.MO|DF.MA      |DF.SO      |DF.OB},
{ 26,  3,  0,  0, 28, 27,  0,DF.MO|DF.MA      |DF.SO|DF.SA|DF.OB},
{ 26,  0,  0,  0, 28,  0,  0,DF.MO            |DF.SO      |DF.OB},
{ 28, 27,  0,  0,  6,  4,  0,DF.MO|DF.MA      |DF.SO|DF.SA|DF.OB},
{ 28,  0,  0,  0,  6,  0,  0,DF.MO            |DF.SO      |DF.OB},
{ 33, 32,  0,  0,  8, 34,  0,DF.MO|DF.MA      |DF.SO|DF.SA|DF.OB},
{ 33,  0,  0,  0,  8,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{  8,  0,  0,  8,  0,  5,  0,DF.MO|DF.MC      |DF.SA      |DF.OI|DF.OG},
{ 52,  0,  0,  0,235,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OX},
{ 53,  0,  0,  0,235,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OX},
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
},[DIR.L]={                           -- Digging left tile data
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
{  3,  0,  0,  0,230,  0,  0,DF.MO            |DF.SO      |DF.OB},
{  4,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OB},
{ 10,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OB},
{230,  0,  0,  0,231,  0,  0,DF.MO            |DF.SO      |DF.OB},
{231,  0,  0,  0,232,  0,  0,DF.MO            |DF.SO      |DF.OB},
{232,  0,  0,  0, -1,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{  8,  0,  0,  0,  6,  0,  0,DF.MO            |DF.SO      |DF.OB|DF.OG},
{ 11,  0,  0,  0, 12,  0,  0,DF.MO            |DF.SO      |DF.OB|DF.OG},
{ 10,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OB|DF.OG},
{  9,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 49,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 50,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 52,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 53,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 55,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 56,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 61,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{  6,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{173,  0,  0,  0,174,  0,  0,DF.MO            |DF.SO      |DF.OB},
{174,  0,  0,  0,175,  0,  0,DF.MO            |DF.SO      |DF.OB},
{175,  0,  0,  0, -1,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 13,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 14,  0,  0,  0,  6,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 15,  0,  0,  0, 12,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 16,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 46,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 51,  0,  0,  0, 12,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 54,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 57,  0,  0,  0,  6,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{235,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
}, [DIR.R]={                           -- Digging right tile data
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
{  3,  0,  0,  0,227,  0,  0,DF.MO            |DF.SO      |DF.OB},
{  4,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OB},
{ 10,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OB},
{227,  0,  0,  0,228,  0,  0,DF.MO            |DF.SO      |DF.OB},
{228,  0,  0,  0,229,  0,  0,DF.MO            |DF.SO      |DF.OB},
{229,  0,  0,  0, -1,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{  8,  0,  0,  0,  6,  0,  0,DF.MO            |DF.SO      |DF.OB|DF.OG},
{ 11,  0,  0,  0, 12,  0,  0,DF.MO            |DF.SO      |DF.OB|DF.OG},
{  4,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OB|DF.OG},
{  5,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 49,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 50,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 52,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 53,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 55,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 56,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 58,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 12,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{173,  0,  0,  0,174,  0,  0,DF.MO            |DF.SO      |DF.OB},
{174,  0,  0,  0,175,  0,  0,DF.MO            |DF.SO      |DF.OB},
{175,  0,  0,  0, -1,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 13,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 14,  0,  0,  0,  6,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 15,  0,  0,  0, 12,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 16,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 46,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 51,  0,  0,  0, 12,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 54,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 57,  0,  0,  0,  6,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{236,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
},[DIR.DL]={                           -- Digging down-left tile data
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
{  3,  0,  3,  5, 23,  0, 24,DF.MO|DF.MB|DF.MC|DF.SO|DF.SB|DF.OB},
{  3,  0,  3,  0, 17,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{  3,  0,  7,  5, 23,  0,  0,DF.MO|DF.MB|DF.MC|DF.SO      |DF.OB},
{  3,  0,  7,  0, 17,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{  3,  0,150,  5, 23,  0,  0,DF.MO|DF.MB|DF.MC|DF.SO      |DF.OB},
{  3,  0,150,  0, 17,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{  3,  0,170,  5, 23,  0,  0,DF.MO|DF.MB|DF.MC|DF.SO      |DF.OB},
{  3,  0,170,  0, 17,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{  3,  0,171,  5, 23,  0,  0,DF.MO|DF.MB|DF.MC|DF.SO      |DF.OB},
{  3,  0,171,  0, 17,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{  3,  0,172,  5, 23,  0,  0,DF.MO|DF.MB|DF.MC|DF.SO      |DF.OB},
{  3,  0,172,  0, 17,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{ 17,  0,  3,  0, 18,  0, 19,DF.MO|DF.MB      |DF.SO|DF.SB|DF.OB},
{ 17,  0,  0,  0, 18,  0,  0,DF.MO            |DF.SO      |DF.OB},
{ 18,  0, 19,  0,  5,  0,  8,DF.MO|DF.MB      |DF.SO|DF.SB|DF.OB},
{ 18,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OB},
{ 23,  0, 24,  0,  4,  0, 25,DF.MO|DF.MB      |DF.SO|DF.SB|DF.OB},
{ 23,  0,  0,  0,  4,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{  4,  0,  0,  0,  0,  0,  6,DF.MO            |DF.SB      |DF.OI|DF.OG},
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
},[DIR.D]={                            -- Digging down?
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
{  3,  0,  0,  0,173,  0,  0,DF.MO            |DF.SO      |DF.OB},
{173,  0,  0,  0,174,  0,  0,DF.MO            |DF.SO      |DF.OB},
{174,  0,  0,  0,175,  0,  0,DF.MO            |DF.SO      |DF.OB},
{175,  0,  0,  0, -2,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{  8,  0,  0,  0,  6,  0,  0,DF.MO            |DF.SO      |DF.OB},
{ 11,  0,  0,  0, 12,  0,  0,DF.MO            |DF.SO      |DF.OB},
{  4,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OB},
{ 10,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OB},
{  5,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{  9,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 47,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 48,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 52,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 53,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 12,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{  6,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 13,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 14,  0,  0,  0,  6,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 15,  0,  0,  0, 12,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 16,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 46,  0,  0,  0,  5,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 51,  0,  0,  0, 12,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 54,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{ 57,  0,  0,  0,  6,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{235,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
{236,  0,  0,  0,  7,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG|DF.OX},
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
},[DIR.DR]={                           -- Digging down right?
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
{  3,  0,  3,  9, 41,  0, 42,DF.MO|DF.MB|DF.MC|DF.SO|DF.SB|DF.OB},
{  3,  0,  3,  0, 35,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{  3,  0,  7,  9, 41,  0,  0,DF.MO|DF.MB|DF.MC|DF.SO      |DF.OB},
{  3,  0,  7,  0, 35,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{  3,  0,150,  9, 41,  0,  0,DF.MO|DF.MB|DF.MC|DF.SO      |DF.OB},
{  3,  0,150,  0, 35,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{  3,  0,170,  9, 41,  0,  0,DF.MO|DF.MB|DF.MC|DF.SO      |DF.OB},
{  3,  0,170,  0, 35,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{  3,  0,171,  9, 41,  0,  0,DF.MO|DF.MB|DF.MC|DF.SO      |DF.OB},
{  3,  0,171,  0, 35,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{  3,  0,172,  9, 41,  0,  0,DF.MO|DF.MB|DF.MC|DF.SO      |DF.OB},
{  3,  0,172,  0, 35,  0,  0,DF.MO|DF.MB      |DF.SO      |DF.OB},
{ 35,  0,  3,  0, 36,  0, 37,DF.MO|DF.MB      |DF.SO|DF.SB|DF.OB},
{ 35,  0,  0,  0, 36,  0,  0,DF.MO            |DF.SO      |DF.OB},
{ 36,  0, 37,  0,  9,  0, 11,DF.MO|DF.MB      |DF.SO|DF.SB|DF.OB},
{ 36,  0,  0,  0,  9,  0,  0,DF.MO            |DF.SO      |DF.OB},
{ 41,  0, 42,  0, 10,  0, 43,DF.MO|DF.MB      |DF.SO|DF.SB|DF.OB},
{ 41,  0,  0,  0, 10,  0,  0,DF.MO            |DF.SO      |DF.OI|DF.OG},
{ 10,  0,  0,  0,  0,  0, 12,DF.MO            |DF.SB      |DF.OI|DF.OG},
-- FO FA  FB  FC  TO  TA  TB FLAGS ----------------------------------------- --
} };
-- Dug data ---------------------------------------------------------------- --
local aDugRandShaftData<const> = {
  [-1] = { 150, 170, 171 }, -- Horizontal (translated from TO=-1 above)
  [-2] = { 150, 172, 172 }  -- Vertical (translated from TO=-2 above)
};
-- Tile flag defines ------------------------------------------------------- --
local TF<const> = {
  -- ----------------------------------------------------------------------- --
  NONE = 0x00000, -- Tile is non-destructible and insignificant
  F    = 0x00001, -- Tile is firm ground
  W    = 0x00002, -- Tile is water
  AB   = 0x00004, -- Increment terrain tile animation?
  AE   = 0x00008, -- Reset terrain tile animation?
  D    = 0x00010, -- Tile is destructible
  AD   = 0x00020, -- Tile has been artificially dug
  PO   = 0x00040, -- Player one start position?
  PT   = 0x00080, -- Player two start position?
  EL   = 0x00100, -- The left edge of the tile is exposed to flood
  ET   = 0x00200, -- The top edge of the tile is exposed to flood
  ER   = 0x00400, -- The right edge of the tile is exposed to flood
  EB   = 0x00800, -- The bottom edge of the tile is exposed to flood
  P    = 0x01000, -- The tile below is protected from digging
  T    = 0x02000, -- Tile is a track for train
  E    = 0x04000, -- Line support for elevator
  G    = 0x08000, -- Tile is a flood gate
  -- End of tile flags ----------------------------------------------------- --
};
-- Extended tile data flags ------------------------------------------------ --
TF.ELT  = TF.EL|TF.ET;                 -- <^   Left/Up exposed
TF.ELR  = TF.EL|TF.ER;                 -- <  > Left/right exposed
TF.ETR  = TF.ET|TF.ER;                 --  ^ > Up/right exposed
TF.ETB  = TF.ET|TF.EB;                 --  ^v  Up/down exposed
TF.ERB  = TF.ER|TF.EB;                 --   v> Down/right exposed
TF.ELB  = TF.EL|TF.EB;                 -- < v  Left/Down exposed
TF.ELTR = TF.EL|TF.ET|TF.ER;           -- <^ > Left/Up/Right exposed
TF.ETRB = TF.ET|TF.ER|TF.EB;           --  ^v> Up/Down/Right exposed
TF.ELRB = TF.EL|TF.ER|TF.EB;           -- < v> Left/Down/Right exposed
TF.ELTB = TF.EL|TF.ET|TF.EB;           -- <^v  Left/Up/Down exposed
TF.EA   = TF.EL|TF.ET|TF.ER|TF.EB;     -- <^v> All directions exposed
-- Tile data flags lookup ------------------------------------------------- --
local aTileData<const> = {             -- TID TXxTY NOTE (total 480 tiles)
  TF.NONE,                             -- 001 01x01 Air (Passive)
  TF.NONE,                             -- 002 02x01 Grass (Passive)
  TF.NONE,                             -- 003 03x01 Solid undiggable stone
  TF.F|TF.D,                           -- 004 04x01 Solid diggable dirt
  TF.F|TF.D|TF.ERB,                    -- 005 05x01 75%dirt+25%dug 22deg UR>DL
  TF.F|TF.D|TF.ELRB,                   -- 006 06x01 25%dirt+75%dug 22deg UR>DL
  TF.F|TF.D|TF.ELTR,                   -- 007 07x01 75%dug+25%dirt 22deg UR>DL
  TF.AD|TF.EA,                         -- 008 08x01 Clear dug tile
  TF.D|TF.ELT,                         -- 009 09x01 25%dug+75%dirt 22deg UR>DL
  TF.F|TF.D|TF.ELRB,                   -- 010 10x01 25%dirt+75%dug 22deg UL>DR
  TF.F|TF.D|TF.ELB,                    -- 011 11x01 75%dirt+25%dug 22deg UL>DR
  TF.D|TF.ETR,                         -- 012 12x01 25%dug+75%dirt 22deg UL>DR
  TF.D|TF.ELTR,                        -- 013 13x01 75%dug+25%dirt 22deg UL>DR
  TF.F|TF.D|TF.ERB,                    -- 014 14x01 50%dirt+50%dug 45deg UR>DL
  TF.D|TF.ELT,                         -- 015 15x01 50%dug+50%dirt 45deg UR>DL
  TF.D|TF.ETR,                         -- 016 16x01 50%dug+50%dirt 45deg UL>DR
  TF.F|TF.D|TF.ELB,                    -- 017 17x01 50%dirt+50%dug 45deg UL>DR
  TF.F|TF.D|TF.ER,                     -- 018 18x01
  TF.F|TF.D|TF.ERB,                    -- 019 19x01
  TF.D|TF.ET,                          -- 020 20x01
  TF.F|TF.D|TF.ERB,                    -- 021 21x01
  TF.D|TF.ETR,                         -- 022 22x01
  TF.D|TF.ELT,                         -- 023 23x01
  TF.F|TF.D|TF.ERB,                    -- 024 24x01
  TF.D|TF.ETR,                         -- 025 25x01
  TF.D|TF.ETR,                         -- 026 26x01
  TF.F|TF.D|TF.EL,                     -- 027 27x01
  TF.F|TF.D|TF.EB,                     -- 028 28x01
  TF.D|TF.ELT,                         -- 029 29x01
  TF.F|TF.D|TF.EB,                     -- 030 30x01
  TF.D|TF.ELTR,                        -- 031 31x01
  TF.F|TF.D|TF.EL,                     -- 032 32x01
  TF.F|TF.D|TF.ELB,                    -- 033 01x02
  TF.D|TF.ELT,                         -- 034 02x02
  TF.F|TF.D|TF.ELB,                    -- 035 03x02
  TF.F|TF.D|TF.ELB,                    -- 036 04x02
  TF.F|TF.D|TF.ELB,                    -- 037 05x02
  TF.D|TF.ET,                          -- 038 06x02
  TF.F|TF.D|TF.ELB,                    -- 039 07x02
  TF.D|TF.ETR,                         -- 040 08x02
  TF.D|TF.ELT,                         -- 041 09x02
  TF.D|TF.ELB,                         -- 042 10x02
  TF.F|TF.D|TF.ELT,                    -- 043 11x02
  TF.D|TF.ELT,                         -- 044 12x02
  TF.D|TF.ER,                          -- 045 13x02
  TF.F|TF.D|TF.EB,                     -- 046 14x02
  TF.F|TF.D|TF.ERB,                    -- 047 15x02
  TF.F|TF.D|TF.ELRB,                   -- 048 16x02
  TF.F|TF.D|TF.ELRB,                   -- 049 17x02
  TF.F|TF.D|TF.ETRB,                   -- 050 18x02
  TF.D|TF.ETRB,                        -- 051 19x02
  TF.D|TF.ETR,                         -- 052 20x02
  TF.D|TF.ELTR,                        -- 053 21x02 Cave floor 1
  TF.D|TF.ELTR,                        -- 054 22x02 Cave floor 2
  TF.D|TF.ELB,                         -- 055 23x02
  TF.F|TF.D|TF.ELTB,                   -- 056 24x02
  TF.F|TF.D|TF.ELTB,                   -- 057 25x02
  TF.D|TF.ELT,                         -- 058 26x02
  TF.D|TF.EA,                          -- 059 27x02
  TF.D|TF.EA,                          -- 060 28x02
  TF.D|TF.EA,                          -- 061 29x02
  TF.D|TF.EA,                          -- 062 30x02
  TF.ELRB,                             -- 063 31x02
  TF.F,                                -- 064 32x02
  TF.F,                                -- 065 01x03
  TF.NONE,                             -- 066 02x03
  TF.NONE,                             -- 067 03x03
  TF.NONE,                             -- 068 04x03
  TF.NONE,                             -- 069 05x03
  TF.NONE,                             -- 070 06x03
  TF.NONE,                             -- 071 07x03
  TF.NONE,                             -- 072 08x03
  TF.NONE,                             -- 073 09x03
  TF.NONE,                             -- 074 10x03
  TF.NONE,                             -- 075 11x03
  TF.NONE,                             -- 076 12x03
  TF.P|TF.EA,                          -- 077 13x03
  TF.P|TF.EA,                          -- 078 14x03
  TF.P|TF.EA,                          -- 079 15x03
  TF.P|TF.EA,                          -- 080 16x03
  TF.P|TF.EA,                          -- 081 17x03
  TF.P|TF.EA,                          -- 082 18x03
  TF.P|TF.EA,                          -- 083 19x03
  TF.P|TF.EA,                          -- 084 20x03
  TF.P|TF.EA,                          -- 085 21x03
  TF.NONE,                             -- 086 22x03 Unused
  TF.NONE,                             -- 087 23x03 Unused
  TF.NONE,                             -- 088 24x03 Platform cliff edge right
  TF.D|TF.P|TF.EA,                     -- 089 25x03 Left end of track
  TF.NONE,                             -- 090 26x03 Floor to wall right
  TF.NONE,                             -- 091 27x03 Platform cliff edge left
  TF.D|TF.P|TF.EA,                     -- 092 28x03 Right end of track
  TF.NONE,                             -- 093 29x03 Right cliff wall
  TF.NONE,                             -- 094 30x03 Left cliff wall
  TF.NONE,                             -- 095 31x03 Floor to wall left
  TF.EA,                               -- 096 32x03 Clear dug tile with window
  TF.EA,                               -- 097 01x04 Clear dug tile with skull
  TF.W|TF.AB,                          -- 098 02x04 Ocean surface 1/4
  TF.W|TF.AB,                          -- 099 03x04 Ocean surface 2/4
  TF.W|TF.AB,                          -- 100 04x04 Ocean surface 3/4
  TF.W|TF.AE,                          -- 101 05x04 Ocean surface 4/4
  TF.W|TF.AB,                          -- 102 06x04 Ocean bubble large 1/4
  TF.W|TF.AB,                          -- 103 07x04 Ocean bubble large 2/4
  TF.W|TF.AB,                          -- 104 08x04 Ocean bubble large 3/4
  TF.W|TF.AE,                          -- 105 09x04 Ocean bubble large 4/4
  TF.W|TF.AB,                          -- 106 10x04 Ocean surface bubble L 1/4
  TF.W|TF.AB,                          -- 107 11x04 Ocean surface bubble L 2/4
  TF.W|TF.AB,                          -- 108 12x04 Ocean surface bubble L 3/4
  TF.W|TF.AE,                          -- 109 13x04 Ocean surface bubble L 4/4
  TF.W|TF.AB,                          -- 110 14x04 Ocean bubble small 1/4
  TF.W|TF.AB,                          -- 111 15x04 Ocean bubble small 2/4
  TF.W|TF.AB,                          -- 112 16x04 Ocean bubble small 3/4
  TF.W|TF.AE,                          -- 113 17x04 Ocean bubble small 4/4
  TF.W,                                -- 114 18x04 Ocean surface bubble S 1/4
  TF.W,                                -- 115 19x04 Ocean surface bubble S 2/4
  TF.W,                                -- 116 20x04 Ocean surface bubble S 3/4
  TF.W,                                -- 117 21x04 Ocean surface bubble S 4/4
  TF.W,                                -- 118 22x04 Ocean
  TF.W|TF.AB,                          -- 119 23x04 Ocean surface weed 1/4
  TF.W|TF.AB,                          -- 120 24x04 Ocean surface weed 2/4
  TF.W|TF.AB,                          -- 121 25x04 Ocean surface weed 3/4
  TF.W|TF.AE,                          -- 122 26x04 Ocean surface weed 4/4
  TF.NONE,                             -- 123 27x04
  TF.NONE,                             -- 124 28x04
  TF.NONE,                             -- 125 29x04
  TF.NONE,                             -- 126 30x04
  TF.NONE,                             -- 127 31x04
  TF.NONE,                             -- 128 32x04
  TF.NONE,                             -- 129 01x05
  TF.NONE,                             -- 130 02x05
  TF.NONE,                             -- 131 03x05
  TF.NONE,                             -- 132 04x05
  TF.NONE,                             -- 133 05x05
  TF.NONE,                             -- 134 06x05
  TF.P|TF.EA,                          -- 135 07x05
  TF.P|TF.EA,                          -- 136 08x05
  TF.P|TF.EA,                          -- 137 09x05
  TF.P|TF.EA,                          -- 138 10x05
  TF.W,                                -- 139 11x05
  TF.W,                                -- 140 12x05
  TF.W,                                -- 141 13x05
  TF.W,                                -- 142 14x05
  TF.NONE,                             -- 143 15x05
  TF.NONE,                             -- 144 16x05
  TF.W,                                -- 145 17x05
  TF.NONE,                             -- 146 18x05
  TF.NONE,                             -- 147 19x05
  TF.W,                                -- 148 20x05
  TF.NONE,                             -- 149 21x05
  TF.D|TF.AD|TF.P|TF.T|TF.EA,          -- 150 22x05 Dug tile track with light
  TF.D|TF.AD|TF.EA,                    -- 151 23x05 Dug tile with light
  TF.F,                                -- 152 24x05 Quad rock bottom left
  TF.F,                                -- 153 25x05 Quad rock bottom right
  TF.F,                                -- 154 26x05 Quad rock top left
  TF.F,                                -- 155 27x05 Quad rock top right
  TF.F,                                -- 156 28x05 Impenetrable rock
  TF.F,                                -- 157 29x05 Impenetrable rock
  TF.F,                                -- 158 30x05 Horizontal rock left
  TF.F,                                -- 159 31x05 Horizontal rock right
  TF.F,                                -- 160 32x05 Impenetrable rock
  TF.F,                                -- 161 01x06 Unused
  TF.NONE,                             -- 162 02x06 Unused
  TF.NONE,                             -- 163 03x06
  TF.NONE,                             -- 164 04x06 Outside decoration only
  TF.NONE,                             -- 165 05x06 Outside decoration only
  TF.NONE,                             -- 166 06x06 Outside decoration only
  TF.NONE,                             -- 167 07x06 Outside decoration only
  TF.NONE,                             -- 168 08x06 Outside decoration only
  TF.NONE,                             -- 169 09x06 Outside decoration only
  TF.D|TF.AD|TF.P|TF.T|TF.EA,          -- 170 10x06 Dug tile beam forward track
  TF.D|TF.AD|TF.EA,                    -- 171 11x06 Dug tile beam forward
  TF.D|TF.AD|TF.EA,                    -- 172 12x06 Dug tile beam backwards
  TF.D|TF.AD|TF.EA,                    -- 173 13x06 Dug tile beam horizontal
  TF.D|TF.ELTR,                        -- 174 14x06 Dug tile down 1/3
  TF.D|TF.ELTR,                        -- 175 15x06 Dug tile down 2/3
  TF.D|TF.ELTR,                        -- 176 16x06 Dug tile down 3/3
  TF.NONE,                             -- 177 17x06 Outside decoration only
  TF.NONE,                             -- 178 18x06 Outside decoration only
  TF.NONE,                             -- 179 19x06 Outside decoration only
  TF.NONE,                             -- 180 20x06 Outside decoration only
  TF.NONE,                             -- 181 21x06 Outside decoration only
  TF.NONE,                             -- 182 22x06 Outside decoration only
  TF.NONE,                             -- 183 23x06 Outside decoration only
  TF.NONE,                             -- 184 24x06 Outside decoration only
  TF.NONE,                             -- 185 25x06 Outside decoration only
  TF.NONE,                             -- 186 26x06 Outside decoration only
  TF.NONE,                             -- 187 27x06 Outside decoration only
  TF.NONE,                             -- 188 28x06 Outside decoration only
  TF.NONE,                             -- 189 29x06 Outside decoration only
  TF.EA|TF.E,                          -- 190 30x06 Elevator shaft wire
  TF.NONE,                             -- 191 31x06 Elevator base
  TF.NONE,                             -- 192 32x06 Trade centre top left
  TF.NONE,                             -- 193 01x07 Trade centre top right
  TF.NONE,                             -- 194 02x07 Trade centre bottom left
  TF.NONE,                             -- 195 03x07 Trade centre bottom right
  TF.AB|TF.PO,                         -- 196 04x07 Player 1 home flag (1/4)
  TF.AB|TF.PO,                         -- 197 05x07 Player 1 home flag (2/4)
  TF.AB|TF.PO,                         -- 198 06x07 Player 1 home flag (3/4)
  TF.AE|TF.PO,                         -- 199 07x07 Player 1 home flag (4/4)
  TF.AB|TF.PT,                         -- 200 08x07 Player 2 home flag (1/4)
  TF.AB|TF.PT,                         -- 201 09x07 Player 2 home flag (2/4)
  TF.AB|TF.PT,                         -- 202 10x07 Player 2 home flag (3/4)
  TF.AE|TF.PT,                         -- 203 11x07 Player 2 home flag (4/4)
  TF.D|TF.ELR,                         -- 204 12x07
  TF.F|TF.D|TF.EB,                     -- 205 13x07
  TF.F|TF.D|TF.ER,                     -- 206 14x07
  TF.D|TF.ELTR,                        -- 207 15x07
  TF.F|TF.D|TF.ERB,                    -- 208 16x07
  TF.D|TF.ETR,                         -- 209 17x07
  TF.F|TF.D|TF.ERB,                    -- 210 18x07
  TF.D|TF.AD|TF.P|TF.T|TF.EA,          -- 211 19x07 Clear tile with track
  TF.F|TF.D|TF.ETR,                    -- 212 20x07
  TF.F|TF.D|TF.ETRB,                   -- 213 21x07
  TF.D|TF.ETR,                         -- 214 22x07
  TF.D|TF.ETR,                         -- 215 23x07
  TF.D|TF.ELTB,                        -- 216 24x07
  TF.F|TF.D|TF.ELB,                    -- 217 25x07
  TF.D|TF.ELTB,                        -- 218 26x07
  TF.D|TF.ELRB,                        -- 219 27x07
  TF.D|TF.ELT,                         -- 220 28x07
  TF.D|TF.ELTB,                        -- 221 29x07
  TF.D|TF.ELT,                         -- 222 30x07
  TF.D|TF.ELT,                         -- 223 31x07
  TF.F|TF.D|TF.ETRB,                   -- 224 32x07
  TF.W|TF.D|TF.ERB,                    -- 225 01x08
  TF.F|TF.D|TF.ETRB,                   -- 226 02x08
  TF.D|TF.ELRB,                        -- 227 03x08
  TF.D|TF.ELTB,                        -- 228 04x08
  TF.D|TF.ELTB,                        -- 229 05x08
  TF.D|TF.ELTB,                        -- 230 06x08
  TF.D|TF.ETRB,                        -- 231 07x08
  TF.D|TF.ETRB,                        -- 232 08x08
  TF.D|TF.ETRB,                        -- 233 09x08
  TF.NONE,                             -- 234 10x08
  TF.NONE,                             -- 235 11x08
  TF.D|TF.EA,                          -- 236 12x08 90%dug+10%dirt 11deg UR>DL
  TF.D|TF.EA,                          -- 237 13x08 90%dug+10%dirt 11deg UL>DR
  TF.NONE,                             -- 238 14x08 Unused
  TF.NONE,                             -- 239 15x08 Unused
  TF.NONE,                             -- 240 16x08 Unused
  TF.F,                                -- 241 17x08 Double horizontal rock 1/2
  TF.F,                                -- 242 18x08 Double horizontal rock 2/2
  TF.F,                                -- 243 19x08 Double vertical rock 1/2
  TF.F,                                -- 244 20x08 Double vertical rock 2/2
  TF.F|TF.D|TF.W|TF.ERB,               -- 245 21x08
  TF.F|TF.D|TF.W|TF.ELRB,              -- 246 22x08
  TF.D|TF.W|TF.ELTR,                   -- 247 23x08
  TF.D|TF.W|TF.EA|TF.AD,               -- 248 24x08 Clear water tile
  TF.D|TF.W|TF.ELT,                    -- 249 25x08
  TF.F|TF.D|TF.W|TF.ELRB,              -- 250 26x08
  TF.F|TF.D|TF.W|TF.ELB,               -- 251 27x08
  TF.D|TF.W|TF.ETR,                    -- 252 28x08
  TF.D|TF.W|TF.ELTR,                   -- 253 29x08
  TF.F|TF.D|TF.W|TF.EA,                -- 254 30x08
  TF.D|TF.W|TF.EA,                     -- 255 31x08
  TF.D|TF.W|TF.EA,                     -- 256 32x08
  TF.F|TF.D|TF.W|TF.EA,                -- 257 01x09
  TF.F|TF.D|TF.W|TF.ERB,               -- 258 02x09
  TF.F|TF.D|TF.W|TF.ERB,               -- 259 03x09
  TF.D|TF.W|TF.ET,                     -- 260 04x09
  TF.F|TF.D|TF.W|TF.ERB,               -- 261 05x09
  TF.D|TF.W|TF.ETR,                    -- 262 06x09
  TF.D|TF.W|TF.ELT,                    -- 263 07x09
  TF.F|TF.D|TF.W|TF.ERB,               -- 264 08x09
  TF.D|TF.W|TF.ETR,                    -- 265 09x09
  TF.D|TF.W|TF.ETR,                    -- 266 10x09
  TF.F|TF.D|TF.W|TF.EL,                -- 267 11x09
  TF.F|TF.D|TF.W|TF.EB,                -- 268 12x09
  TF.D|TF.W|TF.ELT,                    -- 269 13x09
  TF.F|TF.D|TF.W|TF.EB,                -- 270 14x09
  TF.D|TF.W|TF.ELTR,                   -- 271 15x09
  TF.F|TF.D|TF.W|TF.EL,                -- 272 16x09
  TF.F|TF.D|TF.W|TF.ELB,               -- 273 17x09
  TF.D|TF.W|TF.ELT,                    -- 274 18x09
  TF.F|TF.D|TF.W|TF.ELB,               -- 275 19x09
  TF.F|TF.D|TF.W|TF.ELB,               -- 276 20x09
  TF.F|TF.D|TF.W|TF.ELB,               -- 277 21x09
  TF.D|TF.W|TF.EB,                     -- 278 22x09
  TF.F|TF.D|TF.W|TF.ELB,               -- 279 23x09
  TF.D|TF.W|TF.ERB,                    -- 280 24x09
  TF.D|TF.W|TF.ELT,                    -- 281 25x09
  TF.D|TF.W|TF.ELB,                    -- 282 26x09
  TF.F|TF.D|TF.W|TF.ELT,               -- 283 27x09
  TF.D|TF.W|TF.ELT,                    -- 284 28x09
  TF.F|TF.D|TF.W|TF.ER,                -- 285 29x09
  TF.F|TF.D|TF.W|TF.EB,                -- 286 30x09
  TF.F|TF.D|TF.W|TF.ERB,               -- 287 31x09 Flood cave ceiling left
  TF.F|TF.D|TF.W|TF.ELRB,              -- 288 32x09 Flood cave ceiling 1
  TF.F|TF.D|TF.W|TF.ELRB,              -- 289 01x10 Flood cave ceiling 2
  TF.D|TF.W|TF.ETRB,                   -- 290 02x10 Flood cave left 1
  TF.D|TF.W|TF.ETRB,                   -- 291 03x10 Flood cave left 2
  TF.D|TF.W|TF.ETR,                    -- 292 04x10 Flood cave floor left
  TF.D|TF.W|TF.ELTR,                   -- 293 05x10 Flood cave floor 1
  TF.D|TF.W|TF.ELTR,                   -- 294 06x10 Flood cave floor 2
  TF.F|TF.D|TF.W|TF.ELB,               -- 295 07x10 Flood cave ceiling right
  TF.D|TF.W|TF.ELTB,                   -- 296 08x10 Flood cave right 1
  TF.D|TF.W|TF.ELTB,                   -- 297 09x10 Flood cave right 2
  TF.D|TF.W|TF.ELT,                    -- 298 10x10 Flood cave floor right
  TF.D|TF.W|TF.EA,                     -- 299 11x10 Flood cave floor bot-left
  TF.D|TF.W|TF.EA,                     -- 300 12x10 Flood cave ceil top-left
  TF.D|TF.W|TF.EA,                     -- 301 13x10 Flood cave ceil top-right
  TF.D|TF.W|TF.EA,                     -- 302 14x10 Flood cave floor bot-right
  TF.D|TF.W|TF.ELRB,                   -- 303 15x10 Flood elevator ceiling
  TF.F,                                -- 304 16x10 Outside top-right ceiling 1
  TF.F,                                -- 305 17x10 Outside top-right ceiling 2
  TF.F,                                -- 306 18x10 Outside ceiling
  TF.F,                                -- 307 19x10 Outside top-left ceiling 1
  TF.F,                                -- 308 20x10 Outside top-left ceiling 2
  TF.F,                                -- 309 21x10 Corpse column 1 row 1
  TF.F,                                -- 310 22x10 Corpse column 2 row 1
  TF.F,                                -- 311 23x10 Corpse column 3 row 1
  TF.F,                                -- 312 24x10 Corpse column 4 row 1
  TF.F,                                -- 313 25x10 Corpse column 1 row 2
  TF.F,                                -- 314 26x10 Corpse column 2 row 2
  TF.F,                                -- 315 27x10 Corpse column 3 row 2
  TF.F,                                -- 316 28x10 Corpse column 4 row 2
  TF.D|TF.W|TF.P|TF.EA,                -- 317 29x10 Top of gold pillar
  TF.D|TF.W|TF.P|TF.EA,                -- 318 30x10 Vertical gold pillar
  TF.D|TF.W|TF.P|TF.EA,                -- 319 31x10 Bottom of gold pillar
  TF.D|TF.W|TF.P|TF.EA,                -- 320 32x10 Broken top pillar
  TF.D|TF.W|TF.P|TF.EA,                -- 321 01x11 Gold house bottom
  TF.D|TF.W|TF.P|TF.EA,                -- 322 02x11 Gold house top
  TF.D|TF.W|TF.P|TF.EA,                -- 323 03x11 Broken half top pillar
  TF.D|TF.W|TF.P|TF.EA,                -- 324 04x11 Broken horizontal top
  TF.D|TF.W|TF.P|TF.EA,                -- 325 05x11 Broken horizontal bottom
  TF.D|TF.W,                           -- 326 06x11 Unused
  TF.NONE,                             -- 327 07x11 Oil rig col 1 row 2
  TF.NONE,                             -- 328 08x11 Oil rig col 2 row 2
  TF.D|TF.W|TF.P|TF.EA,                -- 329 09x11 Water dug tile track end L
  TF.NONE,                             -- 330 10x11 Oil rig col 1 row 1 anim 1
  TF.NONE,                             -- 331 11x11 Oil rig col 2 row 1 anim 1
  TF.D|TF.W|TF.P|TF.EA,                -- 332 12x11 Water dug tile track end R
  TF.NONE,                             -- 333 13x11 Oil rig col 1 row 1 anim 2
  TF.NONE,                             -- 334 14x11 Oil rig col 2 row 1 anim 2
  TF.NONE,                             -- 335 15x11 Unused
  TF.D|TF.W|TF.EA,                     -- 336 16x11 Water dug tile with window
  TF.D|TF.W|TF.EA,                     -- 337 17x11 Water dug tile with skull
  TF.NONE,                             -- 338 18x11 Unused
  TF.NONE,                             -- 339 19x11 Unused
  TF.NONE,                             -- 340 20x11 Unused
  TF.NONE,                             -- 341 21x11 Unused
  TF.NONE,                             -- 342 22x11 Sun top-left
  TF.NONE,                             -- 343 23x11 Sun top-right
  TF.NONE,                             -- 344 24x11 Moon top-left
  TF.NONE,                             -- 345 25x11 Moon top-right
  TF.NONE,                             -- 346 26x11 Moon bottom-right
  TF.NONE,                             -- 347 27x11
  TF.NONE,                             -- 348 28x11
  TF.NONE,                             -- 349 29x11
  TF.NONE,                             -- 350 30x11
  TF.NONE,                             -- 351 31x11
  TF.NONE,                             -- 352 32x11
  TF.NONE,                             -- 353 01x12 Cave entrance right
  TF.NONE,                             -- 354 02x12 Cave entrance left
  TF.NONE,                             -- 355 03x12 Cave entrance down
  TF.NONE,                             -- 356 04x12 Unused
  TF.NONE,                             -- 357 05x12 Unused
  TF.NONE,                             -- 358 06x12 Unused
  TF.NONE,                             -- 359 07x12 Unused
  TF.NONE,                             -- 360 08x12 Unused
  TF.NONE,                             -- 361 09x12 Outside decoration only
  TF.NONE,                             -- 362 10x12 Outside decoration only
  TF.NONE,                             -- 363 11x12 Outside decoration only
  TF.NONE,                             -- 364 12x12 Outside decoration only
  TF.NONE,                             -- 365 13x12 Outside decoration only
  TF.NONE,                             -- 366 14x12 Outside decoration only
  TF.NONE,                             -- 367 15x12 Outside decoration only
  TF.NONE,                             -- 368 16x12 Outside decoration only
  TF.NONE,                             -- 369 17x12 Outside decoration only
  TF.NONE,                             -- 370 18x12 Outside decoration only
  TF.NONE,                             -- 371 19x12 Outside decoration only
  TF.NONE,                             -- 372 20x12 Outside decoration only
  TF.NONE,                             -- 373 21x12 Outside decoration only
  TF.NONE,                             -- 374 22x12 Outside decoration only
  TF.D|TF.W|TF.P|TF.EA,                -- 375 23x12 Water monument top-left
  TF.D|TF.W|TF.P|TF.EA,                -- 376 24x12 Water monument top right
  TF.D|TF.W|TF.P|TF.EA,                -- 377 25x12 Water monument bot-left
  TF.D|TF.W|TF.P|TF.EA,                -- 378 26x12 Water monument bot-right
  TF.NONE,                             -- 379 27x12 Outside decoration only
  TF.NONE,                             -- 380 28x12 Outside decoration only
  TF.NONE,                             -- 381 29x12 Outside decoration only
  TF.NONE,                             -- 382 30x12 Outside decoration only
  TF.NONE,                             -- 383 31x12 Outside decoration only
  TF.NONE,                             -- 384 32x12 Outside decoration only
  TF.NONE,                             -- 385 01x13 Outside decoration only
  TF.NONE,                             -- 386 02x13 Outside decoration only
  TF.NONE,                             -- 387 03x13 Outside decoration only
  TF.NONE,                             -- 388 04x13 Outside decoration only
  TF.NONE,                             -- 389 05x13 Outside decoration only
  TF.D|TF.W|TF.AD|TF.T|TF.P|TF.EA,     -- 390 06x13 Water clear light track
  TF.D|TF.W|TF.AD|TF.EA,               -- 391 07x13 Water clear light
  TF.NONE,                             -- 392 08x13 Outside decoration only
  TF.NONE,                             -- 393 09x13 Outside decoration only
  TF.NONE,                             -- 394 10x13 Outside decoration only
  TF.NONE,                             -- 395 11x13 Outside decoration only
  TF.NONE,                             -- 396 12x13 Outside decoration only
  TF.NONE,                             -- 397 13x13 Outside decoration only
  TF.NONE,                             -- 398 14x13 Outside decoration only
  TF.NONE,                             -- 399 15x13 Outside decoration only
  TF.NONE,                             -- 400 16x13 Outside decoration only
  TF.NONE,                             -- 401 17x13 Outside decoration only
  TF.NONE,                             -- 402 18x13 Outside decoration only
  TF.NONE,                             -- 403 19x13 Outside decoration only
  TF.NONE,                             -- 404 20x13 Outside decoration only
  TF.NONE,                             -- 405 21x13 Outside decoration only
  TF.NONE,                             -- 406 22x13 Outside decoration only
  TF.F,                                -- 407 23x13 Outside ceiling left
  TF.F,                                -- 408 24x13 Outside ceiling straight
  TF.F,                                -- 409 25x13 Outside ceiling right
  TF.D|TF.W|TF.T|TF.P|TF.EA|TF.AD,     -- 410 26x13 Water beam forward track
  TF.D|TF.W|TF.EA|TF.AD,               -- 411 27x13 Water beam forward
  TF.D|TF.W|TF.EA|TF.AD,               -- 412 28x13 Water beam backward
  TF.D|TF.W|TF.EA|TF.AD,               -- 413 29x13 Water beam horizontal
  TF.D|TF.W|TF.ELTR,                   -- 414 30x13 Water dig down 1/3
  TF.D|TF.W|TF.ELTR,                   -- 415 31x13 Water dig down 2/3
  TF.D|TF.W|TF.ELTR,                   -- 416 32x13 Water dig down 3/3
  TF.NONE,                             -- 417 01x14 Top-right cliff ceiling
  TF.NONE,                             -- 418 02x14 Bottom-left cliff floor
  TF.NONE,                             -- 419 03x14 Top-left cliff ceiling
  TF.NONE,                             -- 420 04x14 Top-right cliff ceiling
  TF.NONE,                             -- 421 05x14 Bottom-right cliff floor
  TF.NONE,                             -- 422 06x14 Unused
  TF.NONE,                             -- 423 07x14 Unused
  TF.NONE,                             -- 424 08x14 Unused
  TF.NONE,                             -- 425 09x14 Unused
  TF.F,                                -- 426 10x14 Outside ceil L/R straight
  TF.F,                                -- 427 11x14 Outside ceil R/D straight
  TF.F,                                -- 428 12x14 Outside ceil U/R straight
  TF.NONE,                             -- 429 13x14 Vertical dug leads outside
  TF.D|TF.W|TF.E|TF.EA,                -- 430 14x14 Elevator wire (water)
  TF.NONE,                             -- 431 15x14 Elevator foundation
  TF.NONE,                             -- 432 16x14 Out half ceil D/R corner
  TF.F,                                -- 433 17x14 Out half ceil L/R straight
  TF.NONE,                             -- 434 18x14 Out half ceil R/U corner
  TF.D|TF.P|TF.EL|TF.ER|TF.G,          -- 435 19x14 Flood Gate (No water)
  TF.D|TF.P|TF.EL|TF.ER|TF.G,          -- 436 20x14 Flood Gate (Water left)
  TF.D|TF.P|TF.EL|TF.ER|TF.G,          -- 437 21x14 Flood Gate (Water right)
  TF.D|TF.W|TF.D|TF.P|TF.EL|TF.ER|TF.G,-- 438 22x14 Flood Gate (All water)
  TF.D|TF.P|TF.EL|TF.ER|TF.G,          -- 439 23x14 Flood Gate open (No water)
  TF.D|TF.W|TF.D|TF.P|TF.EL|TF.ER|TF.G,-- 440 24x14 Flood Gate open (Water)
  TF.NONE,                             -- 441 25x14 Desert crystal 1
  TF.NONE,                             -- 442 26x14 Desert crystal 2
  TF.NONE,                             -- 443 27x14 Unused
  TF.D|TF.W|TF.ETR,                    -- 444 28x14 Diagonal up-left flooded
  TF.F|TF.D|TF.W|TF.EB,                -- 445 29x14 Diagonal up-left flooded
  TF.F|TF.D|TF.W|TF.ER,                -- 446 30x14 Diagonal up-left flooded
  TF.D|TF.W|TF.ELTR,                   -- 447 31x14 Diagonal up-left flooded
  TF.F|TF.D|TF.W|TF.ERB,               -- 448 32x14 Diagonal up-left flooded
  TF.D|TF.W|TF.ETR,                    -- 449 01x15 Diagonal up-left flooded
  TF.F|TF.D|TF.W|TF.ERB,               -- 450 02x15 Diagonal up-left flooded
  TF.D|TF.W|TF.AD|TF.T|TF.P|TF.EA,     -- 451 03x15 Watered clear track tile
  TF.D|TF.W|TF.ETR,                    -- 452 04x15 Diagonal up-right flooded
  TF.D|TF.W|TF.ETRB,                   -- 453 05x15 Diagonal up-right flooded
  TF.D|TF.W|TF.ETR,                    -- 454 06x15 Diagonal up-right flooded
  TF.D|TF.W|TF.ETR,                    -- 455 07x15 Diagonal up-right flooded
  TF.D|TF.W|TF.ELTB,                   -- 456 08x15 Diagonal up-right flooded
  TF.F|TF.D|TF.W|TF.ELB,               -- 457 09x15 Diagonal up-right flooded
  TF.D|TF.W|TF.ELT,                    -- 458 10x15 Diagonal up-right flooded
  TF.F|TF.D|TF.W|TF.ELRB,              -- 459 11x15 Diagonal up-right flooded
  TF.D|TF.W|TF.ELT,                    -- 460 12x15
  TF.D|TF.W|TF.ELTB,                   -- 461 13x15
  TF.D|TF.W|TF.ELB,                    -- 462 14x15
  TF.D|TF.W|TF.ELB,                    -- 463 15x15
  TF.D|TF.W|TF.ETRB,                   -- 464 16x15
  TF.D|TF.W|TF.F|TF.ERB,               -- 465 17x15
  TF.D|TF.W|TF.ETRB,                   -- 466 18x15
  TF.D|TF.W|TF.F|TF.ELRB,              -- 467 19x15
  TF.D|TF.W|TF.ELTB,                   -- 468 20x15
  TF.D|TF.W|TF.ELTB,                   -- 469 21x15
  TF.D|TF.W|TF.ELTB,                   -- 470 22x15
  TF.D|TF.W|TF.ETRB,                   -- 471 23x15
  TF.D|TF.W|TF.ETRB,                   -- 472 24x15
  TF.D|TF.W|TF.ETRB,                   -- 473 25x15
  TF.D|TF.W,                           -- 474 26x15
  TF.NONE,                             -- 475 27x15
  TF.D|TF.W|TF.P|TF.EA,                -- 476 28x15
  TF.D|TF.W|TF.P|TF.EA,                -- 477 29x15
  TF.NONE,                             -- 478 30x15 Unused
  TF.NONE,                             -- 479 31x15 Unused
  TF.NONE,                             -- 480 32x15 Unused
  TF.NONE,                             -- 481 01x16 Unused
  TF.NONE,                             -- 482 02x16 Unused
  TF.NONE,                             -- 483 03x16 Unused
  TF.NONE,                             -- 484 04x16 Unused
  TF.NONE,                             -- 485 05x16 Unused
  TF.NONE,                             -- 486 06x16 Unused
  TF.NONE,                             -- 487 07x16 Unused
  TF.NONE,                             -- 488 08x16 Unused
  TF.NONE,                             -- 489 09x16 Unused
  TF.NONE,                             -- 490 10x16 Unused
  TF.NONE,                             -- 491 11x16 Unused
  TF.NONE,                             -- 492 12x16 Unused
  TF.NONE,                             -- 493 13x16 Unused
  TF.NONE,                             -- 494 14x16 Unused
  TF.NONE,                             -- 495 15x16 Unused
  TF.NONE,                             -- 496 16x16 Unused
  TF.NONE,                             -- 497 17x16 Unused
  TF.NONE,                             -- 498 18x16 Unused
  TF.NONE,                             -- 499 19x16 Unused
  TF.NONE,                             -- 500 20x16 Unused
  TF.NONE,                             -- 501 21x16 Unused
  TF.NONE,                             -- 502 22x16 Unused
  TF.NONE,                             -- 503 23x16 Unused
  TF.NONE,                             -- 504 24x16 Unused
  TF.NONE,                             -- 505 25x16 Unused
  TF.NONE,                             -- 506 26x16 Unused
  TF.NONE,                             -- 507 27x16 Unused
  TF.NONE,                             -- 508 28x16 Unused
  TF.NONE,                             -- 509 29x16 Unused
  TF.NONE,                             -- 510 30x16 Unused
  TF.NONE,                             -- 511 31x16 Unused
  TF.NONE,                             -- 512 32x16 Unused
};
assert(#aTileData == 512, "aTileData must only have 512 tiles!");
-- Explode directions data ------------------------------------------------- --
local aExplodeDirData<const> = {
  -- X -- Y -- Flags -----
  {   0,  -1, TF.W|TF.EB },            -- [Up] Flood if above tile exposed
  {   0,   0, TF.W       },            -- [Centre] No flooding check needed
  {  -1,   0, TF.W|TF.ER },            -- [Left] Flood if left tile exposed
  {   1,   0, TF.W|TF.EL },            -- [Right] Flood if right tile exposed
  {   0,   1, TF.W|TF.ET },            -- [Down] Flood if below tile exposed
};
-- Explode directions data ------------------------------------------------- --
local aExplodeAboveData<const> = {
  [ 88] =   7, -- Remove left end of track and set clear tile
  [ 91] =   7, -- Remove right end of track and set clear tile
  [149] = 150, -- Remove track from dug tile with light
  [169] = 170, -- Remove track from dug tile with forward beam
  [210] =   7, -- Remove track from dug tile beam backwards
  [328] = 247, -- Remove watered right end of track and set to cleared water
  [331] = 247, -- Remove watered left end of track and set to cleared water
  [389] = 390, -- Remove watered light and set to watered light
  [409] = 410, -- Remove watered beam forward and set to watered beam forward
  [450] = 247, -- Remove watered clear track and set to clear
};
-- Train track data -------------------------------------------------------- --
local aTrainTrackData<const> = {
  [  7] = 210, -- Dug tile to clear track tile
  [171] = 210, -- Dug tile beam backwards to clear track tile
  [172] = 210, -- Dug tile beam horizontal to clear track tile
  [150] = 149, -- Dug tile with light to light tile with track
  [170] = 169, -- Dug tile with forward beam to same version with track
  [247] = 450, -- Dug tile with watered cleared to track tile
  [329] = 450, -- Dug tile with watered backward beam to tile with track
  [330] = 450, -- Dug tile with watered horizontal beam to tile with track
  [390] = 389, -- Dug tile with watered light to watered light with track
  [410] = 409, -- Dug tile with watered beam forward to tile with track
};
-- Flood gate data --------------------------------------------------------- --
local aFloodGateData<const> = {
  -- (TID=Tile Id, FFL=Flood from left, FFR=Flood right)
  -- TID  FFR  CON   FFL   CON         (CON=Continue flooding)
  [434]={{436,false},{435,false}}, -- Gate is closed and dry on both sides?
  [435]={{435,false},{437,false}}, -- Gate is closed and wet on the left?
  [436]={{436,false},{437,false}}, -- Gate is closed and wet on the right?
  [437]={{437,false},{437,false}}, -- Gate is closed and wet on both sides?
  [438]={{439,true },{439,true }}, -- Gate is open and dry
  [439]={{439,true },{439,true }}, -- Gate is open and wet
  -- TID  FFR  CON   FFL   CON
};
-- Menu data --------------------------------------------------------------- --
local aMenuData<const> = {
  --  ID         W H
  [MNU.MAIN]  = {8,1,{
  -- BID FLAGS     SUBMENU   ACTION    JOB        DIREC    TIP
    {765,0,        MNU.MOVE, 0,        0,         0,       "MOVE" },
    {761,0,        MNU.DIG,  0,        0,         0,       "DIG" },
    {755,0,        MNU.NONE, ACT.GRAB, JOB.KEEP,  DIR.KEEP,"GRAB" },
    {756,0,        MNU.DROP, 0,        0,         0,       "INVENTORY" },
    {762,MFL.BUSY, MNU.NONE, ACT.KEEP, JOB.HOME,  DIR.HOME,"GO HOME" },
    {760,MFL.BUSY, MNU.NONE, ACT.STOP, JOB.NONE,  DIR.NONE,"HALT" },
    {759,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.SEARCH,DIR.LR,  "SEARCH" },
    {772,MFL.BUSY, MNU.NONE, ACT.PHASE,JOB.PHASE, DIR.U,   "TELEPORT" },
  -- BID FLAGS     SUBMENU   ACTION    JOB        DIREC    TIP
               }    },
  --  ID        W H
  [MNU.MOVE]  ={4,2,{
  -- BID FLAGS     SUBMENU   ACTION    JOB        DIREC    TIP
    {750,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.KNDD,  DIR.L,   "WALK LEFT" },
    {763,MFL.BUSY, MNU.NONE, ACT.JUMP, JOB.KEEP,  DIR.KEEP,"JUMP" },
    {751,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.KNDD,  DIR.R,   "WALK RIGHT" },
    {760,MFL.BUSY, MNU.NONE, ACT.STOP, JOB.NONE,  DIR.NONE,"HALT" },
    {753,MFL.BUSY, MNU.NONE, ACT.RUN,  JOB.KNDD,  DIR.L,   "RUN LEFT" },
    {752,MFL.BUSY, MNU.NONE, ACT.STOP, JOB.KNDD,  DIR.KEEP,"STOP" },
    {754,MFL.BUSY, MNU.NONE, ACT.RUN,  JOB.KNDD,  DIR.R,   "RUN RIGHT" },
    {771,0,        MNU.MAIN, 0,        0,         0,       "MAIN MENU" },
  -- BID FLAGS     SUBMENU   ACTION    JOB        DIREC    TIP
               }    },
  --  ID        W H
  [MNU.DIG]   ={4,2,{
  -- BID FLAGS     SUBMENU   ACTION    JOB          DIREC    TIP
    {773,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.DIG,     DIR.UL,  "DIG UP-LEFT" },
    {750,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.DIG,     DIR.L,   "DIG LEFT" },
    {751,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.DIG,     DIR.R,   "DIG RIGHT" },
    {775,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.DIG,     DIR.UR,  "DIG UP-RIGHT" },
    {774,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.DIG,     DIR.DL,  "DIG DN-LEFT" },
    {764,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.DIGDOWN, DIR.TCTR,"DIG DOWN" },
    {771,0,        MNU.MAIN, 0,        0,           0,       "MAIN MENU" },
    {776,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.DIG,     DIR.DR,  "DIG DN-RIGHT" },
  -- BID FLAGS     SUBMENU   ACTION    JOB          DIREC    TIP
               }    },
  --  ID        W H
  [MNU.DROP]  ={3,2,{
  -- BID FLAGS  SUBMENU   ACTION    JOB        DIREC    TIP
    {777,0,     MNU.NONE, ACT.NEXT, JOB.KEEP,  DIR.KEEP,"NEXT ITEM" },
    {779,0,     MNU.NONE, ACT.DROP, JOB.KEEP,  DIR.KEEP,"DROP ITEM" },
    {780,0,     MNU.NONE, ACT.DROP, JOB.KEEP,  DIR.KEEP,"DROP ITEM" },
    {778,0,     MNU.MAIN, 0,        0,         0,       "MAIN MENU" },
    {781,0,     MNU.NONE, ACT.DROP, JOB.KEEP,  DIR.KEEP,"DROP ITEM" },
    {782,0,     MNU.NONE, ACT.DROP, JOB.KEEP,  DIR.KEEP,"DROP ITEM" },
  -- BID FLAGS  SUBMENU   ACTION    JOB        DIREC    TIP
               }    },
  --  ID        W H
  [MNU.TUNNEL]={3,1,{
  -- BID FLAGS     SUBMENU   ACTION    JOB        DIREC    TIP
    {750,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.DIG,   DIR.L,   "DIG LEFT" },
    {752,MFL.BUSY, MNU.NONE, ACT.STOP, JOB.NONE,  DIR.KEEP,"HALT" },
    {751,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.DIG,   DIR.R,   "DIG RIGHT" },
  -- BID FLAGS     SUBMENU   ACTION    JOB        DIREC    TIP
               }    },
  --  ID        W H
  [MNU.CORK]  ={2,2,{
  -- BID FLAGS     SUBMENU   ACTION    JOB         DIREC    TIP
    {750,MFL.BUSY, MNU.NONE, ACT.CREEP,JOB.NONE,   DIR.L,   "GO LEFT" },
    {751,MFL.BUSY, MNU.NONE, ACT.CREEP,JOB.NONE,   DIR.R,   "GO RIGHT" },
    {764,MFL.BUSY, MNU.NONE, ACT.CREEP,JOB.DIGDOWN,DIR.TCTR,"DIG DOWN" },
    {752,MFL.BUSY, MNU.NONE, ACT.STOP, JOB.NONE,   DIR.KEEP,"HALT" },
  -- BID FLAGS     SUBMENU   ACTION    JOB         DIREC    TIP
               }    },
  --  ID        W H
  [MNU.TNT]   ={1,1,{
  -- BID FLAGS     SUBMENU   ACTION    JOB       DIREC    TIP
    {769,MFL.BUSY, MNU.NONE, ACT.DYING,JOB.NONE, DIR.NONE,"DETONATE" },
  -- BID FLAGS     SUBMENU   ACTION    JOB       DIREC    TIP
               }    },
  --  ID        W H
  [MNU.MAP]   ={1,1,{
    -- BID FLAGS   SUBMENU   ACTION    JOB       DIREC    TIP
    {783,MFL.BUSY, MNU.NONE, ACT.MAP,  JOB.NONE, DIR.NONE,"DISPLAY" },
    -- BID FLAGS   SUBMENU   ACTION    JOB       DIREC    TIP
               }          },
  --  ID        W H
  [MNU.TRAIN] ={3,2,{
    -- BID FLAGS   SUBMENU   ACTION    JOB        DIREC    TIP
    {750,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.NONE,  DIR.L,   "GO LEFT" },
    {752,MFL.BUSY, MNU.NONE, ACT.STOP, JOB.NONE,  DIR.KEEP,"HALT" },
    {751,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.NONE,  DIR.R,   "GO RIGHT" },
    {755,0,        MNU.NONE, ACT.GRAB, JOB.KEEP,  DIR.KEEP,"GRAB" },
    {756,0,        MNU.NONE, ACT.DROP, JOB.KEEP,  DIR.KEEP,"INVENTORY" },
    {759,MFL.BUSY, MNU.NONE, ACT.WALK, JOB.SEARCH,DIR.LR,  "SEARCH" },
    -- BID FLAGS   SUBMENU   ACTION    JOB        DIREC    TIP
               }    },
  --  ID        W H
  [MNU.TRDROP]={3,2,{
    -- BID FLAGS  SUBMENU   ACTION    JOB        DIREC    TIP
    {777,0,       MNU.NONE, ACT.NEXT, JOB.KEEP,  DIR.KEEP,"NEXT ITEM" },
    {779,0,       MNU.NONE, ACT.DROP, JOB.KEEP,  DIR.KEEP,"DROP ITEM" },
    {780,0,       MNU.NONE, ACT.DROP, JOB.KEEP,  DIR.KEEP,"DROP ITEM" },
    {778,0,       MNU.TRAIN,0,        0,         0,       "MAIN CONTROL" },
    {781,0,       MNU.NONE, ACT.DROP, JOB.KEEP,  DIR.KEEP,"DROP ITEM" },
    {782,0,       MNU.NONE, ACT.DROP, JOB.KEEP,  DIR.KEEP,"DROP ITEM" },
    -- BID FLAGS  SUBMENU   ACTION    JOB        DIREC    TIP
               }    },
  --  ID        W H
  [MNU.FLOAT] ={3,1,{
    -- BID FLAGS   SUBMENU   ACTION    JOB        DIREC    TIP
    {750,MFL.BUSY, MNU.NONE, ACT.CREEP,JOB.NONE,  DIR.L,   "MOVE LEFT" },
    {752,MFL.BUSY, MNU.NONE, ACT.STOP, JOB.NONE,  DIR.KEEP,"HALT" },
    {751,MFL.BUSY, MNU.NONE, ACT.CREEP,JOB.NONE,  DIR.R,   "MOVE RIGHT" },
    -- BID FLAGS   SUBMENU   ACTION    JOB        DIREC    TIP
               }    },
  --  ID        W H
  [MNU.GATE]  ={2,1,{
    -- BID FLAGS   SUBMENU   ACTION    JOB        DIREC    TIP
    {767,MFL.BUSY, MNU.NONE, ACT.CLOSE,JOB.NONE,  DIR.NONE,"CLOSE" },
    {768,MFL.BUSY, MNU.NONE, ACT.OPEN, JOB.NONE,  DIR.NONE,"OPEN" },
    -- BID FLAGS   SUBMENU   ACTION    JOB        DIREC    TIP
               }    },
  --  ID        W H
  [MNU.DEPLOY]={1,1,{
    -- BID FLAGS   SUBMENU   ACTION     JOB       DIREC    TIP
    {770,MFL.BUSY, MNU.NONE, ACT.DEPLOY,JOB.NONE, DIR.NONE,"DEPLOY" },
    -- BID FLAGS   SUBMENU   ACTION     JOB       DIREC    TIP
               }    },
  --  ID        W H
  [MNU.LIFT]  ={3,1,{
    -- BID FLAGS   SUBMENU   ACTION     JOB       DIREC    TIP
    {763,MFL.BUSY, MNU.NONE, ACT.CREEP, JOB.NONE, DIR.U,   "ASCEND" },
    {752,MFL.BUSY, MNU.NONE, ACT.CREEP, JOB.NONE, DIR.NONE,"STOP" },
    {764,MFL.BUSY, MNU.NONE, ACT.CREEP, JOB.NONE, DIR.D,   "DESCEND" },
    -- BID FLAGS   SUBMENU   ACTION     JOB       DIREC    TIP
               }    },
  --  ID        W H
};
-- Endings data ------------------------------------------------------------ --
local aEndingData<const> = {
  [TYP.FTARG]    = { 1, 0, "THE F'TARGS CONSTRUCT THEIR OWN MUSEUM",
                           "OF METAL MARVELS WITH THEIR EARNINGS" },
  [TYP.HABBISH]  = { 1, 1, "THE HABBISH CONSTRUCT A TEMPLE IN HONOUR OF",
                           "THE LORD HIGH HABBORG WITH THEIR EARNINGS" },
  [TYP.GRABLIN]  = { 2, 0, "THE GRABLINS CONSTRUCT THEIR OWN GROK",
                           "BREWERY WITH THEIR EARNINGS" },
  [TYP.QUARRIOR] = { 2, 1, "THE QUARRIORS CONSTRUCT AN IMPENETRABLE",
                           "FORTRESS WITH THEIR EARNINGS" }
};
-- Credits data ------------------------------------------------------------ --
local aCreditsData<const> = {
  { "Diggers by",                      "Toby Simpson" },
  { "Designed & programmed by",        "Mike Froggatt\n\z
                                        Toby Simpson" },
  { "Additional DOS Programming",      "Keith Hook" },
  { "Graphics",                        "Jason Wilson" },
  { "Music and Effects",               "Richard Joseph\n\z
                                        Graham King" },
  { "Additional graphics",             "Tony Heap\n\z
                                        Jason Riley" },
  { "Narrative text",                  "Martin Oliver" },
  { "Additional Design",               "Michael Hayward\n\z
                                        Ian Saunter\n\z
                                        Tony Fagelman" },
  { "Level Design",                    "Toby Simpson\n\z
                                        Tony Fagelman" },
  { "Quality Assurance",               "Steve Murphy\n\z
                                        Paul Dobson\n\z
                                        Kelly Thomas" },
  { "Introduction Sequence",           "Mike Ball\n\z
                                        Mike Froggatt" },
  { "E-Book Production",               "Alan Brand\n\z
                                        Steve Loughran\n\z
                                        Martin Oliver\n\z
                                        Tony Fagelman" },
  { "Thanks to",                       "Chris Ludwig\n\z
                                        Wayne Lutz\n\z
                                        Dave Pocock\n\z
                                        Sharon McGuffie\n\z
                                        Ben Simpson" },
  { "Produced by",                     "Tony Fagelman\n\z
                                        Millennium Interactive" }

};
-- Extra credits ----------------------------------------------------------- --
local aCreditsXData<const> = {
  { "Complete conversion",             "Mhatxotic Design" },
  { "Setup music loop",                "S.S. Secret Mission 1\n\z
                                        By PowerTrace\n\z
                                        Edited by Mhat\n\z
                                        AmigaRemix.com" },
  { "Credits music loop",              "4U 07:00 V2001\n\z
                                        By Enuo\n\z
                                        Edited by Mhat\n\z
                                        ModArchive.org" },
  { "Gameover music loop",             "1000 Years Of Funk\n\z
                                        By Dimitri D. L.\n\z
                                        Edited by Mhat\n\z
                                        ModArchive.Org" },
  { "Conversion powered by",           "Mhatxotic Engine" },
  { "GLFW OpenGL front-end",           "Marcus Geelnard\n\z
                                        Camilla Lowy" },
  { "LUA scripting engine",            "Lua.org, PUC-Rio" },
  { "OpenALSoft audio manager",        "Chris Robinson\n\z
                                        Creative Technology" },
  { "Ogg, Vorbis & Theora A/V codec",  "Xiph.Org" },
  { "FreeType font rendering",         "The FreeType Project" },
  { "SQLite database engine",          "SQLite Consortium" },
  { "LibJPEG-Turbo image codec",       "Contributing authors\n\z
                                        IJG" },
  { "LibNSGif image decoder",          "NetSurf Developers\n\z
                                        Richard Wilson\n\z
                                        Sean Fox" },
  { "LibPNG image codec",              "Contributing authors" },
  { "LZMA general codec",              "Igor Pavlov" },
  { "Z-Lib general codec",             "Jean-loup Gailly\n\z
                                        Mark Adler" },
  { "OpenSSL crypto & socket engine",  "OpenSSL SW Foundation" },
  { "RapidJSON parsing engine",        "THL A29 Ltd.\n\z
                                        Tencent co.\n\z
                                        Milo Yip" },
  { "Special thanks",                  "ModArchive.Org\n\z
                                        AmigaRemix.Com\n\z
                                        Toby Simpson\n\z
                                        You!" },
  { "Thank you for playing!",          "The End!" },
};
-- Setup buttons data ------------------------------------------------------ --
local aSetupButtonData<const> = {
  -- -- Legend ------------------------------------------------------------- --
  -- [*1] Unique hot spot template.    [!2] Unique tip id.
  -- [*3] X text position.             [*4] Y text position.
  -- [!5] Button text.                 [!6] Scrolling text information tip.
  -- ! = static, * = Filled in at setup.lua > OnScriptLoaded() function.
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.EXIT, false, false, false }, 101, nil, nil,
    "CLOSE", "CLOSE THIS CONFIGURATION DIALOG AND RETURN TO THE GAME. YOU \z
     WILL LOSE ANY SYSTEM AFFECTING CHANGES THAT REQUIRE THE 'APPLY' BUTTON \z
     TO BE CLICKED" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.OK, false, false, false }, 101, nil, nil,
    "APPLY", "CLICK TO APPLY ANY SYSTEM AFFECTING SETTINGS YOU HAVE CHANGED" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.OK, false, false, false }, 103, nil, nil,
    "RESET", "CLICK TO RESET ALL VALUES TO DEFAULTS AND AUTOMATICALLY APPLY \z
     THE SETTINGS" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.OK, false, false, false }, 104, nil, nil,
    "BINDS", "CLICK TO CHANGE KEYBINDS FOR THIS GAME" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.OK, false, false, false }, 105, nil, nil,
    "ABOUT", "CLICK TO VIEW THE ACKNOWLEDGEMENTS FOR THIS GAME" },
};-- Setup options data ---------------------------------------------------- --
local aSetupOptionData<const> = {
  -- -- Legend ------------------------------------------------------------- --
  -- [*1] Unique hot spot template.    [*2] Update text value [7] callback.
  -- [*3] Y position of text line.     [*4] X position of left justified text.
  -- [!5] Category text.               [*6] X position of right justified text.
  -- [*7] Value text.                  [!8] Scrolling text information tip.
  -- ! = static, * = Filled in at setup.lua > OnScriptLoaded() function.
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Monitor",         0, "",
    "CHANGES THE MONITOR THE GAME WILL APPEAR ON BY DEFAULT. PRESS APPLY \z
     WHEN YOU ARE HAPPY WITH THE SELECTION TO ACTIVATE IT", },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Display State",   0, "",
    "CHANGES THE DEFAULT WINDOW STYLE OF THE GAME. PRESS APPLY WHEN YOU ARE \z
     HAPPY WITH THE SELECTION TO ACTIVATE IT" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Full-Resolution", 0, "",
    "ALLOWS YOU TO SET A CUSTOM DESKTOP RESOLUTION FOR EXCLUSIVE FULL-SCREEN \z
     MODE. PRESS APPLY WHEN YOU ARE HAPPY WITH THE SELECTION TO ACTIVATE IT" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Window Size",     0, "",
    "ALLOWS YOU TO SET A CUSTOM WINDOW SIZE FOR DECORATED WINDOW ONLY MODE. \z
     PRESS APPLY WHEN YOU ARE HAPPY WITH THE SELECTION TO ACTIVATE IT" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Frame Limiter",   0, "",
    "ALLOWS YOU TO CHOOSE FROM A RANGE OF FRAME-LIMITING OPTIONS TO BALANCE \z
     THE PERFORMANCE VERSUS POWER USAGE OF RENDERING. SOME VALUES MAY BE \z
     INEFFECTIVE WHEN THE VSYNC VALUE IS BEING OVERRIDDEN IN YOUR VIDEO \z
     ADAPTER SETTINGS OR WHEN YOUR SYSTEM IS UNDERPERFORMING. THE CHANGE IS \z
     INSTANTLY APPLIED" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Texture Filter",  0, "",
    "APPLY A BILINEAR UPSCALE FILTER TO THE MAIN FRAMEBUFFER. THE GAME IS \z
     RENDERED IN 320x240. THE CHANGE OF OPTION IS INSTANTLY APPLIED" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Audio Device",    0, "",
    "ALLOWS YOU TO SET THE DEFAULT AUDIO DEVICE TO USE FOR THE GAME. PRESS \z
     APPLY WHEN YOU ARE HAPPY WITH THE SELECTION TO ACTIVATE IT" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Master Volume",   0, "",
    "CHANGES THE FINAL OUTPUT VOLUME OF ALL MUSIC, SOUND EFFECTS AND FMV \z
     MIXED TOGETHER. THE CHANGE OF OPTION IS INSTANTLY APPLIED" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Music Volume",    0, "",
    "CHANGES THE MUSIC VOLUME. THE CHANGE OF OPTION IS INSTANTLY APPLIED" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Effect Volume",   0, "",
    "CHANGES THE SFX VOLUME. THE CHANGE OF OPTION IS INSTANTLY APPLIED" },
  -- ----------------------------------------------------------------------- --
  { { nil, nil, nil, nil, 0, CID.SELECT, false, false, false },
    false, 0, 0, "Fmv Volume",      0, "",
    "CHANGES THE FMV VOLUME. THE CHANGE OF OPTION IS INSTANTLY APPLIED" },
};-- Option name -- Value -- UpdateFunc --- DownFunc --- UpFunc ------------ --
local aIntroSubTitles<const> = {
  -- SHO - HID - LINES ----------------------------------------------------- --
  {  380,  440, { "This is no ordinary day on the planet Zarg." } },
  {  450,  510, { "Today is the glorious four-hundred and twelfth." } },
  {  520,  640, { "The day that each year, signals the commencement",
                  "of one months frenzied digging." } },
  {  650,  770, { "Four races of diggers, are tunneling their way",
                  "to the Zargon mineral trading centre." } },
  {  780,  810, { "They each have an ambition..." } },
  {  820,  930, { "That requires them to mine as much of the",
                  "planets mineral wealth, as possible." } },
  { 1150, 1280, { "Observing the quarrelsome diggers from afar,",
                  "is a mysterious stranger." } },
  { 1290, 1410, { "Each of the races are hoping that this 'stranger'",
                  "will control their mining operations." } },
  { 1420, 1550, { "His expertese, will be vital, in guiding",
                  "them along the long, dangerous path..." } },
  { 1560, 1620, { "...that leads them to their ultimate goal." } },
  { 1630, 1790, { "His first step, is to register, at the",
                  "Zargon mineral trading centre." } },
  { 1900, 2040, { "As the diggers wait nervously, the stranger",
                  "heads towards the trading centre." } },
  { 2050, 2180, { "For him, the ultimate test. The greatest",
                  "challenge of his life lies ahead." } },
  { 2200, 2290, { "The reward for success,",
                  "will be wealth unlimited." } },
  { 2310, 2440, { "The results of failure,",
                  "are unthinkable!" } },
};-- SHO - HID - LINES ----------------------------------------------------- --
-- Convert key index to literal string ------------------------------------- --
local aKeys<const> = Input.KeyCodes;
local aKeyToLiteral<const> = {
  [0]                   = "UNBOUND",   [aKeys.APOSTROPHE]    = "APOSTROPHE",
  [aKeys.A]             = "A",         [aKeys.BACKSLASH]     = "BACKSLASH",
  [aKeys.BACKSPACE]     = "BACKSPACE", [aKeys.B]             = "B",
  [aKeys.CAPS_LOCK]     = "CAPS LOCK", [aKeys.COMMA]         = "COMMA",
  [aKeys.C]             = "C",         [aKeys.DELETE]        = "DELETE",
  [aKeys.DOWN]          = "DOWN",      [aKeys.D]             = "D",
  [aKeys.END]           = "END",       [aKeys.ENTER]         = "ENTER",
  [aKeys.EQUAL]         = "EQUALS",    [aKeys.ESCAPE]        = "ESCAPE",
  [aKeys.E]             = "E",         [aKeys.F10]           = "F10",
  [aKeys.F11]           = "F11",       [aKeys.F12]           = "F12",
  [aKeys.F13]           = "F13",       [aKeys.F14]           = "F14",
  [aKeys.F15]           = "F15",       [aKeys.F16]           = "F16",
  [aKeys.F17]           = "F17",       [aKeys.F18]           = "F18",
  [aKeys.F19]           = "F19",       [aKeys.F1]            = "F1",
  [aKeys.F20]           = "F20",       [aKeys.F21]           = "F21",
  [aKeys.F22]           = "F22",       [aKeys.F23]           = "F23",
  [aKeys.F24]           = "F24",       [aKeys.F25]           = "F25",
  [aKeys.F2]            = "F2",        [aKeys.F3]            = "F3",
  [aKeys.F4]            = "F4",        [aKeys.F5]            = "F5",
  [aKeys.F6]            = "F6",        [aKeys.F7]            = "F7",
  [aKeys.F8]            = "F8",        [aKeys.F9]            = "F9",
  [aKeys.F]             = "F",         [aKeys.GRAVE_ACCENT]  = "GRAVE ACCENT",
  [aKeys.G]             = "G",         [aKeys.HOME]          = "HOME",
  [aKeys.H]             = "H",         [aKeys.INSERT]        = "INSERT",
  [aKeys.I]             = "I",         [aKeys.J]             = "J",
  [aKeys.KP_0]          = "KEYPAD 0",  [aKeys.KP_1]          = "KEYPAD 1",
  [aKeys.KP_2]          = "KEYPAD 2",  [aKeys.KP_3]          = "KEYPAD 3",
  [aKeys.KP_4]          = "KEYPAD 4",  [aKeys.KP_5]          = "KEYPAD 5",
  [aKeys.KP_6]          = "KEYPAD 6",  [aKeys.KP_7]          = "KEYPAD 7",
  [aKeys.KP_8]          = "KEYPAD 8",  [aKeys.KP_9]          = "KEYPAD 9",
  [aKeys.KP_ADD]        = "KEYPAD ADD",
  [aKeys.KP_DECIMAL]    = "KEYPAD DECIMAL",
  [aKeys.KP_DIVIDE]     = "KEYPAD DIVIDE",
  [aKeys.KP_ENTER]      = "KEYPAD ENTER",
  [aKeys.KP_EQUAL]      = "KEYPAD EQUAL",
  [aKeys.KP_MULTIPLY]   = "KEYPAD MULTIPLY",
  [aKeys.KP_SUBTRACT]   = "KEYPAD SUBTRACT",
  [aKeys.K]             = "K",         [aKeys.LEFT]          = "LEFT",
  [aKeys.LEFT_ALT]      = "LEFT ALT",  [aKeys.LEFT_BRACKET]  = "LEFT BRACKET",
  [aKeys.LEFT_CONTROL]  = "LEFT CONTROL",
  [aKeys.LEFT_SHIFT]    = "LEFT SHIFT",
  [aKeys.LEFT_SUPER]    = "LEFT SUPER",
  [aKeys.L]             = "L",         [aKeys.MENU]          = "MENU",
  [aKeys.MINUS]         = "MINUS",     [aKeys.M]             = "M",
  [aKeys.N0]            = "0",         [aKeys.N1]            = "1",
  [aKeys.N2]            = "2",         [aKeys.N3]            = "3",
  [aKeys.N4]            = "4",         [aKeys.N5]            = "5",
  [aKeys.N6]            = "6",         [aKeys.N7]            = "7",
  [aKeys.N8]            = "8",         [aKeys.N9]            = "9",
  [aKeys.NUM_LOCK]      = "NUM LOCK",  [aKeys.N]             = "N",
  [aKeys.O]             = "O",         [aKeys.PAGE_DOWN]     = "PAGE DOWN",
  [aKeys.PAGE_UP]       = "PAGE UP",   [aKeys.PAUSE]         = "PAUSE",
  [aKeys.PERIOD]        = "PERIOD",    [aKeys.PRINT_SCREEN]  = "PRINT SCREEN",
  [aKeys.P]             = "P",         [aKeys.Q]             = "Q",
  [aKeys.RIGHT]         = "RIGHT",     [aKeys.RIGHT_ALT]     = "RIGHT ALT",
  [aKeys.RIGHT_BRACKET] = "RIGHT BRACKET",
  [aKeys.RIGHT_CONTROL] = "RIGHT CONTROL",
  [aKeys.RIGHT_SHIFT]   = "RIGHT SHIFT",
  [aKeys.RIGHT_SUPER]   = "RIGHT SUPER",
  [aKeys.R]             = "R",         [aKeys.SCROLL_LOCK]   = "SCROLL LOCK",
  [aKeys.SEMICOLON]     = "SEMICOLON", [aKeys.SLASH]         = "SLASH",
  [aKeys.SPACE]         = "SPACE",     [aKeys.S]             = "S",
  [aKeys.TAB]           = "TAB",       [aKeys.T]             = "T",
  [aKeys.UNKNOWN]       = "UNKNOWN",   [aKeys.UP]            = "UP",
  [aKeys.U]             = "U",         [aKeys.V]             = "V",
  [aKeys.WORLD_1]       = "WORLD 1",   [aKeys.WORLD_2]       = "WORLD 2",
  [aKeys.W]             = "W",         [aKeys.X]             = "X",
  [aKeys.Y]             = "Y",         [aKeys.Z]             = "Z"
};
-- Imports and exports ----------------------------------------------------- --
return { F = Util.Blank, A = {         -- Sending API to main loader
  -- Exports --------------------------------------------------------------- --
  aAIChoicesData = aAIChoicesData, aAITypesData = AI,
  aCreditsData = aCreditsData, aCreditsXData = aCreditsXData,
  aCursorData = aCursorData, aCursorIdData = CID,
  aDigBlockData = aDigBlockData, aDigData = aDigData,
  aDigTileData = aDigTileData, aDigTileFlags = DF,
  aDugRandShaftData = aDugRandShaftData, aEndingData = aEndingData,
  aExplodeAboveData = aExplodeAboveData, aExplodeDirData = aExplodeDirData,
  aFloodGateData = aFloodGateData, aIntroSubTitles = aIntroSubTitles,
  aKeyToLiteral = aKeyToLiteral, aJumpFallData = aJumpFallData,
  aJumpRiseData = aJumpRiseData, aLevelTypesData = aLevelTypesData,
  aLevelsData = aLevelsData, aMenuData = aMenuData, aMenuFlags = MFL,
  aMenuIds = MNU, aObjToUIData = aObjToUIData, aObjectActions = ACT,
  aObjectData = aObjectData, aObjectDirections = DIR, aObjectFlags = OFL,
  aObjectJobs = JOB, aObjectTypes = TYP, aRaceStatData = aRaceStatData,
  aRacesData = aRacesData, aSetupButtonData = aSetupButtonData,
  aSetupOptionData = aSetupOptionData, aSfxData = aSfxData,
  aShopData = aShopData, aShroudCircle = aShroudCircle,
  aShroudTileLookup = aShroudTileLookup, aTileData = aTileData,
  aTileFlags = TF, aTimerData = aTimerData, aTrainTrackData = aTrainTrackData,
  aZoneData = aZoneData
  -- ----------------------------------------------------------------------- --
} };                                   -- End of definitions to send to loader
-- End-of-File ============================================================= --
