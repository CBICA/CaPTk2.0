#include "CaPTkInteractiveSegmentation.h"

#include <mitkImageCast.h>
#include <mitkConvert2Dto3DImageFilter.h>
#include <mitkDataStorage.h>
#include <mitkRenderingManager.h>
#include <mitkNodePredicateAnd.h>
#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>

#include "itkExtractImageFilter.h"

#include <QtConcurrent/QtConcurrent>

#include <iostream>
#include <fstream>

#include "CaPTkInteractiveSegmentationAdapter.h"

CaPTkInteractiveSegmentation::CaPTkInteractiveSegmentation(QObject *parent)
	: QObject(parent)
{
	connect(&m_Watcher, SIGNAL(finished()), this, SLOT(OnAlgorithmFinished()));
}

void CaPTkInteractiveSegmentation::Run(std::vector<mitk::Image::Pointer> &images,
									   mitk::LabelSetImage::Pointer &seeds)
{
	std::cout << "[CaPTkInteractiveSegmentation::Run] "
			  << "Number of images: " << std::to_string(images.size()) << "\n";

	/* ---- Check if it's already running ---- */

	if (m_IsRunning)
	{
		emit finished(
			false, 
			"The algorithm is already running!\nPlease wait for it to finish.",
			nullptr
		);
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
		problemStr = std::string("No input images. At least one image") + 
					 std::string(" should be loaded ") + 
					 std::string(" in the data manager.");
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
			if (refDim == 3 && refDim != seeds->GetDimension())
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
				problemStr = std::string("All the images should have the same ") + 
							 std::string("size, spacing, origin, direction.");
				break;
			}
		}

		// Check if the seeds are on par with the ref
		if (ok)
		{
			// TODO

			if (!ok)
			{
				problemStr = std::string("The seeds should have the same size, ") + 
							 std::string("spacing, origin, direction as the images.");
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
			problemStr = std::string("Please draw with at least two labels.") + 
						 std::string(" One has to always be for the background tissue.");
		}
	}

	// Return if there is an issue
	if (!ok)
	{
		emit finished(
			false, 
			problemStr.c_str(),
			nullptr
		);
		m_IsRunning = false;
		return;
	}

	/* ---- Run ---- */

	m_FutureResult = QtConcurrent::run(this, &CaPTkInteractiveSegmentation::RunThread,
									   images, seeds);
	m_Watcher.setFuture(m_FutureResult);
}

void CaPTkInteractiveSegmentation::OnAlgorithmFinished()
{
	std::cout << "[CaPTkInteractiveSegmentation::OnAlgorithmFinished]\n";

	emit finished(
		m_FutureResult.result().ok, 
		m_FutureResult.result().errorMessage,
		m_FutureResult.result().segmentation
	);
	
	m_FutureResult = QFuture<Result>(); // Don't keep the results indefinitely

	m_IsRunning = false;
}

void CaPTkInteractiveSegmentation::OnProgressUpdateInternalReceived(int progress)
{
	emit progressUpdate(progress);
}

