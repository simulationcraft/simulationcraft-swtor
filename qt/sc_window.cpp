// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraftqt.hpp"
#ifdef SC_PAPERDOLL
#include "simcpaperdoll.h"
#endif
#include <QtWebKit>
#ifdef Q_WS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

// ==========================================================================
// Utilities
// ==========================================================================

struct OptionEntry
{
  const char* label;
  const char* option;
  const char* tooltip;
};

static OptionEntry* getBuffOptions()
{
  static OptionEntry options[] =
  {
    { "Toggle All Buffs", "",                         "Toggle all buffs on/off"                           },
    { "Crit",             "override.coordination",    "+5% Crit"                                          },
    { "Stats",            "override.force_valor",     "+5% Stat Buff ( Str, Aim, Cunning, Willpower )"    },
    { "Endurance",        "override.fortification_hunters_boon","+5% Endurance"                                    },
    { "Damage Bonus",     "override.unnatural_might", "+5% Damage Bonus"                                  },
    { NULL, NULL, NULL }
  };
  return options;
}

#if 0
static OptionEntry* getItemSourceOptions()
{
  static OptionEntry options[] =
  {
    { "Local Item Database", "local",   "Use Simulationcraft item database" },
    { "Blizzard API",        "bcpapi",  "Remote Blizzard Community Platform API source" },
    { "Wowhead.com",         "wowhead", "Remote Wowhead.com item data source" },
    { "Mmo-champion.com",    "mmoc",    "Remote Mmo-champion.com item data source" },
    { "Blizzard Armory",     "armory",  "Remote item database from Blizzard (DEPRECATED, SHOULD NOT BE USED)" },
    { "Wowhead.com (PTR)",   "ptrhead", "Remote Wowhead.com PTR item data source" },
    { NULL, NULL, NULL }
  };

  return options;
}
#endif

static OptionEntry* getDebuffOptions()
{
  static OptionEntry options[] =
  {
    { "Toggle All Debuffs",     "",                        "Toggle all debuffs on/off"     },
    { "Armor Reduction",        "override.shatter_shot",   "-20% Armor Reduction"          },
    //{ "Armor Reduction SU",     "override.sunder",         "-20% Armor Sundering Strike"   },
    //{ "Armor Reduction HS",     "override.heat_signature", "-20% Armor Heat Signature"     },
    { NULL, NULL, NULL }
  };
  return options;
}

static OptionEntry* getScalingOptions()
{
  static OptionEntry options[] =
  {
    { "Analyze All Stats",                "",         "Scale factors are necessary for gear ranking.\nThey only require an additional simulation for each RELEVANT stat." },
    {
      "Use Positive Deltas Only",         "",         "Normally Accuracy/Expertise use negative scale factors to show DPS lost by reducing that stat.\n"
      "This option forces a positive scale delta, which is useful for classes with soft caps."
    },
    { "Analyze Aim",                      "aim",          "Calculate scale factor for Aim"                    },
    { "Analyze Cunning",                  "cunning",      "Calculate scale factor for Strength"               },
    { "Analyze Strength",                 "strength",     "Calculate scale factor for Strength"               },
    { "Analyze Willpower",                "willpower",    "Calculate scale factor for Willpower"              },
    { "Analyze Accuracy Rating",          "acc",          "Calculate scale factor for Accuracy Rating"        },
    { "Analyze Crit Rating",              "crit",         "Calculate scale factor for Crit Rating"            },
    { "Analyze Alacrity Rating",          "alacrity",     "Calculate scale factor for Alacrity Rating"        },
    { "Analyze Surge Rating",             "surge",        "Calculate scale factor for Surge Rating"           },
    { "Analyze Power",                    "power",        "Calculate scale factor for Power"                  },
    { "Analyze Force Power",              "force_power",  "Calculate scale factor for Force Power"            },
    { "Analyze Tech Power",               "tech_power",   "Calculate scale factor for Tech Power"             },
    { "Analyze Weapon Damage",            "wdmg",         "Calculate scale factor for Weapon Damage"          },
    { "Analyze Off-hand Weapon Damage",   "wohdmg",       "Calculate scale factor for Off-hand Weapon Damage" },
    { "Analyze Armor",                    "armor",        "Calculate scale factor for Armor"                  },
    { "Analyze Defense Rating",           "defense",      "Calculate scale factor for Defense Rating"         },
    { NULL, NULL, NULL }  };
  return options;
}

static OptionEntry* getPlotOptions()
{
  static OptionEntry options[] =
  {
    { "Plot DPS per Strength",            "str",        "Generate DPS curve for Strength"         },
    { "Plot DPS per Willpower",           "willpower",  "Generate DPS curve for Willpower"        },
    { "Plot DPS per Accuracy Rating",     "acc",        "Generate DPS curve for Accuracy Rating"  },
    { "Plot DPS per Crit Rating",         "crit",       "Generate DPS curve for Crit Rating"      },
    { "Plot DPS per Alacrity Rating",     "alacrity",   "Generate DPS curve for Alacrity Rating"  },
    { "Plot DPS per Surge Rating",        "surge",      "Generate DPS curve for Surge Rating"     },
    { "Plot DPS per Power",               "power",      "Generate DPS curve for Power"            },
    { "Plot DPS per Force Power",         "forcepower", "Generate DPS curve for Force Power"      },
    { "Plot DPS per Tech Power",          "techpower",  "Generate DPS curve for Tech Power"       },
    { "Plot DPS per Weapon Damage",       "wdmg",       "Generate DPS curve for Weapon Damage"    },
    { "Plot DPS per Off-hand Weapon Damage",       "wohdmg",       "Generate DPS curve for Off-hand Weapon Damage"   },
    { NULL, NULL, NULL }
  };
  return options;
}

static OptionEntry* getReforgePlotOptions()
{
  static OptionEntry options[] =
  {
    { "Plot Reforge Options for Willpower",         "willpower", "Generate reforge plot data for Willpower"        },
    { "Plot Reforge Options for Power",             "power",     "Generate reforge plot data for Power"            },
    { "Plot Reforge Options for Surge Rating",      "surge",     "Generate reforge plot data for Surge Rating"     },
    { "Plot Reforge Options for Accuracy Rating",   "acc",       "Generate reforge plot data for Accuracy Rating"  },
    { "Plot Reforge Options for Crit Rating",       "crit",      "Generate reforge plot data for Crit Rating"      },
    { "Plot Reforge Options for Alacrity Rating",   "alacrity",  "Generate reforge plot data for Alacrity Rating"  },
    { NULL, NULL, NULL }
  };
  return options;
}

static QString defaultSimulateText()
{
  return QString( "# Profile will be downloaded into here.\n"
                  "# Use the Back/Forward buttons to cycle through the script history.\n"
                  "# Use the Up/Down arrow keys to cycle through the command-line history.\n"
                  "#\n"
                  "# Clicking Simulate will create a simc_gui.simc profile for review.\n" );
}

static QComboBox* createChoice( int count, ... )
{
  QComboBox* choice = new QComboBox();
  va_list vap;
  va_start( vap, count );
  for ( int i=0; i < count; i++ )
  {
    const char* s = ( char* ) va_arg( vap, char* );
    choice->addItem( s );
  }
  va_end( vap );
  return choice;
}

ReforgeButtonGroup::ReforgeButtonGroup( QObject* parent ) :
  QButtonGroup( parent ), selected( 0 )
{}

void ReforgeButtonGroup::setSelected( int state )
{
  if ( state ) selected++; else selected--;

  // Three selected, disallow selection of any more
  if ( selected >= 3 )
  {
    QList< QAbstractButton* > b = buttons();
    for ( QList< QAbstractButton* >::iterator i = b.begin(); i != b.end(); i++ )
      if ( ! ( *i ) -> isChecked() ) ( *i ) -> setEnabled( false );
  }
  // Less than three selected, allow selection of all/any
  else
  {
    QList< QAbstractButton* > b = buttons();
    for ( QList< QAbstractButton* >::iterator i = b.begin(); i != b.end(); i++ )
      ( *i ) -> setEnabled( true );
  }
}

