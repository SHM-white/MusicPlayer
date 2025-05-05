#include "LyricsDelegate.h"
#include <QPainter>
#include <QFontMetrics>

LyricsDelegate::LyricsDelegate(QObject *parent)
    : QStyledItemDelegate(parent), highlightedIndex(-1), maxWidth(0)
{
}

void LyricsDelegate::setHighlightedIndex(int index)
{
    highlightedIndex = index;
}

void LyricsDelegate::setMaxWidth(int width)
{
    maxWidth = width;
}

void LyricsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Replace two spaces with a line break
    QString text = opt.text;
    text.replace("  ", "\n");
    text.replace("//", "");

    // Center-align the text
    opt.displayAlignment = Qt::AlignCenter;

    // Adjust font size for highlighted lyric
    if (index.row() == highlightedIndex) {
        opt.font.setPointSize(opt.font.pointSize() + 4); // Increase font size for highlighted lyric
        opt.font.setBold(true);
    }

    // Draw the text with the updated options
    painter->save();
    painter->setFont(opt.font);

    // Calculate the bounding rectangle for the text with wrapping
    QFontMetrics metrics(opt.font);
    QRect textRect = metrics.boundingRect(0, 0, maxWidth, INT_MAX, Qt::TextWordWrap | opt.displayAlignment, text);

    // Adjust the height of the current lyric
    opt.rect.setHeight(textRect.height());

    painter->drawText(opt.rect, Qt::TextWordWrap | opt.displayAlignment, text);
    painter->restore();
}

QSize LyricsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Replace two spaces with a line break
    QString text = opt.text;
    text.replace("  ", "\n");

    // Adjust font size for highlighted lyric
    opt.font.setPointSize(opt.font.pointSize() + 4); // Increase font size for highlighted lyric
    // Calculate the size of the text with wrapping
    QFontMetrics metrics(opt.font);
    QRect textRect = metrics.boundingRect(0, 0, maxWidth, INT_MAX, Qt::TextWordWrap | Qt::AlignCenter, text);

    return QSize(textRect.width(), textRect.height() + 4);
}
