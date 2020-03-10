namespace captk
{
namespace itk
{
using namespace itk;

template <typename TInputImage, typename TOutputImage>
AdaptiveGeodesicDistanceFilter<TInputImage, TOutputImage>::
AdaptiveGeodesicDistanceFilter()
{
    // constructor
}

template <typename TInputImage, typename TOutputImage>
void 
AdaptiveGeodesicDistanceFilter<TInputImage, TOutputImage>::
GenerateData()
{
    // Instructions:
    // https://itk.org/Doxygen/html/Examples_2Filtering_2CompositeFilterExample_8cxx-example.html
    // https://itk.org/ITKSoftwareGuide/html/Book1/ITKSoftwareGuide-Book1ch8.html
    
    /*---- Template dimensionality checks ----*/
    
    if (TInputImage::ImageDimension != 2 && TInputImage::ImageDimension != 3)
    {
        throw itk::ExceptionObject(__FILE__, __LINE__, 
                "Only 2D and 3D input images are supported.");
    }
    
    if (TOutputImage::ImageDimension != 2 && TOutputImage::ImageDimension != 3)
    {
        throw itk::ExceptionObject(__FILE__, __LINE__, 
                "Only 2D and 3D output images are supported.");
    }

    if (TInputImage::ImageDimension != TOutputImage::ImageDimension)
    {
        throw itk::ExceptionObject(__FILE__, __LINE__, 
                "Input and output images should have the same dimensionality.");     
    }

    /*---- Check that label of interest is set ----*/

    if (m_LabelOfInterest == nullptr)
    {
        throw itk::ExceptionObject(__FILE__, __LINE__, 
                "Label of interest is not set.");
    }

    /*---- Create mask if necessary ----*/

    // TODO: Create mask if necessary (either by duplicating the input image, 
    // or by copying the info from the input image and filling everything with 1)

    /*---- Check that input image, scribbles, 
           and mask have the same information ----*/

    // TODO

    // TODO: the rest...
    // stuff = this->InternalProcessing(...);
    // this->GraftOutput(m_RescaleFilter->GetOutput());
}

template <typename TInputImage, typename TOutputImage>
typename TOutputImageType::Pointer
AdaptiveGeodesicDistanceFilter<TInputImage, TOutputImage>::
InternalProcessing(
    const TInputImage* inputImage,
    const TInputImage* mask,
    const LabelsImageType* labels,
    const int labelOfInterest,
    bool limitAt255)
{
    // TODO
}

}
}