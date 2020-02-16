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
#include <mitkNodePredicateOr.h>

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

// namespace
// {
//   // Helper function to create a fully set up instance of our
//   // ExampleImageInteractor, based on the state machine specified in Paint.xml
//   // as well as its configuration in PaintConfig.xml. Both files are compiled
//   // into ExtPHIEstimatorModule as resources.
//   static ExampleImageInteractor::Pointer CreateExampleImageInteractor()
//   {
//     auto PHIEstimatorModule = us::ModuleRegistry::GetModule("MitkCaPTkPHIEstimator");

//     if (nullptr != PHIEstimatorModule)
//     {
//       auto interactor = ExampleImageInteractor::New();
//       interactor->LoadStateMachine("Paint.xml", PHIEstimatorModule);
//       interactor->SetEventConfig("PaintConfig.xml", PHIEstimatorModule);
//       return interactor;
//     }

//     return nullptr;
//   }
// }

// Don't forget to initialize the VIEW_ID.
const std::string QmitkPHIEstimatorView::VIEW_ID = "org.mitk.views.captk.phiestimator";

QmitkPHIEstimatorView::QmitkPHIEstimatorView() :
	m_ReferenceNode(nullptr),
	m_WorkingNode(nullptr)
{
	m_SegmentationPredicate = mitk::NodePredicateAnd::New();
	m_SegmentationPredicate->AddPredicate(
		mitk::TNodePredicateDataType<mitk::LabelSetImage>::New()
	);
	m_SegmentationPredicate->AddPredicate
	(mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object"))
	);

	mitk::TNodePredicateDataType<mitk::Image>::Pointer isImage = mitk::TNodePredicateDataType<mitk::Image>::New();
	mitk::NodePredicateProperty::Pointer isBinary =
		mitk::NodePredicateProperty::New("binary", mitk::BoolProperty::New(true));
	mitk::NodePredicateAnd::Pointer isMask = mitk::NodePredicateAnd::New(isBinary, isImage);

	mitk::NodePredicateDataType::Pointer isDwi = mitk::NodePredicateDataType::New("DiffusionImage");
	mitk::NodePredicateDataType::Pointer isDti = mitk::NodePredicateDataType::New("TensorImage");
	mitk::NodePredicateDataType::Pointer isOdf = mitk::NodePredicateDataType::New("OdfImage");
	auto isSegment = mitk::NodePredicateDataType::New("Segment");

	mitk::NodePredicateOr::Pointer validImages = mitk::NodePredicateOr::New();
	validImages->AddPredicate(mitk::NodePredicateAnd::New(isImage, mitk::NodePredicateNot::New(isSegment)));
	validImages->AddPredicate(isDwi);
	validImages->AddPredicate(isDti);
	validImages->AddPredicate(isOdf);

	m_ReferencePredicate = mitk::NodePredicateAnd::New();
	m_ReferencePredicate->AddPredicate(validImages);
	m_ReferencePredicate->AddPredicate(mitk::NodePredicateNot::New(m_SegmentationPredicate));
	m_ReferencePredicate->AddPredicate(mitk::NodePredicateNot::New(isMask));
	m_ReferencePredicate->AddPredicate(mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object")));

}

QmitkPHIEstimatorView::~QmitkPHIEstimatorView()
{

}

void QmitkPHIEstimatorView::CreateQtPartControl(QWidget* parent)
{
  // Setting up the UI is a true pleasure when using .ui files, isn't it?
  m_Controls.setupUi(parent);
  m_Controls.groupBox_Result->hide();

  // *------------------------
  // * DATA SELECTION WIDGETS
  // *------------------------

  m_Controls.m_cbReferenceNodeSelector->SetAutoSelectNewItems(true);
  m_Controls.m_cbReferenceNodeSelector->SetPredicate(m_ReferencePredicate);
  m_Controls.m_cbReferenceNodeSelector->SetDataStorage(this->GetDataStorage());

  m_Controls.m_cbWorkingNodeSelector->SetAutoSelectNewItems(true);
  m_Controls.m_cbWorkingNodeSelector->SetPredicate(m_SegmentationPredicate);
  m_Controls.m_cbWorkingNodeSelector->SetDataStorage(this->GetDataStorage());

  // signals/slots connections
  connect(m_Controls.m_cbReferenceNodeSelector,
	  SIGNAL(OnSelectionChanged(const mitk::DataNode *)),
	  this,
	  SLOT(OnReferenceSelectionChanged(const mitk::DataNode *)));

  connect(m_Controls.m_cbWorkingNodeSelector,
	  SIGNAL(OnSelectionChanged(const mitk::DataNode *)),
	  this,
	  SLOT(OnSegmentationSelectionChanged(const mitk::DataNode *)));

  // Wire up the UI widgets with our functionality.
  connect(m_Controls.processImageButton, SIGNAL(clicked()), this, SLOT(ProcessSelectedImage()));
}

void QmitkPHIEstimatorView::ResetResults()
{
	// clear results shown in UI
	m_Controls.PhiValue->clear();
	m_Controls.PhiRatioValue->clear();
	m_Controls.NearROIVoxelsUsedValue->clear();
	m_Controls.FarROIVoxelsUsedValue->clear();
	m_Controls.PHIThresholdValue->clear();
	m_Controls.TumorTypeValue->clear();

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
      return;
    }
  }

}