void SimulationCraftWindow::decodeOptions( QString encoding )
{
  int i = 0;
  QStringList tokens = encoding.split( ' ' );
  if ( i < tokens.count() )
          versionChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
       iterationsChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
      fightLengthChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
    fightVarianceChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
       fightStyleChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
       targetRaceChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
      playerSkillChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
          threadsChoice->setCurrentIndex( tokens[ i++ ].toInt() );
#if 0
  if ( i < tokens.count() )
     armoryRegionChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
       armorySpecChoice->setCurrentIndex( tokens[ i++ ].toInt() );
#endif
  if ( i < tokens.count() )
      defaultRoleChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
          latencyChoice->setCurrentIndex( tokens[ i++ ].toInt() );
#if 0
  if ( i < tokens.count() )
      targetLevelChoice->setCurrentIndex( tokens[ i++ ].toInt() );
#endif
  if ( i < tokens.count() )
       reportpetsChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
       printstyleChoice->setCurrentIndex( tokens[ i++ ].toInt() );
  if ( i < tokens.count() )
       statisticslevel_Choice->setCurrentIndex( tokens[ i++ ].toInt() );

  QList<QAbstractButton*>       buff_buttons  =        buffsButtonGroup->buttons();
  QList<QAbstractButton*>     debuff_buttons  =      debuffsButtonGroup->buttons();
  QList<QAbstractButton*>    scaling_buttons  =      scalingButtonGroup->buttons();
  QList<QAbstractButton*>        plot_buttons =        plotsButtonGroup->buttons();
  QList<QAbstractButton*> reforgeplot_buttons = reforgeplotsButtonGroup->buttons();

  OptionEntry*        buffs = getBuffOptions();
  OptionEntry*      debuffs = getDebuffOptions();
  OptionEntry*      scaling = getScalingOptions();
  OptionEntry*        plots = getPlotOptions();
  OptionEntry* reforgeplots = getReforgePlotOptions();

  for ( ; i < tokens.count(); i++ )
  {
    QStringList opt_tokens = tokens[ i ].split( ':' );

    OptionEntry* options=0;
    QList<QAbstractButton*>* buttons=0;

    if (      ! opt_tokens[ 0 ].compare( "buff"           ) ) { options = buffs;           buttons = &buff_buttons;        }
    else if ( ! opt_tokens[ 0 ].compare( "debuff"         ) ) { options = debuffs;         buttons = &debuff_buttons;      }
    else if ( ! opt_tokens[ 0 ].compare( "scaling"        ) ) { options = scaling;         buttons = &scaling_buttons;     }
    else if ( ! opt_tokens[ 0 ].compare( "plots"          ) ) { options = plots;           buttons = &plot_buttons;        }
    else if ( ! opt_tokens[ 0 ].compare( "reforge_plots"  ) ) { options = reforgeplots;    buttons = &reforgeplot_buttons; }

    if ( ! options ) continue;

    QStringList opt_value = opt_tokens[ 1 ].split( '=' );
    for ( int opt=0; options[ opt ].label; opt++ )
    {
      if ( ! opt_value[ 0 ].compare( options[ opt ].option ) )
      {
        buttons -> at( opt )->setChecked( 1 == opt_value[ 1 ].toInt() );
        break;
      }
    }
  }
}

QString SimulationCraftWindow::encodeOptions()
{
  QString encoded;
  QTextStream ss( &encoded );

  ss << versionChoice->currentIndex();
  ss << ' ' << iterationsChoice->currentIndex();
  ss << ' ' << fightLengthChoice->currentIndex();
  ss << ' ' << fightVarianceChoice->currentIndex();
  ss << ' ' << fightStyleChoice->currentIndex();
  ss << ' ' << targetRaceChoice->currentIndex();
  ss << ' ' << playerSkillChoice->currentIndex();
  ss << ' ' << threadsChoice->currentIndex();
  //ss << ' ' << armoryRegionChoice->currentIndex();
  //ss << ' ' << armorySpecChoice->currentIndex();
  ss << ' ' << defaultRoleChoice->currentIndex();
  ss << ' ' << latencyChoice->currentIndex();
  //ss << ' ' << targetLevelChoice->currentIndex();
  ss << ' ' << reportpetsChoice->currentIndex();
  ss << ' ' << printstyleChoice->currentIndex();
  ss << ' ' << statisticslevel_Choice->currentIndex();

  QList<QAbstractButton*> buttons = buffsButtonGroup->buttons();
  OptionEntry* buffs = getBuffOptions();
  for ( int i=1; buffs[ i ].label; i++ )
  {
    ss << " buff:" << buffs[ i ].option << '='
       << ( buttons.at( i ) -> isChecked() ? '1' : '0' );
  }

  buttons = debuffsButtonGroup->buttons();
  OptionEntry* debuffs = getDebuffOptions();
  for ( int i=1; debuffs[ i ].label; i++ )
  {
    ss << " debuff:" << debuffs[ i ].option << '='
       << ( buttons.at( i ) -> isChecked() ? '1' : '0' );
  }

  buttons = scalingButtonGroup->buttons();
  OptionEntry* scaling = getScalingOptions();
  for ( int i=2; scaling[ i ].label; i++ )
  {
    ss << " scaling:" << scaling[ i ].option << '='
       << ( buttons.at( i ) -> isChecked() ? '1' : '0' );
  }

  buttons = plotsButtonGroup->buttons();
  OptionEntry* plots = getPlotOptions();
  for ( int i=0; plots[ i ].label; i++ )
  {
    ss << " plots:" << plots[ i ].option << '='
       << ( buttons.at( i ) -> isChecked() ? '1' : '0' );
  }

  buttons = reforgeplotsButtonGroup->buttons();
  OptionEntry* reforgeplots = getReforgePlotOptions();
  for ( int i=0; reforgeplots[ i ].label; i++ )
  {
    ss << " reforge_plots:" << reforgeplots[ i ].option << '='
       << ( buttons.at( i ) -> isChecked() ? '1' : '0' );
  }

  /*if ( itemDbOrder -> count() > 0 )
  {
    ss << " item_db_source:";
    for ( int i = 0; i < itemDbOrder -> count(); i++ )
    {
      QListWidgetItem *it = itemDbOrder -> item( i );
      ss << it -> data( Qt::UserRole ).toString();
      if ( i < itemDbOrder -> count() - 1 )
        ss << '/';
    }
  }*/

  return encoded;
}

void SimulationCraftWindow::updateSimProgress()
{
  if ( sim )
  {
    simProgress = ( int ) ( 100.0 * sim->progress( simPhase ) );
  }
  else
  {
    simPhase = "%p%";
    simProgress = 100;
  }
  if ( mainTab->currentIndex() != TAB_IMPORT &&
       mainTab->currentIndex() != TAB_RESULTS )
  {
    progressBar->setFormat( QString::fromUtf8( simPhase.c_str() ) );
    progressBar->setValue( simProgress );
  }
}

void SimulationCraftWindow::loadHistory()
{
  http_t::cache_load();
  QFile file( "simc_history.dat" );
  if ( file.open( QIODevice::ReadOnly ) )
  {
    QDataStream in( &file );
    QString historyVersion;
    in >> historyVersion;
    if ( historyVersion != HISTORY_VERSION ) return;
    in >> historyWidth;
    in >> historyHeight;
    in >> historyMaximized;
    QStringList importHistory;
    in >> simulateCmdLineHistory;
    in >> logCmdLineHistory;
    in >> resultsCmdLineHistory;
    in >> optionsHistory;
    in >> simulateTextHistory;
    in >> overridesTextHistory;
    in >> importHistory;
    file.close();

    int count = importHistory.count();
    for ( int i=0; i < count; i++ )
    {
      QListWidgetItem* item = new QListWidgetItem( importHistory.at( i ) );
      historyList->addItem( item );
    }

    decodeOptions( optionsHistory.backwards() );

    QString s = overridesTextHistory.backwards();
    if ( ! s.isEmpty() ) overridesText->setPlainText( s );
  }
}

void SimulationCraftWindow::saveHistory()
{
  if ( cookieJar ) cookieJar->save();
  http_t::cache_save();
  QFile file( "simc_history.dat" );
  if ( file.open( QIODevice::WriteOnly ) )
  {
    optionsHistory.add( encodeOptions() );

    QStringList importHistory;
    int count = historyList->count();
    for ( int i=0; i < count; i++ )
    {
      importHistory.append( historyList->item( i )->text() );
    }

    QDataStream out( &file );
    out << QString( HISTORY_VERSION );
    out << ( qint32 ) width();
    out << ( qint32 ) height();
    out << ( qint32 ) ( ( windowState() & Qt::WindowMaximized ) ? 1 : 0 );
    out << simulateCmdLineHistory;
    out << logCmdLineHistory;
    out << resultsCmdLineHistory;
    out << optionsHistory;
    out << simulateTextHistory;
    out << overridesTextHistory;
    out << importHistory;
    file.close();
  }
}

// ==========================================================================
// Widget Creation
// ==========================================================================

SimulationCraftWindow::SimulationCraftWindow( QWidget *parent )
  : QWidget( parent ),
    historyWidth( 0 ), historyHeight( 0 ), historyMaximized( 1 ),
    mrRobotBuilderView( 0 ), visibleWebView( 0 ), cookieJar( 0 ),
    sim(), simPhase( "%p%" ), simProgress( 100 ), simResults( 0 )
{
#ifndef Q_WS_MAC
  logFileText = "log.txt";
  resultsFileText = "results.html";
#else
  logFileText = QDir::currentPath() + QDir::separator() + "log.txt";
  resultsFileText = QDir::currentPath() + QDir::separator() + "results.html";
#endif

  mainTab = new QTabWidget();
  createWelcomeTab();
  createOptionsTab();
  createImportTab();
  createSimulateTab();
  createOverridesTab();
  createHelpTab();
  createLogTab();
  createResultsTab();
  createSiteTab();
  createCmdLine();
  createToolTips();
#ifdef SC_PAPERDOLL
  createPaperdoll();
#endif

  connect( mainTab, SIGNAL( currentChanged( int ) ), this, SLOT( mainTabChanged( int ) ) );

  QVBoxLayout* vLayout = new QVBoxLayout();
  vLayout->addWidget( mainTab );
  vLayout->addWidget( cmdLineGroupBox );
  setLayout( vLayout );

  timer = new QTimer( this );
  connect( timer, SIGNAL( timeout() ), this, SLOT( updateSimProgress() ) );

  importThread = new ImportThread( this );
  simulateThread = new SimulateThread( this );

  connect(   importThread, SIGNAL( finished() ), this, SLOT(  importFinished() ) );
  connect( simulateThread, SIGNAL( finished() ), this, SLOT( simulateFinished() ) );

  setAcceptDrops( true );

  importTab->setCurrentIndex( TAB_MR_ROBOT );

  loadHistory();
}

