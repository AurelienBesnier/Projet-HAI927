#include "MainWindow.h"
#include <QFileDialog>
#include <filesystem>
#include <QMouseEvent>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "Commands.h"

using namespace std::placeholders;
namespace fs = std::filesystem;


namespace
{
    void blur(cv::Mat& img, cv::Rect roi, int radius)
    {
        cv::GaussianBlur(img(roi), img(roi), {radius*2+1,radius*2+1}, 0);
    }

    void pixel(cv::Mat& img, cv::Rect roi, int size)
    {
        roi.height = (roi.height + size) / (size + 1) * (size + 1);
        roi.width = (roi.width + size) / (size + 1) * (size + 1);
        if(roi.height + roi.y > img.rows || roi.width + roi.x > img.cols)
        {
            return;
        }
        cv::Mat area = img(roi);
        cv::Mat smol(roi.height / (size + 1), roi.width / (size + 1), area.type());
        cv::resize(area, smol, smol.size(), 0, 0, cv::INTER_AREA);
        cv::resize(smol, area, area.size(), 0, 0, cv::INTER_NEAREST);
    }

    void blackhead(cv::Mat& img, cv::Rect roi)
    {
        img(roi) *= 0;
    }
}

MainWindow::MainWindow()
{
    setupUi(this);

    //Remove intermediate widget in scroll area
    image_label->setParent(nullptr);
    scroll_area->setWidget(image_label);
    
    //Build undo system
    _undo_stack = new QUndoStack(this);
    QAction* action = _undo_stack->createUndoAction(this, tr("Undo"));
    action->setShortcuts(QKeySequence::Undo);
    edit_menu->addAction(action);
    action = _undo_stack->createRedoAction(this, tr("Redo"));
    action->setShortcuts(QKeySequence::Redo);
    edit_menu->addAction(action);
}

const cv::Mat& MainWindow::image() const
{
    return _img;
}

void MainWindow::setImage(cv::Mat img)
{
    _img = std::move(img);
    updateDisplay();
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
    updateDisplay();
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if(!image_label->underMouse()) return;
    auto coords = image_label->mapFrom(this, event->pos());
}

void MainWindow::on_open_action_triggered()
{
    fs::path path = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)")).toStdString();
    if(path.empty()) return;
    _img = cv::imread(path, cv::IMREAD_COLOR);
    updateDisplay();
    image_label->adjustSize();
    _scale = 1;
    save_action->setEnabled(true);
}

void MainWindow::on_save_action_triggered()
{
    fs::path path = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Image Files (*.png *.jpg *.bmp)")).toStdString();
    if(path.empty()) return;
    if(!path.has_extension()) path.replace_extension(".png");
    cv::imwrite(path, _img);
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


void MainWindow::on_blur_button_pressed()
{
    _undo_stack->push(new FilterCommand(this, std::bind(blur, _1, _selection, blur_slider->value())));
}

void MainWindow::on_pixel_button_pressed()
{
    _undo_stack->push(new FilterCommand(this, std::bind(pixel, _1, _selection, pixel_slider->value())));
}

void MainWindow::on_blackhead_button_pressed()
{
    _undo_stack->push(new FilterCommand(this, std::bind(blackhead, _1, _selection)));
}

QPoint MainWindow::getImageCoord(QMouseEvent* event) const
{
    return image_label->mapFrom(this, event->pos()) / _scale;
}

void MainWindow::updateDisplay()
{
    cv::Mat img = _img.clone();
    cv::rectangle(img, _selection, cv::Scalar(0));
    image_label->setPixmap(QPixmap::fromImage(QImage{img.data, img.cols, img.rows, img.step, QImage::Format_BGR888}));
}

void MainWindow::updateScale()
{
    image_label->resize(image_label->pixmap(Qt::ReturnByValue).size() * _scale);
}
