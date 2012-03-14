// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================
#ifndef SIMULATIONCRAFT_H
#define SIMULATIONCRAFT_H

// Platform Initialization ==================================================

#if defined( _MSC_VER ) || defined( __MINGW__ ) || defined( __MINGW32__ ) || defined( _WINDOWS ) || defined( WIN32 )
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRALEAN
#  define _CRT_SECURE_NO_WARNINGS
#  define DIRECTORY_DELIMITER "\\"
#  ifndef UNICODE
#    define UNICODE
#  endif
#else
#  define DIRECTORY_DELIMITER "/"
#  define SC_SIGACTION
#endif

#if defined(_LP64) || defined(__LP64__) || defined(_WIN64) || defined(_AMD64_)
#  define SC_X64
#endif

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

// Boost includes
#include <boost/checked_delete.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>

#if __BSD_VISIBLE
#  include <netinet/in.h>
#  if !defined(CLOCKS_PER_SEC)
#    define CLOCKS_PER_SEC 1000000
#  endif
#endif

#if defined( NO_THREADS )
#elif defined( __MINGW32__ ) || defined( _MSC_VER )
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else // POSIX
#include <pthread.h>
#include <unistd.h>
#endif

#if defined(__GNUC__)
#  define likely(x)   __builtin_expect((x),1)
#  define unlikely(x) __builtin_expect((x),0)
#else
#  define likely(x)    (x)
#  define unlikely(x)  (x)
#  define __attribute__(x)
#endif

#define PRINTF_ATTRIBUTE(a,b) __attribute__((format(printf,a,b)))
#define SC_FINLINE_EXT        __attribute__((always_inline))

#if defined( _MSC_VER )
# define finline              __forceinline
#else
# define finline              inline
#endif

#define SC_MAJOR_VERSION "114"
#define SC_MINOR_VERSION "4"
#define SWTOR_VERSION_LIVE "1.1.5"
#define SWTOR_VERSION_PTR "1.2.0"
#define SC_USE_PTR ( 1 )
#define SC_BETA ( 0 )
#define SC_EPSILON ( 0.000001 )
#ifndef M_PI
#define M_PI ( 3.14159265358979323846 )
#endif

#define MAX_PLAYERS_PER_CHART 20

// TODO: Integer time is only partially working.
//#define SC_USE_INTEGER_TIME

// Forward Declarations =====================================================

struct action_t;
struct action_callback_t;
struct action_expr_t;
struct action_priority_list_t;
struct alias_t;
class attack_policy_interface_t;
struct benefit_t;
struct buff_t;
struct buff_uptime_t;
struct callback_t;
struct companion_t;
struct cooldown_t;
struct dot_t;
struct effect_t;
struct enemy_t;
struct enchant_t;
struct event_t;
struct gain_t;
struct heal_t;
struct item_t;
struct js_node_t;
struct option_t;
struct pet_t;
struct player_t;
struct plot_t;
struct proc_t;
struct raid_event_t;
struct rating_t;
struct reforge_plot_t;
struct reforge_plot_data_t;
struct report_t;
struct rng_t;
struct sample_data_t;
struct scaling_t;
struct sim_t;
struct stats_t;
struct talent_t;
struct timespan_t;
struct unique_gear_t;
struct uptime_t;
struct weapon_t;
struct xml_node_t;

// Advanced Classes
struct commando_mercenary_t;
struct gunslinger_sniper_t;
struct juggernaut_guardian_t;
struct sentinel_marauder_t;
struct sage_sorcerer_t;
struct scoundrel_operative_t;
struct shadow_assassin_t;
struct vanguard_powertech_t;

typedef const attack_policy_interface_t* attack_policy_t;

// Targetdata
class targetdata_t
{
public:
  player_t& source;
  player_t& target;

private:
  std::vector<buff_t*> buffs;
  std::vector<dot_t*> dots;
  std::unordered_map<std::string,buff_t*> buffs_by_name;
  std::unordered_map<std::string,dot_t*> dots_by_name;

public:
  targetdata_t( player_t& s, player_t& t );
  virtual ~targetdata_t();

  void    add( buff_t& );
  void    alias( buff_t&, const std::string& name );
  void    add( dot_t& );
  void    alias( dot_t&, const std::string& name );

  buff_t* get_buff( const std::string& name ) const;
  dot_t*  get_dot( const std::string& name ) const;

  void reset();
  void clear_debuffs();
};

// Enumerations =============================================================

enum race_type
{
  RACE_NONE=0,
  // Target Races
  RACE_BEAST, RACE_DRAGONKIN, RACE_GIANT, RACE_HUMANOID, RACE_DEMON, RACE_ELEMENTAL,
  // Player Races
  RACE_CHISS, RACE_CYBORG, RACE_HUMAN, RACE_MIRALUKA, RACE_MIRIALAN, RACE_RATTATAKI, RACE_SITH_PUREBLOOD, RACE_TWILEK, RACE_ZABRAK,
  RACE_MAX
};

enum player_type
{
  PLAYER_SPECIAL_SCALE=-1,
  PLAYER_NONE=0,

  SITH_MARAUDER, SITH_JUGGERNAUT,
  SITH_ASSASSIN, SITH_SORCERER,
  BH_MERCENARY, BH_POWERTECH,
  IA_SNIPER, IA_OPERATIVE,

  JEDI_GUARDIAN, JEDI_SENTINEL,
  JEDI_SAGE, JEDI_SHADOW,
  S_GUNSLINGER, S_SCOUNDREL,
  T_COMMANDO, T_VANGUARD,

  PLAYER_PET, PLAYER_COMPANION, PLAYER_GUARDIAN,
  ENEMY, ENEMY_ADD,
  PLAYER_MAX
};

enum pet_type_t
{
  PET_NONE=0,

  PET_ENEMY,

  PET_MAX
};

enum dmg_type { DMG_DIRECT=0, DMG_OVER_TIME=1, HEAL_DIRECT, HEAL_OVER_TIME, ABSORB };

enum stats_type { STATS_DMG, STATS_HEAL, STATS_ABSORB };

enum dot_behavior_type { DOT_CLIP=0, DOT_REFRESH };

enum attribute_type { ATTRIBUTE_NONE=0, ATTR_STRENGTH, ATTR_AIM, ATTR_CUNNING, ATTR_WILLPOWER, ATTR_ENDURANCE, ATTR_PRESENCE, ATTRIBUTE_MAX };

enum base_stat_type { BASE_STAT_STRENGTH=0, BASE_STAT_AIM, BASE_STAT_CUNNING, BASE_STAT_WILLPOWER, BASE_STAT_ENDURANCE, BASE_STAT_PRESENCE,
                      BASE_STAT_HEALTH, BASE_STAT_MANA,
                      BASE_STAT_MELEE_CRIT_PER_AGI, BASE_STAT_SPELL_CRIT_PER_INT,
                      BASE_STAT_DODGE_PER_AGI,
                      BASE_STAT_MELEE_CRIT, BASE_STAT_SPELL_CRIT, BASE_STAT_SPI_REGEN, BASE_STAT_MAX
                    };

enum resource_type
{
  RESOURCE_NONE=0,
  RESOURCE_HEALTH, RESOURCE_MANA,  RESOURCE_RAGE, RESOURCE_ENERGY, RESOURCE_AMMO,
  RESOURCE_FORCE,
  RESOURCE_MAX
};

enum result_type
{
  RESULT_UNKNOWN=-1,
  RESULT_NONE=0,
  RESULT_MISS,  RESULT_AVOID,
  RESULT_BLOCK, RESULT_CRIT, RESULT_HIT,
  RESULT_MAX
};

#define RESULT_HIT_MASK  ( (1<<RESULT_BLOCK) | (1<<RESULT_CRIT) | (1<<RESULT_HIT) )
#define RESULT_CRIT_MASK ( (1<<RESULT_CRIT) )
#define RESULT_MISS_MASK ( (1<<RESULT_MISS) )
#define RESULT_NONE_MASK ( (1<<RESULT_NONE) )
#define RESULT_ALL_MASK  ( -1 )

enum proc_type
{
  PROC_NONE=0,
  PROC_DAMAGE,
  PROC_HEAL,
  PROC_TICK_DAMAGE,
  PROC_DIRECT_DAMAGE,
  PROC_DIRECT_HEAL,
  PROC_TICK_HEAL,
  PROC_ATTACK,
  PROC_SPELL,
  PROC_TICK,
  PROC_SPELL_AND_TICK,
  PROC_HEAL_SPELL,
  PROC_HARMFUL_SPELL,
  PROC_DAMAGE_HEAL,
  PROC_MAX
};

enum action_type { ACTION_USE=0, ACTION_ATTACK, ACTION_HEAL, ACTION_ABSORB, ACTION_SEQUENCE, ACTION_OTHER, ACTION_MAX };

enum school_type
{
  SCHOOL_NONE=0,

  SCHOOL_KINETIC,  SCHOOL_ENERGY,
  SCHOOL_INTERNAL, SCHOOL_ELEMENTAL,

  SCHOOL_MAX
};

#define SCHOOL_ALL_MASK ( ( 1 << SCHOOL_KINETIC ) | ( 1 << SCHOOL_ENERGY ) | \
                          ( 1 << SCHOOL_INTERNAL ) | ( 1 << SCHOOL_ELEMENTAL ) )

enum talent_tree_type
{
  TREE_NONE=0,
  TREE_SEER,         TREE_TELEKINETICS,  TREE_BALANCE,
  TREE_CORRUPTION,   TREE_LIGHTNING,     TREE_MADNESS,
  TREE_KINETIC_COMBAT, TREE_INFILTRATION,
  TREE_DARKNESS, TREE_DECEPTION,
  TALENT_TREE_MAX
};

enum talent_tab_type
{
  TALENT_TAB_NONE = -1,

  JEDI_SAGE_SEER = 0,           JEDI_SAGE_TELEKINETICS,   JEDI_SAGE_BALANCE,
  SITH_SORCERER_CORRUPTION = 0, SITH_SORCERER_LIGHTNING,  SITH_SORCERER_MADNESS,

  JEDI_SHADOW_KINETIC_COMBAT = 0,   JEDI_SHADOW_INFILTRATION, JEDI_SHADOW_BALANCE,
  SITH_ASSASSIN_DARKNESS = 0,       SITH_ASSASSIN_DECEPTION,  SITH_ASSASSIN_MADNESS,
};

enum weapon_type
{

  WEAPON_NONE=0,
  WEAPON_LIGHTSABER, WEAPON_VIBROKNIFE, WEAPON_TECHBLADE, WEAPON_VIBROSWORD,
  WEAPON_1H,
  WEAPON_DOUBLE_BLADED_LIGHTSABER, WEAPON_ELECTROSTAFF, WEAPON_TECHSTAFF,
  WEAPON_2H,
  WEAPON_ASSAULT_CANON, WEAPON_BLASTER_PISTOL, WEAPON_BLASTER_RIFLE, WEAPON_SCATTERGUN, WEAPON_SNIPER_RIFLE,
  WEAPON_RANGED,
  WEAPON_MAX
};

enum slot_type
{
  SLOT_NONE      = -1,
  SLOT_HEAD      = 0,
  SLOT_EAR       = 1,
  SLOT_SHOULDERS = 2,
  SLOT_SHIRT     = 3,
  SLOT_CHEST     = 4,
  SLOT_WAIST     = 5,
  SLOT_LEGS      = 6,
  SLOT_FEET      = 7,
  SLOT_WRISTS    = 8,
  SLOT_HANDS     = 9,
  SLOT_IMPLANT_1 = 10,
  SLOT_IMPLANT_2 = 11,
  SLOT_RELIC_1   = 12,
  SLOT_RELIC_2   = 13,
  SLOT_BACK      = 14,
  SLOT_MAIN_HAND = 15,
  SLOT_OFF_HAND  = 16,
  SLOT_RANGED    = 17,
  SLOT_TABARD    = 18,
  SLOT_MAX       = 19
};

typedef uint_fast32_t slot_mask_t;
static_assert( SLOT_MAX <= std::numeric_limits<slot_mask_t>::digits,
               "slot masks won't fit in slot_mask_t." );

constexpr slot_mask_t slot_mask( slot_type s )
{ return slot_mask_t( 1 ) << s; }

template <typename ... Types>
constexpr slot_mask_t slot_mask( slot_type s, Types ... args )
{ return slot_mask( s ) | slot_mask( args... ); }

constexpr slot_mask_t DEFAULT_SET_BONUS_SLOT_MASK = slot_mask( SLOT_HEAD, SLOT_CHEST, SLOT_HANDS, SLOT_LEGS, SLOT_FEET );

// Keep this in sync with enum attribute_type
enum stat_type
{
  STAT_NONE=0,
  STAT_STRENGTH, STAT_AIM, STAT_CUNNING, STAT_WILLPOWER, STAT_ENDURANCE, STAT_PRESENCE,
  STAT_HEALTH, STAT_MANA, STAT_RAGE, STAT_ENERGY, STAT_AMMO,
  STAT_MAX_HEALTH, STAT_MAX_MANA, STAT_MAX_RAGE, STAT_MAX_ENERGY, STAT_MAX_AMMO,
  STAT_EXPERTISE_RATING, STAT_EXPERTISE_RATING2,
  STAT_ACCURACY_RATING, STAT_ACCURACY_RATING2, STAT_CRIT_RATING, STAT_ALACRITY_RATING,
  STAT_WEAPON_DMG,
  STAT_WEAPON_OFFHAND_DMG,
  STAT_ARMOR, STAT_BONUS_ARMOR,
  STAT_POWER, STAT_FORCE_POWER, STAT_TECH_POWER,
  STAT_SURGE_RATING,
  STAT_DEFENSE_RATING, STAT_SHIELD_RATING, STAT_ABSORB_RATING,
  STAT_MAX
};

enum stim_type
{
  STIM_NONE=0,
  STIM_EXOTECH_RESOLVE,
  STIM_RAKATA_RESOLVE,
  STIM_MAX
};

enum position_type { POSITION_NONE=0, POSITION_FRONT, POSITION_BACK, POSITION_RANGED_FRONT, POSITION_RANGED_BACK, POSITION_MAX };

enum profession_type
{
  PROFESSION_NONE=0,
  PROFESSION_UNKNOWN,
  PROFESSION_BIOCHEM,
  PROFESSION_MAX
};

enum role_type { ROLE_NONE=0, ROLE_ATTACK, ROLE_SPELL, ROLE_HYBRID, ROLE_DPS, ROLE_TANK, ROLE_HEAL, ROLE_MAX };

enum rng_type
{
  // Specifies the general class of RNG desired
  RNG_DEFAULT=0,     // Do not care/know where it will be used
  RNG_GLOBAL,        // Returns reference to global RNG on sim_t
  RNG_DETERMINISTIC, // Returns reference to global deterministic RNG on sim_t
  RNG_CYCLIC,        // Normalized even/periodical results are acceptable
  RNG_DISTRIBUTED,   // Normalized variable/distributed values should be returned

  // Specifies a particular RNG desired
  RNG_STANDARD,          // Creates RNG using srand() and rand()
  RNG_MERSENNE_TWISTER,  // Creates RNG using Double precision SIMD-oriented Fast Mersenne Twister (dSFMT)
  RNG_PHASE_SHIFT,       // Simplistic cycle-based RNG, unsuitable for overlapping procs
  RNG_DISTANCE_SIMPLE,   // Simple normalized proc-separation RNG, suitable for fixed proc chance
  RNG_DISTANCE_BANDS,    // Complex normalized proc-separation RNG, suitable for varying proc chance
  RNG_PRE_FILL,          // Deterministic number of procs with random distribution
  RNG_MAX
};

enum save_type
{
  // Specifies the type of profile data to be saved
  SAVE_ALL=0,
  SAVE_GEAR,
  SAVE_TALENTS,
  SAVE_ACTIONS,
  SAVE_MAX
};

// Data Access ==============================================================
#ifndef MAX_LEVEL
#define MAX_LEVEL (50)
#endif

#ifndef MAX_RANK
#define MAX_RANK (3)
#endif

#ifndef NUM_SPELL_FLAGS
#define NUM_SPELL_FLAGS (10)
#endif

#ifndef MAX_EFFECTS
#define MAX_EFFECTS (3)
#endif

#ifndef MAX_TALENT_TABS
#define MAX_TALENT_TABS (3)
#endif

#ifndef MAX_TALENTS
#define MAX_TALENTS (100)
#endif

enum power_type
{
  POWER_MANA = 0,
  POWER_RAGE = 1,
  POWER_FOCUS = 2,
  POWER_ENERGY = 3,
  POWER_HAPPINESS = 4,
  // not yet used
  POWER_RUNE = 5,
  POWER_RUNIC_POWER = 6,
  POWER_SOUL_SHARDS = 7,
  POWER_ECLIPSE = 8,
  POWER_HOLY_POWER = 9,
  POWER_HEALTH = 0xFFFFFFFE, // (or -2 if signed)
  POWER_NONE = 0xFFFFFFFF, // None.
  //these are not yet used
  //POWER_RUNE_BLOOD = 10,
  //POWER_RUNE_FROST = 11,
  //POWER_RUNE_UNHOLY = 12
};

enum rating_type
{
  RATING_MELEE_ACCURACY,
  RATING_RANGED_ACCURACY,
  RATING_SPELL_ACCURACY,
  RATING_MELEE_CRIT,
  RATING_RANGED_CRIT,
  RATING_SPELL_CRIT,
  RATING_MELEE_ALACRITY,
  RATING_RANGED_ALACRITY,
  RATING_SPELL_ALACRITY,
  RATING_EXPERTISE,
  RATING_MASTERY,
  RATING_MAX
};

// Type utilities and generic programming tools =============================
template <typename T, std::size_t N>
inline std::size_t sizeof_array( const T ( & )[N] )
{ return N; }

class noncopyable
{
public:
  noncopyable() = default;
  noncopyable( noncopyable&& ) = default;
  noncopyable& operator = ( noncopyable&& ) = default;
private:
  noncopyable( const noncopyable& ) = delete;
  noncopyable& operator = ( const noncopyable& ) = delete;
};

class nonmoveable : public noncopyable
{
public:
  nonmoveable() = default;
private:
  nonmoveable( const nonmoveable& ) = delete;
  nonmoveable& operator = ( const nonmoveable& ) = delete;
  nonmoveable( nonmoveable&& ) = delete;
  nonmoveable& operator = ( nonmoveable&& ) = delete;
};

