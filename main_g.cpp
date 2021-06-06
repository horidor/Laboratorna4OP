#include <iostream>
#include <string>
#include <fstream>

typedef struct {
    int8_t id[2];            // Завжди дві літери 'B' і 'M'
    int32_t filesize;        // Розмір файла в байтах
    int16_t reserved[2];     // 0, 0
    int32_t headersize;      // 54L для 24-бітних зображень
    int32_t infoSize;        // 40L для 24-бітних зображень
    int32_t width;           // ширина зображення в пікселях
    int32_t depth;           // висота зображення в пікселях
    int16_t biPlanes;        // 1 (для 24-бітних зображень)
    int16_t bits;            // 24 (для 24-бітних зображень)
    int32_t biCompression;   // 0L
    int32_t biSizeImage;     // Можна поставити в 0L для зображень без компрессії (наш варіант)
    int32_t biXPelsPerMeter; // Рекомендована кількість пікселів на метр, можна 0L
    int32_t biYPelsPerMeter; // Те саме, по висоті
    int32_t biClrUsed;       // Для індексованих зображень, можна поставити 0L
    int32_t biClrImportant;  // Те саме
} BMPHEAD;

typedef struct {
    int8_t redC;
    int8_t greenC;
    int8_t blueC;
} PIXELDATA;

int main(int _argc, char* _argv[])
{

    BMPHEAD bh;


    std::fstream pFile("lab4.bmp", std::ios::in | std::ios::binary);
    unsigned char HEAD[54];
    pFile.read((char*)&HEAD, 54);
    //char* b;
    int* a;
    //b = (char*) &HEAD[22];
    //a = (int*) &HEAD[3];
    //std::cout << *a;

    int mult;
    int* wid, * height;
    wid = (int*)&HEAD[18];
    height = (int*)&HEAD[22];
    std::cout << *wid << std::endl;
    std::cout << *height << std::endl;
    mult = 2;
    PIXELDATA** pixels = new PIXELDATA * [*wid];
    for (int i = 0; i < *wid; i++)
    {
        pixels[i] = new PIXELDATA[*height];
    }
    int* file_size = (int*)&HEAD[2];
    *file_size -= 54;
    unsigned char* pixels_byte = new unsigned char[*file_size];
    pFile.read((char*)pixels_byte, *file_size);
    int pointer;
    for (int i = 0; i < *wid; i++)
    {
        for (int j = 0; j < *height; j++)
        {
            pointer = i * (*wid) + j * 3;
            pixels[i][j].blueC = pixels_byte[pointer];
            pixels[i][j].greenC = pixels_byte[pointer + 1];
            pixels[i][j].redC = pixels_byte[pointer + 2];
        }
    }

    int widn;
    int heightn;
    int file_sizen;

    pFile.close();

    widn = (*wid) * mult;
    heightn = (*height) * mult;
    file_sizen = ((widn) * (heightn) * 3) + 54;

    PIXELDATA** pixels_n = new PIXELDATA * [widn];
    for (int i = 0; i < widn; i++)
    {
        pixels_n[i] = new PIXELDATA[heightn];
    }

    int w = 0, h = 0;

    w = *wid;
    h = *height;
    for (int i = 0; i < widn; i++)
    {
        for (int j = 0; j < heightn; j++)
        {
            pixels_n[i][j].blueC = pixels[(i * w) / widn][(j * h) / heightn].blueC;
            pixels_n[i][j].greenC = pixels[(i * w) / widn][(j * h) / heightn].greenC;
            pixels_n[i][j].redC = pixels[(i * w) / widn][(j * h) / heightn].redC;
        }
    }

    /*for (int i = 0; i < *wid; i++)
    {
        for (int j = 0; j < *height; j++)
        {
            pixels_n[i_n][j_n].blueC = pixels[(i * (*wid)) / widn][(j * (*height)) / heightn].blueC;
            pixels_n[i][j_n].greenC = pixels[(i * (*wid)) / widn][(j * (*height)) / heightn].greenC;
            pixels_n[i*mult][j].redC = pixels[(i * (*wid)) / widn][(j * (*height)) / heightn].redC;
        }
    }*/

    std::fstream pfile("lab4r.bmp", std::ios::out | std::ios::binary);

    HEAD[2] = 0x03;
    HEAD[3] = 0x36;
    HEAD[18] = 0x10;
    HEAD[22] = 0x10;

    std::cout << file_sizen;
    pfile.write((char*)&HEAD, 54);

    unsigned char* all_pixels = new unsigned char[file_sizen - 54];


    /*for (int i = 0; i < widn; i++)
    {
        for (int j = 0; j < heightn; j++)
        {
            pfile << pixels_n[i][j].blueC;
            pfile << pixels_n[i][j].greenC;
            pfile << pixels_n[i][j].redC;
        }
    }*/
    for (int i = 0; i < widn; i++)
    {
        for (int j = 0; j < heightn; j++)
        {
            pointer = i * (widn) + j * 3;
            all_pixels[pointer] = pixels_n[i][j].blueC;
            all_pixels[pointer+1] = pixels_n[i][j].greenC;
            all_pixels[pointer+2] = pixels_n[i][j].redC;
        }
    }
    pfile.write((char*)all_pixels, file_sizen - 54);
    pfile.close();
}