void SimulationCraftWindow::createCmdLine()
{
  QHBoxLayout* cmdLineLayout = new QHBoxLayout();
  cmdLineLayout->addWidget( backButton = new QPushButton( "<" ) );
  cmdLineLayout->addWidget( forwardButton = new QPushButton( ">" ) );
  cmdLineLayout->addWidget( cmdLine = new SimulationCraftCommandLine( this ) );
  cmdLineLayout->addWidget( progressBar = new QProgressBar() );
  cmdLineLayout->addWidget( mainButton = new QPushButton( "Simulate!" ) );
  backButton->setMaximumWidth( 30 );
  forwardButton->setMaximumWidth( 30 );
  progressBar->setMaximum( 100 );
  progressBar->setMaximumWidth( 200 );
  progressBar->setMinimumWidth( 150 );
  QFont progfont( progressBar -> font() );
  progfont.setPointSize( 11 );
  progressBar->setFont( progfont );
  connect( backButton,    SIGNAL( clicked( bool ) ),   this, SLOT(    backButtonClicked() ) );
  connect( forwardButton, SIGNAL( clicked( bool ) ),   this, SLOT( forwardButtonClicked() ) );
  connect( mainButton,    SIGNAL( clicked( bool ) ),   this, SLOT(    mainButtonClicked() ) );
  connect( cmdLine,       SIGNAL( returnPressed() ),            this, SLOT( cmdLineReturnPressed() ) );
  connect( cmdLine,       SIGNAL( textEdited( const QString& ) ), this, SLOT( cmdLineTextEdited( const QString& ) ) );
  cmdLineGroupBox = new QGroupBox();
  cmdLineGroupBox->setLayout( cmdLineLayout );
}

void SimulationCraftWindow::createWelcomeTab()
{
  QString welcomeFile = QDir::currentPath() + "/Welcome.html";
#ifdef Q_WS_MAC
  CFURLRef fileRef    = CFBundleCopyResourceURL( CFBundleGetMainBundle(), CFSTR( "Welcome" ), CFSTR( "html" ), 0 );
  if ( fileRef )
  {
    CFStringRef macPath = CFURLCopyFileSystemPath( fileRef, kCFURLPOSIXPathStyle );
    welcomeFile         = CFStringGetCStringPtr( macPath, CFStringGetSystemEncoding() );

    CFRelease( fileRef );
    CFRelease( macPath );
  }
#endif
  QString url = "file:///" + welcomeFile;

  QWebView* welcomeBanner = new QWebView();
  welcomeBanner->setUrl( url );
  mainTab->addTab( welcomeBanner, "Welcome" );
}

void SimulationCraftWindow::createOptionsTab()
{
  optionsTab = new QTabWidget();
  mainTab->addTab( optionsTab, "Options" );

  createGlobalsTab();
  createBuffsTab();
  createDebuffsTab();
  createScalingTab();
  createPlotsTab();
  createReforgePlotsTab();

  QAbstractButton* allBuffs   =   buffsButtonGroup->buttons().at( 0 );
  QAbstractButton* allDebuffs = debuffsButtonGroup->buttons().at( 0 );
  QAbstractButton* allScaling = scalingButtonGroup->buttons().at( 0 );

  //connect( armoryRegionChoice, SIGNAL( currentIndexChanged( const QString& ) ), this, SLOT( armoryRegionChanged( const QString& ) ) );

  connect( allBuffs,   SIGNAL( toggled( bool ) ), this, SLOT( allBuffsChanged( bool ) )   );
  connect( allDebuffs, SIGNAL( toggled( bool ) ), this, SLOT( allDebuffsChanged( bool ) ) );
  connect( allScaling, SIGNAL( toggled( bool ) ), this, SLOT( allScalingChanged( bool ) ) );
}

void SimulationCraftWindow::createGlobalsTab()
{
  QFormLayout* globalsLayout = new QFormLayout();
  globalsLayout->setFieldGrowthPolicy( QFormLayout::FieldsStayAtSizeHint );
  globalsLayout->addRow(        "Version",       versionChoice = createChoice( 3, "Live", "PTR", "Both" ) );
  globalsLayout->addRow(     "Iterations",    iterationsChoice = createChoice( 6, "100", "1000", "10000", "25000", "50000", "100000" ) );
  globalsLayout->addRow(      "World Lag",       latencyChoice = createChoice( 3, "Low", "Medium", "High" ) );
  globalsLayout->addRow(   "Length (sec)",   fightLengthChoice = createChoice( 9, "100", "150", "200", "250", "300", "350", "400", "450", "500" ) );
  globalsLayout->addRow(    "Vary Length", fightVarianceChoice = createChoice( 3, "0%", "10%", "20%" ) );
  globalsLayout->addRow(    "Fight Style",    fightStyleChoice = createChoice( 5, "Patchwerk", "HelterSkelter", "Ultraxion", "LightMovement", "HeavyMovement" ) );
  //globalsLayout->addRow(   "Target Level",   targetLevelChoice = createChoice( 3, "Raid Boss", "5-man heroic", "5-man normal" ) );
  globalsLayout->addRow(    "Target Race",    targetRaceChoice = createChoice( 7, "humanoid", "beast", "demon", "dragonkin", "elemental", "giant", "undead" ) );
  globalsLayout->addRow(   "Player Skill",   playerSkillChoice = createChoice( 4, "Elite", "Good", "Average", "Ouch! Fire is hot!" ) );
  globalsLayout->addRow(        "Threads",       threadsChoice = createChoice( 5, "1", "2", "4", "8", "12" ) );
  //globalsLayout->addRow(  "Armory Region",  armoryRegionChoice = createChoice( 5, "us", "eu", "tw", "cn", "kr" ) );
  //globalsLayout->addRow(    "Armory Spec",    armorySpecChoice = createChoice( 2, "active", "inactive" ) );
  globalsLayout->addRow(   "Default Role",   defaultRoleChoice = createChoice( 4, "auto", "dps", "heal", "tank" ) );
  globalsLayout->addRow( "Generate Debug",         debugChoice = createChoice( 3, "None", "Log Only", "Gory Details" ) );
  globalsLayout->addRow( "Report Pets Separately", reportpetsChoice = createChoice( 2, "Yes", "No" ) );
  globalsLayout->addRow( "Report Print Style", printstyleChoice = createChoice( 2, "Classic", "White" ) );
  globalsLayout->addRow( "Statistics Level", statisticslevel_Choice = createChoice( 5, "0", "1", "2", "3", "8" ) );
  iterationsChoice->setCurrentIndex( 1 );
  fightLengthChoice->setCurrentIndex( 4 );
  fightVarianceChoice->setCurrentIndex( 2 );
  reportpetsChoice->setCurrentIndex( 1 );
  statisticslevel_Choice->setCurrentIndex( 1 );
  QGroupBox* globalsGroupBox = new QGroupBox();
  globalsGroupBox->setLayout( globalsLayout );

  optionsTab->addTab( globalsGroupBox, "Globals" );

  //createItemDataSourceSelector( globalsLayout );
}

void SimulationCraftWindow::createBuffsTab()
{
  QVBoxLayout* buffsLayout = new QVBoxLayout();
  buffsButtonGroup = new QButtonGroup();
  buffsButtonGroup->setExclusive( false );
  OptionEntry* buffs = getBuffOptions();
  for ( int i=0; buffs[ i ].label; i++ )
  {
    QCheckBox* checkBox = new QCheckBox( buffs[ i ].label );
    if ( i > 0 ) checkBox->setChecked( true );
    checkBox->setToolTip( buffs[ i ].tooltip );
    buffsButtonGroup->addButton( checkBox );
    buffsLayout->addWidget( checkBox );
  }
  buffsLayout->addStretch( 1 );
  QGroupBox* buffsGroupBox = new QGroupBox();
  buffsGroupBox->setLayout( buffsLayout );

  optionsTab->addTab( buffsGroupBox, "Buffs" );
}

void SimulationCraftWindow::createDebuffsTab()
{
  QVBoxLayout* debuffsLayout = new QVBoxLayout();
  debuffsButtonGroup = new QButtonGroup();
  debuffsButtonGroup->setExclusive( false );
  OptionEntry* debuffs = getDebuffOptions();
  for ( int i=0; debuffs[ i ].label; i++ )
  {
    QCheckBox* checkBox = new QCheckBox( debuffs[ i ].label );
    if ( i>0 ) checkBox->setChecked( true );
    checkBox->setToolTip( debuffs[ i ].tooltip );
    debuffsButtonGroup->addButton( checkBox );
    debuffsLayout->addWidget( checkBox );
  }
  debuffsLayout->addStretch( 1 );
  QGroupBox* debuffsGroupBox = new QGroupBox();
  debuffsGroupBox->setLayout( debuffsLayout );

  optionsTab->addTab( debuffsGroupBox, "Debuffs" );
}

