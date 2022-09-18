#include "locatejobwidget.hpp"
#include "ui_locatejobwidget.h"
#include "../util/fileinfomodel.hpp"

#include <orient/fs_pred_tree/fs_expr_builder.hpp>
#include <QtCore/QJsonObject>
#include <QtWidgets/QMessageBox>

namespace seev {

LocateJobWidget::LocateJobWidget(const QJsonObject& obj, orie::app &app, QWidget *parent)
    : LocateJobWidget(obj[QStringLiteral("command")].toString(), app, parent)
{
    ui->browseIconBut->setIcon(QIcon(obj[QStringLiteral("iconPath")].toString()));
    ui->saveNameEdit->setText(obj[QStringLiteral("description")].toString(tr("New Search")));
}

LocateJobWidget::LocateJobWidget(const QString& cmd, orie::app &app, QWidget *parent)
    : QWidget(parent), ui(new Ui::LocateJobWidget)
    , m_orieApp(app), m_resMdl(new FileinfoModel)
{
    ui->setupUi(this);
    ui->locateResLst->setModel(m_resMdl);

    connect(this, &LocateJobWidget::resultYielded, 
            m_resMdl, &FileinfoModel::addInfo);

    try {
        // TODO: Prevent the usage of -[f]print[f], -exec, -delete
        auto builder = orie::pred_tree::fs_expr_builder();
        builder.build(cmd.toStdString());
        if (app.start_paths().size() == 0) {
            QMessageBox::critical(this, tr("No Starting Paths?"),
                tr("No starting paths. Cannot proceed. Add one in Home Page"));
            return;
        }

        size_t jobSize = 20 / app.start_paths().size() + 1;
        m_jobList = m_orieApp.get_jobs(*builder.release(), 
            [this] (bool isAsync, orie::fs_data_iter& it) {
                QFileInfo resInfo(QString::fromStdString(it.path()));
                if (isAsync)
                    emit resultYielded(resInfo);
                else m_resMdl->addInfo(resInfo);
            }, jobSize, jobSize);

    } catch (std::exception& e) {
        QMessageBox::warning(this, tr("Expression Error"),
            tr("Invalid orient Expression:\n") + e.what() + 
            tr("\nIf the expression is not typed by yourself in advanced mode, "
               "\nfeel free to start a GitHub Issue."));
    }
}

LocateJobWidget::~LocateJobWidget() {
    delete ui; // auto-free m_resmdl
}

QJsonObject LocateJobWidget::toJson() const {
    return QJsonObject();
}

void LocateJobWidget::locateMore(size_t threshold) {

}

void LocateJobWidget::onResmdlSelected(const QModelIndex &mdl) {

}

void LocateJobWidget::onScroll(int value) {

}

void LocateJobWidget::onIcoPathSel() {

}

} // namespace seev
