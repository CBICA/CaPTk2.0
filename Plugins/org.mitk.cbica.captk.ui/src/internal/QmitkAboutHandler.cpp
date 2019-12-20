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

#include "QmitkAboutHandler.h"

#include <QmitkAboutDialog.h>

#include <QMessageBox>

berry::Object::Pointer QmitkAboutHandler::Execute(const berry::SmartPointer<const berry::ExecutionEvent>& /*event*/)
{
  // QMessageBox mb("CaPTk",
  //                QString("Cancer Imaging Phenomics Toolkit\n\n")
  //                   + QString("by CBICA (Center for Biomedical Image Computing & Analytics)\n\n")
  //                   + QString("University of Pennsylvania"),
  //               QMessageBox::NoIcon,
  //               QMessageBox::Yes | QMessageBox::Default,
  //               QMessageBox::NoButton,
  //               QMessageBox::NoButton);
  // QPixmap upennIcon(":/org.mitk.gui.qt.ext/upenn.png");
  // mb.setIconPixmap(upennIcon);

  // Is this function useless?

  return berry::Object::Pointer(); // for some reason

  // auto   aboutDialog = new QmitkAboutDialog(QApplication::activeWindow(), nullptr);
  // aboutDialog->open();
  // return berry::Object::Pointer();
}