void QmitkPHIEstimatorView::OnReferenceSelectionChanged(const mitk::DataNode * /*node*/)
{
	m_Controls.groupBox_Result->hide();
	this->ResetResults();
}

void QmitkPHIEstimatorView::OnSegmentationSelectionChanged(const mitk::DataNode * /*node*/)
{
	m_Controls.groupBox_Result->hide();
	this->ResetResults();
}

void QmitkPHIEstimatorView::ProcessSelectedImage()
{
	typedef itk::Image<unsigned short, 3> MaskImageType;
	typedef itk::Image<short, 4> PerfusionImageType;
	typedef itk::Image<float, 4> Float4DImage;
	MaskImageType::Pointer maskimg;
	Float4DImage::Pointer perfImg;

	mitk::DataNode::Pointer referenceNode = m_Controls.m_cbReferenceNodeSelector->GetSelectedNode();

	// is something selected
	if (referenceNode.IsNull())
	{
		QMessageBox::information(nullptr, "New PHI Estimator Session", "Please load a reference image before starting some action.");
		return;
	}

	// Something is selected, but does it contain data?
	mitk::BaseData::Pointer rdata = referenceNode->GetData();
	if (rdata.IsNull())
	{
		QMessageBox::information(nullptr, "New PHI Estimator Session", "Please load a reference image before starting some action.");
		return;
	}

	// Something is selected and it contains data, but is it an image?
	mitk::Image::Pointer rimage = dynamic_cast<mitk::Image*>(rdata.GetPointer());
	if (rimage.IsNull())
	{
		QMessageBox::information(nullptr, "New PHI Estimator Session", "Please load a reference image before starting some action.");
		return;
	}

	// is segmentation selected
	mitk::DataNode::Pointer workingNode = m_Controls.m_cbWorkingNodeSelector->GetSelectedNode();
	if (workingNode.IsNull())
	{
		QMessageBox::information(nullptr, "New PHI Estimator Session", "Please load a seed image before starting some action.");
		return;
	}

	// Something is selected, but does it contain data?
	mitk::BaseData::Pointer wdata = workingNode->GetData();
	if (wdata.IsNull())
	{
		QMessageBox::information(nullptr, "New PHI Estimator Session", "Please load a seed image before starting some action.");
		return;
	}

	// Something is selected and it contains data, but is it an image?
	mitk::Image::Pointer maskimage = dynamic_cast<mitk::Image*>(wdata.GetPointer());
	if (maskimage.IsNull())
	{
		QMessageBox::information(nullptr, "New PHI Estimator Session", "Please load a seed image before starting some action.");
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

      // We're finally using the ExampleImageFilter
 
	  //typename PerfusionImageType::Pointer pimg = PerfusionImageType::New();
	  //mitk::CastToItkImage(image, pimg);
	  PerfusionImageType::Pointer pimg = mitk::ImageToItkImage<short,4>(rimage);

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

	  // update results to be shown in UI
	  m_Controls.PhiValue->setText(QString::number(EGFRStatusParams[0]));
	  m_Controls.PhiRatioValue->setText(QString::number(EGFRStatusParams[1] / EGFRStatusParams[2]));
	  m_Controls.NearROIVoxelsUsedValue->setText(QString::number(EGFRStatusParams[3]) + "/" + QString::number(nearIndices.size()));
	  m_Controls.FarROIVoxelsUsedValue->setText(QString::number(EGFRStatusParams[4]) + "/" + QString::number(farIndices.size()));
	  m_Controls.PHIThresholdValue->setText(QString::number(0.1377));

	  QString tumorType;
	  //! if phi value less than threshold, then tumor type is mutant else wildtype
	  if (EGFRStatusParams[0] < 0.1377) // threshold = 0.1377 
		  tumorType = "EGFRvIII-Mutant";
	  else
		  tumorType = "EGFRvIII-Wildtype";

	  m_Controls.TumorTypeValue->setText(tumorType);

	  m_Controls.groupBox_Result->show();

      MITK_INFO << "  done";
}
