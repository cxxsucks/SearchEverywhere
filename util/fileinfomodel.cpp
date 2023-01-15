#include "fileinfomodel.hpp"
#include <QtCore/QDateTime>
#include <QtWidgets/QFileIconProvider>

namespace seev {

FileinfoModel::FileinfoModel(QObject* obj) 
    : QAbstractTableModel(obj) {}

bool FileinfoModel::insertRows(int row, int count, const QModelIndex &par) {
    if (row > infos.size() || row < 0 || count <= 0)
        return false;
    beginInsertRows(par, row, row + count - 1);
#if QT_VERSION_MAJOR >= 6
    infos.insert(row, count, QFileInfo());
#else // Very inefficient insertion in Qt5 :(
    for (int i = 0; i < count; ++i)
        infos.insert(row, QFileInfo());
#endif
    endInsertRows();
    return true;
}

bool FileinfoModel::removeRows(int row, int count, const QModelIndex &par) {
    if (row < 0 || count <= 0 || row + count > infos.size())
        return false;
    beginRemoveRows(par, row, row + count - 1);
#if QT_VERSION_MAJOR >= 6
    infos.remove(row, count);
#else
    infos.erase(infos.begin() + row, infos.begin() + count);
#endif
    endRemoveRows();
    return true;
}

void FileinfoModel::clear() {
    beginResetModel();
    infos.clear();
    endResetModel();
}

void FileinfoModel::addInfo(const QFileInfo &info) {
    qsizetype oldSize = infos.size();
    insertRow(oldSize);
    infos.back() = info;
    emit dataChanged(
        createIndex(oldSize, 0),
        createIndex(oldSize, 2)
    );
}

QVariant FileinfoModel::headerData(int section, Qt::Orientation orint,
                                     int role) const {
    if (role != Qt::DisplayRole) return QVariant();
    if (orint == Qt::Vertical)
        return section;

    switch (section) {
        case 0: return tr("Name");
        case 1: return tr("Size");
        case 2: return tr("Modified");
    }
    return QVariant();
}

QVariant FileinfoModel::data(const QModelIndex &index, int role) const {
    const QFileInfo& toShow = infos.at(index.row());
    if (role == Qt::DecorationRole && index.column() == 0)
        return QFileIconProvider().icon(toShow);
    else if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column()) {
        case 0: return toShow.fileName();
        case 1: return numToSizeStrBad(toShow.size());
        case 2: return toShow.lastModified().toString(tr("hh:mm dd-MM-yy"));
    }
    return QVariant();
}

QString FileinfoModel::numToSizeStrBad(qsizetype sz) {
    static constexpr char suff[] = " KMGT";
    double suffSz = sz; int suffIdx = 0;
    while (suffSz >= 1000.0) {
        suffSz /= 1000.0;
        ++suffIdx;
    }
    return QString::number(suffSz, 'f', 1) + suff[suffIdx];
}

}
