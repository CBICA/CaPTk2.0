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


#ifndef QMITKCAPTKEDITORPERSPECTIVE_H_
#define QMITKCAPTKEDITORPERSPECTIVE_H_

#include <berryIPerspectiveFactory.h>

class QmitkCaPTkEditorPerspective : public QObject, public berry::IPerspectiveFactory
{
  Q_OBJECT
  Q_INTERFACES(berry::IPerspectiveFactory)

public:

  QmitkCaPTkEditorPerspective() {}
  ~QmitkCaPTkEditorPerspective() override {}

  void CreateInitialLayout(berry::IPageLayout::Pointer /*layout*/) override;
};

#endif /* QMITKCAPTKEDITORPERSPECTIVE_H_ */
