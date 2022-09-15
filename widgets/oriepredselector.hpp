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
    explicit OriePredSelector(QWidget *parent = nullptr);
    ~OriePredSelector();

private:
    Ui::OriePredSelector *ui;
};


} // namespace seev
#endif // SEEV_ORIEPREDSELECTOR_HPP
