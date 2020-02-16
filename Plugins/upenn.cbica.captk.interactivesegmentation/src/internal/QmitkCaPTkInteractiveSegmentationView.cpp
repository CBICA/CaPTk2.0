#include "QmitkCaPTkInteractiveSegmentationView.h"

// blueberry
#include <berryConstants.h>
#include <berryIWorkbenchPage.h>

// mitk
#include "mitkApplicationCursor.h"
#include "mitkLabelSetImage.h"
#include "mitkStatusBar.h"
#include "mitkPlanePositionManager.h"
#include "mitkPluginActivator.h"
#include "mitkSegTool2D.h"

// Qmitk
#include "QmitkNewSegmentationDialog.h"
#include "QmitkRenderWindow.h"

// us
#include <usGetModuleContext.h>
#include <usModule.h>
#include <usModuleContext.h>
#include <usModuleResource.h>
#include <usModuleResourceStream.h>

// Qt
#include <QDateTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

// CaPTk
#include "CaPTkInteractiveSegmentation.h"

#include "tinyxml.h"

#include <itksys/SystemTools.hxx>

#include <regex>

const std::string QmitkCaPTkInteractiveSegmentationView::VIEW_ID = 
  "upenn.cbica.captk.views.interactivesegmentation";

QmitkCaPTkInteractiveSegmentationView::QmitkCaPTkInteractiveSegmentationView()
  : m_Parent(nullptr),
    m_ReferenceNode(nullptr),
    m_WorkingNode(nullptr),
    m_AutoSelectionEnabled(false),
    m_MouseCursorSet(false)
{
  m_CaPTkInteractiveSegmentation = 
      new CaPTkInteractiveSegmentation(GetDataStorage(), this);

  m_SegmentationPredicate = mitk::NodePredicateAnd::New();
  m_SegmentationPredicate->AddPredicate(
      mitk::TNodePredicateDataType<mitk::LabelSetImage>::New()
  );
  m_SegmentationPredicate->AddPredicate
      (mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object"))
  );
  m_SegmentationPredicate->AddPredicate
      (mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("captk.interactive.segmentation.output"))
  );

  mitk::TNodePredicateDataType<mitk::Image>::Pointer isImage = mitk::TNodePredicateDataType<mitk::Image>::New();
  mitk::NodePredicateProperty::Pointer isBinary =
    mitk::NodePredicateProperty::New("binary", mitk::BoolProperty::New(true));
  mitk::NodePredicateAnd::Pointer isMask = mitk::NodePredicateAnd::New(isBinary, isImage);

  mitk::NodePredicateDataType::Pointer isDwi = mitk::NodePredicateDataType::New("DiffusionImage");
  mitk::NodePredicateDataType::Pointer isDti = mitk::NodePredicateDataType::New("TensorImage");
  mitk::NodePredicateDataType::Pointer isOdf = mitk::NodePredicateDataType::New("OdfImage");
  auto isSegment = mitk::NodePredicateDataType::New("Segment");

  mitk::NodePredicateOr::Pointer validImages = mitk::NodePredicateOr::New();
  validImages->AddPredicate(mitk::NodePredicateAnd::New(isImage, mitk::NodePredicateNot::New(isSegment)));
  validImages->AddPredicate(isDwi);
  validImages->AddPredicate(isDti);
  validImages->AddPredicate(isOdf);

  m_ReferencePredicate = mitk::NodePredicateAnd::New();
  m_ReferencePredicate->AddPredicate(validImages);
  m_ReferencePredicate->AddPredicate(mitk::NodePredicateNot::New(m_SegmentationPredicate));
  m_ReferencePredicate->AddPredicate(mitk::NodePredicateNot::New(isMask));
  m_ReferencePredicate->AddPredicate(mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object")));
  m_ReferencePredicate->AddPredicate
      (mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("captk.interactive.segmentation.output"))
  );
}

QmitkCaPTkInteractiveSegmentationView::~QmitkCaPTkInteractiveSegmentationView()
{
  // Loose LabelSetConnections
  OnLooseLabelSetConnection();
}

