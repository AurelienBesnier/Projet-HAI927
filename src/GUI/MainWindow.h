#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUndoStack>
#include <opencv2/core/mat.hpp>
#include <fdeep/fdeep.hpp>
#include <future>
#include <optional>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
    
public:
    MainWindow();
    const cv::Mat& image() const;
    void setImage(cv::Mat img);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void on_tools_action_triggered();
    void on_open_action_triggered();
    void on_save_action_triggered();
    void on_evaluate_button_pressed();
    void on_zoom_in_action_triggered();
    void on_zoom_out_action_triggered();
    void on_blur_button_pressed();
    void on_pixel_button_pressed();
    void on_blackhead_button_pressed();
    void handleModelLoaded();

signals:
    void modelLoaded(); //Qt's signal system handles thread communication for us

private:
    fdeep::model loadModel();
    QPoint getImageCoord(QMouseEvent* event) const;
    void updateDisplay();
    void updateScale();
    void checkEvaluateButton();

    QUndoStack* _undo_stack;
    cv::Mat _img;
    cv::Rect _selection{};
    QPoint _last_click{};
    std::future<fdeep::model> _model_future;
    std::optional<fdeep::model> _model;
    float _scale = 1;
};

#endif
