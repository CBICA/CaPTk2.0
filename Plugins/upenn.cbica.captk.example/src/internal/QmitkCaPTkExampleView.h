#ifndef QmitkCaPTkExampleView_h
#define QmitkCaPTkExampleView_h

#include <QmitkAbstractView.h>

#include <mitkILifecycleAwarePart.h>
#include <mitkNodePredicateAnd.h>

#include "ui_QmitkCaPTkExampleControls.h"

// berry
#include <berryIBerryPreferences.h>

class QmitkRenderWindow;

class QmitkCaPTkExampleView : public QmitkAbstractView, 
                                              public mitk::ILifecycleAwarePart
{
  Q_OBJECT

public:
  static const std::string VIEW_ID;

  QmitkCaPTkExampleView();
  virtual ~QmitkCaPTkExampleView();

  typedef std::map<mitk::DataNode *, unsigned long> NodeTagMapType;

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

  /** \brief Run Button clicked slot */
  void OnRunButtonPressed();

  /// \brief reaction to the selection of a new image in the DataStorage combobox
  void OnImageSelectionChanged(const mitk::DataNode* node);

protected:

  // reimplemented from QmitkAbstractView
  void OnSelectionChanged(berry::IWorkbenchPart::Pointer part, const QList<mitk::DataNode::Pointer> &nodes) override;

  // reimplemented from QmitkAbstractView
  void OnPreferencesChanged(const berry::IBerryPreferences *prefs) override;

  // reimplemented from QmitkAbstractView
  void NodeAdded(const mitk::DataNode *node) override;

  // reimplemented from QmitkAbstractView
  void NodeRemoved(const mitk::DataNode *node) override;

  void SetFocus();

  void UpdateControls();

  void InitializeListeners();

  /// \brief the Qt parent of our GUI (NOT of this object)
  QWidget *m_Parent;

  /// \brief Qt GUI file
  Ui::QmitkCaPTkExampleControls m_Controls;

  mitk::DataNode::Pointer m_ImageNode;

  mitk::NodePredicateAnd::Pointer m_ImagePredicate;

};

#endif // QmitkCaPTkExampleView_h
