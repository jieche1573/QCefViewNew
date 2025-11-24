#include <QCefView.h>

#pragma region qt_headers
#include <QPainter>
#include <QPoint>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QtDebug>
#pragma endregion

#include <QCefContext.h>

#include "details/QCefEventPrivate.h"
#include "details/QCefViewPrivate.h"
#include "details/utils/CommonUtils.h"
#include <qmessagebox.h>
#include <qstandardpaths.h>
#include <QDir>
#include <qfiledialog.h>
#include <qprocess.h>

#if CEF_VERSION_MAJOR < 122
const QCefFrameId QCefView::MainFrameID = 0;
const QCefFrameId QCefView::AllFrameID = -1;
#else
const QCefFrameId QCefView::MainFrameID = "0";
const QCefFrameId QCefView::AllFrameID = "-1";
#endif

QCefView::QCefView(const QString& url,         //
                   const QCefSetting* setting, //
                   QWidget* parent,            // /*= nullptr*/
                   Qt::WindowFlags f           // /*= Qt::WindowFlags()*/
                   )
  : QWidget(parent, f)
  , d_ptr(new QCefViewPrivate(this, QCefContext::instance()->d_func(), setting ? setting->d_func() : nullptr),
          &QObject::deleteLater)
{
  // set attributes
  setAttribute(Qt::WA_DontCreateNativeAncestors);

  // track mouse
  setMouseTracking(true);

  // set focus policy
  setFocusPolicy(Qt::WheelFocus);

  // create browser
  d_ptr->createCefBrowser(url, setting ? setting->d_func() : nullptr);

  // set attributes to optimize the OSR rendering
  if (d_ptr->osr.pRenderer_) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // for hardware renderer, set the paint on screen attribute to disable
    // the default Qt paint event this will trigger the invocation of
    // the QWidget::paintEngine
    // NOTE: for software rendering, this flag must not be set on Linux platform
    if (d_ptr->osr.pRenderer_->isHardware()) {
      setAttribute(Qt::WA_PaintOnScreen);
    }
  }
}

QCefView::QCefView(QWidget* parent /*= 0*/, Qt::WindowFlags f /*= Qt::WindowFlags()*/)
  : QCefView("about:blank", nullptr, parent, f)
{
}

QCefView::~QCefView()
{
  qDebug() << this << "is being destructed";

  if (d_ptr) {
    // destroy under layer cef browser
    d_ptr->destroyCefBrowser();
    d_ptr.reset();
  }
}

void
QCefView::addLocalFolderResource(const QString& path, const QString& url, int priority /*= 0*/)
{
  Q_D(QCefView);

  d->addLocalFolderResource(path, url, priority);
}

void
QCefView::addArchiveResource(const QString& path,
                             const QString& url,
                             const QString& password /*= ""*/,
                             int priority /*= 0*/)
{
  Q_D(QCefView);

  d->addArchiveResource(path, url, password, priority);
}

QCefBrowserId
QCefView::browserId()
{
  Q_D(QCefView);

  return d->browserId();
}

void
QCefView::navigateToString(const QString& content)
{
  Q_D(QCefView);

  d->navigateToString(content);
}

void
QCefView::navigateToUrl(const QString& url)
{
  Q_D(QCefView);

  d->navigateToUrl(url);
}

bool
QCefView::browserCanGoBack()
{
  Q_D(QCefView);

  return d->browserCanGoBack();
}

bool
QCefView::browserCanGoForward()
{
  Q_D(QCefView);

  return d->browserCanGoForward();
}

void
QCefView::browserGoBack()
{
  Q_D(QCefView);

  d->browserGoBack();
}

void
QCefView::browserGoForward()
{
  Q_D(QCefView);

  d->browserGoForward();
}

bool
QCefView::browserIsLoading()
{
  Q_D(QCefView);

  return d->browserIsLoading();
}

void
QCefView::browserReload()
{
  Q_D(QCefView);

  d->browserReload();
}

void
QCefView::browserStopLoad()
{
  Q_D(QCefView);

  d->browserStopLoad();
}

bool
QCefView::triggerEvent(const QCefEvent& event)
{
  Q_D(QCefView);

  return d->triggerEvent(event.eventName(), event.d_func()->args, QCefView::MainFrameID);
}

