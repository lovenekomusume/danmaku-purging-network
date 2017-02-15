#include <iostream>
#include "ListEditForm.h"
#include "ui_ListEditForm.h"
#include <QStandardItem>
#include <QAbstractItemModel>
#include <QMessageBox>
#include "Delegate.h"
#include <fstream>
#include <QApplication>
#include <QDir>
#include <QTextStream>
#include <stdexcept>
#include <QModelIndex>
#include <QDateTime>
#include <qxmlstream.h>
#include <QFileDialog>
#include <QDebug>

using namespace std;

void ListEditForm::initUI(ListEditForm *t)
{
    t->ui->setupUi(t);
    t->model = new QStandardItemModel();
    t->model->setColumnCount(2);
    t->ui->keywordsTable->setModel(model);
    t->ui->keywordsTable->horizontalHeader()->hide();
    t->ui->keywordsTable->verticalHeader()->setDefaultSectionSize(24);
    t->ui->keywordsTable->horizontalHeader()->setDefaultSectionSize(50);
    t->ui->keywordsTable->setMouseTracking(true);
    ButtonDelegate *bd = new ButtonDelegate();
    bd->setText("移除");
    bd->setColor(QColor(255, 68, 102), QColor(170, 0, 34));
    bd->setClickHandler(removeButtonClickHandler);
    t->ui->keywordsTable->setItemDelegateForColumn(0, bd);
    t->ui->keywordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

ListEditForm::ListEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListEditForm)
{
    initUI(this);
}

ListEditForm::ListEditForm(DPList list, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListEditForm)
{
    initUI(this);

    ui->listTitleInput->setText(list.title);
    ui->listAuthorInput->setText(list.author);
    ui->listIntroInput->setPlainText(list.intro);

    for(int i = 0; i < list.items.count(); i++){
        model->setItem(i, 1, new QStandardItem(list.items.at(i)));
    }

    ui->keywordsTable->resizeColumnToContents(1);
}

ListEditForm::~ListEditForm()
{
    delete ui;
}

void ListEditForm::removeButtonClickHandler(ButtonDelegate *delegate, QAbstractItemModel *model, const QModelIndex &index)
{
    model->removeRow(index.row());
}

bool ListEditForm::addKeywordItem(QString keyword, bool repeatWarning, bool addOnly)
{
    if(keyword == "")
        keyword = ui->keywordInput->text();

    if(keyword == "")
        return false;

    int count = model->rowCount();

    for(int i = 0; i < count; i++)
    {
        if(model->index(i, 1).data().toString() == keyword)
        {
            if(repeatWarning)
                QMessageBox::warning(this, "提示", "关键字已存在！");
            return false;
        }
    }


    int nextRowIndex = count;
    QStandardItem *newItem = new QStandardItem(keyword);
    model->setItem(nextRowIndex, 1, newItem);

    if(!addOnly)
    {
        ui->keywordsTable->resizeColumnToContents(1);
        ui->keywordInput->clear();
        ui->keywordsTable->scrollToBottom();
    }

    return true;
}

void ListEditForm::on_addButton_clicked()
{
    addKeywordItem();
}

void ListEditForm::on_keywordInput_returnPressed()
{
    addKeywordItem();
}

void ListEditForm::on_saveButton_clicked()
{
    try
    {
        QString listTitle = ui->listTitleInput->text();
        QString listAuthor = ui->listAuthorInput->text();
        QString listIntro = ui->listIntroInput->toPlainText();
        QString listRootDir = QApplication::applicationDirPath() + "/mylists/";

        if(listTitle == "" || listAuthor == "" || listIntro == "")
        {
            QMessageBox::warning(this, "提示", "还有未填写的部分！");
            return;
        }

        int count = model->rowCount();

        if(count == 0)
        {
            QMessageBox::warning(this, "提示", "屏蔽列表里并没有条目！");
            return;
        }

        QDir dir(listRootDir);

        if(!dir.exists())
            dir.mkdir(listRootDir);

        QString filePath = listRootDir + listTitle + ".dplist";

        QFile file(filePath);

        if(file.exists())
        {
            QMessageBox::StandardButton btn = QMessageBox::question(this, "询问", "已有同名列表，是否覆盖？", QMessageBox::Yes|QMessageBox::No);
            if(btn == QMessageBox::No)
                return;
        }


        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            throw exception();


        //构成XML
        QXmlStreamWriter *writer = new QXmlStreamWriter(&file);
        writer->writeStartDocument();

        writer->writeStartElement("dplist");

        writer->writeTextElement("title", listTitle);
        writer->writeTextElement("author", listAuthor);
        writer->writeTextElement("date", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
        writer->writeTextElement("intro", listIntro);

        writer->writeStartElement("items");

        for(int i = 0; i < count; i++)
        {
            writer->writeTextElement("item", model->index(i, 1).data().toString());
        }

        writer->writeEndElement();
        writer->writeEndDocument();

        file.close();

        QMessageBox::information(this, "提示", "列表保存成功！");
    }
    catch(...)
    {
        QMessageBox::critical(this, "错误", "保存列表时发生错误，请重试！");
    }
}

void ListEditForm::on_importButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString("选择从B站导出的XML文件"), "",  QString("全部文件(*.xml *.dplist);;XML文件(*.xml);;DPLIST列表(*.dplist)"));
    if(fileName == "")
        return;

    QFile *file = new QFile(fileName);
    if(!file->open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "错误", "文件打开失败");
        return;
    }

    int mode = 0; // 0 - xml 1 - dplist

    if(fileName.toLower().split(".").at(1) == "dplist")
        mode = 1;

    int itemCount = 0;

    QXmlStreamReader *reader = new QXmlStreamReader(file);
    QString kw = "";

    while(!reader->atEnd() && !reader->hasError())
    {
        reader->readNext();
        if(reader->isStartElement())
        {
            if(reader->name() == "item")
            {
                kw = reader->readElementText();
                if(mode == 1 || (mode == 0 && kw.at(0) == 't'))
                    if(addKeywordItem(kw.mid(2 - mode * 2), false, true))
                        ++itemCount;
            }
        }
    }

    file->close();

    ui->keywordsTable->resizeColumnToContents(1);
    ui->keywordsTable->scrollToBottom();

    QMessageBox::information(this, "提示", QString("导入完成！共导入") + QString::number(itemCount) + "项。");
}
