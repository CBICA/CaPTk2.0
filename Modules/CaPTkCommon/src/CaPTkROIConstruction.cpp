#include <CaPTkROIConstruction.h>

#include <mitkImageCast.h>
#include <itkExtractImageFilter.h>
#include <mitkImageAccessByItk.h>

#include <mitkITKImageImport.h>
// #include <boost/preprocessor/tuple/rem.hpp>
// #include <mitkImageAccessByItk.h>

captk::ROIConstruction::ROIConstruction() :
    m_CurrentIndex(0)
{
    
}

captk::ROIConstruction::~ROIConstruction()
{
    
}


void captk::ROIConstruction::SetInputMask(mitk::LabelSetImage::Pointer input)
{   
    m_Input = input;
}

typename itk::Image<int,2>::Pointer
captk::ROIConstruction::Mitk3DLabelSetImageToItk2D(mitk::LabelSetImage::Pointer mask)
{
    using PixelType = int;
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
    bool  lattice,
    float window,
    bool  fluxNeumannCondition,
    bool  patchConstructionROI,
    bool  patchConstructionNone,
    float step)
{
    // using ImageType2D = itk::Image<int,2>;
    // using ImageType3D = itk::Image<int,3>;
    // using ImageType4D = itk::Image<int,4>;
    
    // // ---- Clear old results ----
    // m_ROIIndices->ind2D.clear();
    // m_ROIIndices->ind3D.clear();
    // m_ROIIndices->ind4D.clear();

    // // ---- Check if lattice ----
    // if (!lattice)
    // {
    //     step = window = 0;
    // }

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

    AccessByItk(m_Input, CreateHelper);

    if (m_Input->GetDimension() == 2)
    {
        m_Helper2D.SetValuesAndNames(roiValues, roiNames);
        m_Helper2D.Update(
            lattice,
            window,
            fluxNeumannCondition,
            patchConstructionROI,
            patchConstructionNone,
            step
        );
    }
    else if (m_Input->GetDimension() == 3)
    {
        m_Helper3D.SetValuesAndNames(roiValues, roiNames);
        m_Helper3D.Update(
            lattice,
            window,
            fluxNeumannCondition,
            patchConstructionROI,
            patchConstructionNone,
            step
        );
    }
    else if (m_Input->GetDimension() == 4)
    {
        m_Helper3D.SetValuesAndNames(roiValues, roiNames);
        m_Helper3D.Update(
            lattice,
            window,
            fluxNeumannCondition,
            patchConstructionROI,
            patchConstructionNone,
            step
        );
    }

    // // ---- Different operation depending on dimensionality
    // if (m_Input->GetDimension() == 2 || (m_Input->GetDimension() == 3 && m_Input->GetDimension(2) == 1))
    // {
    //     // [ 2D ]

    //     // TODO: Convert mitk::LabelSetImage::Pointer to ImageType2D ....
    //     typename ImageType2D::Pointer inputITK;
    //     if (m_Input->GetDimension() == 2) // unlikely
    //     {
    //         // 2D image that is a 2D LabelSetImage
    //         mitk::CastToItkImage(m_Input, inputITK);
    //     }
    //     else
    //     {
    //         // 2D image that is a 3D LabelSetImage with the 3rd dim's size == 1
    //         inputITK = this->Mitk3DLabelSetImageToItk2D(m_Input);
    //     }
        
    //     captk::ROIConstructionImplementation< ImageType2D > roiConstructor;
    //     roiConstructor.SetInputMask(inputITK);
    //     roiConstructor.SetSelectedROIsAndLabels(roiValues, roiNames);
    //     roiConstructor.SetLatticeGridStep(step);
    //     roiConstructor.SetLatticeWindowSize(window);
    //     roiConstructor.SetBoundaryCondition(fluxNeumannCondition);
    //     roiConstructor.SetPatchConstructionConditionROI(patchConstructionROI);
    //     roiConstructor.SetPatchConstructionConditionNone(patchConstructionNone);
    //     roiConstructor.Update();

    //     m_ROIIndices->dimension = 2;
    //     m_ROIIndices->ind2D = roiConstructor.GetOutput();
    // }
    // else if (m_Input->GetDimension() == 3)
    // {
    //     // [ 3D ]

    //     // Convert mitk::LabelSetImage::Pointer to ImageType3D ....
    //     typename ImageType3D::Pointer inputITK;
    //     mitk::CastToItkImage(m_Input, inputITK);

    //     captk::ROIConstructionImplementation< ImageType3D > roiConstructor;
    //     roiConstructor.SetInputMask(inputITK);
    //     roiConstructor.SetSelectedROIsAndLabels(roiValues, roiNames);
    //     roiConstructor.SetLatticeGridStep(step);
    //     roiConstructor.SetLatticeWindowSize(window);
    //     roiConstructor.SetBoundaryCondition(fluxNeumannCondition);
    //     roiConstructor.SetPatchConstructionConditionROI(patchConstructionROI);
    //     roiConstructor.SetPatchConstructionConditionNone(patchConstructionNone);
    //     roiConstructor.Update();

    //     m_ROIIndices->dimension = 3;
    //     m_ROIIndices->ind3D = roiConstructor.GetOutput();
    // }
    // else if (m_Input->GetDimension() == 4)
    // {
    //     // [ 4D ]

    //     // Convert mitk::LabelSetImage::Pointer to ImageType4D ....
    //     typedef mitk::ImageToItk<ImageType4D> ImageToItkType;
    //     typename ImageToItkType::Pointer imagetoitk = ImageToItkType::New();
    //     imagetoitk->SetInput(m_Input);
    //     imagetoitk->Update();
    //     typename ImageType4D::Pointer inputITK = imagetoitk->GetOutput();

    //     // mitk::CastToItkImage(m_Input, inputITK);

    //     captk::ROIConstructionImplementation< ImageType4D > roiConstructor;
    //     roiConstructor.SetInputMask(inputITK);
    //     // try
    //     // {
    //     //     AccessByItk(m_Input, roiConstructor.SetInputMask);
    //     // }
    //     // catch (const mitk::AccessByItkException& e)
    //     // {
    //     //     // mitk::Image is of wrong pixel type or dimension,
    //     //     // insert error handling here
    //     // }
    //     roiConstructor.SetSelectedROIsAndLabels(roiValues, roiNames);
    //     roiConstructor.SetLatticeGridStep(step);
    //     roiConstructor.SetLatticeWindowSize(window);
    //     roiConstructor.SetBoundaryCondition(fluxNeumannCondition);
    //     roiConstructor.SetPatchConstructionConditionROI(patchConstructionROI);
    //     roiConstructor.SetPatchConstructionConditionNone(patchConstructionNone);
    //     roiConstructor.Update();

    //     m_ROIIndices->dimension = 4;
    //     m_ROIIndices->ind4D = roiConstructor.GetOutput();
    // }
}


