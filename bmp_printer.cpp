#include "headers/bmp_printer.h"

uint16_t read16(File &f)
{
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read(); // MSB
    return result;
}

uint32_t read24(File &f)
{
    uint32_t result = 0;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    return result;
}

uint32_t read32(File &f)
{
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read(); // MSB
    return result;
}

char get_color_specific_char(unsigned int pixel)
{
    //o culoare in format rgb este cu atat mai apropiata de componenta care o defineste cu cat
    //restul sunt obligatoriu mai mici
    //vom nota orice culoare care are byte-ul R mai mare ca restul cu R
    //orice culoare are byte-ul G mai mare ca restul cu G
    //si orice culoare care are byte-ul B mai mare ca restul cu B
    //nuantele gray in care R=G=B, vor fi notate cu N
    //iar culoarea alb cu ' ' (spatiu)
    unsigned char r, g, b, i, minim, k;
    unsigned int temp = pixel;
    char folosite[16] = {'x', '@', '-', '+', '$', '%', '^', '&', '*', ';', '/', ' ', '?', '#', '~', '.'};
    //culorile negru si alb sunt spatiu
    unsigned char cnr[16];
    unsigned char rr[16] = {0, 0, 0, 0, 128, 128, 128, 192, 128, 0, 0, 0, 255, 255, 255, 255};
    unsigned char gg[16] = {0, 0, 128, 128, 0, 0, 128, 192, 128, 0, 255, 255, 0, 0, 255, 255};
    unsigned char bb[16] = {0, 128, 0, 128, 0, 128, 0, 192, 128, 255, 0, 255, 0, 255, 0, 255};
    // 4 bit colors     = bk  bl  gr  aq  rd  pl  yl  wt  gy  lbl lgr laq lrd lpl lyl bwt
    b = temp;
    temp >>= 8;
    g = temp;
    temp >>= 8;
    r = temp;

    //compute 16 custom numbers defining colors
    for (i = 0; i < 16; i++)
        cnr[i] = (abs(r - rr[i]) + abs(g - gg[i]) + abs(b - bb[i])) / 3;

    minim = cnr[0];
    k = 0;

    //compute most appropiate color
    for (i = 1; i < 16; i++)
    {
        if (cnr[i] < minim)
        {
            minim = cnr[i];
            k = i;
        }
    }
    return folosite[k];
}

void PrintBMPASCII(const char *fname)
{
    File bmpFile;
    unsigned int bmpWidth, bmpHeight, start_pos, linesize, pos, txtpos, linecount, pixel;
    unsigned int i, j;

    if (!(bmpFile = SD.open(fname)))
    {
        Serial.println("File not found");
        return;
    }

    if (read16(bmpFile) == 0x4D42)
    {
        read32(bmpFile);
        read32(bmpFile);
        start_pos = read32(bmpFile); // Start of image data
        read32(bmpFile);             // Dummy read to throw away and move on
        bmpWidth = read32(bmpFile);  // Image width
        bmpHeight = read32(bmpFile); // Image height

        linesize = bmpWidth * 3 + bmpWidth % 4;

        Serial.print("Width = ");
        Serial.println(bmpWidth);
        Serial.print("Height = ");
        Serial.println(bmpHeight);
        Serial.print("Start = ");
        Serial.println(start_pos);
        Serial.print("LineSize = ");
        Serial.println(linesize);

        if ((read16(bmpFile) == 1) && (read16(bmpFile) == 24) && (read32(bmpFile) == 0))
        {
            for (i = 0; i < bmpHeight; i += 2)
            {
                bmpFile.seek(start_pos + linesize * (bmpHeight - 1 - i));
                for (j = 0; j < bmpWidth; j++)
                {
                    pixel = read24(bmpFile);
                    Serial.print(get_color_specific_char(pixel));
                }
                Serial.println("");
            }
        }
        else
            Serial.println("BMP format nesuportat!");
    }

    bmpFile.close();
}