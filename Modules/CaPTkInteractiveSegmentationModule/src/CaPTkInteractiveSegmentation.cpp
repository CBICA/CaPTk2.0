#include "CaPTkInteractiveSegmentation.h"

#include <iostream>

void CaPTkInteractiveSegmentation::Run(std::vector<mitk::Image::Pointer> images, 
         mitk::LabelSetImage::Pointer labels)
{
    std::cout << images[0] << "\n";
    std::cout << labels << "\n";
    this->OnAlgorithmFinished();
}

void CaPTkInteractiveSegmentation::OnAlgorithmFinished()
{
    std::cout << "[CaPTkInteractiveSegmentation::OnAlgorithmFinished]\n";
}