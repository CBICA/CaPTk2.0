#include "QmitkCaPTkExampleView.h"

// blueberry
#include <berryConstants.h>
#include <berryIWorkbenchPage.h>

// mitk
#include "mitkApplicationCursor.h"
#include "mitkStatusBar.h"
#include "mitkPlanePositionManager.h"
#include "mitkPluginActivator.h"
#include <mitkImageAccessByItk.h>
#include <mitkNodePredicateAnd.h>
#include <mitkNodePredicateOr.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>
#include <mitkNodePredicateDataType.h>

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
#include "CaPTkExampleAlgorithm.h"

#include "tinyxml.h"

#include <itksys/SystemTools.hxx>

#include <regex>

const std::string QmitkCaPTkExampleView::VIEW_ID =
    "upenn.cbica.captk.views.example";

QmitkCaPTkExampleView::QmitkCaPTkExampleView() : 
    m_Parent(nullptr),
    m_ImageNode(nullptr),
    m_ImagePredicate(mitk::NodePredicateAnd::New())
{
  // ---- Setup the data combo box predicate ----
  // The predicate dictates which type of images are allowed to be shown
  // in the combo box. In general, we want every type of image,
  // and not helper objects

  // We construct the final predicate for the combo box. 
  // (m_ImagePredicate is an AND predicate. Right now we give it only one argument that HAS to be true
  // for the image to show up in the combo box, but you can give it many)
  // We tell it to show images that are not helper objects.
  m_ImagePredicate->AddPredicate(mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object")));
}

QmitkCaPTkExampleView::~QmitkCaPTkExampleView()
{

}

void QmitkCaPTkExampleView::CreateQtPartControl(QWidget *parent)
{
  // setup the basic GUI of this view
  m_Parent = parent;
  m_Controls.setupUi(parent);

  m_Controls.lineEdit_Maximum->setValidator(new QIntValidator(0, 100, this));

  // *------------------------
  // * DATA SELECTION WIDGETS
  // *------------------------

  m_Controls.m_cbNodeSelector->SetAutoSelectNewItems(true);
  m_Controls.m_cbNodeSelector->SetPredicate(m_ImagePredicate);
  m_Controls.m_cbNodeSelector->SetDataStorage(this->GetDataStorage());

  connect(m_Controls.m_cbNodeSelector,
          SIGNAL(OnSelectionChanged(const mitk::DataNode *)),
          this,
          SLOT(OnImageSelectionChanged(const mitk::DataNode *)));

  // *------------------------*
  // * Connect signals/slots
  // *------------------------*

  /* Run Button */
  connect(m_Controls.pushButtonRun, SIGNAL(clicked()), 
    this, SLOT(OnRunButtonPressed())
  );

  /* Make sure the GUI notices if appropriate data is already present on creation */
  this->OnImageSelectionChanged(m_Controls.m_cbNodeSelector->GetSelectedNode());
}

void QmitkCaPTkExampleView::Activated()
{
  // Not yet implemented
}

void QmitkCaPTkExampleView::Deactivated()
{
  // Not yet implemented
}

void QmitkCaPTkExampleView::Visible()
{
  // Not yet implemented
}

void QmitkCaPTkExampleView::Hidden()
{
  // Not yet implemented
}

int QmitkCaPTkExampleView::GetSizeFlags(bool width)
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

int QmitkCaPTkExampleView::ComputePreferredSize(bool width,
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

void QmitkCaPTkExampleView::OnRunButtonPressed()
{
  // ---- Check if no image is selected ----
  if (m_ImageNode.IsNull())
  {
    QMessageBox msgError;
    msgError.setText(
      "Please load an image."
    );
    msgError.setIcon(QMessageBox::Critical);
    msgError.setWindowTitle("No input");
    msgError.exec();
    return;
  }

  // ---- Input ----
  int maximum = m_Controls.lineEdit_Maximum->text().toInt();
  mitk::Image::Pointer input = dynamic_cast<mitk::Image*>( m_ImageNode->GetData() );
  auto inputName = m_ImageNode->GetName();

  // ---- Call module ----
  mitk::Image::Pointer output = mitk::Image::New();
  AccessByItk_n(input, captk::ExampleAlgorithm, (maximum, output));

  // ---- Load it ----
  mitk::DataNode::Pointer node = mitk::DataNode::New();
  node->SetData(output);
  node->SetName(inputName + std::string("-negative"));
  this->GetDataStorage()->Add(node, m_ImageNode); // 2nd parameter is parent (and is optional)
}

void QmitkCaPTkExampleView::OnImageSelectionChanged(const mitk::DataNode* node)
{
  m_ImageNode = const_cast<mitk::DataNode*>(node);

  if (m_ImageNode.IsNull())
  {
    return;
  }

  m_ImageNode->SetVisibility(true);

  UpdateControls();
}

/************************************************************************/
/* protected                                                            */
/************************************************************************/
void QmitkCaPTkExampleView::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList<mitk::DataNode::Pointer>& /*nodes*/)
{

}

void QmitkCaPTkExampleView::OnPreferencesChanged(const berry::IBerryPreferences* /*prefs*/)
{

}

void QmitkCaPTkExampleView::NodeAdded(const mitk::DataNode* /*node*/)
{

}

void QmitkCaPTkExampleView::NodeRemoved(const mitk::DataNode* /*node*/)
{

}

void QmitkCaPTkExampleView::SetFocus()
{

}

void QmitkCaPTkExampleView::UpdateControls()
{
  // Here you can hide views that are not useful

  // this->RequestRenderWindowUpdate(mitk::RenderingManager::REQUEST_UPDATE_ALL);
}

void QmitkCaPTkExampleView::InitializeListeners()
{

}