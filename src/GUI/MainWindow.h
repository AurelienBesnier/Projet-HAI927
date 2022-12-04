#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core/mat.hpp>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
    
public:
    MainWindow();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void on_open_action_triggered();
    void on_save_action_triggered();
    void on_evaluate_button_pressed();
    void on_zoom_in_action_triggered();
    void on_zoom_out_action_triggered();
    void on_blur_button_pressed();
    void on_pixel_button_pressed();
    void on_blackhead_button_pressed();

private:
    QPoint getImageCoord(QMouseEvent* event) const;
    void updateDisplay(const cv::Mat& img);
    void updateScale();
    void showSelection();

    cv::Mat _img;
    cv::Rect _selection{};
    QPoint _last_click{};
    float _scale = 1;
};

#endif
