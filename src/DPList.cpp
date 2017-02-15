#include "src/DPList.h"
#include <QFile>
#include <stdexcept>
#include <qxmlstream.h>

using namespace std;


DPList::DPList(QString dir)
{
    try
    {
        this->dir = dir;
        QFile *file = new QFile(dir);
        if(!file->exists())
            return;

        if(!file->open(QIODevice::ReadOnly))
            throw (dir + " 打开失败！");

        QXmlStreamReader *reader = new QXmlStreamReader(file);
        QStringRef name;

        while(!reader->atEnd() && !reader->hasError())
        {
            reader->readNext();
            if(reader->isStartElement())
            {
                name = reader->name();

                if(name == "title")
                    title = reader->readElementText();
                else if(name == "author")
                    author = reader->readElementText();
                else if(name == "date")
                    date = QDate::fromString(reader->readElementText(), "yyyy-MM-dd");
                else if(name == "intro")
                    intro = reader->readElementText();
                else if(name == "item")
                    items.append(reader->readElementText());
            }
        }

        file->close();
    }
    catch(QString err)
    {
        throw err;
    }


}
