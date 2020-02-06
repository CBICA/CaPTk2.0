#include <CaPTkROIConstruction.h>

#include <mitkImageCast.h>
#include <itkExtractImageFilter.h>
#include <mitkImageAccessByItk.h>
#include <mitkLabelSetImageConverter.h>
#include <mitkITKImageImport.h>

#include "CaPTkROIConstructionHelperBase.h"

captk::ROIConstruction::ROIConstruction()
{
    
}

captk::ROIConstruction::~ROIConstruction()
{
    
}

void captk::ROIConstruction::Update(
        mitk::LabelSetImage::Pointer input,
        captk::ROIConstructionHelperBase::MODE mode,
        float radius,
        float step)
{
    // ---- Save template image for GetNext() use
    m_MaskTemplate = mitk::LabelSetImage::New();
    m_MaskTemplate->Initialize(
        mitk::ConvertLabelSetImageToImage(input.GetPointer())
    ); // This creates an empty copy, with the same meta-data

    // Create helper with the mask, by calling CreateHelper
    // AccessByItk calls the function with the ITK image (not MITK)
    // using the correct template
    AccessByItk(input, CreateHelper);

    m_Helper.SetMode(mode);

    // Construct the ROI indices
    m_Helper.Update(
        radius,
        step
    );
}

bool captk::ROIConstruction::IsAtEnd()
{
    return m_Helper.IsAtEnd();
}

float captk::ROIConstruction::PopulateMask(
    mitk::LabelSetImage::Pointer& rMask)
{
    rMask->Initialize(
        mitk::ConvertLabelSetImageToImage(m_MaskTemplate.GetPointer())
    ); // This creates an empty copy, with the same meta-data

    return m_Helper.PopulateMask(rMask);
}

void captk::ROIConstruction::GoToBegin()
{
    m_Helper.GoToBegin();
}

captk::ROIConstruction& captk::ROIConstruction::operator++() //suffix
{
    m_Helper++; // actual operation
    return *this;
}

captk::ROIConstruction captk::ROIConstruction::operator++(int) //postfix(calls suffix)
{
    ROIConstruction tmp(*this);
    operator++(); // call suffix
    return tmp;
}