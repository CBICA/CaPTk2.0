#include <CaPTkROIConstruction.h>

#include <mitkImageCast.h>
#include <itkExtractImageFilter.h>
#include <mitkImageAccessByItk.h>
#include <mitkLabelSetImageConverter.h>
#include <mitkITKImageImport.h>

captk::ROIConstruction::ROIConstruction() :
    m_CurrentIndex(0)
{
    
}

captk::ROIConstruction::~ROIConstruction()
{
    
}

void captk::ROIConstruction::Update(
    mitk::LabelSetImage::Pointer input,
    bool  lattice,
    float window,
    bool  fluxNeumannCondition,
    bool  patchConstructionROI,
    bool  patchConstructionNone,
    float step)
{
    // ---- Save template image for GetNext() use
    m_MaskTemplate = mitk::LabelSetImage::New();
    m_MaskTemplate->Initialize( //InitializeByLabeledImage(
        mitk::ConvertLabelSetImageToImage(input.GetPointer())
    );

    // ---- Construct roi values and names from LabelSet
    std::vector<int> roiValues;
    std::vector<std::string>  roiNames;
    mitk::LabelSet::Pointer labelSet = input->GetActiveLabelSet();
    mitk::LabelSet::LabelContainerConstIteratorType it;
    for (it = labelSet->IteratorConstBegin();
         it != labelSet->IteratorConstEnd();
         ++it)
    {
        if (it->second->GetValue() != 0)
        {
            std::cout << "Found label set name: " 
                      << it->second->GetName() << "\n"; 
            roiValues.push_back(it->second->GetValue());
            roiNames.push_back(it->second->GetName());
        }
    }

    // Create helper with the mask
    AccessByItk(input, CreateHelper);

    // Add ROI names and pixel values
    m_Helper->SetValuesAndNames(roiValues, roiNames);

    // Construct the ROI indices
    m_Helper->Update(
        lattice,
        window,
        fluxNeumannCondition,
        patchConstructionROI,
        patchConstructionNone,
        step
    );

    m_CurrentIndex = 0;
}

bool captk::ROIConstruction::HasNext()
{
    return (m_CurrentIndex < m_Helper->GetPropertiesSize());
}

mitk::LabelSetImage::Pointer captk::ROIConstruction::GetNext()
{
    auto result = mitk::LabelSetImage::New();

    result->Initialize(//InitializeByLabeledImage(
        mitk::ConvertLabelSetImageToImage(m_MaskTemplate.GetPointer())
    );

    m_Helper->PopulateMaskAtPatch(m_CurrentIndex, result);

    m_CurrentIndex++;
    return result;
}

void captk::ROIConstruction::GoToBegin()
{
    m_CurrentIndex = 0;
}

std::string captk::ROIConstruction::GetCurrentName()
{
    return m_Helper->GetPropertyLabel(m_CurrentIndex);
}

int captk::ROIConstruction::GetCurrentValue()
{
    return m_Helper->GetPropertyValue(m_CurrentIndex);
}