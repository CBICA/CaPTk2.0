
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
#include "CaPTkClassifierUtils.h"

#include <string>
#include <QFileInfo>
#include <QDir>
#include <QApplication>


// TODO: clean up the paths used for locating configurations -- they should be alongside the model, not in it
namespace captk {
typedef itk::Image< float, 3 > ImageType;
typedef itk::CSVArray2DFileReader<double> CSVFileReaderType;
typedef vnl_matrix<double> MatrixType;
typedef std::vector<double> VectorDouble;
typedef itk::VariableSizeMatrix<double> VariableSizeMatrixType;
typedef itk::VariableLengthVector<double> VariableLengthVectorType;

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
    return res;
}
std::vector<std::string> subdirectoriesInDirectory(std::string directoryname) {
    QStringList subdirs = qsubdirectoriesInDirectory(directoryname);
    std::vector<std::string> res;
    for (int i = 0; i < subdirs.size(); i++) {
        res.push_back(subdirs[i].toStdString());
    }
    return res;
}

bool directoryExists(std::string path) {
    auto fileInfo = QFileInfo(QString::fromStdString(path));
    return (fileInfo.exists() && fileInfo.isDir());
}

bool fileExists(std::string path) {
    auto fileInfo = QFileInfo(QString::fromStdString(path));
    return (fileInfo.exists() && fileInfo.isFile());
}

std::string getFilenameExtension(std::string path) {
    return QFileInfo(QString::fromStdString(path)).completeSuffix().toStdString();
}

std::string getApplicationDirectory() {
    return qApp->applicationDirPath().toStdString();
}



std::tuple<bool, std::string>
SurvivalPredictionModuleAlgorithm::Run(
        QString modelDir,
        QString subjectDir,
        QString outputDir,
        bool trainNewModel,
        bool useCustomModel,
        QString cbicaModelDir)
{

    std::tuple<bool, std::string> res; // result tuple (boolean success and potential error message)
    try {
        if (trainNewModel){
            CallForNewSurvivalPredictionModel(subjectDir.toStdString(),
                                              outputDir.toStdString());
        }
        else if (useCustomModel) {
            CallForSurvivalPredictionOnExistingModel(modelDir.toStdString(),
                                                     subjectDir.toStdString(),
                                                     outputDir.toStdString());
        }
        else if (!useCustomModel) { // Use the CBICA CaPTk model
            CallForSurvivalPredictionOnExistingModel(cbicaModelDir.toStdString(),
                                                     subjectDir.toStdString(),
                                                     outputDir.toStdString());
        }

        res = std::make_tuple(true, ""); // if nothing was thrown, we succeeded

    }
    catch (std::exception error) { // catch error strings thrown by underlying functions
        std::string errorMessage = error.what();
        res = std::make_tuple(false, errorMessage);
        MITK_ERROR << errorMessage;
    }
    catch (...) {
        /* an uncaught exception in this background thread will terminate the thread
         * and therefore will never update the module  with
         * the OnAlgorithmFinished() signal. Therefore, we must catch ANY exception here
         * or the algorithm will appear to be running forever.
         * Only exceptions can be caught. Stuff like segfaults in the algorithm will still crash us. */

        std::string errorMessage = "An uncaught exception of unexpected type was thrown"
                                   " in the survival prediction thread, forcing"
                                   " termination of the algorithm.";
        MITK_ERROR << errorMessage;
        res = std::make_tuple(false, errorMessage);
    }

    return res;
}

void SurvivalPredictionModuleAlgorithm::CallForSurvivalPredictionOnExistingModel(const std::string modeldirectory, const std::string inputdirectory, const std::string outputdirectory)
{
  if (modeldirectory.empty())
  {
    throw std::runtime_error("Please provide path of a directory having SVM model.");
    return;
  }
  if (!directoryExists(modeldirectory))
  {
    throw std::runtime_error("The given SVM model directory does not exist.");
    return;
  }
  if (!(fileExists(modeldirectory + "/Survival_SVM_Model6.csv") || fileExists(modeldirectory + "/Survival_SVM_Model6.xml"))
    || !(fileExists(modeldirectory + "/Survival_SVM_Model18.csv") || fileExists(modeldirectory + "/Survival_SVM_Model18.xml"))
    || !fileExists(modeldirectory + "/Survival_ZScore_Std.csv") || !fileExists(modeldirectory + "/Survival_ZScore_Mean.csv"))
  {
    throw std::runtime_error("The given SVM model directory does not contain all the required model files.");
    return;
  }

  if (inputdirectory.empty())
  {
    throw std::runtime_error("Please provide the path of a directory containing input subjects.");
    return;
  }
  if (!directoryExists(inputdirectory))
  {
    throw std::runtime_error("The given input directory does not exist.");
    return;
  }
  if (outputdirectory.empty())
  {
    throw std::runtime_error("Please provide path of a directory to save output.");
    return;
  }
  if (!directoryExists(outputdirectory))
  {
    if (!QDir(QString::fromStdString(outputdirectory)).mkpath(QString::fromStdString(outputdirectory)))
    {
      throw std::runtime_error("Unable to create the output directory.");
      return;
    }
  }


  std::vector<double> finalresult;
  std::vector<std::map<captk::ImageModalityType, std::string>> QualifiedSubjects = LoadQualifiedSubjectsFromGivenDirectory(inputdirectory);
  if (QualifiedSubjects.size() == 0)
  {
    throw std::runtime_error("No patient inside the given input directory has required scans");
    return;
  }

  VectorDouble result = SurvivalPredictionOnExistingModel(modeldirectory, inputdirectory, QualifiedSubjects, outputdirectory);
  QString msg;
  if (result.size() == 0)
  {
    throw std::runtime_error("The survival model did not finish as expected, "
                      "please see log file for details.");
  }
  else
  {
      return;
  }
}

