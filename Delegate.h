#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>

//! [0]
class ButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ButtonDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    void setClickHandler(void (*handler)(ButtonDelegate *delegate, QAbstractItemModel *model, const QModelIndex &index) = NULL, int status = -1);
    void setColor(QColor color_normal = QColor(Qt::black), QColor color_mouse_move = QColor(Qt::black), QColor color_status_1 = QColor(Qt::black), QColor color_status_1_mouse_over = QColor(Qt::black));
    void setText(QString text_normal = "text button", QString text_mouse_over = "", QString text_status_1 = "", QString text_status_1_mouse_over = "");
    void switchStatus(const QModelIndex &index, int status = -1);
    QMap<int, int> *statusMap;
private:
    bool isMouseOver;
    void (*clickHandler_0)(ButtonDelegate *delegate, QAbstractItemModel *model, const QModelIndex &index);
    void (*clickHandler_1)(ButtonDelegate *delegate, QAbstractItemModel *model, const QModelIndex &index);
    QColor color_normal;
    QColor color_mouse_move;
    QColor color_status_1;
    QColor color_status_1_mouse_over;
    QString text_normal;
    QString text_mouse_move;
    QString text_status_1;
    QString text_status_1_mouse_over;
};
//! [0]


#endif // DELEGATE_H
