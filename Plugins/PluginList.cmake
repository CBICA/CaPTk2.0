### Add upenn.cbica.* to the list of allowed naming schemes
list(APPEND MITK_PLUGIN_REGEX_LIST "^upenn_cbica_[a-zA-Z0-9_]+$")

# The order matters! Dependencies should go first

set(MITK_PLUGINS
  upenn.cbica.captk.brain.phiestimator:OFF
  upenn.cbica.captk.interactivesegmentation:ON
  upenn.cbica.captk.ui:ON
  upenn.cbica.captk.application:ON
  upenn.cbica.captk.training:ON
  upenn.cbica.captk.brain.survival:ON
  upenn.cbica.captk.example:OFF
)
