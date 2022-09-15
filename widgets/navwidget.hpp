#ifndef SEEV_NAVWIDGET_H
#define SEEV_NAVWIDGET_H

#include <QWidget>
#include <orient/app.hpp>

namespace seev {

namespace Ui { class NavWidget; }

class NavWidget : public QWidget
{
    Q_OBJECT

public:
    QJsonDocument toJsonDoc() const;
    void fromJsonDoc(const QJsonDocument& doc);

    NavWidget(QWidget *parent = nullptr);
    ~NavWidget();

private:
    Ui::NavWidget *ui;
    orie::fifo_thpool _pool;
    orie::app _orie_app;

    static QString seevDefaultConfPath;
};

} // namespace seev
#endif // SEEV_NAVWIDGET_H
