#include <CaPTkROIConstruction.h>

#include <mitkImageCast.h>
#include <itkExtractImageFilter.h>
#include <mitkImageAccessByItk.h>
#include <mitkLabelSetImageConverter.h>
#include <mitkITKImageImport.h>

#include "CaPTkROIConstructionItkHelperBase.h"

captk::ROIConstruction::ROIConstruction() :
    m_CurrentIndex(0)
{
    
}

captk::ROIConstruction::~ROIConstruction()
{
    
}

void captk::ROIConstruction::Update(
        mitk::LabelSetImage::Pointer input,
        captk::ROIConstructionHelper::MODE mode,
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

    m_Helper->SetMode(mode);

    // Construct the ROI indices
    m_Helper->Update(
        radius,
        step
    );

    m_CurrentIndex = 0;
}

bool captk::ROIConstruction::IsAtEnd()
{
    return (m_CurrentIndex >= m_Helper->GetPropertiesSize());
}

mitk::LabelSetImage::Pointer captk::ROIConstruction::Get(mitk::LabelSetImage::Pointer& rMask)
{
    auto result = mitk::LabelSetImage::New();

    result->Initialize(
        mitk::ConvertLabelSetImageToImage(m_MaskTemplate.GetPointer())
    ); // This creates an empty copy, with the same meta-data


    m_Helper->PopulateMaskAtPatch(m_CurrentIndex, result);

    return result;
}

void captk::ROIConstruction::GoToBegin()
{
    m_CurrentIndex = 0;
}