void QmitkCaPTkInteractiveSegmentationView::CreateQtPartControl(QWidget *parent)
{
  // setup the basic GUI of this view
  m_Parent = parent;
  m_Controls.setupUi(parent);

  m_CaPTkInteractiveSegmentation->SetProgressBar(m_Controls.progressBar);

  // *------------------------
  // * DATA SELECTION WIDGETS
  // *------------------------

  m_Controls.m_cbReferenceNodeSelector->SetAutoSelectNewItems(true);
  m_Controls.m_cbReferenceNodeSelector->SetPredicate(m_ReferencePredicate);
  m_Controls.m_cbReferenceNodeSelector->SetDataStorage(this->GetDataStorage());

  m_Controls.m_cbWorkingNodeSelector->SetAutoSelectNewItems(true);
  m_Controls.m_cbWorkingNodeSelector->SetPredicate(m_SegmentationPredicate);
  m_Controls.m_cbWorkingNodeSelector->SetDataStorage(this->GetDataStorage());

  connect(m_Controls.m_cbReferenceNodeSelector,
          SIGNAL(OnSelectionChanged(const mitk::DataNode *)),
          this,
          SLOT(OnReferenceSelectionChanged(const mitk::DataNode *)));

  connect(m_Controls.m_cbWorkingNodeSelector,
          SIGNAL(OnSelectionChanged(const mitk::DataNode *)),
          this,
          SLOT(OnSegmentationSelectionChanged(const mitk::DataNode *)));

  // *------------------------*
  // * Connect signals/slots
  // *------------------------*

  connect(m_Controls.m_pbNewSegmentationSession, SIGNAL(clicked()), this, SLOT(OnNewSegmentationSession()));

  /* CaPTk Interactive Segmentation Run Button */
  connect(m_Controls.pushButtonRun, SIGNAL(clicked()), 
    this, SLOT(OnRunButtonPressed())
  );

  /* Make sure the GUI notices if appropriate data is already present on creation */
  this->OnReferenceSelectionChanged(m_Controls.m_cbReferenceNodeSelector->GetSelectedNode());
  this->OnSegmentationSelectionChanged(m_Controls.m_cbWorkingNodeSelector->GetSelectedNode());
}

void QmitkCaPTkInteractiveSegmentationView::Activated()
{
  // Not yet implemented
}

void QmitkCaPTkInteractiveSegmentationView::Deactivated()
{
  // Not yet implemented
}

void QmitkCaPTkInteractiveSegmentationView::Visible()
{
  // Not yet implemented
}

void QmitkCaPTkInteractiveSegmentationView::Hidden()
{
  // Not yet implemented
}

int QmitkCaPTkInteractiveSegmentationView::GetSizeFlags(bool width)
{
  if (!width)
  {
    return berry::Constants::MIN | berry::Constants::MAX | berry::Constants::FILL;
  }
  else
  {
    return 0;
  }
}

int QmitkCaPTkInteractiveSegmentationView::ComputePreferredSize(bool width,
                                                          int /*availableParallel*/,
                                                          int /*availablePerpendicular*/,
                                                          int preferredResult)
{
  if (width == false)
  {
    return 100;
  }
  else
  {
    return preferredResult;
  }
}

/************************************************************************/
/* protected slots                                                      */
/************************************************************************/

