#include "Commands.h"
#include "MainWindow.h"


FilterCommand::FilterCommand(MainWindow* window, std::function<void(cv::Mat&)> filter, QUndoCommand* parent)
    :_window(window), _filter(std::move(filter)), _old_img(window->image().clone())
{
}

void FilterCommand::undo()
{
    _window->setImage(_old_img.clone());
}

void FilterCommand::redo()
{
    cv::Mat img = _window->image();
    _filter(img);
    _window->setImage(img);
}
