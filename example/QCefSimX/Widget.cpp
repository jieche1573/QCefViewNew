#include "Widget.h"

#include "iconhelper.h"
#include "quihelper.h"
#include "ui_Widget.h"

#include <QDesktopServices>
#include <QDir>
#include <QJsonDocument>
#include <QTimer>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <AircraftModelTest.h>

Widget::Widget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::Widget)
{
  ui->setupUi(this);
  Qt::WindowFlags m_flags = windowFlags();
  this->setWindowFlags(m_flags | Qt::WindowStaysOnTopHint);
  // setWindowState(Qt::WindowMaximized);
  QTimer::singleShot(500, this, [this]() {
    // on_btnMenu_Max_clicked();
    this->resize(1000, 800);
  });

  QUIHelper::setFramelessForm(this);
  //设置图标
  // IconHelper::setIcon(ui->labIco, 0xf099, 35);
  IconHelper::setIcon(ui->btnMenu_Min, 0xf068);
  IconHelper::setIcon(ui->btnMenu_Max, 0xf067);
  IconHelper::setIcon(ui->btnMenu_Close, 0xf00d);

  // ui->widgetMenu->setVisible(false);
  ui->widgetTitle->installEventFilter(this);
  ui->widgetTitle->setProperty("form", "title");
  ui->widgetTop->setProperty("nav", "top");

  QCefSetting settings;
  //settings.(downloadPath); // 设置下载目录
  settings.setAcceptLanguageList("zh-CN,zh");
  QDir dir = QCoreApplication::applicationDirPath();
  QString htmlPath = QString("file://") + QDir::toNativeSeparators(dir.filePath("dist/index.html"));

  m_cefView = new QCefView(htmlPath, &settings, this);
  // m_cefView->setDisablePopupContextMenu(true);
  ui->widget_main->setLayout(new QHBoxLayout(this));
  ui->widget_main->layout()->addWidget(m_cefView);
#ifdef _DEBUG
  QTimer::singleShot(2000, this, []() {
    // 启动时运行 AircraftModel 单元测试
    // 会自动检测表是否为空，如果为空则插入模拟数据
    AircraftModelTest::initTest();
   
  });
#endif // DEBUG
}

Widget::~Widget() {}

bool
Widget::eventFilter(QObject* watched, QEvent* event)
{
  QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
  if (mouseEvent->type() == QEvent::MouseButtonDblClick) {
    if (mouseEvent->button() == Qt::LeftButton) {
      if (mouseEvent->globalPos().y() - this->pos().y() < 48) {
        on_btnMenu_Max_clicked();
        return true;
      }
    }
  }
  return QWidget::eventFilter(watched, event);
}

void
Widget::on_btnMenu_Min_clicked()
{
  showMinimized();
}

void
Widget::on_btnMenu_Max_clicked()
{
  static bool max = false;
  static QRect location = this->geometry();

  if (max) {
    this->setGeometry(location);
  } else {
    location = this->geometry();
    this->setGeometry(QUIHelper::getScreenRect());
  }

  this->setProperty("canMove", max);
  max = !max;
}

void
Widget::on_btnMenu_Close_clicked()
{
  close();
}
