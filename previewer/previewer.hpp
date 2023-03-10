#pragma once
#include <QtWidgets/QWidget>
#include <thread>

QT_BEGIN_NAMESPACE
namespace Ui { class Previewer; }
QT_END_NAMESPACE

namespace seev {

class Previewer : public QWidget
{
    Q_OBJECT

public:
    explicit Previewer(QWidget *parent = nullptr,
        const QString& sofficePath = QStringLiteral("soffice"),
        bool showOpen = false);
    ~Previewer();

    QString previewPath() const {return m_viewingPath;}

public slots:
    void setPreviewPath(const QString& path);
    void selectedPreviewPath();
    void clearPreview();

signals:
    void officeConvertFinished(int execRet);

private:
    ::Ui::Previewer *ui;
    QString m_viewingPath;
    QString m_sofficePath;
    std::thread m_officeConvertThread;

    void _media_previewImpl(const QString &path);
    void _image_previewImpl(const QString &path);
    void _txt_previewImpl(const QString &path);
    void _html_previewimpl(const QUrl &path, bool md);
    void _dir_previewimpl(const QString &path);
    void _office_previewimpl(const QString& path);

private slots:
    void onOpenFileClicked();
    void onOpenDirClicked();
    void onCopyFilePathClicked();
    void onCopyDirPathClicked();
    void onOfficeCvtFinish(int ret);

protected:
    void resizeEvent(QResizeEvent*) override;
};
}
