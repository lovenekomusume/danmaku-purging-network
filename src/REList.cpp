#include "src/REList.h"
#include "src/DPList.h"
#include <QDebug>


REList::REList(DPList dpList)
{
    int itemCount = dpList.items.count();
    QString item = "";
    QString keyword = "";
    QString temp = "";

    this->rList.append("--" + dpList.title + "@" + dpList.author + "--");

    for(int i = 0; i < itemCount; i++)
    {
        keyword = dpList.items.at(i);
        temp = item + "(" + keyword + ")|";

        if(temp.length() > 48)
        {

            this->rList.append(item.mid(0, item.length() - 1));
            item = "";
        }

        item += "(" + keyword + ")|";


        if(i + 1 == itemCount)
            this->rList.append(item.mid(0, item.length() - 1));
    }

    this->count = this->rList.count();
}

bool REList::isEnd()
{
    if(this->index >= this->count)
        return true;
    return false;
}

QString REList::next()
{
    if(this->index < this->count)
        return this->rList.at(index++);
    return "";
}