void SurvivalPredictionModuleAlgorithm::CallForNewSurvivalPredictionModel(const std::string inputdirectory, const std::string outputdirectory)
{
  std::vector<double> finalresult;

  if (inputdirectory.empty())
  {
    throw std::runtime_error("Please provide path of a directory containing input subjects.");
    return;
  }
  if (!directoryExists(inputdirectory))
  {
    throw std::runtime_error("The given input directory does not exist.");
    return;
  }


  if (outputdirectory.empty())
  {
    throw std::runtime_error("Please provide path of a directory to save output");
    return;
  }
  if (!directoryExists(outputdirectory))
  {
    if (!QDir(QString::fromStdString(outputdirectory)).mkpath(QString::fromStdString(outputdirectory)))
    {
      throw std::runtime_error("Unable to create the output directory");
      return;
    }
  }

  std::vector<std::map<captk::ImageModalityType, std::string>> QualifiedSubjects = LoadQualifiedSubjectsFromGivenDirectory(inputdirectory);
  if (QualifiedSubjects.size() == 0)
  {
    throw std::runtime_error("No patient inside the given input directory has required scans");
    return;
  }


  if (PrepareNewSurvivalPredictionModel(inputdirectory, QualifiedSubjects, outputdirectory) == false)
  {
    throw std::runtime_error("Survival Training did not finish as expected, please see log file for details. ");
  }
  else
  {
  }
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
                for (unsigned long i = 0; i < files.size(); i++)
                {


                    std::string filePath = subjectPath + "/SEGMENTATION" + "/" + files[i];
                    std::string filePath_lower = filePath;
                    std::transform(filePath_lower.begin(), filePath_lower.end(), filePath_lower.begin(), ::tolower);
                    std::string extension = getFilenameExtension(filePath_lower);


                    if ((filePath_lower.find("atlas") != std::string::npos || filePath_lower.find("jakob_label") != std::string::npos)
                        && (extension ==HDR_EXT || extension == std::string(NII_EXT) || extension ==NII_GZ_EXT)) {
                        atlasPath = subjectPath + "/SEGMENTATION" + "/" + files[i];
                    }
                    else if ((filePath_lower.find("segmentation") != std::string::npos)
                        && (extension ==HDR_EXT || extension
                        == NII_EXT || extension ==NII_GZ_EXT)) {
                        labelPath = subjectPath + "/SEGMENTATION" + "/" + files[i];
                    }
                    else if ((filePath_lower.find("parameter") != std::string::npos)
                        && (extension ==PARAM_EXT)) {
                        parametersPath = subjectPath + "/SEGMENTATION" + "/" + files[i];
                    }
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

                if ((files[i].find("t1ce") != std::string::npos || files[i].find("T1CE") != std::string::npos || files[i].find("T1ce") != std::string::npos || files[i].find("T1-gd") != std::string::npos) && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
                    t1ceFilePath = subjectPath + "/CONVENTIONAL" + "/" + files[i];
                else if ((files[i].find("t1") != std::string::npos || files[i].find("T1") != std::string::npos) && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
                    t1FilePath = subjectPath + "/CONVENTIONAL" + "/" + files[i];
                else if ((files[i].find("t2") != std::string::npos || files[i].find("T2") != std::string::npos) && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
                    t2FilePath = subjectPath + "/CONVENTIONAL" + "/" + files[i];
                else if ((files[i].find("flair") != std::string::npos || files[i].find("FLAIR") != std::string::npos || files[i].find("Flair") != std::string::npos || files[i].find("T2-Flair") != std::string::npos) && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
                    t2FlairFilePath = subjectPath + "/CONVENTIONAL" + "/" + files[i];
            }
        }

        if (directoryExists(subjectPath + "/PERFUSION"))
        {

            files = filesInDirectory(subjectPath + "/PERFUSION");
            for (unsigned int i = 0; i < files.size(); i++)
            {
                std::string filePath = subjectPath + "/PERFUSION" + "/" + files[i];
                std::string filePath_lower = filePath;
                std::string extension = getFilenameExtension(filePath);
                std::transform(filePath_lower.begin(), filePath_lower.end(), filePath_lower.begin(), ::tolower);

                if ((filePath_lower.find("rcbv") != std::string::npos)
                    && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
                    rcbvFilePath = subjectPath + "/PERFUSION" + "/" + files[i];
                else if ((filePath_lower.find("psr") != std::string::npos)
                    && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
                    psrFilePath = subjectPath + "/PERFUSION" + "/" + files[i];
                else if ((filePath_lower.find("ph") != std::string::npos)
                    && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
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

                if ((files[i].find("Axial") != std::string::npos || files[i].find("axial") != std::string::npos) && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
                    axFilePath = subjectPath + "/DTI/" + files[i];
                else if ((files[i].find("Fractional") != std::string::npos || files[i].find("fractional") != std::string::npos) && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
                    faFilePath = subjectPath + "/DTI/" + files[i];
                else if ((files[i].find("Radial") != std::string::npos || files[i].find("radial") != std::string::npos) && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
                    radFilePath = subjectPath + "/DTI/" + files[i];
                else if ((files[i].find("Trace") != std::string::npos || files[i].find("trace") != std::string::npos) && (extension ==HDR_EXT || extension ==NII_EXT || extension ==NII_GZ_EXT))
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

VectorDouble SurvivalPredictionModuleAlgorithm::GetVolumetricFeatures(const double &edemaSize,const double &tuSize, const double &neSize, const double &totalSize)
{
  VectorDouble VolumetricFeatures;
  VolumetricFeatures.push_back(tuSize);
  VolumetricFeatures.push_back(neSize);
  VolumetricFeatures.push_back(edemaSize);
  VolumetricFeatures.push_back(totalSize);

  VolumetricFeatures.push_back(tuSize + neSize);
  VolumetricFeatures.push_back( (tuSize + neSize) / totalSize);
  VolumetricFeatures.push_back(edemaSize / totalSize);
  return VolumetricFeatures;
}

VectorDouble SurvivalPredictionModuleAlgorithm::GetStatisticalFeatures(const VectorDouble &intensities)
{
  VectorDouble StatisticalFeatures;

  double temp = 0.0;
  double mean = 0.0;
  double std = 0.0;

  for (unsigned int i = 0; i < intensities.size(); i++)
    temp = temp + intensities[i];
  mean = temp / intensities.size();

  temp = 0;
  for (unsigned int i = 0; i < intensities.size(); i++)
    temp = temp + (intensities[i] - mean)*(intensities[i] - mean);
  std = std::sqrt(temp / (intensities.size() - 1));

  StatisticalFeatures.push_back(mean);
  StatisticalFeatures.push_back(std);

  return StatisticalFeatures;
}

VectorDouble SurvivalPredictionModuleAlgorithm::GetHistogramFeatures(const VectorDouble &intensities, const double &start, const double &interval, const double &end)
{
  VariableLengthVectorType BinCount;
  VectorDouble finalBins;
  std::vector<VectorDouble> Ranges;
  for (double i = start; i <= end; i = i + interval)
  {
    VectorDouble onerange;
    onerange.push_back(i - (interval / 2));
    onerange.push_back(i + (interval / 2));
    Ranges.push_back(onerange);
  }
  Ranges[Ranges.size() - 1][1] = 255;
  Ranges[0][0] = 0;


  BinCount.SetSize(Ranges.size());
  for (unsigned int j = 0; j < Ranges.size(); j++)
  {
    VectorDouble onerange = Ranges[j];
    int counter = 0;
    for (unsigned int i = 0; i < intensities.size(); i++)
    {
      if (onerange[0] == 0)
      {
        if (intensities[i] >= onerange[0] && intensities[i] <= onerange[1])
          counter = counter + 1;
      }
      else
      {
        if (intensities[i] > onerange[0] && intensities[i] <= onerange[1])
          counter = counter + 1;
      }
    }
    finalBins.push_back(counter);
  }
  return finalBins;
}

int SurvivalPredictionModuleAlgorithm::PrepareNewSurvivalPredictionModel(const std::string& /*inputdirectory*/ ,const std::vector< std::map< captk::ImageModalityType, std::string > > &qualifiedsubjects, const std::string &outputdirectory)
{
  VectorDouble AllSurvival;
  VariableSizeMatrixType FeaturesOfAllSubjects;
  VariableSizeMatrixType HistogramFeaturesConfigurations;
  HistogramFeaturesConfigurations.SetSize(33, 3); //11 modalities*3 regions = 33 configurations*3 histogram features for each configuration
  CSVFileReaderType::Pointer reader = CSVFileReaderType::New();
  MatrixType dataMatrix;
  try
  {
      reader->SetFileName(getApplicationDirectory() + "/models/survival_model/Survival_HMFeatures_Configuration.csv");
      reader->SetFieldDelimiterCharacter(',');
      reader->HasColumnHeadersOff();
      reader->HasRowHeadersOff();
      reader->Parse();
      dataMatrix = reader->GetArray2DDataObject()->GetMatrix();

      for (unsigned int i = 0; i < dataMatrix.rows(); i++)
          for (unsigned int j = 0; j < dataMatrix.cols(); j++)
              HistogramFeaturesConfigurations(i, j) = dataMatrix(i, j);
  }
  catch (const std::exception& e1)
  {
      throw std::runtime_error(
                  std::string("Cannot find the file 'Survival_HMFeatures_Configuration.csv' in the ../models/survival_model directory.")
                  + e1.what()
                  ); // propagate error message up to algorithm-running logic
  }
 //---------------------------------------------------------------------------
  FeaturesOfAllSubjects.SetSize(qualifiedsubjects.size(), 161);
  for (unsigned int sid = 0; sid < qualifiedsubjects.size(); sid++)
  {
      MITK_INFO << "Patient's data loaded:" << sid + 1;
      std::map< captk::ImageModalityType, std::string > currentsubject = qualifiedsubjects[sid];
      try
      {
          ImageType::Pointer LabelImagePointer = ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_SEG]));
          ImageType::Pointer AtlasImagePointer = ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_ATLAS]));
          ImageType::Pointer TemplateImagePointer = ReadNiftiImage<ImageType>(getApplicationDirectory() + "/models/survival_model/Template.nii.gz");
          ImageType::Pointer RCBVImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_RCBV])));
          ImageType::Pointer PHImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_PH])));
          ImageType::Pointer T1CEImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_T1CE])));
          ImageType::Pointer T2FlairImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_T2FLAIR])));
          ImageType::Pointer T1ImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_T1])));
          ImageType::Pointer T2ImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_T2])));
          ImageType::Pointer AXImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_AX])));
          ImageType::Pointer RADImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_RAD])));
          ImageType::Pointer FAImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_FA])));
          ImageType::Pointer TRImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_TR])));
          ImageType::Pointer PSRImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_PSR])));

          VectorDouble ages;
          VectorDouble survival;

          reader->SetFileName(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_FEATURES]));
          reader->SetFieldDelimiterCharacter(',');
          reader->HasColumnHeadersOff();
          reader->HasRowHeadersOff();
          reader->Parse();
          dataMatrix = reader->GetArray2DDataObject()->GetMatrix();

          for (unsigned int i = 0; i < dataMatrix.rows(); i++)
          {
              ages.push_back(dataMatrix(i, 0));
              survival.push_back(dataMatrix(i, 1));
              AllSurvival.push_back(dataMatrix(i, 1));
          }

          VectorDouble TestFeatures = LoadTestData<ImageType>(T1CEImagePointer, T2FlairImagePointer, T1ImagePointer, T2ImagePointer,
              RCBVImagePointer, PSRImagePointer, PHImagePointer, AXImagePointer, FAImagePointer, RADImagePointer, TRImagePointer, LabelImagePointer, AtlasImagePointer, TemplateImagePointer, HistogramFeaturesConfigurations);

          FeaturesOfAllSubjects(sid, 0) = ages[0];
          for (unsigned int i = 1; i <= TestFeatures.size(); i++)
              FeaturesOfAllSubjects(sid, i) = TestFeatures[i - 1];
      }
      catch (const std::exception& e1)
      {
          throw std::runtime_error(
                      "Error in calculating the features for patient ID = " +
                      static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_PSR]) +
                      "Error code : " + std::string(e1.what())
                      ); // propagate error message up to algorithm-running logic
      }
  }
  MITK_INFO << std::endl << "Building model.....";
  VariableSizeMatrixType scaledFeatureSet;
  scaledFeatureSet.SetSize(qualifiedsubjects.size(), 161);
  VariableLengthVectorType meanVector;
  VariableLengthVectorType stdVector;
  FeatureScalingClass featureScalingLocalPtr;
  featureScalingLocalPtr.ScaleGivenTrainingFeatures(FeaturesOfAllSubjects, scaledFeatureSet, meanVector, stdVector);

  for (unsigned int i = 0; i < scaledFeatureSet.Rows(); i++)
    for (unsigned int j = 0; j < scaledFeatureSet.Cols(); j++)
      if (std::isnan(scaledFeatureSet(i, j)))
        scaledFeatureSet(i, j) = 0;

