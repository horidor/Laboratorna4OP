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

class BITMAP
{
private:
    BMPHEAD bh;
    PIXELDATA* pixels;
    int padding;

    int32_t* give_by_address32(unsigned char &a)
    {
        return (int32_t*)&a;
    }

    int16_t* give_by_address16(unsigned char &a)
    {
        return (int16_t*)&a;
    }

    void create_header(unsigned char* HEAD)
    {
        int32_t* int32p;
        int16_t* int16p;
        bh.id[0] = HEAD[0];
        bh.id[1] = HEAD[1];
        //bh.filesize = *(give_by_address32(HEAD[2]));
        //int32p = (int32_t*)&HEAD[2];
        bh.filesize = *(give_by_address32(HEAD[2]));
        //std::cout << bh.filesize;
        for (int i = 0; i < 2; i++)
        {
            bh.reserved[i] = *(give_by_address16(HEAD[6 + i * 2]));
        }
        bh.headersize = *(give_by_address32(HEAD[10]));
        bh.infoSize = *(give_by_address32(HEAD[14]));
        bh.width = *(give_by_address32(HEAD[18]));
        bh.depth = *(give_by_address32(HEAD[22]));
        bh.biPlanes = *(give_by_address16(HEAD[26]));
        bh.bits = *(give_by_address16(HEAD[28]));
        bh.biCompression = *(give_by_address32(HEAD[30]));
        bh.biSizeImage = *(give_by_address32(HEAD[34]));
        bh.biXPelsPerMeter = *(give_by_address32(HEAD[38]));
        bh.biYPelsPerMeter = *(give_by_address32(HEAD[42]));
        bh.biClrUsed = *(give_by_address32(HEAD[46]));
        bh.biClrImportant = *(give_by_address32(HEAD[50]));
    }

    void create_pixeldata(unsigned char* pixel_bytes)
    {
        pixels = new PIXELDATA[bh.width*bh.depth];
        int pointer;
        for (int i = 0; i < bh.depth; i++)
        {
            for (int j = 0; j < bh.width; j++)
            {
                pointer = i * (bh.width) * 3 + j * 3 + padding * i;
                pixels[i*bh.width + j].blueC = pixel_bytes[pointer];
                pixels[i*bh.width + j].greenC = pixel_bytes[pointer + 1];
                pixels[i*bh.width + j].redC = pixel_bytes[pointer + 2];
                //std::cout << pixels[i * bh.depth + j].blueC + 48 << " " << pixels[i * bh.depth + j].greenC + 48 << " " << pixels[i * bh.depth + j].redC + 48 << std::endl;
            }
        }
    }

    void copy_header(BMPHEAD bhc, int mult = 1)
    {
        bh.id[0] = bhc.id[0];
        bh.id[1] = bhc.id[1];
        bh.filesize = ((bhc.width * mult) * (bhc.depth * mult) * 3) + 54 + ((4 - ((bhc.width * mult) * 3) % 4) % 4) * (bhc.depth * mult);
        bh.reserved[0] = bhc.reserved[0];
        bh.reserved[1] = bhc.reserved[1];
        bh.headersize = bhc.headersize;
        bh.infoSize = bhc.infoSize;
        bh.width = bhc.width * mult;
        bh.depth = bhc.depth * mult;
        bh.biPlanes = bhc.biPlanes;
        bh.bits = bhc.bits;
        bh.biCompression = bhc.biCompression;
        bh.biSizeImage = bhc.biSizeImage;
        bh.biXPelsPerMeter = bhc.biXPelsPerMeter;
        bh.biYPelsPerMeter = bhc.biYPelsPerMeter;
        bh.biClrUsed = bhc.biClrUsed;
        bh.biClrImportant = bhc.biClrImportant;
    }

    void copy_resiz_pixeldata(PIXELDATA* pixels_o, int d_old, int w_old)
    {
        pixels = new PIXELDATA[bh.width * bh.depth];
        int scale;
        for (int i = 0; i < bh.depth; i++)
        {
            for (int j = 0; j < bh.width; j++)
            {
                scale = (i * d_old)/bh.depth*w_old + ((j*w_old) / bh.width);
                pixels[i * bh.width + j].blueC = pixels_o[scale].blueC;
                pixels[i * bh.width + j].greenC = pixels_o[scale].greenC;
                pixels[i * bh.width + j].redC = pixels_o[scale].redC;
                //std::cout << scale << " ";
                //std::cout << pixels[i*bh.depth + j].blueC + 48 << " " << pixels[i * bh.depth + j].greenC + 48 << " " << pixels[i * bh.depth + j].redC + 48 << std::endl;
                //std::cout << pixels_o[scale].blueC + 48 << " " << pixels_o[scale].greenC + 48 << " " << pixels_o[scale].redC + 48 << std::endl;
            }
        }
    }