class delete_disposer_t
{
public:
  template <typename T>
  void operator() ( T* t ) const
  { boost::checked_delete( t ); }
};

// Generic algorithms =======================================================

template <typename I, typename D=delete_disposer_t>
void dispose( I first, I last, D disposer=D() )
{
  while ( first != last )
  {
    I tmp = first;
    ++first;
    disposer( *tmp );
  }
}

template <typename Range, typename D=delete_disposer_t>
inline auto dispose( Range&& r, D disposer=D() ) -> decltype( std::forward<Range>( r ) )
{
  dispose( std::begin( r ), std::end( r ), disposer );
  return std::forward<Range>( r );
}

template <typename T, typename D=delete_disposer_t>
void dispose_list( T* t, D disposer=D() )
{
  while ( t )
  {
    T* tmp = t;
    t = t -> next;
    disposer( tmp );
  }
}

template <unsigned HW, typename Fwd, typename Out>
void sliding_window_average( Fwd first, Fwd last, Out out )
{
  typedef typename std::iterator_traits<Fwd>::value_type value_t;
  typedef typename std::iterator_traits<Fwd>::difference_type diff_t;
  const diff_t n = std::distance( first, last );
  const diff_t HALFWINDOW = static_cast<diff_t>( HW );

  if ( n >= 2 * HALFWINDOW )
  {
    value_t window_sum = value_t();

    // Fill right half of sliding window
    Fwd right = first;
    for ( diff_t count = 0; count < HALFWINDOW; ++count )
      window_sum += *right++;

    // Fill left half of sliding window
    for ( diff_t count = HALFWINDOW; count < 2 * HALFWINDOW; ++count )
    {
      window_sum += *right++;
      *out++ = window_sum / ( count + 1 );
    }

    // Slide until window hits end of data
    while ( right != last )
    {
      window_sum += *right++;
      *out++ = window_sum / ( 2 * HALFWINDOW + 1 );
      window_sum -= *first++;
    }

    // Empty right half of sliding window
    for ( diff_t count = 2 * HALFWINDOW; count > HALFWINDOW; --count )
    {
      *out++ = window_sum / count;
      window_sum -= *first++;
    }
  }
  else
  {
    // input is pathologically small compared to window size, just average everything.
    std::fill_n( out, n, std::accumulate( first, last, value_t() ) / n );
  }
}

template <unsigned HW, typename Range, typename Out>
inline auto sliding_window_average( Range&& r, Out out ) -> decltype( std::forward<Range>( r ) )
{
  sliding_window_average<HW>( boost::begin( r ), boost::end( r ), out );
  return std::forward<Range>( r );
}

struct timespan_t
{

#ifdef SC_USE_INTEGER_TIME

#ifdef SC_X64
  typedef int64_t time_t;
#else
  // CAREFUL: Using int32_t implies that no overflowing happens during calculation.
  typedef int32_t time_t;
#endif

private:
  time_t time;

  static const time_t MILLIS_PER_SECOND;
  static const double SECONDS_PER_MILLI;
  static const time_t MILLIS_PER_MINUTE;
  static const double MINUTES_PER_MILLI;


  explicit timespan_t( const time_t millis ) : time( millis ) { }

public:

  double total_minutes() const { return time * MINUTES_PER_MILLI; }
  double total_seconds() const { return time * SECONDS_PER_MILLI; }
  time_t total_millis() const { return time; }

  static timespan_t from_millis( const uint64_t millis ) { return timespan_t( ( time_t )millis ); }
  static timespan_t from_millis( const int64_t millis ) { return timespan_t( ( time_t )millis ); }
  static timespan_t from_millis( const uint32_t millis ) { return timespan_t( ( time_t )millis ); }
  static timespan_t from_millis( const int32_t millis ) { return timespan_t( ( time_t )millis ); }
  static timespan_t from_millis( const double millis ) { return timespan_t( ( time_t )millis ); }
  static timespan_t from_seconds( const double seconds ) { return timespan_t( ( time_t )( seconds * MILLIS_PER_SECOND ) );  }
  static timespan_t from_minutes( const double minutes ) { return timespan_t( ( time_t )( minutes * MILLIS_PER_MINUTE ) ); }

#else // !SC_USE_INTEGER_TIME

  typedef double time_t;

private:
  time_t time;

  static const double MILLIS_PER_SECOND;
  static const double MINUTES_PER_SECOND;
  static const time_t SECONDS_PER_MILLI;
  static const time_t SECONDS_PER_MINUTE;


  explicit timespan_t( const time_t millis ) : time( millis ) { }

public:
  double total_minutes() const { return time * MINUTES_PER_SECOND; }
  double total_seconds() const { return time; }
  time_t total_millis() const { return time * MILLIS_PER_SECOND; }

  static timespan_t from_millis( const uint64_t millis ) { return timespan_t( ( time_t )millis * SECONDS_PER_MILLI ); }
  static timespan_t from_millis( const int64_t millis ) { return timespan_t( ( time_t )millis * SECONDS_PER_MILLI ); }
  static timespan_t from_millis( const uint32_t millis ) { return timespan_t( ( time_t )millis * SECONDS_PER_MILLI ); }
  static timespan_t from_millis( const int32_t millis ) { return timespan_t( ( time_t )millis * SECONDS_PER_MILLI ); }
  static timespan_t from_millis( const double millis ) { return timespan_t( ( time_t )millis * SECONDS_PER_MILLI ); }
  static timespan_t from_seconds( const double seconds ) { return timespan_t( seconds );  }
  static timespan_t from_minutes( const double minutes ) { return timespan_t( ( time_t )( minutes * SECONDS_PER_MINUTE ) ); }

#endif

public:
  timespan_t() : time( 0 ) { }

  bool operator==( const timespan_t right ) const { return time == right.time; }
  bool operator!=( const timespan_t right ) const { return time != right.time; }

  bool operator>( const timespan_t right ) const { return time > right.time; }
  bool operator>=( const timespan_t right ) const { return time >= right.time; }
  bool operator<( const timespan_t right ) const { return time < right.time; }
  bool operator<=( const timespan_t right ) const { return time <= right.time; }

  timespan_t & operator+=( const timespan_t right )
  {
    time += right.time;
    return *this;
  }
  timespan_t operator-=( const timespan_t right )
  {
    time -= right.time;
    return *this;
  }
  timespan_t operator*=( const double right )
  {
    time = ( time_t )( time * right );
    return *this;
  }
  timespan_t operator*=( const int32_t right )
  {
    time *= right;
    return *this;
  }
  timespan_t operator*=( const int64_t right )
  {
    time = ( time_t )( time * right );
    return *this;
  }
  timespan_t operator*=( const uint32_t right )
  {
    time *= right;
    return *this;
  }
  timespan_t operator*=( const uint64_t right )
  {
    time = ( time_t )( time * right );
    return *this;
  }

  timespan_t operator/=( const double right )
  {
    time = ( time_t )( time / right );
    return *this;
  }
  timespan_t operator/=( const int32_t right )
  {
    time /= right;
    return *this;
  }
  timespan_t operator/=( const int64_t right )
  {
    time = ( time_t )( time / right );
    return *this;
  }
  timespan_t operator/=( const uint32_t right )
  {
    time /= right;
    return *this;
  }
  timespan_t operator/=( const uint64_t right )
  {
    time = ( time_t )( time / right );
    return *this;
  }

  friend timespan_t operator+( const timespan_t right );
  friend timespan_t operator-( const timespan_t right );

  friend timespan_t operator+( const timespan_t left, const timespan_t right );
  friend timespan_t operator-( const timespan_t left, const timespan_t right );
  friend timespan_t operator*( const timespan_t left, const double right );
  friend timespan_t operator*( const timespan_t left, const int32_t right );
  friend timespan_t operator*( const timespan_t left, const int64_t right );
  friend timespan_t operator*( const timespan_t left, const uint32_t right );
  friend timespan_t operator*( const timespan_t left, const uint64_t right );
  friend timespan_t operator/( const timespan_t left, const double right );
  friend timespan_t operator/( const timespan_t left, const int32_t right );
  friend timespan_t operator/( const timespan_t left, const int64_t right );
  friend timespan_t operator/( const timespan_t left, const uint32_t right );
  friend timespan_t operator/( const timespan_t left, const uint64_t right );
  friend double operator/( const timespan_t left, const timespan_t right );

  friend timespan_t operator*( const double left, const timespan_t right );
  friend timespan_t operator*( const int32_t left, const timespan_t right );
  friend timespan_t operator*( const int64_t left, const timespan_t right );
  friend timespan_t operator*( const uint32_t left, const timespan_t right );
  friend timespan_t operator*( const uint64_t left, const timespan_t right );

  static const timespan_t zero;
  static const timespan_t min;
  static const timespan_t max;
};

#ifdef SC_USE_INTEGER_TIME
#define TIMESPAN_TO_NATIVE_VALUE(t) ((t).total_millis())
#define TIMESPAN_FROM_NATIVE_VALUE(v) (timespan_t::from_millis(v))
#else // #ifndef SC_USE_INTEGER_TIME
#define TIMESPAN_TO_NATIVE_VALUE(t) ((t).total_seconds())
#define TIMESPAN_FROM_NATIVE_VALUE(v) (timespan_t::from_seconds(v))
#endif

inline timespan_t operator+( const timespan_t right )
{
  return right;
}

inline timespan_t operator-( const timespan_t right )
{
  return timespan_t( -right.time );
}

inline timespan_t operator+( const timespan_t left, const timespan_t right )
{
  return timespan_t( left.time + right.time );
}
inline timespan_t operator-( const timespan_t left, const timespan_t right )
{
  return timespan_t( left.time - right.time );
}
inline timespan_t operator*( const timespan_t left, const double right )
{
  return timespan_t( ( timespan_t::time_t )( left.time * right ) );
}
inline timespan_t operator*( const timespan_t left, const int32_t right )
{
  return timespan_t( ( timespan_t::time_t )( left.time * right ) );
}
inline timespan_t operator*( const timespan_t left, const int64_t right )
{
  return timespan_t( ( timespan_t::time_t )( left.time * right ) );
}
inline timespan_t operator*( const timespan_t left, const uint32_t right )
{
  return timespan_t( ( timespan_t::time_t )( left.time * right ) );
}
inline timespan_t operator*( const timespan_t left, const uint64_t right )
{
  return timespan_t( ( timespan_t::time_t )( left.time * right ) );
}
inline timespan_t operator/( const timespan_t left, const double right )
{
  return timespan_t( ( timespan_t::time_t )( left.time / right ) );
}
inline timespan_t operator/( const timespan_t left, const int32_t right )
{
  return timespan_t( ( timespan_t::time_t )( left.time / right ) );
}
inline timespan_t operator/( const timespan_t left, const int64_t right )
{
  return timespan_t( ( timespan_t::time_t )( left.time / ( timespan_t::time_t )right ) );
}
inline timespan_t operator/( const timespan_t left, const uint32_t right )
{
  return timespan_t( ( timespan_t::time_t )( left.time / ( timespan_t::time_t )right ) );
}
inline timespan_t operator/( const timespan_t left, const uint64_t right )
{
  return timespan_t( ( timespan_t::time_t )( left.time / ( timespan_t::time_t )right ) );
}
inline double operator/( const timespan_t left, const timespan_t right )
{
  return ( double )left.time / right.time;
}

inline timespan_t operator*( const double left, const timespan_t right )
{
  return timespan_t( ( timespan_t::time_t )( left * right.time ) );
}
inline timespan_t operator*( const int32_t left, const timespan_t right )
{
  return timespan_t( ( timespan_t::time_t )( left * right.time ) );
}
inline timespan_t operator*( const int64_t left, const timespan_t right )
{
  return timespan_t( ( timespan_t::time_t )( left * right.time ) );
}
inline timespan_t operator*( const uint32_t left, const timespan_t right )
{
  return timespan_t( ( timespan_t::time_t )( left * right.time ) );
}
inline timespan_t operator*( const uint64_t left, const timespan_t right )
{
  return timespan_t( ( timespan_t::time_t )( left * right.time ) );
}

// Cache Control ============================================================

namespace cache { // ========================================================

typedef int era_t;
static const era_t INVALID_ERA = -1;
static const era_t IN_THE_BEGINNING = 0;  // A time before any other possible era;
                                          // used to mark persistent caches at load.

enum behavior_t
{
  ANY,      // * Use any version present in the cache, retrieve if not present.
  CURRENT,  // * Use only current info from the cache; validate old versions as needed.
  ONLY,     // * Use any version present in the cache, fail if not present.
};

class cache_control_t
{
private:
  era_t current_era;
  behavior_t player_cache_behavior;
  behavior_t item_cache_behavior;

public:
  cache_control_t() :
    current_era( IN_THE_BEGINNING ),
    player_cache_behavior( CURRENT ),
    item_cache_behavior( ANY )
  {}

  era_t era() const { return current_era; }
  void advance_era() { ++current_era; }

  behavior_t cache_players() const { return player_cache_behavior; }
  void cache_players( behavior_t b ) { player_cache_behavior = b; }

  behavior_t cache_items() const { return item_cache_behavior; }
  void cache_items( behavior_t b ) { item_cache_behavior = b; }

  static cache_control_t singleton;
};

// Caching system's global notion of the current time.
inline era_t era()
{ return cache_control_t::singleton.era(); }

// Time marches on.
inline void advance_era()
{ cache_control_t::singleton.advance_era(); }

// Get/Set default cache behaviors.
inline behavior_t players()
{ return cache_control_t::singleton.cache_players(); }
inline void players( behavior_t b )
{ cache_control_t::singleton.cache_players( b ); }

inline behavior_t items()
{ return cache_control_t::singleton.cache_items(); }
inline void items( behavior_t b )
{ cache_control_t::singleton.cache_items( b ); }

} // namespace cache ========================================================

// Options ==================================================================

enum option_type_t
{
  OPT_NONE=0,
  OPT_STRING,     // std::string*
  OPT_APPEND,     // std::string* (append)
  OPT_BOOL,       // int (only valid values are 1 and 0)
  OPT_INT,        // int
  OPT_FLT,        // double
  OPT_TIMESPAN,   // time interval
  OPT_LIST,       // std::vector<std::string>*
  OPT_FUNC,       // function pointer
  OPT_TALENT_RANK, // talent rank
  OPT_DEPRECATED,
  OPT_UNKNOWN
};

typedef bool ( *option_function_t )( sim_t* sim, const std::string& name, const std::string& value );

struct option_t
{
  const char* name;
  int type;
  void* address;

  void print( FILE* );
  void save ( FILE* );
  bool parse( sim_t*, const std::string& name, const std::string& value );

  static void add( std::vector<option_t>&, const char* name, int type, void* address );
  static void copy( std::vector<option_t>& opt_vector, const option_t* opt_array );
  static bool parse( sim_t*, std::vector<option_t>&, const std::string& name, const std::string& value );
  static bool parse( sim_t*, const char* context, std::vector<option_t>&, const std::string& options_str );
  static bool parse( sim_t*, const char* context, const option_t*,        const std::string& options_str );
  static bool parse_file( sim_t*, FILE* file );
  static bool parse_line( sim_t*, char* line );
  static bool parse_token( sim_t*, std::string& token );
  static option_t* merge( std::vector<option_t>& out, const option_t* in1, const option_t* in2 );
};

// Talent Translation =======================================================

#define MAX_TALENT_POINTS 41
#define MAX_TALENT_ROW ((MAX_TALENT_POINTS+4)/5)
#define MAX_TALENT_TREES 3
#define MAX_TALENT_COL 4
#define MAX_TALENT_SLOTS (MAX_TALENT_TREES*MAX_TALENT_ROW*MAX_TALENT_COL)
#define MAX_TALENT_RANK_SLOTS ( 90 )

// Utilities ================================================================

#ifdef _MSC_VER
// C99-compliant snprintf - MSVC _snprintf is NOT the same.

#undef vsnprintf
int vsnprintf_simc( char* buf, size_t size, const char* fmt, va_list ap );
#define vsnprintf vsnprintf_simc

#undef snprintf
inline int snprintf( char* buf, size_t size, const char* fmt, ... )
{
  va_list ap;
  va_start( ap, fmt );
  int rval = vsnprintf( buf, size, fmt, ap );
  va_end( ap );
  return rval;
}
#endif

struct util_t
{
private:
  static void str_to_utf8_( std::string& str );
  static void str_to_latin1_( std::string& str );
  static void urlencode_( std::string& str );
  static void urldecode_( std::string& str );
  static void format_text_( std::string& name, bool input_is_utf8 );
  static void html_special_char_decode_( std::string& str );
  static void tolower_( std::string& );
  static void string_split_( std::vector<std::string>& results, const std::string& str, const char* delim, bool allow_quotes );
  static void replace_all_( std::string&, const char* from, char to );
  static void replace_all_( std::string&, char from, const char* to );
  static int vfprintf_helper( FILE *stream, const char *format, va_list fmtargs );

public:
  static double talent_rank( int num, int max, double increment );
  static double talent_rank( int num, int max, double value1, double value2, ... );

  static int talent_rank( int num, int max, int increment );
  static int talent_rank( int num, int max, int value1, int value2, ... );

  static double ability_rank( int player_level, double ability_value, int ability_level, ... );
  static int    ability_rank( int player_level, int    ability_value, int ability_level, ... );

  static const char* attribute_type_string     ( int type );
  static const char* dmg_type_string           ( int type );
  static const char* stim_type_string         ( int type );
  static const char* player_type_string        ( int type );
  static const char* pet_type_string           ( int type );
  static const char* position_type_string      ( int type );
  static const char* profession_type_string    ( int type );
  static const char* race_type_string          ( int type );
  static const char* role_type_string          ( int type );
  static const char* resource_type_string      ( int type );
  static const char* result_type_string        ( int type );
  static const char* school_type_string        ( int type );
  static const char* armor_type_string         ( player_type ptype, int slot_type );
  static const char* slot_type_string          ( int type );
  static const char* stat_type_string          ( int type );
  static const char* stat_type_abbrev          ( int type );
  static const char* stat_type_wowhead         ( int type );
  static int         talent_tree               ( int tree, player_type ptype );
  static const char* talent_tree_string        ( int tree, bool armory_format = true );
  static const char* weapon_type_string        ( int type );
  static const char* item_quality_string       ( int item_quality );

