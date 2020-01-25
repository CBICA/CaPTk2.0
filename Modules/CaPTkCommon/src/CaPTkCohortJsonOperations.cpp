#include "CaPTkCohortJsonOperations.h"

#include "CaPTkCohort.h"
#include "CaPTkCohortSubject.h"
#include "CaPTkCohortStudy.h"
#include "CaPTkCohortSeries.h"
#include "CaPTkCohortImage.h"

#include "mitkExceptionMacro.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>

#include <utility>

QSharedPointer<QJsonDocument> 
captk::CohortJsonMergeObjects(QList<QSharedPointer<QJsonDocument>> jsons)
{
	if (jsons.size() == 0)
	{
		return QSharedPointer<QJsonDocument>(new QJsonDocument());
	}

	// Start with the first and merge the rest to it
	QSharedPointer<QJsonDocument> mergedJson = jsons[0];
	jsons.pop_front(); // Remove the first entry

	for(QSharedPointer<QJsonDocument> json_p : jsons)
	{
		// TODO: Actual merging
		std::cout << "\n!MERGING JSON DOCUMENTS NOT IMPLEMENTED YET!\n\n";
	}

	return mergedJson;
}

QSharedPointer<QJsonDocument> 
captk::CohortJsonFromDirectoryStructure(QString& directory)
{
	QJsonObject root;

	// ---- Cohort name ----

	if (!captk::internal::IsDir(directory))
	{
		mitkThrow() << "Directory doesn't exist";
		return QSharedPointer<QJsonDocument>(new QJsonDocument());
	}
	else
	{
		root["cohort_name"] = captk::internal::GetFileNameFromPath(directory);
	}

	// ---- Subjects ----

	auto subjects = QJsonArray();

	// For iterating the subdirectories of the cohort.
	// Each directory is a subject
	auto subjPaths = captk::internal::GetSubdirectories(directory);

	for (auto& subjPath : subjPaths)
	{
		if (!captk::internal::IsDir(subjPath)) { continue; }
		auto subj = captk::internal::GetFileNameFromPath(subjPath);
		qDebug() << subj;
		
		// ---- Create subject ----
		QJsonObject subject;
		subject["name"] = subj;
		auto studies = QJsonArray();

		// ---- Add all the images ----

		auto studiesPaths = captk::internal::GetSubdirectories(subjPath);

		for (auto& studyPath : studiesPaths)
		{
			if (!captk::internal::IsDir(studyPath)) { continue; }
			auto study = captk::internal::GetFileNameFromPath(studyPath);
			qDebug() << "\t" << study;

			QJsonObject studyjson;
			studyjson["name"] = study;
			auto series_of_study = QJsonArray();

			auto modalitiesPaths = captk::internal::GetSubdirectories(studyPath);
			for (auto& modalityPath : modalitiesPaths)
			{
				if (!captk::internal::IsDir(modalityPath)) { continue; }
				auto modality = captk::internal::GetFileNameFromPath(modalityPath);
				qDebug() << "\t" << "\t"<< modality;

				auto serDescPaths = captk::internal::GetSubdirectories(modalityPath);
				for (auto& serDescPath : serDescPaths)
				{
					if (!captk::internal::IsDir(serDescPath)) { continue; }
					auto serDesc = captk::internal::GetFileNameFromPath(serDescPath);
					qDebug() << "\t" << "\t" << "\t" << serDesc;

					auto series = QJsonObject();
					series["modality"] = modality;
					if (modality != "seg")
					{
						series["series_description"] = serDesc;
					}
					else 
					{
						series["segment_label"] = serDesc;
					}

					auto images = QJsonArray();

					auto filesPaths = captk::internal::GetContainedFiles(serDescPath);
					
					for (auto& filePath : filesPaths)
					{
						if (captk::internal::IsDir(filePath)) { continue; /*Discard dirs*/}
						if (filePath.endsWith("json")) { continue; }
						if (filePath.endsWith("csv")) { continue; }

						auto fileName = captk::internal::GetFileNameFromPath(filePath);
						qDebug() << "\t" << "\t" << "\t" << "\t" << fileName;

						// Add image
						auto image = QJsonObject();
						image["path"] = filePath;
						
						// Check if an "image_info" file exists for the image that will be added
						// That file should be in the same directory and have the same basename, but
						// end in .json
						// i.e.: patient0_flair.dcm -> patient0_flair.json
						QFileInfo fi (filePath);
						auto dirOfFile =  fi.absoluteDir().absolutePath();
						auto baseName = fi.completeBaseName();
						// Remove .nii in case of .nii.gz
						if (baseName.endsWith(".nii")) { baseName = QFileInfo(baseName).completeBaseName(); }
						if (filesPaths.contains(dirOfFile + QDir::separator() + baseName + ".json"))
						{
							image["image_info"] = dirOfFile 
								+ QDir::separator()
								+ baseName 
								+ ".json";
						}
						
						images.push_back(image);
					}

					if (images.size() > 0) {
						series["images"] = images;
						series_of_study.push_back(series);
					}
				}
			}

			if (series_of_study.size() > 0)
			{
				studyjson["series"] = series_of_study;
				studies.push_back(studyjson);
			}
		}


		// ---- Add the subject to the list ----
		if (studies.size() > 0)
		{
			subject["studies"] = studies;
			subjects.push_back(subject);
		}
	}
	
	root["subjects"] = subjects;

	return QSharedPointer<QJsonDocument>(new QJsonDocument(root));
}

