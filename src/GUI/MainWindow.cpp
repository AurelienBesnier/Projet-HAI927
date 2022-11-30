#include "MainWindow.h"
#include <QFileDialog>
#include <iostream>

MainWindow::MainWindow()
{
    setupUi(this);
}

void MainWindow::on_open_action_triggered()
{
    std::cout << "fbbvusbudsbvudrbv\n";
    QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files (*.png *.jpg *.bmp)"));
}
