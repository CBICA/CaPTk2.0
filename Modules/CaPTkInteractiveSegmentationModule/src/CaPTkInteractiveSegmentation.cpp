#include "CaPTkInteractiveSegmentation.h"

#include "CaPTkInteractiveSegmentationAdapter.h"

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <QMessageBox>

#include <iostream>

void CaPTkInteractiveSegmentation(QObject *parent) : 
    QObject(parent),
    m_DataStorage(mitk::DataStorage::GetInstance())
{

}

void CaPTkInteractiveSegmentation::Run(std::vector<mitk::Image::Pointer> images, 
         mitk::LabelSetImage::Pointer labels)
{
    std::cout << images[0] << "\n";
    std::cout << labels << "\n";

    // TODO: Test images

    // TODO: Test labels

    // TODO: Find dimensionality
    unsigned int dimensions = 3;

    // Recreate adapter
    auto m_CaPTkInteractiveSegmentationAdapter = CaPTkInteractiveSegmentationAdapter<3>();

    //...

    this->OnAlgorithmFinished();
}

void CaPTkInteractiveSegmentation::OnAlgorithmFinished()
{
    std::cout << "[CaPTkInteractiveSegmentation::OnAlgorithmFinished]\n";

    //...

    m_IsRunning = false;
}

void CaPTkInteractiveSegmentation::OnRunButtonPressed()
{
    std::cout << "[CaPTkInteractiveSegmentation::OnRunButtonPressed]\n";

    if (m_IsRunning)
    {
        QMessageBox msgError;
        msgError.setText("The algorithm is already running!\nPlease wait for it to finish.");
        msgError.setIcon(QMessageBox::Critical);
        msgError.setWindowTitle("Please wait");
        msgError.exec();
        return;
    }

    m_IsRunning = true;

    //...
}