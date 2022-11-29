#include "orieconfwidget.hpp"
#include "ui_orieconfwidget.h"
#include "ui_startpathconfwidget.h"
#include <orient/app.hpp>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#ifdef _WIN32
#define fromStdString fromStdWString
#define toStdString toStdWString
#endif

// Helper function: Select and add a path to a QListWidget
static void __selAndAddPath(QListWidget* lw) {
    QString selPath = QFileDialog::getExistingDirectory(lw, 
        QObject::tr("Add Starting Point"), QDir::rootPath());
    if (selPath.isEmpty())
        return;
    selPath = QDir(selPath).canonicalPath();
    if (selPath.contains('`')) {
        QMessageBox::critical(lw, QObject::tr("No Backquote"),
            QObject::tr("Sorry, but current implementation has trouble dealing"
                        " with paths containing backquotes."));
        return;
    }

    // Check for duplicates
    for (int i = 0; i < lw->count(); ++i) 
        if (selPath.startsWith(lw->item(i)->text())) {
            QMessageBox::information(lw, QObject::tr("Path Duplicate"),
                selPath + QObject::tr(" has already been added"));
            break;
        }
    lw->addItem(selPath);
}

// Helper function: Delete current item in a QListWidget
static void __delCurPath(QListWidget* lw) {
    delete lw->currentItem();
}

namespace seev {

void OrieConfWidget::writeConfFile() const {
    static orie::fifo_thpool dummy_pool(0);
    orie::app writer(dummy_pool);

    for (int i = 0; i < ui->ssdRootPathLst->count(); ++i) 
        writer.add_root_path(ui->ssdRootPathLst->item(i)->text().toStdString(), true);
    for (int i = 0; i < ui->hddRootPathLst->count(); ++i) 
        writer.add_root_path(ui->hddRootPathLst->item(i)->text().toStdString(), false);
    for (int i = 0; i < ui->prunedPathLst->count(); ++i) 
        writer.add_ignored_path(ui->prunedPathLst->item(i)->text().toStdString());
    writer.set_db_path(ui->dbPathEdit->text().toStdString())
          .write_conf(m_confPath.toStdString());
}

void OrieConfWidget::readConfFile(const QString& path) {
    static orie::fifo_thpool dummy_pool(0);
    orie::app reader(dummy_pool);

    reader.read_conf(path.toStdString());
    ui->dbPathEdit->setText(QString::fromStdString(reader.db_path()));
    for (const auto& [pathStr, isConcur] : reader.root_paths()) {
        isConcur ? ui->ssdRootPathLst->addItem(QString::fromStdString(pathStr))
                 : ui->hddRootPathLst->addItem(QString::fromStdString(pathStr));
    }
    for (const auto& pathStr : reader.ignored_paths())
        ui->prunedPathLst->addItem(QString::fromStdString(pathStr));
}

OrieConfWidget::OrieConfWidget(const QString &orieConfPath, QWidget *parent) 
    : QWidget(parent),  ui(new Ui::OrieConfWidget), m_confPath(orieConfPath)
{
    ui->setupUi(this);
    readConfFile(orieConfPath);

    // Two "Move To" buttons
    connect(ui->moveToHddBut, &QPushButton::clicked, this, [this] () {
        ui->hddRootPathLst->addItem(
            ui->ssdRootPathLst->takeItem(ui->ssdRootPathLst->currentRow())
    ); });
    connect(ui->moveToSsdBut, &QPushButton::clicked, this, [this] () {
        ui->ssdRootPathLst->addItem(
            ui->hddRootPathLst->takeItem(ui->hddRootPathLst->currentRow())
    ); });

    connect(ui->hddRootAddBut, &QPushButton::clicked,
            std::bind(__selAndAddPath, ui->hddRootPathLst));
    connect(ui->ssdRootAddBut, &QPushButton::clicked,
            std::bind(__selAndAddPath, ui->ssdRootPathLst));
    connect(ui->addPrunedBut, &QPushButton::clicked,
            std::bind(__selAndAddPath, ui->prunedPathLst));
    connect(ui->hddRootEraseBut, &QPushButton::clicked,
            std::bind(__delCurPath, ui->hddRootPathLst));
    connect(ui->ssdRootEraseBut, &QPushButton::clicked,
            std::bind(__delCurPath, ui->ssdRootPathLst));
    connect(ui->erasePrunedBut, &QPushButton::clicked,
            std::bind(__delCurPath, ui->prunedPathLst));
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &OrieConfWidget::confDone);
}

OrieConfWidget::~OrieConfWidget() {
    delete ui;
}

StartPathConfDialog::StartPathConfDialog(orie::app &orieApp, QWidget *parent)
    : QDialog(parent), m_orieApp(orieApp), ui(new Ui::StartPathConfWidget)
{
    ui->setupUi(this);
    // Load starting paths to list widget
    for (const orie::str_t& sp : m_orieApp.start_paths()) 
        ui->startPathLst->addItem(QString::fromStdString(sp));

    connect(ui->eraseBut, &QPushButton::clicked, 
            this, &StartPathConfDialog::onEraseClicked);
    connect(ui->addBut, &QPushButton::clicked, 
            this, &StartPathConfDialog::onAddClicked);
}

StartPathConfDialog::~StartPathConfDialog() {
    delete ui;
}

void StartPathConfDialog::onAddClicked() {
    QString selPath = QFileDialog::getExistingDirectory(this, 
        tr("Add Starting Point"), QDir::rootPath());
    if (selPath.isEmpty())
        return;
    selPath = QDir(selPath).canonicalPath();

    // Check for redundancy
    for (int i = 0; i < ui->startPathLst->count(); ++i) 
        if (selPath.startsWith(ui->startPathLst->item(i)->text())) {
            QMessageBox::information(this, tr("Redundant Start Point"),
                ui->startPathLst->item(i)->text() + tr(" already covers ") + selPath +
                tr("\nTherefore the start point being added will be searched for twice"));
            break;
        }

    m_orieApp.add_start_path(selPath.toStdString());
    ui->startPathLst->addItem(selPath);
}

void StartPathConfDialog::onEraseClicked() {
    QString curPath = ui->startPathLst->currentItem()->text();
    m_orieApp.erase_start_path(curPath.toStdString());
    // for (auto item : ui->startPathLst->findItems(curPath, Qt::MatchExactly))
    //     delete item;;
    delete ui->startPathLst->currentItem();
}

} // namespace seev
