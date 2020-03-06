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
#include <mitkImageAccessByItk.h>

#include <EGFRvIIISurrogateIndex.h>
#include <EGFRStatusPredictor.h>
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
		"This command-line app takes the given perfusion image and a near_far mask and calculates the PHI Value.");

	// How should arguments be prefixed
	parser.setArgumentPrefix("--", "-");

	// Add arguments. Unless specified otherwise, each argument is optional.
	// See mitkCommandLineParser::addArgument() for more information.
	parser.addArgument(
		"input",
		"i",
		mitkCommandLineParser::String,
		"Input Perfusion image path",
		"Path to the input perfusion image",
		us::Any(),
		false); // false -> required parameter

	parser.addArgument(
		"mask",
		"m",
		mitkCommandLineParser::String,
		"Near Far Mask Image",
		"Path to mask image.",
		us::Any(),
		false);

	parser.addArgument( // optional
		"verbose",
		"v",
		mitkCommandLineParser::Bool,
		"Verbose Output",
		"Whether to produce verbose output");

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
		auto maskImage = mitk::IOUtil::Load<mitk::Image>(maskFilename);

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
		AccessFixedDimensionByItk_n(inImage.GetPointer(), captk::PhiEstimator::Run, 4, (maskImage, nearIndices, farIndices, EGFRStatusParams));

		if (verbose)
			MITK_INFO << "printing output";

		float phiThreshold = 0.1377;//this is a hard value

		std::string tumorType;
		//! if phi value less than threshold, then tumor type is mutant else wildtype
		if (EGFRStatusParams[0] < phiThreshold) // threshold = 0.1377 
			tumorType = "EGFRvIII-Mutant";
		else
			tumorType = "EGFRvIII-Wildtype";
		
		// print output
		std::cout << " PHI Value = " << EGFRStatusParams[0] << std::endl;
		std::cout << " (Near:Far)Peak Height Ratio = " << EGFRStatusParams[1] / EGFRStatusParams[2] << std::endl;
		std::cout << " # Near Voxels Used = " << EGFRStatusParams[3] << "/" << nearIndices.size() << std::endl;
		std::cout << " # far voxels Used = " << EGFRStatusParams[4] << "/" << farIndices.size() << std::endl;
		std::cout << " PHI Threshold(based on 142 UPenn brain tumor scans) = " << std::fixed << setprecision(4) << phiThreshold << std::endl;
		std::cout << "Based on this threshold and resulting PHI value the tumor type is = " << tumorType << std::endl;

		//log output
		MITK_INFO << " PHI Value = " << EGFRStatusParams[0] << std::endl;
		MITK_INFO << " (Near:Far)Peak Height Ratio = " << EGFRStatusParams[1] / EGFRStatusParams[2] << std::endl;
		MITK_INFO << " # Near Voxels Used = " << EGFRStatusParams[3] << "/" << nearIndices.size() << std::endl;
		MITK_INFO << " # far voxels Used = " << EGFRStatusParams[4] << "/" << farIndices.size() << std::endl;
		MITK_INFO << " PHI Threshold(based on 142 UPenn brain tumor scans) = " << std::fixed << setprecision(4) << phiThreshold << std::endl;
		MITK_INFO << "Based on this threshold and resulting PHI value the tumor type is = " << tumorType << std::endl;
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
