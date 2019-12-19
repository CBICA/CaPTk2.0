# The order matters! Dependencies should go first

set(MITK_PLUGINS
  org.mitk.gui.qt.exampleplugin:OFF
  org.mitk.exampleplugin.eageractivation:ON
  org.mitk.gui.qt.captkinteractivesegmentation:ON
  org.mitk.cbica.captk.ui:ON
  org.mitk.gui.qt.captkapplication:ON
)
