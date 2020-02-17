#ifndef QmitkCaPTkTrainingView_h
#define QmitkCaPTkTrainingView_h

#include <QmitkAbstractView.h>

#include <mitkILifecycleAwarePart.h>

#include "ui_QmitkCaPTkTrainingControls.h"

class CaPTkTraining;

// berry
#include <berryIBerryPreferences.h>

class QmitkRenderWindow;

/**
 * \ingroup ToolManagerEtAl
 * \ingroup upenn_cbica_captk_training_internal
 */
class QmitkCaPTkTrainingView : public QmitkAbstractView, public mitk::ILifecycleAwarePart
{
  Q_OBJECT

public:
  static const std::string VIEW_ID;

  QmitkCaPTkTrainingView();
  virtual ~QmitkCaPTkTrainingView();

  // GUI setup
  void CreateQtPartControl(QWidget *parent);

  // ILifecycleAwarePart interface
public:
  void Activated();
  void Deactivated();
  void Visible();
  void Hidden();

  virtual int GetSizeFlags(bool width);
  virtual int ComputePreferredSize(bool width,
                                   int /*availableParallel*/,
                                   int /*availablePerpendicular*/,
                                   int preferredResult);

protected slots:

  void OnKernelComboBoxCurrentTextChanged(const QString& text);

  void OnConfigurationComboBoxCurrentTextChanged(const QString& text);

  void OnFeaturesCsvButtonClicked();

  void OnResponsesCsvButtonClicked();

  void OnModelDirectoryButtonClicked();

  void OnOutputDirectoryButtonClicked();

  /** \brief CaPTk Training Plugin Run Button clicked slot */
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

  void SetFocus();

  void UpdateControls();

  void InitializeListeners();

  QString GetLastFileOpenPath();

  void SetLastFileOpenPath(const QString &path);

  /// \brief the Qt parent of our GUI (NOT of this object)
  QWidget *m_Parent;

  /// \brief Qt GUI file
  Ui::QmitkCaPTkTrainingControls m_Controls;

  CaPTkTraining* m_CaPTkTraining;
};

#endif // QmitkCaPTkTrainingView_h
