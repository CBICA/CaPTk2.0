### Add upenn.cbica.* to the list of allowed naming schemes
list(APPEND MITK_PLUGIN_REGEX_LIST "^upenn_cbica_[a-zA-Z0-9_]+$")

# The order matters! Dependencies should go first

set(MITK_PLUGINS
  org.mitk.cbica.captk.phiestimator:OFF
  org.mitk.cbica.captk.interactivesegmentation:ON
  org.mitk.cbica.captk.ui:ON
  org.mitk.cbica.captk.application:ON
  org.mitk.cbica.captk.training:ON
  upenn.cbica.captk.brain.survival:ON
)