// //-----------------------writing in files to compare results------------------------------
  typedef vnl_matrix<double> MatrixType;
  MatrixType data;


  VariableSizeMatrixType SixModelFeatures;
  VariableSizeMatrixType EighteenModelFeatures;
  FeatureExtractionClass featureExtractionLocalPtr;
  featureExtractionLocalPtr.FormulateSurvivalTrainingData(scaledFeatureSet, AllSurvival, SixModelFeatures, EighteenModelFeatures);

  try
  {
      data.set_size(161, 1); // TOCHECK - are these hard coded sizes fine?
      for (unsigned int i = 0; i < meanVector.Size(); i++)
          data(i, 0) = meanVector[i];
      typedef itk::CSVNumericObjectFileWriter<double, 161, 1> WriterTypeVector;
      WriterTypeVector::Pointer writerv = WriterTypeVector::New();
      writerv->SetFileName(outputdirectory + "/Survival_ZScore_Mean.csv");
      writerv->SetInput(&data);
      writerv->Write();

      for (unsigned int i = 0; i < stdVector.Size(); i++)
          data(i, 0) = stdVector[i];
      writerv->SetFileName(outputdirectory + "/Survival_ZScore_Std.csv");
      writerv->SetInput(&data);
      writerv->Write();
  }
  catch (const std::exception& e1)
  {

      throw std::runtime_error(
                  "Error in writing output files to the output directory = " +
                  outputdirectory +
                  "Error code : " + std::string(e1.what())
                  );
  }

