#ifndef SEEV_LOCATEJOBWIDGET_H
#define SEEV_LOCATEJOBWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCore/QFileInfo>
#include <orient/app.hpp>

namespace seev {

class FileinfoModel;
class Previewer;
namespace Ui {
class LocateJobWidget;
}

class LocateJobWidget : public QWidget {
    Q_OBJECT

public:
    explicit LocateJobWidget(const QString& command, orie::app& orieApp,
                             Previewer* previewer, QWidget *parent = nullptr);
    explicit LocateJobWidget(const QJsonObject& jsonObj, orie::app& orieApp,
                             Previewer* previewer, QWidget *parent = nullptr);
    ~LocateJobWidget();

    QJsonObject toJson() const;

signals:
    void saveRequested(const QJsonObject&);
    // `private` signal; for bringing async locate result to GUI thread
    void resultYielded(const QFileInfo&);

private slots:
    void onResmdlClicked(const QModelIndex& mdl);
    void onScroll(int value);
    void onIcoPathSel();

private:
    Ui::LocateJobWidget *ui;
    FileinfoModel* m_resMdl;
    Previewer* ref_previewer;
    QFileInfo m_lastSelected;

    orie::app& m_orieApp;
    orie::app::job_list m_jobList;
    std::unique_ptr<orie::fsearch_expr> m_expr;
    std::thread m_searchThread;
    std::atomic<bool> m_isSearching;

    QString m_command;
    QString m_iconPath;

    void locResCallback(orie::fs_data_iter& it);
};


} // namespace seev
#endif // SEEV_LOCATEJOBWIDGET_H
