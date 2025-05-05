#pragma once

#include <QStyledItemDelegate>

class LyricsDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit LyricsDelegate(QObject *parent = nullptr);

    void setHighlightedIndex(int index);
    void setMaxWidth(int width); // New method to set maximum width for lyrics

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    int highlightedIndex;
    int maxWidth; // Stores the maximum width for lyrics
};
