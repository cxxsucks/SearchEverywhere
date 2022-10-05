#ifndef SEEV_ORIEPREDSELECTOR_HPP
#define SEEV_ORIEPREDSELECTOR_HPP

#include <QWidget>

namespace seev {

namespace Ui {
class OriePredSelector;
}

class OriePredSelector : public QWidget
{
    Q_OBJECT

public:
    QString genCommand() const;
    // TODO: fromString

    explicit OriePredSelector(QWidget *parent = nullptr);
    ~OriePredSelector();

private:
    Ui::OriePredSelector *ui;

    QString __genPathCommand() const;
    QString __genContCommand() const;
    QString __genFileStatCommand() const;
    QString __genAccessCommand() const;
    QString __genFileTypeCommand() const;
    QString __genModifCommand(QString modifName) const;
    QString __genBridgeCommand(QString bridName) const;

private slots:
    void onPredKindChange(int index);
};


} // namespace seev
#endif // SEEV_ORIEPREDSELECTOR_HPP
