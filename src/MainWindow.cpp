#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "src/Delegate.h"
#include <QFile>
#include <QDir>
#include "src/DPList.h"
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QtXml/QDomDocument>
#include <QProcess>
#include "src/REList.h"


MainWindow* MainWindow::s_this;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    s_this = this;
    ui->setupUi(this);
    this->setWindowTitle("弹幕净化网络小工具 v1.0.0");
    listsModel = new QStandardItemModel();
    ui->listsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listsTable->setModel(listsModel);
    ui->listsTable->verticalHeader()->setDefaultSectionSize(24);
    ui->listsTable->setMouseTracking(true);
    ButtonDelegate *bd1 = new ButtonDelegate();
    bd1->setText("禁用", "禁用", "启用", "启用");
    bd1->setColor(QColor(Qt::gray), QColor(Qt::gray), QColor(0, 204, 68), QColor(0, 204, 68));
    bd1->setClickHandler(switchListStatus);
    ui->listsTable->setItemDelegateForColumn(0, bd1);

    ButtonDelegate *bd2 = new ButtonDelegate();
    bd2->setText("编辑");
    bd2->setColor(QColor(Qt::gray), QColor(Qt::black));
    bd2->setClickHandler(editList);
    ui->listsTable->setItemDelegateForColumn(1, bd2);

    loadList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listCreateButton_clicked()
{

    ListEditForm *listCreateForm = new ListEditForm();
    listCreateForm->setAttribute(Qt::WA_DeleteOnClose);
    listCreateForm->setAttribute(Qt::WA_QuitOnClose, false);
    listCreateForm->setWindowTitle("新建列表");
    listCreateForm->show();

}

void MainWindow::loadList()
{
    ui->introBrowser->setText("正在加载可用列表，请稍候...");

    lists.clear();
    ButtonDelegate *delegate = (ButtonDelegate*)ui->listsTable->itemDelegateForColumn(0);
    delegate->statusMap->clear();

    QString rootPath = QApplication::applicationDirPath();
    QDir dir(rootPath + "/mylists");
    QFileInfoList fileList = dir.entryInfoList();
    QFileInfo fileInfo;
    for(int i = 0; i < fileList.count(); i++)
    {
        fileInfo = fileList.at(i);
        if(fileInfo.fileName() == "." || fileInfo.fileName() == ".." || fileInfo.fileName().toLower().split(".").at(1) != "dplist")
            continue;
        try
        {
            DPList list(fileInfo.filePath());
            lists.append(list);
        }
        catch(QString err)
        {
            //QMessageBox::warning(this, "提示", err);
            continue;
        }
    }

    // 显示在QTableView中
    listsModel->clear();
    listsModel->setHorizontalHeaderItem(0, new QStandardItem("状态"));
    listsModel->setHorizontalHeaderItem(1, new QStandardItem("编辑"));
    listsModel->setHorizontalHeaderItem(2, new QStandardItem("标题"));
    listsModel->setHorizontalHeaderItem(3, new QStandardItem("作者"));
    listsModel->setHorizontalHeaderItem(4, new QStandardItem("关键词统计"));
    listsModel->setHorizontalHeaderItem(5, new QStandardItem("创建时间"));

    for(int i = 0; i < lists.count(); i++)
    {
        DPList list = lists.at(i);
        listsModel->setItem(i, 2, new QStandardItem(list.title));
        listsModel->setItem(i, 3, new QStandardItem(list.author));
        listsModel->setItem(i, 4, new QStandardItem(QString::number(list.items.count())));
        listsModel->setItem(i, 5, new QStandardItem(list.date.toString("yyyy-MM-dd")));
    }

    ui->listsTable->resizeColumnsToContents();
    ui->listsTable->setColumnWidth(0, 35);
    ui->listsTable->setColumnWidth(1, 35);
    ui->introBrowser->setText("可用列表加载完成！");
}

REList MainWindow::generateREList(DPList list)
{
    REList rList = REList(list);
    return rList;
}

void MainWindow::on_listsTable_clicked(const QModelIndex &index)
{
    ui->introBrowser->setText(lists.at(index.row()).intro);
}

void MainWindow::switchListStatus(ButtonDelegate *delegate, QAbstractItemModel *model, const QModelIndex &index)
{
    delegate->switchStatus(index);
}

void MainWindow::editList(ButtonDelegate *delegate, QAbstractItemModel *model, const QModelIndex &index)
{
    ListEditForm *listEditForm = new ListEditForm(s_this->lists.at(index.row()));
    listEditForm->setAttribute(Qt::WA_DeleteOnClose);
    listEditForm->setAttribute(Qt::WA_QuitOnClose, false);
    listEditForm->setWindowTitle("编辑列表");
    listEditForm->show();
}