//  //---------------------------------------------------------------------------
  VariableSizeMatrixType SixModelSelectedFeatures = SelectSixMonthsModelFeatures(SixModelFeatures);
  VariableSizeMatrixType EighteenModelSelectedFeatures = SelectEighteenMonthsModelFeatures(EighteenModelFeatures);

   try
   {
       trainOpenCVSVM(SixModelSelectedFeatures, outputdirectory + "/" + mSixTrainedFile, false, captk::ApplicationCallingSVM::Survival);
       trainOpenCVSVM(EighteenModelSelectedFeatures, outputdirectory + "/" + mEighteenTrainedFile, false, captk::ApplicationCallingSVM::Survival);
   }
   catch (const std::exception& e1)
   {
     throw std::runtime_error(
                 "Training on the given subjects failed. Error code : " +
                 std::string(e1.what())
                 ); // propagate error message up to algorithm-running logic;
   }
   MITK_INFO << std::endl << "Model saved to the output directory.";
   return true;
}

VectorDouble SurvivalPredictionModuleAlgorithm::SurvivalPredictionOnExistingModel(const std::string &modeldirectory, const std::string& /*inputdirectory*/, const std::vector < std::map < captk::ImageModalityType, std::string>> &qualifiedsubjects, const std::string &outputdirectory)
{
    VariableSizeMatrixType HistogramFeaturesConfigurations;
    HistogramFeaturesConfigurations.SetSize(33, 3); //11 modalities*3 regions = 33 configurations*3 histogram features for each configuration
    VectorDouble results;
    FeatureScalingClass featureScalingLocalPtr;

    CSVFileReaderType::Pointer reader = CSVFileReaderType::New();
    VectorDouble ages;
    MatrixType dataMatrix;
    try
    {
        reader->SetFileName(getApplicationDirectory() + "/models/survival_model/Survival_HMFeatures_Configuration.csv");
        reader->SetFieldDelimiterCharacter(',');
        reader->HasColumnHeadersOff();
        reader->HasRowHeadersOff();
        reader->Parse();
        dataMatrix = reader->GetArray2DDataObject()->GetMatrix();

        for (unsigned int i = 0; i < dataMatrix.rows(); i++)
            for (unsigned int j = 0; j < dataMatrix.cols(); j++)
                HistogramFeaturesConfigurations(i, j) = dataMatrix(i, j);
    }
    catch (const std::exception& e1)
    {
        throw std::runtime_error(
                    "Cannot find the file 'Survival_HMFeatures_Configuration.csv' in the ../models/survival_model directory."
                    "Error code : " + std::string(e1.what())
                    );
    }

    MatrixType meanMatrix;
    VariableLengthVectorType mean;
    VariableLengthVectorType stddevition;
    try
    {
        reader->SetFileName(modeldirectory + "/Survival_ZScore_Mean.csv");
        reader->SetFieldDelimiterCharacter(',');
        reader->HasColumnHeadersOff();
        reader->HasRowHeadersOff();
        reader->Parse();
        meanMatrix = reader->GetArray2DDataObject()->GetMatrix();

        mean.SetSize(meanMatrix.size());
        for (unsigned int i = 0; i < meanMatrix.size(); i++)
            mean[i] = meanMatrix(i, 0);
    }
    catch (const std::exception& e1)
    {
        throw std::runtime_error(
                    "Error in reading the file: "
                    + modeldirectory +
                    "/Survival_ZScore_Mean.csv. Error code : " +
                    std::string(e1.what())
                    ); // propagate the error message to the algorithm-running logic
    }
    MatrixType stdMatrix;
    try
    {
        reader->SetFileName(modeldirectory + "/Survival_ZScore_Std.csv");
        reader->SetFieldDelimiterCharacter(',');
        reader->HasColumnHeadersOff();
        reader->HasRowHeadersOff();
        reader->Parse();
        stdMatrix = reader->GetArray2DDataObject()->GetMatrix();

        stddevition.SetSize(stdMatrix.size());
        for (unsigned int i = 0; i < stdMatrix.size(); i++)
            stddevition[i] = stdMatrix(i, 0);
    }
    catch (const std::exception& e1)
    {
        throw std::runtime_error(
                    "Error in reading the file: " +
                    modeldirectory + "/Survival_ZScore_Std.csv."
                    "Error code : " + std::string(e1.what())
                    );
    }
    //----------------------------------------------------
    VariableSizeMatrixType FeaturesOfAllSubjects;
    FeaturesOfAllSubjects.SetSize(qualifiedsubjects.size(), 161);

    for (unsigned int sid = 0; sid < qualifiedsubjects.size(); sid++)
    {
        MITK_INFO << "Subject No:" << sid;
        std::map<captk::ImageModalityType, std::string> currentsubject = qualifiedsubjects[sid];
        try
        {
            ImageType::Pointer LabelImagePointer = ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_SEG]));
            ImageType::Pointer AtlasImagePointer = ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_ATLAS]));
            ImageType::Pointer TemplateImagePointer = ReadNiftiImage<ImageType>(getApplicationDirectory() + "/models/survival_model/Template.nii.gz");
            ImageType::Pointer RCBVImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_RCBV])));
            ImageType::Pointer PHImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_PH])));
            ImageType::Pointer T1CEImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_T1CE])));
            ImageType::Pointer T2FlairImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_T2FLAIR])));
            ImageType::Pointer T1ImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_T1])));
            ImageType::Pointer T2ImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_T2])));
            ImageType::Pointer AXImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_AX])));
            ImageType::Pointer RADImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_RAD])));
            ImageType::Pointer FAImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_FA])));
            ImageType::Pointer TRImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_TR])));
            ImageType::Pointer PSRImagePointer = RescaleImageIntensity<ImageType>(ReadNiftiImage<ImageType>(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_PSR])));

            VectorDouble TestFeatures = LoadTestData<ImageType>(T1CEImagePointer, T2FlairImagePointer, T1ImagePointer, T2ImagePointer,
                RCBVImagePointer, PSRImagePointer, PHImagePointer, AXImagePointer, FAImagePointer, RADImagePointer, TRImagePointer, LabelImagePointer, AtlasImagePointer, TemplateImagePointer, HistogramFeaturesConfigurations);

            double age;

            reader->SetFileName(static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_FEATURES]));
            reader->SetFieldDelimiterCharacter(',');
            reader->HasColumnHeadersOff();
            reader->HasRowHeadersOff();
            reader->Parse();
            dataMatrix = reader->GetArray2DDataObject()->GetMatrix();

            for (unsigned int i = 0; i < dataMatrix.rows(); i++)
                age = dataMatrix(i, 0);

            FeaturesOfAllSubjects(sid, 0) = age;
            for (unsigned int i = 1; i <= TestFeatures.size(); i++)
                FeaturesOfAllSubjects(sid, i) = TestFeatures[i - 1];
        }
        catch (const std::exception& e1)
        {
            throw std::runtime_error("Error in calculating the features for patient ID = "
                                     + static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_PSR]) +
                                    "Error code : " + std::string(e1.what())
                                    );
        }
    }
    VariableSizeMatrixType ScaledTestingData = featureScalingLocalPtr.ScaleGivenTestingFeatures(FeaturesOfAllSubjects, mean, stddevition);
    for (unsigned int i = 0; i < ScaledTestingData.Rows(); i++)
        for (unsigned int j = 0; j < ScaledTestingData.Cols(); j++)
            if (std::isnan(ScaledTestingData(i, j)))
            ScaledTestingData(i, j) = 0;

    VariableSizeMatrixType ScaledFeatureSetAfterAddingLabel;
    ScaledFeatureSetAfterAddingLabel.SetSize(ScaledTestingData.Rows(), ScaledTestingData.Cols() + 1);
    for (unsigned int i = 0; i < ScaledTestingData.Rows(); i++)
    {
        unsigned int j = 0;
        for (j = 0; j < ScaledTestingData.Cols(); j++)
            ScaledFeatureSetAfterAddingLabel(i, j) = ScaledTestingData(i, j);
        ScaledFeatureSetAfterAddingLabel(i, j) = 0;
    }

    VariableSizeMatrixType SixModelSelectedFeatures = SelectSixMonthsModelFeatures(ScaledFeatureSetAfterAddingLabel);
    VariableSizeMatrixType EighteenModelSelectedFeatures = SelectEighteenMonthsModelFeatures(ScaledFeatureSetAfterAddingLabel);


    try
    {
        std::ofstream myfile;
        myfile.open(outputdirectory + "/results.csv");
        myfile << "SubjectName,SPI (6 months), SPI (18 months), Composite SPI\n";
        std::string modeldirectory1 = getApplicationDirectory() + "/models/survival_model";
        if (fileExists(modeldirectory1 + "/Survival_SVM_Model6.csv") == true && fileExists(modeldirectory1 + "/Survival_SVM_Model18.csv") == true)
        {
            VariableLengthVectorType result_6;
            VariableLengthVectorType result_18;
            result_6 = DistanceFunction(SixModelSelectedFeatures, modeldirectory1 + "/Survival_SVM_Model6.csv", -1.0927, 0.0313);
            result_18 = DistanceFunction(EighteenModelSelectedFeatures, modeldirectory1 + "/Survival_SVM_Model18.csv", -0.2854, 0.5);
            results = CombineEstimates(result_6, result_18);
            for (size_t i = 0; i < results.size(); i++)
            {
                std::map<captk::ImageModalityType, std::string> currentsubject = qualifiedsubjects[i];
                myfile << static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_SUDOID]) + "," + std::to_string(result_6[i]) + "," + std::to_string(result_18[i]) + "," + std::to_string(results[i]) + "\n";
            }
        }
        else if (fileExists(modeldirectory1 + "/Survival_SVM_Model6.xml") == true && fileExists(modeldirectory1 + "/Survival_SVM_Model18.xml") == true)
        {
            VectorDouble result_6;
            VectorDouble result_18;
            result_6 = testOpenCVSVM(ScaledTestingData, modeldirectory1 + "/Survival_SVM_Model6.xml");
            result_18 = testOpenCVSVM(ScaledTestingData, modeldirectory1 + "/Survival_SVM_Model18.xml");
            results = CombineEstimates(result_6, result_18);
            for (size_t i = 0; i < results.size(); i++)
            {
                std::map<captk::ImageModalityType, std::string> currentsubject = qualifiedsubjects[i];
                myfile << static_cast<std::string>(currentsubject[captk::ImageModalityType::IMAGE_TYPE_SUDOID]) + "," + std::to_string(result_6[i]) + "," + std::to_string(result_18[i]) + "," + std::to_string(results[i]) + "\n";
            }
        }
        myfile.close();
    }
    catch (itk::ExceptionObject & excp)
    {
        // propagate error message
        throw std::runtime_error("Error caught during testing: " + std::string(excp.GetDescription()));
    }
    return results;

}