bool
QCefView::triggerEvent(const QCefEvent& event, const QCefFrameId& frameId)
{
  Q_D(QCefView);

  return d->triggerEvent(event.eventName(), event.d_func()->args, frameId);
}

bool
QCefView::broadcastEvent(const QCefEvent& event)
{
  Q_D(QCefView);

  return d->triggerEvent(event.eventName(), event.d_func()->args, QCefView::AllFrameID);
}

bool
QCefView::responseQCefQuery(const QCefQuery& query)
{
  Q_D(QCefView);

  return d->responseQCefQuery(query);
}

bool
QCefView::executeJavascript(const QCefFrameId& frameId, const QString& code, const QString& url)
{
  Q_D(QCefView);

  return d->executeJavascript(frameId, code, url);
}

bool
QCefView::executeJavascriptWithResult(const QCefFrameId& frameId,
                                      const QString& code,
                                      const QString& url,
                                      const QString& context)
{
  Q_D(QCefView);

  return d->executeJavascriptWithResult(frameId, code, url, context);
}

bool
QCefView::setPreference(const QString& name, const QVariant& value, QString& error)
{
  Q_D(QCefView);

  return d->setPreference(name, value, error);
}

void
QCefView::setOSRFrameRate(int fps)
{
  Q_D(QCefView);

  return d->setOSRFrameRate(fps);
}

bool
QCefView::hasDevTools()
{
  Q_D(QCefView);

  return d->hasDevTools();
}

void
QCefView::showDevTools()
{
  Q_D(QCefView);

  d->showDevTools();
}

void
QCefView::closeDevTools()
{
  Q_D(QCefView);

  d->closeDevTools();
}

void
QCefView::setAllowDrag(bool allow)
{
  Q_D(QCefView);

  d->allowDrag_ = allow;
}

bool
QCefView::allowDrag() const
{
  Q_D(const QCefView);

  return d->allowDrag_;
}

void
QCefView::setZoomLevel(double level)
{
  Q_D(QCefView);

  d->setZoomLevel(level);
}

double
QCefView::zoomLevel()
{
  Q_D(QCefView);

  return d->zoomLevel();
}

QCefView*
QCefView::onNewBrowser(const QCefFrameId& sourceFrameId,
                       const QString& url,
                       const QString& name,
                       CefWindowOpenDisposition targetDisposition,
                       QRect& rect,
                       QCefSetting& settings)
{
  QCefView* popup = new QCefView(url, &settings, nullptr, Qt::WindowFlags());
  if (!popup) {
    // failed to create QCefView, cancel popup
    return nullptr;
  }

  // config the popup QCefView
  if (!name.isEmpty()) {
    popup->setWindowTitle(name);
  }
  popup->setAttribute(Qt::WA_DeleteOnClose, true);
  popup->resize(rect.size());
  popup->show();

  return popup;
}

bool
QCefView::onNewPopup(const QCefFrameId& frameId,
                     const QString& targetUrl,
                     QString& targetFrameName,
                     QCefView::CefWindowOpenDisposition targetDisposition,
                     QRect& rect,
                     QCefSetting& settings,
                     bool& disableJavascriptAccess)
{
  return false;
}
// 顶部静态状态（或移动到类成员）
static QSet<void*> g_handledByPointer;
static QSet<QString> g_handledBySuggestedName;
static QMutex g_handledMutex;
static QHash<void*, QSharedPointer<QCefDownloadItem>> g_activeDownloads;
static QMutex g_activeDownloadsMutex;

