# The order matters! Dependencies should go first

set(MITK_PLUGINS
  org.mitk.cbica.captk.phiestimator:OFF
  org.mitk.cbica.captk.interactivesegmentation:ON
  org.mitk.cbica.captk.ui:ON
  org.mitk.cbica.captk.application:ON
  org.mitk.cbica.captk.training:ON
)