bool MainWindow::exportXML(QString itemPrefix, QString filename)
{
    try
    {
        ButtonDelegate *delegate = (ButtonDelegate*)ui->listsTable->itemDelegateForColumn(0);
        QMap<int, int> *sm = delegate->statusMap;
        QMap<int, int>::iterator it = sm->begin();
        QDomDocument doc;
        QDomElement root = doc.createElement("filters");
        doc.appendChild(root);

        while(it != sm->end())
        {
            if(it.value())
            {
                // 添加条目
                REList rl = generateREList(lists.at(it.key()));
                while(!rl.isEnd())
                {
                    QDomElement item = doc.createElement("item");
                    item.setAttribute("enabled", "true");
                    item.appendChild(doc.createTextNode(itemPrefix + "=" + rl.next()));
                    root.appendChild(item);
                }
            }

            ++it;
        }
        if(root.childNodes().size() > 0)
        {
            QString filePath = filename + ".xml";
            QFile *file = new QFile(filePath);
            if(!file->open(QIODevice::WriteOnly | QIODevice::Text))
                throw("导出发生错误！");
            QTextStream out(file);
            out.setCodec("UTF-8");
            doc.save(out, 4, QDomNode::EncodingFromTextStream);
            file->close();
            return true;
        }
        else
            QMessageBox::warning(this, "提示", "请至少启用一个列表！");
        return false;
    }
    catch(...)
    {
        QMessageBox::critical(this, "错误", "生成结果时发生错误，请重试！");
        return false;
    }
}

void MainWindow::on_exportButton_clicked()
{
    QString path = QApplication::applicationDirPath() + "/export/";
    QDir dir = QDir(path);
    if(!dir.exists())
        dir.mkdir(path);
    QString filename = path + "导出列表" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    if(exportXML("r", filename + "_for_flash") && exportXML("t", filename + "_for_html5"))
        QMessageBox::information(this, "提示", "目标导出成功！文件名为" + filename + "_for_xxx.xml");
}

void MainWindow::on_flushListButton_clicked()
{
    loadList();
}

void MainWindow::openDir(QString path)
{
    QDir dir = QDir(path);
    if(!dir.exists())
        dir.mkdir(path);
    path.replace("/","\\");
    QProcess::startDetached("explorer " + path);
}

void MainWindow::on_openListDirButton_clicked()
{
    QString path = QApplication::applicationDirPath() + "/mylists/";
    openDir(path);
}

void MainWindow::on_openExportDirButton_clicked()
{
    QString path = QApplication::applicationDirPath() + "/export/";
    openDir(path);
}

void MainWindow::on_selectAllListsButton_clicked()
{
    ButtonDelegate *delegate = (ButtonDelegate*)ui->listsTable->itemDelegateForColumn(0);
    QMap<int, int> *sm = delegate->statusMap;

    for(int i = 0; i < listsModel->rowCount(); i++)
    {
        sm->insert(i, 1);
    }

    ui->listsTable->repaint();
}

void MainWindow::on_clearSelectedListsButton_clicked()
{
    ButtonDelegate *delegate = (ButtonDelegate*)ui->listsTable->itemDelegateForColumn(0);
    QMap<int, int> *sm = delegate->statusMap;

    for(int i = 0; i < listsModel->rowCount(); i++)
    {
        sm->insert(i, 0);
    }

    ui->listsTable->repaint();
}

void MainWindow::on_action_about_this_tool_triggered()
{
    QString aboutStr = "<p>弹幕净化网络小工具 v1.0.0</p>";
    aboutStr += "<p>弹幕净化网络这个名字启发自御坂网络，该项目旨在让需要的人能够更好地体验B站弹幕带来的乐趣，帮助大家屏蔽一些可能会引起不适的字词，或让各位分享自己的屏蔽经验。</p>";
    aboutStr += "<p>本工具使用 QT 5.7.1 制作。</p><p>作者: 月之庭</p><p>联系作者: lovenekomusume@163.com</p><p>本工具为免费开源软件，请勿用于任何商业用途，谢谢！</p>";
    aboutStr += "<p>License(开源协议): GPLv3</p>";
    aboutStr += "<p><a href='www.1000-7.com/danmaku-purging-network/'>官方主页</a></p>";
    aboutStr += "<p><a href='www.1000-7.com'>作者的小站</a></p>";
    QMessageBox::information(this, "关于本工具", aboutStr);
}
