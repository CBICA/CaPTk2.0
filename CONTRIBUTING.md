Thanks for your interest in contributing to CaPTk! 🎉

The following guidelines will help keep CaPTk maintainable and useful. We understand that they might not always be easy to follow, so we welcome you to use your best judgment when interpreting them.

# Contents

- [Opening issues if you are not a developer](#Opening-issues-if-you-are-not-a-developer)
- [Handling issues if you are a developer](#Handling-issues-if-you-are-a-developer)
- [Code contributions](#Code-contributions)
- [Pull requests](#Pull-requests)

## Opening issues if you are not a developer

- Create a github account if you don't have one!
- Take a moment to look for past issues that might be identical to yours, using the search bar. There might be discussion or solution about the problem you faced.
- Please open a separate issue for each problem. That makes solving them and keeping track of things easier.
- Include a clear description of the problem.
- Include steps to reproduce the problem.
- Include screenshots if applicable.
- Provide a short description of your computer configuration and what is relevant (OS+version, version of CaPTk you are using, RAM etc)
- If the data you used to reach the problematic state can be shared, please do share them! It will help a lot. Please only share anonymized data, never share data containing [Protected Health Information](https://en.wikipedia.org/wiki/Protected_health_information).

## Handling issues if you are a developer

- Make sure to assist the person opening the issue, and guide him through giving a clear description / steps to reproduce.
- Label the issue based on severity. In ascending order, we use 4 levels: Trivial, Minor, Major, Critical.
- Prioritize solving critical issues.
- If you know the person that usually handles the relevant part of the code, assign them to the issue.
- If the issue is a feature request, label it with "Enhancement".
- If an issue is a duplicate, label it as "Duplicate" and close it.

## Code contributions

- Please see [[the wiki]](https://github.com/CBICA/CaPTk2.0/wiki/Developer-Guide) for instructions on how to build.
- Please refrain from writing lines using more than ~90 characters.
- Comment/comment/comment your code
- Document your functions & classes using [Doxygen](http://www.doxygen.nl/manual/docblocks.html)
- Write documentation for your plugins, in the file ```PLUGINDIR/documentation/UserManual/${YourPluginName}Plugin.dox``` 
- Each function should do one thing (generally speaking)
- Please do calculations that last > 1 second in background threads, to not hang the GUI.
- Namespace your modules (and not plugins) classes/functions with the ```captk``` namespace.
- One file per class. 
- If you develop the class ```captk::ExampleAlgorithm```, it should be defined in a file called CaPTkExampleAlgorithm.h
- Separate implementations from definitions.
- Don't include new dependencies unless they are absolutely needed.
- All test names should start with "CaPTk". That way, if ```BUILD_TESTING``` is enabled in CMake, it is possible to run only CaPTk tests, and not MITK (superbuild) ones, by running ```make test ARGS="-R CaPTk"``` in the build directory.

## Pull requests

- Strive to create small and easy-to-review PRs. 
- One issue per PR (unless there is a connection between 2+ issues)
- Don't fix random things you see at random places that are outside the scope of your PR. Alternatively, make a note of them and open a separate PR later with fixes for those, or open an issue about them.
- At least one approval is required before merging your changes.
- Write the issue you are solving at the description (i.e. first comment) of your PR. That will also close the issue automatically when your PR gets merged.
