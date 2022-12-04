#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <functional>
#include <opencv2/core/mat.hpp>
class MainWindow;


class FilterCommand : public QUndoCommand
{
public:
    FilterCommand(MainWindow* window, std::function<void(cv::Mat&)> filter, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

protected:
    MainWindow* _window;
    std::function<void(cv::Mat&)> _filter;
    cv::Mat _old_img;
};

#endif
