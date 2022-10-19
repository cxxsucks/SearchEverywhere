#ifndef SEEV_NAVWIDGET_H
#define SEEV_NAVWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCore/QJsonArray>
#include <orient/app.hpp>
class QJsonObject;
class QHBoxLayout;
class QTabWidget;

namespace seev {

class Previewer;
namespace Ui { class HomePageWidget; }

class HomePageWidget : public QWidget {
    Q_OBJECT

public slots:
    void fromJsonObj(const QJsonObject& obj);
    void setSeevConfPath(const QString& path);
    void addSavedSearch(const QJsonObject& obj);
    void eraseSavedSearch(int idx);

public:
    QJsonObject toJsonObj() const;
    HomePageWidget(Previewer* previewer, QWidget *parent = nullptr);
    ~HomePageWidget();

signals:
    void seevWidgetCreated(QWidget*);

private:
    Ui::HomePageWidget *ui;
    orie::fifo_thpool m_pool;
    orie::app m_orieApp;

    Previewer *ref_previewer;
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

class AppWidget : public QWidget {
    Q_OBJECT

public:
    AppWidget(QWidget* parent = nullptr);
    ~AppWidget();

private:
    QTabWidget* m_tabs;
    Previewer* m_previewer;
    HomePageWidget* m_homePage;
    QHBoxLayout* m_layout;

protected:
    void resizeEvent(QResizeEvent*) override;
};

} // namespace seev
#endif // SEEV_NAVWIDGET_H
