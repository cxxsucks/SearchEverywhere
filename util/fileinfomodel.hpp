#pragma once
#include <QtCore/QAbstractTableModel>
#include <QtCore/QFileInfoList>
class QFileInfo;

namespace seev {

class FileinfoModel : public QAbstractTableModel {
    Q_OBJECT
private:
    QFileInfoList infos;

public:
    bool insertRows(int row, int count, const QModelIndex& par = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& par = QModelIndex()) override;
    int columnCount(const QModelIndex& = QModelIndex()) const override { return 3; }
    int rowCount(const QModelIndex& = QModelIndex()) const override {return infos.size();}

    QVariant headerData(int section, Qt::Orientation orint,
                        int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // Qt::ItemFlags flags(const QModelIndex &index) const override;

    void clear();
    void addInfo(const QFileInfo& info);
    const QFileInfo& at(qsizetype sz) const {return infos.at(sz);}

    explicit FileinfoModel(QObject* obj = nullptr);
    ~FileinfoModel() override = default;

    static QString numToSizeStrBad(qsizetype sz);
};

}