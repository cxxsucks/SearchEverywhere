#include "previewer.hpp"
#include "ui_previewer.h"

#include "imagePreviewer/imagepreviewer.h"
#ifdef _ORIEA_PREVIEW_MEDIA
#include "mediaPreviewer/mediapreviewer.h"
#endif

#include <QtCore/QMimeDatabase>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QtGui/QClipboard>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QFileDialog>
#include <QtGui/QFileSystemModel>
#include <QtWidgets/QTreeView>

namespace seev {
Previewer::Previewer(QWidget *parent, const QString& soffice,bool hasOpen)
    : QWidget(parent), ui(new Ui::Previewer), m_sofficePath(soffice)
{
    ui->setupUi(this);
    ui->fileOpGrp->hide();
    if (!hasOpen)
        ui->selBut->hide();

    connect(ui->selBut, &QPushButton::clicked, this, &Previewer::selectedPreviewPath);
    connect(ui->openFileBut, &QPushButton::clicked, this,
            &Previewer::onOpenFileClicked);
    connect(ui->openDirBut, &QPushButton::clicked, this,
            &Previewer::onOpenDirClicked);
    connect(ui->cpyFileBut, &QPushButton::clicked, this,
            &Previewer::onCopyFilePathClicked);
    connect(ui->cpyDirBut, &QPushButton::clicked, this,
            &Previewer::onCopyDirPathClicked);
    connect(&m_officePreviewFut, &QFutureWatcher<int>::finished, 
            this, &Previewer::_office_cvtFinish);
}

void Previewer::setPreviewPath(const QString &path) {
    if (m_officePreviewFut.isRunning())
        m_officePreviewFut.cancel();

    QMimeDatabase db;
    auto mmt = db.mimeTypeForFile(path);
    QString mmtNme = mmt.name();
    if (mmtNme.contains(QStringLiteral("pdf"))
            || mmtNme.contains(QStringLiteral("image/")))
        _image_previewImpl(path);
    else if (mmtNme.contains(QStringLiteral("video/"))
            || mmtNme.contains(QStringLiteral("audio/")))
        _media_previewImpl(path);
    else if (mmt.inherits(QStringLiteral("text/html")))
        _html_previewimpl(QUrl::fromLocalFile(path), false);
    else if (mmt.inherits(QStringLiteral("text/markdown")))
        _html_previewimpl(QUrl::fromLocalFile(path), true);
    else if (mmtNme.contains("inode/"))
        _dir_previewimpl(path);
    else if (mmtNme.contains(QStringLiteral("openxml"))
            || mmtNme.contains(QStringLiteral("word"))
            || mmtNme.contains(QStringLiteral("powerpoint"))
            || mmtNme.contains(QStringLiteral("excel"))
            || mmtNme.contains(QStringLiteral("opendocument")))
        _office_previewimpl(path);
    else _txt_previewImpl(path);

    m_viewingPath = path;
    ui->fileOpGrp->setVisible(true);
    resizeEvent(nullptr);
}

void Previewer::resizeEvent(QResizeEvent *) {
    QFileInfo info(m_viewingPath);
    if (info.fileName().isEmpty())
        return;
    const qsizetype maxLenDisp = qMax(
        ui->infoLbl->width() / fontMetrics().averageCharWidth() - 6, 9);
    QString ap = info.absolutePath(), fn = info.fileName();
    if (ap.size() > maxLenDisp)
        ap = "..." + ap.last(maxLenDisp - 9);
    if (fn.size() > maxLenDisp)
        fn = fn.first(maxLenDisp - 9) + "...";
    ui->infoLbl->setText(tr(
        "Name: %1\nSize: %2\nFolder: %3\nTimeModified: %4"
    ).arg(fn).arg(info.size()).arg(ap).arg(
        info.lastModified().toString(tr("dd-MM-yyyy hh:mm"))
    ));
}

Previewer::~Previewer() { delete ui; }

void Previewer::_image_previewImpl(const QString& path) {
    ImagePreviewer* viewerNew = qobject_cast<ImagePreviewer*>(ui->viewer);
    if (viewerNew == nullptr) {
        viewerNew = new ImagePreviewer;
        layout()->replaceWidget(ui->viewer, viewerNew);
        delete ui->viewer;
        ui->viewer = viewerNew;
    }
    viewerNew->setPreview(path);
}

void Previewer::_media_previewImpl(const QString &path) {
    Q_UNUSED(path);
#ifdef _ORIEA_PREVIEW_MEDIA
    MediaPreviewer* viewerNew = qobject_cast<MediaPreviewer*>(ui->viewer);
    if (viewerNew == nullptr) {
        viewerNew = new MediaPreviewer;
        layout()->replaceWidget(ui->viewer, viewerNew);
        delete ui->viewer;
        ui->viewer = viewerNew;
    }
    viewerNew->addToPlaylist(QUrl::fromLocalFile(path));
#else
    _txt_previewImpl(path);
#endif
}

void Previewer::_html_previewimpl(const QUrl &path, bool md) {
    QTextBrowser* viewerNew = qobject_cast<QTextBrowser*>(ui->viewer);
    if (viewerNew == nullptr) {
        viewerNew = new QTextBrowser;
        layout()->replaceWidget(ui->viewer, viewerNew);
        delete ui->viewer;
        ui->viewer = viewerNew;
        QTextCharFormat fmt = viewerNew->currentCharFormat();
        fmt.setFontFamilies(QStringList("monospace"));
        fmt.setFontPointSize(10);
        viewerNew->setCurrentCharFormat(fmt);
        viewerNew->setLineWrapMode(QTextEdit::NoWrap);
    }
    viewerNew->setSource(path, md ?
        QTextDocument::MarkdownResource : QTextDocument::HtmlResource);
}

void Previewer::_txt_previewImpl(const QString &path) {
    QPlainTextEdit* viewerNew = qobject_cast<QPlainTextEdit*>(ui->viewer);
    if (viewerNew == nullptr) {
        viewerNew = new QPlainTextEdit;
        layout()->replaceWidget(ui->viewer, viewerNew);
        delete ui->viewer;
        ui->viewer = viewerNew;
        QTextCharFormat fmt = viewerNew->currentCharFormat();
        fmt.setFontFamilies(QStringList("monospace"));
        fmt.setFontPointSize(10);
        viewerNew->setCurrentCharFormat(fmt);
        viewerNew->setLineWrapMode(QPlainTextEdit::NoWrap);
        viewerNew->setReadOnly(true);
    } else viewerNew->clear();

    QFile stream(path);
    stream.open(QIODevice::ReadOnly);
    if (!stream.isOpen()) {
        viewerNew->setPlainText(tr(
            "Cannot Open %1.\nIt may have been removed."
            "You can click the \"Update Now\" in Explore Tab"
            "to make searches more accurate."
        ).arg(path));
        return;
    }

    char binTestBuf[2048];
    qint64 bufCont = stream.read(binTestBuf, 2048);
    if (std::count(binTestBuf, binTestBuf + bufCont, '\0') != 0) {
        viewerNew->setPlainText(tr(
            "%1 is a Binary File.\n"
            "Binary files are designed for systems and are not "
            "human readable."
        ).arg(path));
        return;
    } else if (stream.readLine(binTestBuf, 1024) > 1000) {
        viewerNew->setPlainText(tr(
            "%1 is an One-Liner Ascii File.\n"
            "These files are deliberately obsfucated so that "
            "computers understand them better, though they make "
            "no sense to human."
        ).arg(path));
        return;
    }

    stream.seek(0);
    for (size_t i = 0; i < 500 && !stream.atEnd(); ++i) {
        viewerNew->insertPlainText(stream.readLine());
    }
}

void Previewer::_dir_previewimpl(const QString &path) {
    QTreeView* viewerNew = qobject_cast<QTreeView*>(ui->viewer);
    if (viewerNew == nullptr) {
        viewerNew = new QTreeView;
        layout()->replaceWidget(ui->viewer, viewerNew);
        delete ui->viewer;
        ui->viewer = viewerNew;
    }

    QFileSystemModel* model = new QFileSystemModel;
    model->setRootPath(path);
    viewerNew->setModel(model);
    viewerNew->setRootIndex(model->index(path));
}

void Previewer::_office_previewimpl(const QString& path) {
    QStringList args {
        QStringLiteral("--headless"), QStringLiteral("--convert-to"),
        QStringLiteral("jpg"), path,
        QStringLiteral("--outdir"), QDir::tempPath()
    };

    m_officePreviewFut.setFuture(QtConcurrent::run(
        QProcess::execute, m_sofficePath, args
    ));
}

void Previewer::_office_cvtFinish() {
    int ret = m_officePreviewFut.result();
    QFileInfo info(m_viewingPath);
    info = QFileInfo(QDir::tempPath() + QDir::separator() + info.baseName() 
                     + QStringLiteral(".jpg"));

    if (ret == -2 || !info.exists()) 
        _html_previewimpl(QStringLiteral("qrc:/previewFailTxt/noAx.md"), true);
    else {
        _image_previewImpl(info.absoluteFilePath());
        QFile::remove(info.absoluteFilePath());
    }

}

void Previewer::selectedPreviewPath() {
    QString sel = QFileDialog::getOpenFileName(this, "Preview File");
    if (!sel.isEmpty())
        setPreviewPath(sel);
}

void Previewer::onOpenFileClicked() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_viewingPath));
}
void Previewer::onOpenDirClicked() {
    QFileInfo info(m_viewingPath);
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.canonicalPath()));
}
void Previewer::onCopyFilePathClicked() {
    QGuiApplication::clipboard()->setText(m_viewingPath);
}
void Previewer::onCopyDirPathClicked() {
    QFileInfo info(m_viewingPath);
    QGuiApplication::clipboard()->setText(info.canonicalFilePath());
}

}
