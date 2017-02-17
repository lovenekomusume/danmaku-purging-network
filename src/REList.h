#ifndef RELIST_H
#define RELIST_H

#include "src/DPList.h"

class REList
{
public:
    REList(DPList dpList);
    QList<QString> rList;
    int index = 0;
    int count = 0;
    QString next();
    bool isEnd();
};

#endif // RELIST_H
