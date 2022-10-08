#ifndef SEEV_NAVWIDGET_H
#define SEEV_NAVWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <orient/app.hpp>
class QJsonObject;

namespace seev {

namespace Ui { class NavWidget; }

class NavWidget : public QWidget
{
    Q_OBJECT

public slots:
    void fromJsonObj(const QJsonObject& obj);
    void setSeevConfPath(const QString& path);
    void addSavedSearch(const QJsonObject& obj);
    void eraseSavedSearch(int idx);

public:
    QJsonObject toJsonObj() const;
    NavWidget(QWidget *parent = nullptr);
    ~NavWidget();

signals:
    void seevWidgetCreated(QWidget*);

private:
    Ui::NavWidget *ui;
    orie::fifo_thpool m_pool;
    orie::app m_orieApp;

    QJsonArray m_savedSearches;
    // Prevent spam-click updatedb button
    time_t m_lastUpdateTime;

    static QString seevDefaultConfPath;
    static QString orieDefaultConfPath;

private slots:
    void updateDbButClicked();
    void typeOrieCmdButClicked();
    void selSeevConfButClicked();
    void showSearchDlg();
};

} // namespace seev
#endif // SEEV_NAVWIDGET_H