void
QCefView::onNewDownloadItem(const QSharedPointer<QCefDownloadItem>& item, const QString& suggestedName)
{
  if (!item)
    return;

  void* ptrKey = item.data();
  QString nameKey = suggestedName.isEmpty() ? QString() : suggestedName;

  // Debug 输出，方便排查（把这段保留并贴到日志来）
  qDebug() << "onNewDownloadItem called. ptr=" << ptrKey << ", suggestedName=" << nameKey
           << ", thread=" << QThread::currentThread();

  // 第一层：以指针去重（同一实例）
  {
    QMutexLocker locker(&g_handledMutex);
    if (g_handledByPointer.contains(ptrKey)) {
      qDebug() << "-> ignored by pointer (already handled):" << ptrKey;
      return;
    }
    // 第二层：以 suggestedName 去重（同名文件来自同一次下载的可能性）
    if (!nameKey.isEmpty() && g_handledBySuggestedName.contains(nameKey)) {
      qDebug() << "-> ignored by suggestedName (already handled):" << nameKey;
      // 仍然插入 pointer 标记以避免后续重复处理
      g_handledByPointer.insert(ptrKey);
      return;
    }

    // 先标记为 handled，防止并发重复弹窗
    g_handledByPointer.insert(ptrKey);
    if (!nameKey.isEmpty())
      g_handledBySuggestedName.insert(nameKey);
  }

  // Helper：开始下载并把引用保留
  auto doStart = [ptrKey, item](const QString& fullPath) {
    if (fullPath.isEmpty()) {
      // caller will handle cancellation
      return;
    }
    item->start(fullPath);
    QMutexLocker locker(&g_activeDownloadsMutex);
    g_activeDownloads.insert(ptrKey, item);
    qDebug() << "-> started download for ptr=" << ptrKey << " to " << fullPath;
  };

  // UI dialog lambda，需要在 lambda 中能访问 ptrKey 和 nameKey 以便用户取消时恢复标记
  auto showSaveDialogOnUiThread = [ptrKey, nameKey, doStart]() {
    QString defaultNameLocal =
      nameKey.isEmpty() ? QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".download" : nameKey;
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (defaultDir.isEmpty())
      defaultDir = QDir::homePath();
    QString defaultPath = QDir(defaultDir).filePath(defaultNameLocal);
    doStart(defaultPath);
    //QString savePath =
    //  QFileDialog::getSaveFileName(nullptr, QObject::tr("保存文件为"), defaultPath, QStringLiteral("*.*"));
    //if (!savePath.isEmpty()) {
    //  doStart(defaultPath);
    //} else {
    //  qDebug() << "-> user canceled save dialog for ptr=" << ptrKey;
    //  // 用户取消：撤销 handled 标记，允许后续重新触发（例如后续 onNewDownloadItem 再来一次时）
    //  QMutexLocker locker(&g_handledMutex);
    //  g_handledByPointer.remove(ptrKey);
    //  if (!nameKey.isEmpty())
    //    g_handledBySuggestedName.remove(nameKey);
    //}
  };

  // 若当前为 UI 线程则直接显示，否则用 invokeMethod 切回 UI 线程
  if (QThread::currentThread() == qApp->thread()) {
    showSaveDialogOnUiThread();
  } else {
    QMetaObject::invokeMethod(qApp, showSaveDialogOnUiThread, Qt::QueuedConnection);
  }
}

void
QCefView::onUpdateDownloadItem(const QSharedPointer<QCefDownloadItem>& item)
{
  if (!item)
    return;
  void* ptrKey = item.data();

  // Debug 输出
  qDebug() << "onUpdateDownloadItem called. ptr=" << ptrKey << ", thread=" << QThread::currentThread();

  if (item->isInProgress()) {
    int pct = item->percentComplete();
    if (pct >= 0)
      qDebug() << "-> downloading" << item->fullPath() << pct << "%";
    else
      qDebug() << "-> downloading (unknown size) " << item->fullPath();
  } else if (item->isComplete()) {
    qDebug() << "-> download complete:" << item->fullPath();
    {
      QMutexLocker locker(&g_activeDownloadsMutex);
      g_activeDownloads.remove(ptrKey);
    }
    {
      QMutexLocker locker(&g_handledMutex);
      g_handledByPointer.remove(ptrKey);
      // 不能安全地移除 suggestedName（不知道是谁占用），但我们也可以尝试根据 fullPath 解析名字并移除：
      QString fname = QFileInfo(item->fullPath()).fileName();
      if (!fname.isEmpty())
        g_handledBySuggestedName.remove(fname);
    }
    QMetaObject::invokeMethod(
      qApp,
      [path = item->fullPath()]() {
        QString program = "explorer.exe";
        QStringList arguments;
        arguments << "/select," + QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);

      },
      Qt::QueuedConnection);
  } else if (item->isCanceled()) {
    qDebug() << "-> download cancelled:" << item->fullPath();
    {
      QMutexLocker locker(&g_activeDownloadsMutex);
      g_activeDownloads.remove(ptrKey);
    }
    {
      QMutexLocker locker(&g_handledMutex);
      g_handledByPointer.remove(ptrKey);
    }
  } else {
    QString err = item->contentDisposition();
    if (!err.isEmpty()) {
      qDebug() << "-> download error:" << err << " path:" << item->fullPath();
      {
        QMutexLocker locker(&g_activeDownloadsMutex);
        g_activeDownloads.remove(ptrKey);
      }
      {
        QMutexLocker locker(&g_handledMutex);
        g_handledByPointer.remove(ptrKey);
      }
      QMetaObject::invokeMethod(
        qApp,
        [err]() { QMessageBox::warning(nullptr, QObject::tr("下载失败"), QObject::tr("下载失败：%1").arg(err)); },
        Qt::QueuedConnection);
    }
  }
}

