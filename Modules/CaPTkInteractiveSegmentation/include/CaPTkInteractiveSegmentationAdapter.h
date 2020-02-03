#ifndef CaPTkInteractiveSegmentationAdapter_h
#define CaPTkInteractiveSegmentationAdapter_h

#include "GeodesicTrainingSegmentation.h"
#include "CaPTkInteractiveSegmentationQtPart.h"

/** \class CaPTkInteractiveSegmentationAdapter
 *  \brief Basically useful to override the progressUpdate function of the original algorithm
 */
template<unsigned long Dimensions>
class CaPTkInteractiveSegmentationAdapter : 
    public CaPTkInteractiveSegmentationQtPart,
    public GeodesicTrainingSegmentation::Coordinator<float, Dimensions>
{
public:
	explicit CaPTkInteractiveSegmentationAdapter(QObject* parent = nullptr) : 
					CaPTkInteractiveSegmentationQtPart(parent) {}

	virtual ~CaPTkInteractiveSegmentationAdapter() {}

protected:
	/** Progress update method overriden from GeodesicTrainingSegmentation */
	void progressUpdate(std::string message, int progress) override
	{
		std::cout << "[CaPTkInteractiveSegmentationAdapter::progressUpdate]\n";

		if (message == "GTS: Executing" && progress == 0)
		{
			emit ProgressUpdate(/*QString::fromStdString(message), */0);
		}
		else if (message == "GTS: AGD Operations" && progress == 0)
		{
			emit ProgressUpdate(/*QString::fromStdString(message), */5);
		}
		else if (message == "GTS: AGD Operations" && progress == 100)
		{
			emit ProgressUpdate(/*QString::fromStdString(message), */20);
		}
		else if (message == "GTS: Converting" && progress == 0)
		{
			emit ProgressUpdate(/*QString::fromStdString(message), */25);
		}
		else if (message == "GTS: Converting" && progress == 100)
		{
			emit ProgressUpdate(/*QString::fromStdString(message), */30);
		}
		else if (message == "GTS: SVM Operations" && progress == 0)
		{
			emit ProgressUpdate(/*QString::fromStdString(message), */35);
		}
		else if (message == "GTS: SVM Operations" && progress == 100)
		{
			emit ProgressUpdate(/*QString::fromStdString(message), */99);
		}
		else if (message == "GTS: Finished" && progress == 0)
		{
			emit ProgressUpdate(/*QString::fromStdString(message), */100);
		}
	}
};

#endif // ! CaPTkInteractiveSegmentationAdapter_h