void QmitkCaPTkInteractiveSegmentationView::OnRunButtonPressed()
{
  // ---- Collect the seeds ----

  std::string seedsNodeName = "";
  mitk::LabelSetImage::Pointer seeds = nullptr;
  if (m_WorkingNode.IsNotNull())
  {
    seeds = dynamic_cast<mitk::LabelSetImage*>( m_WorkingNode->GetData() );
    seedsNodeName = m_WorkingNode->GetName();
  }

  // ---- Collect the images ----

  std::vector<mitk::Image::Pointer> images;
  
  // Predicate to find if node is mitk::Image
  auto predicateIsImage = 
      mitk::TNodePredicateDataType<mitk::Image>::New();
  
  // Predicate to find if node is mitk::LabelSetImage
  auto predicateIsLabelSetImage = 
      mitk::TNodePredicateDataType<mitk::LabelSetImage>::New();

  // Predicate property to find if node is a helper object
  auto predicatePropertyIsHelper =
      mitk::NodePredicateProperty::New("helper object");

  // The images we want are mitk::Image, but not mitk::LabelSetImage and not helper obj
  auto predicateFinal = mitk::NodePredicateAnd::New();
  predicateFinal->AddPredicate(predicateIsImage);
  predicateFinal->AddPredicate(mitk::NodePredicateNot::New(predicateIsLabelSetImage));
  predicateFinal->AddPredicate(mitk::NodePredicateNot::New(predicatePropertyIsHelper));

  // Get those images and add them to the vector
	mitk::DataStorage::SetOfObjects::ConstPointer all = 
      GetDataStorage()->GetSubset(predicateFinal);
	for (mitk::DataStorage::SetOfObjects::ConstIterator it = all->Begin(); 
       it != all->End(); ++it) 
  {
    if (it->Value().IsNotNull())
    {
      std::cout << "[QmitkCaPTkInteractiveSegmentationView::OnRunButtonPressed] "
                << "Image node name: " 
                << it->Value()->GetName()
                << "\n";
			images.push_back( dynamic_cast<mitk::Image*>(it->Value()->GetData()) );
		}
	}

  // ---- Call module ----
  m_CaPTkInteractiveSegmentation->Run(images, seeds);
}

void QmitkCaPTkInteractiveSegmentationView::OnNewSegmentationSession()
{
  m_Controls.progressBar->setValue(0);

  mitk::DataNode *referenceNode = m_Controls.m_cbReferenceNodeSelector->GetSelectedNode();

  if (!referenceNode)
  {
    QMessageBox::information(
      m_Parent, "New Segmentation Session", "Please load a patient image before starting some action.");
    return;
  }

  // Reset progress bar
  m_Controls.progressBar->setValue(0);

  mitk::Image* referenceImage = dynamic_cast<mitk::Image*>(referenceNode->GetData());
  assert(referenceImage);

  QString newName = this->FindNextAvailableSeedsName().c_str();

  this->WaitCursorOn();

  mitk::LabelSetImage::Pointer workingImage = mitk::LabelSetImage::New();
  try
  {
    workingImage->Initialize(referenceImage);
  }
  catch (mitk::Exception& e)
  {
    this->WaitCursorOff();
    MITK_ERROR << "Exception caught: " << e.GetDescription();
    QMessageBox::information(m_Parent, "New Segmentation Session", "Could not create a new segmentation session.\n");
    return;
  }

  // ---- Add default labels ----

  std::vector<QString>     labelNames; 
  std::vector<mitk::Color> colorNames; // Colors are RGB, float value for each of the 3
  
  // Can't push back the color value easily for some reason, but this works
  // Adds predetermined labels to the create seeds image
  labelNames.push_back("Label 1"); 
  {
    mitk::Color color; color.SetRed(1.00f); color.SetGreen(0.00f); color.SetBlue(0.00f); 
    colorNames.push_back(color);
  }
  labelNames.push_back("Label 2"); 
  {
    mitk::Color color; color.SetRed(1.00f); color.SetGreen(1.00f); color.SetBlue(0.00f); 
    colorNames.push_back(color);
  }
  labelNames.push_back("Label 3"); 
  {
    mitk::Color color; color.SetRed(0.00f); color.SetGreen(0.00f); color.SetBlue(1.00f); 
    colorNames.push_back(color);
  }
  labelNames.push_back("Label 4"); 
  {
    mitk::Color color; color.SetRed(0.68f); color.SetGreen(0.50f); color.SetBlue(0.65f); 
    colorNames.push_back(color);
  }
  labelNames.push_back("Label 5"); 
  {
  mitk::Color color; color.SetRed(0.33f); color.SetGreen(0.33f); color.SetBlue(0.33f);
  colorNames.push_back(color);
  }
  labelNames.push_back("Background tissue"); 
  {
  mitk::Color color; color.SetRed(0.03f); color.SetGreen(0.37f); color.SetBlue(0.00f);
  colorNames.push_back(color);
  }
  for (size_t i=0; i<labelNames.size(); i++)
  {
    workingImage->GetActiveLabelSet()->AddLabel(
      labelNames[i].toStdString(), colorNames[i]
    );
  }
  workingImage->GetActiveLabelSet()->SetAllLabelsLocked(false);
  workingImage->GetActiveLabelSet()->SetActiveLabel(1);

  // Continue
  this->WaitCursorOff();

  mitk::DataNode::Pointer workingNode = mitk::DataNode::New();
  workingNode->SetData(workingImage);
  workingNode->SetName(newName.toStdString());

  workingImage->GetExteriorLabel()->SetProperty("name.parent", mitk::StringProperty::New(referenceNode->GetName().c_str()));
  workingImage->GetExteriorLabel()->SetProperty("name.image", mitk::StringProperty::New(newName.toStdString().c_str()));

  if (!GetDataStorage()->Exists(workingNode))
  {
    GetDataStorage()->Add(workingNode);
  }
}

