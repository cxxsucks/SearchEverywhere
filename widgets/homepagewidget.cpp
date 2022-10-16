#include "homepagewidget.hpp"
#include "orieconfwidget.hpp"
#include "locatejobwidget.hpp"
#include "oriepredselector.hpp"
#include "./ui_homepagewidget.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

namespace seev {

QString HomePageWidget::seevDefaultConfPath = 
    QString(::getenv("HOME")) + "/.config/orie/seev_default.txt";
QString HomePageWidget::orieDefaultConfPath = 
    QString(::getenv("HOME")) + "/.config/orie/default.txt";

HomePageWidget::HomePageWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::HomePageWidget)
    , m_orieApp(m_pool), m_lastUpdateTime(::time(nullptr))
{
    ui->setupUi(this);
    ui->orieCmdEdit->setHidden(true);
    ui->advOptBox->setHidden(true);

    connect(ui->updateDbBut, &QPushButton::clicked, this, 
            &HomePageWidget::updateDbButClicked);
    connect(ui->typeOrieCmdBut, &QPushButton::clicked, this, 
            &HomePageWidget::typeOrieCmdButClicked);
    connect(ui->seevConfPathBut, &QPushButton::clicked, this,
            &HomePageWidget::selSeevConfButClicked);
    connect(ui->searchEditDlgBut, &QPushButton::clicked, this,
            &HomePageWidget::showSearchDlg);

    connect(ui->savedSearchLst, &QListWidget::doubleClicked, this,
        [this] (const QModelIndex& idx) { 
            LocateJobWidget* jobw = new LocateJobWidget(
                m_savedSearches.at(idx.column()).toObject(), m_orieApp
            );
            connect(jobw, &LocateJobWidget::saveRequested,
                    this, &HomePageWidget::addSavedSearch);
            emit seevWidgetCreated(jobw);
        });
    connect(ui->initSearchBut, &QPushButton::clicked, this,
        [this] () { 
            LocateJobWidget* jobw = new LocateJobWidget(
                ui->predWidg->genCommand(), m_orieApp
            );
            connect(jobw, &LocateJobWidget::saveRequested,
                    this, &HomePageWidget::addSavedSearch);
            emit seevWidgetCreated(jobw);
        });

    connect(ui->seevConfPathEdit, &QLineEdit::returnPressed, this,
        [this] () {setSeevConfPath(ui->seevConfPathEdit->text());});
    connect(ui->openStartPathDlgBut, &QPushButton::clicked, this,
        [this] () { StartPathConfDialog(m_orieApp, this).exec(); });
    connect(ui->openConfWidgBut, &QPushButton::clicked, this,
        [this] () { emit seevWidgetCreated(new OrieConfWidget(
            QString::fromStdString(m_orieApp.conf_path())));
        });
    connect(ui->delSavedSearchBut, &QPushButton::clicked,
        [this] () { eraseSavedSearch(ui->savedSearchLst->currentRow()); });

    // Always launch with default seev configuration path
    setSeevConfPath(seevDefaultConfPath);
    m_orieApp.read_db()
        .add_start_path(NATIVE_PATH("/")) // TODO: edit start path
        .start_auto_update(std::chrono::seconds(ui->updIntSpin->value()));
}

void HomePageWidget::fromJsonObj(const QJsonObject& obj) {
    QString orieConfPath = obj["orieConfPath"].toString(orieDefaultConfPath);
    // Write default configuration if the specified orie conf file does not exist
    if (!m_orieApp.read_conf(orieConfPath.toStdString())) {
        m_orieApp = orie::app::os_default(m_pool);
        // TODO: Auto update stops here!
        m_orieApp.write_conf(orieConfPath.toStdString());
    }

    // When possible, index with char* instead of QString
    ui->savedSearchLst->clear();
    m_savedSearches = QJsonArray();
    for (const QJsonValueRef& val : obj["savedSearches"].toArray()) {
        if (!val.isObject())
            continue;
        addSavedSearch(val.toObject());
    }
    ui->updIntSpin->setValue(obj["updateDbInterval"].toInt(3600));
}

void HomePageWidget::addSavedSearch(const QJsonObject& obj) {
    m_savedSearches.append(obj);
    ui->savedSearchLst->addItem(new QListWidgetItem(
        QIcon(obj["iconPath"].toString()),
        obj["description"].toString()
    ));
}

void HomePageWidget::eraseSavedSearch(int idx) {
    if (idx < 0 || idx >= ui->savedSearchLst->count())
        return;
    if (QMessageBox::warning(this, tr("Are you sure?"), 
            tr("Are you sure to remove selected search?"), 
            QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok)
        return;

    m_savedSearches.removeAt(idx);
    delete ui->savedSearchLst->item(idx);
}

void HomePageWidget::setSeevConfPath(const QString &path) {
    ui->seevConfPathEdit->setText(path);

    // Read the seev configuration
    QFile confStream(path);
    confStream.open(QIODeviceBase::ReadOnly);
    // Set the internals 
    fromJsonObj(QJsonDocument::fromJson(confStream.readAll()).object());
}

QJsonObject HomePageWidget::toJsonObj() const {
    QJsonObject res;
    res["savedSearches"] = m_savedSearches;
    res["orieConfPath"] = QString::fromStdString(m_orieApp.conf_path());
    res["updateDbInterval"] = ui->updIntSpin->value();
    return res;
}

HomePageWidget::~HomePageWidget()
{
    // Write seev configuration
    QJsonDocument seevConf;
    seevConf.setObject(toJsonObj());
    QFile confStream(ui->seevConfPathEdit->text());
    confStream.open(QIODeviceBase::WriteOnly);
    confStream.write(seevConf.toJson());
    delete ui;
}

void HomePageWidget::updateDbButClicked() {
    time_t nowClk = ::time(nullptr);
    if (nowClk < m_lastUpdateTime + 10) {
        QMessageBox::warning(this, tr("Scan Too Frequent"),
            tr("It is useless to frequently scan filesystem.\n"
               "Too much scanning can only burden your disks more."));
        return;
    }

    m_lastUpdateTime = nowClk;
    m_orieApp.stop_auto_update();
    m_orieApp.start_auto_update(std::chrono::seconds(ui->updIntSpin->value()));
}

void HomePageWidget::typeOrieCmdButClicked() {
    if (ui->orieCmdEdit->isHidden()) {
        ui->orieCmdEdit->setHidden(false);
        ui->typeOrieCmdBut->setText(tr("Search with this command"));
        return;
    }
    // If the command editor is not hidden,
    // the user would have already edited the command
    // therefore start a search
    emit seevWidgetCreated(new LocateJobWidget(
        ui->orieCmdEdit->toPlainText(), m_orieApp
    ));
    ui->typeOrieCmdBut->setText(tr("Type `orient` Command (Expert)"));
    ui->orieCmdEdit->setHidden(true);
}

void HomePageWidget::selSeevConfButClicked() {
    QString confPath = QFileDialog::getOpenFileName(
        this, tr("Select seev Conf File"), QDir::currentPath());
    if (confPath.isEmpty())
        return;
    ui->seevConfPathEdit->setText(confPath);
    setSeevConfPath(confPath);
}

void HomePageWidget::showSearchDlg() {
    QDialog exprEditDlg(this);
    auto editing = ui->predWidg;
    ui->newSearchLayout->removeWidget(editing);
    (new QHBoxLayout(&exprEditDlg))->addWidget(editing);
    exprEditDlg.exec();

    // Edit finished
    ui->newSearchLayout->addWidget(editing, 0, 0, 1, 3);
}

} // namespace seev