  static int parse_attribute_type              ( const std::string& name );
  static int parse_dmg_type                    ( const std::string& name );
  static int parse_stim_type                  ( const std::string& name );
  static player_type parse_player_type         ( const std::string& name );
  static pet_type_t parse_pet_type             ( const std::string& name );
  static int parse_profession_type             ( const std::string& name );
  static position_type parse_position_type     ( const std::string& name );
  static race_type parse_race_type             ( const std::string& name );
  static role_type parse_role_type             ( const std::string& name );
  static int parse_resource_type               ( const std::string& name );
  static int parse_result_type                 ( const std::string& name );
  static school_type parse_school_type         ( const std::string& name );
  static int parse_slot_type                   ( const std::string& name );
  static stat_type parse_stat_type             ( const std::string& name );
  static stat_type parse_reforge_type          ( const std::string& name );
  static int parse_talent_tree                 ( const std::string& name );
  static int parse_weapon_type                 ( const std::string& name );
  static int parse_item_quality                ( const std::string& quality );

  static bool parse_origin( std::string& region, std::string& server, std::string& name, const std::string& origin );

  static int class_id_mask( int type );
  static int class_id( int type );
  static unsigned race_mask( int type );
  static unsigned race_id( int type );
  static unsigned pet_mask( int type );
  static unsigned pet_id( int type );
  static player_type pet_class_type( int type );

  static const char* class_id_string( int type );
  static int translate_class_id( int cid );
  static int translate_class_str( const std::string& s );
  static race_type translate_race_id( int rid );
  static profession_type translate_profession_id( int skill_id );

  static int string_split( std::vector<std::string>& results, const std::string& str, const char* delim, bool allow_quotes = false )
  { string_split_( results, str, delim, allow_quotes ); return static_cast<int>( results.size() ); }
  static int string_split( const std::string& str, const char* delim, const char* format, ... );
  static void string_strip_quotes( std::string& str );
  static std::string& replace_all( std::string& s, const char* from, char to )
  { replace_all_( s, from, to ); return s; }
  static std::string& replace_all( std::string& s, char from, const char* to )
  { replace_all_( s, from, to ); return s; }

  template <typename T>
  static std::string to_string( const T& t )
  { std::ostringstream s; s << t; return s.str(); }

  static std::string to_string( double f );
  static std::string to_string( double f, int precision );

  static int64_t milliseconds();
  static int64_t parse_date( const std::string& month_day_year );

  static int printf( const char *format, ... ) PRINTF_ATTRIBUTE( 1,2 );
  static int fprintf( FILE *stream, const char *format, ... ) PRINTF_ATTRIBUTE( 2,3 );
  static int vfprintf( FILE *stream, const char *format, va_list fmtargs ) PRINTF_ATTRIBUTE( 2,0 )
  { return vfprintf_helper( stream, format, fmtargs ); }
  static int vprintf( const char *format, va_list fmtargs ) PRINTF_ATTRIBUTE( 1,0 )
  { return vfprintf( stdout, format, fmtargs ); }

  static std::string& str_to_utf8( std::string& str ) { str_to_utf8_( str ); return str; }
  static std::string& str_to_latin1( std::string& str ) { str_to_latin1_( str ); return str; }
  static std::string& urlencode( std::string& str ) { urlencode_( str ); return str; }
  static std::string& urldecode( std::string& str ) { urldecode_( str ); return str; }

  static std::string& format_text( std::string& name, bool input_is_utf8 )
  { format_text_( name, input_is_utf8 ); return name; }

  static std::string& html_special_char_decode( std::string& str )
  { html_special_char_decode_( str ); return str; }

  static bool str_compare_ci( const std::string& l, const std::string& r );
  static bool str_in_str_ci ( const std::string& l, const std::string& r );
  static bool str_prefix_ci ( const std::string& str, const std::string& prefix );

  static double floor( double X, unsigned int decplaces = 0 );
  static double ceil( double X, unsigned int decplaces = 0 );
  static double round( double X, unsigned int decplaces = 0 );

  static std::string& tolower( std::string& str ) { tolower_( str ); return str; }

private:
  static void format_name_( std::string& s );
public:
  static std::string& format_name( std::string& s )
  { format_name_( s ); return s; }

  static int snprintf( char* buf, size_t size, const char* fmt, ... ) PRINTF_ATTRIBUTE( 3,4 );

  template <typename T, std::size_t N>
  static std::vector<T> array_to_vector( const T (&array)[N] )
  { return std::vector<T>( array, array + N ); }
};

// Spell information struct, holding static functions to output spell data in a human readable form

/*
struct spell_info_t
{
  static std::string to_str( sim_t* sim, const spell_data_t* spell );
  static std::string to_str( sim_t* sim, uint32_t spell_id );
  static std::string talent_to_str( sim_t* sim, const talent_data_t* talent );
  static std::ostringstream& effect_to_str( sim_t* sim, const spell_data_t* spell, const spelleffect_data_t* effect, std::ostringstream& s );
};
*/

// Spell ID class

enum s_type_t
{
  T_SPELL = 0,
  T_TALENT,
  T_MASTERY,
  T_GLYPH,
  T_CLASS,
  T_RACE,
  T_SPEC,
  T_ITEM
};

// Talent class
class talent_t
{
private:
  unsigned    _rank;
  unsigned    _tab_page;    // Talent tab page
  const char* _name;        // Talent name
  //unsigned    _col;         // Talent column
  //unsigned    _row;         // Talent row
  unsigned    _max_rank;


public:
  // Careful: name needs
  talent_t( player_t* p, const char* name, unsigned tab_page, unsigned max_rank );

  bool set_rank( unsigned value );
  bool ok() const { return ( _rank > 0 ); }

  unsigned rank() const { return _rank; }
  unsigned max_rank() const { return _max_rank; }

  unsigned tab_page() const { return _tab_page; }

  const char* name_cstr() const { return _name; }
};

// Raid Event

struct raid_event_t
{
  sim_t* sim;
  std::string name_str;
  int64_t num_starts;
  timespan_t first, last;
  timespan_t cooldown;
  timespan_t cooldown_stddev;
  timespan_t cooldown_min;
  timespan_t cooldown_max;
  timespan_t duration;
  timespan_t duration_stddev;
  timespan_t duration_min;
  timespan_t duration_max;
  double     distance_min;
  double     distance_max;
  timespan_t saved_duration;
  rng_t* rng;
  std::vector<player_t*> affected_players;

  raid_event_t( sim_t*, const char* name );
  virtual ~raid_event_t() {}

  virtual timespan_t cooldown_time() const;
  virtual timespan_t duration_time() const;
  virtual void schedule();
  virtual void reset();
  virtual void start();
  virtual void finish();
  virtual void parse_options( option_t*, const std::string& options_str );
  virtual const char* name() const { return name_str.c_str(); }
  static raid_event_t* create( sim_t* sim, const std::string& name, const std::string& options_str );
  static void init( sim_t* );
  static void reset( sim_t* );
  static void combat_begin( sim_t* );
  static void combat_end( sim_t* ) {}
};

// Gear Stats ===============================================================

namespace internal {
struct gear_stats_t
{
  double attribute[ ATTRIBUTE_MAX ];
  double resource[ RESOURCE_MAX ];
  double expertise_rating;
  double expertise_rating2;
  double accuracy_rating;
  double accuracy_rating2;
  double crit_rating;
  double alacrity_rating;
  double surge_rating;
  double power;
  double force_power;
  double tech_power;
  double weapon_dmg;
  double weapon_offhand_dmg;
  double armor;
  double bonus_armor;
  double defense_rating;
  double shield_rating;
  double absorb_rating;
};
}

struct gear_stats_t : public internal::gear_stats_t
{
  typedef internal::gear_stats_t base_t;
  gear_stats_t() : base_t( base_t() ) {}

  void   add_stat( int stat, double value );
  void   set_stat( int stat, double value );
  double get_stat( int stat ) const;
  void   print( FILE* );
  static double stat_mod( int stat );
};


// Statistical Sample Data

struct sample_data_t
{
  std::vector<double> data;
  // Analyzed Results
  double sum;
  double mean;
  double min;
  double max;
  double variance;
  double std_dev;
  double mean_std_dev;
  std::vector<int> distribution;
  const bool simple;
  const bool min_max;

private:
  int count;

  bool analyzed_basics;
  bool analyzed_variance;
  bool created_dist;
  bool is_sorted;
public:

  sample_data_t( bool s=true, bool mm=false );

  void reserve( std::size_t capacity )
  { if ( ! simple ) data.reserve( capacity ); }

  void add( double x=0 );

  bool basics_analyzed() const { return analyzed_basics; }
  bool variance_analyzed() const { return analyzed_variance; }
  bool distribution_created() const { return created_dist; }
  bool sorted() const { return is_sorted; }
  int size() const { if ( simple ) return count; return ( int ) data.size(); }

  void analyze(
    bool calc_basics=true,
    bool calc_variance=true,
    bool s=true,
    unsigned int create_dist=0 );


  void analyze_basics();

  void analyze_variance();

  void sort();

  void create_distribution( unsigned int num_buckets=50 );

  double percentile( double );

  void merge( const sample_data_t& );

  void clear() { count = 0; sum = 0; data.clear(); distribution.clear(); }

  static double pearson_correlation( const sample_data_t&, const sample_data_t& );
};

// Buffs ====================================================================

struct buff_t
{
  struct actor_pair_t
  {
    player_t* target;
    player_t* source;

    actor_pair_t( player_t* target, player_t* source )
      : target( target ), source( source )
    {}

    actor_pair_t( player_t* p )
      : target( p ), source( p )
    {}

    actor_pair_t( targetdata_t* td );
  };

  double current_value, react;
  timespan_t buff_duration, buff_cooldown;
  double default_chance;
  timespan_t last_start;
  timespan_t last_trigger;
  timespan_t start_intervals_sum;
  timespan_t trigger_intervals_sum;
  timespan_t iteration_uptime_sum;
  int64_t up_count, down_count, start_intervals, trigger_intervals, start_count, refresh_count;
  int64_t trigger_attempts, trigger_successes;
  double benefit_pct, trigger_pct, avg_start_interval, avg_trigger_interval, avg_start, avg_refresh;
  std::string name_str;
  std::vector<timespan_t> stack_occurrence, stack_react_time;
  std::vector<buff_uptime_t> stack_uptime;
  sim_t* sim;
  player_t* player;
  player_t* source;
  player_t* initial_source;
  event_t* expiration;
  event_t* delay;
  rng_t* rng;
  cooldown_t* cooldown;
  buff_t* next;
  int current_stack, max_stack;
  int aura_id;
  int rng_type;
  bool activated;
  bool reverse, constant, quiet, overridden;
  sample_data_t uptime_pct;

  buff_t() : sim( 0 ) {}
  virtual ~buff_t();

  // Raid Aura
  buff_t( sim_t*, const std::string& name,
          int max_stack=1, timespan_t buff_duration=timespan_t::zero, timespan_t buff_cooldown=timespan_t::zero,
          double chance=1.0, bool quiet=false, bool reverse=false, int rng_type=RNG_CYCLIC, int aura_id=0 );

  // Player Buff
  buff_t( actor_pair_t pair, const std::string& name,
          int max_stack=1, timespan_t buff_duration=timespan_t::zero, timespan_t buff_cooldown=timespan_t::zero,
          double chance=1.0, bool quiet=false, bool reverse=false, int rng_type=RNG_CYCLIC, int aura_id=0, bool activated=true );

  // Use check() inside of ready() methods to prevent skewing of "benefit" calculations.
  // Use up() where the presence of the buff affects the action mechanics.

  int    check() { return current_stack; }
  inline bool   up()    { if ( current_stack > 0 ) { up_count++; } else { down_count++; } return current_stack > 0; }
  inline int    stack() { if ( current_stack > 0 ) { up_count++; } else { down_count++; } return current_stack; }
  inline double value() { if ( current_stack > 0 ) { up_count++; } else { down_count++; } return current_value; }
  timespan_t remains();
  bool   remains_gt( timespan_t time );
  bool   remains_lt( timespan_t time );
  bool   trigger  ( action_t*, int stacks=1, double value=-1.0 );
  virtual bool   trigger  ( int stacks=1, double value=-1.0, double chance=-1.0 );
  virtual void   execute ( int stacks=1, double value=-1.0 );
  virtual void   increment( int stacks=1, double value=-1.0 );
  void   decrement( int stacks=1, double value=-1.0 );
  void   extend_duration( player_t* p, timespan_t seconds );
  void   start_expiration( timespan_t );

  virtual void start    ( int stacks=1, double value=-1.0 );
  virtual void refresh  ( int stacks=0, double value=-1.0 );
  virtual void bump     ( int stacks=1, double value=-1.0 );
  virtual void override ( int stacks=1, double value=-1.0 );
  virtual bool may_react( int stacks=1 );
  virtual int stack_react();
  virtual void expire();
  virtual void predict();
  virtual void reset();
  virtual void aura_gain();
  virtual void aura_loss();
  virtual void merge( const buff_t* other_buff );
  virtual void analyze();
  void init_buff_shared();
  void init();
  void init_buff_t_();
  virtual void parse_options( va_list vap );
  virtual void combat_begin();
  virtual void combat_end();

  const char* name() { return name_str.c_str(); }

  action_expr_t* create_expression( action_t*, const std::string& type );

  static buff_t* find(   buff_t*, const std::string& name );
  static buff_t* find(    sim_t*, const std::string& name );
  static buff_t* find( player_t*, const std::string& name );
};

struct stat_buff_t : public buff_t
{
  double amount;
  int stat;

  stat_buff_t( player_t*, const std::string& name,
               int stat, double amount,
               int max_stack=1, timespan_t buff_duration=timespan_t::zero, timespan_t buff_cooldown=timespan_t::zero,
               double chance=1.0, bool quiet=false, bool reverse=false, int rng_type=RNG_CYCLIC, int aura_id=0, bool activated=true );

  virtual void bump     ( int stacks=1, double value=-1.0 );
  virtual void decrement( int stacks=1, double value=-1.0 );
  virtual void expire();
};

struct cost_reduction_buff_t : public buff_t
{
  double amount;
  int school;
  bool refreshes;

  cost_reduction_buff_t( player_t*, const std::string& name,
                         int school, double amount,
                         int max_stack=1, timespan_t buff_duration=timespan_t::zero, timespan_t buff_cooldown=timespan_t::zero,
                         double chance=1.0, bool refreshes=false, bool quiet=false, bool reverse=false, int rng_type=RNG_CYCLIC, int aura_id=0, bool activated=true );

  virtual void bump     ( int stacks=1, double value=-1.0 );
  virtual void decrement( int stacks=1, double value=-1.0 );
  virtual void expire();
  virtual void refresh  ( int stacks=0, double value=-1.0 );
};

struct debuff_t : public buff_t
{
  // Player De-Buff
  debuff_t( player_t*, const std::string& name,
            int max_stack=1, timespan_t buff_duration=timespan_t::zero, timespan_t buff_cooldown=timespan_t::zero,
            double chance=1.0, bool quiet=false, bool reverse=false, int rng_type=RNG_CYCLIC, int aura_id=0 );

};

typedef struct buff_t aura_t;

// Expressions ==============================================================

enum token_type_t
{
  TOK_UNKNOWN=0,
  TOK_PLUS,
  TOK_MINUS,
  TOK_MULT,
  TOK_DIV,
  TOK_ADD,
  TOK_SUB,
  TOK_AND,
  TOK_OR,
  TOK_NOT,
  TOK_EQ,
  TOK_NOTEQ,
  TOK_LT,
  TOK_LTEQ,
  TOK_GT,
  TOK_GTEQ,
  TOK_LPAR,
  TOK_RPAR,
  TOK_IN,
  TOK_NOTIN,
  TOK_NUM,
  TOK_STR,
  TOK_ABS,
  TOK_SPELL_LIST,
  TOK_FLOOR,
  TOK_CEIL
};

struct expr_token_t
{
  int type;
  std::string label;
};

enum expr_data_type_t
{
  DATA_SPELL = 0,
  DATA_TALENT,
  DATA_EFFECT,
  DATA_TALENT_SPELL,
  DATA_CLASS_SPELL,
  DATA_RACIAL_SPELL,
  DATA_MASTERY_SPELL,
  DATA_SPECIALIZATION_SPELL,
  DATA_GLYPH_SPELL,
  DATA_SET_BONUS_SPELL
};

struct expression_t
{
  static int precedence( int token_type );
  static int is_unary( int token_type );
  static int is_binary( int token_type );
  static int next_token( action_t* action, const std::string& expr_str, int& current_index, std::string& token_str, token_type_t prev_token );
  static void parse_tokens( action_t* action, std::vector<expr_token_t>& tokens, const std::string& expr_str );
  static void print_tokens( std::vector<expr_token_t>& tokens, sim_t* sim );
  static void convert_to_unary( action_t* action, std::vector<expr_token_t>& tokens );
  static bool convert_to_rpn( action_t* action, std::vector<expr_token_t>& tokens );
};

struct action_expr_t : public noncopyable
{
  action_t* action;
  std::string name_str;

  int result_type;
  double result_num;
  std::string result_str;

  action_expr_t( action_t* a, const std::string& n, int t=TOK_UNKNOWN ) :
    action( a ), name_str( n ), result_type( t ), result_num( 0 ) {}
  action_expr_t( action_t* a, const std::string& n, double constant_value ) :
    action( a ), name_str( n ), result_type( TOK_NUM ), result_num( constant_value ) {}
  action_expr_t( action_t* a, const std::string& n, std::string& constant_value ) :
    action( a ), name_str( n ), result_type( TOK_STR ), result_str( constant_value ) {}
  virtual ~action_expr_t() {}
  virtual int evaluate() { return result_type; }
  virtual const char* name() { return name_str.c_str(); }
  bool success() { return ( evaluate() == TOK_NUM ) && ( result_num != 0 ); }

  static action_expr_t* parse( action_t*, const std::string& expr_str );
};

namespace thread_impl { // ===================================================

#if defined( NO_THREADS )

class mutex : public nonmoveable
{
public:
  void lock() {}
  void unlock() {}
};

class thread : public noncopyable
{
public:
  virtual void run() = 0;

  void launch() { run(); } // Run sequentially in foreground.
  void wait() {}