bool
QCefView::onRequestCloseFromWeb()
{
  // delete self
  deleteLater();

  return true;
}

QVariant
QCefView::inputMethodQuery(Qt::InputMethodQuery query) const
{
  Q_D(const QCefView);

  if (d->isOSRModeEnabled_) {
    // OSR mode
    auto r = d->onViewInputMethodQuery(query);
    if (r.isValid())
      return r;
  }

  return QWidget::inputMethodQuery(query);
}

QPaintEngine*
QCefView::paintEngine() const
{
  Q_D(const QCefView);

  if (d->osr.pRenderer_ && d->osr.pRenderer_->isHardware()) {
    return nullptr;
  }

  return QWidget::paintEngine();
}

void
QCefView::paintEvent(QPaintEvent* event)
{
  Q_D(QCefView);

  d->onPaintEvent(event);
}

bool
QCefView::event(QEvent* event)
{
  Q_D(QCefView);

  switch (event->type()) {
    case QEvent::InputMethod: {
      QInputMethodEvent* e = static_cast<QInputMethodEvent*>(event);
      d->onViewInputMethodEvent(e);
    } break;
    case QEvent::Show: {
      d->onViewVisibilityChanged(true);
    } break;
    case QEvent::Hide: {
      d->onViewVisibilityChanged(false);
    } break;
    case QEvent::FocusIn: {
      d->onViewFocusChanged(true);
      if (!d->isOSRModeEnabled_) {
        return true;
      }
    } break;
    case QEvent::FocusOut: {
      d->onViewFocusChanged(false);
      if (!d->isOSRModeEnabled_) {
        return true;
      }
    } break;
    case QEvent::Move: {
      d->onViewMoved();
    } break;
    case QEvent::Resize: {
      QResizeEvent* e = static_cast<QResizeEvent*>(event);
      d->onViewSizeChanged(e->size(), e->oldSize());
    } break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
      QKeyEvent* e = static_cast<QKeyEvent*>(event);
      d->onViewKeyEvent(e);
      if (d->isOSRModeEnabled_ && (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab)) {
        return true;
      }
    } break;
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick: {
      QMouseEvent* e = static_cast<QMouseEvent*>(event);
      d->onViewMouseEvent(e);
    } break;
    case QEvent::Wheel: {
      QWheelEvent* e = static_cast<QWheelEvent*>(event);
      d->onViewWheelEvent(e);
    } break;
    case QEvent::Leave: {
      QPoint mousePos = QCursor::pos();
      QMouseEvent moveEvent(QEvent::MouseMove, mousePos, mousePos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
      d->onViewMouseEvent(&moveEvent);
    } break;
    case QEvent::ContextMenu: {
      QContextMenuEvent* e = static_cast<QContextMenuEvent*>(event);
      d->onContextMenuEvent(mapToGlobal(e->pos()));
    } break;
    case QEvent::DragEnter: {
      if (d->isOSRModeEnabled_) {
        QDragEnterEvent* e = static_cast<QDragEnterEvent*>(event);
        d->onDragEnter(e);
      }
    } break;
    case QEvent::DragMove: {
      QDragMoveEvent* e = static_cast<QDragMoveEvent*>(event);
      d->onDragMove(e);
    } break;
    case QEvent::DragLeave: {
      QDragLeaveEvent* e = static_cast<QDragLeaveEvent*>(event);
      d->onDragLeave(e);
    } break;
    case QEvent::Drop: {
      QDropEvent* e = static_cast<QDropEvent*>(event);
      d->onDrop(e);
    } break;
    default:
      break;
  }

  return QWidget::event(event);
}