VectorDouble SurvivalPredictionModuleAlgorithm::CombineEstimates(const VariableLengthVectorType &estimates1, const VariableLengthVectorType &estimates2)
{
    VectorDouble returnVec;
    returnVec.resize(estimates1.Size());
    for (size_t i = 0; i < estimates1.Size(); i++)
    {
        float temp_abs, temp_pos1, temp_neg1, temp_1, temp_2;
        // estimate for 1st vector
        if (std::abs(estimates1[i]) < 2)
        {
            temp_abs = estimates1[i];
        }
        else
        {
            temp_abs = 0;
        }

        if (estimates1[i] > 1)
        {
            temp_pos1 = 1;
        }
        else
        {
            temp_pos1 = 0;
        }

        if (estimates1[i] < -1)
        {
            temp_neg1 = 1;
        }
        else
        {
            temp_neg1 = 0;
        }
        temp_1 = temp_abs + (temp_pos1 - temp_neg1);

        // estimate for 2nd vector, all temp values are getting overwritten
        if (std::abs(estimates2[i]) < 2)
        {
            temp_abs = estimates2[i];
        }
        else
        {
            temp_abs = 0;
        }

        if (estimates2[i] > 1)
        {
            temp_pos1 = 1;
        }
        else
        {
            temp_pos1 = 0;
        }

        if (estimates2[i] < -1)
        {
            temp_neg1 = 1;
        }
        else
        {
            temp_neg1 = 0;
        }
        temp_2 = temp_abs + (temp_pos1 - temp_neg1);

        // combine the two
        returnVec[i] = temp_1 + temp_2;
    }
    return returnVec;
}

