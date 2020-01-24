#include "CaPTkCohortJsonFromFilesystem.h"

#include <QString>
#include <QStringList>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>

Json::Value captk::CohortMergeJsonObjects(std::vector<Json::Value> jsons)
{
	if (jsons.size() == 0)
	{
		return Json::Value();
	}
	if (jsons.size() == 1)
	{
		return jsons[0];
	}

	// Actual merging

	//you will need
	// const Json::Value subjects = root["subjects"];
	// if (!subjects.isObject() || subjects.isNull())
	// {
	// 	return root;
	// }

	return Json::Value(); // delete this
}

Json::Value captk::CohortJsonFromFilesystem(std::string directory)
{
	Json::Value root;

	// ---- Cohort name ----

	if (!captk::IsDir(directory))
	{
		return root;
	}
	else
	{
		root["cohort_name"] = captk::GetFileNameFromPath(directory);
	}

	// ---- Subjects ----

	root["subjects"] = Json::Value(Json::arrayValue); // Create empty array

	// For iterating the subdirectories of the cohort.
	// Each directory is a subject
	auto subjPaths = captk::GetSubdirectories(directory);

	for (auto& subjPath : subjPaths)
	{
		if (!captk::IsDir(subjPath)) { continue; }
		auto subj = captk::GetFileNameFromPath(subjPath);
		qDebug() << subj.c_str();
		
		// ---- Create subject ----
		Json::Value subject;
		subject["name"]   = subj;
		subject["studies"] = Json::Value(Json::arrayValue);

		// ---- Add all the images ----

		auto studiesPaths = captk::GetSubdirectories(subjPath);

		for (auto& studyPath : studiesPaths)
		{
			if (!captk::IsDir(studyPath)) { continue; }
			auto study = captk::GetFileNameFromPath(studyPath);
			qDebug() << "\t" << study.c_str();

			Json::Value studyjson;
			studyjson["name"]   = study;
			studyjson["images"] = Json::Value(Json::arrayValue);

			auto modalitiesPaths = captk::GetSubdirectories(studyPath);
			for (auto& modalityPath : modalitiesPaths)
			{
				if (!captk::IsDir(modalityPath)) { continue; }
				auto modality = captk::GetFileNameFromPath(modalityPath);
				qDebug() << "\t" << "\t"<< modality.c_str();

				auto serDescPaths = captk::GetSubdirectories(modalityPath);
				for (auto& serDescPath : serDescPaths)
				{
					if (!captk::IsDir(serDescPath)) { continue; }
					auto serDesc = captk::GetFileNameFromPath(serDescPath);
					qDebug() << "\t" << "\t" << "\t" << serDesc.c_str();

					auto filesPaths = captk::GetContainedFiles(serDescPath);
					qDebug() << filesPaths.size();
					
					std::string image_path = "";
					std::string image_info = "";
					for (auto& filePath : filesPaths)
					{
						if (captk::IsDir(filePath)) { continue; /*Discard dirs*/}
						auto fileName = captk::GetFileNameFromPath(filePath);
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

					Json::Value image;
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

Json::Value captk::CohortJsonFromFilesystem(std::vector<std::string> directoriesVector)
{
	std::vector<Json::Value> jsons;

	for (std::string& directory : directoriesVector)
	{
		jsons.push_back(captk::CohortJsonFromFilesystem(directory));
	}

	return captk::CohortMergeJsonObjects(jsons);
}

std::vector<std::string> captk::GetSubdirectories(std::string directory)
{
	QFileInfo directoryFileInfo(directory.c_str());
	if (!directoryFileInfo.exists() || !directoryFileInfo.isDir())
	{
		return std::vector<std::string>();
	}

	QStringList list;
	
	QDirIterator iter(directory.c_str(), QDir::Dirs | QDir::NoDotAndDotDot);
	while(iter.hasNext())
	{
		QString subDirPath = iter.next();
		QFileInfo subDirFileInfo(subDirPath);
		list << subDirPath;
	}

	list.sort(Qt::CaseInsensitive);

	// Convert to vector
	std::vector<std::string> res;
	for (QString s : list)
	{
		res.push_back(s.toStdString());
	}
	return res;
}

std::vector<std::string> captk::GetContainedFiles(std::string directory)
{
	QFileInfo directoryFileInfo(directory.c_str());
	if (!directoryFileInfo.exists() || !directoryFileInfo.isDir())
	{
		return std::vector<std::string>();
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
	std::vector<std::string> res;
	for (QString s : list)
	{
		res.push_back(s.toStdString());
	}
	return res;	
}

std::string captk::GetFileNameFromPath(std::string path)
{
	QFileInfo fileInfo(path.c_str());
	return fileInfo.fileName().toStdString();
}

bool captk::IsDir(std::string path)
{
	QFileInfo fileInfo(path.c_str());
	return fileInfo.isDir();
}