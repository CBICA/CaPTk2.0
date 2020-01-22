# The order matters! Dependencies should go first

set(MITK_PLUGINS
  org.mitk.gui.qt.exampleplugin:OFF
  org.mitk.exampleplugin.eageractivation:ON
  org.mitk.cbica.captk.interactivesegmentation:ON
  org.mitk.cbica.captk.ui:ON
  org.mitk.cbica.captk.application:ON
  org.mitk.cbica.captk.trainingplugin:ON
)
