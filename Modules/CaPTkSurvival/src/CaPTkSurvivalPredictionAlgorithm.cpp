
#include "itkCSVArray2DFileReader.h"
#include "itkImageFileReader.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkCSVNumericObjectFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"

#include "CaPTkSurvivalPredictionAlgorithm.h"
#include "CaPTkFeatureScalingClass.h"
#include "CaPTkFeatureExtractionClass.h"
#include "CaPTkEnums.h"
#include <QFileInfo>
#include <QDir>

namespace captk {
typedef itk::Image< float, 3 > ImageType;
typedef itk::CSVArray2DFileReader<double> CSVFileReaderType;
typedef vnl_matrix<double> MatrixType;

// utilities -- TBD: move these into the class?
QStringList qsubdirectoriesInDirectory(std::string directoryname) {
    return QStringList(QDir(QString::fromStdString(directoryname)).entryList(QDir::Dirs));
}

QStringList qfilesInDirectory(std::string directoryname) {
    return QStringList(QDir(QString::fromStdString(directoryname)).entryList(QDir::Files));
}

std::vector<std::string> filesInDirectory(std::string directoryname) {
    QStringList files = qfilesInDirectory(directoryname);
    std::vector<std::string> res;
    for (int i = 0; i < files.size(); i++) {
        res.push_back(files[i].toStdString());
    }
}
std::vector<std::string> subdirectoriesInDirectory(std::string directoryname) {
    QStringList subdirs = qsubdirectoriesInDirectory(directoryname);
    std::vector<std::string> res;
    for (int i = 0; i < subdirs.size(); i++) {
        res.push_back(subdirs[i].toStdString());
    }
}

bool directoryExists(std::string path) {
    auto fileInfo = QFileInfo(QString::fromStdString(path + "/SEGMENTATION"));
    return (fileInfo.exists() && fileInfo.isDir());
}

bool fileExists(std::string path) {
    auto fileInfo = QFileInfo(QString::fromStdString(path + "/SEGMENTATION"));
    return (fileInfo.exists() && fileInfo.isFile());
}

std::string getFilenameExtension(std::string path) {
    return QFileInfo(QString::fromStdString(path)).completeSuffix().toStdString();
}



std::tuple<bool, std::string>
SurvivalPredictionModuleAlgorithm::Run(
        QString /*modelDir*/,
        QString /*subjectDir*/,
        QString /*outputDir*/,
        bool /*trainNewModel*/,
        bool /*useCustomModel*/)
{
    // ******************************************
    // TODO: Include actual algorithm logic here
    // ******************************************

    std::tuple<bool, std::string> res; // our result (boolean success and potential error message)
    try {
        // All the stuff required to run should go here (wrapper function)
    }
    catch (std::string errorMessage) {
        res = std::make_tuple(false, errorMessage);
    }

    return res;
}

std::vector<std::map<captk::ImageModalityType, std::string>> SurvivalPredictionModuleAlgorithm::LoadQualifiedSubjectsFromGivenDirectory(const std::string directoryname)
{
    std::map<captk::ImageModalityType, std::string> OneQualifiedSubject;
    std::vector<std::map<captk::ImageModalityType, std::string>> QualifiedSubjects;
    std::vector<std::string> subjectNames = subdirectoriesInDirectory(directoryname);// get only dirs
  std::sort(subjectNames.begin(), subjectNames.end());

    for (unsigned long sid = 0; sid < subjectNames.size(); sid++)
    {
        std::string subjectPath = directoryname + "/" + subjectNames[sid];

        std::string t1ceFilePath = "";
        std::string t1FilePath = "";
        std::string t2FilePath = "";
        std::string t2FlairFilePath = "";
        std::string axFilePath = "";
        std::string faFilePath = "";
        std::string radFilePath = "";
        std::string trFilePath = "";
        std::string rcbvFilePath = "";
        std::string psrFilePath = "";
        std::string phFilePath = "";
        std::string labelPath = "";
        std::string atlasPath = "";
        std::string parametersPath = "";
        std::string featureFilePath = "";

        std::vector<std::string> files;

        if (directoryExists(subjectPath + "/SEGMENTATION"))
        {
            files = filesInDirectory(subjectPath + "/SEGMENTATION");
            if (files.size() == 1)
            {
                labelPath = subjectPath + "/SEGMENTATION" + "/" + files[0];
            }
            else
            {
                for (int i = 0; i < files.size(); i++)
                {
                    std::string filePath = subjectPath + "/SEGMENTATION" + "/" + files[i], filePath_lower;
                    std::string extension = getFilenameExtension(filePath);
                    filePath_lower = filePath;
                    std::transform(filePath_lower.begin(), filePath_lower.end(), filePath_lower.begin(), ::tolower);
                    if ((filePath_lower.find("atlas") != std::string::npos || filePath_lower.find("jakob_label") != std::string::npos)
                        && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                        atlasPath = subjectPath + "/SEGMENTATION" + "/" + files[i];
                    else if ((filePath_lower.find("segmentation") != std::string::npos)
                        && (extension == HDR_EXT || extension
                        == NII_EXT || extension == NII_GZ_EXT))
                        labelPath = subjectPath + "/SEGMENTATION" + "/" + files[i];
                    else if ((filePath_lower.find("parameter") != std::string::npos)
                        && (extension == PARAM_EXT))
                        parametersPath = subjectPath + "/SEGMENTATION" + "/" + files[i];
                }
            }
        }

        if (directoryExists(subjectPath + "/CONVENTIONAL"))
        {
            files = filesInDirectory(subjectPath + "/CONVENTIONAL");
            for (unsigned int i = 0; i < files.size(); i++)
            {
                std::string filePath = subjectPath + "/CONVENTIONAL" + "/" + files[i];
                std::string extension = getFilenameExtension(filePath);

                if ((files[i].find("t1ce") != std::string::npos || files[i].find("T1CE") != std::string::npos || files[i].find("T1ce") != std::string::npos || files[i].find("T1-gd") != std::string::npos) && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    t1ceFilePath = subjectPath + "/CONVENTIONAL" + "/" + files[i];
                else if ((files[i].find("t1") != std::string::npos || files[i].find("T1") != std::string::npos) && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    t1FilePath = subjectPath + "/CONVENTIONAL" + "/" + files[i];
                else if ((files[i].find("t2") != std::string::npos || files[i].find("T2") != std::string::npos) && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    t2FilePath = subjectPath + "/CONVENTIONAL" + "/" + files[i];
                else if ((files[i].find("flair") != std::string::npos || files[i].find("FLAIR") != std::string::npos || files[i].find("Flair") != std::string::npos || files[i].find("T2-Flair") != std::string::npos) && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    t2FlairFilePath = subjectPath + "/CONVENTIONAL" + "/" + files[i];
            }
        }

        if (directoryExists(subjectPath + "/PERFUSION"))
        {
            files = filesInDirectory(subjectPath + "/PERFUSION");
            for (unsigned int i = 0; i < files.size(); i++)
            {
                std::string filePath = subjectPath + "/PERFUSION" + "/" + files[i], filePath_lower;
                std::string extension = getFilenameExtension(filePath);
                filePath_lower = filePath;
                std::transform(filePath_lower.begin(), filePath_lower.end(), filePath_lower.begin(), ::tolower);
                if ((filePath_lower.find("rcbv") != std::string::npos)
                    && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    rcbvFilePath = subjectPath + "/PERFUSION" + "/" + files[i];
                else if ((filePath_lower.find("psr") != std::string::npos)
                    && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    psrFilePath = subjectPath + "/PERFUSION" + "/" + files[i];
                else if ((filePath_lower.find("ph") != std::string::npos)
                    && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    phFilePath = subjectPath + "/PERFUSION" + "/" + files[i];
            }
        }


        if (directoryExists(subjectPath + "/DTI"))
        {
            files = filesInDirectory(subjectPath + "/DTI");
            for (unsigned int i = 0; i < files.size(); i++)
            {
                std::string filePath = subjectPath + "/DTI/" + files[i];
                std::string extension = getFilenameExtension(filePath);

                if ((files[i].find("Axial") != std::string::npos || files[i].find("axial") != std::string::npos) && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    axFilePath = subjectPath + "/DTI/" + files[i];
                else if ((files[i].find("Fractional") != std::string::npos || files[i].find("fractional") != std::string::npos) && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    faFilePath = subjectPath + "/DTI/" + files[i];
                else if ((files[i].find("Radial") != std::string::npos || files[i].find("radial") != std::string::npos) && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    radFilePath = subjectPath + "/DTI/" + files[i];
                else if ((files[i].find("Trace") != std::string::npos || files[i].find("trace") != std::string::npos) && (extension == HDR_EXT || extension == NII_EXT || extension == NII_GZ_EXT))
                    trFilePath = subjectPath + "/DTI/" + files[i];
            }
        }
        if (fileExists(subjectPath + "/features.csv"))
            featureFilePath = subjectPath + "/features.csv";

            if (labelPath.empty() || t1FilePath.empty() || t2FilePath.empty() || t1ceFilePath.empty() || t2FlairFilePath.empty() || rcbvFilePath.empty() || axFilePath.empty() || faFilePath.empty()
        || radFilePath.empty() || trFilePath.empty() || psrFilePath.empty() || phFilePath.empty())
            continue;

        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_T1] = t1FilePath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_T2] = t2FilePath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_T1CE] = t1ceFilePath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_T2FLAIR] = t2FlairFilePath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_AX] = axFilePath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_FA] = faFilePath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_RAD] = radFilePath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_TR] = trFilePath;

        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_PSR] = psrFilePath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_PH] = phFilePath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_RCBV] = rcbvFilePath;

        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_SEG] = labelPath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_ATLAS] = atlasPath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_PARAMS] = parametersPath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_FEATURES] = featureFilePath;
        OneQualifiedSubject[captk::ImageModalityType::IMAGE_TYPE_SUDOID] = subjectNames[sid];

        QualifiedSubjects.push_back(OneQualifiedSubject);
    }
    return QualifiedSubjects;
}
}
