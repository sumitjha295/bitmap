/*! \brief BitMap
 *
 *  BitMap class to apply fileter and threadhold for 8 bit bmp only
 */

#include "bitmap.h"
#include <iostream>
#include <fstream>

BitMap::BitMap(const std::string& filename)

{
    m_filename = filename;
    load();
}

void BitMap::reset()
{

    m_height = 0;
    m_width = 0;
    m_bits_per_pixel = 0;
    m_channel = 0;
    m_pixel_data.clear();
    m_palette.clear();
}

void BitMap::load()
{
    std::ifstream stream(m_filename, std::ios::in | std::ios::binary);
    stream >> std::noskipws;
    reset();

    if(!stream.is_open())
    {
        error("OPEN_ERROR");
        return;
    }

    stream.seekg(0, std::ios::beg);
    stream.read((char*)&m_header_file, sizeof(BITMAPFILEHEADER));
    stream.read((char*)&m_header_info, sizeof(BITMAPINFOHEADER));



    if(m_header_file.bfType != 0x4d42
            || m_header_info.biCompress !=0
            || m_header_info.biPlanes !=1 )
    {
        error("UNSUPORTED_FORMAT");
        return;
    }

    m_width = m_header_info.biWidth;
    m_height = m_header_info.biHeight;
    m_bits_per_pixel = m_header_info.biBitCount;
    m_channel = m_bits_per_pixel/8;

    if(m_header_info.biClrUsed != 256 || m_header_info.biBitCount != 8)
    {
        error("8BPP_SUPPORT_ONLY");
        return;
    }

    size_t m_palette_size = m_header_file.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
    m_palette.reserve(m_palette_size);

    BYTE  blue, green, red, reserve;
    while(m_palette.size() < m_palette_size)
    {
        stream >> blue;
        stream >> green;
        stream >> red;
        stream >> reserve;
        m_palette.push_back(RGB_palette(red,green,blue,reserve));
    }

    int paddings = (4 - ((m_width * m_bits_per_pixel / 8) % 4)) % 4;

    BYTE val;
    size_t datasize_with_paddings = m_header_file.bfSize - m_header_file.bfOffBits;
    m_pixel_data.reserve(datasize_with_paddings);
    while(m_pixel_data.size() < datasize_with_paddings)
    {
        stream >> val;
        m_pixel_data.push_back(val);
    }

    if(paddings > 0)
    {
        removePadding(&m_pixel_data, m_width, m_height, paddings);
    }
    // no need to close though, ifstream implemented as RAII so will close when out of scope
    stream.close();

}

void BitMap::removePadding(std::vector<BYTE> *data, const int &row_size, const int &col_size, const int &padding)
{
    auto it = data->begin();
    int c=0;
    if((int) data->size() != (row_size + padding)*col_size)
    {
        std::cerr << "INVALID_PIXEL_SIZE" << std::endl;
        return;
    }

    while (c < col_size )
    {
        std::advance(it, row_size);
        it = data->erase(it, it+padding);
        c += 1;
    }
}

void BitMap::addPadding(std::vector<BYTE> *data, const int &row_size, const int &col_size, const int &padding)
{
    auto it = data->begin();
    int c=0;
    int size_with_padding = (row_size + padding) * col_size;

    if((int) data->size() != row_size * col_size)
    {
        std::cerr << "INVALID_PIXEL_SIZE" << std::endl;
        return;
    }

    data->reserve(size_with_padding);
    while (c < col_size )
    {
        std::advance(it, row_size);
        BYTE value;
        for (int i = 0; i < padding ; ++i)
        {
            value = *it;
            it = data->insert( it , value);
        }
        c += 1;
    }
}

void BitMap::applyThreshold(const float& threshold)
{

    int threshold_num = 255 * threshold;
    for(BYTE &x : m_pixel_data)
    {
        x = (int) x > threshold_num ? 0 : 255;

    }
}

void BitMap::fetchKernelFromFile(std::vector<int> &kernel, const std::string& filename)
{
    std::ifstream stream(filename);
    if(!stream.is_open())
    {
        error("OPEN_ERROR");
        return;
    }

    int v;
    while (stream >> v) {
        kernel.push_back(v);
    }
    stream.close();
}


void BitMap::applyFilterFromFile(const std::string& filename)
{
    std::vector<int>  filter;
    fetchKernelFromFile(filter, filename);
    applyFilter(filter);
}


void BitMap::applyFilter(const std::vector<int>  &filter)
{
    if (filter.size() != 3*3)
    {
        std::cerr << "UNSUPPORTED_FILTER_SIZE" << filter.size()<< std::endl;
        return;
    }
    std::vector<BYTE> output(m_pixel_data.size(), 0);
    apply2DConvolution(output,
                       m_pixel_data,
                       m_channel * m_width,
                       m_height,
                       filter,
                       3,
                       3);

    m_pixel_data = output;
}


