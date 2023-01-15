#include "homepagewidget.hpp"
#include "orieconfwidget.hpp"
#include "locatejobwidget.hpp"
#include "oriepredselector.hpp"
#include "../previewer/previewer.hpp"
#include "./ui_homepagewidget.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTabWidget>

#ifdef _WIN32
#define fromStdString fromStdWString
#define toStdString toStdWString
#endif

namespace seev {
#ifdef _WIN32
QString HomePageWidget::seevDefaultConfPath = 
    QString(::getenv("APPDATA")) + "\\.orie\\seev_default.txt";
QString HomePageWidget::orieDefaultConfPath = 
    QString(::getenv("APPDATA")) + "\\.orie\\default.txt";
#else
QString HomePageWidget::seevDefaultConfPath = 
    QString(::getenv("HOME")) + "/.config/orie/seev_default.txt";
QString HomePageWidget::orieDefaultConfPath = 
    QString(::getenv("HOME")) + "/.config/orie/default.txt";
#endif

HomePageWidget::HomePageWidget(Previewer *previewer, QWidget *parent)
    : QWidget(parent), ui(new Ui::HomePageWidget), ref_previewer(previewer)
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
                m_savedSearches.at(idx.row()).toObject(), m_orieApp, ref_previewer
            );
            connect(jobw, &LocateJobWidget::saveRequested,
                    this, &HomePageWidget::addSavedSearch);
            emit seevWidgetCreated(jobw);
        });
    connect(ui->initSearchBut, &QPushButton::clicked, this,
        [this] () { 
            LocateJobWidget* jobw = new LocateJobWidget(
                ui->predWidg->genCommand(), m_orieApp, ref_previewer
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
        .add_start_path(orie::str_t())
        .start_auto_update(std::chrono::seconds(ui->updIntSpin->value()), false);
}

void HomePageWidget::fromJsonObj(const QJsonObject& obj) {
    QString orieConfPath = obj["orieConfPath"].toString(orieDefaultConfPath);
    // Write default configuration if the specified orie conf file does not exist
    if (!m_orieApp.read_conf(orieConfPath.toStdString())) {
        m_orieApp = orie::app::os_default(m_pool);
        // TODO: Auto update stops here!
        m_orieApp.write_conf(orieConfPath.toStdString());
    }

    ui->savedSearchLst->clear();
    m_savedSearches = QJsonArray();
    QJsonArray searchesRead = obj["savedSearches"].toArray();
    if (searchesRead.empty()) {
        // Provide some example searches if no searches saved
        QJsonObject tmpObj;
        tmpObj["iconPath"] = QStringLiteral(":/search_img/music.png");
        tmpObj["description"] = tr("MP3/MP4 Files");
        tmpObj["command"] = QStringLiteral("-type f -a -iname *.mp?");
        searchesRead.push_back(tmpObj);
        tmpObj["iconPath"] = QStringLiteral(":/search_img/doc.png");
        tmpObj["description"] = tr("Recent Office Documents");
        tmpObj["command"] = QStringLiteral("-mtime -5 -a -ibregex "
                                           "'\\.(docx?|pptx?|xlsx?)$'");
        searchesRead.push_back(tmpObj);
        tmpObj["iconPath"] = QStringLiteral(":/search_img/c.png");
        tmpObj["description"] = tr("C sources containing 'Hello'");
        tmpObj["command"] = QStringLiteral("-content-strstr --ignore-case Hello"
                                           " -a -name *.c");
        searchesRead.push_back(tmpObj);
        tmpObj["iconPath"] = QStringLiteral(":/search_img/git.png");
        tmpObj["description"] = tr("Git Repositories (First Level Only)");
        tmpObj["command"] = QStringLiteral("-downdir -name '*.git' -a -prune");
        searchesRead.push_back(tmpObj);
    }

    for (const QJsonValueRef& val : searchesRead) {
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
    confStream.open(QIODevice::ReadOnly);
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
    confStream.open(QIODevice::WriteOnly);
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
    m_orieApp.stop_auto_update()
             .start_auto_update(std::chrono::seconds(ui->updIntSpin->value()),
                                true);
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
        ui->orieCmdEdit->toPlainText(), m_orieApp, ref_previewer
    ));
    ui->typeOrieCmdBut->setText(tr("Type `orient` Command (Expert)"));
    ui->orieCmdEdit->setHidden(true);
}

void HomePageWidget::selSeevConfButClicked() {
    QString confPath = QFileDialog::getOpenFileName(
        this, tr("Select seev Conf File"), QDir::currentPath());
#ifdef _WIN32
    confPath.replace(QChar(orie::reverse_sep), QChar(orie::separator));
#endif
    if (confPath.isEmpty())
        return;
    ui->seevConfPathEdit->setText(confPath);
    setSeevConfPath(confPath);
}

void HomePageWidget::showSearchDlg() {
    QDialog exprEditDlg(this);
    auto editing = ui->predScrollArea->takeWidget();
    (new QHBoxLayout(&exprEditDlg))->addWidget(editing);
    exprEditDlg.exec();

    // Edit finished
    ui->predScrollArea->setWidget(editing);
}

AppWidget::AppWidget(QWidget* p) 
    : QWidget(p), m_tabs(new QTabWidget) , m_previewer(new Previewer)
    , m_homePage(new HomePageWidget(m_previewer))
    , m_layout(new QHBoxLayout(this))
{
    QFile qssFile(":/qss/seev.qss");
    qssFile.open(QIODevice::ReadOnly);
    setStyleSheet(QString::fromLatin1(qssFile.readAll()));

    m_layout->addWidget(m_tabs, 1);
    m_layout->addWidget(m_previewer, 2);

    m_tabs->addTab(m_homePage, tr("Home Page"));
    m_tabs->setTabsClosable(true);
    setWindowIcon(QIcon(QStringLiteral(":/search_img/stolenLogo.png")));
    setWindowTitle(tr("SearchEverywhere"));

    QObject::connect(m_tabs, &QTabWidget::tabCloseRequested, 
        [this] (int i) { if (i >= 1) delete m_tabs->widget(i); });
    QObject::connect(m_homePage, &seev::HomePageWidget::seevWidgetCreated,
        [this] (QWidget* w) { m_tabs->addTab(w, w->windowIcon(), w->windowTitle()); });
    QObject::connect(m_homePage, &seev::HomePageWidget::seevWidgetCreated,
                     m_tabs, &QTabWidget::setCurrentWidget);
    resizeEvent(nullptr);
}

AppWidget::~AppWidget() {
    // Must delete all locate jobs and conf widgets
    while (m_tabs->count() > 1)
        delete m_tabs->widget(1);
    // then main page, then finally previewer
    delete m_homePage;
    delete m_previewer;
}

void AppWidget::resizeEvent(QResizeEvent*) {
    if (width() < 600) {
        // Hide Previewer and clear previewing content
        if (m_previewer->isHidden())
            // Already hidden; do nothing
            return;
        m_previewer->setHidden(true);
        m_previewer->clearPreview();
    } else {
        // Show Previewer
        m_previewer->setHidden(false);
    }
}

} // namespace seev
