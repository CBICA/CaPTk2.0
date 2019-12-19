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

#ifndef QcaptkAboutHandler_H
#define QcaptkAboutHandler_H

#include <berryAbstractHandler.h>

class QcaptkAboutHandler : public berry::AbstractHandler
{
  Q_OBJECT

public:

  Object::Pointer Execute(const berry::SmartPointer<const berry::ExecutionEvent>& event) override;
};

#endif // QcaptkAboutHandler_H
