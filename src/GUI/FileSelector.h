#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QWidget>
#include "ui_FileSelector.h"


class FileSelector : public QWidget, private Ui::FileSelector
{
    Q_OBJECT
public:
    FileSelector(QWidget* parent = nullptr);
    QString path() const;

    QString filter;
    bool for_saving = false;

signals:
    void pathChanged(QString path);

public slots:
    void setPath(QString path);

private slots:
    void on_browse_button_clicked();
    void on_file_edit_textChanged(QString str);
};

#endif
