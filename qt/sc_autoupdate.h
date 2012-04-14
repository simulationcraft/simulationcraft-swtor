#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QString>

class AutoUpdater
{
public:
  virtual ~AutoUpdater() {}
  virtual void checkForUpdates() = 0;
};

#ifdef Q_WS_MAC

class SparkleAutoUpdater : public AutoUpdater
{
public:
  SparkleAutoUpdater( const QString& url );
  ~SparkleAutoUpdater();

  void checkForUpdates();

private:
  class Private;
  std::unique_ptr<Private> d;
};


class CocoaInitializer
{
public:
  CocoaInitializer();
  ~CocoaInitializer();

private:
  class Private;
  std::unique_ptr<Private> d;
};

#endif

#endif