void SimulationCraftWindow::createScalingTab()
{
  QVBoxLayout* scalingLayout = new QVBoxLayout();
  scalingButtonGroup = new QButtonGroup();
  scalingButtonGroup->setExclusive( false );
  OptionEntry* scaling = getScalingOptions();
  for ( int i=0; scaling[ i ].label; i++ )
  {
    QCheckBox* checkBox = new QCheckBox( scaling[ i ].label );
    checkBox->setToolTip( scaling[ i ].tooltip );
    scalingButtonGroup->addButton( checkBox );
    scalingLayout->addWidget( checkBox );
  }
  scalingLayout->addStretch( 1 );
  QGroupBox* scalingGroupBox = new QGroupBox();
  scalingGroupBox->setLayout( scalingLayout );

  optionsTab->addTab( scalingGroupBox, "Scaling" );
}

void SimulationCraftWindow::createPlotsTab()
{
  QFormLayout* plotsLayout = new QFormLayout();
  plotsLayout -> setFieldGrowthPolicy( QFormLayout::FieldsStayAtSizeHint );

  // Creat Combo Boxes
  plotsPointsChoice = createChoice( 4, "20", "30", "40", "50" );
  plotsLayout -> addRow( "Number of Plot Points", plotsPointsChoice );

  plotsStepChoice = createChoice( 5, "5", "10", "15", "20", "25" );
  plotsStepChoice -> setCurrentIndex( 3 );
  plotsLayout -> addRow( "Plot Step Amount", plotsStepChoice );

  plotsButtonGroup = new QButtonGroup();
  plotsButtonGroup -> setExclusive( false );
  OptionEntry* plots = getPlotOptions();
  for ( int i=0; plots[ i ].label; i++ )
  {
    QCheckBox* checkBox = new QCheckBox( plots[ i ].label );
    checkBox -> setToolTip( plots[ i ].tooltip );
    plotsButtonGroup -> addButton( checkBox );
    plotsLayout -> addWidget( checkBox );
  }
  QGroupBox* plotsGroupBox = new QGroupBox();
  plotsGroupBox -> setLayout( plotsLayout );

  optionsTab -> addTab( plotsGroupBox, "Plots" );
}

void SimulationCraftWindow::createReforgePlotsTab()
{
  QFormLayout* reforgePlotsLayout = new QFormLayout();
  reforgePlotsLayout -> setFieldGrowthPolicy( QFormLayout::FieldsStayAtSizeHint );

  // Create Combo Boxes
  reforgePlotAmountChoice = createChoice( 5, "100", "200", "300", "400", "500" );
  reforgePlotAmountChoice -> setCurrentIndex( 1 ); // Default is 200
  reforgePlotsLayout -> addRow( "Reforge Amount", reforgePlotAmountChoice );

  reforgePlotStepChoice = createChoice( 5, "10", "20", "30", "40", "50" );
  reforgePlotStepChoice -> setCurrentIndex( 1 ); // Default is 20
  reforgePlotsLayout -> addRow( "Step Amount", reforgePlotStepChoice );

  QLabel* messageText = new QLabel( "A maximum of three stats may be run at once." );
  reforgePlotsLayout -> addRow( messageText );

  reforgeplotsButtonGroup = new ReforgeButtonGroup();
  reforgeplotsButtonGroup -> setExclusive( false );
  OptionEntry* reforgeplots = getReforgePlotOptions();
  for ( int i=0; reforgeplots[ i ].label; i++ )
  {
    QCheckBox* checkBox = new QCheckBox( reforgeplots[ i ].label );
    checkBox -> setToolTip( reforgeplots[ i ].tooltip );
    reforgeplotsButtonGroup -> addButton( checkBox );
    reforgePlotsLayout -> addWidget( checkBox );
    QObject::connect( checkBox, SIGNAL( stateChanged( int ) ),
                      reforgeplotsButtonGroup, SLOT( setSelected( int ) ) );
  }

  QGroupBox* reforgeplotsGroupBox = new QGroupBox();
  reforgeplotsGroupBox -> setLayout( reforgePlotsLayout );

  optionsTab->addTab( reforgeplotsGroupBox, "Reforge Plots" );
}

void SimulationCraftWindow::createImportTab()
{
  importTab = new QTabWidget();
  mainTab->addTab( importTab, "Import" );

  cookieJar = new PersistentCookieJar( "simcqt.cookies" );
  cookieJar->load();
  mrRobotBuilderView = new SimulationCraftWebView( this );
  mrRobotBuilderView->page()->networkAccessManager()->setCookieJar( cookieJar );
  mrRobotBuilderView->setUrl( QUrl( "http://swtor.askmrrobot.com/character/search" ) );
  importTab->addTab( mrRobotBuilderView, "Mr. Robot" );

  createBestInSlotTab();

  historyList = new QListWidget();
  historyList->setSortingEnabled( true );
  importTab->addTab( historyList, "History" );

  connect( historyList, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this, SLOT( historyDoubleClicked( QListWidgetItem* ) ) );
  connect( importTab,   SIGNAL( currentChanged( int ) ),                 this, SLOT( importTabChanged( int ) ) );

  // Commenting out until it is more fleshed out.
  // createCustomTab();

}

void SimulationCraftWindow::createBestInSlotTab()
{
  QStringList headerLabels( "Player Class" ); headerLabels += QString( "Location" );

  bisTree = new QTreeWidget();
  bisTree->setColumnCount( 1 );
  bisTree->setHeaderLabels( headerLabels );
  importTab->addTab( bisTree, "BiS" );

  QTreeWidgetItem* top[ PLAYER_MAX ];
  boost::fill( top, nullptr );
  QStringList profile_folder_list = QStringList() << "profiles" << "profiles_heal" << "profiles_tank";
  QStringList profile_folder_names = QStringList() << "Damage" << "Heal" << "Tank";

  QTreeWidgetItem* rootItems[ profile_folder_list.count() ][ PLAYER_MAX ];

  for ( int k=0; k < profile_folder_list.count(); k++ )
  {
    QTreeWidgetItem* top = new QTreeWidgetItem( QStringList( profile_folder_names[ k ] ) );
    bisTree -> addTopLevelItem( top );
    if ( k == 0 ) top -> setExpanded( true );

    boost::fill( rootItems[ k ], nullptr );

    // Scan all subfolders and create a list
#ifndef Q_WS_MAC
  QDir tdir = QString( profile_folder_list[ k ] );
#else
  CFURLRef fileRef    = CFBundleCopyResourceURL( CFBundleGetMainBundle(), CFSTR( profile_folder_list[ k ] ), 0, 0 );
  QDir tdir;
  if ( fileRef )
  {
    CFStringRef macPath = CFURLCopyFileSystemPath( fileRef, kCFURLPOSIXPathStyle );
    tdir            = QString( CFStringGetCStringPtr( macPath, CFStringGetSystemEncoding() ) );

    CFRelease( fileRef );
    CFRelease( macPath );
  }
#endif
  tdir.setFilter( QDir::Dirs );

  QStringList tprofileList = tdir.entryList();
  int tnumProfiles = tprofileList.count();
  // Main loop through all subfolders of ./profiles/
  for ( int i=0; i < tnumProfiles; i++ )
  {
#ifndef Q_WS_MAC
    QDir dir = QString( profile_folder_list[ k ] + "/" + tprofileList[ i ] );
#else
    CFURLRef fileRef = CFBundleCopyResourceURL( CFBundleGetMainBundle(),
                                                                           CFSTR(  profile_folder_list[ k ] + tprofileList[ i ] ) + CFStringCreateWithCString( NULL,
                                                                           tprofileList[ i ].toAscii().constData(),
                                                                           kCFStringEncodingUTF8 ),
                                                0,
                                                CFSTR( "profiles" ) );
    QDir dir;
    if ( fileRef )
    {
      CFStringRef macPath = CFURLCopyFileSystemPath( fileRef, kCFURLPOSIXPathStyle );
      dir            = QString( CFStringGetCStringPtr( macPath, CFStringGetSystemEncoding() ) );

      CFRelease( fileRef );
      CFRelease( macPath );
    }
#endif
    dir.setSorting( QDir::Name );
    dir.setFilter( QDir::Files );
    dir.setNameFilters( QStringList( "*.simc" ) );

    QStringList profileList = dir.entryList();
    int numProfiles = profileList.count();
    for ( int i=0; i < numProfiles; i++ )
    {
      // exclude generate/create profiles
      if ( profileList[ i ].contains( "generate" ) || profileList[ i ].contains( "create" ))
        continue;

      QString profile = dir.absolutePath() + "/";
      profile = QDir::toNativeSeparators( profile );
      profile += profileList[ i ];

      for ( player_type pt = PLAYER_NONE; pt < PLAYER_MAX; ++pt )
      {
        if (   profileList[ i ].contains( util_t::player_type_string( pt ), Qt::CaseInsensitive )
            || profileList[ i ].contains( util_t::player_type_string_short( pt ), Qt::CaseInsensitive ) )
        {
          if ( !rootItems[ k ][ pt ] )
          {
            top-> addChild( rootItems[ k ][ pt ] = new QTreeWidgetItem( QStringList( util_t::player_type_string( pt ) ) ) );
          }

          QTreeWidgetItem* item = new QTreeWidgetItem( QStringList() << profileList[ i ] << profile );
          rootItems[ k ][ pt ] -> addChild( item );
          break;
        }
      }
   }
 }
  }

  bisTree->setColumnWidth( 0, 300 );

  connect( bisTree, SIGNAL( itemDoubleClicked( QTreeWidgetItem*,int ) ), this, SLOT( bisDoubleClicked( QTreeWidgetItem*,int ) ) );
}

