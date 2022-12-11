#include "MainWindow.h"
#include <QFileDialog>
#include <filesystem>
#include <QMouseEvent>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "Commands.h"
#include "obscuration.h"
#include "ToolWindow.h"

using namespace std::placeholders;
namespace fs = std::filesystem;


namespace
{
    auto asTensor(const cv::Mat& img)
    {
        cv::Mat resized, converted;
        cv::resize(img, resized, cv::Size(250, 250));
        cv::cvtColor(resized, converted, cv::COLOR_BGR2RGB);
        return fdeep::tensor_from_bytes(converted.ptr(), converted.rows, converted.cols, converted.channels(), -1.0f, 1.0f);
    }

    float squared(float x) { return x*x; }

    template<typename F>
    struct RaiiCall
    {
        F&& func;
        ~RaiiCall()
        {
            func();
        }
    };
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

    statusBar()->showMessage("Loading model...");
    connect(this, SIGNAL(modelLoaded()), this, SLOT(handleModelLoaded()));
    _model_future = std::async(std::launch::async, &MainWindow::loadModel, this);
}

fdeep::model MainWindow::loadModel()
{
    RaiiCall rc{[&](){ emit modelLoaded(); }};
    return fdeep::load_model("model_trained.json");
}

void MainWindow::handleModelLoaded()
{
    try
    {
        _model = _model_future.get();
        statusBar()->showMessage("Model loaded!");
        checkEvaluateButton();
    }
    catch(std::exception& e)
    {
        statusBar()->showMessage(QString("Unable to load model : ") + e.what());
    }
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

void MainWindow::on_tools_action_triggered()
{
    ToolWindow* tw = new ToolWindow();
    tw->setAttribute(Qt::WA_DeleteOnClose);
    tw->setAttribute(Qt::WA_QuitOnClose, false);
    tw->show();
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
    checkEvaluateButton();
}

void MainWindow::on_save_action_triggered()
{
    fs::path path = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Image Files (*.png *.jpg *.bmp)")).toStdString();
    if(path.empty()) return;
    if(!path.has_extension()) path.replace_extension(".png");
    cv::imwrite(path, _img);
}

void MainWindow::on_evaluate_button_clicked()
{
    fs::path path = QFileDialog::getOpenFileName(this, tr("Open image for comparison"), "", tr("Image Files (*.png *.jpg *.bmp)")).toStdString();
    if(path.empty()) return;
    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
    const fdeep::tensor result[] = {
        _model->predict({asTensor(_img)})[0],
        _model->predict({asTensor(img)})[0],
    };
    double distance = 0;
    int n = result[0].as_vector()->size();
    for(int i = 0; i < n; ++i)
    {
        distance += squared((*result[0].as_vector())[i] - (*result[1].as_vector())[i]);
    }
    distance_label->setText(QString::number(distance));

    const double positive_distance = 36.96293;
    const double negative_distance = 45.819515;
    if(distance <= positive_distance) security_label->setText(tr("None"));
    else if(distance >= negative_distance) security_label->setText(tr("Excellent"));
    else security_label->setText(QString::number((distance - positive_distance) / (negative_distance - positive_distance) * 100) + '%');
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


void MainWindow::on_blur_button_clicked()
{
    _undo_stack->push(new FilterCommand(this, std::bind(blur, _1, _selection, blur_slider->value())));
}

void MainWindow::on_pixel_button_clicked()
{
    _undo_stack->push(new FilterCommand(this, std::bind(pixel, _1, _selection, pixel_slider->value())));
}

void MainWindow::on_blackhead_button_clicked()
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

void MainWindow::checkEvaluateButton()
{
    if(_model && !_img.empty())
    {
        evaluate_button->setEnabled(true);
    }
}
