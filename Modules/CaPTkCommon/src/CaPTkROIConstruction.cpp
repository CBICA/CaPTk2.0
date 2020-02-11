#include <CaPTkROIConstruction.h>

#include <mitkImageCast.h>
#include <itkExtractImageFilter.h>

captk::ROIConstruction::ROIConstruction() :
    m_ROIIndices(new captk::ROIConstruction::ROIIndices()),
    m_CurrentIndex(0)
{
    
}

captk::ROIConstruction::~ROIConstruction()
{
    delete m_ROIIndices;
}


void captk::ROIConstruction::SetInputMask(mitk::LabelSetImage::Pointer input)
{   
    m_Input = input;
}

typename itk::Image<float,2>::Pointer
captk::ROIConstruction::Mitk3DLabelSetImageToItk2D(mitk::LabelSetImage::Pointer mask)
{
    using PixelType = float;
    typename itk::Image<PixelType, 2>::Pointer maskITK;
    
    typedef itk::Image<PixelType, 2> LabelsImageType2D;
    typedef itk::Image<PixelType, 3> LabelsImageType3D;
    typename LabelsImageType3D::Pointer maskITK3D;
    mitk::CastToItkImage(mask, maskITK3D);
    auto regionSize = maskITK3D->GetLargestPossibleRegion().GetSize();
    regionSize[2] = 0; // Only 2D image is needed
    LabelsImageType3D::IndexType regionIndex;
    regionIndex.Fill(0);    
    LabelsImageType3D::RegionType desiredRegion(regionIndex, regionSize);
    auto extractor = itk::ExtractImageFilter< LabelsImageType3D, LabelsImageType2D >::New();
    extractor->SetExtractionRegion(desiredRegion);
    extractor->SetInput(maskITK3D);
    extractor->SetDirectionCollapseToIdentity();
    extractor->Update();
    maskITK = extractor->GetOutput();
    maskITK->DisconnectPipeline();

    return maskITK;
}

void captk::ROIConstruction::Update(
    bool  /*lattice*/,
    float window,
    bool  fluxNeumannCondition,
    bool  patchConstructionROI,
    bool  patchConstructionNone,
    float step)
{
    using ImageType2D = itk::Image<float,2>;
    using ImageType3D = itk::Image<float,3>;
    
    // ---- Construct roi values and names from LabelSet
    std::vector<int> roiValues;
    std::vector<std::string>  roiNames;
    mitk::LabelSet::Pointer labelSet = m_Input->GetActiveLabelSet();
    mitk::LabelSet::LabelContainerConstIteratorType it;
    for (it = labelSet->IteratorConstBegin();
         it != labelSet->IteratorConstEnd();
         ++it)
    {
        if (it->second->GetValue() != 0)
        {
            roiValues.push_back(it->second->GetValue());
            roiNames.push_back(it->second->GetName());
        }
    }

    // ---- Different operation depending on dimensionality
    if (m_Input->GetDimension() == 2 || (m_Input->GetDimension() == 3 && m_Input->GetDimension(2) == 1))
    {
        // [ 2D ]

        // TODO: Convert mitk::LabelSetImage::Pointer to ImageType2D ....
        typename ImageType2D::Pointer inputITK;
        if (m_Input->GetDimension() == 2) // unlikely
        {
            // 2D image that is a 2D LabelSetImage
            mitk::CastToItkImage(m_Input, inputITK);
        }
        else
        {
            // 2D image that is a 3D LabelSetImage with the 3rd dim's size == 1
            inputITK = this->Mitk3DLabelSetImageToItk2D(m_Input);
        }
        
        captk::ROIConstructionImplementation< ImageType2D > roiConstructor;
        roiConstructor.SetInputMask(inputITK);
        roiConstructor.SetSelectedROIsAndLabels(roiValues, roiNames);
        roiConstructor.SetLatticeGridStep(step);
        roiConstructor.SetLatticeWindowSize(window);
        roiConstructor.SetBoundaryCondition(fluxNeumannCondition);
        roiConstructor.SetPatchConstructionConditionROI(patchConstructionROI);
        roiConstructor.SetPatchConstructionConditionNone(patchConstructionNone);
        roiConstructor.Update();

        m_ROIIndices->dimension = 2;
        m_ROIIndices->ind2D = roiConstructor.GetOutput();
    }
    else
    {
        // Convert mitk::LabelSetImage::Pointer to ImageType3D ....
        typename ImageType3D::Pointer inputITK;
        mitk::CastToItkImage(m_Input, inputITK);

        captk::ROIConstructionImplementation< ImageType3D > roiConstructor;
        roiConstructor.SetInputMask(inputITK);
        roiConstructor.SetSelectedROIsAndLabels(roiValues, roiNames);
        roiConstructor.SetLatticeGridStep(step);
        roiConstructor.SetLatticeWindowSize(window);
        roiConstructor.SetBoundaryCondition(fluxNeumannCondition);
        roiConstructor.SetPatchConstructionConditionROI(patchConstructionROI);
        roiConstructor.SetPatchConstructionConditionNone(patchConstructionNone);
        roiConstructor.Update();

        m_ROIIndices->dimension = 3;
        m_ROIIndices->ind3D = roiConstructor.GetOutput();        
    }
    

}


bool captk::ROIConstruction::HasNext()
{
    // TODO
    return false;
}


mitk::LabelSetImage::Pointer captk::ROIConstruction::GetNext()
{
    // TODO
    return mitk::LabelSetImage::New();
}

void captk::ROIConstruction::GoToBegin()
{
    m_CurrentIndex = 0;
}

std::string captk::ROIConstruction::GetCurrentName()
{
    // TODO
    return "";
}

int captk::ROIConstruction::GetCurrentValue()
{
    // TODO
    return 0;
}