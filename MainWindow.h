#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ListEditForm.h"
#include <DPList.h>
#include <Delegate.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_listCreateButton_clicked();
    void on_listsTable_clicked(const QModelIndex &index);
    void on_exportButton_clicked();
    void on_flushListButton_clicked();
    void on_openListDirButton_clicked();
    void on_openExportDirButton_clicked();
    void on_selectAllListsButton_clicked();
    void on_clearSelectedListsButton_clicked();

    void on_action_about_this_tool_triggered();

private:
    static MainWindow *s_this;
    Ui::MainWindow *ui;
    void loadList();
    QList<DPList> lists;
    QStandardItemModel *listsModel;
    static void switchListStatus(ButtonDelegate *delegate, QAbstractItemModel *model, const QModelIndex &index);
    QString generateRegex(DPList list);
    static void editList(ButtonDelegate *delegate, QAbstractItemModel *model, const QModelIndex &index);
    void openDir(QString path);
};

#endif // MAINWINDOW_H
