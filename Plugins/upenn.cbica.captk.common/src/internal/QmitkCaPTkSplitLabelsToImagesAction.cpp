#include "QmitkCaPTkSplitLabelsToImagesAction.h"

#include "mitkRenderingManager.h"
#include "mitkLabelSetImage.h"
#include "mitkToolManagerProvider.h"

//needed for qApp
#include <qcoreapplication.h>

QmitkCaPTkSplitLabelsToImagesAction::QmitkCaPTkSplitLabelsToImagesAction()
{
}

QmitkCaPTkSplitLabelsToImagesAction::~QmitkCaPTkSplitLabelsToImagesAction()
{
}

void QmitkCaPTkSplitLabelsToImagesAction::Run( const QList<mitk::DataNode::Pointer> &selectedNodes )
{
  // Iterate through each node
  for ( mitk::DataNode::Pointer maskNode : selectedNodes )
  {
    if (!maskNode)
    {
      MITK_INFO << "   a nullptr node was selected";
      continue;
    }

    // The input node
    mitk::LabelSetImage::Pointer mask = dynamic_cast<mitk::LabelSetImage*>(
      maskNode->GetData() 
    );

    if (mask.IsNull()) continue;
    std::string name = maskNode->GetName();


    mitk::LabelSet::Pointer labelSet = mask->GetActiveLabelSet();
    mitk::LabelSet::LabelContainerConstIteratorType it;

    // Find all the labels in the mask (-> their integer value)
    std::vector< mitk::Label::PixelType > labels;
    for (it = labelSet->IteratorConstBegin();
         it != labelSet->IteratorConstEnd();
         ++it)
    {
      auto label = it->second->GetValue();
      if (label != 0) { labels.push_back( label ); }
    }

    // Iterate through the labels again. 
    // Each time clone the image, and delete all
    // labels, except the one of the iteration 
    for (it = labelSet->IteratorConstBegin();
         it != labelSet->IteratorConstEnd();
         ++it)
    {
      auto currentLabel = it->second->GetValue();
      if (currentLabel == 0) { continue; }

      mitk::LabelSetImage::Pointer labelMask = mask->Clone();

      for (auto& label : labels)
      {
        if (label != currentLabel)
        {
          labelMask->EraseLabel(label);
          labelMask->GetActiveLabelSet()->RemoveLabel(label);
        }
      }

      // The output node for this label
      mitk::DataNode::Pointer outNode = mitk::DataNode::New();

      outNode->SetData(labelMask);
      outNode->SetName(name + "-" + it->second->GetName());
      m_DataStorage->Add(outNode, maskNode);
    }
  }
}

void QmitkCaPTkSplitLabelsToImagesAction::SetSmoothed(bool /*smoothed*/)
{
 //not needed
}

void QmitkCaPTkSplitLabelsToImagesAction::SetDecimated(bool /*decimated*/)
{
  //not needed
}

void QmitkCaPTkSplitLabelsToImagesAction::SetDataStorage(mitk::DataStorage* dataStorage)
{
  m_DataStorage = dataStorage;
}

void QmitkCaPTkSplitLabelsToImagesAction::SetFunctionality(berry::QtViewPart* /*functionality*/)
{
  //not needed
}
