#ifndef CaPTkROIConstruction_h
#define CaPTkROIConstruction_h

#include "mitkLabel.h"
#include "mitkLabelSetImage.h"

#include <vector>
#include <string>
#include <memory>

namespace captk
{
struct MITKCAPTKCOMMON_EXPORT ROIIndices;
}

namespace captk
{
/** \class ROIConstruction 
 * \brief Takes an input label mask and lattice parameters and produces masks
 * 
 * These masks are each at the intersection of input mask and lattice point.
 * The masks can be obtained one at a time, using HasNext() and GetNext().
 * Update(...) should be called prior to attempting to get the masks.
 */
class ROIConstruction
{
public:
    ROIConstruction();
    ~ROIConstruction();

    void SetInputMask(mitk::LabelSetImage::Pointer input);

    /** \brief Find all the different lattice points
     * 
     * See CaPTkROIConstructionHelper for what the parameters are
     */
    void Update(
        // JUST CALCULATE FOR ALL LABELS std::vector<mitk::Label::PixelType> rois,
        // CAN BE DEDUCED: std::vector< std::string > roi_labels,
        float window,
        bool  fluxNeumannCondition,
        bool  patchConstructionROI,
        bool  patchConstructionNone,
        float step
    );

    /** \brief Shows if there is another lattice mask left */
    bool HasNext();

    /** \brief Get the next lattice mask */
    mitk::LabelSetImage::Pointer GetNext();

    /** \brief Get the name of the current lattice mask */
    std::string GetCurrentName();

private:
    /** The input label mask */
    mitk::LabelSetImage::Pointer m_Input;

    ROIIndices* m_ROIIndices;
    
    size_t m_CurrentIndex;
};
}

#endif // ! CaPTkROIConstruction_h