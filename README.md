# Image Storage Estimator

Programming assignment by Sectra. Programmed in C++11 by Denny Lindberg using Visual Studio 2017 and Premake5.



## Generate solution files using Premake5

[Premake5](https://premake.github.io/) is a build configuration tool contained in a single exe file. Open the root folder, paste the exe and run any of the following commands to generate a solution file in the Temporary folder. 

* premake5 vs2017
* premake5 vs2016
* premake5 vs2015
* premake5 gmake
* premake5 gmake2

The executable is generated in the Binaries folder.



## Assumptions and additional functionality
I assume that images or groups should not be removable according to the requirements. (it can however be easily implemented in the current structure)

I took the liberty of making the program output the current image/stack structure whenever a change is made. This includes individual storage sizes, compressions and total size. The command Q thus shuts down the program instead of printing the results.

I also took the liberty of allowing JPEG as a variant input for J and JPG.

The program will print warnings whenever a faulty input is made. 

I allow both upper and lower case characters. 

I allow 2 or more images to form stacks. I also allow already grouped images to be regrouped inside new stacks. Stacks are removed automatically if images are moved and a stack ends up empty.



## Comments regarding code structure and design

Classes are implemented using polymorphism, with inheritance and virtual functions, to help maintainability and support future extension of functionality. (for example new image types or even totally different file types)

I have attempted to be as descriptive as possible using class-, function- and variable names. This is to challenge myself to ensure that the code expresses its intent. I have thus kept comments minimal to let you focus on the code and not to clutter the files. Please note, I am well aware that decent documentation of production code is necessary for maintainability and I do actively write comments for tools like Doxygen in bigger projects.

I use namespaces to avoid redundant names like ImageJPEG2000StorageEstimator and ImageStackStorageEstimator. Instead these examples become StorageEstimator::Image::JPEG and StorageEstimator::Image::Stack, which by 'using namespace' become Image::JPEG and Image::Stack. The namespaces also allow me to typedef variables and enums with decent names, such as Image::Type (enum) and Image::Dimension (unsigned int).

Typedefs are used to ensure that variable types are consistent between classes and functions, and to minimize human error. It also helps convey meaning to the variables.

I use shared_ptr to implicitely keep track of and delete allocated objects when they go out of scope. It is hidden underneath typedefs, and helper functions, to ease usage and maintainability.

Enums are used to improve readability of code and to improve execution performance by using switches. It also minimizes potential human errors, which can happen when ints or strings are compared.

The Image::Type enum together with the MakeSharedPtrByType  function allows the code to be quickly extended with new image types. This allows all internal states to be evaluated using the enum type rather than passing arbitrary values around. To add a new image type, it is done by adding a new class to ImageVariants, adding a new Image::Type value which matches the class and to update the enum helper functions. By then adding a new switch entry for class instance allocation in MakeSharedPtrByType, the user can use the new type in the application. 

I use iterators extensively to allow usage of STL-functions to avoid repetitive use of similar for loops and to ease std::vector manipulation. The iterators could have easily been replaced by id-based lookups, but that would both increase code size and probably degrade maintainability. The only downsides of iterators are potential degradation in performance, which is not an issue here, and that they cause crashes if passed to incompatible vectors, which is defined behavior.