  static void sleep( int seconds );
};

#elif defined( __MINGW32__ ) || defined( _MSC_VER )

class mutex : public nonmoveable
{
  CRITICAL_SECTION cs;
public:
  mutex() { InitializeCriticalSection( &cs ); }
  ~mutex() { DeleteCriticalSection( &cs ); }
  void lock() { EnterCriticalSection( &cs ); }
  void unlock() { LeaveCriticalSection( &cs ); }
};

class thread : public noncopyable
{
private:
  HANDLE handle;
public:
  virtual void run() = 0;

  void launch();
  void wait();

  static void sleep( int seconds ) { Sleep( seconds * 1000 ); }
};

#else // POSIX

class mutex : public nonmoveable
{
  pthread_mutex_t m;
public:
  mutex() { pthread_mutex_init( &m, NULL ); }
  ~mutex() { pthread_mutex_destroy( &m ); }
  void lock() { pthread_mutex_lock( &m ); }
  void unlock() { pthread_mutex_unlock( &m ); }
};

class thread : public noncopyable
{
  pthread_t t;
public:
  virtual void run() = 0;

  void launch();
  void wait() { pthread_join( t, NULL ); }

  static void sleep( int seconds ) { ::sleep( seconds ); }
};

#endif

} // namespace thread_impl ===================================================

typedef thread_impl::mutex mutex_t;

class thread_t : public thread_impl::thread
{
protected:
  thread_t() {}
  virtual ~thread_t() {}
public:
  static void init() {}
  static void de_init() {}
};

class auto_lock_t
{
private:
  mutex_t& mutex;
public:
  auto_lock_t( mutex_t& mutex_ ) : mutex( mutex_ ) { mutex.lock(); }
  ~auto_lock_t() { mutex.unlock(); }
};


// Simple freelist allocator for events =====================================

class event_freelist_t
{
private:
  struct free_event_t { free_event_t* next; };
  free_event_t* list;

public:
  event_freelist_t() : list( 0 ) {}
  ~event_freelist_t();

  void* allocate( std::size_t );
  void deallocate( void* );
};


// Simulation Engine ========================================================

struct sim_t : private thread_t
{
  int         argc;
  char**      argv;
  sim_t*      parent;
  event_freelist_t free_list;
  player_t*   target;
  player_t*   target_list;
  player_t*   player_list;
  player_t*   active_player;
  int         num_players;
  int         num_enemies;
  size_t      num_targetdata_ids;
  int         max_player_level;
  int         canceled;
  timespan_t  queue_lag, queue_lag_stddev;
  timespan_t  gcd_lag, gcd_lag_stddev;
  timespan_t  channel_lag, channel_lag_stddev;
  timespan_t  queue_gcd_reduction;
  int         strict_gcd_queue;
  double      confidence;
  double      confidence_estimator;
  // Latency
  timespan_t  world_lag, world_lag_stddev;
  double      travel_variance, default_skill;
  timespan_t  reaction_time, regen_periodicity;
  timespan_t  current_time, max_time, expected_time;
  double      vary_combat_length;
  timespan_t  last_event;
  int         fixed_time;
  int64_t     events_remaining, max_events_remaining;
  int64_t     events_processed, total_events_processed;
  int         seed, id, iterations, current_iteration, current_slot;
  int         armor_update_interval;
  int         optimal_raid, log, debug;
  int         save_profiles, default_actions;
  int         normalized_stat;
  std::string current_name, default_region_str, default_server_str, save_prefix_str,save_suffix_str;
  int         save_talent_str;
  bool        input_is_utf8;
  std::vector<player_t*> actor_list;
  std::string main_target_str;
  bool        ptr;

  // Target options
  double      target_death_pct;
  int         target_level;
  std::string target_race;
  int         target_adds;

  // Default stat enchants
  gear_stats_t enchant;

  std::unordered_map<std::string,std::string> var_map;
  std::vector<option_t> options;
  std::vector<std::string> party_encoding;
  std::vector<std::string> item_db_sources;


  // Random Number Generation
private:
  rng_t* default_rng_;     // == (deterministic_roll ? deterministic_rng : rng )
  rng_t* rng_list;
  int deterministic_roll;
public:
  rng_t* rng;
  rng_t* deterministic_rng;
  int smooth_rng, average_range, average_gauss;

  rng_t* default_rng() const { return default_rng_; }

  bool      roll( double chance ) const;
  double    range( double min, double max );
  double    gauss( double mean, double stddev );
  timespan_t gauss( timespan_t mean, timespan_t stddev );
  double    real() const;
  rng_t*    get_rng( const std::string& name, int type=RNG_DEFAULT );


  // Timing Wheel Event Management
  event_t** timing_wheel;
  int    wheel_seconds, wheel_size, wheel_mask, timing_slice;
  double wheel_granularity;

  // Raid Events
  std::vector<raid_event_t*> raid_events;
  std::string raid_events_str;
  std::string fight_style;

  // Buffs and Debuffs Overrides
  struct overrides_t
  {
    int bleeding;
    int coordination;// 5% crit. implement as buff, also add equal republic buffs
    int force_valor; // 5% stat bonus. implement sith version
    int shatter_shot; // -20% target armor. implement as buff, also add equal buffs
    int unnatural_might;// 5% damage bonus. implement as buff, also add equal republic buffs
    int fortification_hunters_boon; // 5% endurance
  };
  overrides_t overrides;

  // Auras
  struct auras_t
  {

  };
  auras_t auras;

  // Auras and De-Buffs
  buff_t* buff_list;
  timespan_t aura_delay;

  // Global aura related delay
  timespan_t default_aura_delay;
  timespan_t default_aura_delay_stddev;

  cooldown_t* cooldown_list;

  // Replenishment
  int replenishment_targets;
  std::vector<player_t*> replenishment_candidates;

  // Reporting
  report_t*  report;
  scaling_t* scaling;
  plot_t*    plot;
  reforge_plot_t* reforge_plot;
  timespan_t elapsed_cpu;
  double     iteration_dmg, iteration_heal;
  sample_data_t raid_dps, total_dmg, raid_hps, total_heal, simulation_length;
  int        report_progress;
  std::string reference_player_str;
  std::vector<player_t*> players_by_dps;
  std::vector<player_t*> players_by_hps;
  std::vector<player_t*> players_by_name;
  std::vector<player_t*> targets_by_name;
  std::vector<std::string> id_dictionary;
  std::vector<std::string> dps_charts, hps_charts, gear_charts, dpet_charts;
  std::string downtime_chart;
  std::vector<timespan_t> iteration_timeline;
  std::vector<int> divisor_timeline;
  std::string timeline_chart;
  std::string output_file_str, html_file_str;
  std::string xml_file_str, xml_stylesheet_file_str;
  std::string path_str;
  std::stack<std::string> active_files;
  std::vector<std::string> error_list;
  FILE* output_file;
  int armory_throttle;
  int current_throttle;
  int debug_exp;
  int report_precision;
  int report_pets_separately;
  int report_targets;
  int report_details;
  int report_rng;
  int hosted_html;
  int print_styles;
  int report_overheal;
  int save_raid_summary;
  int statistics_level;
  int separate_stats_by_actions;

  // Multi-Threading
  int threads;
  std::vector<sim_t*> children;
  int thread_index;
  virtual void run() { iterate(); }

  sim_t( sim_t* parent=0, int thrdID=0 );
  virtual ~sim_t();

  int       main( int argc, char** argv );
  void      cancel();
  double    progress( std::string& phase );
  void      combat( int iteration );
  void      combat_begin();
  void      combat_end();
  void      add_event( event_t*, timespan_t delta_time );
  void      reschedule_event( event_t* );
  void      flush_events();
  void      cancel_events( player_t* );
  event_t*  next_event();
  void      reset();
  bool      init();
  void      analyze_player( player_t* p );
  void      analyze();
  void      merge( sim_t& other_sim );
  void      merge();
  bool      iterate();
  void      partition();
  bool      execute();
  void      print_options();
  void      create_options();
  bool      parse_option( const std::string& name, const std::string& value );
  bool      parse_options( int argc, char** argv );
  bool      time_to_think( timespan_t proc_time );
  timespan_t total_reaction_time ();
  double    iteration_adjust();
  player_t* find_player( const std::string& name );
  player_t* find_player( int index );
  cooldown_t* get_cooldown( const std::string& name );
  void      use_optimal_buffs_and_debuffs( int value );
  void      aura_gain( const char* name, int aura_id=0 );
  void      aura_loss( const char* name, int aura_id=0 );
  action_expr_t* create_expression( action_t*, const std::string& name );
  int       errorf( const char* format, ... ) PRINTF_ATTRIBUTE( 2,3 );
};

// Scaling ==================================================================

struct scaling_t
{
  sim_t* sim;
  sim_t* baseline_sim;
  sim_t* ref_sim;
  sim_t* delta_sim;
  sim_t* ref_sim2;
  sim_t* delta_sim2;
  int    scale_stat;
  double scale_value;
  double scale_delta_multiplier;
  int    calculate_scale_factors;
  int    center_scale_delta;
  int    positive_scale_delta;
  int    scale_lag;
  double scale_factor_noise;
  int    normalize_scale_factors;
  int    smooth_scale_factors;
  int    debug_scale_factors;
  std::string scale_only_str;
  int    current_scaling_stat, num_scaling_stats, remaining_scaling_stats;
  double    scale_alacrity_iterations, scale_expertise_iterations, scale_crit_iterations, scale_accuracy_iterations;
  std::string scale_over;
  std::string scale_over_player;

  // Gear delta for determining scale factors
  gear_stats_t stats;

  scaling_t( sim_t* s );

  void init_deltas();
  void analyze();
  void analyze_stats();
  void analyze_ability_stats( int, double, player_t*, player_t*, player_t* );
  void analyze_lag();
  void analyze_gear_weights();
  void normalize();
  void derive();
  double progress( std::string& phase );
  void create_options();
  bool has_scale_factors();
  double scale_over_function( sim_t* s, player_t* p );
  double scale_over_function_error( sim_t* s, player_t* p );
};

// Plot =====================================================================

struct plot_t
{
  sim_t* sim;
  std::string dps_plot_stat_str;
  double dps_plot_step;
  int    dps_plot_points;
  int    dps_plot_iterations;
  int    dps_plot_debug;
  int    current_plot_stat, num_plot_stats, remaining_plot_stats;
  int    remaining_plot_points;
  bool	 dps_plot_positive;

  plot_t( sim_t* s );

  void analyze();
  void analyze_stats();
  double progress( std::string& phase );
  void create_options();
};

// Reforge Plot =============================================================

struct reforge_plot_t
{
  sim_t* sim;
  sim_t* current_reforge_sim;
  std::string reforge_plot_stat_str;
  std::string reforge_plot_output_file_str;
  FILE* reforge_plot_output_file;
  std::vector<int> reforge_plot_stat_indices;
  int    reforge_plot_step;
  int    reforge_plot_amount;
  int    reforge_plot_iterations;
  int    reforge_plot_debug;
  int    current_stat_combo;
  int    num_stat_combos;

  reforge_plot_t( sim_t* s );

  void generate_stat_mods( std::vector<std::vector<int> > &stat_mods,
                           const std::vector<int> &stat_indices,
                           int cur_mod_stat,
                           std::vector<int> cur_stat_mods );
  void analyze();
  void analyze_stats();
  double progress( std::string& phase );
  void create_options();
};

struct reforge_plot_data_t
{
  double value;
  double error;
};

// Event ====================================================================

struct event_t : public noncopyable
{
private:
  static void cancel_( event_t* e );
  static void early_( event_t* e );

  static void* operator new( std::size_t ) throw(); // DO NOT USE!

public:
  event_t*  next;
  sim_t*    sim;
  player_t* player;
  uint32_t  id;
  timespan_t time;
  timespan_t reschedule_time;
  int       canceled;
  const char* name;
  event_t( sim_t* s, player_t* p=0, const char* n="" ) :
    next( 0 ), sim( s ), player( p ), time( timespan_t::zero ), reschedule_time( timespan_t::zero ), canceled( 0 ), name( n )
  {
    if ( ! name ) name = "unknown";
  }
  timespan_t occurs()  const { return ( reschedule_time != timespan_t::zero ) ? reschedule_time : time; }
  timespan_t remains() const { return occurs() - sim -> current_time; }
  virtual void reschedule( timespan_t new_time );
  virtual void execute() = 0;
  virtual ~event_t() {}

  // T must be implicitly convertible to event_t* --
  // basically, a pointer to a type derived from event_t.
  template <typename T> static void cancel( T& e )
  { if ( e ) { cancel_( e ); e = 0; } }
  template <typename T> static void early( T& e )
  { if ( e ) { early_( e ); e = 0; } }

  // Simple free-list memory manager.
  static void* operator new( std::size_t size, sim_t* sim )
  { return sim -> free_list.allocate( size ); }

  static void operator delete( void* p )
  {
    event_t* e = static_cast<event_t*>( p );
    e -> sim -> free_list.deallocate( e );
  }

  static void operator delete( void* p, sim_t* sim )
  { sim -> free_list.deallocate( p ); }
};

// Gear Rating Conversions ==================================================
struct rating_t
{
private:
  static double swtor_diminishing_return( double cap, double divisor, int level, double rating )
  {
    assert( rating >= 0.0 );
    return cap * ( 1.0 - std::exp( ( std::log( 1.0 - ( 0.01 / cap ) ) / divisor ) *
                                   rating / std::max( 20, level ) ) );
  }

  template <int cap, int divisor>
  class scaling_t
  {
    // Performs the swtor_diminishing_return function in 2 steps. The first step (init)
    // precomputes as much as possible given that cap, divisor, and level are static
    // for a full simulation run. The second step (operator()) completes the computation
    // for a given rating.
  private:
    double multiplier;
    mutable double cached_rating, cached_value;
  public:
    void init( int level )
    {
      multiplier = std::log( 1.0 - ( 1.0 / cap ) ) / ( divisor / 100.0 ) / std::max( 20, level );
      cached_rating = cached_value = -1;
    }

    double operator() ( double rating ) const
    {
      if ( likely( cached_rating == rating ) )
        return cached_value;

      cached_rating = rating;
      return cached_value = ( cap / 100.0 ) * ( 1.0 - std::exp( multiplier * rating ) );
    }
  };

public:
  scaling_t<50,  18> absorb;
  scaling_t<30,  55> accuracy;
  scaling_t<30,  55> alacrity;
  scaling_t<30,  45> crit;
  scaling_t<30, 250> crit_from_stat;
  scaling_t<30,  55> defense;
  scaling_t<50,  32> shield;
  scaling_t<30,  11> surge;

  void init( int level )
  {
    absorb.init( level );
    accuracy.init( level );
    alacrity.init( level );
    crit.init( level );
    crit_from_stat.init( level );
    defense.init( level );
    shield.init( level );
    surge.init( level );
  }

  static int armor_divisor( int attacker_level )
  { return 200 * attacker_level + 800; }

  static double base_accuracy_chance( int attacker_level, int defender_level )
  {
    ( void )attacker_level; ( void )defender_level;
    return 1.0;
  }

  static double standardhealth_damage( int level );
  static double standardhealth_healing( int level );
  static int get_base_health( int level );
};

// Weapon ===================================================================

struct weapon_t
{
  int    type;
  school_type school;
  double damage;
  double min_dmg, max_dmg;

  int    slot;
  int    buff_type;
  double buff_value;
  double bonus_dmg;

  int    group() const;

  weapon_t( int t=WEAPON_NONE, double d=0, school_type s=SCHOOL_KINETIC ) :
    type( t ), school( s ), damage( d ), min_dmg( d ), max_dmg( d ),
    slot( SLOT_NONE ), buff_type( 0 ), buff_value( 0 ), bonus_dmg( 0 )
  {}
};

// Item =====================================================================

struct item_t
{
  sim_t* sim;
  player_t* player;
  int slot, quality, ilevel;
  bool unique, unique_enchant, unique_addon, is_heroic, is_lfr, is_ptr, is_matching_type, is_reforged;
  stat_type reforged_from;
  stat_type reforged_to;

  // Option Data
  std::string option_name_str;
  std::string option_id_str;
  std::string option_stats_str;
  std::string option_enchant_str;
  std::string option_addon_str;
  std::string option_equip_str;
  std::string option_use_str;
  std::string option_weapon_str;
  std::string option_heroic_str;
  std::string option_lfr_str;
  std::string option_armor_type_str;
  std::string option_reforge_str;
  std::string option_random_suffix_str;
  std::string option_ilevel_str;
  std::string option_quality_str;
  std::string option_data_source_str;
  std::string options_str;

  // Armory Data
  std::string armory_name_str;
  std::string armory_id_str;
  std::string armory_stats_str;
  std::string armory_enchant_str;
  std::string armory_addon_str;
  std::string armory_weapon_str;
  std::string armory_heroic_str;
  std::string armory_lfr_str;
  std::string armory_armor_type_str;
  std::string armory_reforge_str;
  std::string armory_ilevel_str;
  std::string armory_quality_str;
  std::string armory_random_suffix_str;

  // Encoded Data
  std::string id_str;
  std::string encoded_name_str;
  std::string encoded_stats_str;
  std::string encoded_enchant_str;
  std::string encoded_addon_str;
  std::string encoded_equip_str;
  std::string encoded_use_str;
  std::string encoded_weapon_str;
  std::string encoded_heroic_str;
  std::string encoded_lfr_str;
  std::string encoded_armor_type_str;
  std::string encoded_reforge_str;
  std::string encoded_ilevel_str;
  std::string encoded_quality_str;
  std::string encoded_random_suffix_str;

  // Extracted data
  gear_stats_t base_stats,stats;
  struct special_effect_t
  {
    std::string name_str, trigger_str;
    int trigger_type;
    int64_t trigger_mask;
    int stat;
    school_type school;
    int max_stacks;
    double stat_amount, discharge_amount, discharge_scaling;
    double proc_chance;
    timespan_t duration, cooldown, tick;
    bool cost_reduction;
    bool no_crit;
    bool no_player_benefits;
    bool no_debuffs;
    bool no_refresh;
    bool chance_to_discharge;
    bool reverse;
    special_effect_t() :
      trigger_type( 0 ), trigger_mask( 0 ), stat( 0 ), school( SCHOOL_NONE ),
      max_stacks( 0 ), stat_amount( 0 ), discharge_amount( 0 ), discharge_scaling( 0 ),
      proc_chance( 0 ), duration( timespan_t::zero ), cooldown( timespan_t::zero ),
      tick( timespan_t::zero ), cost_reduction( false ), no_crit( false ), no_player_benefits( false ), no_debuffs( false ),
      no_refresh( false ), chance_to_discharge( false ), reverse( false ) {}
    bool active() { return stat || school; }
  } use, equip, enchant, addon;