void SimulationCraftWindow::createCustomTab()
{
  //In Dev - Character Retrieval Boxes & Buttons
  //In Dev - Load & Save Profile Buttons
  //In Dev - Profiler Slots, Talent & Glyph Layout
  QHBoxLayout* customLayout = new QHBoxLayout();
  QGroupBox* customGroupBox = new QGroupBox();
  customGroupBox->setLayout( customLayout );
  importTab->addTab( customGroupBox, "Custom Profile" );
  customLayout->addWidget( createCustomCharData = new QGroupBox( tr( "Character Data" ) ), 1 );
  createCustomCharData->setObjectName( QString::fromUtf8( "createCustomCharData" ) );
  customLayout->addWidget( createCustomProfileDock = new QTabWidget(), 1 );
  createCustomProfileDock->setObjectName( QString::fromUtf8( "createCustomProfileDock" ) );
  createCustomProfileDock->setAcceptDrops( true );
  customGearTab = new QWidget();
  customGearTab->setObjectName( QString::fromUtf8( "customGearTab" ) );
  createCustomProfileDock->addTab( customGearTab, QString() );
  customTalentsTab = new QWidget();
  customTalentsTab->setObjectName( QString::fromUtf8( "customTalentsTab" ) );
  createCustomProfileDock->addTab( customTalentsTab, QString() );
  customGlyphsTab = new QWidget();
  customGlyphsTab->setObjectName( QString::fromUtf8( "customGlyphsTab" ) );
  createCustomProfileDock->addTab( customGlyphsTab, QString() );
  createCustomProfileDock -> setTabText( createCustomProfileDock -> indexOf( customGearTab ), tr( "Gear", "createCustomTab" ) );
  createCustomProfileDock -> setTabToolTip( createCustomProfileDock -> indexOf( customGearTab ), tr( "Customise Gear Setup", "createCustomTab" ) );
  createCustomProfileDock -> setTabText( createCustomProfileDock -> indexOf( customTalentsTab ), tr( "Talents", "createCustomTab" ) );
  createCustomProfileDock -> setTabToolTip( createCustomProfileDock -> indexOf( customTalentsTab ), tr( "Customise Talents", "createCustomTab" ) );
  createCustomProfileDock -> setTabText( createCustomProfileDock -> indexOf( customGlyphsTab ), tr( "Glyphs", "createCustomTab" ) );
  createCustomProfileDock -> setTabToolTip( createCustomProfileDock -> indexOf( customGlyphsTab ), tr( "Customise Glyphs", "createCustomTab" ) );
}

void SimulationCraftWindow::createSimulateTab()
{
  simulateText = new SimulationCraftTextEdit();
  simulateText->setLineWrapMode( QPlainTextEdit::NoWrap );
  simulateText->setPlainText( defaultSimulateText() );
  mainTab->addTab( simulateText, "Simulate" );
}

void SimulationCraftWindow::createOverridesTab()
{
  overridesText = new SimulationCraftTextEdit();
  overridesText->setLineWrapMode( QPlainTextEdit::NoWrap );
  //overridesText->document()->setDefaultFont( QFont( "fixed" ) );
  overridesText->setPlainText( "# User-specified persistent global and player parms will set here.\n" );
  mainTab->addTab( overridesText, "Overrides" );
}

void SimulationCraftWindow::createLogTab()
{
  logText = new QPlainTextEdit();
  logText->setLineWrapMode( QPlainTextEdit::NoWrap );
  //logText->document()->setDefaultFont( QFont( "fixed" ) );
  logText->setReadOnly( true );
  logText->setPlainText( "Look here for error messages and simple text-only reporting.\n" );
  mainTab->addTab( logText, "Log" );
}

void SimulationCraftWindow::createHelpTab()
{
  helpView = new SimulationCraftWebView( this );
  helpView->setUrl( QUrl( "http://code.google.com/p/simulationcraft-swtor/wiki/StartersGuide" ) );
  mainTab->addTab( helpView, "Help" );
}

void SimulationCraftWindow::createResultsTab()
{
  resultsTab = new QTabWidget();
  resultsTab->setTabsClosable( true );
  connect( resultsTab, SIGNAL( currentChanged( int ) ),    this, SLOT( resultsTabChanged( int ) )      );
  connect( resultsTab, SIGNAL( tabCloseRequested( int ) ), this, SLOT( resultsTabCloseRequest( int ) ) );
  mainTab->addTab( resultsTab, "Results" );
}

void SimulationCraftWindow::createSiteTab()
{
  siteView = new SimulationCraftWebView( this );
  siteView->setUrl( QUrl( "http://code.google.com/p/simulationcraft-swtor/" ) );
  mainTab->addTab( siteView, "Site" );
}

void SimulationCraftWindow::createToolTips()
{
  versionChoice->setToolTip( "Live: Use mechanics on Live servers\n"
                             "PTR:  Use mechanics on PTR server\n"
                             "Both: Create Evil Twin with PTR mechanics" );

  iterationsChoice->setToolTip( "100:    Fast and Rough\n"
                                "1000:   Sufficient for DPS Analysis\n"
                                "10000:  Recommended for Scale Factor Generation\n"
                                "25000:  Use if 10,000 isn't enough for Scale Factors\n"
                                "50000:  If you're patient\n"
                                "100000: Showing off" );

  fightLengthChoice->setToolTip( "For custom fight lengths use max_time=seconds." );

  fightVarianceChoice->setToolTip( "Varying the fight length over a given spectrum improves\n"
                                   "the analysis of trinkets and abilities with long cooldowns." );

  fightStyleChoice->setToolTip( "Patchwerk:\n"
                                "    Tank-n-Spank\n"
                                "HelterSkelter:\n"
                                "    Movement, Stuns, Interrupts,\n"
                                "    Target-Switching (every 2min)\n"
                                "Ultraxion:\n"
                                "    Periodic Stuns, Raid Damage\n"
                                "LightMovement:\n"
                                "    7s Movement, 85s CD,\n"
                                "    10% into the fight until 20% before the end\n"
                                "HeavyMovement:\n"
                                "    4s Movement, 10s CD,\n"
                                "    beginning 10s into the fight" );

  targetRaceChoice->setToolTip( "Race of the target and any adds." );

  //targetLevelChoice->setToolTip( "Level of the target and any adds." );

  playerSkillChoice->setToolTip( "Elite:       No mistakes.  No cheating either.\n"
                                 "Good:        Rare DoT-clipping and skipping high-priority abilities\n"
                                 "Avearge:     Occasional DoT-clipping and skipping high-priority abilities\n"
                                 "Fire-is-Hot: Frequent DoT-clipping and skipping high-priority abilities." );

  threadsChoice->setToolTip( "Match the number of CPUs for optimal performance.\n"
                             "Most modern desktops have at least two CPU cores." );

  //armoryRegionChoice->setToolTip( "United States, Europe, Taiwan, China, Korea" );

  //armorySpecChoice->setToolTip( "Controls which Talent/Glyph specification is used when importing profiles from the Armory." );

  defaultRoleChoice->setToolTip( "Specify the character role during import to ensure correct action priority list." );

  reportpetsChoice->setToolTip( "Specify if pets get reported separately in detail." );

  printstyleChoice->setToolTip( "Specify html report print style." );


  statisticslevel_Choice->setToolTip( "Statistics Level determines how much information will be collected during simulation.\n"
                                      " Higher Statistics Level require more memory.\n"
                                      " Level 0: Only Simulation Length data is collected.\n"
                                      " Level 1: DPS/HPS data is collected.\n"
                                      " Level 2: Player Fight Length, Death Time, DPS(e), HPS(e), DTPS, HTPS, DMG, HEAL data is collected.\n"
                                      " Level 3: Ability Amount and  portion APS is collected.\n"
                                      " *Warning* Levels above 3 are usually not recommended when simulating more than 1 player.\n"
                                      " Level 8: Ability Result Amount, Count and average Amount is collected. ");

  debugChoice->setToolTip( "When a log is generated, only one iteration is used.\n"
                           "Gory details are very gory.  No documentation will be forthcoming.\n"
                           "Due to the forced single iteration, no scale factor calculation." );

  latencyChoice->setToolTip( "World Lag is the equivalent of the 'world lag' shown in the WoW Client.\n"
                             "It is currently used to extend the cooldown duration of user executable abilities "
                             " that have a cooldown.\n"
                             "Each setting adds an amount of 'lag' with a default standard deviation of 10%:\n"
                             "    'Low'   : 100ms\n"
                             "    'Medium': 300ms\n"
                             "    'High'  : 500ms" );

  //backButton->setToolTip( "Backwards" );
  //forwardButton->setToolTip( "Forwards" );

  plotsPointsChoice -> setToolTip( "The number of points that will appear on the graph" );
  plotsStepChoice -> setToolTip( "The delta between two points of the graph.\n"
                                 "The deltas on the horizontal axis will be within the [-points * steps / 2 ; +points * steps / 2] interval" );

  reforgePlotAmountChoice -> setToolTip( "The maximum amount to reforge per stat." );
  reforgePlotStepChoice -> setToolTip( "The stat difference between two points.\n"
                                       "It's NOT the number of steps: a lower value will generate more points!" );
}

