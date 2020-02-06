template < typename TPixel, unsigned int VImageDimension >
void captk::ROIConstruction::CreateHelper(
    typename itk::Image<TPixel,VImageDimension>* mask)
{
    m_Helper = captk::ROIConstructionHelper<TPixel, VImageDimension>(mask);
}