#ifndef SPACEDMASKPOINTSHPP_H
#define SPACEDMASKPOINTSHPP_H

#include <vector>
#include <itkImage.h>

// Template alias ImageType using pixel type (float, int, whatever) and N-dimensionality (this should only really ever be up to N=4...)
template <typename TPixel, unsigned int NDimensional>
using ImageType = itk::Image<TPixel, NDimensional>;

// Template alias IndexType for readability and proper return types
template <typename TPixel, unsigned int NDimensional>
using IndexType = typename ImageType<TPixel, NDimensional>::IndexType;

// Template alias Pointer for taking the proper parameter
template <typename TPixel, unsigned int NDimensional>
using ImagePointer = typename ImageType<TPixel, NDimensional>::Pointer;

template <typename TPixel, unsigned int NDimensional>
using SpacingType = typename ImageType<TPixel, NDimensional>::SpacingType;

template <typename TPixel, unsigned int NDimensional>
using SizeType = typename ImageType<TPixel, NDimensional>::SizeType;

// Templated function to get the desired indices in NDimensional space with pixel type TPixel
template <typename TPixel, unsigned int NDimensional>
std::vector<IndexType<TPixel, NDimensional>> getSpacedPointIndices(ImagePointer<TPixel, NDimensional> mask, const double step_in_millimeters) {
    // Currently this function only takes one "step" parameter (same step along all dimensions). May be better to accept one per dimension of the image.
    // Also, we may need to make the "step_in_mm" a double, or have the same type as ImageType::SpacingValueType...

    const int time_spacing = 1; // Set time (or other non-spatial) spacing to be one unit, e.g. one frame of video
    const unsigned int dims = NDimensional;

    // make our lives easier with aliasing
    using SpacingType = SpacingType<TPixel, NDimensional>;
    using SizeType = SizeType<TPixel, NDimensional>;
    using IndexType = IndexType<TPixel, NDimensional>;

    const SpacingType& spacing_between_pixels = mask->GetSpacing(); // physical spacing along each axis (units are machine and application-dependent)
    // note: this function assumes spacing in mm

    // calculate the required pixel-wise spacing
    SpacingType px_per_step;
    for (unsigned int current_dim = 0; current_dim < dims; current_dim++) {
        if (current_dim < 3) { // spatial dimensions
            if (step_in_millimeters >= spacing_between_pixels[current_dim]) {
                px_per_step[current_dim] = step_in_millimeters / spacing_between_pixels[current_dim]; // calculate spatial px/step ratio
            } else { // the resolution along this dimension is too low -- every index along this dimension is selected
                px_per_step[current_dim] = 1;
            }
        }
        else { // time dimension, or some other dimension, for which physical spacing has no meaning
            px_per_step[current_dim] = time_spacing;
        }
    }

    std::vector<IndexType> result_vector;

    SizeType mask_size = mask->GetLargestPossibleRegion().GetSize(); // Size of image in each dimension is used to calculate bounding
    std::vector<std::vector<int>> all_valid_index_sets; // initialize storage for spaced index lists for each dimension

    for (unsigned int current_dim = 0; current_dim < dims; current_dim++) {
        std::vector<int> valid_indices_along_current_dimension; // initialize vector to store spaced indices along one dimension

        for (double cumulative_index_by_px = 0.0; cumulative_index_by_px < mask_size[current_dim]; cumulative_index_by_px += px_per_step[current_dim]) {
                valid_indices_along_current_dimension.push_back(int(cumulative_index_by_px));
        }
        all_valid_index_sets.push_back(valid_indices_along_current_dimension);
    }

    int total_points = 1; // initialize for multiplicative counting
    for (unsigned int current_dim = 0; current_dim < dims; current_dim++) {
        total_points *= all_valid_index_sets[current_dim].size();
    }
    for (int i = 0; i<total_points; i++) {
        IndexType this_point;
        /* Explanation for why/how this works >>
         * The below works by using the numerical indices as divisors. if X axis has 5 valid spaced positions,
         * and Y has 3, and Z has 6, the total number of valid point indices is 5*3*6 = 90 total valid, unique points (which we calculated above).
         * "Selecting" any valid position along X means there are 90/5 = 18 = 3*6 valid points with that specific X-position.
         * Using modulus "selects" a position in the current range based on the unique point's corresponding number. 90%5 = 0, 89%5 = 1, 88%5 = 3...
         *
         * So for each point, we start with i, or its place in the set of all valid points.
         * "factor" tracks our current factor as we go over each dimension (getting divided as we "select" a position for each dimension).
         * For each dimension, we take "n_index" to be the remainder (modulus) of the division of our current factor
         * by the amount of valid positions in the dimension we are currently considering.
         * This value "n_index" acts as our index into the current dimension to find what that dimension's n-th valid spaced position is.
         * The newly-divided "factor" is then used in the same way to find the remainder/index for the next dimension, and so on.
        */
        auto factor = i;
        for (unsigned int dim = 0; dim < all_valid_index_sets.size(); dim++) {
            std::vector<int> const positions = all_valid_index_sets[dim];
            auto n_index = factor % positions.size();
            factor /= positions.size();
            this_point[dim] = positions[n_index];
        }
        result_vector.push_back(this_point);
    }

    return result_vector; // currently returns a vector. Could be improved to return an iterator of some sort

}

#endif