void SurvivalPredictionModuleAlgorithm::WriteCSVFiles(VariableSizeMatrixType inputdata, std::string filepath)
{
  std::ofstream myfile;
  myfile.open(filepath);
  for (unsigned int index1 = 0; index1 < inputdata.Rows(); index1++)
  {
    for (unsigned int index2 = 0; index2 < inputdata.Cols(); index2++)
    {
      if (index2 == 0)
        myfile << std::to_string(inputdata[index1][index2]);
      else
        myfile << "," << std::to_string(inputdata[index1][index2]);
    }
    myfile << "\n";
  }
}

VariableLengthVectorType SurvivalPredictionModuleAlgorithm::DistanceFunction(const VariableSizeMatrixType &testData, const std::string &filename, const double &rho, const double &bestg)
{
    CSVFileReaderType::Pointer readerMean = CSVFileReaderType::New();
    readerMean->SetFileName(filename);
    readerMean->SetFieldDelimiterCharacter(',');
    readerMean->HasColumnHeadersOff();
    readerMean->HasRowHeadersOff();
    readerMean->Parse();
    MatrixType dataMatrix = readerMean->GetArray2DDataObject()->GetMatrix();

    VariableSizeMatrixType SupportVectors;
    VariableLengthVectorType Coefficients;
    VariableLengthVectorType Distances;

    SupportVectors.SetSize(dataMatrix.rows(), dataMatrix.cols() - 1);
    Coefficients.SetSize(dataMatrix.rows(), 1);
    Distances.SetSize(testData.Rows(), 1);

    for (unsigned int i = 0; i < dataMatrix.rows(); i++)
    {
        unsigned int j = 0;
        for (j = 0; j < dataMatrix.cols() - 1; j++)
            SupportVectors(i, j) = dataMatrix(i, j);
        Coefficients[i] = dataMatrix(i, j);
    }

    for (unsigned int patID = 0; patID < testData.Rows(); patID++)
    {
        double distance = 0;
        for (unsigned int svID = 0; svID < SupportVectors.Rows(); svID++)
        {
            double euclideanDistance = 0;
            for (unsigned int iterator = 0; iterator < SupportVectors.Cols(); iterator++)
                euclideanDistance = euclideanDistance + (SupportVectors(svID, iterator) - testData(patID, iterator))*(SupportVectors(svID, iterator) - testData(patID, iterator));
            double result = std::exp(-1 * bestg*euclideanDistance);
            distance = distance + result*Coefficients[svID];
        }
        Distances[patID] = distance - rho;
    }
    return Distances;
}

