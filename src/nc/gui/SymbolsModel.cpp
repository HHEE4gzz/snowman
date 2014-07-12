/* The file is part of Snowman decompiler.             */
/* See doc/licenses.txt for the licensing information. */

#include "SymbolsModel.h"

#include <QStringList>

#include <nc/common/CheckedCast.h>
#include <nc/common/Unreachable.h>
#include <nc/common/Warnings.h>

#include <nc/core/image/Image.h>
#include <nc/core/image/Section.h>

namespace nc { namespace gui {

enum SymbolsModelColumns {
    COL_NAME,
    COL_TYPE,
    COL_VALUE,
    COL_SECTION,
    COL_COUNT
};

SymbolsModel::SymbolsModel(QObject *parent):
    QAbstractItemModel(parent)
{
    updateContents();
}

void SymbolsModel::setImage(const std::shared_ptr<const core::image::Image> &image) {
    if (image != image_) {
        image_ = image;
        updateContents();
    }
}

void SymbolsModel::updateContents() {
    beginResetModel();
    endResetModel();
}

const core::image::Symbol *SymbolsModel::getSymbol(const QModelIndex &index) const {
    return static_cast<const core::image::Symbol *>(index.internalPointer());
}

int SymbolsModel::rowCount(const QModelIndex &parent) const {
    if (!image()) {
        return 0;
    }
    if (parent == QModelIndex()) {
        return checked_cast<int>(image()->symbols().size());
    } else {
        return 0;
    }
}

int SymbolsModel::columnCount(const QModelIndex & /*parent*/) const {
    return COL_COUNT;
}

QModelIndex SymbolsModel::index(int row, int column, const QModelIndex &parent) const {
    if (!image()) {
        return QModelIndex();
    }
    if (row < rowCount(parent)) {
        return createIndex(row, column, (void *)image()->symbols()[row]);
    } else {
        return QModelIndex();
    }
}

QModelIndex SymbolsModel::parent(const QModelIndex & /*index*/) const {
    return QModelIndex();
}

QVariant SymbolsModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole || role == SortRole) {
        auto symbol = getSymbol(index);
        assert(symbol);

        switch (index.column()) {
            case COL_NAME: return symbol->name();
            case COL_TYPE: {
                using core::image::Symbol;

                switch (symbol->type()) {
                    case Symbol::NOTYPE:
                        return tr("None");
                    case Symbol::FUNCTION:
                        return tr("Function");
                    case Symbol::OBJECT:
                        return tr("Object");
                    case Symbol::SECTION:
                        return tr("Section");
                    default:
                        ncWarning("Unknown symbol type: %1.", symbol->type());
                        return tr("Unknown");
                }
            }
            case COL_VALUE: {
                if (role == Qt::DisplayRole) {
                    return QString("%1").arg(symbol->value(), 0, 16);
                } else {
                    return static_cast<qlonglong>(symbol->value());
                }
            }
            case COL_SECTION: return symbol->section() ? symbol->section()->name() : QString();
            default:
                unreachable();
        }
    }
    return QVariant();
}

QVariant SymbolsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
                case COL_NAME: return tr("Name");
                case COL_TYPE: return tr("Type");
                case COL_VALUE: return tr("Value");
                case COL_SECTION: return tr("Section");
                default: unreachable();
            }
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

}} // namespace nc::gui
/* vim:set et sts=4 sw=4: */
