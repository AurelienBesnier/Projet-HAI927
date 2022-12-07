#include "ToolWindow.h"
#include "tools.h"
#include <QMessageBox>
#include <QFileDialog>
#include <fstream>

using std::filesystem::path;


ToolWindow::ToolWindow(QWidget* parent)
    :QWidget(parent)
{
    setupUi(this);

    pairs_selector->filter = dataset_selector->filter = tr("Text file (*.txt)");
    dataset_selector->for_saving = true;
    connect(lfw_selector, SIGNAL(pathChanged(QString)), this, SLOT(checkButtons()));
    connect(dataset_selector, SIGNAL(pathChanged(QString)), this, SLOT(checkButtons()));
    connect(result_folder_selector, SIGNAL(pathChanged(QString)), this, SLOT(checkButtons()));
}


void ToolWindow::on_genpairs_button_clicked()
{
    bool use_pairs = false;
    path lfw = lfw_selector->path().toStdString();
    path dataset = dataset_selector->path().toStdString();
    path pairs = pairs_selector->path().toStdString();
    int count = count_spinbox->value();
    if(!pairs.empty())
    {
        auto result = QMessageBox::question(this, tr("Generate dataset"), tr("Use selected pair file?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(result == QMessageBox::Cancel) return;
        use_pairs = result == QMessageBox::Yes;
    }
    std::ofstream output(dataset);
    if(use_pairs)
        runTask([=, output = std::move(output)]() mutable {
            genPairDataset(output, lfw, pairs, count);
        });
    else
        runTask([=, output = std::move(output)]() mutable {
            genPairDataset(output, lfw, count);
        });
}

void ToolWindow::on_gentriplets_button_clicked()
{
    bool use_pairs = false;
    path lfw = lfw_selector->path().toStdString();
    path dataset = dataset_selector->path().toStdString();
    path pairs = pairs_selector->path().toStdString();
    int count = count_spinbox->value();
    if(!pairs.empty())
    {
        auto result = QMessageBox::question(this, tr("Generate dataset"), tr("Use selected pair file?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(result == QMessageBox::Cancel) return;
        use_pairs = result == QMessageBox::Yes;
    }
    std::ofstream output(dataset);
    if(use_pairs)
        runTask([=, output = std::move(output)]() mutable {
            genTripletDataset(output, lfw, pairs, count);
        });
    else
        runTask([=, output = std::move(output)]() mutable {
            genTripletDataset(output, lfw, count);
        });
}

void ToolWindow::on_annotate_button_clicked()
{
    path lfw = lfw_selector->path().toStdString();
    std::ifstream input(dataset_selector->path().toStdString());
    runTask([=, input = std::move(input)]() mutable {
        processFile(lfw, input);
    });
}

void ToolWindow::on_obscurate_button_clicked()
{
    std::ofstream output;
    if(QString path = QFileDialog::getSaveFileName(this, tr("Choose where to save log"), "", tr("Text file (*.txt)")); !path.isEmpty())
    {
        output.open(path.toStdString());
    }
    QStringList splitted = obscuration_edit->text().split(' ', Qt::SkipEmptyParts);
    std::vector<std::string> desc(splitted.size());
    std::ranges::transform(splitted, desc.begin(), &QString::toStdString);
    path lfw = lfw_selector->path().toStdString();
    path dataset = dataset_selector->path().toStdString();
    path result = result_folder_selector->path().toStdString();
    bool random = random_checkbox->isChecked();

    runTask([=, output = std::move(output)]() mutable {
        obscureDataSet(output, dataset, lfw, result, desc, random);
    });
}

void ToolWindow::checkButtons()
{
    path lfw = lfw_selector->path().toStdString();
    path dataset = dataset_selector->path().toStdString();
    path result = result_folder_selector->path().toStdString();

    bool has_lfw = !lfw.empty() && is_directory(lfw);

    genpairs_button->setEnabled(has_lfw && !dataset.empty());
    gentriplets_button->setEnabled(genpairs_button->isEnabled());
    annotate_button->setEnabled(has_lfw && exists(dataset));
    obscurate_button->setEnabled(has_lfw && exists(dataset) && !result.empty() && is_directory(result));
}

template<typename F>
void ToolWindow::runTask(F&& task)
{
    setEnabled(false);
    try
    {
        task();
        QMessageBox::information(this, "", tr("Operation completed!"));
    }
    catch(const std::exception& e)
    {
        QMessageBox::critical(this, tr("Error"), tr("Unexpected exception : ") + e.what());
    }
    setEnabled(true);
    checkButtons();
}
