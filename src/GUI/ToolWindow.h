#ifndef TOOLWINDOW_H
#define TOOLWINDOW_H

#include <QWidget>
#include "ui_ToolWindow.h"


class ToolWindow : public QWidget, private Ui::ToolWindow
{
    Q_OBJECT
public:
    ToolWindow(QWidget* parent = nullptr);

private slots:
    void on_genpairs_button_clicked();
    void on_gentriplets_button_clicked();
    void on_annotate_button_clicked();
    void on_obscurate_button_clicked();
    void checkButtons();

private:
    template<typename F>
    void runTask(F&& task);
};

#endif