#ifdef SC_PAPERDOLL
void SimulationCraftWindow::createPaperdoll()
{
  QWidget* paperdollTab = new QWidget( this );
  QHBoxLayout* paperdollMainLayout = new QHBoxLayout();
  paperdollMainLayout -> setAlignment( Qt::AlignLeft | Qt::AlignTop );
  paperdollTab -> setLayout( paperdollMainLayout );

  PaperdollProfile* profile = new PaperdollProfile();
  Paperdoll* paperdoll = new Paperdoll( profile, paperdollTab );
  ItemSelectionWidget* items = new ItemSelectionWidget( profile, paperdollTab );

  paperdollMainLayout -> addWidget( items );
  paperdollMainLayout -> addWidget( paperdoll );


  mainTab -> addTab( paperdollTab, "Paperdoll" );
}
#endif

#if 0
void SimulationCraftWindow::createItemDataSourceSelector( QFormLayout* layout )
{
  itemDbOrder = new QListWidget( this );
  itemDbOrder -> setDragDropMode( QAbstractItemView::InternalMove );
  itemDbOrder -> setResizeMode( QListView::Fixed );
  itemDbOrder -> setSelectionRectVisible( false );
  itemDbOrder -> setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
  itemDbOrder -> setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  OptionEntry* item_sources = getItemSourceOptions();

  for ( int i = 0; item_sources[ i ].label; i++ )
  {
    QListWidgetItem* item = new QListWidgetItem( item_sources[ i ].label );
    item -> setData( Qt::UserRole, QVariant( item_sources[ i ].option ) );
    item -> setToolTip( item_sources[ i ].tooltip );
    itemDbOrder -> addItem( item );
  }

  itemDbOrder -> setFixedHeight( ( itemDbOrder -> model() -> rowCount() + 1 ) * itemDbOrder -> sizeHintForRow( 0 ) );

  layout->addRow( "Item Source Order", itemDbOrder );
}
#endif

void SimulationCraftWindow::updateVisibleWebView( SimulationCraftWebView* wv )
{
  visibleWebView = wv;
  if ( wv )
  {
    progressBar->setFormat( "%p%" );
    progressBar->setValue( wv->progress );
    cmdLine->setText( wv->url().toString() );
  }
  else
  {
    progressBar->setFormat( simPhase.c_str() );
    progressBar->setValue( simProgress );
    cmdLine->setText( "" );
  }
}

// ==========================================================================
// Sim Initialization
// ==========================================================================

sim_t* SimulationCraftWindow::initSim()
{
  if ( ! sim )
  {
    sim.reset( new sim_t );
    sim -> input_is_utf8 = true; // Presume GUI input is always UTF-8
    sim -> output_file = fopen( "simc_log.txt", "w" );
    sim -> report_progress = 0;
#if SC_USE_PTR
    sim -> parse_option( "ptr", ( ( versionChoice->currentIndex() == 1 ) ? "1" : "0" ) );
#endif
    sim -> parse_option( "debug", ( (   debugChoice->currentIndex() == 2 ) ? "1" : "0" ) );
  }
  return get_pointer( sim );
}

void SimulationCraftWindow::deleteSim()
{
  if ( sim )
  {
    fclose( sim -> output_file );
    sim.reset();
    QFile logFile( "simc_log.txt" );
    logFile.open( QIODevice::ReadOnly );
    logText->appendPlainText( logFile.readAll() );
    logText->moveCursor( QTextCursor::End );
    logFile.close();
  }
}

// ==========================================================================
// Import
// ==========================================================================
void ImportThread::importMrRobot()
{
  QStringList parts = QUrl( url ).path().split( '/' );
  QString id = parts[ parts.size() - 1 ];

  if ( id.isEmpty() )
  {
    fprintf( sim->output_file, "Unable to determine Profile ID!\n" );
  }
  else
  {
    // Windows 7 64bit somehow cannot handle straight toStdString() conversion, so
    // do it in a silly way as a workaround for now.
    player = mrrobot::download_player( sim, id.toUtf8().constData() );
  }
}

void ImportThread::run()
{
  cache::advance_era();

  switch ( tab )
  {
  case TAB_MR_ROBOT: importMrRobot(); break;
  default: assert( 0 ); break;
  }

  if ( player )
  {
    player -> role = util_t::parse_role_type( mainWindow->defaultRoleChoice->currentText().toUtf8().constData() );

    if ( sim->init() )
      profile = QString::fromUtf8( player->create_profile().c_str() );
    else
      player = 0;
  }
}

void SimulationCraftWindow::startImport( int tab, const QString& url )
{
  if ( sim )
  {
    sim -> cancel();
    return;
  }
  simProgress = 0;
  mainButton->setText( "Cancel!" );
  importThread->start( initSim(), tab, url );
  simulateText->setPlainText( defaultSimulateText() );
  mainTab->setCurrentIndex( TAB_SIMULATE );
  timer->start( 500 );
}

void SimulationCraftWindow::importFinished()
{
  timer->stop();
  simPhase = "%p%";
  simProgress = 100;
  progressBar->setFormat( simPhase.c_str() );
  progressBar->setValue( simProgress );
  if ( importThread->player )
  {
    simulateText->setPlainText( importThread->profile );
    simulateTextHistory.add( importThread->profile );

    QString label = QString::fromUtf8( importThread->player->name_str.c_str() );
    while ( label.size() < 20 ) label += ' ';
    label += QString::fromUtf8( importThread->player->origin_str.c_str() );

    bool found = false;
    for ( int i=0; i < historyList->count() && ! found; i++ )
      if ( historyList->item( i )->text() == label )
        found = true;

    if ( ! found )
    {
      QListWidgetItem* item = new QListWidgetItem( label );
      //item->setFont( QFont( "fixed" ) );

      historyList->addItem( item );
      historyList->sortItems();
    }

    mainButton->setText( "Simulate!" );
    mainTab->setCurrentIndex( TAB_SIMULATE );
  }
  else
  {
    simulateText->setPlainText( QString( "# Unable to generate profile from: " ) + importThread->url );
    mainButton->setText( "Save!" );
    mainTab->setCurrentIndex( TAB_LOG );
  }
  deleteSim();
}

// ==========================================================================
// Simulate
// ==========================================================================

void SimulateThread::run()
{
  QByteArray utf8_profile = options.toUtf8();
  QFile file( "simc_gui.simc" );
  if ( file.open( QIODevice::WriteOnly ) )
  {
    file.write( utf8_profile );
    file.close();
  }

  sim -> html_file_str = "simc_report.html";
  sim -> xml_file_str  = "simc_report.xml";

  QStringList stringList = options.split( '\n', QString::SkipEmptyParts );

  int argc = stringList.count() + 1;
  char** argv = new char*[ argc ];

  QList<QByteArray> lines;
  lines.append( "simc" );
  for ( int i=1; i < argc; i++ )
  {
    lines.append( stringList[ i-1 ].toUtf8().constData() );
  }
  for ( int i=0; i < argc; i++ ) argv[ i ] = lines[ i ].data();

  if ( sim -> parse_options( argc, argv ) )
  {
    success = sim -> execute();

    if ( success )
    {
      sim -> scaling -> analyze();
      sim -> plot -> analyze();
      sim -> reforge_plot -> analyze();
      report_t::print_suite( sim );
    }
  }
}

void SimulationCraftWindow::startSim()
{
  if ( sim )
  {
    sim -> cancel();
    return;
  }
  optionsHistory.add( encodeOptions() );
  optionsHistory.current_index = 0;
  if ( simulateText->toPlainText() != defaultSimulateText() )
  {
    simulateTextHistory.add(  simulateText->toPlainText() );
  }
  overridesTextHistory.add( overridesText->toPlainText() );
  simulateCmdLineHistory.add( cmdLine->text() );
  simProgress = 0;
  mainButton->setText( "Cancel!" );
  simulateThread->start( initSim(), mergeOptions() );
  // simulateText->setPlainText( defaultSimulateText() );
  cmdLineText = "";
  cmdLine->setText( cmdLineText );
  timer->start( 100 );
}

