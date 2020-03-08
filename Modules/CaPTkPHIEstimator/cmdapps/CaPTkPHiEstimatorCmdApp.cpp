/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include <mitkCommandLineParser.h>
#include <mitkIOUtil.h>
#include <mitkImageToItk.h>
#include <mitkLabelSetImage.h>
#include <mitkImageAccessByItk.h>

#include <EGFRvIIISurrogateIndex.h>
#include <PHIEstimator.h>

#include "itkCastImageFilter.h"
#include <mitkImageCast.h>

#include <algorithm>
#include <string>

/** \brief PHI Estimator command-line app
 */

int main(int argc, char* argv[])
{
	mitkCommandLineParser parser;

	// Set general information about your command-line app
	parser.setCategory("CaPTk Cmd App Category");
	parser.setTitle("CaPTk PHI Estimator Cmd App");
	parser.setContributor("CBICA");
	parser.setDescription(
		"This command-line app takes the given perfusion image and a near_far mask and calculates the PHI Value.\n \
Example Usage : CaPTkPHiEstimatorCmdApp.exe - i C:/DSC-MRI_data.nii.gz -m C:/Near_Far_masks.nii.gz -v");

	// How should arguments be prefixed
	parser.setArgumentPrefix("--", "-");

	// Add arguments. Unless specified otherwise, each argument is optional.
	// See mitkCommandLineParser::addArgument() for more information.
	parser.beginGroup("Required parameters"); // required parameters
	parser.addArgument(
		"input",
		"i",
		mitkCommandLineParser::String,
		"Input Perfusion image path",
		"Path to the input perfusion image.",
		us::Any(),
		false); // false -> required parameter

	parser.addArgument(
		"mask",
		"m",
		mitkCommandLineParser::String,
		"Near Far Mask Image",
		"Path to mask image, with near(1) and far(2) label.",
		us::Any(),
		false);
	parser.endGroup();

	parser.beginGroup("Optional parameters"); // optional parameters
	parser.addArgument( // optional
		"verbose",
		"v",
		mitkCommandLineParser::Bool,
		"Verbose Output",
		"Whether to produce verbose output.");
	parser.endGroup();

	// Parse arguments. This method returns a mapping of long argument names to
	// their values.
	auto parsedArgs = parser.parseArguments(argc, argv);

	if (parsedArgs.empty())
		return EXIT_FAILURE; // Just exit, usage information was already printed.

	if (parsedArgs["input"].Empty() || parsedArgs["mask"].Empty())
	{
		MITK_INFO << parser.helpText();
		return EXIT_FAILURE;
	}

	// Parse, cast and set required arguments
	auto inFilename = us::any_cast<std::string>(parsedArgs["input"]);
	auto maskFilename = us::any_cast<std::string>(parsedArgs["mask"]);

	// Default values for optional arguments
	auto verbose = false;

	// Parse, cast and set optional arguments
	if (parsedArgs.end() != parsedArgs.find("verbose"))
		verbose = us::any_cast<bool>(parsedArgs["verbose"]);

	try
	{
		if (verbose)
			MITK_INFO << "Read input file";


		// read input perfusion image
		auto inImage = mitk::IOUtil::Load<mitk::Image>(inFilename);

		// exit if image could not be read
		if (inImage.IsNull())
		{
			MITK_ERROR << "Could not read \"" << inFilename << "\"!";
			return EXIT_FAILURE;
		}

		if (verbose)
			MITK_INFO << "Read mask file";

		// read near far mask
		mitk::LabelSetImage::Pointer maskImage = mitk::LabelSetImage::New();

		// read mask image - this is temporary needed to initialize the label set image
		mitk::Image::Pointer maskImageTmp =	mitk::IOUtil::Load<mitk::Image>(maskFilename);

		// initialize labelset image by read mask image
		maskImage->InitializeByLabeledImage(maskImageTmp); 

		// we no longer need the mask image as we have converted it to
		// label set image
		maskImageTmp->Delete();

		// exit if mask could not be read
		if (maskImage.IsNull())
		{
			MITK_ERROR << "Could not read \"" << maskFilename << "\"!";
			return EXIT_FAILURE;
		}

		if (verbose)
			MITK_INFO << "Run PHI Estimation";

		if (verbose)
			MITK_INFO << "Check input image dimensions";
		
		if (inImage->GetDimension() != 4)
		{
			MITK_ERROR << "Input Image must be 4D only" << "\"!";
			return EXIT_FAILURE;
		}

		if (verbose)
			MITK_INFO << "Check input mask dimensions";
		
		if (maskImage->GetDimension() != 3)
		{
			MITK_ERROR << "Mask Image must be 3D only" << "\"!";
			return EXIT_FAILURE;
		}

		std::vector<double> EGFRStatusParams;
		using MaskImageType = itk::Image<float, 3>;
		std::vector<MaskImageType::IndexType> nearIndices, farIndices;
		AccessFixedDimensionByItk_n(inImage.GetPointer(), //input perfusion image
			captk::PhiEstimator::Run, //templated function to call
			4,						  //templated function handles fixed dimension(4) images
			(maskImage,				  //mask image passed to called function as parameter
				nearIndices,		  //indices returned as reference from called function
				farIndices,			  //indices returned as reference from called function
				EGFRStatusParams)	  //result of PHI Estimation returned from called function
		);

		if (verbose)
			MITK_INFO << "printing output";

		float phiThreshold = 0.1377;//this is a hard value

		std::string tumorType;
		//! if phi value less than threshold, then tumor type is mutant else wildtype
		if (EGFRStatusParams[0] < phiThreshold) // threshold = 0.1377 
			tumorType = "EGFRvIII-Mutant";
		else
			tumorType = "EGFRvIII-Wildtype";
		
		//log output
		MITK_INFO << " PHI Value = " << EGFRStatusParams[0] << std::endl;
		MITK_INFO << " (Near:Far)Peak Height Ratio = " << EGFRStatusParams[1] / EGFRStatusParams[2] << std::endl;
		MITK_INFO << " # Near Voxels Used = " << EGFRStatusParams[3] << "/" << nearIndices.size() << std::endl;
		MITK_INFO << " # far voxels Used = " << EGFRStatusParams[4] << "/" << farIndices.size() << std::endl;
		MITK_INFO << " PHI Threshold(based on 142 UPenn brain tumor scans) = " << std::fixed << setprecision(4) << phiThreshold << std::endl;
		MITK_INFO << " Based on this threshold and resulting PHI value the tumor type is = " << tumorType << std::endl;
	}
	catch (const std::exception &e)
	{
		MITK_ERROR << e.what();
		return EXIT_FAILURE;
	}
	catch (...)
	{
		MITK_ERROR << "Unexpected error!";
		return EXIT_FAILURE;
	}

	MITK_INFO << "Done";
	return EXIT_SUCCESS;
}
