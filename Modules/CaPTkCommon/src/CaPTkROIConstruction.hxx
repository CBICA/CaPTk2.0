template < typename TPixel, unsigned int VImageDimension >
void captk::ROIConstruction::CreateHelper(
    typename itk::Image<TPixel,VImageDimension>* mask)
{
    // Create ROIConstructionHelperItk but cast it to
    // ROIConstructionHelperBase to avoid templates
    m_Helper = std::shared_ptr<ROIConstructionHelperBase>(
        new ROIConstructionHelperItk<TPixel, VImageDimension>(mask)
    );
}
