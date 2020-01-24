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

QSharedPointer<QJsonDocument> 
captk::CohortJsonMergeObjects(QList<QSharedPointer<QJsonDocument>>& jsons)
{
	if (jsons.size() == 0)
	{
		return QJsonObject();
	}
	if (jsons.size() == 1)
	{
		return jsons[0];
	}

	// TODO: Actual merging
	std::cout << "\n!MERGING JSON OBJECTS NOT IMPLEMENTED YET!\n\n";
	return QJsonObject(); // delete this when actual implementation is here
}

QSharedPointer<QJsonDocument> 
captk::CohortJsonFromDirectoryStructure(QStringList& directory)
{
	QJsonObject root;

	// ---- Cohort name ----

	if (!captk::internal::IsDir(directory))
	{
		mitkThrow() << "Directory doesn't exist";
		return root;
	}
	else
	{
		root["cohort_name"] = captk::internal::GetFileNameFromPath(directory);
	}

	// ---- Subjects ----

	root["subjects"] = QJsonArray(); // Create empty array

	// For iterating the subdirectories of the cohort.
	// Each directory is a subject
	auto subjPaths = captk::internal::GetSubdirectories(directory);

	for (auto& subjPath : subjPaths)
	{
		if (!captk::internal::IsDir(subjPath)) { continue; }
		auto subj = captk::internal::GetFileNameFromPath(subjPath);
		qDebug() << subj.c_str();
		
		// ---- Create subject ----
		QJsonObject subject;
		subject["name"]   = subj;
		subject["studies"] = QJsonObject(Json::arrayValue);

		// ---- Add all the images ----

		auto studiesPaths = captk::internal::GetSubdirectories(subjPath);

		for (auto& studyPath : studiesPaths)
		{
			if (!captk::internal::IsDir(studyPath)) { continue; }
			auto study = captk::internal::GetFileNameFromPath(studyPath);
			qDebug() << "\t" << study.c_str();

			QJsonObject studyjson;
			studyjson["name"]   = study;
			studyjson["images"] = QJsonObject(Json::arrayValue);

			auto modalitiesPaths = captk::internal::GetSubdirectories(studyPath);
			for (auto& modalityPath : modalitiesPaths)
			{
				if (!captk::internal::IsDir(modalityPath)) { continue; }
				auto modality = captk::internal::GetFileNameFromPath(modalityPath);
				qDebug() << "\t" << "\t"<< modality.c_str();

				auto serDescPaths = captk::internal::GetSubdirectories(modalityPath);
				for (auto& serDescPath : serDescPaths)
				{
					if (!captk::internal::IsDir(serDescPath)) { continue; }
					auto serDesc = captk::internal::GetFileNameFromPath(serDescPath);
					qDebug() << "\t" << "\t" << "\t" << serDesc.c_str();

					auto filesPaths = captk::internal::GetContainedFiles(serDescPath);
					qDebug() << filesPaths.size();
					
					QString image_path = "";
					QString image_info = "";
					for (auto& filePath : filesPaths)
					{
						if (captk::internal::IsDir(filePath)) { continue; /*Discard dirs*/}
						auto fileName = captk::internal::GetFileNameFromPath(filePath);
						qDebug() << "\t" << "\t" << "\t" << "\t" << fileName.c_str();

						if (QString(fileName.c_str()).endsWith(QString("json")))
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

					studyjson["images"].append(image);
				}				
			}

			if (studyjson["images"].size() > 0)
			{
				subject["studies"].append(studyjson);			
			}
		}

		// ---- Add the subject to the list ----
		if (subject["studies"].size() > 0)
		{
			root["subjects"].append(subject);
		}
	}
	
	return root;
}

QStringList captk::internal::GetSubdirectories(QString& directory)
{
	QStringList list;

	QFileInfo directoryFileInfo(directory.c_str());
	if (!directoryFileInfo.exists() || !directoryFileInfo.isDir())
	{
		return list;
	}
	
	QDirIterator iter(directory.c_str(), QDir::Dirs | QDir::NoDotAndDotDot);
	while(iter.hasNext())
	{
		QString subDirPath = iter.next();
		QFileInfo subDirFileInfo(subDirPath);
		list << subDirPath;
	}

	list.sort(Qt::CaseInsensitive);

	// Convert to vector
	QStringList res;
	for (QString s : list)
	{
		res.push_back(s.toStdString());
	}
	return res;
}

QStringList captk::internal::GetContainedFiles(QString& directory)
{
	QFileInfo directoryFileInfo(directory.c_str());
	if (!directoryFileInfo.exists() || !directoryFileInfo.isDir())
	{
		return QStringList();
	}

	QStringList list;
	
	QDirIterator iter(directory.c_str(), QDir::Files);
	while(iter.hasNext())
	{
		QString subDirPath = iter.next();
		list << subDirPath;
	}

	list.sort(Qt::CaseInsensitive);

	// Convert to vector
	QStringList res;
	for (QString s : list)
	{
		res.push_back(s.toStdString());
	}
	return res;	
}

QString captk::internal::GetFileNameFromPath(QString& path)
{
	QFileInfo fileInfo(path.c_str());
	return fileInfo.fileName().toStdString();
}

bool captk::internal::IsDir(QString& path)
{
	QFileInfo fileInfo(path.c_str());
	return fileInfo.isDir();
}