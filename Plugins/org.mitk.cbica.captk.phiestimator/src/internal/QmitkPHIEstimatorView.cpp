/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>

#include <usModuleRegistry.h>

#include <QMessageBox>

#include <ExampleImageInteractor.h>

#include <EGFRvIIISurrogateIndex.h>
#include <EGFRStatusPredictor.h>

#include "QmitkPHIEstimatorView.h"

#include <mitkImageCast.h>
#include <mitkImageToItk.h>
#include "mitkLabelSetImage.h"

#include <itkSmartPointer.h>
#include <itkImage.h>
#include <itkImageIOBase.h>
#include <itkImageIOFactory.h>
#include <itkImageFileWriter.h>
#include <itkNiftiImageIO.h>
#include <itkNiftiImageIOFactory.h>
#include "CaPTkDefines.h"
#include "itkImageDuplicator.h"
#include "itkCastImageFilter.h"

namespace
{
  // Helper function to create a fully set up instance of our
  // ExampleImageInteractor, based on the state machine specified in Paint.xml
  // as well as its configuration in PaintConfig.xml. Both files are compiled
  // into ExtPHIEstimatorModule as resources.
  static ExampleImageInteractor::Pointer CreateExampleImageInteractor()
  {
    auto PHIEstimatorModule = us::ModuleRegistry::GetModule("MitkCaPTkPHIEstimator");

    if (nullptr != PHIEstimatorModule)
    {
      auto interactor = ExampleImageInteractor::New();
      interactor->LoadStateMachine("Paint.xml", PHIEstimatorModule);
      interactor->SetEventConfig("PaintConfig.xml", PHIEstimatorModule);
      return interactor;
    }

    return nullptr;
  }
}

// Don't forget to initialize the VIEW_ID.
const std::string QmitkPHIEstimatorView::VIEW_ID = "org.mitk.views.captk.phiestimator";

QmitkPHIEstimatorView::QmitkPHIEstimatorView()
{
	m_SegmentationPredicate = mitk::NodePredicateAnd::New();
	m_SegmentationPredicate->AddPredicate(
		mitk::TNodePredicateDataType<mitk::LabelSetImage>::New()
	);
	m_SegmentationPredicate->AddPredicate
	(mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object"))
	);

}

QmitkPHIEstimatorView::~QmitkPHIEstimatorView()
{

}

void QmitkPHIEstimatorView::CreateQtPartControl(QWidget* parent)
{
  // Setting up the UI is a true pleasure when using .ui files, isn't it?
  m_Controls.setupUi(parent);
  m_Controls.Results->hide();

  // Wire up the UI widgets with our functionality.
  connect(m_Controls.processImageButton, SIGNAL(clicked()), this, SLOT(ProcessSelectedImage()));
}

void QmitkPHIEstimatorView::SetFocus()
{
  m_Controls.processImageButton->setFocus();
}

void QmitkPHIEstimatorView::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList<mitk::DataNode::Pointer>& dataNodes)
{
  for (const auto& dataNode : dataNodes)
  {
    // Write robust code. Always check pointers before using them. If the
    // data node pointer is null, the second half of our condition isn't
    // even evaluated and we're safe (C++ short-circuit evaluation).
    if (dataNode.IsNotNull() && nullptr != dynamic_cast<mitk::Image*>(dataNode->GetData()))
    {
      m_Controls.selectImageLabel->setVisible(false);
      return;
    }
  }

  // Nothing is selected or the selection doesn't contain an image.
  m_Controls.selectImageLabel->setVisible(true);
}