  item_t() : sim( 0 ), player( 0 ), slot( SLOT_NONE ), quality( 0 ), ilevel( 0 ), unique( false ), unique_enchant( false ),
    unique_addon( false ), is_heroic( false ), is_lfr( false ), is_ptr( false ), is_matching_type( false ), is_reforged( false ) {}
  item_t( player_t*, const std::string& options_str );
  bool active() const;
  bool heroic() const;
  bool lfr() const;
  bool ptr() const;
  bool reforged() const;
  bool matching_type();
  const char* name() const;
  const char* slot_name() const;
  const char* armor_type();
  weapon_t* weapon() const;
  bool init();
  bool parse_options();
  void encode_options();
  bool decode_stats();
  bool decode_enchant();
  bool decode_addon();
  bool decode_special( special_effect_t&, const std::string& encoding );
  bool decode_weapon();
  bool decode_heroic();
  bool decode_lfr();
  bool decode_armor_type();
  bool decode_reforge();
  bool decode_ilevel();
  bool decode_quality();

  static bool download_slot( item_t& item,
                             const std::string& item_id,
                             const std::string& enchant_id,
                             const std::string& addon_id,
                             const std::string& reforge_id,
                             const std::string& rsuffix_id );
  static bool download_item( item_t&, const std::string& item_id );
};

// Set Bonus ================================================================

struct set_bonus_t
{
  bool has_2pc, has_4pc;
  set_bonus_t* next;
  std::string name;
  std::vector<std::string> filters;
  int count;
  slot_mask_t mask;
  bool force_enable_2pc, force_disable_2pc;
  bool force_enable_4pc, force_disable_4pc;

  set_bonus_t( const std::string& name, const std::string& filters=std::string(),
               slot_mask_t s_mask=DEFAULT_SET_BONUS_SLOT_MASK );

  void init( const player_t& );

  bool two_pc() const { return has_2pc; }
  bool four_pc() const { return has_4pc; }

  //action_expr_t* create_expression( action_t*, const std::string& type );

private:
  bool decode( const item_t& item ) const;
};

// Player ===================================================================

struct player_t : public noncopyable
{
  sim_t* const sim;
  std::string name_str, talents_str, id_str, target_str;
  std::string region_str, server_str, origin_str;
  player_t*   next;
  int         index;
  const player_type type;
  role_type   role;
  player_t*   target;
  int         level, use_pre_potion, party, member;
  double      skill, initial_skill, distance, default_distance;
  timespan_t  gcd_ready;
  timespan_t  base_gcd;
  int         potion_used, sleeping, initial_sleeping, initialized;
  pet_t*      pet_list;
  int         bugs;
  int         specialization;
  int         invert_scaling;
  bool        vengeance_enabled;
  double      vengeance_damage, vengeance_value, vengeance_max;
  bool        vengeance_was_attacked;
  int         active_pets;
  timespan_t  reaction_mean,reaction_stddev,reaction_nu;
  int         infinite_resource[ RESOURCE_MAX ];
  std::vector<buff_t*> absorb_buffs;
  int         scale_player;
  double      avg_ilvl;
  pet_t*      active_companion;
  bool        ptr;

  // Latency
  timespan_t  world_lag, world_lag_stddev;
  timespan_t  brain_lag, brain_lag_stddev;
  bool        world_lag_override, world_lag_stddev_override;

  int    events;

  // Option Parsing
  std::vector<option_t> options;

  // Talent Parsing
  int tree_type[ MAX_TALENT_TREES ];
  int talent_tab_points[ MAX_TALENT_TREES ];
  std::vector<talent_t*> talent_trees[ MAX_TALENT_TREES ];
  //std::vector<glyph_t*> glyphs;

  //std::list<spell_id_t*> spell_list;

  // Profs
  std::string professions_str;
  int profession[ PROFESSION_MAX ];

  // Race
  std::string race_str;
  race_type race;


  // Ratings
private:
  double initial_accuracy_rating, accuracy_rating_, base_accuracy_, computed_accuracy;
  double initial_alacrity_rating, alacrity_rating_, base_alacrity_, computed_alacrity;
  double initial_crit_rating, crit_rating;
  double initial_surge_rating, surge_rating;
  double initial_defense_rating, defense_rating;
  double initial_shield_rating, shield_rating;
  double initial_absorb_rating, absorb_rating;

  double crit_from_rating, defense_from_rating, shield_from_rating, absorb_from_rating;

  void recalculate_accuracy();
  void recalculate_alacrity();

protected:
  rating_t rating_scaler;

  void set_base_accuracy( double value )   { base_accuracy_ = value;   recalculate_accuracy(); }
  void set_accuracy_rating( double value ) { accuracy_rating_ = value; recalculate_accuracy(); }
  void set_base_alacrity( double value )   { base_alacrity_ = value;   recalculate_alacrity(); }
  void set_alacrity_rating( double value ) { alacrity_rating_ = value; recalculate_alacrity(); }

public:
  double get_base_accuracy() const { return base_accuracy_; }
  double get_accuracy_rating() const { return accuracy_rating_; }
  double get_base_alacrity() const { return base_alacrity_; }
  double get_alacrity_rating() const { return alacrity_rating_; }


public:
  // Attributes
  attribute_type primary_attribute, secondary_attribute;
  double attribute                   [ ATTRIBUTE_MAX ];
  double attribute_base              [ ATTRIBUTE_MAX ];
  double attribute_initial           [ ATTRIBUTE_MAX ];
  double attribute_multiplier        [ ATTRIBUTE_MAX ];
  double attribute_multiplier_initial[ ATTRIBUTE_MAX ];

  // Attack Mechanics
  double base_power,       initial_power,       power;
  double base_force_power, initial_force_power, force_power;
  double base_tech_power,  initial_tech_power,  tech_power;

  double surge_bonus;
  double base_armor_penetration;

  // Resource Regen
  double base_energy_regen_per_second;
  double base_ammo_regen_per_second;
  double base_force_regen_per_second;
  double resource_reduction[ SCHOOL_MAX ], initial_resource_reduction[ SCHOOL_MAX ];

  int    position;
  std::string position_str;

  // Defense Mechanics
  event_t* target_auto_attack;
  double base_armor,       initial_armor,       armor;
  double base_bonus_armor, initial_bonus_armor, bonus_armor;
  double armor_multiplier, initial_armor_multiplier;

  double base_shield_chance, initial_shield_chance;
  double base_shield_absorb, initial_shield_absorb;

  double base_melee_avoidance, initial_melee_avoidance;
  double base_range_avoidance, initial_range_avoidance;
  double base_force_avoidance, initial_force_avoidance;
  double base_tech_avoidance,  initial_tech_avoidance;

  double base_DR;

  // Weapons
  weapon_t main_hand_weapon;
  weapon_t off_hand_weapon;
  weapon_t ranged_weapon;

  // Resources
  double resource_base   [ RESOURCE_MAX ];
  double resource_initial[ RESOURCE_MAX ];
  double resource_max    [ RESOURCE_MAX ];
  double resource_current[ RESOURCE_MAX ];
  double health_per_endurance;
  uptime_t* primary_resource_cap;

  // Replenishment
  std::vector<player_t*> replenishment_targets;

  // Callbacks
  action_callback_t* dark_intent_cb;

  // Consumables
  int stim;

  // Events
  action_t* executing;
  action_t* channeling;
  event_t*  readying;
  event_t*  off_gcd;
  bool      in_combat;
  bool      action_queued;

  // Delay time used by "cast_delay" expression to determine when an action
  // can be used at minimum after a spell cast has finished, including GCD
  timespan_t cast_delay_reaction;
  timespan_t cast_delay_occurred;

  // Callbacks
  std::vector<action_callback_t*> all_callbacks;
  std::vector<action_callback_t*> attack_callbacks[ RESULT_MAX ];
  std::vector<action_callback_t*>  spell_callbacks[ RESULT_MAX ];
  std::vector<action_callback_t*>  heal_callbacks[ RESULT_MAX ];
  std::vector<action_callback_t*>   tick_callbacks[ RESULT_MAX ];
  std::vector<action_callback_t*> direct_damage_callbacks[ SCHOOL_MAX ];
  std::vector<action_callback_t*>   tick_damage_callbacks[ SCHOOL_MAX ];
  std::vector<action_callback_t*>   direct_heal_callbacks[ SCHOOL_MAX ];
  std::vector<action_callback_t*>     tick_heal_callbacks[ SCHOOL_MAX ];
  std::vector<action_callback_t*> resource_gain_callbacks[ RESOURCE_MAX ];
  std::vector<action_callback_t*> resource_loss_callbacks[ RESOURCE_MAX ];

  // Action Priority List
  action_t*   action_list;
  std::vector<action_t*> off_gcd_actions;
  std::string action_list_str;
  std::string choose_action_list;
  std::string action_list_skip;
  std::string modify_action;
  int         action_list_default;
  cooldown_t* cooldown_list;
  dot_t*      dot_list;
  std::unordered_map<std::string,int> action_map;
  std::vector<action_priority_list_t*> action_priority_list;

  // Reporting
  int       quiet;
  action_t* last_foreground_action;
  timespan_t iteration_fight_length,arise_time;
  sample_data_t fight_length, waiting_time, executed_foreground_actions;
  timespan_t iteration_waiting_time;
  int       iteration_executed_foreground_actions;
  double    resource_lost  [ RESOURCE_MAX ];
  double    resource_gained[ RESOURCE_MAX ];
  double    rps_gain, rps_loss;
  sample_data_t deaths;
  double    deaths_error;

  // Buffed snapshot_stats (for reporting)
  struct buffed_stats_t
  {
    double attribute[ ATTRIBUTE_MAX ];
    double resource[ RESOURCE_MAX ];
    double alacrity;
    double surge;

    double power, force_power, tech_power;

    double shield_chance, shield_absorb;

    double melee_accuracy, range_accuracy, force_accuracy, tech_accuracy;
    double melee_crit, range_crit, force_crit, tech_crit;
    double melee_avoidance, range_avoidance, force_avoidance, tech_avoidance;
    double melee_damage_bonus, range_damage_bonus, force_damage_bonus, tech_damage_bonus, force_healing_bonus, tech_healing_bonus;

    double armor, armor_penetration_debuff;
  } buffed;

  buff_t*   buff_list;
  proc_t*   proc_list;
  gain_t*   gain_list;
  stats_t*  stats_list;
  benefit_t* benefit_list;
  uptime_t* uptime_list;
  set_bonus_t* set_bonus_list;
  std::vector<double> dps_plot_data[ STAT_MAX ];
  std::vector<std::vector<reforge_plot_data_t> > reforge_plot_data;
  std::vector<std::vector<double> > timeline_resource;

  // Damage
  double iteration_dmg, iteration_dmg_taken;
  double dps_error, dpr, dtps_error;
  sample_data_t dmg;
  sample_data_t compound_dmg;
  sample_data_t dps;
  sample_data_t dpse;
  sample_data_t dtps;
  sample_data_t dmg_taken;
  std::vector<double> timeline_dmg;
  std::vector<double> timeline_dps;

  // Heal
  double iteration_heal,iteration_heal_taken;
  double hps_error,hpr;
  sample_data_t heal;
  sample_data_t compound_heal;
  sample_data_t hps;
  sample_data_t hpse;
  sample_data_t htps;
  sample_data_t heal_taken;

  std::string action_sequence;
  std::string action_dpet_chart, action_dmg_chart, time_spent_chart, gains_chart;
  std::vector<std::string> timeline_resource_chart;
  std::string timeline_dps_chart, timeline_resource_health_chart;
  std::string distribution_dps_chart, scaling_dps_chart, scale_factors_chart;
  std::string reforge_dps_chart, dps_error_chart, distribution_deaths_chart;
  std::string gear_weights_lootrank_link, gear_weights_wowhead_link, gear_weights_wowreforge_link;
  std::string gear_weights_pawn_std_string, gear_weights_pawn_alt_string;
  std::string save_str;
  std::string save_gear_str;
  std::string save_talents_str;
  std::string save_actions_str;
  std::string save_json_str;
  std::string comment_str;
  std::string thumbnail_url;

  // Gear
  std::string items_str;
  std::vector<item_t> items;
  gear_stats_t stats, initial_stats, gear, enchant, temporary;

  // Scale Factors
  gear_stats_t scaling;
  gear_stats_t scaling_normalized;
  gear_stats_t scaling_error;
  gear_stats_t scaling_delta_dps;
  gear_stats_t scaling_compare_error;
  double       scaling_lag, scaling_lag_error;
  bool         scales_with[ STAT_MAX ];
  double       over_cap[ STAT_MAX ];
  std::vector<int> scaling_stats; // sorting vector

  // Movement & Position
  double base_movement_speed;
  double x_position, y_position;

  struct buffs_t
  {
    buff_t* coordination;
    buff_t* force_valor;
    buff_t* fortification_hunters_boon;
    buff_t* power_potion;
    buff_t* raid_movement;
    buff_t* self_movement;
    buff_t* stunned;
    buff_t* unnatural_might;
  };
  buffs_t buffs;

  struct debuffs_t
  {
    debuff_t* bleeding;
    debuff_t* flying;
    debuff_t* invulnerable;
    debuff_t* shatter_shot;
    debuff_t* vulnerable;

    bool snared();
  };
  debuffs_t debuffs;

  struct gains_t
  {
    gain_t* energy_regen;
    gain_t* ammo_regen;
    gain_t* force_regen;
    void reset() { *this = gains_t(); }
  };
  gains_t gains;

  rng_t* rng_list;

  struct rngs_t
  {
    rng_t* lag_channel;
    rng_t* lag_gcd;
    rng_t* lag_queue;
    rng_t* lag_ability;
    rng_t* lag_reaction;
    rng_t* lag_world;
    rng_t* lag_brain;
    void reset() { *this = rngs_t(); }
  };
  rngs_t rngs;

  struct set_bonuses_t
  {
    set_bonus_t* rakata_force_masters;
    set_bonus_t* battlemaster_force_masters;
    set_bonus_t* rakata_stalkers;
    void reset() { *this = set_bonuses_t(); }
  };
  set_bonuses_t set_bonus;

  const size_t targetdata_id;
private:
  std::vector<targetdata_t*> targetdata;
  std::vector<targetdata_t*> sourcedata;

public:
  player_t( sim_t* sim, player_type type, const std::string& name, race_type race_type = RACE_NONE );

  virtual ~player_t();

  virtual const char* name() const { return name_str.c_str(); }

  virtual targetdata_t* new_targetdata( player_t& target );
  targetdata_t* get_targetdata( player_t* target );
  targetdata_t* get_sourcedata( player_t* source );

  virtual void init();
  virtual void init_base() = 0;
  virtual void init_items();
  virtual void init_core();
  virtual void init_position();
  virtual void init_race();
  virtual void init_racials();
  virtual void init_spell();
  virtual void init_attack();
  virtual void init_defense();
  virtual void init_weapon( weapon_t* );
  virtual void init_unique_gear();
  virtual void init_enchant();
  virtual void init_resources( bool force = false );
  virtual void init_professions();
  virtual void init_professions_bonus();
  virtual std::string init_use_item_actions( const std::string& append = std::string() );
  virtual std::string init_use_profession_actions( const std::string& append = std::string() );
  virtual std::string init_use_racial_actions( const std::string& append = std::string() );
  virtual void init_actions();
  virtual void init_scaling();
  virtual void init_talents();
  virtual void init_spells();
  virtual void init_buffs();
  virtual void init_gains();
  virtual void init_procs();
  virtual void init_uptimes();
  virtual void init_benefits();
  virtual void init_rng();
  virtual void init_stats();
  virtual void init_values();
  virtual void init_target();

  virtual void reset();
  virtual void combat_begin();
  virtual void combat_end();
  virtual void merge( player_t& other );

  virtual double energy_regen_per_second() const;
  virtual double ammo_regen_per_second() const;
  virtual double force_regen_per_second() const;

  virtual double composite_power() const;
  virtual double composite_force_power() const;
  virtual double composite_tech_power() const;

  virtual double composite_attribute_multiplier( int attr ) const;

  virtual double composite_player_multiplier( const school_type school, action_t* a = NULL ) const;
  virtual double composite_player_dd_multiplier( const school_type /* school */, action_t* /* a */ = NULL ) const { return 1; }
  virtual double composite_player_td_multiplier( const school_type school, action_t* a = NULL ) const;

  virtual double composite_player_heal_multiplier( const school_type school ) const;
  virtual double composite_player_dh_multiplier( const school_type /* school */ ) const { return 1; }
  virtual double composite_player_th_multiplier( const school_type school ) const;

  virtual double composite_player_absorb_multiplier( const school_type school ) const;

  virtual double composite_movement_speed() const;

private:
  double damage_bonus( double stats, double multiplier, double extra_power=0 ) const;
  double healing_bonus( double stats, double multiplier, double extra_power=0 ) const;
  double default_bonus_multiplier() const;
  double default_crit_chance() const;
  double default_accuracy_chance() const { return computed_accuracy; }

protected:

  virtual double melee_bonus_stats() const;
  virtual double melee_bonus_multiplier() const;
  virtual double melee_crit_from_stats() const;

  virtual double range_bonus_stats() const;
  virtual double range_bonus_multiplier() const;
  virtual double range_crit_from_stats() const;

  virtual double force_bonus_stats() const;
  virtual double force_bonus_multiplier() const;
  virtual double force_crit_from_stats() const;

  virtual double tech_bonus_stats() const;
  virtual double tech_bonus_multiplier() const;
  virtual double tech_crit_from_stats() const;

  virtual double force_healing_bonus_stats() const;
  virtual double force_healing_bonus_multiplier() const;
  virtual double force_healing_crit_from_stats() const;

  virtual double tech_healing_bonus_stats() const;
  virtual double tech_healing_bonus_multiplier() const;
  virtual double tech_healing_crit_from_stats() const;

public:
  double melee_damage_bonus() const;
  virtual double melee_accuracy_chance() const;
  virtual double melee_crit_chance() const;

  double range_damage_bonus() const;
  virtual double range_accuracy_chance() const;
  virtual double range_crit_chance() const;