void QmitkCaPTkInteractiveSegmentationView::OnReferenceSelectionChanged(const mitk::DataNode* node)
{
  // m_ToolManager->ActivateTool(-1);

  m_ReferenceNode = const_cast<mitk::DataNode*>(node);
  // m_ToolManager->SetReferenceData(m_ReferenceNode);

  if (m_ReferenceNode.IsNotNull())
  {
    if (m_AutoSelectionEnabled)
    {
      // if an image is selected find a possible working / segmentation image
      mitk::DataStorage::SetOfObjects::ConstPointer derivations = this->GetDataStorage()->GetDerivations(m_ReferenceNode, m_SegmentationPredicate);
      if (derivations->Size() != 0)
      {
        // use the first segmentation child node
        m_WorkingNode = derivations->ElementAt(0);

        m_Controls.m_cbWorkingNodeSelector->blockSignals(true);
        m_Controls.m_cbWorkingNodeSelector->SetSelectedNode(m_WorkingNode);
        m_Controls.m_cbWorkingNodeSelector->blockSignals(false);
      }
      else if (derivations->size() == 0)
      {
        m_Controls.m_cbWorkingNodeSelector->setCurrentIndex(-1);
      }

      // hide all image and segmentation nodes to later show only the automatically selected ones
      mitk::DataStorage::SetOfObjects::ConstPointer patientNodes = GetDataStorage()->GetSubset(m_ReferencePredicate);
      for (mitk::DataStorage::SetOfObjects::const_iterator iter = patientNodes->begin(); iter != patientNodes->end(); ++iter)
      {
        (*iter)->SetVisibility(false);
      }

      mitk::DataStorage::SetOfObjects::ConstPointer segmentationNodes = GetDataStorage()->GetSubset(m_SegmentationPredicate);
      for (mitk::DataStorage::SetOfObjects::const_iterator iter = segmentationNodes->begin(); iter != segmentationNodes->end(); ++iter)
      {
        (*iter)->SetVisibility(false);
      }
    }
    m_ReferenceNode->SetVisibility(true);

    // check match of segmentation and reference image geometries
    if (m_WorkingNode.IsNotNull())
    {
      mitk::Image* workingImage = dynamic_cast<mitk::Image*>(m_WorkingNode->GetData());
      assert(workingImage);

      mitk::Image* referenceImage = dynamic_cast<mitk::Image*>(node->GetData());
      assert(referenceImage);

      if (!this->CheckForSameGeometry(referenceImage, workingImage))
      {
        return;
      }
      m_WorkingNode->SetVisibility(true);
    }
  }

  UpdateControls();
  if (m_WorkingNode.IsNotNull())
  {
    // m_Controls.m_LabelSetWidget->ResetAllTableWidgetItems();
    mitk::RenderingManager::GetInstance()->InitializeViews(m_WorkingNode->GetData()->GetTimeGeometry(), mitk::RenderingManager::REQUEST_UPDATE_ALL, true);
  }
}

