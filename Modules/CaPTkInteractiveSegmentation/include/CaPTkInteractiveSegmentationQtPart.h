#ifndef CaPTkInteractiveSegmentationQtPart_h
#define CaPTkInteractiveSegmentationQtPart_h

#include <QObject>

/** \class CaPTkInteractiveSegmentationQtPart
 *  \brief Signals and slots functionality used by the CaPTkInteractiveSegmentationAdapter
 * 
 * This class exists because you can't mix templates and QObject 
 */
class CaPTkInteractiveSegmentationQtPart : public QObject
{
    Q_OBJECT

public:
    explicit CaPTkInteractiveSegmentationQtPart(QObject* parent = nullptr) : QObject(parent) {}
	virtual ~CaPTkInteractiveSegmentationQtPart() {}

signals:
    void ProgressUpdate(/*QString message, */int progress);
};

#endif // ! CaPTkInteractiveSegmentationQtPart_h