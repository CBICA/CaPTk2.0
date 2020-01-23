#include <CaPTkROIConstruction.h>

#include <CaPTkROIConstructionHelper.h>

namespace captk
{
struct ROIIndices
{
    std::vector<
        captk::ROIConstructionHelper<itk::Image<float,2>>::ROIProperties
    > ind2D;
    std::vector<
        captk::ROIConstructionHelper<itk::Image<float,3>>::ROIProperties
    > ind3D;
    unsigned int dimension = 3;
};
}

captk::ROIConstruction::ROIConstruction() :
    m_ROIIndices(new captk::ROIIndices()),
    m_CurrentIndex(0)
{
    
}

captk::ROIConstruction::~ROIConstruction()
{
    delete m_ROIIndices;
}


void captk::ROIConstruction::SetInputMask(mitk::LabelSetImage::Pointer /*input*/)
{
    // TODO: Get from LabelSet the SetSelectedROIsAndLabels(std::vector< int > roi, std::vector< std::string > roi_labels); part
    // TODO: Convert mitk to mitk

    // m_Input = ...;
}


void captk::ROIConstruction::Update(
    float window,
    bool  fluxNeumannCondition,
    bool  patchConstructionROI,
    bool  patchConstructionNone,
    float step)
{
    using ImageType2D = itk::Image<float,2>;
    using ImageType3D = itk::Image<float,3>;
    
    // TODO: ---- Construct std::vector< int > & std::vector< std::string > from LabelSet
    std::vector<int> roiValues;
    std::vector<std::string>  roiNames;

    // ---- Different operation depending on dimensionality
    if (m_Input->GetDimension() == 2)
    {
        // TODO: Convert mitk::LabelSetImage::Pointer to ImageType2D ....
        typename ImageType2D::Pointer inputITK;// = ...;

        captk::ROIConstructionHelper< ImageType2D > roiConstructor;
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
        // TODO: Convert mitk::LabelSetImage::Pointer to ImageType3D ....
        typename ImageType3D::Pointer inputITK;// = ...;

        captk::ROIConstructionHelper< ImageType3D > roiConstructor;
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


std::string captk::ROIConstruction::GetCurrentName()
{
    // TODO
    return "";
}