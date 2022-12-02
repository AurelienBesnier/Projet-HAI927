#include "MainWindow.h"
#include <QFileDialog>
#include <filesystem>
#include <QMouseEvent>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


namespace fs = std::filesystem;

MainWindow::MainWindow()
{
    setupUi(this);
    image_label->setParent(nullptr);
    scroll_area->setWidget(image_label);
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if(!image_label->underMouse()) return;
    _last_click = getImageCoord(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if(!image_label->underMouse()) return;
    auto coords = getImageCoord(event);
    std::tie(_selection.x, _selection.width)  = std::minmax(_last_click.x(), coords.x()); _selection.width -= _selection.x;
    std::tie(_selection.y, _selection.height) = std::minmax(_last_click.y(), coords.y()); _selection.height -= _selection.y;
    showSelection();
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if(!image_label->underMouse()) return;
    auto coords = image_label->mapFrom(this, event->pos());
}

void MainWindow::on_open_action_triggered()
{
    fs::path path = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files (*.png *.jpg *.bmp)")).toStdString();
    _img = cv::imread(path, cv::IMREAD_COLOR);
    updateDisplay(_img);
    image_label->adjustSize();
    _scale = 1;
}

void MainWindow::on_evaluate_button_pressed()
{
    //TODO
}

 void MainWindow::on_zoom_in_action_triggered()
 {
    _scale += 0.125f;
    updateScale();
 }

 void MainWindow::on_zoom_out_action_triggered()
 {
    if(_scale > 0.125f) _scale -= 0.125f;
    updateScale();
 }

 QPoint MainWindow::getImageCoord(QMouseEvent* event) const
 {
    return image_label->mapFrom(this, event->pos()) / _scale;
 }

 void MainWindow::updateDisplay(const cv::Mat& img)
 {
    image_label->setPixmap(QPixmap::fromImage(QImage{img.data, img.cols, img.rows, img.step, QImage::Format_BGR888}));
 }

 void MainWindow::updateScale()
 {
    image_label->resize(image_label->pixmap(Qt::ReturnByValue).size() * _scale);
 }

 void MainWindow::showSelection()
 {
    cv::Mat img = _img.clone();
    cv::rectangle(img, _selection, cv::Scalar(0));
    updateDisplay(img);
 }
