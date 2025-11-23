#include "appinit.h"

#include <QApplication>

#include <QCefContext.h>

#include "WebSocketServer.h"
#include "Widget.h"
#include "quihelper.h"
#include "savelog.h"

#include <QFile>

int
main(int argc, char* argv[])
{
  //QApplication a(argc, argv);

    QUIHelper::initMain();
  QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/qss/blacksoft/data.ico"));
  AppInit::Instance()->start();

  QUIHelper::setFont();
  QUIHelper::setCode();

  //加载样式表
  QFile file(":/qss/blacksoft.css");
  if (file.open(QFile::ReadOnly)) {
    QString qss = QLatin1String(file.readAll());
    QString paletteColor = qss.mid(20, 7);
    qApp->setPalette(QPalette(QColor(paletteColor)));
    qApp->setStyleSheet(qss);
    file.close();
  }

  //frmMain w;
  //w.resize(800, 600);
  //QUIHelper::setFormInCenter(&w);
  //w.show();

  //return a.exec();

  // build QCefConfig
  QCefConfig config;
  config.setUserAgent("QCefViewTest");
  config.setLogLevel(QCefConfig::LOGSEVERITY_DEFAULT);
  config.setBridgeObjectName("CallBridge");
  config.setRemoteDebuggingPort(9000);
  config.setBackgroundColor(Qt::lightGray);

  // add command line args
  // config.addCommandLineSwitch("allow-universal-access-from-files");
  config.addCommandLineSwitch("enable-media-stream");
  config.addCommandLineSwitch("use-mock-keychain");
  config.addCommandLineSwitch("allow-file-access-from-files");
  config.addCommandLineSwitch("disable-spell-checking");
  config.addCommandLineSwitch("disable-site-isolation-trials");
  config.addCommandLineSwitch("enable-aggressive-domstorage-flushing");
  config.addCommandLineSwitchWithValue("renderer-process-limit", "1");
  config.addCommandLineSwitchWithValue("disable-features", "BlinkGenPropertyTrees,TranslateUI,site-per-process");

  // initialize QCefContext instance with config
  QCefContext cefContext(&a, argc, argv, &config);

  WebSocketServer::Instance();
  Widget w;
  w.resize(1000, 800);
  QUIHelper::setFormInCenter(&w);
  w.show();

  return a.exec();
}
