#### Description
The C++ Bitmap Library for **8-bit per pixel** bitmap
image format.

#### Capabilities
+ Read/Write 8-bit Bitmap Images
+ Apply grayscale
+ Apply 3*3 filter on images
+ Apply threshold on images (between 0 and 1)

```c++
    explicit BitMap(const std::string& filename); //loading
    void save(const std::string& filename); //saving
    void applyThreshold(const float &threshold); // threshold
    void applyGreyScale(); // greyscale
    void applyFilter(const std::vector<int> &filter); // applyfilter 3x3
    void applyFilterFromFile(const std::string& filename); // from file.
```

Compatible
+ Linux
+ OSX
+ Windows (not compiled so far)

```c++
#include "bitmap.h"
#include <iostream>

int main(int argc, char *argv[])
{
    std::cout << argc << "\n";
    bool run = (argc > 1);
    if(!run)
    {
        std::cout << "No image path found" << "\n";
        return 0;
    }

    std::string filename = argv[1];
    try
    {
        BitMap image(filename);
        std::cout << "Image is loaded" << "\n";

        image.printHeaders();
        bool exit  = false;
        while (!exit) {
           char action;
           float threshold;
           std::string filter_filename, dummy;
           std::cout << "Enter a number to perfown following tasks." << "\n";
           std::cout << "1. Apply threadhold" << "\n";
           std::cout << "2. Apply filter from file" << "\n";
           std::cout << "3. Save in the same directory as output.bmp" << "\n";
           std::cout << "4. To exit." << "\n";

           std::cin >> action;
           std::getline(std::cin,dummy);

           switch (action) {
           case '1':
               std::cout << "Enter value between 0 and 1" << "\n";
               std::cin >> threshold;
               if(threshold >= 0 && threshold <= 1.0)
               {
                   image.applyThreshold(threshold);
               }
               else
               {
                    std::cout << "threshold must be between 0 and 1";
               }

               std::cout << "Threshold applied" << "\n\n";
               break;

           case '2':
               std::cout << "Enter complete path of the matrix file." << "\n";
               std::getline(std::cin, filter_filename);
               image.applyFilterFromFile(filter_filename);
               std::cout << "Filter applied" << "\n\n";
               break;

           case '3':
               image.save("ouput.bmp");
               std::cout << "Saved applied" << "\n\n";
               break;
           case '4':
               exit = true;
               break;
           default:
               break;
           }


        }

    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << "\n";
        run = false;
    }
    catch (...)
    {
        std::cout << "ERROR_OCCURED" << "\n";
        run = false;
    }

}

```

