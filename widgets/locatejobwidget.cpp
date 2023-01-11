#include "locatejobwidget.hpp"
#include "ui_locatejobwidget.h"
#include "../util/fileinfomodel.hpp"
#include "../previewer/previewer.hpp"

#include <orient/fs_pred_tree/fs_expr_builder.hpp>
#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QFileIconProvider>
#include <QtWidgets/QFileDialog>
#include <QtGui/QDesktopServices>

#ifdef _WIN32
#define fromStdString fromStdWString
#define toStdString toStdWString
#endif

namespace seev {

void LocateJobWidget::locResCallback(orie::fs_data_iter& it) {
#ifdef _WIN32
        QFileInfo resInfo(QString::fromStdWString(it.path().substr(1)));
#else
        QFileInfo resInfo(QString::fromStdString(it.path()));
#endif
        if (!resInfo.exists())
            return; // Do not display nonexistent files
        emit resultYielded(resInfo);
}

LocateJobWidget::LocateJobWidget(const QJsonObject& obj, orie::app &app,
                                 Previewer *previewer, QWidget *parent)
    : LocateJobWidget(obj["command"].toString(), app, previewer, parent)
{
    m_iconPath = obj["iconPath"].toString();
    ui->browseIconBut->setIcon(QIcon(m_iconPath));
    setWindowIcon(ui->browseIconBut->icon());
    ui->saveNameEdit->setText(obj["description"].toString(tr("New Search")));
}

LocateJobWidget::LocateJobWidget(const QString& cmd, orie::app &app,
                                 Previewer *previewer, QWidget *parent)
    : QWidget(parent), ui(new Ui::LocateJobWidget), ref_previewer(previewer)
    , m_orieApp(app), m_resMdl(new FileinfoModel), m_command(cmd), m_isSearching(false)
{
    qDebug() << m_command;
    ui->setupUi(this);
    ui->locateResLst->setModel(m_resMdl);
    ui->locateResLst->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->locateResLst->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->locateResLst->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    connect(this, &LocateJobWidget::resultYielded, m_resMdl, &FileinfoModel::addInfo);
    connect(ui->locateResLst->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &LocateJobWidget::onScroll);
    connect(ui->locateResLst, &QTableView::clicked, this, &LocateJobWidget::onResmdlClicked);
    connect(ui->browseIconBut, &QPushButton::clicked, this, &LocateJobWidget::onIcoPathSel);
    connect(ui->saveBut, &QPushButton::clicked, this, 
            [this] () { saveRequested(toJson()); });
    connect(ui->infoDispBut, &QPushButton::clicked, this, [this] () {
        if (m_lastSelected.isReadable())
            QDesktopServices::openUrl(
                QUrl::fromLocalFile(m_lastSelected.absoluteFilePath())
            ); 
    });
    connect(ui->fetchMoreBut, &QPushButton::clicked, this,
            [this] () { onScroll(2147483647); });

    try {
        // TODO: Prevent the usage of -[f]print[f], -exec, -delete
        auto builder = orie::pred_tree::fs_expr_builder();
        builder.build(cmd.toStdString());
        m_expr.reset(builder.release());
        if (app.start_paths().size() == 0) {
            QMessageBox::critical(this, tr("No Starting Paths?"),
                tr("No starting paths. Cannot proceed. Add one in Home Page"));
            return;
        }

        m_jobList = m_orieApp.get_jobs(*m_expr);
        // Start first search
        onScroll(2147483647);

    } catch (std::exception& e) {
        QMessageBox::warning(this, tr("Expression Error"),
            tr("Invalid orient Expression:\n") + e.what() + 
            tr("\nIf the expression is not typed by yourself in advanced mode, "
               "\nfeel free to start a GitHub Issue."));
    }
}

LocateJobWidget::~LocateJobWidget() {
    // Request all jobs to stop
    for (auto& datJobPair : m_jobList)
        datJobPair.second->cancel();
    // Wait for all jobs to actually stop
    if (m_searchThread.joinable())
        m_searchThread.join();
    delete ui; // auto-free m_resmdl
}

QJsonObject LocateJobWidget::toJson() const {
    QJsonObject res;
    res["command"] = m_command;
    res["iconPath"] = m_iconPath;
    res["description"] = windowTitle();
    return res;
}

void LocateJobWidget::onResmdlClicked(const QModelIndex &mdl) {
    if (m_resMdl->rowCount() == 0)
        return;
    const qsizetype r = mdl.row() >= m_resMdl->rowCount() ?
                        m_resMdl->rowCount() - 1 : mdl.row();
    m_lastSelected = m_resMdl->at(r);
    if (ref_previewer)
        ref_previewer->setPreviewPath(m_lastSelected.absoluteFilePath());

    // Display file icon in the displaying button
    const QSize butS = ui->infoDispBut->size();
    ui->infoDispBut->setIcon(QFileIconProvider().icon(m_lastSelected));
    // Icon size is set in locatejobwidget.ui
    // ui->infoDispBut->setIconSize(QSize(
    //     butS.height() * 0.8, butS.height() * 0.8
    // ));

    // Set descriptive text
    QString ap = m_lastSelected.absolutePath(), fn = m_lastSelected.fileName();
    int dispWidth = butS.width() * 0.7;
    QFont dispFont = ui->infoDispBut->font();
    QFontMetrics dispMetrics(dispFont);
    ap = dispMetrics.elidedText(tr("Folder: ") + ap, Qt::ElideLeft, dispWidth);
    fn = dispMetrics.elidedText(tr("Name: ") + fn, Qt::ElideLeft, dispWidth);

    ui->infoDispBut->setText(tr( "%1\nSize: %2\n%3\nTimeModified: %4")
        .arg(fn, QString::number(m_lastSelected.size()), ap,
             m_lastSelected.lastModified().toString(tr("dd-MM-yyyy hh:mm"))));
}

void LocateJobWidget::onScroll(int value) {
    if (value <= ui->locateResLst->verticalScrollBar()->maximum() * 0.95 ||
        m_orieApp.start_paths().size() <= 0 || m_isSearching)
        return;

    for (auto& datJobPair : m_jobList) 
        if (!datJobPair.second->is_finished())
            goto notFinished;
    // Already finished; disable the "Fetch More" button
    ui->fetchMoreBut->setDisabled(true);
    ui->fetchMoreSpin->setDisabled(true);
    ui->fetchMoreBut->setText(tr("All Done"));
    return;

notFinished:
    if (m_searchThread.joinable())
        m_searchThread.join();
    m_isSearching = true;
    m_searchThread = std::thread([this] () {
        size_t jobSize = ui->fetchMoreSpin->value() / 
                         m_orieApp.start_paths().size() + 1;
        for (auto& datJobPair : m_jobList) {
            datJobPair.second->start(m_orieApp._pool, 
                [this] (orie::fs_data_iter& it) {
                    locResCallback(it);
                }, jobSize, jobSize);
        }
        m_isSearching = false;
    });
}

void LocateJobWidget::onIcoPathSel() {
    QString confPath = QFileDialog::getOpenFileName(
        this, tr("Select Icon"), QDir::currentPath(),
        tr("Images (*.png *.xpm *.jpg);;All Files (*)"));
#ifdef _WIN32
    confPath.replace(QChar(orie::reverse_sep), QChar(orie::separator));
#endif
    if (confPath.isEmpty())
        return;

    m_iconPath = std::move(confPath);
    ui->browseIconBut->setIcon(QIcon(m_iconPath));
    setWindowIcon(ui->browseIconBut->icon());
}

} // namespace seev
