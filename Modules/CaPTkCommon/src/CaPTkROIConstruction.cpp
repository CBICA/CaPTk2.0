#include <CaPTkROIConstruction.h>

#include <CaPTkROIConstructionHelper.h>

captk::ROIConstruction::ROIConstruction() :
    m_CaPTkROIConstructionHelper(CaPTkROIConstructionHelper())
{
    
}

captk::ROIConstruction::~ROIConstruction()
{
    
}


captk::ROIConstruction::void SetInputMask(typename TImageType::Pointer input)
{
    // TODO: Get from LabelSet the SetSelectedROIsAndLabels(std::vector< int > roi, std::vector< std::string > roi_labels); part
    // TODO: Convert mitk to mitk

    // m_CaPTkROIConstructionHelper.SetInputMask()
}


captk::ROIConstruction::void Update()
{
    
}


captk::ROIConstruction::bool HasNext()
{
    
}


captk::ROIConstruction::mitk::LabelSetImage::Pointer GetNext()
{
    
}


captk::ROIConstruction::std::string GetCurrentName()
{
    
}



/* ---- Parameters (these just call the respective functions in CaPTkROIConstructionHelper) ---- */

/** \brief Set the requested ROIs and Labels
 * 
 * \param roi The vector of ROIs as integers (computed in FeatureExtraction class)
 * \param roi_labels The vector of ROI labels as strings
*/
captk::ROIConstruction::void SetSelectedROIsAndLabels(std::vector< int > roi, std::vector< std::string > roi_labels)
{
    
}


/** \brief Sets the lattice window size (radius around each grid node)
 * 
 * \param window Input Lattice Window in mm
*/
captk::ROIConstruction::void SetLatticeWindowSize(float window) { m_latticeWindow = window; }
{
    
}


/** \brief This sets whether the fluxNeumann Boundary condition is to be used or not.
 * 
 * Ref: https://itk.org/Doxygen/html/classitk_1_1ZeroFluxNeumannBoundaryCondition.html
*/
captk::ROIConstruction::void SetBoundaryCondition(bool fluxNeumannCondition) { m_fluxNeumannEnabled = fluxNeumannCondition; }
{
    
}


/** \brief Sets whether the entire patch is taken into consideration or just the part in the selected ROI
*/
captk::ROIConstruction::void SetPatchConstructionConditionROI(bool patchConstructionROI) { m_patchOnRoiEnabled = patchConstructionROI; }
{
    
}


/** \brief Sets whether the patch gets diregarded when the weight is less than 1
*/
captk::ROIConstruction::void SetPatchConstructionConditionNone(bool patchConstructionNone) { m_patchBoundaryDisregarded = patchConstructionNone; }
{
    
}


/** \brief Sets the lattice grid step (distance between 2 grid nodes)
 * 
 * \param step Input Lattice Grid Step in mm
*/
void SetLatticeGridStep(float step);