void BitMap::apply2DConvolution(std::vector<BYTE> &output,
                                const std::vector<BYTE>& input,
                                const int& mRows,
                                const int& mCols,
                                const std::vector<int>  &kernel,
                                const int& kCols,
                                const int& kRows
                                )
{
    // find center position of kernel (half of kernel size)
    int kCenterX = kCols / 2;
    int kCenterY = kRows / 2;
    for(int i=0; i < mRows; ++i)// rows
    {
        for(int j=0; j < mCols; ++j) // columns
        {
            for(int m=0; m < kRows; ++m) {     // kernel rows
                int mm = kRows - 1 - m;      // row index of flipped kernel
                for(int n=0; n < kCols; ++n) { // kernel columns
                    //int nn = kCols - 1 - n;  // column index of flipped kernel

                    // index of input signal, used for checking boundary
                    int ii = i + (m - kCenterY);
                    int jj = j + (n - kCenterX);

                    if(ii < 0)
                        ii=ii+1;
                    if(jj < 0)
                        jj=jj+1;
                    if(ii >= mRows)
                        ii=ii-1;
                    if(jj >= mCols)
                        jj=jj-1;
                    if( ii >= 0 && ii < mRows && jj >= 0 && jj < mCols )
                    {
                        output[i+ j * mRows] += BYTE ((int) input[ii + jj * mRows] * kernel[mm + n * kRows]);

                    }
                }
            }
        }
    }

}

void BitMap::applyGreyScale()
{
    int i = 0;
    for(RGB_palette &x : m_palette)
    {
        BYTE color = (BYTE) i % 256;
        x.blue = color;
        x.red = color;
        x.green = color;
        x.reserved = (BYTE) 0;
        i++;
    }
}

void BitMap::save(const std::string& filename)
{
    std::ofstream stream(filename, std::ios::out | std::ios::binary);
    if(!stream.is_open())
    {
        error("OPEN_ERROR");
        return;
    }

    int paddings = (4 - ((m_width * m_channel) % 4)) % 4;
    if(paddings > 0)
    {
        addPadding(&m_pixel_data, m_width, m_height, paddings);
    }
    m_header_info.biClrUsed =  1 << (m_channel * 8);
    int extra_header = m_palette.size() - m_header_info.biClrUsed * 4;

    if(m_palette.size() ==  m_header_info.biClrUsed )
    {
        extra_header = 0;
    }

    m_header_info.biSize = sizeof(BITMAPINFOHEADER) + extra_header;
    m_header_info.biSizeImage = (m_width + paddings) * m_height;
    m_header_info.biHeight = m_height;
    m_header_info.biWidth = m_width;
    m_header_file.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) +  m_header_info.biClrUsed * 4   + extra_header;
    m_header_file.bfSize =  m_header_info.biSizeImage + m_header_file.bfOffBits;

    stream.write((char*)&m_header_file, sizeof(BITMAPFILEHEADER));
    stream.write((char*)&m_header_info, sizeof(BITMAPINFOHEADER));

    for(RGB_palette &x : m_palette)
    {
        stream.write((char*)&x.blue, sizeof(BYTE));
        stream.write((char*)&x.green, sizeof(BYTE));
        stream.write((char*)&x.red, sizeof(BYTE));
        stream.write((char*)&x.reserved, sizeof(BYTE));
    }

    for(BYTE &x : m_pixel_data)
    {
        stream.write((char*)&x, sizeof(BYTE));
    }
    stream.close();
}

void BitMap::printHeaders()
{

    std::cout << "heigth: "<< m_height << std::endl ;
    std::cout << "width: "<< m_width << std::endl;
    std::cout << "biSizeImage: "<< m_header_info.biSizeImage << std::endl;
    std::cout << "biBitCount: "<< m_header_info.biBitCount << std::endl;
    std::cout << "biClrUsed: "<< m_header_info.biClrUsed << std::endl;
    std::cout << "bfOffBits: "<< m_header_file.bfOffBits << std::endl;
    std::cout << "bfSize: "<< m_header_file.bfSize << std::endl;

}

void BitMap::error(const std::string& error_string)
{
    throw std::runtime_error(error_string);
}

int BitMap::width() const
{
    return m_width;
}

int BitMap::height() const
{
    return m_height;
}

int BitMap::bitsPerPixel() const
{

    return m_bits_per_pixel;
}

std::string BitMap::filename() const

{
    return m_filename;
}

std::vector<BYTE> BitMap::pixel_data()

{
    return m_pixel_data;
}

std::vector<RGB_palette> BitMap::palette()

{
    return m_palette;
}
