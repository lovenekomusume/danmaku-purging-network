#ifndef LISTEDITFORM_H
#define LISTEDITFORM_H

#include <QWidget>
#include <QStandardItemModel>
#include <Delegate.h>
#include <DPList.h>

namespace Ui {
class ListEditForm;
}

class ListEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit ListEditForm(QWidget *parent = 0);
    explicit ListEditForm(DPList list, QWidget *parent = 0);
    ~ListEditForm();

private slots:
    void on_addButton_clicked();

    void on_saveButton_clicked();

    void on_keywordInput_returnPressed();

    void on_importButton_clicked();

private:
    Ui::ListEditForm *ui;
    QStandardItemModel *model;
    bool addKeywordItem(QString keyword = "", bool repeatWarning = true, bool addOnly = false);
    static void removeButtonClickHandler(ButtonDelegate *delegate, QAbstractItemModel *model, const QModelIndex &index);
    void initUI(ListEditForm* t);
};

#endif // LISTEDITFORM_H