bool captk::ROIConstruction::HasNext()
{
    // size_t total = 0;
    // if (m_ROIIndices->dimension == 2)
    // {
    //     total = m_ROIIndices->ind2D.size();
    // }
    // else if (m_ROIIndices->dimension == 3)
    // {
    //     total = m_ROIIndices->ind3D.size();
    // }
    // else if (m_ROIIndices->dimension == 4)
    // {
    //     total = m_ROIIndices->ind4D.size();
    // }

    size_t total = 0;
    if (m_Input->GetDimension() == 2)
    {
        total = m_Helper2D.GetPropertiesSize();
    }
    else if (m_Input->GetDimension() == 3)
    {
        total = m_Helper3D.GetPropertiesSize();
    }
    else if (m_Input->GetDimension() == 4)
    {
        total = m_Helper4D.GetPropertiesSize();
    }
    return (m_CurrentIndex < total - 1);
}


mitk::LabelSetImage::Pointer captk::ROIConstruction::GetNext()
{
    // TODO
    auto result = mitk::LabelSetImage::New();
    result->InitializeByLabeledImage(static_cast<mitk::Image::Pointer>(m_Input));

    // AccessByItk(result.GetPointer(), 
    //     ItkMaskFromIndeces
    // );

    // // AccessByItk_n(result.GetPointer(), 
    // //     ItkMaskFromIndeces, 
    // //     std::tuple<
    // //         std::shared_ptr<captk::ROIConstruction::ROIIndices>
    // //     >(m_ROIIndices)
    // // );

    //         //     currentMaskIterator.SetIndex(allROIs[j].nonZeroIndeces[m]);
    //         // currentMaskIterator.Set(1);

    if (m_Input->GetDimension() == 2)
    {
        m_Helper2D.PopulateMaskAtPatch(m_CurrentIndex, result);
    }
    else if (m_Input->GetDimension() == 3)
    {
        m_Helper3D.PopulateMaskAtPatch(m_CurrentIndex, result);
    }
    else if (m_Input->GetDimension() == 4)
    {
        m_Helper4D.PopulateMaskAtPatch(m_CurrentIndex, result);
    }

    m_CurrentIndex++;
    return result;
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