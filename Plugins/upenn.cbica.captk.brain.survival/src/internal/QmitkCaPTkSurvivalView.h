#ifndef QmitkCaPTkSurvivalView_h
#define QmitkCaPTkSurvivalView_h

#include <QmitkAbstractView.h>

#include <mitkILifecycleAwarePart.h>

#include "ui_QmitkCaPTkSurvivalControls.h"
#include "CaPTkSurvival.h"

class CaPTkSurvival;

// berry
#include <berryIBerryPreferences.h>

class QmitkRenderWindow;

/**
 * \ingroup ToolManagerEtAl
 * \ingroup upenn_cbica_captk_brain_survival_internal
 */
class QmitkCaPTkSurvivalView : public QmitkAbstractView, public mitk::ILifecycleAwarePart
{
  Q_OBJECT

public:
  static const std::string VIEW_ID;

  QmitkCaPTkSurvivalView();
  virtual ~QmitkCaPTkSurvivalView();

  // GUI setup
  void CreateQtPartControl(QWidget *parent) override;

  // ILifecycleAwarePart interface
public:
  void Activated() override;
  void Deactivated() override;
  void Visible() override;
  void Hidden() override;

  virtual int GetSizeFlags(bool width);
  virtual int ComputePreferredSize(bool width,
                                   int /*availableParallel*/,
                                   int /*availablePerpendicular*/,
                                   int preferredResult);  
protected slots:

  void OnUsageComboBoxCurrentTextChanged(const QString& text);

  void OnModelSourceComboBoxCurrentTextChanged(const QString& text);

  void OnSubjectDirectoryButtonClicked();

  void OnCustomModelDirectoryButtonClicked();

  void OnOutputDirectoryButtonClicked();

  void OnModuleDone();

  /** \brief CaPTk Survival Plugin Run Button clicked slot */
  void OnRunButtonPressed();


protected:

  // reimplemented from QmitkAbstractView
  void OnSelectionChanged(berry::IWorkbenchPart::Pointer part, const QList<mitk::DataNode::Pointer> &nodes) override;

  // reimplemented from QmitkAbstractView
  void OnPreferencesChanged(const berry::IBerryPreferences* prefs) override;

  // reimplemented from QmitkAbstractView
  void NodeAdded(const mitk::DataNode* node) override;

  // reimplemented from QmitkAbstractView
  void NodeRemoved(const mitk::DataNode* node) override;

  void SetFocus() override;

  void UpdateControls();

  void InitializeListeners();

  QString GetLastFileOpenPath();

  void SetLastFileOpenPath(const QString &path);

  /// \brief the Qt parent of our GUI (NOT of this object)
  QWidget *m_Parent;

  /// \brief Qt GUI file
  Ui::QmitkCaPTkSurvivalControls m_Controls;

  captk::CaPTkSurvival* m_CaPTkSurvival;

};

#endif // QmitkCaPTkSurvivalView_h
