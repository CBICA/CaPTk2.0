#include <CaPTkROIConstruction.h>

#include <CaPTkROIConstructionHelper.h>

captk::ROIConstruction::ROIConstruction() :
    m_CaPTkROIConstructionHelper(CaPTkROIConstructionHelper())
{
    
}

captk::ROIConstruction::~ROIConstruction()
{
    
}


void captk::ROIConstruction::SetInputMask(typename TImageType::Pointer input)
{
    // TODO: Get from LabelSet the SetSelectedROIsAndLabels(std::vector< int > roi, std::vector< std::string > roi_labels); part
    // TODO: Convert mitk to mitk

    // m_CaPTkROIConstructionHelper.SetInputMask()
}


void captk::ROIConstruction::Update()
{
    
}


bool captk::ROIConstruction::HasNext()
{
    
}


mitk::LabelSetImage::Pointer captk::ROIConstruction::GetNext()
{
    
}


std::string captk::ROIConstruction::GetCurrentName()
{
    
}

void captk::ROIConstruction::SetSelectedROIsAndLabels(std::vector< int > roi, std::vector< std::string > roi_labels)
{
    
}

void captk::ROIConstruction::SetLatticeWindowSize(float window)
{
    m_CaPTkROIConstructionHelper.SetLatticeWindowSize(window);
}

void captk::ROIConstruction::SetBoundaryCondition(bool fluxNeumannCondition)
{
    m_CaPTkROIConstructionHelper.SetBoundaryCondition(fluxNeumannCondition);
}


void captk::ROIConstruction::SetPatchConstructionConditionROI(bool patchConstructionROI)
{
    m_CaPTkROIConstructionHelper.SetPatchConstructionConditionROI(patchConstructionROI);
}


void captk::ROIConstruction::SetPatchConstructionConditionNone(bool patchConstructionNone) { m_patchBoundaryDisregarded = patchConstructionNone; }
{
    m_CaPTkROIConstructionHelper.SetPatchConstructionConditionNone(patchConstructionNone);
}

void captk::ROIConstruction::SetLatticeGridStep(float step)
{
    m_CaPTkROIConstructionHelper.SetLatticeGridStep(step);
}