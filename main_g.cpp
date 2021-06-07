#include <iostream>
#include <string>
#include <fstream>

//typedef struct {
//    int8_t id[2];            // ������ �� ����� 'B' � 'M'
//    int32_t filesize;        // ����� ����� � ������
//    int16_t reserved[2];     // 0, 0
//    int32_t headersize;      // 54L ��� 24-����� ���������
//    int32_t infoSize;        // 40L ��� 24-����� ���������
//    int32_t width;           // ������ ���������� � �������
//    int32_t depth;           // ������ ���������� � �������
//    int16_t biPlanes;        // 1 (��� 24-����� ���������)
//    int16_t bits;            // 24 (��� 24-����� ���������)
//    int32_t biCompression;   // 0L
//    int32_t biSizeImage;     // ����� ��������� � 0L ��� ��������� ��� �������� (��� ������)
//    int32_t biXPelsPerMeter; // ������������� ������� ������ �� ����, ����� 0L
//    int32_t biYPelsPerMeter; // �� ����, �� �����
//    int32_t biClrUsed;       // ��� ������������ ���������, ����� ��������� 0L
//    int32_t biClrImportant;  // �� ����
//} BMPHEAD;

typedef struct {
    int8_t redC;
    int8_t greenC;
    int8_t blueC;
} PIXELDATA;

int main(int _argc, char* _argv[])
{

    //BMPHEAD bh;


    std::fstream pFile("lab4.bmp", std::ios::in | std::ios::binary);
    unsigned char HEAD[54];
    pFile.read((char*)&HEAD, 54);
    //char* b;
    //int* a;
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
    PIXELDATA** pixels = new PIXELDATA * [*height];
    for (int i = 0; i < *height; i++)
    {
        pixels[i] = new PIXELDATA[*wid];
    }
    //PIXELDATA pixels[8][8];
    int* file_size = (int*)&HEAD[2];
    *file_size -= 54;

    unsigned char* pixels_byte = new unsigned char[*file_size];
    //unsigned char pixels_byte[192];
    pFile.read((char*)pixels_byte, *file_size);
    int pointer;
    int padding;
    padding = *wid % 4;
    for (int i = 0; i < *height; i++)
    {
        for (int j = 0; j < *wid; j++)
        {
            pointer = i * (*height)*3 + j * 3;
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

    PIXELDATA** pixels_n = new PIXELDATA * [heightn];
    for (int i = 0; i < heightn; i++)
    {
        pixels_n[i] = new PIXELDATA[widn];
    }
    //PIXELDATA pixels_n[16][16];

    int w = 0, h = 0;

    w = *wid;
    h = *height;
    for (int i = 0; i < heightn; i++)
    {
        for (int j = 0; j < widn; j++)
        {
            pixels_n[i][j].blueC = pixels[(i * h) / heightn][(j * w) / widn].blueC;
            pixels_n[i][j].greenC = pixels[(i * h) / heightn][(j * w) / widn].greenC;
            pixels_n[i][j].redC = pixels[(i * h) / heightn][(j * w) / widn].redC;
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
    //FILE* pfile = fopen("lab4r.bmp", "wb");
    //std::cout << sizeof(int) << std::endl;
    /*unsigned char* WID = (unsigned char*)&widn;
    unsigned char* HEI = (unsigned char*)&heightn;*/
    int* WID;
    int* HEI;

    

    int FILE_LEN = (widn * heightn * 3) + 54;
    int* len1;
    len1 = (int*)&HEAD[2];
    *len1 = FILE_LEN;
    len1 = (int*)&HEAD[18];
    *len1 = widn;
    len1 = (int*)&HEAD[22];
    *len1 = heightn;
    //HEAD[2] = 0xC0;
    //HEAD[3] = 0x36;
    //HEAD[18] = 0x80;
    //HEAD[22] = 0x80;
    /*HEAD[18] = WID[0];
    HEAD[19] = WID[1];
    HEAD[20] = WID[2];
    HEAD[21] = WID[3];
    HEAD[22] = HEI[0];
    HEAD[23] = HEI[1];
    HEAD[24] = HEI[2];
    HEAD[24] = HEI[3];*/

    //std::cout << file_sizen;
    //pfile.write((char*)&HEAD, 54);
    for (int i = 0; i < 54; i++)
    {
        pfile << HEAD[i];
    }
    //fwrite((unsigned char*)&HEAD, 1, 54, pfile);
    //std::cout << std::endl;

   // unsigned char* all_pixels = new unsigned char[file_sizen - 54];
   // //unsigned char all_pixels[768];

   // for (int i = 0; i < widn; i++)
   // {
   //     for (int j = 0; j < heightn; j++)
   //     {
   //         pfile << pixels_n[i][j].blueC;
   //         pfile << pixels_n[i][j].greenC;
   //         pfile << pixels_n[i][j].redC;
   //     }
   // }
   // int count = 0;
   // for (int i = 0; i < widn; i++)
   // {
   //     for (int j = 0; j < heightn; j++)
   //     {
   //         pointer = i * (widn)*3 +j * 3;
   //         all_pixels[pointer] = pixels_n[i][j].blueC;
   //         all_pixels[pointer+1] = pixels_n[i][j].greenC;
   //         all_pixels[pointer+2] = pixels_n[i][j].redC;
   //         /*all_pixels[pointer] = 0x00;
   //         all_pixels[pointer + 1] = 0x00;
   //         all_pixels[pointer + 2] = 0x00;
   //         count++;*/
   //     }
   // }
   // /*std::cout << std::endl;
   // std::cout << pointer;
   // std::cout << std::endl;
   // std::cout << count;*/
   ///* for (int i = 0; i < 768; i++)
   // {
   //     std::cout << all_pixels[i] << std::endl;
   // }*/
   // for (int i = 0; i < 54; i++)
   // {
   //     std::cout << i+1 << " " << HEAD[i] << std::endl;
   // }
   padding = (widn*3) % 4;
   for (int i = 0; i < heightn; i++)
   {
        for (int j = 0; j < widn; j++)
        {
            pfile << pixels_n[i][j].blueC;
            pfile << pixels_n[i][j].greenC;
            pfile << pixels_n[i][j].redC;
        }
        pfile.write("", padding);
   }
   // //pfile.write((char*)all_pixels, file_sizen - 54);
   pfile.close();
}
