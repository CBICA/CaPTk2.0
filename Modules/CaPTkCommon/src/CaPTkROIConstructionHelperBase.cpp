#include "CaPTkROIConstructionHelperBase.h"

captk::ROIConstructionHelperBase::ROIConstructionHelperBase()
{

}

captk::ROIConstructionHelperBase::~ROIConstructionHelperBase()
{

}

void captk::ROIConstructionHelperBase::Update(
    float /*radius*/,
    float /*step*/)
{
    // Cannot have pure virtual because we want to use the base class
}

void captk::ROIConstructionHelperBase::GoToBegin()
{

}

bool captk::ROIConstructionHelperBase::IsAtEnd()
{
    return true; // Return is required and we don't want this to be pure virtual
}

void captk::ROIConstructionHelperBase::SetValuesAndNames(
    std::vector<int>         /*values*/,
    std::vector<std::string> /*names*/)
{
    
}

captk::ROIConstructionHelperBase& captk::ROIConstructionHelperBase::operator++() // prefix
{
    this->OnIncrement(); // call actual operation function
    return *this;
}

captk::ROIConstructionHelperBase captk::ROIConstructionHelperBase::operator++(int) // suffix (calls prefix)
{
    ROIConstructionHelperBase tmp(*this);
    ++*this; // call prefix
    return tmp;
}

float captk::ROIConstructionHelperBase::PopulateMask(mitk::LabelSetImage::Pointer /*rMask*/)
{
    return 0.0f; // Return is required and we don't want this to be pure virtual
}

void captk::ROIConstructionHelperBase::OnIncrement()
{

}