QString SimulationCraftWindow::mergeOptions()
{
  QString options = "";
#if SC_USE_PTR
  options += "ptr="; options += ( ( versionChoice->currentIndex() == 1 ) ? "1" : "0" ); options += "\n";
#endif
#if 0
  if ( itemDbOrder -> count() > 0 )
  {
    options += "item_db_source=";
    for ( int i = 0; i < itemDbOrder -> count(); i++ )
    {
      QListWidgetItem *it = itemDbOrder -> item( i );
      options += it -> data( Qt::UserRole ).toString();
      if ( i < itemDbOrder -> count() - 1 )
        options += "/";
    }
    options += "\n";
  }
#endif
  options += "iterations=" + iterationsChoice->currentText() + "\n";
  if ( iterationsChoice->currentText() == "10000" )
  {
    options += "dps_plot_iterations=1000\n";
  }
  const char *world_lag[] = { "0.1", "0.3", "0.5" };
  options += "default_world_lag=";
  options += world_lag[ latencyChoice->currentIndex() ];
  options += "\n";
  options += "max_time=" + fightLengthChoice->currentText() + "\n";
  options += "vary_combat_length=";
  const char *variance[] = { "0.0", "0.1", "0.2" };
  options += variance[ fightVarianceChoice->currentIndex() ];
  options += "\n";
  options += "fight_style=" + fightStyleChoice->currentText() + "\n";

#if 0
  static const char* const targetlevel[] = { "53", "52", "50" };
  options += "target_level=";
  options += targetlevel[ targetLevelChoice->currentIndex() ];
  options += "\n";
#endif

  options += "target_race=" + targetRaceChoice->currentText() + "\n";
  options += "default_skill=";
  static const char * const skill[] = { "1.0", "0.9", "0.75", "0.50" };
  options += skill[ playerSkillChoice->currentIndex() ];
  options += "\n";
  options += "threads=" + threadsChoice->currentText() + "\n";
  options += "optimal_raid=0\n";
  QList<QAbstractButton*> buttons = buffsButtonGroup->buttons();
  OptionEntry* buffs = getBuffOptions();
  for ( int i=1; buffs[ i ].label; i++ )
  {
    options += buffs[ i ].option;
    options += "=";
    options += buttons.at( i )->isChecked() ? "1" : "0";
    options += "\n";
  }
  buttons = debuffsButtonGroup->buttons();
  OptionEntry* debuffs = getDebuffOptions();
  for ( int i=1; debuffs[ i ].label; i++ )
  {
    options += debuffs[ i ].option;
    options += "=";
    options += buttons.at( i )->isChecked() ? "1" : "0";
    options += "\n";
  }
  buttons = scalingButtonGroup->buttons();
  OptionEntry* scaling = getScalingOptions();
  for ( int i=2; scaling[ i ].label; i++ )
  {
    if ( buttons.at( i )->isChecked() )
    {
      options += "calculate_scale_factors=1\n";
      break;
    }
  }
  if( buttons.at( 1 )->isChecked() ) options += "positive_scale_delta=1\n";
  //if( buttons.at( 15 )->isChecked() || buttons.at( 17 )->isChecked() ) options += "weapon_speed_scale_factors=1\n";
  // FIXME: Use the correct weapon scale factor position once all stats are implemented
  options += "scale_only=none";
  for ( int i=2; scaling[ i ].label; i++ )
  {
    if ( buttons.at( i )->isChecked() )
    {
      options += ",";
      options += scaling[ i ].option;
    }
  }
  options += "\n";
  options += "dps_plot_stat=none";
  buttons = plotsButtonGroup->buttons();
  OptionEntry* plots = getPlotOptions();
  for ( int i=0; plots[ i ].label; i++ )
  {
    if ( buttons.at( i )->isChecked() )
    {
      options += ",";
      options += plots[ i ].option;
    }
  }
  options += "\n";
  options += "dps_plot_points=" + plotsPointsChoice -> currentText() + "\n";
  options += "dps_plot_step=" + plotsStepChoice -> currentText() + "\n";
  options += "reforge_plot_stat=none";
  buttons = reforgeplotsButtonGroup->buttons();
  OptionEntry* reforgeplots = getReforgePlotOptions();
  for ( int i=0; reforgeplots[ i ].label; i++ )
  {
    if ( buttons.at( i )->isChecked() )
    {
      options += ",";
      options += reforgeplots[ i ].option;
    }
  }
  options += "\n";
  options += "reforge_plot_amount=" + reforgePlotAmountChoice -> currentText() + "\n";
  options += "reforge_plot_step=" + reforgePlotStepChoice -> currentText() + "\n";
  options += "reforge_plot_output_file=reforge_plot.csv\n"; // This should be set in the gui if possible
  if ( statisticslevel_Choice->currentIndex() >= 0 )
  {
    options += "statistics_level=" + statisticslevel_Choice->currentText() + "\n";
  }
  options += "\n";
  options += simulateText->toPlainText();
  options += "\n";
  options += overridesText->toPlainText();
  options += "\n";
  options += cmdLine->text();
  options += "\n";
#if SC_USE_PTR
  if ( versionChoice->currentIndex() == 2 )
  {
    options += "ptr=1\n";
    options += "copy=EvilTwinPTR\n";
    options += "ptr=0\n";
  }
#endif
  if ( debugChoice->currentIndex() != 0 )
  {
    options += "log=1\n";
    options += "scale_only=none\n";
    options += "dps_plot_stat=none\n";
  }
  if ( reportpetsChoice->currentIndex() != 1 )
  {
    options += "report_pets_separately=1\n";
  }
  if ( printstyleChoice->currentIndex() == 1 )
  {
    options += "print_styles=1\n";
  }
  return options;
}

void SimulationCraftWindow::simulateFinished()
{
  timer->stop();
  simPhase = "%p%";
  simProgress = 100;
  progressBar->setFormat( simPhase.c_str() );
  progressBar->setValue( simProgress );
  QFile file( sim -> html_file_str.c_str() );
  deleteSim();
  if ( ! simulateThread->success )
  {
    logText->appendPlainText( "Simulation failed!\n" );
    logText->moveCursor( QTextCursor::End );
    mainTab->setCurrentIndex( TAB_LOG );
  }
  else if ( file.open( QIODevice::ReadOnly ) )
  {
    // Html results will _ALWAYS_ be utf-8, regardless of the input encoding
    // so read them to the WebView through QTextStream
    QTextStream s( &file );
    s.setCodec( "UTF-8" );
    QString resultsName = QString( "Results %1" ).arg( ++simResults );
    SimulationCraftWebView* resultsView = new SimulationCraftWebView( this );
    QString resultHtml = s.readAll();
//    resultsHtml.append( resultHtml );
    resultsView->setProperty( "resultHTML", QVariant(resultHtml) );
    resultsView->setHtml( resultHtml );
    resultsTab->addTab( resultsView, resultsName );
    resultsTab->setCurrentWidget( resultsView );
    resultsView->setFocus();
    mainTab->setCurrentIndex( debugChoice->currentIndex() ? TAB_LOG : TAB_RESULTS );
  }
  else
  {
    logText->appendPlainText( "Unable to open html report!\n" );
    mainTab->setCurrentIndex( TAB_LOG );
  }
}

// ==========================================================================
// Save Results
// ==========================================================================

void SimulationCraftWindow::saveLog()
{
  logCmdLineHistory.add( cmdLine->text() );

  QFile file( cmdLine->text() );

  if ( file.open( QIODevice::WriteOnly ) )
  {
    file.write( logText->toPlainText().toUtf8() );
    file.close();
  }

  logText->appendPlainText( QString( "Log saved to: %1\n" ).arg( cmdLine->text() ) );
}

void SimulationCraftWindow::saveResults()
{
  int index = resultsTab->currentIndex();
  if ( index < 0 ) return;

  if (visibleWebView == NULL) return;

  resultsCmdLineHistory.add( cmdLine->text() );

  QFile file( cmdLine->text() );

  if ( file.open( QIODevice::WriteOnly ) )
  {
    file.write( visibleWebView->property("resultHTML").toString().toUtf8() );
    file.close();
  }

  logText->appendPlainText( QString( "Results saved to: %1\n" ).arg( cmdLine->text() ) );
}

// ==========================================================================
// Window Events
// ==========================================================================

void SimulationCraftWindow::closeEvent( QCloseEvent* e )
{
  saveHistory();
  if ( mrRobotBuilderView ) mrRobotBuilderView->stop();
  QCoreApplication::quit();
  e->accept();
}

void SimulationCraftWindow::cmdLineTextEdited( const QString& s )
{
  switch ( mainTab->currentIndex() )
  {
  case TAB_WELCOME:   cmdLineText = s; break;
  case TAB_OPTIONS:   cmdLineText = s; break;
  case TAB_SIMULATE:  cmdLineText = s; break;
  case TAB_OVERRIDES: cmdLineText = s; break;
  case TAB_HELP:      cmdLineText = s; break;
  case TAB_SITE:      cmdLineText = s; break;
  case TAB_LOG:       logFileText = s; break;
  case TAB_RESULTS:   resultsFileText = s; break;
  case TAB_IMPORT:    break;
  }
}

