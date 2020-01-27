// #include "itkImageRegionIteratorWithIndex.h"

// template < typename TPixel>//, 2/*unsigned int VImageDimension*/ >
// void captk::ROIConstruction::ItkMaskFromIndeces<2>(
//     typename itk::Image<TPixel,2/*VImageDimension*/>* mask)
// {
//     using TImageType = itk::Image<TPixel, 2/*VImageDimension*/>;
//     using TIteratorType = itk::ImageRegionIteratorWithIndex<TImageType>;
//     TIteratorType currentMaskIterator(
//         mask, mask->GetBufferedRegion()
//     );

//     if (this->m_ROIIndices->dimension == 2)
//     {
//         // Patch: this->m_ROIIndices->ind2D[m_CurrentIndex]
//         // Patch == collection of points
//         for (auto& index : this->m_ROIIndices->ind2D[m_CurrentIndex].nonZeroIndeces)
//         {
//             currentMaskIterator.SetIndex(index);
//             currentMaskIterator.Set(1);
//         }
//     }
// }

// template <>// < typename TPixel>, unsigned int VImageDimension >
// void captk::ROIConstruction::CreateHelper<short int,2>(
//     typename itk::Image<short int,2/*VImageDimension*/>* mask)
// {
//     // if (VImageDimension == 2)
//     // {
//         // assert(VImageDimension == 2);
//         // m_Helper2D = captk::ROIConstructionItkHelper<short int, 2>(mask);
//         m_Helper2D.SetMask(mask);
//     // }
//     // else if (VImageDimension == 3)
//     // {
//     //     assert(VImageDimension == 3);
//     //     m_Helper3D = captk::ROIConstructionItkHelper<short int, VImageDimension>(mask);
//     // }
//     // else if (VImageDimension == 4)
//     // {
//     //     assert(VImageDimension == 4);
//     //     m_Helper4D = captk::ROIConstructionItkHelper<short int, VImageDimension>(mask);
//     // }
// }

// template <>
// void captk::ROIConstruction::CreateHelper<short int,3>(
//     typename itk::Image<short int,3>* mask)
// {
//     // m_Helper3D = captk::ROIConstructionItkHelper<short int, 3>(mask);
//     m_Helper3D.SetMask(mask);
// }

// template <>
// void captk::ROIConstruction::CreateHelper<short int,4>(
//     typename itk::Image<short int,4>* mask)
// {
//     // m_Helper4D = captk::ROIConstructionItkHelper<short int, 4>(mask);
//     m_Helper4D.SetMask(mask);
// }

template < typename TPixel, unsigned int VImageDimension >
void captk::ROIConstruction::CreateHelper(
    typename itk::Image<TPixel,VImageDimension>* mask)
{
    // std::cout << "[captk::ROIConstruction::CreateHelper\n"
    //           << "GENERIC CASE, SHOULDN'T BE REACHED!]\n";
    // if (std::is_same<TPixel, unsigned int>::value)
    // {
    //     std::cout << "unsigned int\n";
    // }
    // if (std::is_same<TPixel, int>::value)
    // {
    //     std::cout << "int\n";
    // }
    // if (std::is_same<TPixel, short>::value)
    // {
    //     std::cout << "short\n";
    // }
    // if (std::is_same<TPixel, float>::value)
    // {
    //     std::cout << "float\n";
    // }
    // if (std::is_same<TPixel, double>::value)
    // {
    //     std::cout << "double\n";
    // }
    // std::cout << "---> is of pixel type: "<<typeid(TPixel).name() <<std::endl;
    // std::cout << "---> is of image dim:  "<< VImageDimension <<std::endl;

    m_Helper = std::shared_ptr<captk::ROIConstructionItkHelperBase>(
        new captk::ROIConstructionItkHelper<TPixel, VImageDimension>(mask)
    );
}