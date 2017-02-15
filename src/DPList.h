#ifndef DPLIST_H
#define DPLIST_H

#include <Qt>
#include <QDateTime>


class DPList
{
public:
    DPList(QString dir);
    QString title;
    QString author;
    QDate date;
    QString intro;
    QString dir;
    QList<QString> items;
};

#endif // DPLIST_H
