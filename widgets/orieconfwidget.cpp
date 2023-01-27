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
#ifdef _WIN32
    selPath.replace(QChar(orie::reverse_sep), QChar(orie::separator));
#endif
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

    // Hard-code indexing from root
    try {
        writer.set_db_path(ui->dbPathEdit->text().toStdString().c_str())
              .set_root_path(orie::str_t(1, orie::separator));
        for (int i = 0; i < ui->prunedPathLst->count(); ++i) 
            writer.add_ignored_path(ui->prunedPathLst->item(i)->text().toStdString());
        writer.write_conf(m_confPath.toStdString());
    } catch (std::runtime_error&) {
        QMessageBox::warning(nullptr, tr("Bad database file"), 
            tr("No read permission to selected database file."));
    }
}

void OrieConfWidget::readConfFile(const QString& path) {
    static orie::fifo_thpool dummy_pool(0);
    orie::app reader(dummy_pool);

    try {
        reader.read_conf(path.toStdString());
    } catch(const std::runtime_error& e) {
        qDebug() << path << " " << e.what();
        qDebug() << tr("Using an empty one in config GUI.");
        return;
    }
    
    ui->dbPathEdit->setText(QString::fromStdString(reader.db_path()));
    for (const orie::str_t& slowPathStr : reader.slow_paths())
        ui->slowPathLst->addItem(QString::fromStdString(slowPathStr));
    for (const orie::str_t& pathStr : reader.ignored_paths())
        ui->prunedPathLst->addItem(QString::fromStdString(pathStr));
}

OrieConfWidget::OrieConfWidget(const QString &orieConfPath, QWidget *parent) 
    : QWidget(parent),  ui(new Ui::OrieConfWidget), m_confPath(orieConfPath)
{
    ui->setupUi(this);
    readConfFile(orieConfPath);

    connect(ui->addSlowBut, &QPushButton::clicked,
            std::bind(__selAndAddPath, ui->slowPathLst));
    connect(ui->addPrunedBut, &QPushButton::clicked,
            std::bind(__selAndAddPath, ui->prunedPathLst));
    connect(ui->eraseSlowBut, &QPushButton::clicked,
            std::bind(__delCurPath, ui->slowPathLst));
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
#ifdef _WIN32
    selPath.replace(QChar(orie::reverse_sep), QChar(orie::separator));
#endif

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