CaPTkInteractiveSegmentation::Result
CaPTkInteractiveSegmentation::RunThread(std::vector<mitk::Image::Pointer> &images,
										mitk::LabelSetImage::Pointer &seeds)
{
	std::cout << "[CaPTkInteractiveSegmentation::RunThread]\n";

	CaPTkInteractiveSegmentation::Result runResult;

	mitk::LabelSetImage::Pointer segm = mitk::LabelSetImage::New();

	if (images[0]->GetDimension() == 3)
	{
		// [ 3D ]

		/* ---- Convert images from mitk to itk ---- */

		std::vector<itk::Image<float, 3>::Pointer> imagesItk;
		for (auto &image : images)
		{
			typename itk::Image<float, 3>::Pointer imageItk;
			mitk::CastToItkImage(image, imageItk);
			imagesItk.push_back(imageItk);
		}

		/* ---- Convert seeds from mitk to itk ---- */

		typedef itk::Image<int, 3> LabelsImageType3D;
		typename LabelsImageType3D::Pointer seedsItk;
		mitk::CastToItkImage(seeds, seedsItk);

		/* ---- Run algorithm ---- */

		CaPTkInteractiveSegmentationAdapter<3> *algorithm =
			new CaPTkInteractiveSegmentationAdapter<3>();
		
		connect(
			algorithm,
			SIGNAL(ProgressUpdate(int)),
			this,
			SLOT(OnProgressUpdateInternalReceived(int))
		);

		algorithm->SetInputImages(imagesItk);
		algorithm->SetLabels(seedsItk);
		auto result = algorithm->Execute();

		/* ---- Parse result ---- */

		if (result->ok)
		{
			mitk::Image::Pointer segmNormal = mitk::Image::New();
			mitk::CastToMitkImage(result->labelsImage, segmNormal);
			segm->InitializeByLabeledImage(segmNormal);
			segm->CopyInformation(seeds);
			runResult.segmentation = segm;
		}

		delete algorithm;
		runResult.ok = result->ok;
		runResult.errorMessage = result->errorMessage;
	}
	else
	{
		// [ 2D ]

		/* ---- Convert images from mitk to itk ---- */

		std::cout << "Transforming images...\n";
		std::vector<itk::Image<float, 2>::Pointer> imagesItk;
		for (auto &image : images)
		{
			typename itk::Image<float, 2>::Pointer imageItk;
			try
			{
				mitk::CastToItkImage(image, imageItk);
			}
			catch(const std::exception& e)
			{
				// Image type is not supported (probably a png or something)
				std::cerr << e.what() << '\n';
				runResult.ok = false;
				runResult.errorMessage = "Image type is not supported";
				return runResult;
			}
			
			imagesItk.push_back(imageItk);
		}
		std::cout << "Transforming images finished.\n";

		/* ---- Convert seeds from mitk to itk ---- */

		typename itk::Image<int, 2>::Pointer seedsItk;
		// (mitk::LabelSetImage is always 3D)
		{
			typedef itk::Image<int, 2> LabelsImageType2D;
			typedef itk::Image<int, 3> LabelsImageType3D;
			typename LabelsImageType3D::Pointer seedsItk3D;
			mitk::CastToItkImage(seeds, seedsItk3D);
			auto regionSize = seedsItk3D->GetLargestPossibleRegion().GetSize();
			regionSize[2] = 0; // Only 2D image is needed
			LabelsImageType3D::IndexType regionIndex;
			regionIndex.Fill(0);
			LabelsImageType3D::RegionType desiredRegion(regionIndex, regionSize);
			auto extractor = 
				itk::ExtractImageFilter< LabelsImageType3D, LabelsImageType2D >::New();
			extractor->SetExtractionRegion(desiredRegion);
			extractor->SetInput(seedsItk3D);
			extractor->SetDirectionCollapseToIdentity();
			extractor->Update();
			seedsItk = extractor->GetOutput();
			seedsItk->DisconnectPipeline();
		}
		std::cout << "Transformed seeds.\n";

		/* ---- Run algorithm ---- */

		CaPTkInteractiveSegmentationAdapter<2> *algorithm =
			new CaPTkInteractiveSegmentationAdapter<2>();
		
		connect(
			algorithm,
			SIGNAL(ProgressUpdate(int)),
			this,
			SLOT(OnProgressUpdateInternalReceived(int))
		);
		
		algorithm->SetInputImages(imagesItk);
		algorithm->SetLabels(seedsItk);
		auto result = algorithm->Execute();

		/* ---- Parse result ---- */

		if (result->ok)
		{
			mitk::Image::Pointer segmNormal = mitk::Image::New();

			// Convert to 3D
			mitk::CastToMitkImage(result->labelsImage, segmNormal);
			mitk::Convert2Dto3DImageFilter::Pointer filter = 
				mitk::Convert2Dto3DImageFilter::New();
			filter->SetInput(segmNormal);
			filter->Update();
			segmNormal = filter->GetOutput();

			segm->InitializeByLabeledImage(segmNormal);
			segm->CopyInformation(seeds);
			runResult.segmentation = segm;
		}

		delete algorithm;
		runResult.ok = result->ok;
		runResult.errorMessage = result->errorMessage;
	}

	// Copy the labels from seeds image (same for 2D and 3D)
	if (runResult.ok)
	{
		mitk::LabelSet::Pointer referenceLabelSet = seeds->GetActiveLabelSet();
		mitk::LabelSet::Pointer outputLabelSet = segm->GetActiveLabelSet();

		mitk::LabelSet::LabelContainerConstIteratorType itR;
		mitk::LabelSet::LabelContainerConstIteratorType it;

		for (itR = referenceLabelSet->IteratorConstBegin();
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

					segm->GetActiveLabelSet()->UpdateLookupTable(it->second->GetValue()); // Update it
				}
			}
		}
	}

	return runResult;
}