  double force_damage_bonus() const;
  virtual double force_accuracy_chance() const;
  virtual double force_crit_chance() const;

  double force_healing_bonus() const;
  virtual double force_healing_crit_chance() const;

  double tech_damage_bonus() const;
  virtual double tech_accuracy_chance() const;
  virtual double tech_crit_chance() const;

  double tech_healing_bonus() const;
  virtual double tech_healing_crit_chance() const;

  virtual double armor_penetration() const;

  double get_attribute( attribute_type a ) const;
  double strength() const { return get_attribute( ATTR_STRENGTH ); }
  double aim() const { return get_attribute( ATTR_AIM ); }
  double cunning() const { return get_attribute( ATTR_CUNNING ); }
  double willpower() const { return get_attribute( ATTR_WILLPOWER ); }
  double endurance() const { return get_attribute( ATTR_ENDURANCE ); }
  double presence() const { return get_attribute( ATTR_PRESENCE ); }

  virtual double alacrity() const;

  virtual double composite_armor() const;
  virtual double composite_armor_multiplier() const;
  virtual double armor_penetration_debuff() const;

  virtual double shield_chance() const;
  virtual double shield_absorb() const;

  virtual double melee_avoidance() const;
  virtual double range_avoidance() const;
  virtual double force_avoidance() const;
  virtual double tech_avoidance() const;

  virtual double kinetic_damage_reduction() const;
  virtual double energy_damage_reduction() const;
  virtual double internal_damage_reduction() const;
  virtual double elemental_damage_reduction() const;
  virtual double school_damage_reduction( school_type ) const;

protected:
  void recalculate_crit_from_rating();
  void recalculate_surge_from_rating();
  void recalculate_defense_from_rating();
  void recalculate_shield_from_rating();
  void recalculate_absorb_from_rating();
public:

  virtual void      interrupt();
  virtual void      halt();
  virtual void      moving();
  virtual void      stun();
  virtual void      clear_debuffs();
  virtual void      schedule_ready( timespan_t delta_time=timespan_t::zero, bool waiting=false );
  virtual void      arise();
  virtual void      demise();
  virtual timespan_t available() const { return timespan_t::from_seconds( 0.1 ); }
  virtual action_t* execute_action();

  std::string print_action_map( int iterations, int precision ) const;

  virtual void   regen( timespan_t periodicity=timespan_t::from_seconds( 0.25 ) );
          double resource_gain( int resource, double amount, gain_t* g=0, action_t* a=0 );
          double resource_loss( int resource, double amount, gain_t* g=0, action_t* a=0 );
  virtual void   recalculate_resource_max( int resource );
  virtual bool   resource_available( int resource, double cost ) const;
  virtual int    primary_resource() const { return RESOURCE_NONE; }
  virtual int    primary_role() const;
  virtual int    primary_tree() const;
  int            primary_tab() const;
  const char*    primary_tree_name() const;
  virtual int    normalize_by() const;
  virtual bool   report_attack_type( attack_policy_t ) { return false; }

  virtual double health_percentage() const;
  virtual timespan_t time_to_die() const;
  virtual timespan_t total_reaction_time() const;

  virtual void stat_gain( int stat, double amount, gain_t* g=0, action_t* a=0, bool temporary=false );
  virtual void stat_loss( int stat, double amount, action_t* a=0, bool temporary=false );

  virtual void cost_reduction_gain( int school, double amount, gain_t* g=0, action_t* a=0 );
  virtual void cost_reduction_loss( int school, double amount, action_t* a=0 );

  virtual double assess_damage( double amount, const school_type school, int type, int result, action_t* a );
  virtual double target_mitigation( double amount, const school_type school, int type, int result, action_t* a );

  struct heal_info_t { double actual, amount; };
  virtual heal_info_t assess_heal( double amount, const school_type school, int type, int result, action_t* a );

  virtual void  summon_pet( const char* name, timespan_t duration=timespan_t::zero );
  virtual void dismiss_pet( const char* name );

  virtual bool ooc_buffs() { return true; }

  virtual bool is_moving() { return buffs.raid_movement -> check() || buffs.self_movement -> check(); }

  virtual void register_callbacks();
  virtual void register_resource_gain_callback( int resource,        action_callback_t* );
  virtual void register_resource_loss_callback( int resource,        action_callback_t* );
  virtual void register_attack_callback       ( int64_t result_mask, action_callback_t* );
  virtual void register_spell_callback        ( int64_t result_mask, action_callback_t* );
  virtual void register_tick_callback         ( int64_t result_mask, action_callback_t* );
  virtual void register_heal_callback         ( int64_t result_mask, action_callback_t* );
  virtual void register_harmful_spell_callback( int64_t result_mask, action_callback_t* );
  virtual void register_tick_damage_callback  ( int64_t result_mask, action_callback_t* );
  virtual void register_direct_damage_callback( int64_t result_mask, action_callback_t* );
  virtual void register_tick_heal_callback    ( int64_t result_mask, action_callback_t* );
  virtual void register_direct_heal_callback  ( int64_t result_mask, action_callback_t* );

  bool parse_talent_trees( const int talents[MAX_TALENT_SLOTS] );
  bool parse_talents_armory ( const std::string& talent_string );
  bool parse_talents_wowhead( const std::string& talent_string );

  virtual void create_talents();
  //virtual void create_glyphs();

  talent_t* find_talent( const std::string& name, int tree = TALENT_TAB_NONE ) const;
  //virtual glyph_t*  find_glyph ( const std::string& name );

  virtual action_expr_t* create_expression( action_t*, const std::string& name );

  virtual void create_options();
  virtual bool create_profile( std::string& profile_str, int save_type=SAVE_ALL, bool save_html=false );
  virtual bool create_json_profile( std::string&, int =SAVE_ALL, bool =false );

  virtual void copy_from( const player_t& source );

  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual void      create_pets() { }
  virtual pet_t*    create_pet( const std::string& /* name*/,  const std::string& /* type */ = std::string() ) { return 0; }
  virtual pet_t*    find_pet  ( const std::string& name );

  virtual void trigger_replenishment();

  virtual void armory_extensions( const std::string& /* region */, const std::string& /* server */, const std::string& /* character */,
                                  cache::behavior_t /* behavior */=cache::players() )
  {}

  // Class-Specific Methods

  static player_t* create( sim_t* sim, const std::string& type, const std::string& name, race_type r = RACE_NONE );

  static player_t* create_commando( sim_t* sim, const std::string& name, race_type r = RACE_NONE );
  static player_t* create_mercenary( sim_t* sim, const std::string& name, race_type r = RACE_NONE );

  static player_t* create_gunslinger( sim_t* sim, const std::string& name, race_type r = RACE_NONE );
  static player_t* create_sniper( sim_t* sim, const std::string& name, race_type r = RACE_NONE );

  static player_t* create_sith_juggernaut( sim_t* sim, const std::string& name, race_type r = RACE_NONE );
  static player_t* create_jedi_guardian( sim_t* sim, const std::string& name, race_type r = RACE_NONE );

  static player_t* create_jedi_sage( sim_t* sim, const std::string& name, race_type r = RACE_NONE );
  static player_t* create_sith_sorcerer( sim_t* sim, const std::string& name, race_type r = RACE_NONE );

  static player_t* create_scoundrel( sim_t* sim, const std::string& name, race_type r = RACE_NONE );
  static player_t* create_operative( sim_t* sim, const std::string& name, race_type r = RACE_NONE );

  static player_t* create_sith_marauder( sim_t* sim, const std::string& name, race_type r = RACE_NONE );
  static player_t* create_jedi_sentinel( sim_t* sim, const std::string& name, race_type r = RACE_NONE );

  static player_t* create_jedi_shadow( sim_t* sim, const std::string& name, race_type r = RACE_NONE );
  static player_t* create_sith_assassin( sim_t* sim, const std::string& name, race_type r = RACE_NONE );

  static player_t* create_vanguard( sim_t* sim, const std::string& name, race_type r = RACE_NONE );
  static player_t* create_powertech( sim_t* sim, const std::string& name, race_type r = RACE_NONE );


  static player_t* create_enemy       ( sim_t* sim, const std::string& name, race_type r = RACE_NONE );

  // Raid-wide aura/buff/debuff maintenance
  static bool init        ( sim_t* sim );
  static void combat_begin( sim_t* sim );
  static void combat_end  ( sim_t* sim );

  // Raid-wide Commando/Mercenary buff maintenance
  static void commando_mercenary_init        ( sim_t* sim );
  static void commando_mercenary_combat_begin( sim_t* sim );
  static void commando_mercenary_combat_end  ( sim_t* /* sim */ ) {}

  // Raid-wide Gunslinger / Sniper buff maintenance
  static void gunslinger_sniper_init        ( sim_t* sim );
  static void gunslinger_sniper_combat_begin( sim_t* sim );
  static void gunslinger_sniper_combat_end  ( sim_t* /* sim */ ) {}

  // Raid-wide Juggernaut|Guardian buff maintenance
  static void juggernaut_guardian_init        ( sim_t* sim );
  static void juggernaut_guardian_combat_begin( sim_t* sim );
  static void juggernaut_guardian_combat_end  ( sim_t* /* sim */ ) {}

  // Raid-wide Sage / Sorcerer buff maintenance
  static void sage_sorcerer_init        ( sim_t* sim );
  static void sage_sorcerer_combat_begin( sim_t* sim );
  static void sage_sorcerer_combat_end  ( sim_t* /* sim */ ) {}

  // Raid-wide Scoundrel / Operative buff maintenance
  static void scoundrel_operative_init        ( sim_t* sim );
  static void scoundrel_operative_combat_begin( sim_t* sim );
  static void scoundrel_operative_combat_end  ( sim_t* /* sim */ ) {}

  // Raid-wide Sentinel / Marauder buff maintenance
  static void sentinel_marauder_init        ( sim_t* sim );
  static void sentinel_marauder_combat_begin( sim_t* sim );
  static void sentinel_marauder_combat_end  ( sim_t* /* sim */ ) {}

  // Raid-wide Shadow / Assassin buff maintenance
  static void shadow_assassin_init        ( sim_t* sim );
  static void shadow_assassin_combat_begin( sim_t* sim );
  static void shadow_assassin_combat_end  ( sim_t* /* sim */ ) {}

  // Raid-wide Vanguard / Powertech buff maintenance
  static void vanguard_powertech_init        ( sim_t* sim );
  static void vanguard_powertech_combat_begin( sim_t* sim );
  static void vanguard_powertech_combat_end  ( sim_t* /* sim */ ) {}

  // Raid-wide Enemy buff maintenance
  static void enemy_init        ( sim_t* sim );
  static void enemy_combat_begin( sim_t* sim );
  static void enemy_combat_end  ( sim_t* /* sim */ ) {}

  bool is_pet() const { return type == PLAYER_PET || type == PLAYER_GUARDIAN || type == ENEMY_ADD; }
  bool is_enemy() const { return type == ENEMY || type == ENEMY_ADD; }
  bool is_add() const { return type == ENEMY_ADD; }

  pet_t*                  cast_pet()                  { assert( is_pet() );                 return ( pet_t* )                 this; }
  enemy_t*                cast_enemy()                { assert( type == ENEMY );            return ( enemy_t*)                this; }

  bool      in_gcd() const { return gcd_ready > sim -> current_time; }
  item_t*   find_item( const std::string& );
  action_t* find_action( const std::string& );
  set_bonus_t* find_set_bonus( const std::string& name );
  bool      dual_wield() const { return main_hand_weapon.type != WEAPON_NONE && off_hand_weapon.type != WEAPON_NONE; }
  void      aura_gain( const char* name, double value=0 );
  void      aura_loss( const char* name, double value=0 );

  cooldown_t* find_cooldown( const std::string& name ) const;
  dot_t*      find_dot     ( const std::string& name ) const;
  action_priority_list_t* find_action_priority_list( const std::string& name ) const;
  stats_t*    find_stats   ( const std::string& name );

  cooldown_t* get_cooldown( const std::string& name );
  dot_t*      get_dot     ( const std::string& name );
  gain_t*     get_gain    ( const std::string& name );
  proc_t*     get_proc    ( const std::string& name );
  stats_t*    get_stats   ( const std::string& name, action_t* action=0 );
  benefit_t*  get_benefit ( const std::string& name );
  uptime_t*   get_uptime  ( const std::string& name );
  rng_t*      get_rng     ( const std::string& name, int type=RNG_DEFAULT );
  set_bonus_t* get_set_bonus( const std::string& name, std::string filter,
                              slot_mask_t slot_filter=DEFAULT_SET_BONUS_SLOT_MASK );
  double      get_player_distance( const player_t* p ) const;
  double      get_position_distance( double m=0, double v=0 ) const;
  action_priority_list_t* get_action_priority_list( const std::string& name );

  // Opportunity to perform any stat fixups before analysis
  virtual void pre_analyze_hook() {}
};

// Pet ======================================================================

struct pet_t : public player_t
{
  std::string full_name_str;
  player_t* owner;
  pet_t* next_pet;
  double endurance_per_owner;
  bool summoned;
  pet_type_t pet_type;
  event_t* expiration;

private:
  void init_pet_t_();
public:
  pet_t( sim_t* sim, player_t* owner, const std::string& name, bool guardian=false );
  pet_t( sim_t* sim, player_t* owner, const std::string& name, pet_type_t pt, player_type type=PLAYER_PET );


  virtual double endurance() const;

  virtual void init_base();
  virtual void init_talents();
  virtual void init_target();
  virtual void reset();
  virtual void summon( timespan_t duration=timespan_t::zero );
  virtual void dismiss();
  virtual bool ooc_buffs() { return false; }
  virtual double assess_damage( double amount, const school_type school, int type, int result, action_t* a=0 );
  virtual void combat_begin();

  virtual const char* name() const { return full_name_str.c_str(); }
  virtual const char* id();
};

// Companion

struct companion_t : pet_t
{
  companion_t( sim_t* sim, player_t* owner, const std::string& name, pet_type_t pt );

  virtual void summon( timespan_t duration=timespan_t::zero );
  virtual void dismiss();
};

// Stats ====================================================================

struct stats_t
{
  std::string name_str;
  sim_t* sim;
  player_t* player;
  stats_t* next;
  stats_t* parent;
  school_type school;
  stats_type type;
  std::vector<action_t*> action_list;
  bool analyzed;
  bool quiet;
  bool background;

  int resource;
  double resource_consumed, resource_portion;
  double frequency, num_executes, num_ticks;
  double num_direct_results, num_tick_results;
  timespan_t total_execute_time, total_tick_time, total_time;
  double portion_amount, overkill_pct;
  double aps, ape, apet, apr, rpe, etpe, ttpt;
  timespan_t total_intervals;
  double num_intervals;
  timespan_t last_execute;
  double iteration_actual_amount, iteration_total_amount;
  sample_data_t actual_amount, total_amount, portion_aps;
  std::string aps_distribution_chart;

  std::vector<stats_t*> children;
  double compound_actual,compound_amount;
  double opportunity_cost;

  gear_stats_t scaling;
  gear_stats_t scaling_error;

  struct stats_results_t
  {
    sample_data_t actual_amount, total_amount,fight_actual_amount, fight_total_amount,count,avg_actual_amount;
    int iteration_count;
    double iteration_actual_amount, iteration_total_amount,pct, overkill_pct;

    stats_results_t( sim_t* s );

    void merge( const stats_results_t& other );
    void combat_end();
  };
  std::vector<stats_results_t> direct_results;
  std::vector<stats_results_t>   tick_results;

  std::vector<double> timeline_amount;
  std::vector<double> timeline_aps;
  std::string timeline_aps_chart;

  stats_t( const std::string& name, player_t* );

  void add_child( stats_t* child );
  void consume_resource( double r ) { resource_consumed += r; }
  void add_result( double act_amount, double tot_amount, int dmg_type, int result );
  void add_tick   ( timespan_t time );
  void add_execute( timespan_t time );
  void combat_begin();
  void combat_end();
  void reset();
  void analyze();
  void merge( const stats_t* other );
};

// Action ===================================================================

class attack_policy_interface_t
{
protected:
  std::string name_;
public:
  const std::string& name() const { return name_; }

  virtual double accuracy_chance( const player_t& source ) const = 0;
  virtual double crit_chance( const player_t& source ) const = 0;
  virtual double damage_bonus( const player_t& source ) const = 0;

  virtual double avoidance( const player_t& target ) const = 0;
  virtual double shield_chance( const player_t& target ) const = 0;
  virtual double shield_absorb( const player_t& target ) const = 0;
};

struct action_t
{
  typedef attack_policy_t policy_t;
  static const policy_t default_policy;
  static const policy_t melee_policy;
  static const policy_t range_policy;
  static const policy_t force_policy;
  static const policy_t tech_policy;
  static const policy_t force_heal_policy;
  static const policy_t tech_heal_policy;

  sim_t* const sim;
  const int type;
  std::string name_str;
  player_t* const player;
  player_t* target;
  uint32_t id;
  policy_t attack_policy;
  school_type school;
  int resource, result, aoe;
  bool dual, callbacks, channeled, background, sequence, use_off_gcd;
  bool direct_tick, repeating, harmful, proc, item_proc, proc_ignores_slot, discharge_proc, auto_cast, initialized;
  bool may_crit, tick_may_crit, tick_zero, hasted_ticks;
  bool no_buffs, no_debuffs;
  int dot_behavior;
  timespan_t ability_lag, ability_lag_stddev;
  timespan_t min_gcd, trigger_gcd;
  double range;
  double weapon_power_mod;
  timespan_t base_execute_time;
  timespan_t base_tick_time;
  double base_cost;

  struct damage_factors_t {
    double standardhealthpercentmin, standardhealthpercentmax;
    double base_min, base_max;
    double base_multiplier;
    double player_multiplier;
    double target_multiplier;
    double power_mod;

    damage_factors_t() :
      standardhealthpercentmin( 0 ), standardhealthpercentmax( 0 ),
      base_min( 0 ), base_max( 0 ),
      base_multiplier( 1 ), player_multiplier( 1 ), target_multiplier( 1 ),
      power_mod( 0 )
    {}
  } dd, td;

