#include <iostream>
#include <string>
#include <fstream>

typedef struct { //��������� �����
    int8_t id[2];            // ������ �� ����� 'B' � 'M'
    int32_t filesize;        // ����� ����� � ������
    int16_t reserved[2];     // 0, 0
    int32_t headersize;      // 54L ��� 24-����� ���������
    int32_t infoSize;        // 40L ��� 24-����� ���������
    int32_t width;           // ������ ���������� � �������
    int32_t depth;           // ������ ���������� � �������
    int16_t biPlanes;        // 1 (��� 24-����� ���������)
    int16_t bits;            // 24 (��� 24-����� ���������)
    int32_t biCompression;   // 0L
    int32_t biSizeImage;     // ����� ��������� � 0L ��� ��������� ��� �������� (��� ������)
    int32_t biXPelsPerMeter; // ������������� ������� ������ �� ����, ����� 0L
    int32_t biYPelsPerMeter; // �� ����, �� �����
    int32_t biClrUsed;       // ��� ������������ ���������, ����� ��������� 0L
    int32_t biClrImportant;  // �� ����
} BMPHEAD;

typedef struct { //������
    int8_t redC;
    int8_t greenC;
    int8_t blueC;
} PIXELDATA;

class BITMAP //���� ������, ������ ���������� ���� �����, ��������� � ������ ���� �� ����� � ���� ������� .bmp
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

    void download_header(unsigned char* HEAD) //������������ ��������� � �����
    {
        bh.id[0] = HEAD[0];
        bh.id[1] = HEAD[1];
        bh.filesize = *(give_by_address32(HEAD[2]));
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

    void create_pixeldata(unsigned char* pixel_bytes) //������������ ������� � ����
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
            }
        }
    }

    void copy_header(BMPHEAD bhc, int mult = 1)//��������� ������ � ��������� ������ � ����� ������ ����� �� ���� ������ �� ������
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

    void copy_resiz_pixeldata(PIXELDATA* pixels_o, int d_old, int w_old) //��������� (�� ���������) ������ �������
    {
        std::cout << "Resizing file by a factor of " << bh.depth / d_old << "... ";
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
            }
        }
        std::cout << "Done." << std::endl;
    }

    unsigned char* readdress32(int32_t a)
    {
        return (unsigned char*)&a;
    }

    unsigned char* readdress16(int16_t a)
    {
        return (unsigned char*)&a;
    }

    unsigned char* buffer_header() //���������� ��������� � �����
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

    unsigned char* buffer_pixeldata() //���������� ������� � �����
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
                
            }
            if (padding != 0)
            {
                pointer += 3;
                for (int k = 0; k < padding; k++)
                {
                    buf_pixels[pointer] = '\0';
                    pointer++;
                }
            }
        }
        return buf_pixels;
    }

public:
    BITMAP(std::string name) //������ ����������� - ������ ����� �����, ��������� ��'��� �� �����������, ��� ���������� � ����
    {
        std::fstream pFile(name, std::ios::in | std::ios::binary);
        if (pFile.fail())
        {
            std::cout << "There is no such file." << std::endl;
            exit(1);
        }

        unsigned char HEAD[54];
        pFile.read((char*)&HEAD, 54);
        download_header(HEAD);

        padding = (4 - (bh.width * 3) % 4) % 4;

        unsigned char* pixel_byte = new unsigned char[bh.filesize - 54];
        pFile.read((char*)pixel_byte, bh.filesize - 54);
        create_pixeldata(pixel_byte);

        pFile.close();
        delete[] pixel_byte;
    }

    BITMAP(BITMAP BM, int mult) //������ ���������� - ������ ����� ����� �� ���������� ���������, ������� ��'��� ���������� �� ���������� ���������
    {
        copy_header(BM.get_header(), mult);
        copy_resiz_pixeldata(BM.get_pixels(), bh.depth / mult, bh.width / mult);
        padding = (4 - (bh.width * 3) % 4) % 4;
    }

    void OUTPUT_BITMAP(std::string name) //����� ������ � ���� �� ������
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
        std::cout << "Result is saved in " << name << std::endl;
        delete[] HEAD;
        delete[] PIXELS;
    }

    BMPHEAD get_header() { return bh; } //�������
    PIXELDATA* get_pixels() { return pixels; }

};


int main(int _argc, char* _argv[])
{
    BITMAP input_bmp(_argv[1]); //�������� ����
    BITMAP output_bmp(input_bmp, atoi(_argv[3])); //�������� ���� ����������
    output_bmp.OUTPUT_BITMAP(_argv[2]); //�������� ����
}
