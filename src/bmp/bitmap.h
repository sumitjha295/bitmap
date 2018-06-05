#ifndef BITMAP_H
#define BITMAP_H
#include <string>
#include <vector>

// for Linux platform, plz make sure the size of data type is correct for BMP spec.
// if you use this on Windows or other platforms, plz pay attention to this.

typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;
// __attribute__((packed)) on non-Intel arch may cause some unexpected error, plz be informed.

typedef struct tagBITMAPFILEHEADER
{
    WORD    bfType; // 2  /* Magic identifier */
    DWORD   bfSize; // 4  /* File size in bytes */
    WORD    bfReserved1; // 2
    WORD    bfReserved2; // 2
    DWORD   bfOffBits; // 4 /* Offset to image data, bytes */
} __attribute__((packed)) BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{

    DWORD    biSize; // 4 /* Header size in bytes */
    LONG     biWidth; // 4 /* Width of image */
    LONG     biHeight; // 4 /* Height of image */
    WORD     biPlanes; // 2 /* Number of colour planes */
    WORD     biBitCount; // 2 /* Bits per pixel */
    DWORD    biCompress; // 4 /* Compression type */
    DWORD    biSizeImage; // 4 /* Image size in bytes */
    LONG     biXPelsPerMeter; // 4
    LONG     biYPelsPerMeter; // 4 /* Pixels per meter */
    DWORD    biClrUsed; // 4 /* Number of colours */
    DWORD    biClrImportant; // 4 /* Important colours */
} __attribute__((packed)) BITMAPINFOHEADER;


struct RGB_palette
{
    BYTE    blue;
    BYTE    green;
    BYTE    red;
    BYTE    reserved;
    RGB_palette(BYTE r, BYTE g, BYTE b, BYTE res):blue(b),green(g),red(r), reserved(res){}
}; // RGB TYPE, plz also make sure the order

class BitMap
{
public:
    explicit BitMap(const std::string& filename);
    std::vector<BYTE> pixel_data();
    std::vector<RGB_palette> palette();
    int width() const;
    int height() const;
    int bitsPerPixel() const;
    std::string filename() const;
    static void error(const std::string& error_string);
    void printHeaders();
    void save(const std::string& filename);
    void applyThreshold(const float &threshold);
    void applyGreyScale();
    void applyFilter(const std::vector<int> &filter);
    void applyFilterFromFile(const std::string& filename);

private:
    //member functions
    void load();
    void reset();


    static void apply2DConvolution(std::vector<BYTE> &output,
                                                   const std::vector<BYTE>& input,
                                                   const int& mRows,
                                                   const int& mCols,
                                                   const std::vector<int>  &kernel,
                                                   const int& kCols,
                                                   const int& kRows);

    static void removePadding(std::vector<BYTE> *data, const int&row, const int& col, const int& padding);
    static void addPadding(std::vector<BYTE> *data, const int&row, const int& col, const int& padding);
    static void fetchKernelFromFile(std::vector<int> &kernel, const std::string& filename);

    //member variables
    int m_height, m_width, m_bits_per_pixel;
    short m_channel;
    BITMAPFILEHEADER m_header_file;
    BITMAPINFOHEADER m_header_info;
    std::string m_filename;
    std::vector<BYTE> m_pixel_data;
    std::vector<RGB_palette> m_palette;

};
#endif // BITMAP_H