void QmitkCaPTkInteractiveSegmentationView::OnSegmentationSelectionChanged(const mitk::DataNode* node)
{
  if (node != nullptr)
  {
    std::cout << "[QmitkCaPTkInteractiveSegmentationView::OnSegmentationSelectionChanged] "
              << "Selection changed to: "
              << node->GetName()
              << "\n";
  }

  if (m_WorkingNode.IsNotNull())
  {
    OnLooseLabelSetConnection();
  }

  m_WorkingNode = const_cast<mitk::DataNode*>(node);
  if (m_WorkingNode.IsNotNull())
  {
    // OnEstablishLabelSetConnection();

    if (m_AutoSelectionEnabled)
    {
      // if a segmentation is selected find a possible reference image
      mitk::DataStorage::SetOfObjects::ConstPointer sources = this->GetDataStorage()->GetSources(m_WorkingNode, m_ReferencePredicate);
      if (sources->Size() != 0)
      {
        m_ReferenceNode = sources->ElementAt(0);
        // m_ToolManager->SetReferenceData(m_ReferenceNode);

        m_Controls.m_cbReferenceNodeSelector->blockSignals(true);
        m_Controls.m_cbReferenceNodeSelector->SetSelectedNode(m_ReferenceNode);
        m_Controls.m_cbReferenceNodeSelector->blockSignals(false);
      }
      else if(sources->size() == 0)
      {
        m_Controls.m_cbReferenceNodeSelector->setCurrentIndex(-1);
      }

      // hide all image and segmentation nodes to later show only the automatically selected ones
      mitk::DataStorage::SetOfObjects::ConstPointer patientNodes = GetDataStorage()->GetSubset(m_ReferencePredicate);
      for (mitk::DataStorage::SetOfObjects::const_iterator iter = patientNodes->begin(); iter != patientNodes->end(); ++iter)
      {
        (*iter)->SetVisibility(false);
      }

      mitk::DataStorage::SetOfObjects::ConstPointer segmentationNodes = GetDataStorage()->GetSubset(m_SegmentationPredicate);
      for (mitk::DataStorage::SetOfObjects::const_iterator iter = segmentationNodes->begin(); iter != segmentationNodes->end(); ++iter)
      {
        (*iter)->SetVisibility(false);
      }
    }
    m_WorkingNode->SetVisibility(true);

    // check match of segmentation and reference image geometries
    if (m_ReferenceNode.IsNotNull())
    {
      mitk::Image* referenceImage = dynamic_cast<mitk::Image*>(m_ReferenceNode->GetData());
      assert(referenceImage);

      mitk::Image* workingImage = dynamic_cast<mitk::Image*>(m_WorkingNode->GetData());
      assert(workingImage);

      if (!this->CheckForSameGeometry(referenceImage, workingImage))
      {
        return;
      }
      m_ReferenceNode->SetVisibility(true);
    }
  }

  UpdateControls();
  if (m_WorkingNode.IsNotNull())
  {
    // m_Controls.m_LabelSetWidget->ResetAllTableWidgetItems();
    // mitk::RenderingManager::GetInstance()->InitializeViews(m_WorkingNode->GetData()->GetTimeGeometry(), mitk::RenderingManager::REQUEST_UPDATE_ALL, true);
  }
}

/************************************************************************/
/* protected                                                            */
/************************************************************************/
void QmitkCaPTkInteractiveSegmentationView::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList<mitk::DataNode::Pointer> &nodes)
{
  if (m_AutoSelectionEnabled)
  {
    // automatically set the reference node and the working node of the multi label plugin
    if (1 == nodes.size())
    {
      mitk::DataNode::Pointer selectedNode = nodes.at(0);
      if (selectedNode.IsNull())
      {
        return;
      }

      // check selected node
      mitk::LabelSetImage::Pointer labelSetImage = dynamic_cast<mitk::LabelSetImage*>(selectedNode->GetData());
      if (labelSetImage.IsNotNull())
      {
        // reset the image / reference node selector in case the current selected segmentation has no image parent
        m_Controls.m_cbReferenceNodeSelector->setCurrentIndex(-1);
        // selected a label set image (a segmentation ( working node)
        m_Controls.m_cbWorkingNodeSelector->SetSelectedNode(selectedNode);
        return;
      }

      mitk::Image::Pointer selectedImage = dynamic_cast<mitk::Image*>(selectedNode->GetData());
      if (selectedImage.IsNotNull())
      {
        // reset the segmentation / working node selector in case the current selected image has no segmentation child
        m_Controls.m_cbWorkingNodeSelector->setCurrentIndex(-1);
        // selected an image (a reference node)
        m_Controls.m_cbReferenceNodeSelector->SetSelectedNode(selectedNode);
        return;
      }
    }
  }
}