  double   base_multiplier,   base_accuracy,   base_crit, base_armor_penetration;
  double player_multiplier, player_accuracy, player_crit, player_armor_penetration;
  double target_multiplier, target_accuracy, target_crit, target_armor_penetration;
  double target_avoidance, target_shield, target_absorb;
  double crit_multiplier, crit_bonus_multiplier, crit_bonus;
  double base_dd_adder, player_dd_adder, target_dd_adder;
  double player_alacrity;
  double resource_consumed;
  double direct_dmg, tick_dmg;
  int num_ticks;
  weapon_t* weapon;
  double weapon_multiplier;
  double base_add_multiplier;
  double base_aoe_multiplier; // Static reduction of damage for AoE
  bool normalize_weapon_speed;
  rng_t* rng[ RESULT_MAX ];
  rng_t* rng_travel;
  cooldown_t* cooldown;
  stats_t* stats;
  event_t* execute_event;
  event_t* travel_event;
  timespan_t time_to_execute, time_to_travel;
  double travel_speed;
  int bloodlust_active;
  double max_alacrity;
  double alacrity_gain_percentage;
  timespan_t min_current_time, max_current_time;
  double min_health_percentage, max_health_percentage;
  int moving, vulnerable, invulnerable, wait_on_ready, interrupt, not_flying, flying;
  bool round_base_dmg;

  std::string if_expr_str;
  std::unique_ptr<action_expr_t> if_expr;
  std::string interrupt_if_expr_str;
  std::unique_ptr<action_expr_t> interrupt_if_expr;
  std::string sync_str;
  action_t* sync_action;
  action_t* next;
  char marker;
  std::string signature_str;
  std::string target_str;
  std::string label_str;
  timespan_t last_reaction_time;

protected:
  std::vector<int> rank_level_list;
  int rank_level;

private:
  mutable player_t* cached_targetdata_target;
  mutable targetdata_t* cached_targetdata;
  mutable player_t* cached_dot_target;
  mutable dot_t* cached_dot;

  void init_action_t_();

public:
  action_t( int type, const char* name, player_t* p=0, policy_t policy=default_policy,
            int r=RESOURCE_NONE, const school_type s=SCHOOL_NONE );
  virtual ~action_t();

  //void parse_data();
  //void parse_effect_data( int spell_id, int effect_nr );
  virtual void   parse_options( option_t*, const std::string& options_str );
  virtual double cost() const;
  virtual double alacrity() const;
  virtual timespan_t gcd() const;
  virtual timespan_t execute_time() const;
  virtual timespan_t tick_time() const;
  virtual int    hasted_num_ticks( timespan_t d=timespan_t::min ) const;
  virtual timespan_t travel_time();
  virtual void   player_buff();
  virtual void   target_debuff( player_t* t, int dmg_type );
  virtual void   calculate_result();

  static  bool   result_is_hit ( int r );
          bool   result_is_hit () const { return action_t::result_is_hit( result ); }
  static  bool   result_is_miss( int r );
          bool   result_is_miss() const { return action_t::result_is_miss( result ); }

  virtual double calculate_direct_damage( int = 0 );
  virtual double calculate_tick_damage();
  virtual double calculate_weapon_damage();
  virtual double armor() const;
  virtual void   consume_resource();
  virtual void   execute();
  virtual void   tick( dot_t* d );
  virtual void   last_tick( dot_t* d );
  virtual void   impact( player_t*, int result, double dmg );
  virtual void   assess_damage( player_t* t, double amount, int dmg_type, int impact_result );
  virtual void   additional_damage( player_t* t, double amount, int dmg_type, int impact_result );
  virtual void   schedule_execute();
  virtual void   schedule_travel( player_t* t );
  virtual void   reschedule_execute( timespan_t time );
  virtual void   update_ready();
  virtual bool   usable_moving();
  virtual bool   ready();
  virtual void   init();
  virtual void   reset();
  virtual void   cancel();
  virtual void   interrupt_action();
  virtual void   check_talent( int talent_rank );
  virtual void   check_spec( int necessary_spec );
  virtual void   check_race( int race );
  virtual const char* name() const { return name_str.c_str(); }

  double total_multiplier() const { return base_multiplier * player_multiplier * target_multiplier; }
  double total_accuracy() const        { return base_accuracy   + player_accuracy;                       }
  double total_crit() const       { return base_crit       + player_crit       + target_crit;       }
  double total_crit_bonus() const;
  double total_armor_penetration() const
  { return base_armor_penetration + player_armor_penetration + target_armor_penetration; }

  virtual double total_power() const;

  // Some actions require different multipliers for the "direct" and "tick" portions.

  virtual double total_dd_multiplier() const { return total_multiplier() * dd.base_multiplier * dd.player_multiplier * dd.target_multiplier; }
  virtual double total_td_multiplier() const { return total_multiplier() * td.base_multiplier * td.player_multiplier * td.target_multiplier; }

  virtual action_expr_t* create_expression( const std::string& name );

  virtual double ppm_proc_chance( double PPM ) const;

  dot_t* dot() const
  {
    if ( cached_dot_target != target )
    {
      cached_dot = targetdata() -> get_dot( name_str );
      cached_dot_target = target;
    }

    if ( ! cached_dot )
    {
      sim -> errorf( "action_t::dot() action %s from player %s trying to access dot not available in targetdata().\n", name(), player -> name() );
      assert( 0 );
    }

    return cached_dot;
  }

  void add_child( action_t* child ) { stats -> add_child( child -> stats ); }

  targetdata_t* targetdata() const
  {
    if ( cached_targetdata_target != target )
    {
      cached_targetdata = player -> get_targetdata( target );
      cached_targetdata_target = target;
    }
    return cached_targetdata;
  }

  virtual std::vector< player_t* > available_targets() const;
  virtual std::vector< player_t* > target_list() const;
};

struct attack_t : public action_t
{
  attack_t( const char* n=0, player_t* p=0, int r=RESOURCE_NONE, school_type s=SCHOOL_KINETIC ) :
    action_t( ACTION_ATTACK, n, p, melee_policy, r, s )
  {}
};

// Heal =====================================================================

struct heal_t : public action_t
{
  bool group_only;

  heal_t( const char* n=0, player_t* p=0, policy_t policy=default_policy,
          int r=RESOURCE_NONE, school_type s=SCHOOL_NONE );

  virtual void player_buff();
  virtual void execute();
  virtual void assess_damage( player_t* t, double amount,
                              int dmg_type, int impact_result );

  player_t* find_greatest_difference_player();
  player_t* find_lowest_player();

  virtual std::vector<player_t*> available_targets() const;
};

// Absorb ===================================================================

struct absorb_t : public action_t
{
  absorb_t( const char* n=0, player_t* p=0, policy_t policy=default_policy,
            int r=RESOURCE_NONE, const school_type s=SCHOOL_NONE );

  virtual void player_buff();
  virtual void execute();
  virtual void assess_damage( player_t* t, double amount,
                              int dmg_type, int impact_result );
  virtual void calculate_result();
  virtual void impact( player_t*, int impact_result, double travel_dmg );
};

// Sequence =================================================================

struct sequence_t : public action_t
{
  std::vector<action_t*> sub_actions;
  int current_action;
  bool restarted;

  sequence_t( player_t*, const std::string& sub_action_str );

  virtual void schedule_execute();
  virtual void reset();
  virtual bool ready();
  virtual void restart() { current_action=0; restarted=true;}
};

// Cooldown =================================================================

struct cooldown_t
{
  sim_t* sim;
  player_t* player;
  std::string name_str;
  timespan_t duration;
  timespan_t ready;
  cooldown_t* next;

  cooldown_t( const std::string& n, player_t* p ) : sim( p->sim ), player( p ), name_str( n ), duration( timespan_t::zero ), ready( timespan_t::min ), next( 0 ) {}
  cooldown_t( const std::string& n, sim_t* s ) : sim( s ), player( 0 ), name_str( n ), duration( timespan_t::zero ), ready( timespan_t::min ), next( 0 ) {}

  void reset() { ready=timespan_t::min; }
  void start( timespan_t override=timespan_t::min, timespan_t delay=timespan_t::zero )
  {
    if ( override >= timespan_t::zero ) duration = override;
    if ( duration > timespan_t::zero ) ready = sim -> current_time + duration + delay;
  }
  timespan_t remains()
  {
    timespan_t diff = ready - sim -> current_time;
    if ( diff < timespan_t::zero ) diff = timespan_t::zero;
    return diff;
  }
  const char* name() { return name_str.c_str(); }
};

// DoT ======================================================================

struct dot_t
{
  sim_t* sim;
  player_t* player;
  action_t* action;
  event_t* tick_event;
  dot_t* next;
  int num_ticks, current_tick, added_ticks, ticking;
  timespan_t added_seconds;
  timespan_t ready;
  timespan_t miss_time;
  timespan_t time_to_tick;
  std::string name_str;

  dot_t() : player( 0 ) {}
  dot_t( const std::string& n, player_t* p );

  void   cancel();
  void   extend_duration( int extra_ticks, bool cap=false );
  void   extend_duration_seconds( timespan_t extra_seconds );
  void   recalculate_ready();
  void   refresh_duration();
  void   reset();
  timespan_t remains();
  void   schedule_tick();
  int    ticks();

  const char* name() { return name_str.c_str(); }

  action_expr_t* create_expression( action_t* action, const std::string& name );
};

// Action Callback ==========================================================

struct action_callback_t
{
  sim_t* sim;
  player_t* listener;
  bool active;
  bool allow_self_procs;
  bool allow_item_procs;
  bool allow_procs;

  action_callback_t( sim_t* s, player_t* l, bool ap=false, bool aip=false, bool asp=false ) :
    sim( s ), listener( l ), active( true ), allow_self_procs( asp ), allow_item_procs( aip ), allow_procs( ap )
  {
    if ( l )
    {
      l -> all_callbacks.push_back( this );
    }
  }
  virtual ~action_callback_t() {}
  virtual void trigger( action_t*, void* call_data=0 ) = 0;
  virtual void reset() {}
  virtual void activate() { active=true; }
  virtual void deactivate() { active=false; }

  static void trigger( std::vector<action_callback_t*>& v, action_t* a, void* call_data=0 )
  {
    if ( a && ! a -> player -> in_combat ) return;

    std::size_t size = v.size();
    for ( std::size_t i=0; i < size; i++ )
    {
      action_callback_t* cb = v[ i ];
      if ( cb -> active )
      {
        if ( ! cb -> allow_item_procs && a && a -> item_proc ) return;
        if ( ! cb -> allow_procs && a && a -> proc ) return;
        cb -> trigger( a, call_data );
      }
    }
  }

  static void reset( std::vector<action_callback_t*>& v )
  {
    std::size_t size = v.size();
    for ( std::size_t i=0; i < size; i++ )
    {
      v[ i ] -> reset();
    }
  }
};

// Action Priority List =====================================================

struct action_priority_list_t
{
  std::string name_str;
  std::string action_list_str;
  player_t* player;
  action_priority_list_t( std::string name, player_t* p ) : name_str( name ), player( p )
  {}
};

// Player Ready Event =======================================================

struct player_ready_event_t : public event_t
{
  player_ready_event_t( sim_t* sim, player_t* p, timespan_t delta_time );
  virtual void execute();
};

struct player_gcd_event_t : public event_t
{
  player_gcd_event_t( sim_t* sim, player_t* p, timespan_t delta_time );
  virtual void execute();
};

// Action Execute Event =====================================================

struct action_execute_event_t : public event_t
{
  action_t* action;
  action_execute_event_t( sim_t* sim, action_t* a, timespan_t time_to_execute );
  virtual void execute();
};

// DoT Tick Event ===========================================================

struct dot_tick_event_t : public event_t
{
  dot_t* dot;
  dot_tick_event_t( sim_t* sim, dot_t* d, timespan_t time_to_tick );
  virtual void execute();
};

// Action Travel Event ======================================================

struct action_travel_event_t : public event_t
{
  action_t* action;
  player_t* target;
  int result;
  double damage;
  action_travel_event_t( sim_t* sim, player_t* t, action_t* a, timespan_t time_to_travel );
  virtual void execute();
};

// Regen Event ==============================================================

struct regen_event_t : public event_t
{
  regen_event_t( sim_t* sim );
  virtual void execute();
};

// Unique Gear ==============================================================

struct unique_gear_t
{
  static void init( player_t* );

  static action_callback_t* register_stat_proc( int type, int64_t mask, const std::string& name, player_t*,
                                                int stat, int max_stacks, double amount,
                                                double proc_chance, timespan_t duration, timespan_t cooldown,
                                                timespan_t tick=timespan_t::zero, bool reverse=false, int rng_type=RNG_DEFAULT );

  static action_callback_t* register_cost_reduction_proc( int type, int64_t mask, const std::string& name, player_t*,
                                                          int school, int max_stacks, double amount,
                                                          double proc_chance, timespan_t duration, timespan_t cooldown,
                                                          bool refreshes=false, bool reverse=false, int rng_type=RNG_DEFAULT );

  static action_callback_t* register_discharge_proc( int type, int64_t mask, const std::string& name, player_t*,
                                                     int max_stacks, const school_type school, double amount, double scaling,
                                                     double proc_chance, timespan_t cooldown, bool no_crits, bool no_buffs, bool no_debuffs,
                                                     int rng_type=RNG_DEFAULT );

  static action_callback_t* register_chance_discharge_proc( int type, int64_t mask, const std::string& name, player_t*,
                                                            int max_stacks, const school_type school, double amount, double scaling,
                                                            double proc_chance, timespan_t cooldown, bool no_crits, bool no_buffs, bool no_debuffs,
                                                            int rng_type=RNG_DEFAULT );

  static action_callback_t* register_stat_discharge_proc( int type, int64_t mask, const std::string& name, player_t*,
                                                          int stat, int max_stacks, double stat_amount,
                                                          const school_type school, double discharge_amount, double discharge_scaling,
                                                          double proc_chance, timespan_t duration, timespan_t cooldown, bool no_crits, bool no_buffs,
                                                          bool no_debuffs );

  static action_callback_t* register_stat_proc( item_t&, item_t::special_effect_t& );
  static action_callback_t* register_cost_reduction_proc( item_t&, item_t::special_effect_t& );
  static action_callback_t* register_discharge_proc( item_t&, item_t::special_effect_t& );
  static action_callback_t* register_chance_discharge_proc( item_t&, item_t::special_effect_t& );
  static action_callback_t* register_stat_discharge_proc( item_t&, item_t::special_effect_t& );

  static bool get_equip_encoding( std::string& encoding,
                                  const std::string& item_name,
                                  const bool         item_heroic,
                                  const bool         item_lfr,
                                  const bool         ptr,
                                  const std::string& item_id=std::string() );

  static bool get_use_encoding  ( std::string& encoding,
                                  const std::string& item_name,
                                  const bool         heroic,
                                  const bool         lfr,
                                  const bool         ptr,
                                  const std::string& item_id=std::string() );
};

// Enchants =================================================================

struct enchant_t
{
  static void init( player_t* );
  static bool get_encoding        ( std::string& name, std::string& encoding, const std::string& enchant_id, const bool ptr );
  static bool get_addon_encoding  ( std::string& name, std::string& encoding, const std::string& addon_id, const bool ptr );
  static bool get_reforge_encoding( std::string& name, std::string& encoding, const std::string& reforge_id );
  static int  get_reforge_id      ( stat_type stat_from, stat_type stat_to );
  static bool download        ( item_t&, const std::string& enchant_id );
  static bool download_addon  ( item_t&, const std::string& addon_id   );
  static bool download_reforge( item_t&, const std::string& reforge_id );
  static bool download_rsuffix( item_t&, const std::string& rsuffix_id );
};

// Consumable ===============================================================

struct consumable_t
{
  static action_t* create_action( player_t*, const std::string& name, const std::string& options );
};

// Benefit ==================================================================

struct benefit_t : public noncopyable
{
  int up, down;

  double ratio;

  benefit_t* next;
  std::string name_str;

  explicit benefit_t( const std::string& n ) :
    up( 0 ), down( 0 ),
    ratio( 0.0 ), name_str( n ) {}

  void update( int is_up ) { if ( is_up ) up++; else down++; }

  const char* name() const { return name_str.c_str(); }

  void analyze()
  {
    if ( up != 0 )
      ratio = 1.0 * up / ( down + up );
  }

  void merge( const benefit_t* other )
  { up += other -> up; down += other -> down; }
};

// Uptime ==================================================================

struct uptime_common_t
{
  timespan_t last_start;
  timespan_t uptime_sum;
  sim_t* sim;

  double uptime;

  uptime_common_t( sim_t* s ) :
    last_start( timespan_t::min ), uptime_sum( timespan_t::zero ), sim( s ),
    uptime( std::numeric_limits<double>::quiet_NaN() )
  {}

  void update( bool is_up )
  {
    if ( is_up )
    {
      if ( last_start < timespan_t::zero )
        last_start = sim -> current_time;
    }
    else if ( last_start >= timespan_t::zero )
    {
      uptime_sum += sim -> current_time - last_start;
      last_start = timespan_t::min;
    }
  }

  void reset() { last_start = timespan_t::min; }

  void analyze()
  { uptime = uptime_sum.total_seconds() / sim -> iterations / sim -> simulation_length.mean; }

  void merge( const uptime_common_t& other )
  { uptime_sum += other.uptime_sum; }
};

struct uptime_t : public uptime_common_t
{
  std::string name_str;
  uptime_t* next;

  uptime_t( sim_t* s, const std::string& n ) :
    uptime_common_t( s ), name_str( n )
  {}

  const char* name() const { return name_str.c_str(); }
};

struct buff_uptime_t : public uptime_common_t
{ buff_uptime_t( sim_t* s ) : uptime_common_t( s ) {} };

// Gain =====================================================================

struct gain_t
{
  double actual, overflow, count;

  std::string name_str;
  resource_type type;
  gain_t* next;

  gain_t( const std::string& n ) :
    actual( 0 ), overflow( 0 ), count( 0 ), name_str( n ), type( RESOURCE_NONE )
  {}

  void add( double a, double o=0 ) { actual += a; overflow += o; count++; }
  void merge( const gain_t* other ) { actual += other -> actual; overflow += other -> overflow; count += other -> count; }
  void analyze( const sim_t* sim ) { actual /= sim -> iterations; overflow /= sim -> iterations; count /= sim -> iterations; }
  const char* name() const { return name_str.c_str(); }
};

// Proc =====================================================================

struct proc_t
{
  double count;
  timespan_t last_proc;
  timespan_t interval_sum;
  double interval_count;

