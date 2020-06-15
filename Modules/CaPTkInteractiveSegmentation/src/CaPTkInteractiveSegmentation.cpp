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
#include <QMessageBox>

#include <iostream>
#include <fstream>

CaPTkInteractiveSegmentation::CaPTkInteractiveSegmentation(
	mitk::DataStorage::Pointer dataStorage,
	QObject *parent)
	: QObject(parent)
{
	m_DataStorage = dataStorage;

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
		QMessageBox msgError;
		msgError.setText(
			"The algorithm is already running!\nPlease wait for it to finish."
		);
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
}

// void CaPTkInteractiveSegmentation::Run(Json::Value& task_json, Json::Value& cohort_json)
// {
//     /* ---- Parse the task ---- */
    
// 	std::string task_name   = task_json.get("task_name", "UTF-8" ).asString();
// 	std::string application = task_json.get("application", "UTF-8" ).asString();
//     std::string task_type   = task_json.get("task_type", "UTF-8" ).asString();
	
// 	std::string results_dir = task_json.get("results_dir", "UTF-8" ).asString();

// 	/* ---- Run for each subject of the cohort ---- */
	
// 	std::string cohort_name = cohort_json.get("cohort_name", "UTF-8" ).asString();

// 	for (auto& subj : cohort_json["subjects"])
// 	{
// 		auto name = subj["name"];
// 		std::cout << name << std::endl;
// 		// std::vector<mitk::Image::Pointer> images;
// 		// mitk::LabelSetImage::Pointer seeds;

// 		// Find all the images
// 		for (auto& image : subj["images"])
// 		{
// 			auto modality = image["modality"];
// 			std::cout << modality << std::endl;
// 		}
// 	}

// 	std::cout << "cohort name: " << cohort_name << std::endl;
// }

// void CaPTkInteractiveSegmentation::Run(std::string task_json_path, std::string cohort_json_path)
// {
// 	try
// 	{
// 		Json::Value taskRoot, cohortRoot;

// 		// Read the two JSON from file
// 		std::ifstream taskStream(task_json_path, std::ifstream::binary);
// 		taskStream >> taskRoot;
// 		std::ifstream cohortStream(cohort_json_path, std::ifstream::binary);
// 		cohortStream >> cohortRoot;

// 		this->Run(taskRoot, cohortRoot);
// 	}
// 	catch (const std::exception &e)
// 	{
// 		MITK_ERROR << e.what();
// 	}
// 	catch (...)
// 	{
// 		MITK_ERROR << "Unexpected error!";
// 	}
// }

void CaPTkInteractiveSegmentation::SetProgressBar(QProgressBar* progressBar)
{
	m_ProgressBar = progressBar;
	// if (m_ProgressBar) { m_ProgressBar->setValue(42); }
}

void CaPTkInteractiveSegmentation::OnAlgorithmFinished()
{
	std::cout << "[CaPTkInteractiveSegmentation::OnAlgorithmFinished]\n";

	mitk::DataNode::Pointer node;

	if (m_FutureResult.result().ok)
	{
		/* ---- Make seeds invisible ---- */
		mitk::DataStorage::SetOfObjects::ConstPointer all =
		m_DataStorage->GetAll();
		for (mitk::DataStorage::SetOfObjects::ConstIterator it = all->Begin();
			it != all->End(); ++it)
		{
			if (it->Value().IsNotNull())
			{
				std::string name = it->Value()->GetName();
				if (name.rfind("Seeds", 0) == 0) // Starts with
				{
					it->Value()->SetVisibility(false);
				}
				else if (name.rfind("Segmentation", 0) == 0) // Starts with
				{
					it->Value()->SetVisibility(false);
				}
			}
		}

		/* ---- Add segmentation ---- */
		node = mitk::DataNode::New();
		node->SetData(m_FutureResult.result().segmentation);
		node->SetName(FindNextAvailableSegmentationName());
		node->SetBoolProperty("captk.interactive.segmentation.output", true);
		m_DataStorage->Add(node);
		node->SetVisibility(true);
	}
	else
	{
		// Something went wrong
		QMessageBox msgError;
		msgError.setText(m_FutureResult.result().errorMessage.c_str());
		msgError.setIcon(QMessageBox::Critical);
		msgError.setWindowTitle("CaPTk Interactive Segmentation Error!");
		msgError.exec();
	}

	m_FutureResult = QFuture<Result>(); // Don't keep the results indefinitely

	// Change the layer of every image to 1
	auto predicateIsImage = // Predicate to find if node is mitk::Image
		mitk::TNodePredicateDataType<mitk::Image>::New();
	auto predicatePropertyIsHelper = // Predicate property to find if node is a helper object
		mitk::NodePredicateProperty::New("helper object");
	auto predicateFinal = mitk::NodePredicateAnd::New();
	predicateFinal->AddPredicate(predicateIsImage);
	predicateFinal->AddPredicate(
		mitk::NodePredicateNot::New(predicatePropertyIsHelper));
	mitk::DataStorage::SetOfObjects::ConstPointer all =
		m_DataStorage->GetSubset(predicateFinal);
	for (mitk::DataStorage::SetOfObjects::ConstIterator it = all->Begin();
		 it != all->End(); 
		 ++it)
	{
		if (it->Value().IsNotNull())
		{
			it->Value()->SetProperty("layer", mitk::IntProperty::New(1));
		}
	}

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	mitk::RenderingManager::GetInstance()->ForceImmediateUpdateAll();

	node->SetProperty("layer", mitk::IntProperty::New(10));
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	m_IsRunning = false;
}

