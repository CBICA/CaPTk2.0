#ifndef upenn_cbica_captk_interactivesegmentation_Activator_h
#define upenn_cbica_captk_interactivesegmentation_Activator_h

#include <ctkPluginActivator.h>

namespace mitk
{
  class PluginActivator : public QObject, public ctkPluginActivator
  {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "upenn_cbica_captk_interactivesegmentation")
    Q_INTERFACES(ctkPluginActivator)

  public:
    void start(ctkPluginContext *context);
    void stop(ctkPluginContext *context);

    static ctkPluginContext* getContext();

  private:
    static ctkPluginContext* m_Context;
  };
}

#endif // ! upenn_cbica_captk_interactivesegmentation_Activator_h
