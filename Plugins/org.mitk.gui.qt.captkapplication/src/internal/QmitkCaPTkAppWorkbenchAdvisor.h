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

#ifndef QMITKCAPTKAPPWORKBENCHADVISOR_H_
#define QMITKCAPTKAPPWORKBENCHADVISOR_H_

#include <berryQtWorkbenchAdvisor.h>

class QmitkCaPTkAppWorkbenchAdvisor: public berry::QtWorkbenchAdvisor
{
public:

  static const QString DEFAULT_PERSPECTIVE_ID; // = "org.mitk.captkapp.defaultperspective"

  void Initialize(berry::IWorkbenchConfigurer::Pointer configurer) override;

  berry::WorkbenchWindowAdvisor* CreateWorkbenchWindowAdvisor(
      berry::IWorkbenchWindowConfigurer::Pointer configurer) override;

  QString GetInitialWindowPerspectiveId() override;

private:
  // For CaPTk's about menu
  berry::IWorkbenchWindowConfigurer::Pointer m_WorkbenchWindowConfigurer;

};

#endif /*QMITKCAPTKAPPWORKBENCHADVISOR_H_*/
