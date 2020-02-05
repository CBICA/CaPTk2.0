template < typename TPixel, unsigned int VImageDimension >
void captk::ROIConstruction::CreateHelper(
    typename itk::Image<TPixel,VImageDimension>* mask)
{
    m_Helper = std::shared_ptr<captk::ROIConstructionHelperBase>(
        new captk::ROIConstructionHelper<TPixel, VImageDimension>(mask)
    );
}