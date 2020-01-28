#include "CaPTkCohortOperations.h"

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

QSharedPointer<captk::Cohort> 
captk::CohortMerge(QList<QSharedPointer<captk::Cohort>> cohorts)
{
	QSharedPointer<captk::Cohort> mergedCohort(new captk::Cohort());

	if (cohorts.size() == 0)
	{
		return mergedCohort;
	}

	QString mergedCohortName = QString();

	QList<QSharedPointer<captk::CohortSubject>> mergedCohortSubjects;
	QStringList mergedCohortSubjectNames;

	for (QSharedPointer<captk::Cohort> cohort : cohorts)
	{
		if (mergedCohortName == "")
		{
			mergedCohortName = cohort->GetName();
		}
		else
		{
			mergedCohortName += "_" + cohort->GetName();
		}

		for (auto subject : cohort->GetSubjects())
		{
			// Check if new subject
			if (!mergedCohortSubjectNames.contains(subject->GetName()))
			{
				mergedCohortSubjectNames.append(subject->GetName());
				mergedCohortSubjects.append(subject);
				continue;
			}
			
			// From now one assume it's a duplicate subject

			// for (auto study : subject->GetStudies())
			// {
			// 	// Check if new study
			// 	for (auto study_s : )
			// 	if (!)
			// }
		}		
		
		// TODO: Actual merging
		std::cout << "\n!MERGING COHORTS NOT IMPLEMENTED YET!\n\n";
	}

	mergedCohort->SetName(mergedCohortName);
	mergedCohort->SetSubjects(mergedCohortSubjects);

	return mergedCohort;
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

					auto filesPaths = captk::internal::GetFilesInDir(serDescPath);
					
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

QSharedPointer<captk::Cohort> 
captk::CohortJsonLoad(QSharedPointer<QJsonDocument> json)
{
	QSharedPointer<captk::Cohort> cohort(new captk::Cohort());
	
	QJsonObject root = json->object();

	if (root.contains("cohort_name"))
	{
		cohort->SetName(root["cohort_name"].toString());
	}

	if (!root.contains("subjects"))
	{
		return cohort; // No subjects found
	}

	QList<QSharedPointer<captk::CohortSubject>> subjects;

	// Iterate over the subjects of the cohort
	for (QJsonValueRef subjRef : root["subjects"].toArray())
	{
		auto subjObj = subjRef.toObject();

		QSharedPointer<captk::CohortSubject> subject(new captk::CohortSubject());

		if (subjObj.contains("name"))
		{
			subject->SetName(subjObj["name"].toString());
		}
		if (!subjObj.contains("studies"))
		{
			continue;
		}

		QList<QSharedPointer<captk::CohortStudy>> studies;

		// Iterate over the studies of the subject
		for (QJsonValueRef studyRef : subjObj["studies"].toArray())
		{
			auto studyObj = studyRef.toObject();

			QSharedPointer<captk::CohortStudy> study(new captk::CohortStudy());

			if (studyObj.contains("name"))
			{
				study->SetName(studyObj["name"].toString());
			}
			if (!studyObj.contains("series"))
			{
				continue;
			}

			QList<QSharedPointer<captk::CohortSeries>> series_of_study;

			// Iterate over the collection of series of the study
			for (QJsonValueRef seriesRef : studyObj["series"].toArray())
			{
				auto seriesObj = seriesRef.toObject();

				QSharedPointer<captk::CohortSeries> series(new captk::CohortSeries());

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
					continue;
				}

				QList<QSharedPointer<captk::CohortImage>> images;

				// Iterate over the images and add them
				for (QJsonValueRef imageRef : seriesObj["images"].toArray())
				{
					auto imageObj = imageRef.toObject();

					QSharedPointer<captk::CohortImage> image(new captk::CohortImage());

					if (!imageObj.contains("path"))
					{
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
					continue;
				}

				// Add to series
				series->SetImages(images);

				series_of_study.push_back(series);
			}

			if (series_of_study.size() == 0)
			{
				continue;
			}

			// Add to study
			study->SetSeries(series_of_study);

			studies.push_back(study);
		}

		// Check if there are any studies
		if (studies.size() == 0)
		{
			continue;
		}

		// Add to subject
		subject->SetStudies(studies);

		subjects.push_back(subject);
	}

	cohort->SetSubjects(subjects);
	return cohort;
}

QSharedPointer<QJsonDocument>
captk::CohortToJson(QSharedPointer<captk::Cohort> /*cohort*/)
{
	// TODO
	return QSharedPointer<QJsonDocument>(new QJsonDocument());
}

QStringList captk::internal::GetSubdirectories(QString& directory)
{
	QStringList list;

	// Check if input is actually a directory
	QFileInfo directoryFileInfo(directory);
	if (!directoryFileInfo.exists() || !directoryFileInfo.isDir())
	{
		return list;
	}
	
	// Iterator only for subdirs
	QDirIterator iter(directory, QDir::Dirs | QDir::NoDotAndDotDot);
	while(iter.hasNext())
	{
		QString subDirPath = iter.next();
		list << subDirPath;
	}

	// We want the first one alphabetically to show up first
	list.sort(Qt::CaseInsensitive);

	return list;
}

QStringList captk::internal::GetFilesInDir(QString& directory)
{
	// Check if input is actually a directory
	QFileInfo directoryFileInfo(directory);
	if (!directoryFileInfo.exists() || !directoryFileInfo.isDir())
	{
		return QStringList();
	}

	QStringList list;
	
	// Iterator only for sub-files
	QDirIterator iter(directory, QDir::Files);
	while(iter.hasNext())
	{
		QString subDirPath = iter.next();
		list << subDirPath;
	}

	// We (maybe) want the alphabetically smaller -> first
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