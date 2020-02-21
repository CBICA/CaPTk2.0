/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/
#include "QmitkCaPTkSplitLabelsToImages.h"

#include "mitkRenderingManager.h"
#include "mitkLabelSetImage.h"
#include "mitkToolManagerProvider.h"

//needed for qApp
#include <qcoreapplication.h>

QmitkCaPTkSplitLabelsToImages::QmitkCaPTkSplitLabelsToImages()
{
}

QmitkCaPTkSplitLabelsToImages::~QmitkCaPTkSplitLabelsToImages()
{
}

void QmitkCaPTkSplitLabelsToImages::Run( const QList<mitk::DataNode::Pointer> &selectedNodes )
{
  // mitk::ToolManager::Pointer toolManager = mitk::ToolManagerProvider::GetInstance()->GetToolManager();
  // assert(toolManager);

  // mitk::DataNode* workingNode = toolManager->GetWorkingData(0);
  // if (!workingNode)
  // {
  //   MITK_INFO << "There is no available segmentation. Please load or create one before using this tool.";
  //   return;
  // }

  // mitk::LabelSetImage* workingImage = dynamic_cast<mitk::LabelSetImage*>( workingNode->GetData() );
  // assert(workingImage);

  for ( mitk::DataNode::Pointer maskNode : selectedNodes )
  {
    if (!maskNode)
    {
      MITK_INFO << "   a nullptr node was selected";
      continue;
    }

    mitk::LabelSetImage::Pointer mask = dynamic_cast<mitk::LabelSetImage*>(maskNode->GetData() );
    if (mask.IsNull()) continue;
    std::string name = maskNode->GetName();

    mitk::DataNode::Pointer node = mitk::DataNode::New();
    node->SetData(mask->Clone());
    m_DataStorage->Add(node);

    // mitk::Color color;
    // mitk::ColorProperty::Pointer colorProp;
    // maskNode->GetProperty(colorProp,"color");
    // if (colorProp.IsNull()) continue;
    // color = colorProp->GetValue();
    // workingImage->GetLabelSet()->AddLabel(name,color);
    // //workingImage->AddLabelEvent.Send();

    // try
    // {
    //   workingImage->MaskStamp( mask, false );
    // }
    // catch ( mitk::Exception& e )
    // {
    //   MITK_ERROR << "Exception caught: " << e.GetDescription();
    //   return;
    // }

    // maskNode->SetVisibility(false);
    // mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  }
}

void QmitkCaPTkSplitLabelsToImages::SetSmoothed(bool /*smoothed*/)
{
 //not needed
}

void QmitkCaPTkSplitLabelsToImages::SetDecimated(bool /*decimated*/)
{
  //not needed
}

void QmitkCaPTkSplitLabelsToImages::SetDataStorage(mitk::DataStorage* dataStorage)
{
  m_DataStorage = dataStorage;
}

void QmitkCaPTkSplitLabelsToImages::SetFunctionality(berry::QtViewPart* /*functionality*/)
{
  //not needed
}
