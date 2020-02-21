#ifndef __QmitkCaPTkSplitLabelsToImages_H_
#define __QmitkCaPTkSplitLabelsToImages_H_

#include "mitkIContextMenuAction.h"

#include "upenn_cbica_captk_common_Export.h"

#include "vector"
#include "mitkDataNode.h"
#include "mitkImage.h"

class UPENN_CBICA_CAPTK_COMMON_EXPORTS QmitkCaPTkSplitLabelsToImages : 
        public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:

  QmitkCaPTkSplitLabelsToImages();
  virtual ~QmitkCaPTkSplitLabelsToImages();

  //interface methods
  void Run( const QList<mitk::DataNode::Pointer>& selectedNodes );
  void SetDataStorage(mitk::DataStorage* dataStorage);
  void SetSmoothed(bool smoothed);
  void SetDecimated(bool decimated);
  void SetFunctionality(berry::QtViewPart* functionality);

protected:

private:

  typedef QList<mitk::DataNode::Pointer> NodeList;

  mitk::DataStorage* m_DataStorage;

};

#endif // __QmitkCaPTkSplitLabelsToImages_H_
