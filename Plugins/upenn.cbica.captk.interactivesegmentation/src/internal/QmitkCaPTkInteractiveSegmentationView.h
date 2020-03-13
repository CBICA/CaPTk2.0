#ifndef QmitkCaPTkInteractiveSegmentationView_h
#define QmitkCaPTkInteractiveSegmentationView_h

#include <QmitkAbstractView.h>

#include <mitkILifecycleAwarePart.h>

#include "ui_QmitkCaPTkInteractiveSegmentationControls.h"

class CaPTkInteractiveSegmentation;

// berry
#include <berryIBerryPreferences.h>

class QmitkRenderWindow;

class QmitkCaPTkInteractiveSegmentationView : public QmitkAbstractView, 
                                              public mitk::ILifecycleAwarePart
{
  Q_OBJECT

public:
  static const std::string VIEW_ID;

  QmitkCaPTkInteractiveSegmentationView();
  virtual ~QmitkCaPTkInteractiveSegmentationView();

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

public slots:
  /** \brief This function is called with the async execution in the module finishes. 
   * \param ok whether the execution was successful
   * \param errorMessage if (!ok), what went wrong?
   * \param result the output segmentation, if ok == true
  */
  void OnModuleFinished(bool ok, std::string errorMessage, mitk::LabelSetImage::Pointer result);

  /** \brief This function is called when the module wants to update the progress */
  void OnModuleProgressUpdate(int progress);

protected slots:

  /** \brief CaPTk Interactive Segmentation Run Button clicked slot */
  void OnRunButtonPressed();

  /// \brief reaction to button "New Segmentation Session"
  void OnNewSegmentationSession();

  /// \brief reaction to the selection of a new patient (reference) image in the DataStorage combobox
  void OnReferenceSelectionChanged(const mitk::DataNode* node);

  /// \brief reaction to the selection of a new Segmentation (working) image in the DataStorage combobox
  void OnSegmentationSelectionChanged(const mitk::DataNode* node);

protected:

  // reimplemented from QmitkAbstractView
  void OnSelectionChanged(berry::IWorkbenchPart::Pointer part, const QList<mitk::DataNode::Pointer> &nodes) override;

  // reimplemented from QmitkAbstractView
  void OnPreferencesChanged(const berry::IBerryPreferences* prefs) override;

  // reimplemented from QmitkAbstractView
  void NodeAdded(const mitk::DataNode* node) override;

  // reimplemented from QmitkAbstractView
  void NodeRemoved(const mitk::DataNode* node) override;

  void OnEstablishLabelSetConnection();

  void OnLooseLabelSetConnection();

  void SetFocus();

  void UpdateControls();

  void ResetMouseCursor();

  void SetMouseCursor(const us::ModuleResource, int hotspotX, int hotspotY);

  /// \brief Checks if two images have the same size and geometry
  bool CheckForSameGeometry(const mitk::Image *image1, const mitk::Image *image2) const;

  QString GetLastFileOpenPath();

  void SetLastFileOpenPath(const QString &path);

  std::string FindNextAvailableSeedsName();

  /** \brief Used to give the appropriate name to the output segmentation. 
   * 
   * The first one is called "Segmentation". Subsequent ones "Segmentation-2" etc 
   */
  std::string FindNextAvailableSegmentationName();

  /** \brief Helper function to identify if a string is a number */
  bool IsNumber(const std::string &s);

  /// \brief the Qt parent of our GUI (NOT of this object)
  QWidget *m_Parent;

  /// \brief Qt GUI file
  Ui::QmitkCaPTkInteractiveSegmentationControls m_Controls;

  mitk::DataNode::Pointer m_ReferenceNode;
  mitk::DataNode::Pointer m_WorkingNode;

  mitk::NodePredicateAnd::Pointer m_ReferencePredicate;
  mitk::NodePredicateAnd::Pointer m_SegmentationPredicate;

  bool m_AutoSelectionEnabled;
  bool m_MouseCursorSet;

  // mitk::SegmentationInteractor::Pointer m_Interactor;

  /**
   * Reference to the service registration of the observer,
   * it is needed to unregister the observer on unload.
   */
  us::ServiceRegistration<mitk::InteractionEventObserver> m_ServiceRegistration;

  /** The algorithm */
  CaPTkInteractiveSegmentation* m_CaPTkInteractiveSegmentation;

  bool m_IsAlgorithmRunning;
};

#endif // QmitkCaPTkInteractiveSegmentationView_h