void QmitkCaPTkInteractiveSegmentationView::OnPreferencesChanged(const berry::IBerryPreferences* /*prefs*/)
{

}

void QmitkCaPTkInteractiveSegmentationView::NodeAdded(const mitk::DataNode *)
{

}

void QmitkCaPTkInteractiveSegmentationView::NodeRemoved(const mitk::DataNode *node)
{
  bool isHelperObject(false);
  node->GetBoolProperty("helper object", isHelperObject);
  if (isHelperObject)
  {
    return;
  }

  if (m_ReferenceNode.IsNotNull() && dynamic_cast<mitk::LabelSetImage *>(node->GetData()))
  {
    // remove all possible contour markers of the segmentation
    mitk::DataStorage::SetOfObjects::ConstPointer allContourMarkers = this->GetDataStorage()->GetDerivations(
      node, mitk::NodePredicateProperty::New("isContourMarker", mitk::BoolProperty::New(true)));

    ctkPluginContext *context = mitk::PluginActivator::getContext();
    ctkServiceReference ppmRef = context->getServiceReference<mitk::PlanePositionManagerService>();
    mitk::PlanePositionManagerService *service = context->getService<mitk::PlanePositionManagerService>(ppmRef);

    for (mitk::DataStorage::SetOfObjects::ConstIterator it = allContourMarkers->Begin(); it != allContourMarkers->End(); ++it)
    {
      std::string nodeName = node->GetName();
      unsigned int t = nodeName.find_last_of(" ");
      unsigned int id = atof(nodeName.substr(t + 1).c_str()) - 1;

      service->RemovePlanePosition(id);

      this->GetDataStorage()->Remove(it->Value());
    }

    context->ungetService(ppmRef);
    service = nullptr;
  }
}

void QmitkCaPTkInteractiveSegmentationView::OnEstablishLabelSetConnection()
{
  if (m_WorkingNode.IsNull())
  {
    return;
  }
  mitk::LabelSetImage *workingImage = dynamic_cast<mitk::LabelSetImage *>(m_WorkingNode->GetData());
  assert(workingImage);

  workingImage->BeforeChangeLayerEvent += mitk::MessageDelegate<QmitkCaPTkInteractiveSegmentationView>(
    this, &QmitkCaPTkInteractiveSegmentationView::OnLooseLabelSetConnection);
}

void QmitkCaPTkInteractiveSegmentationView::OnLooseLabelSetConnection()
{

}

void QmitkCaPTkInteractiveSegmentationView::SetFocus()
{

}

void QmitkCaPTkInteractiveSegmentationView::UpdateControls()
{
  // Hide views that are not useful
  m_Controls.label_PatientImage->setVisible(false);
  m_Controls.m_cbReferenceNodeSelector->setVisible(false);

  this->RequestRenderWindowUpdate(mitk::RenderingManager::REQUEST_UPDATE_ALL);
}

void QmitkCaPTkInteractiveSegmentationView::ResetMouseCursor()
{
  if (m_MouseCursorSet)
  {
    mitk::ApplicationCursor::GetInstance()->PopCursor();
    m_MouseCursorSet = false;
  }
}

