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
 MENU      = MNU.GATE,                 LONGNAME  = "FLOOD GATE",
 NAME      = "GATE",                   STAMINA   = -1,
 STRENGTH  = 0,                        TELEDELAY = 200,
 VALUE     = 0,                        WEIGHT    = 0
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
local aTileData<const> = {             -- 0TITXTY NOTE (total 512 tiles)
  TF.NONE,                             -- 0000000 Air (Passive)
  TF.NONE,                             -- 0010100 Grass (Passive)
  TF.NONE,                             -- 0020200 Solid undiggable stone
  TF.F|TF.D,                           -- 0030300 Solid diggable dirt
  TF.F|TF.D|TF.ERB,                    -- 0040400 75%dirt+25%dug 22deg UR>DL
  TF.F|TF.D|TF.ELRB,                   -- 0050500 25%dirt+75%dug 22deg UR>DL
  TF.F|TF.D|TF.ELTR,                   -- 0060600 75%dug+25%dirt 22deg UR>DL
  TF.AD|TF.EA,                         -- 0070700 Clear dug tile
  TF.D|TF.ELT,                         -- 0080800 25%dug+75%dirt 22deg UR>DL
  TF.F|TF.D|TF.ELRB,                   -- 0090900 25%dirt+75%dug 22deg UL>DR
  TF.F|TF.D|TF.ELB,                    -- 0101000 75%dirt+25%dug 22deg UL>DR
  TF.D|TF.ETR,                         -- 0111100 25%dug+75%dirt 22deg UL>DR
  TF.D|TF.ELTR,                        -- 0121200 75%dug+25%dirt 22deg UL>DR
  TF.F|TF.D|TF.ERB,                    -- 0131300 50%dirt+50%dug 45deg UR>DL
  TF.D|TF.ELT,                         -- 0141400 50%dug+50%dirt 45deg UR>DL
  TF.D|TF.ETR,                         -- 0151500 50%dug+50%dirt 45deg UL>DR
  TF.F|TF.D|TF.ELB,                    -- 0161600 50%dirt+50%dug 45deg UL>DR
  TF.F|TF.D|TF.ER,                     -- 0171700
  TF.F|TF.D|TF.ERB,                    -- 0181800
  TF.D|TF.ET,                          -- 0191900
  TF.F|TF.D|TF.ERB,                    -- 0202000
  TF.D|TF.ETR,                         -- 0212100
  TF.D|TF.ELT,                         -- 0222200
  TF.F|TF.D|TF.ERB,                    -- 0232300
  TF.D|TF.ETR,                         -- 0242400
  TF.D|TF.ETR,                         -- 0252500
  TF.F|TF.D|TF.EL,                     -- 0262600
  TF.F|TF.D|TF.EB,                     -- 0272700
  TF.D|TF.ELT,                         -- 0282800
  TF.F|TF.D|TF.EB,                     -- 0292900
  TF.D|TF.ELTR,                        -- 0303000
  TF.F|TF.D|TF.EL,                     -- 0313100
  TF.F|TF.D|TF.ELB,                    -- 0320001
  TF.D|TF.ELT,                         -- 0330101
  TF.F|TF.D|TF.ELB,                    -- 0340201
  TF.F|TF.D|TF.ELB,                    -- 0350301
  TF.F|TF.D|TF.ELB,                    -- 0360401
  TF.D|TF.ET,                          -- 0370501
  TF.F|TF.D|TF.ELB,                    -- 0380601
  TF.D|TF.ETR,                         -- 0390701
  TF.D|TF.ELT,                         -- 0400801
  TF.D|TF.ELB,                         -- 0410901
  TF.F|TF.D|TF.ELT,                    -- 0421001
  TF.D|TF.ELT,                         -- 0431101
  TF.D|TF.ER,                          -- 0441201
  TF.F|TF.D|TF.EB,                     -- 0451301
  TF.F|TF.D|TF.ERB,                    -- 0461401
  TF.F|TF.D|TF.ELRB,                   -- 0471501
  TF.F|TF.D|TF.ELRB,                   -- 0481601
  TF.F|TF.D|TF.ETRB,                   -- 0491701
  TF.D|TF.ETRB,                        -- 0501801
  TF.D|TF.ETR,                         -- 0511901
  TF.D|TF.ELTR,                        -- 0522001 Cave floor 1
  TF.D|TF.ELTR,                        -- 0532101 Cave floor 2
  TF.D|TF.ELB,                         -- 0542201
  TF.F|TF.D|TF.ELTB,                   -- 0552301
  TF.F|TF.D|TF.ELTB,                   -- 0562401
  TF.D|TF.ELT,                         -- 0572501
  TF.D|TF.EA,                          -- 0582601
  TF.D|TF.EA,                          -- 0592701
  TF.D|TF.EA,                          -- 0602801
  TF.D|TF.EA,                          -- 0612901
  TF.ELRB,                             -- 0623001
  TF.F,                                -- 0633101
  TF.F,                                -- 0640002
  TF.NONE,                             -- 0650102
  TF.NONE,                             -- 0660202
  TF.NONE,                             -- 0670302
  TF.NONE,                             -- 0680402
  TF.NONE,                             -- 0690502
  TF.NONE,                             -- 0700602
  TF.NONE,                             -- 0710702
  TF.NONE,                             -- 0720802
  TF.NONE,                             -- 0730902
  TF.NONE,                             -- 0741002
  TF.NONE,                             -- 0751102
  TF.P|TF.EA,                          -- 0761202
  TF.P|TF.EA,                          -- 0771302
  TF.P|TF.EA,                          -- 0781402
  TF.P|TF.EA,                          -- 0791502
  TF.P|TF.EA,                          -- 0801602
  TF.P|TF.EA,                          -- 0811702
  TF.P|TF.EA,                          -- 0821802
  TF.P|TF.EA,                          -- 0831902
  TF.P|TF.EA,                          -- 0842002
  TF.NONE,                             -- 0852102 Unused
  TF.NONE,                             -- 0862202 Unused
  TF.NONE,                             -- 0872302 Platform cliff edge right
  TF.D|TF.P|TF.EA,                     -- 0882402 Left end of track
  TF.NONE,                             -- 0892502 Floor to wall right
  TF.NONE,                             -- 0902602 Platform cliff edge left
  TF.D|TF.P|TF.EA,                     -- 0912702 Right end of track
  TF.NONE,                             -- 0922802 Right cliff wall
  TF.NONE,                             -- 0932902 Left cliff wall
  TF.NONE,                             -- 0943002 Floor to wall left
  TF.EA,                               -- 0953102 Clear dug tile with window
  TF.EA,                               -- 0960003 Clear dug tile with skull
  TF.W|TF.AB,                          -- 0970103 Ocean surface 1/4
  TF.W|TF.AB,                          -- 0980203 Ocean surface 2/4
  TF.W|TF.AB,                          -- 0990303 Ocean surface 3/4
  TF.W|TF.AE,                          -- 1000403 Ocean surface 4/4
  TF.W|TF.AB,                          -- 1010503 Ocean bubble large 1/4
  TF.W|TF.AB,                          -- 1020603 Ocean bubble large 2/4
  TF.W|TF.AB,                          -- 1030703 Ocean bubble large 3/4
  TF.W|TF.AE,                          -- 1040803 Ocean bubble large 4/4
  TF.W|TF.AB,                          -- 1050903 Ocean surface bubble L 1/4
  TF.W|TF.AB,                          -- 1061003 Ocean surface bubble L 2/4
  TF.W|TF.AB,                          -- 1071103 Ocean surface bubble L 3/4
  TF.W|TF.AE,                          -- 1081203 Ocean surface bubble L 4/4
  TF.W|TF.AB,                          -- 1091303 Ocean bubble small 1/4
  TF.W|TF.AB,                          -- 1101403 Ocean bubble small 2/4
  TF.W|TF.AB,                          -- 1111503 Ocean bubble small 3/4
  TF.W|TF.AE,                          -- 1121603 Ocean bubble small 4/4
  TF.W,                                -- 1131703 Ocean surface bubble S 1/4
  TF.W,                                -- 1141803 Ocean surface bubble S 2/4
  TF.W,                                -- 1151903 Ocean surface bubble S 3/4
  TF.W,                                -- 1162003 Ocean surface bubble S 4/4
  TF.W,                                -- 1172103 Ocean
  TF.W|TF.AB,                          -- 1182203 Ocean surface weed 1/4
  TF.W|TF.AB,                          -- 1192303 Ocean surface weed 2/4
  TF.W|TF.AB,                          -- 1202403 Ocean surface weed 3/4
  TF.W|TF.AE,                          -- 1212503 Ocean surface weed 4/4
  TF.NONE,                             -- 1222603
  TF.NONE,                             -- 1232703
  TF.NONE,                             -- 1242803
  TF.NONE,                             -- 1252903 Cliff edge right (Jng/Snow)
  TF.NONE,                             -- 1263003 Cliff edge left (Jng/Snow)
  TF.NONE,                             -- 1273103
  TF.NONE,                             -- 1280004
  TF.NONE,                             -- 1290104
  TF.NONE,                             -- 1300204
  TF.NONE,                             -- 1310304
  TF.NONE,                             -- 1320404
  TF.NONE,                             -- 1330504
  TF.P|TF.EA,                          -- 1340604
  TF.P|TF.EA,                          -- 1350704
  TF.P|TF.EA,                          -- 1360804
  TF.P|TF.EA,                          -- 1370904
  TF.W,                                -- 1381004
  TF.W,                                -- 1391104
  TF.W,                                -- 1401204
  TF.W,                                -- 1411304 Cliff edge left (Jun/Snow)
  TF.NONE,                             -- 1421404
  TF.NONE,                             -- 1431504
  TF.W,                                -- 1441604 Cliff edge right (Jun/Snow)
  TF.NONE,                             -- 1451704
  TF.NONE,                             -- 1461804
  TF.W,                                -- 1471904
  TF.NONE,                             -- 1482004
  TF.D|TF.AD|TF.P|TF.T|TF.EA,          -- 1492104 Dug tile track with light
  TF.D|TF.AD|TF.EA,                    -- 1502204 Dug tile with light
  TF.F,                                -- 1512304 Quad rock bottom left
  TF.F,                                -- 1522404 Quad rock bottom right
  TF.F,                                -- 1532504 Quad rock top left
  TF.F,                                -- 1542604 Quad rock top right
  TF.F,                                -- 1552704 Impenetrable rock
  TF.F,                                -- 1562804 Impenetrable rock
  TF.F,                                -- 1572904 Horizontal rock left
  TF.F,                                -- 1583004 Horizontal rock right
  TF.F,                                -- 1593104 Impenetrable rock
  TF.F,                                -- 1600005 Unused
  TF.NONE,                             -- 1610105 Unused
  TF.NONE,                             -- 1620205
  TF.NONE,                             -- 1630305 Outside decoration only
  TF.NONE,                             -- 1640405 Outside decoration only
  TF.NONE,                             -- 1650505 Outside decoration only
  TF.NONE,                             -- 1660605 Outside decoration only
  TF.NONE,                             -- 1670705 Outside decoration only
  TF.NONE,                             -- 1680805 Outside decoration only
  TF.D|TF.AD|TF.P|TF.T|TF.EA,          -- 1690905 Dug tile beam forward track
  TF.D|TF.AD|TF.EA,                    -- 1701005 Dug tile beam forward
  TF.D|TF.AD|TF.EA,                    -- 1711105 Dug tile beam backwards
  TF.D|TF.AD|TF.EA,                    -- 1721205 Dug tile beam horizontal
  TF.D|TF.ELTR,                        -- 1731305 Dug tile down 1/3
  TF.D|TF.ELTR,                        -- 1741405 Dug tile down 2/3
  TF.D|TF.ELTR,                        -- 1751505 Dug tile down 3/3
  TF.NONE,                             -- 1761605 Outside decoration only
  TF.NONE,                             -- 1771705 Outside decoration only
  TF.NONE,                             -- 1781805 Outside decoration only
  TF.NONE,                             -- 179195 Outside decoration only
  TF.NONE,                             -- 1802005 Outside decoration only
  TF.NONE,                             -- 1812105 Outside decoration only
  TF.NONE,                             -- 1822205 Outside decoration only
  TF.NONE,                             -- 1832305 Outside decoration only
  TF.NONE,                             -- 1842405 Outside decoration only
  TF.NONE,                             -- 1852505 Outside decoration only
  TF.NONE,                             -- 1862605 Outside decoration only
  TF.NONE,                             -- 1872705 Outside decoration only
  TF.NONE,                             -- 1882805 Outside decoration only
  TF.EA|TF.E,                          -- 1892905 Elevator shaft wire
  TF.NONE,                             -- 1903005 Elevator base
  TF.NONE,                             -- 1913105 Trade centre top left
  TF.NONE,                             -- 1920006 Trade centre top right
  TF.NONE,                             -- 1930106 Trade centre bottom left
  TF.NONE,                             -- 1940206 Trade centre bottom right
  TF.AB|TF.PO,                         -- 1950306 Player 1 home flag (1/4)
  TF.AB|TF.PO,                         -- 1960406 Player 1 home flag (2/4)
  TF.AB|TF.PO,                         -- 1970506 Player 1 home flag (3/4)
  TF.AE|TF.PO,                         -- 1980606 Player 1 home flag (4/4)
  TF.AB|TF.PT,                         -- 1990706 Player 2 home flag (1/4)
  TF.AB|TF.PT,                         -- 2000806 Player 2 home flag (2/4)
  TF.AB|TF.PT,                         -- 2010906 Player 2 home flag (3/4)
  TF.AE|TF.PT,                         -- 2021006 Player 2 home flag (4/4)
  TF.D|TF.ELR,                         -- 2031106
  TF.F|TF.D|TF.EB,                     -- 2041206
  TF.F|TF.D|TF.ER,                     -- 2051306
  TF.D|TF.ELTR,                        -- 2061406
  TF.F|TF.D|TF.ERB,                    -- 2071506
  TF.D|TF.ETR,                         -- 2081606
  TF.F|TF.D|TF.ERB,                    -- 2091706
  TF.D|TF.AD|TF.P|TF.T|TF.EA,          -- 2101806 Clear tile with track
  TF.F|TF.D|TF.ETR,                    -- 2111906
  TF.F|TF.D|TF.ETRB,                   -- 2122006
  TF.D|TF.ETR,                         -- 2132106
  TF.D|TF.ETR,                         -- 2142206
  TF.D|TF.ELTB,                        -- 2152306
  TF.F|TF.D|TF.ELB,                    -- 2162406
  TF.D|TF.ELTB,                        -- 2172506
  TF.D|TF.ELRB,                        -- 2182606
  TF.D|TF.ELT,                         -- 2192706
  TF.D|TF.ELTB,                        -- 2202806
  TF.D|TF.ELT,                         -- 2212906
  TF.D|TF.ELT,                         -- 2223006
  TF.F|TF.D|TF.ETRB,                   -- 2233106
  TF.W|TF.D|TF.ERB,                    -- 2240007
  TF.F|TF.D|TF.ETRB,                   -- 2250107
  TF.D|TF.ELRB,                        -- 2260207
  TF.D|TF.ELTB,                        -- 2270307
  TF.D|TF.ELTB,                        -- 2280407
  TF.D|TF.ELTB,                        -- 2290507
  TF.D|TF.ETRB,                        -- 2300607
  TF.D|TF.ETRB,                        -- 2310707
  TF.D|TF.ETRB,                        -- 2320807
  TF.NONE,                             -- 2330907
  TF.NONE,                             -- 2341007
  TF.D|TF.EA,                          -- 2351107 90%dug+10%dirt 11deg UR>DL
  TF.D|TF.EA,                          -- 2361207 90%dug+10%dirt 11deg UL>DR
  TF.NONE,                             -- 2371307 Unused
  TF.NONE,                             -- 2381407 Unused
  TF.NONE,                             -- 2391507 Unused
  TF.F,                                -- 2401607 Double horizontal rock 1/2
  TF.F,                                -- 2411707 Double horizontal rock 2/2
  TF.F,                                -- 2421807 Double vertical rock 1/2
  TF.F,                                -- 2431907 Double vertical rock 2/2
  TF.F|TF.D|TF.W|TF.ERB,               -- 2442007
  TF.F|TF.D|TF.W|TF.ELRB,              -- 2452107
  TF.D|TF.W|TF.ELTR,                   -- 2462207
  TF.D|TF.W|TF.EA|TF.AD,               -- 2472307 Clear water tile
  TF.D|TF.W|TF.ELT,                    -- 2482407
  TF.F|TF.D|TF.W|TF.ELRB,              -- 2492507
  TF.F|TF.D|TF.W|TF.ELB,               -- 2502607
  TF.D|TF.W|TF.ETR,                    -- 2512707
  TF.D|TF.W|TF.ELTR,                   -- 2522807
  TF.F|TF.D|TF.W|TF.EA,                -- 2532907
  TF.D|TF.W|TF.EA,                     -- 2543007
  TF.D|TF.W|TF.EA,                     -- 2553107
  TF.F|TF.D|TF.W|TF.EA,                -- 2560008
  TF.F|TF.D|TF.W|TF.ERB,               -- 2570108
  TF.F|TF.D|TF.W|TF.ERB,               -- 2580208
  TF.D|TF.W|TF.ET,                     -- 2590308
  TF.F|TF.D|TF.W|TF.ERB,               -- 2600408
  TF.D|TF.W|TF.ETR,                    -- 2610508
  TF.D|TF.W|TF.ELT,                    -- 2620608
  TF.F|TF.D|TF.W|TF.ERB,               -- 2630708
  TF.D|TF.W|TF.ETR,                    -- 2640808
  TF.D|TF.W|TF.ETR,                    -- 2650908
  TF.F|TF.D|TF.W|TF.EL,                -- 2661008
  TF.F|TF.D|TF.W|TF.EB,                -- 2671108
  TF.D|TF.W|TF.ELT,                    -- 2681208
  TF.F|TF.D|TF.W|TF.EB,                -- 2691308
  TF.D|TF.W|TF.ELTR,                   -- 2701408
  TF.F|TF.D|TF.W|TF.EL,                -- 2711508
  TF.F|TF.D|TF.W|TF.ELB,               -- 2721608
  TF.D|TF.W|TF.ELT,                    -- 2731708
  TF.F|TF.D|TF.W|TF.ELB,               -- 2741808
  TF.F|TF.D|TF.W|TF.ELB,               -- 2751908
  TF.F|TF.D|TF.W|TF.ELB,               -- 2762008
  TF.D|TF.W|TF.EB,                     -- 2772108
  TF.F|TF.D|TF.W|TF.ELB,               -- 2782208
  TF.D|TF.W|TF.ERB,                    -- 2792308
  TF.D|TF.W|TF.ELT,                    -- 2802408
  TF.D|TF.W|TF.ELB,                    -- 2812508
  TF.F|TF.D|TF.W|TF.ELT,               -- 2822608
  TF.D|TF.W|TF.ELT,                    -- 2832708
  TF.F|TF.D|TF.W|TF.ER,                -- 2842808
  TF.F|TF.D|TF.W|TF.EB,                -- 2852908
  TF.F|TF.D|TF.W|TF.ERB,               -- 2863008 Flood cave ceiling left
  TF.F|TF.D|TF.W|TF.ELRB,              -- 2873108 Flood cave ceiling 1
  TF.F|TF.D|TF.W|TF.ELRB,              -- 2880009 Flood cave ceiling 2
  TF.D|TF.W|TF.ETRB,                   -- 2890109 Flood cave left 1
  TF.D|TF.W|TF.ETRB,                   -- 2900209 Flood cave left 2
  TF.D|TF.W|TF.ETR,                    -- 2910309 Flood cave floor left
  TF.D|TF.W|TF.ELTR,                   -- 2920409 Flood cave floor 1
  TF.D|TF.W|TF.ELTR,                   -- 2930509 Flood cave floor 2
  TF.F|TF.D|TF.W|TF.ELB,               -- 2940609 Flood cave ceiling right
  TF.D|TF.W|TF.ELTB,                   -- 2950709 Flood cave right 1
  TF.D|TF.W|TF.ELTB,                   -- 2960809 Flood cave right 2
  TF.D|TF.W|TF.ELT,                    -- 2970909 Flood cave floor right
  TF.D|TF.W|TF.EA,                     -- 2981009 Flood cave floor bot-left
  TF.D|TF.W|TF.EA,                     -- 2991109 Flood cave ceil top-left
  TF.D|TF.W|TF.EA,                     -- 3001209 Flood cave ceil top-right
  TF.D|TF.W|TF.EA,                     -- 3011309 Flood cave floor bot-right
  TF.D|TF.W|TF.ELRB,                   -- 3021409 Flood elevator ceiling
  TF.F,                                -- 3031509 Outside top-right ceiling 1
  TF.F,                                -- 3041609 Outside top-right ceiling 2
  TF.F,                                -- 3051709 Outside ceiling
  TF.F,                                -- 3061809 Outside top-left ceiling 1
  TF.F,                                -- 3071909 Outside top-left ceiling 2
  TF.F,                                -- 3082009 Corpse column 1 row 1
  TF.F,                                -- 3092109 Corpse column 2 row 1
  TF.F,                                -- 3102209 Corpse column 3 row 1
  TF.F,                                -- 3112309 Corpse column 4 row 1
  TF.F,                                -- 3122409 Corpse column 1 row 2
  TF.F,                                -- 3132509 Corpse column 2 row 2
  TF.F,                                -- 3142609 Corpse column 3 row 2
  TF.F,                                -- 3152709 Corpse column 4 row 2
  TF.D|TF.W|TF.P|TF.EA,                -- 3162809 Top of gold pillar
  TF.D|TF.W|TF.P|TF.EA,                -- 3172909 Vertical gold pillar
  TF.D|TF.W|TF.P|TF.EA,                -- 3183009 Bottom of gold pillar
  TF.D|TF.W|TF.P|TF.EA,                -- 3193109 Broken top pillar
  TF.D|TF.W|TF.P|TF.EA,                -- 3200010 Gold house bottom
  TF.D|TF.W|TF.P|TF.EA,                -- 3210110 Gold house top
  TF.D|TF.W|TF.P|TF.EA,                -- 3220210 Broken half top pillar
  TF.D|TF.W|TF.P|TF.EA,                -- 3230310 Broken horizontal top
  TF.D|TF.W|TF.P|TF.EA,                -- 3240410 Broken horizontal bottom
  TF.D|TF.W,                           -- 3250510 Unused
  TF.NONE,                             -- 3260610 Oil rig col 1 row 2
  TF.NONE,                             -- 3270710 Oil rig col 2 row 2
  TF.D|TF.W|TF.P|TF.EA,                -- 3280810 Water dug tile track end L
  TF.NONE,                             -- 3290910 Oil rig col 1 row 1 anim 1
  TF.NONE,                             -- 3301010 Oil rig col 2 row 1 anim 1
  TF.D|TF.W|TF.P|TF.EA,                -- 3311110 Water dug tile track end R
  TF.NONE,                             -- 3321210 Oil rig col 1 row 1 anim 2
  TF.NONE,                             -- 3331310 Oil rig col 2 row 1 anim 2
  TF.NONE,                             -- 3341410 Unused
  TF.D|TF.W|TF.EA,                     -- 3351510 Water dug tile with window
  TF.D|TF.W|TF.EA,                     -- 3361610 Water dug tile with skull
  TF.NONE,                             -- 3371710 Unused
  TF.NONE,                             -- 3381810 Unused
  TF.NONE,                             -- 3391910 Unused
  TF.NONE,                             -- 3402010 Unused
  TF.NONE,                             -- 3412110 Sun top-left
  TF.NONE,                             -- 3422210 Sun top-right
  TF.NONE,                             -- 3432310 Moon top-left
  TF.NONE,                             -- 3442410 Moon top-right
  TF.NONE,                             -- 3452510 Moon bottom-right
  TF.NONE,                             -- 3462610
  TF.NONE,                             -- 3472710
  TF.NONE,                             -- 3482810
  TF.NONE,                             -- 3492910
  TF.NONE,                             -- 3503010
  TF.NONE,                             -- 3513110
  TF.NONE,                             -- 3520011 Cave entrance right
  TF.NONE,                             -- 3530111 Cave entrance left
  TF.NONE,                             -- 3540211 Cave entrance down
  TF.NONE,                             -- 3550311 Unused
  TF.NONE,                             -- 3560411 Unused
  TF.NONE,                             -- 3570511 Unused
  TF.NONE,                             -- 3580611 Unused
  TF.NONE,                             -- 3590711 Unused
  TF.NONE,                             -- 3600811 Outside decoration only
  TF.NONE,                             -- 3610911 Outside decoration only
  TF.NONE,                             -- 3621011 Outside decoration only
  TF.NONE,                             -- 3631111 Outside decoration only
  TF.NONE,                             -- 3641211 Outside decoration only
  TF.NONE,                             -- 3651311 Outside decoration only
  TF.NONE,                             -- 3661411 Outside decoration only
  TF.NONE,                             -- 3671511 Outside decoration only
  TF.NONE,                             -- 3681611 Outside decoration only
  TF.NONE,                             -- 3691711 Outside decoration only
  TF.NONE,                             -- 3701811 Outside decoration only
  TF.NONE,                             -- 3711911 Outside decoration only
  TF.NONE,                             -- 3722011 Outside decoration only
  TF.NONE,                             -- 3732111 Outside decoration only
  TF.D|TF.W|TF.P|TF.EA,                -- 3742211 Water monument top-left
  TF.D|TF.W|TF.P|TF.EA,                -- 3752311 Water monument top right
  TF.D|TF.W|TF.P|TF.EA,                -- 3762411 Water monument bot-left
  TF.D|TF.W|TF.P|TF.EA,                -- 3772511 Water monument bot-right
  TF.NONE,                             -- 3782611 Outside decoration only
  TF.NONE,                             -- 3792711 Outside decoration only
  TF.NONE,                             -- 3802811 Outside decoration only
  TF.NONE,                             -- 3812911 Outside decoration only
  TF.NONE,                             -- 3823011 Outside decoration only
  TF.NONE,                             -- 3833111 Outside decoration only
  TF.NONE,                             -- 3840012 Outside decoration only
  TF.NONE,                             -- 3850112 Outside decoration only
  TF.NONE,                             -- 3860212 Outside decoration only
  TF.NONE,                             -- 3870312 Outside decoration only
  TF.NONE,                             -- 3880412 Outside decoration only
  TF.D|TF.W|TF.AD|TF.T|TF.P|TF.EA,     -- 3890512 Water clear light track
  TF.D|TF.W|TF.AD|TF.EA,               -- 3900612 Water clear light
  TF.NONE,                             -- 3910712 Outside decoration only
  TF.NONE,                             -- 3920812 Outside decoration only
  TF.NONE,                             -- 3930912 Outside decoration only
  TF.NONE,                             -- 3941012 Outside decoration only
  TF.NONE,                             -- 3951112 Outside decoration only
  TF.NONE,                             -- 3961212 Outside decoration only
  TF.NONE,                             -- 3971312 Outside decoration only
  TF.NONE,                             -- 3981412 Outside decoration only
  TF.NONE,                             -- 3991512 Outside decoration only
  TF.NONE,                             -- 4001612 Outside decoration only
  TF.NONE,                             -- 4011712 Outside decoration only
  TF.NONE,                             -- 4021812 Outside decoration only
  TF.NONE,                             -- 4031912 Outside decoration only
  TF.NONE,                             -- 4042012 Outside decoration only
  TF.NONE,                             -- 4052112 Outside decoration only
  TF.F,                                -- 4062212 Outside ceiling left
  TF.F,                                -- 4072312 Outside ceiling straight
  TF.F,                                -- 4082412 Outside ceiling right
  TF.D|TF.W|TF.T|TF.P|TF.EA|TF.AD,     -- 4092512 Water beam forward track
  TF.D|TF.W|TF.EA|TF.AD,               -- 4102612 Water beam forward
  TF.D|TF.W|TF.EA|TF.AD,               -- 4112712 Water beam backward
  TF.D|TF.W|TF.EA|TF.AD,               -- 4122812 Water beam horizontal
  TF.D|TF.W|TF.ELTR,                   -- 4132912 Water dig down 1/3
  TF.D|TF.W|TF.ELTR,                   -- 4143012 Water dig down 2/3
  TF.D|TF.W|TF.ELTR,                   -- 4153112 Water dig down 3/3
  TF.NONE,                             -- 4160013 Top-right cliff ceiling
  TF.NONE,                             -- 4170113 Bottom-left cliff floor
  TF.NONE,                             -- 4180213 Top-left cliff ceiling
  TF.NONE,                             -- 4190313 Top-right cliff ceiling
  TF.NONE,                             -- 4200413 Bottom-right cliff floor
  TF.NONE,                             -- 4210513 Unused
  TF.NONE,                             -- 4220613 Unused
  TF.NONE,                             -- 4230713 Unused
  TF.NONE,                             -- 4240813 Unused
  TF.F,                                -- 4250913 Outside ceil L/R straight
  TF.F,                                -- 4261013 Outside ceil R/D straight
  TF.F,                                -- 4271113 Outside ceil U/R straight
  TF.NONE,                             -- 4281213 Vertical dug leads outside
  TF.D|TF.W|TF.E|TF.EA,                -- 4291313 Elevator wire (water)
  TF.NONE,                             -- 4301413 Elevator foundation
  TF.NONE,                             -- 4311513 Out half ceil D/R corner
  TF.F,                                -- 4321613 Out half ceil L/R straight
  TF.NONE,                             -- 4331713 Out half ceil R/U corner
  TF.D|TF.P|TF.EL|TF.ER|TF.G,          -- 4341813 Flood Gate (No water)
  TF.D|TF.P|TF.EL|TF.ER|TF.G,          -- 4351913 Flood Gate (Water left)
  TF.D|TF.P|TF.EL|TF.ER|TF.G,          -- 4362013 Flood Gate (Water right)
  TF.D|TF.W|TF.D|TF.P|TF.EL|TF.ER|TF.G,-- 4372113 Flood Gate (All water)
  TF.D|TF.P|TF.EL|TF.ER|TF.G,          -- 4382213 Flood Gate open (No water)
  TF.D|TF.W|TF.D|TF.P|TF.EL|TF.ER|TF.G,-- 4392313 Flood Gate open (Water)
  TF.NONE,                             -- 4402413 Desert crystal 1
  TF.NONE,                             -- 4412513 Desert crystal 2
  TF.NONE,                             -- 4422613 Unused
  TF.D|TF.W|TF.ETR,                    -- 4432713 Diagonal up-left flooded
  TF.F|TF.D|TF.W|TF.EB,                -- 4442813 Diagonal up-left flooded
  TF.F|TF.D|TF.W|TF.ER,                -- 4452913 Diagonal up-left flooded
  TF.D|TF.W|TF.ELTR,                   -- 4463013 Diagonal up-left flooded
  TF.F|TF.D|TF.W|TF.ERB,               -- 4473113 Diagonal up-left flooded
  TF.D|TF.W|TF.ETR,                    -- 4480014 Diagonal up-left flooded
  TF.F|TF.D|TF.W|TF.ERB,               -- 4490114 Diagonal up-left flooded
  TF.D|TF.W|TF.AD|TF.T|TF.P|TF.EA,     -- 4500214 Watered clear track tile
  TF.D|TF.W|TF.ETR,                    -- 4510314 Diagonal up-right flooded
  TF.D|TF.W|TF.ETRB,                   -- 4520414 Diagonal up-right flooded
  TF.D|TF.W|TF.ETR,                    -- 4530514 Diagonal up-right flooded
  TF.D|TF.W|TF.ETR,                    -- 4540614 Diagonal up-right flooded
  TF.D|TF.W|TF.ELTB,                   -- 4550714 Diagonal up-right flooded
  TF.F|TF.D|TF.W|TF.ELB,               -- 4560814 Diagonal up-right flooded
  TF.D|TF.W|TF.ELT,                    -- 4570914 Diagonal up-right flooded
  TF.F|TF.D|TF.W|TF.ELRB,              -- 4581014 Diagonal up-right flooded
  TF.D|TF.W|TF.ELT,                    -- 4591114
  TF.D|TF.W|TF.ELTB,                   -- 4601214
  TF.D|TF.W|TF.ELB,                    -- 4611314
  TF.D|TF.W|TF.ELB,                    -- 4621414
  TF.D|TF.W|TF.ETRB,                   -- 4631514
  TF.D|TF.W|TF.F|TF.ERB,               -- 4641614
  TF.D|TF.W|TF.ETRB,                   -- 4651714
  TF.D|TF.W|TF.F|TF.ELRB,              -- 4661814
  TF.D|TF.W|TF.ELTB,                   -- 4671914
  TF.D|TF.W|TF.ELTB,                   -- 4682014
  TF.D|TF.W|TF.ELTB,                   -- 4692114
  TF.D|TF.W|TF.ETRB,                   -- 4702214
  TF.D|TF.W|TF.ETRB,                   -- 4712314
  TF.D|TF.W|TF.ETRB,                   -- 4722414
  TF.D|TF.W,                           -- 4732514
  TF.NONE,                             -- 4742614
  TF.D|TF.W|TF.P|TF.EA,                -- 4752714
  TF.D|TF.W|TF.P|TF.EA,                -- 4762814
  TF.NONE,                             -- 4772914 Unused
  TF.NONE,                             -- 4783014 Unused
  TF.NONE,                             -- 4793114 Unused
  TF.NONE,                             -- 4800015 Unused
  TF.NONE,                             -- 4810115 Unused
  TF.NONE,                             -- 4820215 Unused
  TF.NONE,                             -- 4830315 Unused
  TF.NONE,                             -- 4840415 Unused
  TF.NONE,                             -- 4850515 Unused
  TF.NONE,                             -- 4860615 Unused
  TF.NONE,                             -- 4870715 Unused
  TF.NONE,                             -- 4880815 Unused
  TF.NONE,                             -- 4890915 Unused
  TF.NONE,                             -- 4901015 Unused
  TF.NONE,                             -- 4911115 Unused
  TF.NONE,                             -- 4921215 Unused
  TF.NONE,                             -- 4931315 Unused
  TF.NONE,                             -- 4941415 Unused
  TF.NONE,                             -- 4951515 Unused
  TF.NONE,                             -- 4961615 Unused
  TF.NONE,                             -- 4971715 Unused
  TF.NONE,                             -- 4981815 Unused
  TF.NONE,                             -- 4991915 Unused
  TF.NONE,                             -- 5002015 Unused
  TF.NONE,                             -- 5012115 Unused
  TF.NONE,                             -- 5022215 Unused
  TF.NONE,                             -- 5032315 Unused
  TF.NONE,                             -- 5042415 Unused
  TF.NONE,                             -- 5052515 Unused
  TF.NONE,                             -- 5062615 Unused
  TF.NONE,                             -- 5072715 Unused
  TF.NONE,                             -- 5082815 Unused
  TF.NONE,                             -- 5092915 Unused
  TF.NONE,                             -- 5103015 Unused
  TF.NONE,                             -- 5113115 Unused
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
  [-1]           = {    "MINING OPERATIONS COMPLETED!",
                        "ZOGS HAVE BEEN RAISED!" },
  [TYP.FTARG]    = { 1, "THE F'TARGS CONSTRUCT THEIR OWN MUSEUM",
                        "OF METAL MARVELS WITH THEIR EARNINGS..." },
  [TYP.HABBISH]  = { 2, "THE HABBISH CONSTRUCT A TEMPLE IN HONOUR OF",
                        "THE LORD HIGH HABBORG WITH THEIR EARNINGS..." },
  [TYP.GRABLIN]  = { 3, "THE GRABLINS CONSTRUCT THEIR OWN GROK",
                        "BREWERY WITH THEIR EARNINGS..." },
  [TYP.QUARRIOR] = { 4, "THE QUARRIORS CONSTRUCT AN IMPENETRABLE",
                        "FORTRESS WITH THEIR EARNINGS..." },
  [-2]           = {    "...WHILST THE MYSTERIOUS FIGURE OF THE",
                        "MASTER MINER RETURNS FROM WHENCE HE CAME!" }
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
  { { nil, nil, nil, nil, 0, CID.OK, false, false, false }, 102, nil, nil,
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
-- Book data --------------------------------------------------------------- --
local aBookData<const> = {
  -- English pages --------------------------------------------------------- --
  ["en"] = {
    -- Page 1 -------------------------------------------------------------- --
    { "THE BOOK OF ZARG\n\z
      \n\z
      This book contains information about most aspects of the planet Zarg \z
      and provides vital details for anyone wishing to set up mining \z
      operations upon it. This information is listed under chapter headings. \z
      To obtain the information you require, simply click on the appropriate \z
      chapter heading and the chosen page will appear. In the lett hand \z
      margin of the book is a row of buttons that enable you to turn to any \z
      page you require.",
    -- Page 2 -------------------------------------------------------------- --
    },{ "CHAPTER HEADINGS\n\z
      \n\z
      ABOUT THIS BOOK\n\z
      HOW TO START DIGGERS (OPTIONS FOR THE GAME)\n\z
      THE PLANET ZARG and THE GLORIOUS 412th\n\z
      DIGGERS RACE DESCRIPTIONS\n\z
      ZONE DESCRIPTIONS\n\z
      FLORA AND FAUNA\n\z
      THE MINING STORE\n\z
      MINING APPARATUS\n\z
      ZARGON STOCK MARKET ZARGON MINING HISTORY"
    -- Page 3 -------------------------------------------------------------- --
    },{ "ABOUT THIS BOOK\n\z
      The Book of Zaro is simplicity itself to use. The Book uses a \z
      revolutionary substance called TNT to display it's pages. (See TNT.) \z
      Consequently, the Book only needs to contain the cover, one sheet of \z
      TNT and the control mechanism for TNT which are 3 cut emeralds that \z
      are embedded into the spine of the Book.\n\z
      \n\z
      The top emerald returns the reader to the index page.\n\z
      The 2nd emerald moves the reader to the next page.\n\z
      The third emerald takes the reader back to the previous page."
    -- Page 4 -------------------------------------------------------------- --
    },{ "Simply pressing the relevant buttons will allow the reader to skip \z
      to the page of their choice.\n\z
      \n\z
      Chapter selection is made by choosing which chapter they would like to \z
      read and simply pointing at the heading. This will take the reader to \z
      the first page of that chapter.\n\z
      \n\z
      TNT\n\z
      \n\z
      The Book of Zarg is made of a remarkable paper-like substance known as \z
      TNT (Texturised Neural Transistors). The process of creating this \z
      paper is so mind-bogglingly complicated that it is only fully"
    -- Page 5 -------------------------------------------------------------- --
    },{ "understood by the three-brained Sloargs that inhabit the Great Hall \z
      of a Thousand Rustling Intellects on the planet Cerebralis, therefore \z
      an explanation will not be attempted. Suffice it to say that however \z
      it is done, TNT has had an impressive, some may say revolutionary (but \z
      then you always get some people - normally with suspicious facial hair \z
      and a poorly recorded demo tape in their pockets - who say that sort \z
      of thing) effect on all kinds of books.\n\z
      \n\z
      TNT's effects are threefold and simple to explain.\n\z
      \n\z
      A. One sheet of TNT can contain an infinite number of words, thereby \z
      reducing the thickness of books to one"
    -- Page 6 -------------------------------------------------------------- --
    },{ "page.\n\z
      \n\z
      B. When a reader touches a sheet of TNT, it scans their brain to \z
      ascertain speech and reading patterns. a fraction of a nano-second it \z
      then presents the text in the reader's preferred language.\n\z
      \n\z
      C. If an illustrator uses a special ink to draw on a page of TNT, when \z
      that page is opened, the illustration moves.\n\z
      \n\z
      Eye-catching indeed, and the results of a random survey of readers' \z
      reactions to TNT are equally illuminating."
    -- Page 7 -------------------------------------------------------------- --
    },{ "\"New fangled nonsense. Blasted pictures won't stop moving. It'll \z
      never catch on, give me my quill and ink any day.\"\n\z
      \n\z
      \"That's radical. It's like, you know, it's revolutionary. I'm in a \z
      band by the way, do you want to hear my tape?\""
    -- Page 8 -------------------------------------------------------------- --
    },{ "HOW TO START DIGGERS\n\z
      \n\z
      THE CONTROLLERS OFFICE\n\z
      \n\z
      Every time you start a new game, complete a level, or terminate an \z
      existing game, you will be returned to the Controller's Office. The \z
      Controller will prompt you to select a level to either begin your \z
      mining operations or continue those successful operations.\n\z
      \n\z
      Before you begin, we recommend you read the sections on the different \z
      types of environmental zones you may encounter later in the Book. It \z
      will help in planning your movement across the planet."
    -- Page 9 -------------------------------------------------------------- --
    },{ "When you first start the game, you will only be able to select one \z
      of two zones for your initial operation, either \"DHOBBS\" or \z
      \"AZERG.\" These are situated in the top left hand corner of the map. \z
      If you successfully complete one of these levels you may continue \z
      expanding your mining operation into any adjacent zones, and so work \z
      your way around the planet.\n\z
      \n\z
      When you successfully complete a zone, either economically or through \z
      wiping out your opponent, a flag of success will be raised on that \z
      zone.\n\z
      \n\z
      After selecting the zone of your choice, you are"
    -- Page 10 ------------------------------------------------------------- --
    },{ "returned back to the Controllers Office. If you are starting a new \z
      game, he will suggest you now choose a race of diggers in which to \z
      invest your money. Each race has different characteristics, different \z
      goals, different strengths and weaknesses. (See chapter on RACES later \z
      in the Book. We recommend you study the race descriptions before you \z
      make your choice, as you must use the same race throughout the game. \n\z
      \n\z
      In the Race Selection area, you can view a summary of the \z
      characteristics of each race, the sketched page and information can be \z
      turned by clicking in the right hand corner of the page. Selection is \z
      made by clicking"
    -- Page 11 ------------------------------------------------------------- --
    },{ "anywhere else on the page.\n\z
      \n\z
      Once you haue proceeded through these selection criteria you can begin \z
      the game.\n\z
      \n\z
      THE BANK\n\z
      \n\z
      The left hand door in the corridor of the Zargon Trading Centre.\n\z
      For a more detailed description of the Banking operations and the \z
      Zargon Stock Exchange, see the relevant chapters further in this book. \z
      But here is a brief description of how to use the system."
    -- Page 12 ------------------------------------------------------------- --
    },{ "If during your mining operations you mine some precious jewels or \z
      minerals you can sell them at the Bank. By selecting the home icon \z
      (see overleaf) you are transported back to the Zargon Trading Centre.\n\z
      \n\z
      When you first enter the Bank. The display screens above each of the\z
      teller's windows will show a representation of which minerals and \z
      jewels they are trading in. If you have any jewels of the type \z
      displayed, you may ask the teller what price he is currently paying \z
      for those minerals. If you feel that it is a good price, you can sell \z
      the jewels, and the value will be added to your Current Cash Account. \z
      If you d not feel it is a good price, or they are not trading"
    -- Page 13 ------------------------------------------------------------- --
    },{ "the minerals you are carrying, then you can leave the Bank and \z
      return at a later date to see if they are offering a better price or \z
      are trading in those jewels."
    -- Page 14 ------------------------------------------------------------- --
    },{ "CONTROL ICONS\n\z
        \n\z
        Character Mouement\n\z
        \n\z
        Digging\n\z
        \n\z
        Inventory Control\n\z
        \n\z
        Home                (only available at base camp)\n\z
        \n\z
        wait search\n\z
        \n\z
        Teleport\n\z
        \n\z
        DESCRIPTION OF RACE"
    -- Page 15 ------------------------------------------------------------- --
    },{ "\n\z
        Jump\n\z
        \n\z
        Walk Right\n\z
        \n\z
        Wait\n\z
        \n\z
        Run left\n\z
        \n\z
        Stop\n\z
        \n\z
        Run Right\n\z
        \n\z
        Return to main menu\n\z
        \n\z
        Walk left"
    -- Page 16 ------------------------------------------------------------- --
    },{ "\n\z
        Digging directions\n\z
        \n\z
        \n\z
        Pick up\n\z
        \n\z
        \n\z
        Put down and inventory\n\z
        \n\z
        \n\z
        Search\n\z
        \n\z
        \n\z
        Cycles between teleport"
    -- Page 17 ------------------------------------------------------------- --
    },{ "                 CONTROL PANEL\n\z
          Cash Account         Gems collected\n\z
      \n\z
      \n\z
                        Stamina        Who is winning\n\z
                                  Green flag is computer player \n\z
                                   Pink flag is you\n\z
      State of individual diggers\n\z
      Panic OK Bered  Dead      Diggers status   Electronic Book\n\z
      \n\z
      \n\z
      Digger selected      machine selection      Diggers location"
    -- Page 18 ------------------------------------------------------------- --
    },{ "DESCRIPTION OF RACE CHARACTERISTICS\n\z
        \n\z
        Stamina              Aggressiveness\n\z
        \n\z
        Strength             Specials\n\z
                             (Habbish only)\n\z
        Patience             Teleport power\n\z
                             (F'Targs only)\n\z
        Digging speed        Double healing\n\z
        \n\z
        Intelligence"
    -- Page 19 ------------------------------------------------------------- --
    },{ "LOADING & SAVING\n\z
        \n\z
        Between zones you are able to save your game or load an existing \z
        game. This is oone when you are standing at the controllers desk. \z
        Selecting the filing tray, will give you the option to save the game \z
        at that point or load an existing game."
    -- Page 20 ------------------------------------------------------------- --
    },{ "THE PLANET ZARG\n\z
        \n\z
        Stories of the Planet Zarg's mineral wealth are legendary. Large\z
        amounts of minerals and ores, including diamonds, rubies, emeralds \z
        and gold can be found below the planet's surtace, out the enormous \z
        volcanic activity that created these riches, also threw up a large \z
        number of perils and hazards. As a result, mining the Planet Zarg is \z
        an extremely hazardous operation. Early diggers lured by labels on \z
        space charts saying 'here he treasures' perished in their \z
        thousands.\n\z
        \n\z
        Another hazard facing workers was the fighting between rival races \z
        of diggers and the general lawlessness on"
    -- Page 21 ------------------------------------------------------------- --
    },{ "the planet. In addition, too much uncontrolled digging was damaging \z
      the planet's subterranean stability and huge chasms were beginning to \z
      appear without warning.\n\z
      \n\z
      The planet's authorities decided to act to contain these problems. The \z
      first result of their deliberations was to allow only one month's \z
      digging per year, beginning on the glorious 412th. For the remaining \z
      17 months a year, mining operations are forbidden.\n\z
      \n\z
      As well as the rule of the glorious 412th, the authorities have also \z
      cleaned up and formalised digging procedures on the planet. The \z
      following rules now also apply:"
    -- Page 22 ------------------------------------------------------------- --
    },{ "1. Only five races of diggers are permitted to dig on the planet.\n\z
      2. Each dig must be registered at the Zargon Mineral Trading Centre.\n\z
      3. All minerals mined must be exchanged for cash at the Zaroon Bank.\n\z
      4. To encourage healthy competition, two races of diggers are allowed \z
      to mine each area of the planet."
    -- Page 23 ------------------------------------------------------------- --
    },{ "RACE DESCRIPTIONS\n\z
      \n\z
      The Habbish\n\z
      An enigmatic secretive breed who are rumoured to be extremely clever \z
      and have developed special telepole transportation powers. These \z
      cowled creatures are the weakest of the races and although they could \z
      continue digging for a long time, they are very impatient and soon \z
      lose interest in digging, preferring wherever possible to pilfer \z
      aluables mined by others."
    -- Page 24 ------------------------------------------------------------- --
    },{ "The Habbish are a mystical order ruled by their Lord High Habborg. \z
      This exalted being has decreed that his followers must build a \z
      fabulous temple complex, encrusted with gold ano jewels in his name. \z
      The Habbish haue begun this work, but money is running out. They need \z
      to mine as many valuables as possible in order to complete the temple \z
      and pay off the galactic repo-men, the baseball bat wielding \z
      Thungurs.\n\z
      \n\z
      The Habbish are ruled by a most peculiar calendar and at various \z
      unpredictable and often inconuenient times, they will drop everything \z
      to gather into a circle and chant to the Lord High Habborg. They \z
      become easily upset if their digging plans are unsuccessful and bow to"
    -- Page 25 ------------------------------------------------------------- --
    },{ "their master for forgiveness if they do not regularly mine valuables."
    -- Page 26 ------------------------------------------------------------- --
    },{ "The Grablins\n\z
      Ideally suited to mining. They are very fast diggers and can keep \z
      digging for long periods of time without stopping. Their small size \z
      makes them very mobile about the mines as they can squeeze into \z
      narrow fissures and work in low tunnels. Although strong, they are \z
      not very good fighters and can be easily defeated by the Quarriers.\n\z
      \n\z
      The Gradins only weakness is for the fiendishly strong drink, Grok. \z
      Although described by others as an \"unaquirable taste\" with a smell \z
      \"worse than the"
    -- Page 27 ------------------------------------------------------------- --
    },{ "breath of a fire-breathing Scabrosaur from the swirling slime pools \z
      of Sulphuria\" and \"more userul as a defensive shield against thermo \z
      nuclear war than as a drink\", the Grablins cannot get enough of the \z
      stuff.\n\z
      \n\z
      Unfortunately, because the ingredients that make up Grok (ingredients \z
      too unfeasibly disgusting to be mentioned here) are extremely \z
      expensive, the Grablins constantly need money.\n\z
      \n\z
      Their ultimate aim is to amass enough riches to build their own \z
      brewery. However, due to the unpleasant side effects of brewing the \z
      drink, they first have to buy ther own deserted planet on which to \z
      site their brewery."
    -- Page 28 ------------------------------------------------------------- --
    },{ "The Quarriors\n\z
      This warlike race are a bunch of real rough diamonds. As their name \z
      suggests, the Quarriors began searching for riches in quarries before \z
      graduating downwards to open cast mining and then into digging.\n\z
      \n\z
      The strongest of all the races, the Quarriors are also expert \z
      saboteurs and dynamite with dynamite. However, the Quarriors have not \z
      yet adapted well to cramped mining conditions, they tire easily and \z
      are slow at digging. They are extremely reliable and patient, but \z
      lack initiative."
    -- Page 29 ------------------------------------------------------------- --
    },{ "The Quarriors are flat broke as they were recently tricked by a \z
      second-hand arms salescreature. Their ambition is to build a fortified \z
      encampment where they can practice weapons and digging skills safe \z
      from their enemies."
    -- Page 30 ------------------------------------------------------------- --
    },{ "The F'Targs\n\z
      This resilient race of diggers are extremely curious and great \z
      collectors of scrap metal. They have an insatiable desire to build \z
      things from the scraps they are always picking up. As a result, their \z
      buildings and machines all have a shambolic patched-up appearance.\n\z
      \n\z
      The F'Targs are the second fastest diggers. They are slower than the \z
      Grablins but can continue mining much longer than the others. They \z
      enjoy digging but can be distracted by objects that take their fancy. \z
      Their"
    -- Page 31 ------------------------------------------------------------- --
    },{ "desire to collect often gets them into trouble outside the mines. \z
      They are not very aggressive or good at fighting but if hurt, they can \z
      heal themselves twice as quickly as any other diggers.\n\z
      \n\z
      The F'Targs ambition is to collect enough money to build their \z
      proposed Museum of Metal Marvels (unkindly nicknamed the Scrapheap) in \z
      which they wish to house historic scrap and sculptures of an unusual \z
      or enlightening nature."
    -- Page 32 ------------------------------------------------------------- --
    },{ "ZONE DESCRIPTIONS\n\z
      \n\z
      Grassland - a flat savannah area with rivers breaking up the swathes \z
      of grass. Below ground are more rivers. cauerns and a small amount of \z
      impenetrable rocks. Recent finds include fossilised remains of large \z
      creatures of unknown origin.\n\z
      \n\z
      Forest/Jungle - mainly flat area broken up by undulating rivers and \z
      small lakes. The top soil in this area is extremely rich in nutrients \z
      and supports the growth of giant trees that reach high up into the \z
      sky. The roots of these trees grow to an enormous length and depth in \z
      the earth. Where roots have grown"
    -- Page 33 ------------------------------------------------------------- --
    },{ "around each other, they form thick entanglements that are too \z
      strong to be dug through or removed. Pit heads should be set up in \z
      clearings between trees to avoid these roots. Wild stories of strange \z
      plant life in this area abound, but no proof has yet been found to \z
      confirm them.\n\z
      \n\z
      Desert - shifting sands and dunes cover this arid area of Zarg. The \z
      effects of erosion are evident here. Huge rock formations have been  \z
      buried by the sand and compressed into impervious strength.\n\z
      \n\z
      Huge, brightly-coloured crystal structures stretch"
    -- Page 34 ------------------------------------------------------------- --
    },{ "under the sand. Underground lakes and water sources exist below \z
      the surface.\n\z
      \n\z
      Ice - this is an area of freezing conditions with ice-cold seas and \z
      many ice-bergs. Particular care should be taken when digging within \z
      these levels because of the danger of flooding, especially in \z
      Icebergs.\n\z
      \n\z
      Islands - this area is made up of a vast archipelago of islands dotted \z
      around a large ocean. Ail islands are joined deep below the water's \z
      surface to form a huge sub-aqua mountain range.\n\z
      \n\z
      Mountains - area of jagged peaks and unstable rocky"
    -- Page 35 ------------------------------------------------------------- --
    },{ "slopes which offers few places suitable for digging. However, \z
      scattered through the mountains are large caves which may provide \z
      better opportunities for operations.\n\z
      \n\z
      Below the tough surface are deposits of hard rock which make digging \z
      impossible in some areas. Water sources can also be found below \z
      ground.\n\z
      \n\z
      Rocky Ground - an area resembling the Grand Canyon, it has numerous \z
      overhangs, precipices and precarious rock formations. Below the \z
      surface are large areas of impenetrable rock. Very little water is \z
      evident near the surface but deep in the earth, water filled caverns"
    -- Page 36 ------------------------------------------------------------- --
    },{ "abound. The course of long dried up rivers has created series of \z
      interlocked caves and passages between rock layers. Lost cities are \z
      believed to be in this level, possibly haunted by their former \z
      occupants."
    -- Page 37 ------------------------------------------------------------- --
    },{ "FLORA AND FAUNA\n\z
      \n\z
      The disappearance of the entire Frinklin Expedition of '95 dealt a \z
      severe blow to anthropological and botanical studies on the Planet \z
      Zaro. As a result, no conclusive report on Zarg's flora and fauna \z
      exists - the following notes and pictures have been taken from a \z
      variety of professional and eye witness reports - and it is certain \z
      that other plant and animal life exists on the planet. Please send \z
      details, samples or sketches of new life forms to Professor A. Mazon, \z
      Tower of Creepers, Attenborough Institute of Galactic Greenery."
    -- Page 38 ------------------------------------------------------------- --
    },{ "Triffidus Carnivorous\n\z
      \n\z
      Lives in jungle and forest areas where it blends in with other trees \z
      and foliage. Extremely ferocious carnivorous plant, possessing a large \z
      appetite. Can be identified by its unusual leaf colour. Upon capture \z
      and infra-red studying, one specimen's stomach was found to contain a \z
      beginner's guide to spotting dangerous plants and a pair of spectacles \z
      of a similar prescription to Dr Frinklin's."
    -- Page 39 ------------------------------------------------------------- --
    },{ "Fungus Kaleidoscopus\n\z
      \n\z
      Found on the surface in a variety of locations, these fungi grow in \z
      large clusters. Easily recognised by large red 'hat' with white spots. \z
      During a scientific study, results showed that eating the mushrooms \z
      can have a wide variety of effects upon Some simply died, some became \z
      twice as strong while others seemed distracted and talked gibberish \z
      about pink giraffes."
    -- Page 40 ------------------------------------------------------------- --
    },{ "Stegosaurus\n\z
        \n\z
        Large dinosaur that lives in subterranean caves. Sandy coloured \z
        skin, two horns.\n\z
        Normally docile, Stegosaurus will charge if provoked or threatened. \z
        In narrow tunnels Stegosaurus is capable of"
    -- Page 41 ------------------------------------------------------------- --
    },{ "crushing victories over enemies.\n\z
      \n\z
      Rotorysaurus\n\z
      \n\z
      A rather strange dinosaur, who inhabits the subterranean levels but \z
      occasionally strays to the surface of the planet. Generally considered \z
      to be rather placid, however, if provoked or attacked can turn and \z
      inflict severe damage on it's aggressors.\n\z
      \n\z
      Velociraptor\n\z
      \n\z
      A dinosaur of quite astounding viciousness. Needs absolutely no \z
      provocation to attack. Simply hates the"
    -- Page 42 ------------------------------------------------------------- --
    },{ "sight of almost all other creatures, and although small in stature \z
      can deliver a blow with the strength of a Stegosaurus. Avoid at all \z
      costs. If encountered, run away very quickly, or lure into your \z
      opponents mines then run away"
    -- Page 43 ------------------------------------------------------------- --
    },{ "Eggus Horribilis\n\z
      \n\z
      Provenance unknown. The only description of these eggs is supplied by \z
      a miner, now retired. His story is below.\n\z
      \n\z
      \"It was 'orrible. The guvnor said we should dig into"
    -- Page 44 ------------------------------------------------------------- --
    },{ "this bit of rock. Well, I don't mind telling you, I didn't like it, \z
      not one bit. Something was wrong you know, it wasn't quite right ... \z
      Anyway, not that I'm one to go on, we dug through this rock and found \z
      ourselues in a cave. I flashed the light around but it seemed empty. \z
      \"Let's go Bolbo,\" I said. \"Time for Grok break.\" Well he didn't \z
      come, he'd seen this dirty great egg. \"Leave it alone,\" I said, but \z
      he didn't listen. He picked it up and this thing leapt out at him. ooh \z
      it was 'orrible, it sort of absorbed itself into him. I couldn't \z
      watch, I turned and ran. It was a terrible thing, where's that \z
      bottle ...?\""
    -- Page 45 ------------------------------------------------------------- --
    },{ "Woolly Mammals\n\z
      \n\z
      Large mammals who are thought to have lived on Zarg a million years \z
      ago. The frozen remains of one ot these beasts was discovered one year \z
      ago and more may be discovered in a petrified condition in icy areas. \z
      Woolly"
    -- Page 46 ------------------------------------------------------------- --
    },{ "mammal meat has been eaten by starving Arctic explorers:\n\z
      \"Mnmn, just give me a ... second to stop mnmnmn ... chewing ...\"\n\z
      \"I think vegetarianism is about to get a new conuert.\"\n\z
      \"Hommm, hommm, Lord Habborg, hommm, hommm.\""
    -- Page 47 ------------------------------------------------------------- --
    },{ "Fish\n\z
      \n\z
      Many varieties of fish are belleved to exist. However anglers' stories \z
      have proved somewhat unreliable. Few examples have ever been brought \z
      to the institute, \"it"
    -- Page 48 ------------------------------------------------------------- --
    },{ "got away\" - and accurate size estimates have proved impossible. \z
      Tales of the ferocious \"Pikosaurus\" must also be taken with a pinch \z
      of salt (and maybe freshly squeezed lemon)."
    -- Page 49 ------------------------------------------------------------- --
    },{ "Sand Worms\n\z
      \n\z
      Large land-locked beasts of obscure gender. These shy creatures are \z
      believed to live deep underground, they are rarely seen and little is \z
      known of their behavioural patterns. The following is a Quarrior's \z
      account of his encounter with a sano worm"
    -- Page 50 ------------------------------------------------------------- --
    },{ "\"We were minding our own business, tunnelling away quietly - well, \z
      as quiet as you can get with dynamite and suddenly the floor starts \z
      moving below me. I don't mind telling you that it confused me for a \z
      minute, I hardly had time to think before I fell back and cracked my \z
      head. Luckily that seemed to help and I realised it was one of them \z
      wormy things - big an' all, it was. Mind you I wasn't frightened, I \z
      grabbed a shovel and was about to take it on. \"Come on.\" I said to \z
      it, but it just sort of wormed its way down the tunnel and \z
      disappeared - I wonder if that's how it got its name, you know wormed? \z
      ... Anyway, I reckon they're chicken...\""
    -- Page 51 ------------------------------------------------------------- --
    },{ "Mysterious sightings!\n\z
      \n\z
      Over the years, rumours have surfaced about strange ethereal beings \z
      inhabiting the caverns deep under the planet Zarqg Miners who have \z
      escaped back to the surface and who could retell their stories speak \z
      of slow moving ghostly apparitions attacking the group, killing the \z
      men, further stories talk of digger like creatures with great speed \z
      swooping down out of the roofs of large caverns and killing swiftly. \z
      Mining stories that are retold in the many bars on the planet, say \z
      that the Ghosts are from those miners who were left to die by claim \z
      jumpers, and the Zombies are the result of those ghosts contact with \z
      living diggers"
    -- Page 52 ------------------------------------------------------------- --
    },{ "Our advice is to avoid these supposed apparitions at all costs. Not \z
      that tne authoritles belleve in such things. you understand.\n\z
      \n\z
      Other strange reported sights include small 'alien' looking creatures \z
      who seem to be connected in someway with the Eggus Horribilus, \z
      sightings of these are unconfirmed as all scientific expeditions to \z
      investigate these rumours have failed to return. They could be \z
      dangerous!\n\z
      \n\z
      Scattered throughout the planet are mowing portals, nicknamed \z
      Swirlyports. These unusual objects float gently through the \z
      subterranean world. If a digger is"
    -- Page 53 ------------------------------------------------------------- --
    },{ "caught in one of these he is instantly transported to a random area \z
      in the zone. The religious zealots of the Habbish are convinced that \z
      the Swirlyports are the spiritual remains of those Habbish who have \z
      not gained full enlightenment and are destined to wander the Planet \z
      Zarg in their present unfulfilled state. Other less spiritually \z
      minded diggers put them down to yet another wonder of this strange \z
      planet, and do not seek an explanation for their existence. A much \z
      more sensible approach, we think.\n\z
      \n\z
      Large areas of the planet remain uncharted and largely undiscovered, \z
      and for this reason the above details are no more than current \z
      intelligence. So take heed ano be"
    -- Page 54 ------------------------------------------------------------- --
    },{ "aware that there is always more than meets the eye."
    -- Page 55 ------------------------------------------------------------- --
    },{ "THE MINING STORE\n\z
      \n\z
      A wide variety of mining equipment is available in the Mining Store. \z
      All the stock has been chosen by a veteran miner with detailed \z
      knowledge of the dangerous conditions underground. The majority of the \z
      equipment was bought at knockdown prices from a rocket boot sale on a \z
      small Earth territory called Britain, where mining is now a distant \z
      memory.\n\z
      \n\z
      When entering the Mining Store, the affable shopkeeper is only too \z
      happy to guide you through the buying process. His stock book \z
      contains prices and descriptions of all equipment in stock. A hologram \z
      of each piece of"
    -- Page 56 ------------------------------------------------------------- --
    },{ "equipment automatically appears to enable the buyer to view his \z
      potential purchase.\n\z
      \n\z
      To chose a particular item, simply click on the appropriate symbol \z
      and its price will be automatically debited from your CASH account."
    -- Page 57 ------------------------------------------------------------- --
    },{ "MINING EQUIPMENT\n\z
      \n\z
      Flood gates\n\z
      \n\z
      Price: 80 credits\n\z
      Weight: 10 groads\n\z
      General Information: Flood gates can be opened or closed only by their \z
      installers. Extremely useful in areas with flash-flood dangers. Can be \z
      used to secure mining operations from rival diggers. Flood gates are \z
      extremely resilient to pressure and can withstand most digging \z
      machines, but they can be opened by large amounts of explosives"
    -- Page 58 ------------------------------------------------------------- --
    },{ "Telepole\n\z
      Price: 260 credits\n\z
      Weight: 12 groads\n\z
      General Information: Possibly THE most vital piece of equipment for \z
      any digger. The telepole allows its user to travel between telepoles \z
      instantly. At the beginning of mining operations, each race of diggers \z
      is supplied with a teleport. Habbish can"
    -- Page 59 ------------------------------------------------------------- --
    },{ "use any of the other diggers' telepoles. Other diggers can only use \z
      their telepoles."
    -- Page 60 ------------------------------------------------------------- --
    },{ "Train Track\n\z
      Price: 10 credits\n\z
      Weight: 3 groads\n\z
      General Information: Individual sections of train track are quite \z
      short, but are provided in lengths of 5. The weight given is for a \z
      pack of 5. Track must be positioned correctly. Once laid in position, \z
      it forms a permanent bond with the ground and cannot be moved, nor can \z
      it be dug through."
    -- Page 61 ------------------------------------------------------------- --
    },{ "Automatic Mine Cart\n\z
      Price: 108 credits\n\z
      Weight: 8 groads\n\z
      General Information: An ingenious self-propelling and self-steering \z
      mine cart capable of holding large amounts of minerals. Ideal for \z
      quick transport of diggers or minerals between locations."
    -- Page 62 ------------------------------------------------------------- --
    },{ "Small Tunnelling Machine\n\z
      Price: 150 credits\n\z
      Weight: 8 groads\n\z
      General Information: Cordless machine. Nicknamed 'the mole', this \z
      tunnelling machine is light and portable. Drills many times quicker \z
      than the fastest spade-wielding digger."
    -- Page 63 ------------------------------------------------------------- --
    },{ "Single Bridge Bit\n\z
      Price: 25 credits\n\z
      Weight: 3 groads\n\z
      General Information: Invaluable item to bridge streams and rivers. \n\z
      Extremely strong, can take heauy loads. Bridge bit must be securely \z
      anchored on firm ground. Bridges are vulnerable targets - caution must \z
      be exercised when crossing them."
    -- Page 64 ------------------------------------------------------------- --
    },{ "Inflatable Boat\n\z
      Price: 60 credits\n\z
      Weight: 5 groads\n\z
      General Information: Extremely durable crafts. Carrying capacity is \z
      limited to one digger. Unsuitable for rough seas or long voyages."
    -- Page 65 ------------------------------------------------------------- --
    },{ "Vertical Digger\n\z
      Price: 170 credits\n\z
      Weight: 10 groads\n\z
      General Information: Nicknamed 'the corkscrew' because of its drilling \z
      action.\n\z
      This machine can only be useo to dig vertically downwards. Once \z
      started, the 'corkscrew' continues drilling automatically for a set \z
      period of time or until"
    -- Page 66 ------------------------------------------------------------- --
    },{ "it hits an obstacle. Beware using this equipment in watery areas."
    -- Page 67 ------------------------------------------------------------- --
    },{ "Large Tunnelling Machine\n\z
      Price: 230 credits\n\z
      Weight: 11 groads\n\z
      General Information: Better known as the 'Monster' this machine is a \z
      formidable digging tool.\n\z
      Able to dig through rocks extremely quickly. Extreme caution should be \z
      exercised when operating the 'Monster'. Point at surface to be \z
      drilled and start up. Do not stand in front of the 'Monster' at any \z
      time."
    -- Page 68 ------------------------------------------------------------- --
    },{ "Explosives\n\z
      Price: 20 credits\n\z
      Weight: 4 groads\n\z
      General Information: WARNING: Handle with care.\n\z
      Vital equipment for blasting through solid rocks or for entering a \z
      rivals' mine.\n\z
      There is very little margin for error when using explosives, mistakes \z
      are often fatal!"
    -- Page 69 ------------------------------------------------------------- --
    },{ "Lift\n\z
      Price: 220 credits\n\z
      Weight: 12 groads\n\z
      General Information: Extremely useful for transporting large amounts \z
      of minerals, equipment or diggers between successful seam and mine \z
      head. Once in position, the lift is permanently sited and cannot be \z
      moved to another location. Lift must be installed correctly - the head \z
      block at the top of the lift and the foot block at the"
    -- Page 70 ------------------------------------------------------------- --
    },{ "bottom MUST be firmly anchored in a block of plain earth. Without \z
      these element in the correct position the lift will refuse to function."
    -- Page 71 ------------------------------------------------------------- --
    },{ "TNT Map\n\z
      Price: 215 Credits\n\z
      Weight: 3 groads\n\z
      General Information: Probably the most useful item a digger can have, \z
      apart from the guide! Utilising the special abilities of TNT paper, \z
      the map shows the entire zone with all it's features and is constantly \z
      updated."
    -- Page 72 ------------------------------------------------------------- --
    },{ "TNT Map\n\z
      Price: 215 Credits\n\z
      Weight: 3 groads\n\z
      General Information: Probably the most useful item a digger can have, \z
      apart from the guide! Utilising the special abilities of TNT paper, \z
      the map shows the entire zone with all it's features and is constantly \z
      updated."
    -- Page 73 ------------------------------------------------------------- --
    },{ "First Aid Kit\n\z
      Price: 60 credits\n\z
      Weight: 5 groads\n\z
      General Information: Once purchased, the healing powers of the kit \z
      will slowly replenish the stamina of the digger holding it. It is \z
      possible to give the kit to another digger who may benefit from any \z
      remaining \'medicine\'\n\z
      \n\z
      i. Economic conditions can cause these prices to"
    -- Page 74 ------------------------------------------------------------- --
    },{ "fluctuate dependent on availability of products.\n\z
      \n\z
      ii. The Management reserve the right to change everything and anything \z
      at anytime and without warning."
    -- Page 75 ------------------------------------------------------------- --
    },{ "ZARGON BANK\n\z
      \n\z
      Recently strengthened by a merger with the Badland Bank, the Zargon \z
      Bank has gone from strength to strength and has expanded it's \z
      operations to include stock market facilities. The Zargon Bank now \z
      controls all money transactions on the planet and has devised a \z
      savings plan for all its customers. The slightly unusual thing about \z
      this plan is that it is mandatory.\n\z
      \n\z
      When a digger opens a current CASH account, the bank opens a savings \z
      STASH account for that customer. The bank will then automatically \z
      transfer a percentage of all CASH and save it in the STASH account. \z
      Money"
    -- Page 76 ------------------------------------------------------------- --
    },{ "cannot be taken from the STASH account until a digger wishes to \z
      leave the planet.\n\z
      \n\z
      A Bank spokescreature, explained \"It's an insurance policy against \z
      customers absconding from the planet and leaving us with debts.\"\n\z
      \n\z
      When a mining operation is first started, the Bank is quite prepared \z
      to make a loan of 100 credits to the Master Miner. However, the Master \z
      Miner must repay the loan at the end of each successful digging \z
      operation within a zone.\n\z
      \n\z
      The Bank is then prepared to make up any deficit that"
    -- Page 77 ------------------------------------------------------------- --
    },{ "a Master Miner may have, when starting the next Zone, to ensure \z
      that they have at least 100 credits. Obuiously, if the Master Miner \z
      has more than 108 credits already, then they will not require the \z
      services of the Bank in providing a loan.\n\z
      \n\z
      The Bank also operates a generous Trade-in policy for capital \z
      equipment, (lifts, mining machines, etc). Assuming a Master Miner is \z
      successful in completing a mining area, they would not wish to leave \z
      behind all the equipment purchased to complete their operations. \z
      Therefore, the Bank arranges for it's valuers to wisit the mine, \z
      remoue the equipment, and pay about 75% of its value, assuming it is \z
      in a salvageable condition, to"
    -- Page 78 ------------------------------------------------------------- --
    },{ "the Master Miner. This ensures that the Master Miner sees some \z
      return on his investment in equipment. This amount is carried forward \z
      without further deductions spending on the next zone. Also the Bank \z
      makes an extremely large profit selling the equipment as new back to \z
      other mining operations, but of course that is only a vicious rumour \z
      spread about by disgruntled, bankrupt miners."
    -- Page 79 ------------------------------------------------------------- --
    },{ "ZARGON STOCK MARKET\n\z
      \n\z
      The Zargon Stock Market follows the rules of supply ano demand like \z
      all other stock markets. It Is linked to a network of other markets \z
      throughout the galaxy, as a result mineral trading and mineral prices \z
      are often affected by dealings and incidents on other planets.\n\z
      \n\z
      Above the bank tellers is a display showing the minerals or jewels \z
      currently being traded by that teller. Selecting the display shows \z
      you the value for that mineral or jewel. Selecting the teller will \z
      enable you to sell any mineral or jewel that you may have."
    -- Page 80 ------------------------------------------------------------- --
    },{ "No other minerals or jewels will be bought at that time but the \z
      bank's dealing interests are flexible and it will buy other minerals \z
      at other times. Prices too are constantly changing in response to the \z
      law of supply and demand. For example: a big find of gold will flood \z
      the market and lower the gold's value.\n\z
      \n\z
      One precious stone always retains it's value however Jennite, a pink \z
      jewel, is extremely rare and hence very valuable and so you will find \z
      that any of the tellers will be keen to trade in it at all times."
    -- Page 81 ------------------------------------------------------------- --
    },{ "ZARGON MINING HISTORY\n\z
      \n\z
      The history of the planet Zarg is as colourful as the jewels that have \z
      been mined from below its surface. Its legendary riches haue attracted \z
      an enormous number of space jetsam and flotsam, diggers, bounty \z
      hunters and other get rich quick merchants.\n\z
      \n\z
      Unfortunately, the planet does not give up its wealth easily and very \z
      few have left richer than when they arrived. Indeed, many have not \z
      left at all. The cause of Zarg's wealth is also the cause of many \z
      mining problems. The underground riches are caused by violent volcanic \z
      and tectonic activity that convulse the planet"
    -- Page 82 ------------------------------------------------------------- --
    },{ "for eight months a year. While this regenerates mineral wealth \z
      below the surface, it destroys most structures above ground. The \z
      remains of many lost cities and civilisations are belleveo to be deep \z
      underground, where they were swallowed up by the planet's upheavals."
    -- Page 83 ------------------------------------------------------------- --
    },{ "DAYLIGHT ROBBERY\n\z
      \n\z
      The Zargon Bank has been the subject of many attempted robberies. The \z
      most successtul robbery was carried out by the legendary jewel thief \z
      Larson E.\n\z
      \n\z
      Wearing a disguise and relying on sharp wits, Larson E. posed as a \z
      trouble shooter sent by the President of Interplanetary Banks Inc to \z
      check their security systems. so convincing was he that the staff gave \z
      him a guided tour, showing their alarms and giving him code numbers.\n\z
      \n\z
      That night the bank robber returned to make good use"
    -- Page 84 ------------------------------------------------------------- --
    },{ "of the information. When the staff opened up in the morning they \z
      found the vaults empty and the following note.\n\z
      \n\z
      \"It wasn't me,\n\z
      It was D.A. Lite & Rob Berry.\n\z
      He he he,\n\z
      Thanks for the rocks suckers.\"\n\z
      \n\z
      Larson E."
    -- Page 85 ------------------------------------------------------------- --
    },{ "THE CRASH OF '94\n\z
      \n\z
      Also known as Black Tuesday (not to be confused with blank Tuesday \z
      when all the dealing systems failed - a F'Targ was later sacked for \z
      taking micro chips from machines. His excuse \"they're for my \z
      collection\" was deemed unacceptable) the bank was rocked to its \z
      foundations when a runaway Monster', digging machine smashed into the \z
      underground vaults.\n\z
      \n\z
      Millions of credits went missing and the thieves were never traced, \z
      although for the entire following month no Grablin was seen vertical."
    -- Page 86 ------------------------------------------------------------- --
    },{ "Ancient Mining Aduice and Player Tips\n\z
      \n\z
      Past digging expeditions have often returned with records of what \z
      appears to be ancient mining graffiti etched into cavern walls. The \z
      apparent purpose of the graffiti being to inform and advise miners who \z
      may follow in the artist's footsteps.\n\z
      \n\z
      The key points being:\n\z
      \n\z
      If you are impatient for riches you will reap more by mining in strips \z
      than deep shafts.\n\z
      \n\z
      Use bridges wisely as they can not only cross water"
    -- Page 87 ------------------------------------------------------------- --
    },{ "but will empower you to jump higher and further.\n\z
      \n\z
      Don't exhaust yourself trying to dig through the immovable, blow the \z
      darn thing out with explosives, but mind you don't flood yourself in \z
      the process. However you can't blow an escape route out of the zone.\n\z
      \n\z
      Blood lust and killing may wipe out your the Zargan Bank trades in \z
      Zogs not bodies, so dig for gems not revenge.\n\z
      \n\z
      Other than this sound advice you would be well advised to"
    -- Page 88 ------------------------------------------------------------- --
    },{ "- Save the game every time you have completed a zone.\n\z
      \n\z
      - Always trade jewels at the bank immediately to maximise your wealth \z
      against that of your opponent, and to prevent theft of jewels below \z
      ground.\n\z
      \n\z
      - Always be mindful of the capital value of all equipment. They can \z
      prove extremely useful as assets to be traded to start a new zone."
    }
  }, -- -------------------------------------------------------------------- --
  -- French pages ---------------------------------------------------------- --
  ["fr"] = {
  },
  -- German pages ---------------------------------------------------------- --
  ["de"] = {
  },
  -- Italian pages --------------------------------------------------------- --
  ["it"] = {
  }
};-- ----------------------------------------------------------------------- --
-- Imports and exports ----------------------------------------------------- --
return { F = Util.Blank, A = {         -- Sending API to main loader
  -- Exports --------------------------------------------------------------- --
  aAIChoicesData = aAIChoicesData, aAITypesData = AI,
  aBookData = aBookData, aCreditsData = aCreditsData,
  aCreditsXData = aCreditsXData, aCursorData = aCursorData,
  aCursorIdData = CID, aDigBlockData = aDigBlockData, aDigData = aDigData,
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
