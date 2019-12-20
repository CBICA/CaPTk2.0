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

#include "QmitkCaPTkDefaultPerspective.h"
#include "berryIViewLayout.h"

QmitkCaPTkDefaultPerspective::QmitkCaPTkDefaultPerspective()
{
}

void QmitkCaPTkDefaultPerspective::CreateInitialLayout(berry::IPageLayout::Pointer layout)
{
  QString editorArea = layout->GetEditorArea();

  // Show interactive segmentation by default. In the future change this to a 
  // non-closable "Start" menu of CaPTk, that also offers shortcuts
  // for different perspectives for Brain, Breast, Lung Cancer etc
  layout->AddView(
    "org.mitk.views.captkinteractivesegmentation", berry::IPageLayout::RIGHT, 
    0.6f, editorArea
  );

  // Data manager
  layout->AddView("org.mitk.views.datamanager", berry::IPageLayout::TOP, 0.3f, editorArea);
  berry::IViewLayout::Pointer lo = layout->GetViewLayout("org.mitk.views.datamanager");
  lo->SetCloseable(false);

  // Image navigator (slice sliders etc)
  layout->AddView("org.mitk.views.imagenavigator",
      berry::IPageLayout::RIGHT, 0.5f, "org.mitk.views.datamanager");

  // Bottom bar
  berry::IPlaceholderFolderLayout::Pointer bottomFolder = 
      layout->CreatePlaceholderFolder("bottom", berry::IPageLayout::BOTTOM, 
                                      0.7f, editorArea);
  bottomFolder->AddPlaceholder("org.blueberry.views.logview");
  bottomFolder->AddPlaceholder("org.mitk.views.modules");

  layout->AddPerspectiveShortcut("org.mitk.captk.perspectives.editor");
  layout->AddPerspectiveShortcut("org.mitk.captk.perspectives.visualization");
}
