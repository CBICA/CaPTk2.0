#include <mitkBaseApplication.h>

#include <QStringList>
#include <QVariant>

#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char **argv)
{
  mitk::BaseApplication app(argc, argv);

  app.setApplicationName("CaPTkInciSe");
  app.setOrganizationName("CBICA");

  // Preload the org.mitk.gui.qt.common plug-in (and hence also Qmitk) to speed
  // up a clean-cache start. This also works around bugs in older gcc and glibc implementations,
  // which have difficulties with multiple dynamic opening and closing of shared libraries with
  // many global static initializers. It also helps if dependent libraries have weird static
  // initialization methods and/or missing de-initialization code.
  QStringList preloadLibs;
  preloadLibs << "liborg_mitk_gui_qt_common";
  // preloadLibs << "liborg_mitk_gui_qt_ext";
  app.setPreloadLibraries(preloadLibs);

  app.setProperty(mitk::BaseApplication::PROP_PRODUCT, "org.mitk.cbica.captk.application.incise");
  // app.setProperty(mitk::BaseApplication::PROP_PRODUCT, "org.mitk.gui.qt.extapplication.workbench");
  // app.setProperty(mitk::BaseApplication::PROP_APPLICATION, "org.mitk.qt.coreapplication"); // Simple

  // Run the workbench
  return app.run();
}