captk::Cohort* 
captk::CohortJsonLoad(
	QSharedPointer<QJsonDocument> json,
	QObject* parent)
{
	captk::Cohort* cohort = new captk::Cohort(parent);
	
	QJsonObject root = json->object();

	if (root.contains("cohort_name"))
	{
		cohort->SetName(root["cohort_name"].toString());
	}

	if (!root.contains("subjects"))
	{
		return cohort; // No subjects found
	}

	QList<captk::CohortSubject*> subjects;

	// Iterate over the subjects of the cohort
	for (QJsonValueRef subjRef : root["subjects"].toArray())
	{
		auto subjObj = subjRef.toObject();

		captk::CohortSubject* subject = new captk::CohortSubject(cohort);

		if (subjObj.contains("name"))
		{
			subject->SetName(subjObj["name"].toString());
		}
		if (!subjObj.contains("studies"))
		{
			delete subject;
			continue;
		}

		QList<captk::CohortStudy*> studies;

		// Iterate over the studies of the subject
		for (QJsonValueRef studyRef : subjObj["studies"].toArray())
		{
			auto studyObj = studyRef.toObject();

			captk::CohortStudy* study = new captk::CohortStudy(subject);

			if (studyObj.contains("name"))
			{
				study->SetName(studyObj["name"].toString());
			}
			if (!studyObj.contains("series"))
			{
				delete study;
				continue;
			}

			QList<captk::CohortSeries*> series_of_study;

			// Iterate over the collection of series of the study
			for (QJsonValueRef seriesRef : studyObj["series"].toArray())
			{
				auto seriesObj = seriesRef.toObject();

				captk::CohortSeries* series = new captk::CohortSeries(study);

				if (seriesObj.contains("modality"))
				{
					auto modality = seriesObj["modality"].toString();
					series->SetModality(modality);

					if (modality != "seg")
					{
						if (seriesObj.contains("series_description"))
						{
							series->SetSeriesDescription(
								seriesObj["series_description"].toString()
							);
						}
					}
					else
					{
						if (seriesObj.contains("segment_label"))
						{
							series->SetSegmentLabel(
								seriesObj["segment_label"].toString()
							);
						}						
					}
				}

				if (!seriesObj.contains("images"))
				{
					delete series;
					continue;
				}

				QList<captk::CohortImage*> images;

				// Iterate over the images and add them
				for (QJsonValueRef imageRef : seriesObj["images"].toArray())
				{
					auto imageObj = imageRef.toObject();

					captk::CohortImage* image = new captk::CohortImage(series);

					if (!imageObj.contains("path"))
					{
						delete image;
						continue;
					}

					image->SetPath(imageObj["path"].toString());

					if (imageObj.contains("image_info"))
					{
						image->SetImageInfoPath(imageObj["image_info"].toString());
					}

					images.push_back(image);
				}

				if (images.size() == 0)
				{
					delete series;
					continue;
				}

				// Add to series
				series->SetImages(images);

				series_of_study.push_back(series);
			}

			if (series_of_study.size() == 0)
			{
				delete study;
				continue;
			}

			// Add to study
			study->SetSeries(series_of_study);

			studies.push_back(study);
		}

		// Check if there are any studies
		if (studies.size() == 0)
		{
			delete subject;
			continue;
		}

		// Add to subject
		subject->SetStudies(studies);

		subjects.push_back(subject);
	}

	cohort->SetSubjects(subjects);
	return cohort;
}

QStringList captk::internal::GetSubdirectories(QString& directory)
{
	QStringList list;

	QFileInfo directoryFileInfo(directory);
	if (!directoryFileInfo.exists() || !directoryFileInfo.isDir())
	{
		return list;
	}
	
	QDirIterator iter(directory, QDir::Dirs | QDir::NoDotAndDotDot);
	while(iter.hasNext())
	{
		QString subDirPath = iter.next();
		QFileInfo subDirFileInfo(subDirPath);
		list << subDirPath;
	}

	list.sort(Qt::CaseInsensitive);

	return list;
}

QStringList captk::internal::GetContainedFiles(QString& directory)
{
	QFileInfo directoryFileInfo(directory);
	if (!directoryFileInfo.exists() || !directoryFileInfo.isDir())
	{
		return QStringList();
	}

	QStringList list;
	
	QDirIterator iter(directory, QDir::Files);
	while(iter.hasNext())
	{
		QString subDirPath = iter.next();
		list << subDirPath;
	}

	list.sort(Qt::CaseInsensitive);

	return list;	
}

QString captk::internal::GetFileNameFromPath(QString& path)
{
	QFileInfo fileInfo(path);
	return fileInfo.fileName();
}

bool captk::internal::IsDir(QString& path)
{
	QFileInfo fileInfo(path);
	return fileInfo.isDir();
}