    unsigned char* readdress32(int32_t a)
    {
        return (unsigned char*)&a;
    }

    unsigned char* readdress16(int16_t a)
    {
        return (unsigned char*)&a;
    }

    unsigned char* buffer_header()
    {
        unsigned char* HEAD = new unsigned char[54];
        unsigned char* ch;
        HEAD[0] = bh.id[0];
        HEAD[1] = bh.id[1];
        ch = readdress32(bh.filesize);
        for (int i = 0; i < 4; i++)
        {
            HEAD[2 + i] = ch[i];
        }
        ch = readdress16(bh.reserved[0]);
        for (int i=0; i<2; i++)
        {
            HEAD[6 + i] = ch[i];
        }
        ch = readdress16(bh.reserved[1]);
        for (int i = 0; i < 2; i++)
        {
            HEAD[8 + i] = ch[i];
        }
        ch = readdress32(bh.headersize);
        for (int i = 0; i < 4; i++)
        {
            HEAD[10 + i] = ch[i];
        }
        ch = readdress32(bh.infoSize);
        for (int i = 0; i < 4; i++)
        {
            HEAD[14 + i] = ch[i];
        }
        ch = readdress32(bh.width);
        for (int i = 0; i < 4; i++)
        {
            HEAD[18 + i] = ch[i];
        }
        ch = readdress32(bh.depth);
        for (int i = 0; i < 4; i++)
        {
            HEAD[22 + i] = ch[i];
        }
        ch = readdress16(bh.biPlanes);
        for (int i = 0; i < 2; i++)
        {
            HEAD[26 + i] = ch[i];
        }
        ch = readdress16(bh.bits);
        for (int i = 0; i < 2; i++)
        {
            HEAD[28 + i] = ch[i];
        }
        ch = readdress32(bh.biCompression);
        for (int i = 0; i < 4; i++)
        {
            HEAD[30 + i] = ch[i];
        }
        ch = readdress32(bh.biSizeImage);
        for (int i = 0; i < 4; i++)
        {
            HEAD[34 + i] = ch[i];
        }
        ch = readdress32(bh.biXPelsPerMeter);
        for (int i = 0; i < 4; i++)
        {
            HEAD[38 + i] = ch[i];
        }
        ch = readdress32(bh.biYPelsPerMeter);
        for (int i = 0; i < 4; i++)
        {
            HEAD[42 + i] = ch[i];
        }
        ch = readdress32(bh.biClrUsed);
        for (int i = 0; i < 4; i++)
        {
            HEAD[46 + i] = ch[i];
        }
        ch = readdress32(bh.biClrImportant);
        for (int i = 0; i < 4; i++)
        {
            HEAD[50 + i] = ch[i];
        }
        return HEAD;
    }

    unsigned char* buffer_pixeldata()
    {
        unsigned char* buf_pixels = new unsigned char[bh.filesize - 54];
        int pointer=0;
        for (int i = 0; i < bh.depth; i++)
        {
            for (int j = 0; j < bh.width; j++)
            {
                pointer = (i * bh.width) * 3 + j * 3 + padding * i;
                buf_pixels[pointer] = pixels[i * bh.width + j].blueC;
                buf_pixels[pointer + 1] = pixels[i * bh.width + j].greenC;
                buf_pixels[pointer + 2] = pixels[i * bh.width + j].redC;
                //std::cout << buf_pixels[pointer]+48 << " " << buf_pixels[pointer+1]+48 << " " << buf_pixels[pointer+2]+48 << std::endl;
                
            }
            if (padding != 0)
            {
                pointer += 3;
                for (int k = 0; k < padding; k++)
                {
                    //std::cout << pointer << std::endl;
                    buf_pixels[pointer] = '\0';
                    pointer++;
                    //std::cout << padding << std::endl;
                }
            }
        }
        return buf_pixels;
    }

public:
    BITMAP(std::string name)
    {
        std::fstream pFile(name, std::ios::in | std::ios::binary);
        if (pFile.fail())
        {
            std::cout << "There is no such file." << std::endl;
            exit(1);
        }

        unsigned char HEAD[54];
        pFile.read((char*)&HEAD, 54);
        create_header(HEAD);

        padding = (4 - (bh.width * 3) % 4) % 4;

        unsigned char* pixel_byte = new unsigned char[bh.filesize - 54];
        pFile.read((char*)pixel_byte, bh.filesize - 54);
        create_pixeldata(pixel_byte);

        pFile.close();
        delete[] pixel_byte;
    }

