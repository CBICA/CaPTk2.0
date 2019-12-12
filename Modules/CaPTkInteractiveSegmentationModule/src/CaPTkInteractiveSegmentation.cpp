#include "CaPTkInteractiveSegmentation.h"

#include <mitkImageCast.h>
#include <mitkDataStorage.h>
#include <mitkNodePredicateAnd.h>
#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>

#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

#include <iostream>

CaPTkInteractiveSegmentation::CaPTkInteractiveSegmentation(
            mitk::DataStorage::Pointer dataStorage,
            QObject *parent) 
    : QObject(parent)
{
    m_DataStorage = dataStorage;

    connect(&m_Watcher, SIGNAL(finished()), this, SLOT(OnAlgorithmFinished()));
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

    std::cout << "[CaPTkInteractiveSegmentation::Run] " 
              << "Number of images: " << std::to_string(images.size()) << "\n";

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
        auto refDim = images[0]->GetDimension();

        // Check if the other images are on par with the ref
        for (size_t i = 1; i < images.size(); i++)
        {
            auto dim = images[i]->GetDimension();

            if (dim != refDim)
            {
                ok = false;
            }

            if (!ok)
            {
                problemStr = "All the images should have the same dimension.";
                break;
            }
        }

        // Check if the seeds are on par with the ref
        if (ok)
        {
            if (refDim != seeds->GetDimension())
            {
                ok = false;
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

            // TODO

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
            // TODO

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

    /* ---- Run ---- */

    m_FutureResult = QtConcurrent::run(this, &CaPTkInteractiveSegmentation::RunThread, 
                                       images, seeds);
    m_Watcher.setFuture(m_FutureResult);
    // this->RunThread(images, seeds);
}

void CaPTkInteractiveSegmentation::OnAlgorithmFinished()
{
    std::cout << "[CaPTkInteractiveSegmentation::OnAlgorithmFinished]\n";

    // TODO: !

    // Find the name for the output segmentation


    if (m_FutureResult.result().ok)
    {
        mitk::DataNode::Pointer node = mitk::DataNode::New();
        node->SetData(m_FutureResult.result().segmentation);
        node->SetName(FindNextAvailableSegmentationName());
        node->SetBoolProperty("captk.interactive.segmentation.output", true);
        // mitk::BoolProperty::Pointer propertySeg = mitk::BoolProperty::New(true);
        // node->SetProperty("captk.interactive.segmentation.output", propertySeg);
        m_DataStorage->Add(node);
    }
    else
    {
        QMessageBox msgError;
        msgError.setText(m_FutureResult.result().errorMessage.c_str());
        msgError.setIcon(QMessageBox::Critical);
        msgError.setWindowTitle("CaPTk Interactive Segmentation Error!");
        msgError.exec();
    }

    m_IsRunning = false;
}

CaPTkInteractiveSegmentation::Result 
CaPTkInteractiveSegmentation::RunThread(std::vector<mitk::Image::Pointer>& images, 
         mitk::LabelSetImage::Pointer& seeds)
{
    // std::shared_ptr< CaPTkInteractiveSegmentation::Result > runResult(
    //     new CaPTkInteractiveSegmentation::Result()
    // );
    CaPTkInteractiveSegmentation::Result runResult;
    runResult.seeds = seeds;

    mitk::LabelSetImage::Pointer segm = mitk::LabelSetImage::New();

    if (seeds->GetDimension() == 3)
    {
        // [ 3D ]

        /* ---- Convert images from mitk to itk ---- */

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
            segm->InitializeByLabeledImage(segmNormal);

            runResult.segmentation = segm;
        }

        runResult.ok = result->ok;
        runResult.errorMessage = result->errorMessage;
    }
    else
    {
        // [ 2D ]

        QMessageBox msgError;
        msgError.setText("Please use a 3D image");
        msgError.setIcon(QMessageBox::Critical);
        msgError.setWindowTitle("2D is not supported yet");
        msgError.exec();
    }

    // Copy the labels from seeds image
    {
        mitk::LabelSet::Pointer referenceLabelSet =	seeds->GetActiveLabelSet();
        mitk::LabelSet::Pointer outputLabelSet    = segm->GetActiveLabelSet();

        mitk::LabelSet::LabelContainerConstIteratorType itR;
        mitk::LabelSet::LabelContainerConstIteratorType it;
        
        for (itR =  referenceLabelSet->IteratorConstBegin();
                itR != referenceLabelSet->IteratorConstEnd(); 
                ++itR) 
        {
            for (it = outputLabelSet->IteratorConstBegin(); 
                    it != outputLabelSet->IteratorConstEnd();
                    ++it)
            {
                if (itR->second->GetValue() == it->second->GetValue())
                {
                    it->second->SetColor(itR->second->GetColor());
                    it->second->SetName(itR->second->GetName());
                }
            }
        }
    }

    return runResult;
    // this->OnAlgorithmFinished(); // TODO: This should be called when the algorithm
    //                              // finishes in the background (watchers etc)
}

std::string CaPTkInteractiveSegmentation::FindNextAvailableSegmentationName()
{
    // Predicate to find if node is mitk::LabelSetImage
    auto predicateIsLabelSetImage = 
        mitk::TNodePredicateDataType<mitk::LabelSetImage>::New();

    // Predicate property to find if node is a helper object
    auto predicatePropertyIsHelper =
        mitk::NodePredicateProperty::New("helper object");

    // The images we want are but mitk::LabelSetImage and not helper obj
    auto predicateFinal = mitk::NodePredicateAnd::New();
    predicateFinal->AddPredicate(predicateIsLabelSetImage);
    predicateFinal->AddPredicate(mitk::NodePredicateNot::New(predicatePropertyIsHelper));

    int lastFound = 0;

    // Get those images
    mitk::DataStorage::SetOfObjects::ConstPointer all = 
        m_DataStorage->GetSubset(predicateFinal);
    for (mitk::DataStorage::SetOfObjects::ConstIterator it = all->Begin(); 
        it != all->End(); ++it) 
    {
        if (it->Value().IsNotNull())
        {
            std::string name = it->Value()->GetName();
            if (name.rfind("Segmentation", 0) == 0) // Starts with
            {
                if (name.length() == std::string("Segmentation").length())
                {
                    // Special case
                    if (lastFound < 1) { lastFound = 1; }
                }
                else
                {
                    if (name.rfind("Segmentation-", 0) == 0) // Starts with
                    {
                        std::string numStr = name.erase(0,std::string("Segmentation-").length());

                        if (IsNumber(numStr))
                        {
                            int num = std::stoi(numStr);
                            if (lastFound < num) { lastFound = num; }
                        }
                    }
                }
            } 
        }
    }

    // Construct and return the correct string
    if (lastFound == 0)
    {
        return "Segmentation";
    }
    else
    {
        return std::string("Segmentation-") + std::to_string(lastFound+1);
    }
}

bool CaPTkInteractiveSegmentation::IsNumber(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}