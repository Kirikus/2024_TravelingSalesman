#include "naming_table.h"

#include "QVariant"
#include "manager.h"

namespace lib {

int NamingTable::rowCount(const QModelIndex &parent) const {
  return Manager::GetInstance()->GetVariablesCount();
}

int NamingTable::columnCount(const QModelIndex &parent) const {
  return columns_data::kCount;
}

QVariant NamingTable::data(const QModelIndex &index, int role) const {
  switch (role) {
    case Qt::DisplayRole:
      switch (index.column()) {
        case columns_data::kTitle:
          return Manager::GetInstance()->GetVariable(index.row()).naming.title;
        case columns_data::kTag:
          return Manager::GetInstance()->GetVariable(index.row()).naming.tag;
      }
    default:
      return QVariant();
  }
}

bool NamingTable::setData(const QModelIndex &index, const QVariant &value,
                          int role) {
  Variable::Naming &naming =
      Manager::GetInstance()->GetVariable(index.row()).naming;
  switch (role) {
    case Qt::EditRole:
      switch (index.column()) {
        case columns_data::kTitle:
          value.toString().isEmpty() ? naming.title = "unnamed"
                                     : naming.title = value.toString();
          emit dataChanged(index, index);
          return true;
        case columns_data::kTag:
          naming.tag = value.toString();
          emit dataChanged(index, index);
          return true;
      }
    default:
      return false;
  }
}

QVariant NamingTable::headerData(int section, Qt::Orientation orientation,
                                 int role) const {
  switch (role) {
    case Qt::DisplayRole:
      switch (orientation) {
        case Qt::Vertical:
          return section + 1;
        case Qt::Horizontal:
          switch (section) {
            case kTitle:
              return QString("Title");
            case kTag:
              return QString("Tag");
          }
      }
    default:
      return QVariant();
  }
}

Qt::ItemFlags NamingTable::flags(const QModelIndex &index) const {
  return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

void NamingTable::insertRow(int index) {
  beginInsertRows(QModelIndex(), index, index);
  endInsertRows();
}

void NamingTable::removeRow(int index) {
  beginRemoveRows(QModelIndex(), index, index);
  endRemoveRows();
}

}  // namespace lib
