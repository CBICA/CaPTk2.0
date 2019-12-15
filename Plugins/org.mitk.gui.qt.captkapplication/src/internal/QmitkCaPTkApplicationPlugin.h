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


#ifndef QMITKCAPTKAPPLICATIONPLUGIN_H_
#define QMITKCAPTKAPPLICATIONPLUGIN_H_

#include <berryAbstractUICTKPlugin.h>

#include <QString>

class QmitkCaPTkApplicationPlugin : public berry::AbstractUICTKPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org_mitk_gui_qt_captkapplication")
  Q_INTERFACES(ctkPluginActivator)

public:

  QmitkCaPTkApplicationPlugin();
  ~QmitkCaPTkApplicationPlugin() override;

  static QmitkCaPTkApplicationPlugin* GetDefault();

  ctkPluginContext* GetPluginContext() const;

  void start(ctkPluginContext*) override;

  QString GetQtHelpCollectionFile() const;

private:

  static QmitkCaPTkApplicationPlugin* inst;

  ctkPluginContext* context;
};

#endif /* QMITKCAPTKAPPLICATIONPLUGIN_H_ */