VectorDouble SurvivalPredictionModuleAlgorithm::CombineEstimates(
        const VectorDouble &estimates1, const VectorDouble &estimates2)
{
    VectorDouble returnVec;
    returnVec.resize(estimates1.size());
    for (size_t i = 0; i < estimates1.size(); i++)
    {
        float temp_abs, temp_pos1, temp_neg1, temp_1, temp_2;
        // estimate for 1st vector
        if (std::abs(estimates1[i]) < 2)
        {
            temp_abs = estimates1[i];
        }
        else
        {
            temp_abs = 0;
        }

        if (estimates1[i] > 1)
        {
            temp_pos1 = 1;
        }
        else
        {
            temp_pos1 = 0;
        }

        if (estimates1[i] < -1)
        {
            temp_neg1 = 1;
        }
        else
        {
            temp_neg1 = 0;
        }
        temp_1 = temp_abs + (temp_pos1 - temp_neg1);

        // estimate for 2nd vector, all temp values are getting overwritten
        if (std::abs(estimates2[i]) < 2)
        {
            temp_abs = estimates2[i];
        }
        else
        {
            temp_abs = 0;
        }

        if (estimates2[i] > 1)
        {
            temp_pos1 = 1;
        }
        else
        {
            temp_pos1 = 0;
        }

        if (estimates2[i] < -1)
        {
            temp_neg1 = 1;
        }
        else
        {
            temp_neg1 = 0;
        }
        temp_2 = temp_abs + (temp_pos1 - temp_neg1);

        // combine the two
        returnVec[i] = temp_1 + temp_2;
    }
    return returnVec;
}


