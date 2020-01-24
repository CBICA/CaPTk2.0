#include "CaPTkCohortJsonOperations.h"

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
			auto images = QJsonArray();

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

					auto filesPaths = captk::internal::GetContainedFiles(serDescPath);
					qDebug() << filesPaths.size();
					
					QString image_path = "";
					QString image_info = "";
					for (auto& filePath : filesPaths)
					{
						if (captk::internal::IsDir(filePath)) { continue; /*Discard dirs*/}
						auto fileName = captk::internal::GetFileNameFromPath(filePath);
						qDebug() << "\t" << "\t" << "\t" << "\t" << fileName;

						if (fileName.endsWith(QString("json")))
						{
							image_info = filePath;
						}
						else
						{
							image_path = filePath;
						}
					}

					if (image_path == "") { continue; }

					QJsonObject image;
					image["path"] = image_path;
					image["modality"] = modality;
					if (serDesc != "segmentation")
					{
						image["series_description"] = serDesc;
					}
					else
					{
						image["segmentation_of"] = serDesc;
					}
					if (image_info != "")
					{
						image["image_info_path"] = image_info;
					} 

					images.push_back(image);
				}				
			}

			if (images.size() > 0)
			{
				studyjson["images"] = images;
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