void QmitkCaPTkInteractiveSegmentationView::SetMouseCursor(const us::ModuleResource resource, int hotspotX, int hotspotY)
{
  // Remove previously set mouse cursor
  if (m_MouseCursorSet)
    this->ResetMouseCursor();

  if (resource)
  {
    us::ModuleResourceStream cursor(resource, std::ios::binary);
    mitk::ApplicationCursor::GetInstance()->PushCursor(cursor, hotspotX, hotspotY);
    m_MouseCursorSet = true;
  }
}

bool QmitkCaPTkInteractiveSegmentationView::CheckForSameGeometry(const mitk::Image *image1, const mitk::Image *image2) const
{
  bool isSameGeometry(true);

  if (image1 && image2)
  {
    mitk::BaseGeometry::Pointer geo1 = image1->GetGeometry();
    mitk::BaseGeometry::Pointer geo2 = image2->GetGeometry();

    isSameGeometry = isSameGeometry && mitk::Equal(geo1->GetOrigin(), geo2->GetOrigin());
    isSameGeometry = isSameGeometry && mitk::Equal(geo1->GetExtent(0), geo2->GetExtent(0));
    isSameGeometry = isSameGeometry && mitk::Equal(geo1->GetExtent(1), geo2->GetExtent(1));
    isSameGeometry = isSameGeometry && mitk::Equal(geo1->GetExtent(2), geo2->GetExtent(2));
    isSameGeometry = isSameGeometry && mitk::Equal(geo1->GetSpacing(), geo2->GetSpacing());
    isSameGeometry = isSameGeometry && mitk::MatrixEqualElementWise(geo1->GetIndexToWorldTransform()->GetMatrix(),
                                                                    geo2->GetIndexToWorldTransform()->GetMatrix());

    return isSameGeometry;
  }
  else
  {
    return false;
  }
}

QString QmitkCaPTkInteractiveSegmentationView::GetLastFileOpenPath()
{
  return this->GetPreferences()->Get("LastFileOpenPath", "");
}

void QmitkCaPTkInteractiveSegmentationView::SetLastFileOpenPath(const QString &path)
{
  this->GetPreferences()->Put("LastFileOpenPath", path);
  this->GetPreferences()->Flush();
}

std::string QmitkCaPTkInteractiveSegmentationView::FindNextAvailableSeedsName()
{
    // Predicate to find if node is mitk::LabelSetImage
    auto predicateIsLabelSetImage =
        mitk::TNodePredicateDataType<mitk::LabelSetImage>::New();

    // Predicate property to find if node is a helper object
    auto predicatePropertyIsHelper =
        mitk::NodePredicateProperty::New("helper object");

    // The images we want are but mitk::LabelSetImage and not helper obj
    auto predicateFinal = mitk::NodePredicateAnd::New();
    predicateFinal->AddPredicate(predicateIsLabelSetImage);
    predicateFinal->AddPredicate(mitk::NodePredicateNot::New(predicatePropertyIsHelper));

    int lastFound = 0;

    // Get those images
    mitk::DataStorage::SetOfObjects::ConstPointer all =
        GetDataStorage()->GetSubset(predicateFinal);
    for (mitk::DataStorage::SetOfObjects::ConstIterator it = all->Begin();
         it != all->End(); ++it)
    {
        if (it->Value().IsNotNull())
        {
            std::string name = it->Value()->GetName();
            if (name.rfind("Seeds", 0) == 0) // Starts with
            {
                if (name.length() == std::string("Seeds").length())
                {
                    // Special case
                    if (lastFound < 1)
                    {
                        lastFound = 1;
                    }
                }
                else
                {
                    if (name.rfind("Seeds-", 0) == 0) // Starts with
                    {
                        std::string numStr = name.erase(0, std::string("Seeds-").length());

                        if (IsNumber(numStr))
                        {
                            int num = std::stoi(numStr);
                            if (lastFound < num)
                            {
                                lastFound = num;
                            }
                        }
                    }
                }
            }
        }
    }

    // Construct and return the correct string
    if (lastFound == 0)
    {
        return "Seeds";
    }
    else
    {
        return std::string("Seeds-") + std::to_string(lastFound + 1);
    }
}

bool QmitkCaPTkInteractiveSegmentationView::IsNumber(const std::string &s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}