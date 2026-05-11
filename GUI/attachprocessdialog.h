#pragma once

#include "qtcommon.h"
#include "ui_attachprocessdialog.h"
#include <QStandardItemModel>

class AttachProcessDialog : public QDialog
{
public:
    struct ProcessInfo
    {
        DWORD id;
        QString fileName;
        QString path;
        HICON icon;
    };

    explicit AttachProcessDialog(QWidget* parent, const std::vector<ProcessInfo>& processes);
    QString SelectedProcess();

private:
    Ui::AttachProcessDialog ui;
    QStandardItemModel model;
};