    BITMAP(BITMAP BM, int mult)
    {
        copy_header(BM.get_header(), mult);
        copy_resiz_pixeldata(BM.get_pixels(), bh.depth / mult, bh.width / mult);
        padding = (4 - (bh.width * 3) % 4) % 4;
    }

    void OUTPUT_BITMAP(std::string name)
    {
        std::fstream pFile(name, std::ios::out | std::ios::binary);
        if (pFile.fail())
        {
            std::cout << "Cannot create such file." << std::endl;
            exit(1);
        }
        
        unsigned char* HEAD = buffer_header();

        pFile.write((char*)HEAD, 54);
        unsigned char* PIXELS = buffer_pixeldata();
        pFile.write((char*)PIXELS, bh.filesize - 54);

        pFile.close();
        delete[] HEAD;
        delete[] PIXELS;
    }

    BMPHEAD get_header() { return bh; }
    PIXELDATA* get_pixels() { return pixels; }

};


int main(int _argc, char* _argv[])
{

   // //BMPHEAD bh;


   // std::fstream pFile("tested.bmp", std::ios::in | std::ios::binary);
   // unsigned char HEAD[54];
   // pFile.read((char*)&HEAD, 54);
   // //char* b;
   // //int* a;
   // //b = (char*) &HEAD[22];
   // //a = (int*) &HEAD[3];
   // //std::cout << *a;

   // int mult;
   // int* wid, * height;
   // wid = (int*)&HEAD[18];
   // height = (int*)&HEAD[22];
   // std::cout << *wid << std::endl;
   // std::cout << *height << std::endl;
   // mult = 2;
   // PIXELDATA** pixels = new PIXELDATA * [*height];
   // for (int i = 0; i < *height; i++)
   // {
   //     pixels[i] = new PIXELDATA[*wid];
   // }
   // //PIXELDATA pixels[8][8];
   // int* file_size = (int*)&HEAD[2];
   // *file_size -= 54;

   // unsigned char* pixels_byte = new unsigned char[*file_size];
   // //unsigned char pixels_byte[192];
   // pFile.read((char*)pixels_byte, *file_size);
   // int pointer;
   // int padding;
   // padding = (4 - (*wid*3) % 4) % 4;
   // for (int i = 0; i < *height; i++)
   // {
   //     for (int j = 0; j < *wid; j++)
   //     {
   //         pointer = i * (*height)*3 + j * 3 + padding * i;
   //         pixels[i][j].blueC = pixels_byte[pointer];
   //         pixels[i][j].greenC = pixels_byte[pointer + 1];
   //         pixels[i][j].redC = pixels_byte[pointer + 2];
   //     }
   // }

   // int widn;
   // int heightn;
   // int file_sizen;

   // pFile.close();

   // widn = (*wid) * mult;
   // heightn = (*height) * mult;
   // file_sizen = ((widn) * (heightn) * 3) + 54;

   // PIXELDATA** pixels_n = new PIXELDATA * [heightn];
   // for (int i = 0; i < heightn; i++)
   // {
   //     pixels_n[i] = new PIXELDATA[widn];
   // }
   // //PIXELDATA pixels_n[16][16];

   // int w = 0, h = 0;

   // w = *wid;
   // h = *height;
   // for (int i = 0; i < heightn; i++)
   // {
   //     for (int j = 0; j < widn; j++)
   //     {
   //         pixels_n[i][j].blueC = pixels[(i * h) / heightn][(j * w) / widn].blueC;
   //         pixels_n[i][j].greenC = pixels[(i * h) / heightn][(j * w) / widn].greenC;
   //         pixels_n[i][j].redC = pixels[(i * h) / heightn][(j * w) / widn].redC;
   //     }
   // }

   // /*for (int i = 0; i < *wid; i++)
   // {
   //     for (int j = 0; j < *height; j++)
   //     {
   //         pixels_n[i_n][j_n].blueC = pixels[(i * (*wid)) / widn][(j * (*height)) / heightn].blueC;
   //         pixels_n[i][j_n].greenC = pixels[(i * (*wid)) / widn][(j * (*height)) / heightn].greenC;
   //         pixels_n[i*mult][j].redC = pixels[(i * (*wid)) / widn][(j * (*height)) / heightn].redC;
   //     }
   // }*/

   // std::fstream pfile("lab4r.bmp", std::ios::out | std::ios::binary);
   // //FILE* pfile = fopen("lab4r.bmp", "wb");
   // //std::cout << sizeof(int) << std::endl;
   // /*unsigned char* WID = (unsigned char*)&widn;
   // unsigned char* HEI = (unsigned char*)&heightn;*/
   // int* WID;
   // int* HEI;

   // 
   // padding = (4 - (widn * 3) % 4) % 4;
   // int FILE_LEN = (widn * heightn * 3) + 54 + padding * heightn;
   // int* len1;
   // len1 = (int*)&HEAD[2];
   // *len1 = FILE_LEN;
   // len1 = (int*)&HEAD[18];
   // *len1 = widn;
   // len1 = (int*)&HEAD[22];
   // *len1 = heightn;
   // //HEAD[2] = 0xC0;
   // //HEAD[3] = 0x36;
   // //HEAD[18] = 0x80;
   // //HEAD[22] = 0x80;
   // /*HEAD[18] = WID[0];
   // HEAD[19] = WID[1];
   // HEAD[20] = WID[2];
   // HEAD[21] = WID[3];
   // HEAD[22] = HEI[0];
   // HEAD[23] = HEI[1];
   // HEAD[24] = HEI[2];
   // HEAD[24] = HEI[3];*/

   // //std::cout << file_sizen;
   // //pfile.write((char*)&HEAD, 54);
   // for (int i = 0; i < 54; i++)
   // {
   //     pfile << HEAD[i];
   // }
   // //fwrite((unsigned char*)&HEAD, 1, 54, pfile);
   // //std::cout << std::endl;

   //// unsigned char* all_pixels = new unsigned char[file_sizen - 54];
   //// //unsigned char all_pixels[768];

   //// for (int i = 0; i < widn; i++)
   //// {
   ////     for (int j = 0; j < heightn; j++)
   ////     {
   ////         pfile << pixels_n[i][j].blueC;
   ////         pfile << pixels_n[i][j].greenC;
   ////         pfile << pixels_n[i][j].redC;
   ////     }
   //// }
   //// int count = 0;
   //// for (int i = 0; i < widn; i++)
   //// {
   ////     for (int j = 0; j < heightn; j++)
   ////     {
   ////         pointer = i * (widn)*3 +j * 3;
   ////         all_pixels[pointer] = pixels_n[i][j].blueC;
   ////         all_pixels[pointer+1] = pixels_n[i][j].greenC;
   ////         all_pixels[pointer+2] = pixels_n[i][j].redC;
   ////         /*all_pixels[pointer] = 0x00;
   ////         all_pixels[pointer + 1] = 0x00;
   ////         all_pixels[pointer + 2] = 0x00;
   ////         count++;*/
   ////     }
   //// }
   //// /*std::cout << std::endl;
   //// std::cout << pointer;
   //// std::cout << std::endl;
   //// std::cout << count;*/
   /////* for (int i = 0; i < 768; i++)
   //// {
   ////     std::cout << all_pixels[i] << std::endl;
   //// }*/
   //// for (int i = 0; i < 54; i++)
   //// {
   ////     std::cout << i+1 << " " << HEAD[i] << std::endl;
   //// }
   //
   //for (int i = 0; i < heightn; i++)
   //{
   //     for (int j = 0; j < widn; j++)
   //     {
   //         pfile << pixels_n[i][j].blueC;
   //         pfile << pixels_n[i][j].greenC;
   //         pfile << pixels_n[i][j].redC;
   //     }
   //     if (padding != 0)
   //     {
   //         pfile.write("\0", padding);
   //     }
   //}
   //// //pfile.write((char*)all_pixels, file_sizen - 54);
   //pfile.close();
    /*std::string name;
    getline(std::cin, name);*/

    BITMAP input_bmp(_argv[1]);
    BITMAP output_bmp(input_bmp, atoi(_argv[3]));
    output_bmp.OUTPUT_BITMAP(_argv[2]);
    }