void QmitkPHIEstimatorView::ProcessSelectedImage()
{
	typedef itk::Image<unsigned short, 3> MaskImageType;
	typedef itk::Image<short, 4> PerfusionImageType;
	typedef itk::Image<float, 4> Float4DImage;
	MaskImageType::Pointer maskimg;
	Float4DImage::Pointer perfImg;

  // Before we even think about processing something, we need to make sure
  // that we have valid input. Don't be sloppy, this is a main reason
  // for application crashes if neglected.
  auto selectedDataNodes = this->GetDataManagerSelection();

  if (selectedDataNodes.empty())
    return;

  auto firstSelectedDataNode = selectedDataNodes.front();

mitk::DataNode::Pointer maskNode = this->GetDataStorage()->GetNamedNode("Near_Far_masks");

if (maskNode.IsNull())
{
	QMessageBox::information(nullptr, "Example View", "Please load mask before starting image processing.");
	return;
}

auto maskdata = maskNode->GetData();

// Something is selected, but does it contain data?
if (maskdata == nullptr)
{
	QMessageBox::information(nullptr, "PHI Estimator View", "Please load mask before starting image processing.");
	return;
}
	mitk::Image::Pointer maskimage = dynamic_cast<mitk::Image*>(maskdata);

	// Something is selected and it contains data, but is it an image?
	if (maskimage.IsNull())
	{
		QMessageBox::information(nullptr, "PHI Estimator View", "Please load mask before starting image processing.");
		return;
	}

		//typename PerfusionImageType::Pointer pimg = PerfusionImageType::New();
		//mitk::CastToItkImage(image, pimg);
		maskimg = mitk::ImageToItkImage<unsigned short, 3>(maskimage);

		//auto writer1 = itk::ImageFileWriter<MaskImageType>::New();
		//writer1->SetImageIO(itk::NiftiImageIO::New());
		//writer1->SetInput(maskimg);
		//writer1->SetFileName("imageMask.nii.gz");
		//writer1->Write();

  if (firstSelectedDataNode.IsNull())
  {
    QMessageBox::information(nullptr, "PHI Estimator View", "Please load and select an image before starting image processing.");
    return;
  }

  auto data = firstSelectedDataNode->GetData();

  // Something is selected, but does it contain data?
  if (data == nullptr)
  {
	  QMessageBox::information(nullptr, "PHI Estimator View", "Please load mask before starting image processing.");
	  return;
  }
    // We don't use the auto keyword here, which would evaluate to a native
    // image pointer. Instead, we want a smart pointer in order to ensure that
    // the image isn't deleted somewhere else while we're using it.
    mitk::Image::Pointer image = dynamic_cast<mitk::Image*>(data);

    // Something is selected and it contains data, but is it an image?
    if (image.IsNull())
    {
		QMessageBox::information(nullptr, "PHI Estimator View", "Please load mask before starting image processing.");
		return;
	}
      auto imageName = firstSelectedDataNode->GetName();

      MITK_INFO << "Process image \"" << imageName << "\" ...";

      // We're finally using the ExampleImageFilter
 
	  //typename PerfusionImageType::Pointer pimg = PerfusionImageType::New();
	  //mitk::CastToItkImage(image, pimg);
	  PerfusionImageType::Pointer pimg = mitk::ImageToItkImage<short,4>(image);

	  typedef itk::ImageDuplicator<PerfusionImageType> ImgDuplicator;
	  ImgDuplicator::Pointer d = ImgDuplicator::New();
	  d->SetInputImage(pimg);
	  d->Update();

	  typedef itk::CastImageFilter<PerfusionImageType, Float4DImage> CastFilterType;
	  auto castFilter = CastFilterType::New();
	  castFilter->SetInput(d->GetOutput());
	  castFilter->Update();

	  perfImg = castFilter->GetOutput();

	  //auto writer = itk::ImageFileWriter<Float4DImage>::New();
	  //writer->SetImageIO(itk::NiftiImageIO::New());
	  //writer->SetInput(perfImg);
	  //writer->SetFileName("image4d.nii.gz");
	  //writer->Write();
	  
	  VectorDouble EGFRStatusParams;
	  EGFRStatusPredictor EGFRPredictor;
	  using ImageType = itk::Image<float, 3>;
	  std::vector<ImageType::Pointer> Perfusion_Registered;
	  std::vector<ImageType::IndexType> nearIndices, farIndices;

	  itk::ImageRegionIteratorWithIndex< MaskImageType > maskIt(maskimg, maskimg->GetLargestPossibleRegion());
	  for (maskIt.GoToBegin(); !maskIt.IsAtEnd(); ++maskIt)
	  {
		  if (maskIt.Get() == 1)
		  {
			  nearIndices.push_back(maskIt.GetIndex());
		  }
		  else if (maskIt.Get() == 2)
			  farIndices.push_back(maskIt.GetIndex());
	  }
	  //EGFRPredictor.SetInputImage(image);
	  EGFRStatusParams = EGFRPredictor.PredictEGFRStatus<ImageTypeFloat3D, Float4DImage>
		  (perfImg, Perfusion_Registered, nearIndices, farIndices, CAPTK::ImageExtension::NIfTI);

	  MITK_INFO << " PHI Value = " << EGFRStatusParams[0];
	  MITK_INFO << " Peak Height Ratio = " << EGFRStatusParams[1]/EGFRStatusParams[2];
	  MITK_INFO << " # Near Voxels = " << EGFRStatusParams[3];
	  MITK_INFO << " # far voxels = " << EGFRStatusParams[4];

	  m_Controls.PhiValue->setText(QString::number(EGFRStatusParams[0]));
	  m_Controls.Results->show();

      MITK_INFO << "  done";
}
