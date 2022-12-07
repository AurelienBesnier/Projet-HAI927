#include "FileSelector.h"
#include <QFileDialog>


FileSelector::FileSelector(QWidget* parent)
    :QWidget(parent)
{
    setupUi(this);
    setFocusProxy(file_edit);
}

QString FileSelector::path() const
{
    return file_edit->text();
}

void FileSelector::setPath(QString path)
{
    file_edit->setText(path);
}

void FileSelector::on_browse_button_clicked()
{
    QString result;
    if(filter.isEmpty())
        result = QFileDialog::getExistingDirectory(this, tr("Select folder"), path());
    else
    {
        if(for_saving)
            result = QFileDialog::getSaveFileName(this, tr("Target file"), path(), filter, nullptr, QFileDialog::Option::DontConfirmOverwrite);
        else
            result = QFileDialog::getOpenFileName(this, tr("Open file"), path(), filter);
    }

    if(!result.isEmpty())
        setPath(result);
}

void FileSelector::on_file_edit_textChanged(QString str)
{
    emit pathChanged(path());
}