VariableSizeMatrixType SurvivalPredictionModuleAlgorithm::SelectSixMonthsModelFeatures(const VariableSizeMatrixType &SixModelFeatures)
{
   int selectedFeatures[20] = { 1,    5,    9,    10,    20,    23,    24,    37,    38,    43,    44,    48,    49,    50,    51,    56,    57,    61,    62,    63};

   for (unsigned int i = 0; i <20; i++)
    selectedFeatures[i] = selectedFeatures[i] - 1;
  VariableSizeMatrixType SixModelSelectedFeatures;
  SixModelSelectedFeatures.SetSize(SixModelFeatures.Rows(),21);
  int counter = 0;
  for (unsigned int i = 0; i < 20; i++)
  {
    for (unsigned int j = 0; j < SixModelFeatures.Rows(); j++)
      SixModelSelectedFeatures(j, counter) = SixModelFeatures(j, selectedFeatures[i]);
    counter++;
  }
 for (unsigned int j = 0; j < SixModelFeatures.Rows(); j++)
      SixModelSelectedFeatures(j, 20) = SixModelFeatures(j, 161);

  return SixModelSelectedFeatures;
}

VariableSizeMatrixType SurvivalPredictionModuleAlgorithm::SelectEighteenMonthsModelFeatures(const VariableSizeMatrixType &EighteenModelFeatures)
{
    int selectedFeatures[20] = { 1, 5, 10, 15, 24, 27, 37, 38, 50, 51, 53, 62, 63, 64, 67, 70, 71, 85, 158, 159 };

   for(unsigned int i=0;i<20;i++)
    selectedFeatures[i] = selectedFeatures[i] - 1;

  VariableSizeMatrixType EighteenModelSelectedFeatures;
  EighteenModelSelectedFeatures.SetSize(EighteenModelFeatures.Rows(), 21);
  int counter = 0;
  for (unsigned int i = 0; i < 20; i++)
  {
    for (unsigned int j = 0; j < EighteenModelFeatures.Rows(); j++)
      EighteenModelSelectedFeatures(j, counter) = EighteenModelFeatures(j, selectedFeatures[i]);
    counter++;
  }
  for (unsigned int j = 0; j < EighteenModelFeatures.Rows(); j++)
    EighteenModelSelectedFeatures(j, 20) = EighteenModelFeatures(j, 161);
  return EighteenModelSelectedFeatures;
}



}