void SimulationCraftWindow::cmdLineReturnPressed()
{
  if ( mainTab->currentIndex() == TAB_IMPORT )
  {
    if ( mrRobotBuilderView && cmdLine->text().count( "askmorrobot.com" ) )
    {
      mrRobotBuilderView->setUrl( QUrl::fromUserInput( cmdLine->text() ) );
      importTab->setCurrentIndex( TAB_MR_ROBOT );
      return;
    }
  }

  if ( ! sim ) mainButtonClicked( true );
}

void SimulationCraftWindow::mainButtonClicked( bool /* checked */ )
{
  switch ( mainTab->currentIndex() )
  {
  case TAB_WELCOME:   startSim(); break;
  case TAB_OPTIONS:   startSim(); break;
  case TAB_SIMULATE:  startSim(); break;
  case TAB_OVERRIDES: startSim(); break;
  case TAB_HELP:      startSim(); break;
  case TAB_SITE:      startSim(); break;
  case TAB_IMPORT:
    switch ( importTab->currentIndex() )
    {
    case TAB_MR_ROBOT: startImport( TAB_MR_ROBOT, cmdLine->text() ); break;
    }
    break;
  case TAB_LOG: saveLog(); break;
  case TAB_RESULTS: saveResults(); break;
  }
}

void SimulationCraftWindow::backButtonClicked( bool /* checked */ )
{
  if ( visibleWebView )
  {
    if ( mainTab->currentIndex() == TAB_RESULTS && ! visibleWebView->history()->canGoBack() )
    {
//        visibleWebView->setHtml( resultsHtml[ resultsTab->indexOf( visibleWebView ) ] );
      visibleWebView->setHtml( visibleWebView->property("resultHTML").toString() );

      QWebHistory* h = visibleWebView->history();
      visibleWebView->history()->clear(); // This is not appearing to work.
      h->setMaximumItemCount( 0 );
      h->setMaximumItemCount( 100 );
    }
    else
    {
      visibleWebView->back();
    }
    visibleWebView->setFocus();
  }
  else
  {
    switch ( mainTab->currentIndex() )
    {
    case TAB_WELCOME:   break;
    case TAB_OPTIONS:   decodeOptions( optionsHistory.backwards() ); break;
    case TAB_IMPORT:    break;
    case TAB_SIMULATE:   simulateText->setPlainText(  simulateTextHistory.backwards() );  simulateText->setFocus(); break;
    case TAB_OVERRIDES: overridesText->setPlainText( overridesTextHistory.backwards() ); overridesText->setFocus(); break;
    case TAB_LOG:       break;
    case TAB_RESULTS:   break;
    }
  }
}

void SimulationCraftWindow::forwardButtonClicked( bool /* checked */ )
{
  if ( visibleWebView )
  {
    visibleWebView->forward();
    visibleWebView->setFocus();
  }
  else
  {
    switch ( mainTab->currentIndex() )
    {
    case TAB_WELCOME:   break;
    case TAB_OPTIONS:   decodeOptions( optionsHistory.forwards() ); break;
    case TAB_IMPORT:    break;
    case TAB_SIMULATE:   simulateText->setPlainText(  simulateTextHistory.forwards() );  simulateText->setFocus(); break;
    case TAB_OVERRIDES: overridesText->setPlainText( overridesTextHistory.forwards() ); overridesText->setFocus(); break;
    case TAB_LOG:       break;
    case TAB_RESULTS:   break;
    }
  }
}

void SimulationCraftWindow::mainTabChanged( int index )
{
  visibleWebView = 0;
  switch ( index )
  {
  case TAB_WELCOME:   cmdLine->setText( cmdLineText ); mainButton->setText( sim ? "Cancel!" : "Simulate!" ); break;
  case TAB_OPTIONS:   cmdLine->setText( cmdLineText ); mainButton->setText( sim ? "Cancel!" : "Simulate!" ); break;
  case TAB_SIMULATE:  cmdLine->setText( cmdLineText ); mainButton->setText( sim ? "Cancel!" : "Simulate!" ); break;
  case TAB_OVERRIDES: cmdLine->setText( cmdLineText ); mainButton->setText( sim ? "Cancel!" : "Simulate!" ); break;
  case TAB_HELP:      cmdLine->setText( cmdLineText ); mainButton->setText( sim ? "Cancel!" : "Simulate!" ); break;
  case TAB_LOG:       cmdLine->setText( logFileText ); mainButton->setText( "Save!" ); break;
  case TAB_IMPORT:
    mainButton->setText( sim ? "Cancel!" : "Import!" );
    importTabChanged( importTab->currentIndex() );
    break;
  case TAB_RESULTS:
    mainButton->setText( "Save!" );
    resultsTabChanged( resultsTab->currentIndex() );
    break;
  case TAB_SITE:
    cmdLine->setText( cmdLineText );
    mainButton->setText( sim ? "Cancel!" : "Simulate!" );
    updateVisibleWebView( siteView );
    break;
  case TAB_PAPERDOLL:
    break;
  default: assert( 0 );
  }
  if ( visibleWebView )
  {
    progressBar->setFormat( "%p%" );
  }
  else
  {
    progressBar->setFormat( simPhase.c_str() );
    progressBar->setValue( simProgress );
  }
}

void SimulationCraftWindow::importTabChanged( int index )
{
  if( index == TAB_BIS  ||
      index == TAB_CUSTOM  ||
      index == TAB_HISTORY )
  {
    visibleWebView = 0;
    progressBar->setFormat( simPhase.c_str() );
    progressBar->setValue( simProgress );
    cmdLine->setText( "" );
  }
  else
  {
    updateVisibleWebView( dynamic_cast<SimulationCraftWebView*>( importTab->widget( index ) ) );
  }
}

void SimulationCraftWindow::resultsTabChanged( int index )
{
  if ( index < 0 )
  {
    cmdLine->setText( "" );
  }
  else
  {
    updateVisibleWebView( dynamic_cast<SimulationCraftWebView*>( resultsTab->widget( index ) ) );
    QString s = visibleWebView->url().toString();
    if ( s == "about:blank" ) s = resultsFileText;
    cmdLine->setText( s );
  }
}

void SimulationCraftWindow::resultsTabCloseRequest( int index )
{
  resultsTab->removeTab( index );
//  resultsHtml.removeAt( index );
}

void SimulationCraftWindow::historyDoubleClicked( QListWidgetItem* item )
{
  QString text = item->text();
  QString url = text.section( ' ', 1, 1, QString::SectionSkipEmpty );

  if ( mrRobotBuilderView )
  {
    mrRobotBuilderView->setUrl( QUrl::fromEncoded( url.toUtf8() ) );
    importTab->setCurrentIndex( TAB_MR_ROBOT );
  }
}

void SimulationCraftWindow::bisDoubleClicked( QTreeWidgetItem* item, int /* col */ )
{
  QString profile = item->text( 1 );

  QString s = "Unable to import profile "; s += profile;

  QFile file( profile );
  if ( file.open( QIODevice::ReadOnly ) )
  {
    s = file.readAll();
    file.close();
  }

  simulateText->setPlainText( s );
  simulateTextHistory.add( s );
  mainTab->setCurrentIndex( TAB_SIMULATE );
  simulateText->setFocus();
}

void SimulationCraftWindow::allBuffsChanged( bool checked )
{
  QList<QAbstractButton*> buttons = buffsButtonGroup->buttons();
  int count = buttons.count();
  for ( int i=1; i < count; i++ )
  {
    buttons.at( i ) -> setChecked( checked );
  }
}

void SimulationCraftWindow::allDebuffsChanged( bool checked )
{
  QList<QAbstractButton*> buttons = debuffsButtonGroup->buttons();
  int count = buttons.count();
  for ( int i=1; i < count; i++ )
  {
    buttons.at( i ) -> setChecked( checked );
  }
}

void SimulationCraftWindow::allScalingChanged( bool checked )
{
  QList<QAbstractButton*> buttons = scalingButtonGroup->buttons();
  int count = buttons.count();
  for ( int i=2; i < count; i++ )
  {
    buttons.at( i ) -> setChecked( checked );
  }
}

#if 0
void SimulationCraftWindow::armoryRegionChanged( const QString& region )
{
  assert( 0 );
  QString importUrl = "http://" + region + ".battle.net/wow/en";

  battleNetView->stop();
  battleNetView->setUrl( QUrl( importUrl ) );
}
#endif

void PersistentCookieJar::save()
{
  QFile file( fileName );
  if ( file.open( QIODevice::WriteOnly ) )
  {
    QDataStream out( &file );
    QList<QNetworkCookie> cookies = allCookies();
    qint32 count = ( qint32 ) cookies.count();
    out << count;
    for ( int i=0; i < count; i++ )
    {
      const QNetworkCookie& c = cookies.at( i );
      out << c.name();
      out << c.value();
    }
    file.close();
  }
}

void PersistentCookieJar::load()
{
  QFile file( fileName );
  if ( file.open( QIODevice::ReadOnly ) )
  {
    QDataStream in( &file );
    QList<QNetworkCookie> cookies;
    qint32 count;
    in >> count;
    for ( int i=0; i < count; i++ )
    {
      QByteArray name, value;
      in >> name;
      in >> value;
      cookies.append( QNetworkCookie( name, value ) );
    }
    setAllCookies( cookies );
    file.close();
  }
}
