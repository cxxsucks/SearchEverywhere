#include "imagepreviewer.h"
#include <QtWidgets/QLabel>
#include <QtCore/QMimeDatabase>
#ifdef _ORIEA_PREVIEW_PDF
#include <QtPdf/QPdfDocument>
#endif

ImagePreviewer::ImagePreviewer(QWidget *parent)
    : QWidget(parent), m_viewing(new QLabel(this))
{
    m_viewing->setBackgroundRole(QPalette::Base);
    m_viewing->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_viewing->setScaledContents(true);
}

void ImagePreviewer::resizeEvent(QResizeEvent *) {
    if (m_viewing->pixmap().isNull())
        return;
    double wScl = static_cast<double>(width()) / m_viewing->pixmap().width();
    double hScl = static_cast<double>(height()) / m_viewing->pixmap().height();
    m_viewing->resize(std::min(wScl, hScl) * m_viewing->pixmap().size());

    m_viewing->move((width() - m_viewing->width()) / 2,
                    (height() - m_viewing->height()) / 2);
}

bool ImagePreviewer::setPreview(const QPixmap &img) {
    if (!img.isNull()) {
        m_viewing->setPixmap(img);
        resizeEvent(nullptr);
        return true;
    }
    loadFail(tr("Image failed to load."));
    return false;
}

bool ImagePreviewer::setPreview(const QString &path) {
#ifdef _ORIEA_PREVIEW_PDF
    QMimeDatabase db;
    QMimeType mmt = db.mimeTypeForFile(path);
    if (mmt.name().contains(QStringLiteral("pdf"))) {
        QPdfDocument pdoc(this);
        pdoc.load(path);
        QImage img = pdoc.render(0, size() * 2);

        if (!setPreview(QPixmap::fromImage(img)))
            goto imgLoadFail;
        return true;
    } else if (mmt.name().contains(QStringLiteral("image/"))) {
        QPixmap img = QPixmap(path);
        if (img.isNull() || !setPreview(img))
            goto imgLoadFail;
        return true;
    }

#else
    QPixmap img = QPixmap(path);
    if (img.isNull() || !setPreview(img))
        goto imgLoadFail;
    return true;
#endif

imgLoadFail:
    if (path.contains(QStringLiteral(".pdf")))
#ifdef _ORIEA_PREVIEW_PDF
        loadFail(tr("%1\nPDF Document is corrupted").arg(path));
#else
        loadFail(tr("%1\nPDF Document Preview is not compiled "
                    "in this executable.").arg(path));
#endif
    loadFail(tr("File %1 failed to load.").arg(path));
    return false;
}

void ImagePreviewer::loadFail(const QString &msg) {
    m_viewing->setPixmap(QPixmap());
    m_viewing->setText(msg);
}

ImagePreviewer::~ImagePreviewer() = default;