  double frequency;
  sim_t* sim;

  player_t* player;
  std::string name_str;
  proc_t* next;

  proc_t( sim_t* s, const std::string& n ) :
    count( 0 ), last_proc( timespan_t::zero ), interval_sum( timespan_t::zero ), interval_count( 0 ),
    frequency( 0 ), sim( s ), player( 0 ), name_str( n ), next( 0 )
  {}

  void occur()
  {
    count++;
    if ( last_proc > timespan_t::zero && last_proc < sim -> current_time )
    {
      interval_sum += sim -> current_time - last_proc;
      interval_count++;
    }
    last_proc = sim -> current_time;
  }

  void merge( const proc_t* other )
  {
    count          += other -> count;
    interval_sum   += other -> interval_sum;
    interval_count += other -> interval_count;
  }

  void analyze( const sim_t* sim )
  {
    count /= sim -> iterations;
    if ( interval_count > 0 ) frequency = interval_sum.total_seconds() / interval_count;
  }

  const char* name() const { return name_str.c_str(); }
};


// Report ===================================================================

struct report_t
{
  static void encode_html( std::string& buffer );
  static std::string encode_html( const char* str );
  //static void print_spell_query( sim_t* );
  static void print_profiles( sim_t* );
  static void print_json_profiles( sim_t* );
  static void print_text( FILE*, sim_t*, bool detail=true );
  static void print_html( sim_t* );
  static void print_html_player( FILE*, player_t*, int );
  static void print_xml( sim_t* );
  static void print_suite( sim_t* );
};

// Chart ====================================================================

struct chart_t
{
  static int raid_aps ( std::vector<std::string>& images, sim_t*, std::vector<player_t*>, bool dps );
  static int raid_dpet( std::vector<std::string>& images, sim_t* );
  static int raid_gear( std::vector<std::string>& images, sim_t* );

  static const char* raid_downtime    ( std::string& s, sim_t* );
  static const char* action_dpet      ( std::string& s, player_t* );
  static const char* action_dmg       ( std::string& s, player_t* );
  static const char* time_spent       ( std::string& s, player_t* );
  static const char* gains            ( std::string& s, player_t*, resource_type );
  static const char* timeline         ( std::string& s, player_t*, const std::vector<double>&, const std::string&, double avg=0, const char* color="FDD017" );
  static const char* scale_factors    ( std::string& s, player_t*, const std::string&, const gear_stats_t&, const gear_stats_t& );
  static const char* scaling_dps      ( std::string& s, player_t* );
  static const char* reforge_dps      ( std::string& s, player_t* );
  static const char* distribution ( std::string& s, sim_t*, const std::vector<int>&, const std::string&, double, double, double );

  static const char* gear_weights_wowhead   ( std::string& s, player_t* );
  static const char* gear_weights_wowreforge( std::string& s, player_t* );
  static const char* gear_weights_pawn      ( std::string& s, player_t*, bool hit_expertise=true );

  static const char* dps_error( std::string& s, player_t* );

  static const char* resource_color( int type );
};

// Log ======================================================================

struct log_t
{
  // Generic Output
  static void output( sim_t*, const char* format, ... ) PRINTF_ATTRIBUTE( 2,3 );

  // Combat Log (unsupported)
};

// Pseudo Random Number Generation ==========================================

struct rng_t
{
  std::string name_str;
  bool   gauss_pair_use;
  double gauss_pair_value;
  double expected_roll,  actual_roll,  num_roll;
  double expected_range, actual_range, num_range;
  double expected_gauss, actual_gauss, num_gauss;
  bool   average_range, average_gauss;
  rng_t* next;

  rng_t( const std::string& n, bool avg_range=false, bool avg_gauss=false );
  virtual ~rng_t() {}

  virtual int    type() const { return RNG_STANDARD; }
  virtual double real();
  virtual bool    roll( double chance );
  virtual double range( double min, double max );
  timespan_t range( timespan_t min, timespan_t max );
  virtual double gauss( double mean, double stddev, const bool truncate_low_end = false );
  timespan_t gauss( timespan_t mean, timespan_t stddev );
  double exgauss( double mean, double stddev, double nu );
  timespan_t exgauss( timespan_t mean, timespan_t stddev, timespan_t nu );
  virtual void   seed( uint32_t start=time(NULL) );
  void   report( FILE* );
  static double stdnormal_cdf( double u );
  static double stdnormal_inv( double p );

  static rng_t* create( sim_t*, const std::string& name, int type=RNG_STANDARD );
};

// String utils =============================================================

std::string tolower( const std::string& src );
std::string proper_option_name( const std::string& full_name );
#if 0 // UNUSED
std::string trim( const std::string& src );
void replace_char( std::string& str, char old_c, char new_c  );
void replace_str( std::string& str, const std::string& old_str, const std::string& new_str  );
bool str_to_float( const std::string& src, double& dest );
#endif // UNUSED

// base36 ===================================================================

class base36_t
{
public:
  typedef char encoding_t[36];
  typedef std::pair<int,int> pair_t;

  struct bad_char {
    char c;
    bad_char( char c ) : c( c ) {}
  };

private:
  const encoding_t& encoding;

public:
  base36_t( const encoding_t& e ) : encoding( e ) {}

  int decode( char c ) const;
  pair_t decode_pair( char c ) const
  {
    int i = decode( c );
    return std::make_pair( i / 6, i % 6 );
  }

  pair_t operator () ( char c ) const
  { return decode_pair( c ); }
};

// Knotor ===================================================================

namespace knotor {
bool parse_talents( player_t& player, const std::string& encoding );
}

// Ask Mr. Robot ============================================================

namespace mrrobot {
bool parse_talents( player_t& player, const std::string& encoding );
player_t* download_player( sim_t* sim, const std::string& profile_id,
                           cache::behavior_t=cache::players() );
}

// Torhead ==================================================================

namespace torhead {
bool parse_talents( player_t& player, const std::string& encoding );
std::string encode_talents( const player_t& p );
}

// HTTP Download  ===========================================================

struct http_t
{
private:
  static void format_( std::string& encoded_url, const std::string& url );
public:
  struct proxy_t
  {
    std::string type;
    std::string host;
    int port;
  };
  static proxy_t proxy;

  static void cache_load();
  static void cache_save();
  static bool clear_cache( sim_t*, const std::string& name, const std::string& value );

  static bool get( std::string& result, const std::string& url, cache::behavior_t b,
                   const std::string& confirmation=std::string(), int throttle_seconds=0 );

  static std::string& format( std::string& encoded_url, const std::string& url )
  { format_( encoded_url, url ); return encoded_url; }
  static std::string& format( std::string& url )
  { format_( url, url ); return url; }
};

// XML ======================================================================

struct xml_t
{
  static const char* get_name( xml_node_t* node );
  static xml_node_t* get_child( xml_node_t* root, const std::string& name );
  static xml_node_t* get_node ( xml_node_t* root, const std::string& path );
  static xml_node_t* get_node ( xml_node_t* root, const std::string& path, const std::string& parm_name, const std::string& parm_value );
  static int  get_children( std::vector<xml_node_t*>&, xml_node_t* root, const std::string& name = std::string() );
  static int  get_nodes   ( std::vector<xml_node_t*>&, xml_node_t* root, const std::string& path );
  static int  get_nodes   ( std::vector<xml_node_t*>&, xml_node_t* root, const std::string& path, const std::string& parm_name, const std::string& parm_value );
  static bool get_value( std::string& value, xml_node_t* root, const std::string& path = std::string() );
  static bool get_value( int&         value, xml_node_t* root, const std::string& path = std::string() );
  static bool get_value( double&      value, xml_node_t* root, const std::string& path = std::string() );
  static xml_node_t* get( sim_t* sim, const std::string& url, cache::behavior_t b,
                          const std::string& confirmation=std::string(), int throttle_seconds=0 );
  static xml_node_t* create( sim_t* sim, const std::string& input );
  static xml_node_t* create( sim_t* sim, FILE* input );
  static void print( xml_node_t* root, FILE* f=0, int spacing=0 );
};


// Java Script ==============================================================

struct js_t
{
  static js_node_t* get_child( js_node_t* root, const std::string& name );
  static js_node_t* get_node ( js_node_t* root, const std::string& path );
  static int  get_children( std::vector<js_node_t*>&, js_node_t* root );
  static int  get_value( std::vector<std::string>& value, js_node_t* root, const std::string& path = std::string() );
  static bool get_value( std::string& value, js_node_t* root, const std::string& path = std::string() );
  static bool get_value( int&         value, js_node_t* root, const std::string& path = std::string() );
  static bool get_value( double&      value, js_node_t* root, const std::string& path = std::string() );
  static js_node_t* create( sim_t* sim, const std::string& input );
  static js_node_t* create( sim_t* sim, FILE* input );
  static void print( js_node_t* root, FILE* f=0, int spacing=0 );
  static const char* get_name( js_node_t* root );
};


// Handy Actions ============================================================

struct wait_action_base_t : public action_t
{
  wait_action_base_t( player_t* player, const char* name ) :
    action_t( ACTION_OTHER, name, player )
  { trigger_gcd = timespan_t::zero; }

  virtual void execute()
  { player -> iteration_waiting_time += time_to_execute; }
};

// Wait For Cooldown Action =================================================

struct wait_for_cooldown_t : public wait_action_base_t
{
  cooldown_t* wait_cd;
  action_t* a;
  wait_for_cooldown_t( player_t* player, const char* cd_name );
  virtual bool usable_moving() { return a -> usable_moving(); }
  virtual timespan_t execute_time() const;
};

// buff_t inlines

inline buff_t* buff_t::find( sim_t* s, const std::string& name ) { return find( s -> buff_list, name ); }
inline buff_t* buff_t::find( player_t* p, const std::string& name ) { return find( p -> buff_list, name ); }
inline buff_t::actor_pair_t::actor_pair_t( targetdata_t* td )
  : target( &td->target ), source( &td->source )
{}

// sim_t inlines

inline double sim_t::real() const                { return default_rng_ -> real(); }
inline bool   sim_t::roll( double chance ) const { return default_rng_ -> roll( chance ); }

#ifdef WHAT_IF

#define AOE_CAP 10

struct ticker_t // replacement for dot_t, handles any ticking buff/debuff
{
  dot_t stuff
  double crit;
  double haste;
};

struct sim_t
{
  ...
  actor_t* actor_list;
  std::vector<player_t*> player_list;
  std::vector<mob_t*> mob_list;
  ...
  int get_aura_slot( const std::string& n, actor_t* source );
  ...
};

struct actor_t
{
  items, stats, action list, resource management...
  actor_t( const std::string& n, int type ) {}
  virtual actor_t*  choose_target();
  virtual void      execute_action() { choose_target(); execute_first_ready_action(); }
  virtual debuff_t* get_debuff( int aura_slot );
  virtual ticker_t* get_ticker( int aura_slot );
  virtual bool      is_ally( actor_t* other );
};

struct player_t : public actor_t
{
  scaling, current_target ( actor ), pet_list ...
  player_t( const std::string& n, int type ) : actor_t( n, type ) { sim.player_list.push_back( this ); }
};

struct pet_t : public actor_t
{
  owner, summon, dismiss...
  pet_t( const std::string& n, int type ) : actor_t( n, type ) {}
};

struct enemy_t : public actor_t
{
  health_by_time, health_per_player, arrise_at_time, arise_at_percent, ...
  enemy_t( const std::string& n ) : actor_t( n, ACTOR_ENEMY ) { sim.enemy_list.push_back( this ); }
};

struct action_t
{
  actor_t, ...
  action_t( const std::string& n );
  virtual int execute();
  virtual void schedule_execute();
  virtual double execute_time();
  virtual double haste();
  virtual double gcd();
  virtual bool ready();
  virtual void cancel();
  virtual void reset();
};

struct result_t
{
  actor_t* target;
  int type;
  bool hit;  // convenience
  bool crit; // convenience, two-roll (blocked crits, etc)
  double amount;
  ticker_t ticker;
};

struct ability_t : public action_t
{
  spell_data, resource, weapon, two_roll, self_cast, aoe, base_xyz (no player_xyz or target_xyz),
  direct_sp_coeff, direct_ap_coeff, tick_sp_coeff, tick_ap_coeff,
  harmful, healing, callbacks, std::vector<result_t> results,
  NO binary, NO repeating, NO direct_dmg, NO tick_dmg
  ability_t( spell_data_t* s ) : action_t( s -> name() ) {}
  virtual void  execute()
  {
    actor_t* targets[ AOE_CAP ];
    int num_targets = area_of_effect( targets );
    results.resize( num_targets );
    for ( int i=0; i < num_targets; i++ )
    {
      calculate_result( results[ i ], targets[ i ] );
      // "result" callbacks
    }
    consume_resource();
    for ( int i=0; i < num_targets; i++ )
    {
      schedule_travel( results[ i ] );
    }
    update_ready();
    // "cast" callbacks
  }
  virtual void impact( result_t& result )
  {
    if ( result.hit )
    {
      if ( result.amount > 0 )
      {
        if ( harmful )
        {
          assess_damage( result );
        }
        else if ( healing )
        {
          assess_healing( result );
        }
      }
      if ( num_ticks > 0 )
      {
        ticker_t* ticker = get_ticker( result.target );  // caches aura_slot
        ticker -> trigger( this, result.ticker );
        // ticker_t::trigger() handles dot work in existing action_t::impact()
      }
    }
    else
    {
      // miss msg
      stat -> add_result( result );
    }
  }
  virtual void   tick         ( ticker_t* );
  virtual void   last_tick    ( ticker_t* );
  virtual void   schedule_tick( ticker_t* );
  virtual int    calculate_num_ticks( double haste, double duration=0 );
  virtual double cost();
  virtual double haste();
  virtual bool   ready();
  virtual void   cancel();
  virtual void   reset();
  virtual void   consume_resource();
  virtual result_type calculate_attack_roll( actor_t* target );
  virtual result_type calculate_spell_roll( actor_t* target );
  virtual result_type calculate_roll( actor_t* target )
  {
    if ( weapon )
      return calculate_attack_roll( target );
    else
      return calculate_spell_roll( target );
  }
  virtual void calculate_result( result_t& result, actor_t* target )
  {
    result.type = calculate_roll( target );
    result.hit  = ( roll == ? );
    result.crit = ( roll == ? ) || ( two_roll );
    result.amount = calculate_direct_amount( target );
    if ( result.hit && num_ticks )
    {
      calculate_ticker( &( result.ticker ), target );
    }
    return result;
  }
  virtual void calculate_ticker( ticker_t* ticker, target )
  {
    if ( target ) ticker -> target = target;
    ticker -> amount  = calculate_tick_amount( ticker -> target );
    ticker -> crit    = calculate_crit_chance( ticker -> target );
    ticker -> haste   = calculate_haste      ( ticker -> target );
  }
  virtual void refresh_ticker( ticker_t* ticker )
  {
    calculate_ticker( ticker );
    ticker -> current_tick = 0;
    ticker -> added_ticks = 0;
    ticker -> added_time = 0;
    ticker -> num_ticks = calculate_num_ticks( ticker -> haste );
    ticker -> recalculate_finish();
  }
  virtual void extend_ticker_by_time( ticker_t* ticker, double extra_time )
  {
    int    full_tick_count   = ticker -> ticks() - 1;
    double full_tick_remains = ticker -> finish - ticker -> tick_event -> time;

    ticker -> haste = calculate_haste( ticker -> target );
    ticker -> num_ticks += calculate_num_ticks( ticker -> haste, full_tick_remains ) - full_tick_count;
    ticker -> recalculate_finish();
  }
  virtual void extend_ticker_by_ticks( ticker_t* ticker, double extra_ticks )
  {
    calculate_ticker( ticker );
    ticker -> added_ticks += extra_ticks;
    ticker -> num_ticks   += extra_ticks;
    ticker -> recalculate_finish();
  }
  virtual double calculate_weapon_amount( actor_t* target );
  virtual double calculate_direct_amount( actor_t* target );
  virtual double calculate_tick_amount  ( actor_t* target );
  virtual double calculate_power( actor_t* target )
  {
    return AP_multiplier * AP() + SP_multiplier * SP();
  }
  virtual double calculate_haste( actor_t* target )
  {
    if ( weapon )
      return calculate_attack_haste( target );
    else
      return calculate_spell_haste( target );
  }
  virtual double calculate_miss_chance( actor_t* target )
  {
    if ( weapon )
      return calculate_attack_miss_chance( target );
    else
      return calculate_spell_miss_chance( target );
  }
  virtual double calculate_dodge_chance( actor_t* target )
  {
    if ( weapon )
      return calculate_attack_dodge_chance( target );
    else
      return 0;
  }
  virtual double calculate_parry_chance( actor_t* target )
  {
    if ( weapon )
      return calculate_attack_parry_chance( target );
    else
      return 0;
  }
  virtual double calculate_block_chance( actor_t* target )
  {
    if ( weapon )
      return calculate_attack_block_chance( target );
    else
      return 0;
  }
  virtual double calculate_crit_chance( actor_t* target )
  {
    if ( weapon )
      return calculate_attack_crit_chance( target );
    else
      return calculate_spell_crit_chance( target );
  }
  virtual double calculate_crit_chance( ticker_t* ticker );
  virtual double calculate_source_multiplier();
  virtual double calculate_direct_multiplier() { return calculate_source_multiplier(); } // include crit bonus here
  virtual double calculate_tick_multiplier  () { return calculate_source_multiplier(); }
  virtual double calculate_target_multiplier( actor_t* target );
  virtual int       area_of_effect( actor_t* targets[] ) { targets[ 0 ] = self_cast ? actor : actor -> current_target; return 1; }
  virtual result_t& result(); // returns 0th "result", asserts if aoe
  virtual double    travel_time( actor_t* target );
  virtual void      schedule_travel( result_t& result );
  virtual void assess_damage( result_t& result )
  {
    result.amount *= calculate_target_multiplier( result.target ); // allows for action-specific target multipliers
    result.target -> assess_damage( result ); // adjust result as needed for flexibility
    stat -> add_result( result );
  }
  virtual void assess_healing( result_t& result )
  {
    result.amount *= calculate_target_multiplier( result.target ); // allows for action-specific target multipliers
    result.target -> assess_healing( result ); // adjust result as needed for flexibility
    stat -> add_result( result );
  }
};

#endif

#endif // SIMULATIONCRAFT_H
