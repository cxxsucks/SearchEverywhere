#ifndef IMAGEPREVIEWER_H
#define IMAGEPREVIEWER_H

#include <QtWidgets/QLabel>

class ImagePreviewer : public QWidget
{
    Q_OBJECT
    QLabel* m_viewing;

public:
    bool setPreview(const QPixmap&);
    bool setPreview(const QString&);

    ImagePreviewer(QWidget *parent = nullptr);
    ~ImagePreviewer();

protected:
    void resizeEvent(QResizeEvent*) override;

private:
    void loadFail(const QString& msg);
};
#endif // IMAGEPREVIEWER_H
