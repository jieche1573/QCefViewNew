#pragma once

#include <QWidget>
#include<QCefView.h>

namespace Ui
{
class Widget;
}


class Widget:public QWidget
{

      Q_OBJECT

public:
    Widget(QWidget* parent = Q_NULLPTR);
    ~Widget();
protected:
     bool eventFilter(QObject* watched, QEvent* event) override;
   private slots:
      void
      on_btnMenu_Min_clicked();
      void
      on_btnMenu_Max_clicked();
      void
      on_btnMenu_Close_clicked();


    private:
    Ui::Widget* ui;


private:
    QCefView* m_cefView = nullptr;
};
