#include "src/Delegate.h"
#include <QPushButton>
#include <QApplication>
#include <QWindow>
#include <QMouseEvent>
#include <iostream>
#include <QMessageBox>
#include <QDebug>

using namespace std;

ButtonDelegate::ButtonDelegate(QObject *parent)
{
    isMouseOver = false;
    clickHandler_0 = NULL;
    clickHandler_1 = NULL;
    statusMap = new QMap<int, int>();
    setColor();
    setText();
}

void ButtonDelegate::setColor(QColor color_normal, QColor color_mouse_move, QColor color_status_1, QColor color_status_1_mouse_over)
{
    this->color_normal = color_normal;
    this->color_mouse_move = color_mouse_move;
    this->color_status_1 = color_status_1;
    this->color_status_1_mouse_over = color_status_1_mouse_over;
}

void ButtonDelegate::setText(QString text_normal, QString text_mouse_move, QString text_status_1, QString text_status_1_mouse_over)
{
    this->text_normal = text_normal;
    this->text_mouse_move = text_mouse_move == "" ? text_normal : text_mouse_move;
    this->text_status_1 = text_status_1 == "" ? text_normal : text_status_1;
    this->text_status_1_mouse_over = text_status_1_mouse_over == "" ? text_normal : text_status_1_mouse_over;
}

void ButtonDelegate::setClickHandler(void (*handler)(ButtonDelegate *delegate, QAbstractItemModel *model, const QModelIndex &index), int status)
{
    if(status == 0)
        clickHandler_0 = handler;
    else if(status == 1)
        clickHandler_1 = handler;
    else
    {
        clickHandler_0 = handler;
        clickHandler_1 = handler;
    }
}

void ButtonDelegate::switchStatus(const QModelIndex &index, int status)
{
    int row = index.row();
    if(status == -1)
        statusMap->insert(row, statusMap->value(row) == 0 ? 1 : 0);
    else if(status == 0 || status == 1)
        statusMap->insert(row, status);

}

void ButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem *button = new QStyleOptionViewItem();
    int status = statusMap->value(index.row());
    button->rect = option.rect.adjusted(5, 5, -5, -5);
    button->state |= QStyle::State_Enabled;
    button->displayAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
    QColor color = isMouseOver ? (status == 0 ? color_mouse_move : color_status_1_mouse_over) : (status == 0 ? color_normal : color_status_1);
    button->palette.setColor(QPalette::WindowText, color);
    button->text = isMouseOver ? (status == 0 ? text_mouse_move : text_status_1_mouse_over) : (status == 0 ? text_normal : text_status_1);
    QApplication::style()->drawItemText(painter, button->rect, button->displayAlignment, button->palette, button->state, button->text, QPalette::WindowText);
}

bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QApplication::restoreOverrideCursor();
    isMouseOver = false;
    int status = statusMap->value(index.row());
    QRect rect = option.rect;
    QMouseEvent *e = (QMouseEvent*)event;
    if(rect.adjusted(5, 5, -5, -5).contains(e->pos()))
        if(event->type() == QEvent::MouseMove)
        {
            QApplication::setOverrideCursor(Qt::PointingHandCursor);
            isMouseOver = true;
        }
        else if(event->type() == QEvent::MouseButtonRelease){
            if(status == 0 && clickHandler_0)
                (*clickHandler_0)(this, model, index);
            else if(status == 1 && clickHandler_1)
                (*clickHandler_1)(this, model, index);
        }
    return true;
}
