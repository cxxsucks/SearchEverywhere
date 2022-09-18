#ifndef SEEV_LOCATEJOBWIDGET_H
#define SEEV_LOCATEJOBWIDGET_H

#include <QWidget>
#include <orient/app.hpp>

class QFileInfo;

namespace seev {

class FileinfoModel;
namespace Ui {
class LocateJobWidget;
}

class LocateJobWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocateJobWidget(const QString& command, orie::app& orieApp,
                             QWidget *parent = nullptr);
    explicit LocateJobWidget(const QJsonObject& jsonObj, orie::app& orieApp,
                             QWidget *parent = nullptr);
    ~LocateJobWidget();

    QJsonObject toJson() const;

public slots:
    void locateMore(size_t threshold);

signals:
    void saveRequested(const QJsonObject&);
    // `private` signal; for bringing async locate result to GUI thread
    void resultYielded(const QFileInfo&);

private slots:
    void onResmdlSelected(const QModelIndex& mdl);
    void onScroll(int value);
    void onIcoPathSel();

private:
    Ui::LocateJobWidget *ui;
    FileinfoModel* m_resMdl;
    orie::app& m_orieApp;
    orie::app::job_list m_jobList;
};


} // namespace seev
#endif // SEEV_LOCATEJOBWIDGET_H
