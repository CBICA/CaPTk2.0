# The order matters! Dependencies should go first

set(MITK_PLUGINS
  org.mitk.gui.qt.PHIEstimatorPlugin:OFF
  org.mitk.cbica.captk.interactivesegmentation:ON
  org.mitk.cbica.captk.ui:ON
  org.mitk.cbica.captk.application:ON
  org.mitk.cbica.captk.training:ON
  upenn.cbica.captk.survival:ON
)
