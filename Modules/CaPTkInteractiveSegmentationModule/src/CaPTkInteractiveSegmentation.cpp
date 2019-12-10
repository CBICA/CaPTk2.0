#include "CaPTkInteractiveSegmentation.h"

#include <QMessageBox>

#include <iostream>

CaPTkInteractiveSegmentation::CaPTkInteractiveSegmentation(QObject *parent) : 
    QObject(parent),
    m_DataStorage(mitk::StandaloneDataStorage::New()) // ds is singleton
{

}

void CaPTkInteractiveSegmentation::Run(std::vector<mitk::Image::Pointer> images, 
         mitk::LabelSetImage::Pointer labels)
{
    std::cout << images[0] << "\n";
    std::cout << labels << "\n";

    // TODO: Test images

    // TODO: Test labels

    // Recreate adapters
    auto m_CaPTkInteractiveSegmentationAdapter2D = new CaPTkInteractiveSegmentationAdapter<2>();
    auto m_CaPTkInteractiveSegmentationAdapter3D = new CaPTkInteractiveSegmentationAdapter<3>();

    // Bypass unused vars
    m_CaPTkInteractiveSegmentationAdapter2D = m_CaPTkInteractiveSegmentationAdapter2D;
    m_CaPTkInteractiveSegmentationAdapter3D = m_CaPTkInteractiveSegmentationAdapter3D;

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