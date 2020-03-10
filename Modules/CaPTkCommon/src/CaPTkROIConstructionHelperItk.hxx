#include "CaPTkROIConstructionHelperItk.h"

namespace captk
{

template <typename TPixel, unsigned int VDim>
ROIConstructionHelperItk<TPixel,VDim>::ROIConstructionHelperItk(TImageTypePointer mask) 
{
    m_Mask = mask;
}

template <typename TPixel, unsigned int VDim>
ROIConstructionHelperItk<TPixel,VDim>::~ROIConstructionHelperItk()
{

}

template <typename TPixel, unsigned int VDim>
void ROIConstructionHelperItk<TPixel,VDim>::SetValuesAndNames(
    std::vector<int> values, 
    std::vector<std::string> names)
{
    m_Values = values;
    m_Names = names;
}

template <typename TPixel, unsigned int VDim>
void ROIConstructionHelperItk<TPixel,VDim>::Update(
    float radius,
    float step//, 
    // bool  fluxNeumannCondition,
    // bool  patchConstructionROI,
    // bool  patchConstructionNone
    )
{
    m_CurrentIndex = 0;

    captk::ROIConstructionImplementation< TImageType > roiConstructorItk;

    roiConstructorItk.SetInputMask(m_Mask);
    roiConstructorItk.SetSelectedROIsAndLabels(m_Values, m_Names);
    roiConstructorItk.SetLatticeGridStep(step);
    roiConstructorItk.SetLatticeWindowSize(2.0 * radius);
    // roiConstructorItk.SetBoundaryCondition(fluxNeumannCondition);
    // roiConstructorItk.SetPatchConstructionConditionROI(patchConstructionROI);
    // roiConstructorItk.SetPatchConstructionConditionNone(patchConstructionNone);
    
    roiConstructorItk.Update();

    // Show info
    MITK_INFO << "\nNumber of valid lattice points: " 
                << roiConstructorItk.GetNumberOfValidLatticePoints()
                << std::endl;

    MITK_INFO << "Lattice radius: " 
                << roiConstructorItk.GetLatticeRadius()
                << std::endl;

    m_Properties = roiConstructorItk.GetOutput();
}

template <typename TPixel, unsigned int VDim>
float ROIConstructionHelperItk<TPixel,VDim>::PopulateMask(mitk::LabelSetImage::Pointer miniMask)
{
    // Add label and label name
    mitk::Label::Pointer label = mitk::Label::New();
    label->SetName(m_Properties[m_CurrentIndex].label 
            + std::string("_") 
            + std::to_string(m_Properties[m_CurrentIndex].value));
    label->SetValue(1);
    miniMask->GetActiveLabelSet()->AddLabel(label);
    //miniMask->SetActiveLabel(label->GetValue());

    // Convert mitk::LabelSetImage::Pointer to TImageType
    using ImageToItkType = mitk::ImageToItk<TImageType>;
    typename ImageToItkType::Pointer imagetoitk = ImageToItkType::New();
    imagetoitk->SetInput(miniMask);
    imagetoitk->Update();
    TImageTypePointer miniMaskItk = imagetoitk->GetOutput();

    using TIteratorType = itk::ImageRegionIteratorWithIndex<TImageType>;
    TIteratorType iter_mm(
        miniMaskItk, miniMaskItk->GetBufferedRegion()
    );

    // Patch: this->m_Properties[m_CurrentIndex]
    // Patch is generally a collection of points
    // auto currentValue = this->m_Properties[m_CurrentIndex].value;
    for (auto& index : this->m_Properties[m_CurrentIndex].nonZeroIndeces)
    {
        iter_mm.SetIndex(index);
        iter_mm.Set(1); // MITK FE needs strictly binary images
    }

    return this->m_Properties[m_CurrentIndex].weight;
}

template <typename TPixel, unsigned int VDim>
void ROIConstructionHelperItk<TPixel,VDim>::GoToBegin()
{
    m_CurrentIndex = 0;
}

template <typename TPixel, unsigned int VDim>
bool ROIConstructionHelperItk<TPixel,VDim>::IsAtEnd()
{
    return (m_CurrentIndex == m_Properties.size() - 1);
}

template <typename TPixel, unsigned int VDim>
void ROIConstructionHelperItk<TPixel,VDim>::OnIncrement()
{
    m_CurrentIndex++;
}

}