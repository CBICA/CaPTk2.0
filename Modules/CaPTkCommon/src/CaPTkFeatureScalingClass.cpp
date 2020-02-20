/**
\file  FeatureScalingClass.h

\brief Implementation of the FeatureScalingClass

https://www.med.upenn.edu/sbia/software/ <br>
software@cbica.upenn.edu

Copyright (c) 2018 University of Pennsylvania. All rights reserved. <br>
See COPYING file or https://www.med.upenn.edu/sbia/software-agreement.html

*/

#include "CaPTkFeatureScalingClass.h"

#include <vtkVersion.h>
#include "vtkSmartPointer.h"
#include "vtkDoubleArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkPCAStatistics.h"
#include "vtkStringArray.h"
#include "vtkTable.h"

captk::FeatureScalingClass::FeatureScalingClass()
{

}

captk::FeatureScalingClass::~FeatureScalingClass()
{
  mMeanVector.SetSize(0);
  mStdVector.SetSize(0);
}


captk::FeatureScalingClass::VariableSizeMatrixType captk::FeatureScalingClass::ScaleGivenTrainingFeatures(const captk::FeatureScalingClass::VariableSizeMatrixType &inputdata)
{
  unsigned int NumberOfSamples = inputdata.Rows();
  unsigned int NumberOfFeatures = inputdata.Cols() - 1;

  //---------calculate mean and variance for each feature-----------------------------
  mMeanVector.SetSize(NumberOfFeatures);
  mStdVector.SetSize(NumberOfFeatures);
  for (unsigned int featureNo = 0; featureNo < NumberOfFeatures; featureNo++)
  {
    double temp = 0.0;
    for (unsigned int sampleNo = 0; sampleNo < NumberOfSamples; sampleNo++)
      temp += inputdata(sampleNo, featureNo);
    
    mMeanVector[featureNo] = temp / NumberOfSamples;
    double mean = mMeanVector[featureNo];
    temp = 0.0;
    for (unsigned int sampleNo = 0; sampleNo < NumberOfSamples; sampleNo++)
      temp += (inputdata(sampleNo, featureNo) - mean)*(inputdata(sampleNo, featureNo) - mean);
    
    mStdVector[featureNo] = std::sqrt(temp / (NumberOfSamples - 1));
  }
  //---------calculate z-score for each feature value-----------------------------
  captk::FeatureScalingClass::VariableSizeMatrixType scaledFeatureSet;
  scaledFeatureSet.SetSize(NumberOfSamples, NumberOfFeatures + 1); //+1 to score the actual label

  for (unsigned int featureNo = 0; featureNo < NumberOfFeatures; featureNo++)
  {
    for (unsigned int sampleNo = 0; sampleNo < NumberOfSamples; sampleNo++)
      scaledFeatureSet(sampleNo, featureNo) = GetZScore(mMeanVector[featureNo], mStdVector[featureNo], inputdata(sampleNo, featureNo));
  }
  //---------------copy label to each sample----------------------------------------
  for (unsigned int sampleNo = 0; sampleNo < NumberOfSamples; sampleNo++)
    scaledFeatureSet(sampleNo, NumberOfFeatures) = inputdata(sampleNo, NumberOfFeatures);

  return scaledFeatureSet;
}

captk::FeatureScalingClass::VariableSizeMatrixType captk::FeatureScalingClass::ScaleGivenTestingFeatures(const captk::FeatureScalingClass::VariableSizeMatrixType &inputdata)
{
  unsigned int  NumberOfSamples = inputdata.Rows();
  unsigned int  NumberOfFeatures = inputdata.Cols() - 1;

  //---------calculate z-score for each feature value-----------------------------
  captk::FeatureScalingClass::VariableSizeMatrixType scaledFeatureSet;
  scaledFeatureSet.SetSize(NumberOfSamples, NumberOfFeatures + 1); //+1 to score the actual label

  for (unsigned int featureNo = 0; featureNo < NumberOfFeatures; featureNo++)
  {
    for (unsigned int sampleNo = 0; sampleNo < NumberOfSamples; sampleNo++)
      scaledFeatureSet(sampleNo, featureNo) = GetZScore(mMeanVector[featureNo], mStdVector[featureNo], inputdata(sampleNo, featureNo));
  }
  //---------------copy label to each sample----------------------------------------
  for (unsigned int sampleNo = 0; sampleNo < NumberOfSamples; sampleNo++)
    scaledFeatureSet(sampleNo, NumberOfFeatures) = inputdata(sampleNo, NumberOfFeatures);
  
  return scaledFeatureSet;
}
double captk::FeatureScalingClass::GetZScore(const double &mean, const double &variance, const double &featureval)
{
  return (featureval - mean) / (variance + 0.0);
}
void captk::FeatureScalingClass::ScaleGivenTrainingFeatures(const captk::FeatureScalingClass::VariableSizeMatrixType &inputdata, captk::FeatureScalingClass::VariableSizeMatrixType &scaledFeatureSet, VariableLengthVectorType &meanVector, VariableLengthVectorType &stdVector)
{
  int NumberOfSamples = inputdata.Rows();
  int NumberOfFeatures = inputdata.Cols();

  //---------calculate mean and variance for each feature----------------
  meanVector.SetSize(NumberOfFeatures);
  stdVector.SetSize(NumberOfFeatures);
  for (int featureNo = 0; featureNo < NumberOfFeatures; featureNo++)
  {
    double temp = 0.0;
    for (int sampleNo = 0; sampleNo < NumberOfSamples; sampleNo++)
      temp = temp + inputdata(sampleNo, featureNo);
    meanVector[featureNo] = temp / NumberOfSamples;
    double mean = meanVector[featureNo];
    temp = 0.0;
    for (int sampleNo = 0; sampleNo < NumberOfSamples; sampleNo++)
      temp = temp + (inputdata(sampleNo, featureNo) - mean)*(inputdata(sampleNo, featureNo) - mean);
    double std = std::sqrt(temp / (NumberOfSamples - 1));
    stdVector[featureNo] = std;
  }
  //---------calculate z-score for each feature value-----------------------------
  scaledFeatureSet.SetSize(NumberOfSamples, NumberOfFeatures);
  for (int featureNo = 0; featureNo < NumberOfFeatures; featureNo++)
  {
    for (int sampleNo = 0; sampleNo < NumberOfSamples; sampleNo++)
      scaledFeatureSet(sampleNo, featureNo) = GetZScore(meanVector[featureNo], stdVector[featureNo], inputdata(sampleNo, featureNo));
  }
}
captk::FeatureScalingClass::VariableSizeMatrixType captk::FeatureScalingClass::ScaleGivenTestingFeatures(const captk::FeatureScalingClass::VariableSizeMatrixType &inputdata, const VariableLengthVectorType &meandata, const VariableLengthVectorType &stddata)
{
  int NumberOfSamples = inputdata.Rows();
  int NumberOfFeatures = inputdata.Cols();

  //---------calculate z-score for each feature value-----------------------------
  captk::FeatureScalingClass::VariableSizeMatrixType scaledFeatureSet;
  scaledFeatureSet.SetSize(NumberOfSamples, NumberOfFeatures); //+1 to score the actual label

  for (int featureNo = 0; featureNo < NumberOfFeatures; featureNo++)
  {
    for (int sampleNo = 0; sampleNo < NumberOfSamples; sampleNo++)
      scaledFeatureSet(sampleNo, featureNo) = GetZScore(meandata[featureNo], stddata[featureNo], inputdata(sampleNo, featureNo));
  }


  return scaledFeatureSet;
}
