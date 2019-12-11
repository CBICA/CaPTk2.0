#include "CaPTkInteractiveSegmentation.h"

#include <mitkImageCast.h>
#include <mitkDataStorage.h>

#include <QMessageBox>

#include <iostream>

CaPTkInteractiveSegmentation::CaPTkInteractiveSegmentation(
            mitk::DataStorage::Pointer dataStorage,
            QObject *parent) 
    : QObject(parent)
{
    m_DataStorage = dataStorage;
}

void CaPTkInteractiveSegmentation::Run(std::vector<mitk::Image::Pointer>& images, 
         mitk::LabelSetImage::Pointer& seeds)
{
    std::cout << "[CaPTkInteractiveSegmentation::Run] Image size: "
              << std::to_string(images.size()) << "\n";
    images = images; // Delete this
    seeds = seeds; // Delete this

    /* ---- Check if it's already running ---- */

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

    /* ---- Check requirements ---- */

    bool ok = true;              // Becomes false if there is an issue
    std::string problemStr = ""; // Populated if there is an issue

    // Check if there is at least one image
    if (images.size() == 0)
    {
        ok = false;
        problemStr = std::string("No input images. At least one image")
                   + std::string(" should be loaded and selected")
                   + std::string(" in the data manager.");
    }

    // Check if the seeds image exists
    if (ok && seeds == nullptr)
    {
        ok = false;
        problemStr = "Please create a seeds image";
    }

    // Check if all the input images and the seeds have the same dimensionality
    if (ok)
    {
        auto ref = images[0];

        // Check if the other images are on par with the ref
        for (size_t i = 1; i < images.size(); i++)
        {
            auto im = images[i];

            //...

            if (!ok)
            {
                problemStr = "All the images should have the same dimension.";
                break;
            }
        }

        // Check if the seeds are on par with the ref
        if (ok)
        {
            //...

            if (!ok)
            {
                problemStr = "The seeds should have the same dimension as the images.";
            }
        }
    }

    // Check if the images and the seeds have the same size, spacing, origin, direction
    if (ok)
    {
        auto ref = images[0];

        // Check if the other images are on par with the ref
        for (size_t i = 1; i < images.size(); i++)
        {
            auto im = images[i];

            //...

            if (!ok)
            {
                problemStr = std::string("All the images should have the same ")
                           + std::string("size, spacing, origin, direction.");
                break;
            }
        }

        // Check if the seeds are on par with the ref
        if (ok)
        {
            //...

            if (!ok)
            {
                problemStr = std::string("The seeds should have the same size, ")
                           + std::string("spacing, origin, direction as the images.");
            }
        }
    }

    // Check if there are at least two labels present in the seeds
    if (ok)
    {
        bool foundOne = false;
        bool foundTwo = false;
        // Iterate through the labels in the seeds
        // TODO: !
        foundTwo = foundOne = true; // TODO: Delete this
        
        if (!foundTwo)
        {
            ok = false;
            problemStr = std::string("Please draw with at least two labels.")
                       + std::string(" One has to always be for the background tissue.");
        }
    }

    // Return if there is an issue
    if (!ok)
    {
        QMessageBox msgError;
        msgError.setText(problemStr.c_str());
        msgError.setIcon(QMessageBox::Critical);
        msgError.setWindowTitle("Incorrect state.");
        msgError.exec();
        m_IsRunning = false;
        return;
    }

    /* ---- Recreate whatever is needed for running ---- */

    // Recreate adapters
    auto m_CaPTkInteractiveSegmentationAdapter2D = 
            new CaPTkInteractiveSegmentationAdapter<2>();
    auto m_CaPTkInteractiveSegmentationAdapter3D = 
            new CaPTkInteractiveSegmentationAdapter<3>();

    // Bypass unused vars
    m_CaPTkInteractiveSegmentationAdapter2D = m_CaPTkInteractiveSegmentationAdapter2D;
    m_CaPTkInteractiveSegmentationAdapter3D = m_CaPTkInteractiveSegmentationAdapter3D;

    // Set the correct dimensionality (to m_RunDimensionality)

    /* ---- ... ---- */

    //...
    std::vector<itk::Image<float,3>::Pointer> imagesItk;
    for (auto& image : images)
    {
        typename itk::Image<float, 3>::Pointer imageItk;
        mitk::CastToItkImage(image, imageItk);
        imagesItk.push_back(imageItk);
    }
    typedef itk::Image<int, 3> LabelsImageType3D;
    typename LabelsImageType3D::Pointer seedsItk3D;
    mitk::CastToItkImage(seeds, seedsItk3D);
    CaPTkInteractiveSegmentationAdapter<3>* algorithm = 
            new CaPTkInteractiveSegmentationAdapter<3>();
    algorithm->SetInputImages(imagesItk);
    algorithm->SetLabels(seedsItk3D);
    auto result = algorithm->Execute();

    if (result->ok)
    {
        mitk::Image::Pointer segmNormal;
        mitk::CastToMitkImage(result->labelsImage, segmNormal);
        mitk::LabelSetImage::Pointer segm = mitk::LabelSetImage::New();
        segm->InitializeByLabeledImage(segmNormal);
        mitk::DataNode::Pointer node = mitk::DataNode::New();
        node->SetData(segm);
        m_DataStorage->Add(node);
    }

    this->OnAlgorithmFinished(); // TODO: This should be called when the algorithm
                                 // finishes in the background (watchers etc)
}

void CaPTkInteractiveSegmentation::OnAlgorithmFinished()
{
    std::cout << "[CaPTkInteractiveSegmentation::OnAlgorithmFinished]\n";

    //...

    m_IsRunning = false;
}