CaPTkInteractiveSegmentation::Result
CaPTkInteractiveSegmentation::RunThread(std::vector<mitk::Image::Pointer> &images,
										mitk::LabelSetImage::Pointer &seeds)
{
	std::cout << "[CaPTkInteractiveSegmentation::RunThread]\n";

	CaPTkInteractiveSegmentation::Result runResult;
	runResult.seeds = seeds;

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
		if (m_ProgressBar)
		{
			std::cout << "[CaPTkInteractiveSegmentation::RunThread] "
					  << "Connecting Progress Bar\n";
			connect(algorithm, SIGNAL(ProgressUpdate(int)),
					m_ProgressBar, SLOT(setValue(int)));
		}
		algorithm->SetInputImages(imagesItk);
		algorithm->SetLabels(seedsItk);
		auto result = algorithm->Execute();

		/* ---- Parse result ---- */

		if (result->ok)
		{
			mitk::Image::Pointer segmNormal;
			mitk::CastToMitkImage(result->labelsImage, segmNormal);
			segm->InitializeByLabeledImage(segmNormal);
			runResult.segmentation = segm;
		}

		delete algorithm;
		runResult.ok = result->ok;
		runResult.errorMessage = result->errorMessage;
	}
	else
	{
		// [ 2D ]

		// QMessageBox msgError;
		// msgError.setText("Please use a 3D image");
		// msgError.setIcon(QMessageBox::Critical);
		// msgError.setWindowTitle("2D is not supported yet");
		// msgError.exec();

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
		if (m_ProgressBar)
		{
			std::cout << "[CaPTkInteractiveSegmentation::RunThread] "
					  << "Connecting Progress Bar\n";
			connect(algorithm, SIGNAL(ProgressUpdate(int)),
					m_ProgressBar, SLOT(setValue(int)));
		}
		algorithm->SetInputImages(imagesItk);
		algorithm->SetLabels(seedsItk);
		auto result = algorithm->Execute();

		/* ---- Parse result ---- */

		if (result->ok)
		{
			mitk::Image::Pointer segmNormal;

			// Convert to 3D
			mitk::CastToMitkImage(result->labelsImage, segmNormal);
			mitk::Convert2Dto3DImageFilter::Pointer filter = 
				mitk::Convert2Dto3DImageFilter::New();
			filter->SetInput(segmNormal);
			filter->Update();
			segmNormal = filter->GetOutput();

			segm->InitializeByLabeledImage(segmNormal);
			runResult.segmentation = segm;
		}

		delete algorithm;
		runResult.ok = result->ok;
		runResult.errorMessage = result->errorMessage;
	}

	// Copy the labels from seeds image (same for 2D and 3D)
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
	predicateFinal->AddPredicate(
		mitk::NodePredicateNot::New(predicatePropertyIsHelper)
	);

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
					if (lastFound < 1)
					{
						lastFound = 1;
					}
				}
				else
				{
					if (name.rfind("Segmentation-", 0) == 0) // Starts with
					{
						std::string numStr = name.erase(
							0, std::string("Segmentation-").length()
						);
						if (IsNumber(numStr))
						{
							int num = std::stoi(numStr);
							if (lastFound < num)
							{
								lastFound = num;
							}
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
		return std::string("Segmentation-") + std::to_string(lastFound + 1);
	}
}

bool CaPTkInteractiveSegmentation::IsNumber(const std::string &s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}