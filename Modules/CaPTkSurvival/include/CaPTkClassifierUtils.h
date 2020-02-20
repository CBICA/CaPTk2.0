#pragma once

#include "CaPTkDefines.h"
#include "CaPTkEnums.h"
#include "opencv2/core/core.hpp"
#include "opencv2/ml.hpp"
/**
\brief Train and save the SVM classifier

\param trainingDataAndLabels Input training data with last column as training labels
\param outputModelName File to save the model
*/
inline VectorDouble trainOpenCVSVM(const VariableSizeMatrixType &trainingDataAndLabels, const std::string &outputModelName, bool considerWeights, int ApplicationCallingSVM)
{
  cv::Mat trainingData = cv::Mat::zeros(trainingDataAndLabels.Rows(), trainingDataAndLabels.Cols() - 1, CV_32FC1),
    trainingLabels = cv::Mat::zeros(trainingDataAndLabels.Rows(), 1, CV_32FC1);


  cv::Mat trainingWeights = cv::Mat::zeros(2, 1, CV_32FC1);
  size_t label1Counter = 0, label2Counter = 0;

  // fast cv::Mat access 
  for (unsigned int i = 0; i < trainingDataAndLabels.Rows(); ++i)
  {
    for (unsigned int j = 0; j < trainingDataAndLabels.Cols() - 1; ++j)
    {
      trainingData.ptr< float >(i)[j] = trainingDataAndLabels(i, j);
    }

    // last column consists of labels
    trainingLabels.ptr< float >(i)[0] = trainingDataAndLabels(i, trainingDataAndLabels.Cols() - 1);
    if (considerWeights)
    {
      // start counter to assign weights
      if (trainingLabels.ptr< float >(i)[0] == 0)
      {
        label1Counter++;
      }
      else
      {
        label2Counter++;
      }
    }
  }

  trainingLabels.convertTo(trainingLabels, CV_32SC1);

  auto svm = cv::ml::SVM::create();
  svm->setType(cv::ml::SVM::C_SVC);
  //svm->setC(1);
  //svm->setGamma(0.01);
  // parameters for auto_train
  cv::ml::ParamGrid grid_C(-5, 5, 2); // Parameter C of a SVM optimization problem (C_SVC / EPS_SVR / NU_SVR)
  cv::ml::ParamGrid grid_Gamma(-5, 5, 2); // Parameter \gamma of a kernel function (POLY / RBF / SIGMOID / CHI2)
  cv::ml::ParamGrid grid_P(-5, 5, 2); // Parameter \epsilon of a SVM optimization problem (EPS_SVR)
  cv::ml::ParamGrid grid_Nu(-5, 5, 2); // Parameter \nu of a SVM optimization problem (NU_SVC / ONE_CLASS / NU_SVR)
  cv::ml::ParamGrid grid_Degree(0, 5, 1); // Parameter degree of a kernel function (POLY)
  cv::ml::ParamGrid grid_Coeff0(-5, 5, 2); // this is Parameter coef0 of a kernel function (POLY / SIGMOID)
  if (ApplicationCallingSVM == captk::ApplicationCallingSVM::Recurrence)
  {
    svm->setKernel(cv::ml::SVM::RBF); // using this produces terrible results for recurrence
  }
  else
  {
    svm->setKernel(cv::ml::SVM::LINEAR);
  }
  svm->setC(1);
  svm->setGamma(0.01);

  bool res = true;
  std::string msg;

  try
  {
    res = svm->train(trainingData, cv::ml::ROW_SAMPLE, trainingLabels);
  }
  catch (cv::Exception ex)
  {
    msg = ex.what();
  }
  //---------------------------------find distances on training adat----------------------------------
  VectorDouble returnVec;
  returnVec.resize(trainingData.rows);
  cv::Mat predicted(1, 1, CV_32F); // not sure if this is required if we do train_auto for cross-validation
  if (ApplicationCallingSVM == captk::ApplicationCallingSVM::Recurrence) // only recurrence needs the distance map
  {
    for (int i = 0; i < trainingData.rows; i++)
    {
      cv::Mat sample = trainingData.row(i);
      svm->predict(sample, predicted, true);
      returnVec[i] = predicted.ptr< float >(0)[0];
    }
  }
  else
  {
    returnVec.push_back(1.0); // just so survival application doesn't thrown an error
  }
  //--------------------------------------------------------------------------------------------------
  if (res)
  {

    svm->save(outputModelName);
  }
  else
  {

  }

  return returnVec;
}

/**
\brief Load the SVM classifier and get the distances from the hyperplane

\param testingData Input training data with last column as training labels
\param inputModelName File to save the model
\return Distances of classification
*/
inline VectorDouble testOpenCVSVM(const VariableSizeMatrixType &testingData, const std::string &inputModelName)
{
  auto svm = cv::Algorithm::load<cv::ml::SVM>(inputModelName);

  VectorDouble returnVec;
  cv::Mat testingDataMat = cv::Mat::zeros(testingData.Rows(), testingData.Cols() - 1, CV_32FC1), outputProbs;

  // fast cv::Mat access
  for (unsigned int i = 0; i < testingData.Rows(); ++i)
  {
    for (unsigned int j = 0; j < testingData.Cols(); ++j)
    {
      testingDataMat.ptr< float >(i)[j] = testingData(i, j);
    }
  }


  returnVec.resize(testingDataMat.rows);
  cv::Mat predicted(1, 1, CV_32F);
  for (int i = 0; i < testingDataMat.rows; i++)
  {
    cv::Mat sample = testingDataMat.row(i);
    svm->predict(sample, predicted, true);
    returnVec[i] = predicted.ptr< float >(0)[0];
  }


  return returnVec;
}


