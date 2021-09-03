#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "image.h"


float HCLgamma = 3;
float HCLy0 = 100;
float HCLmaxL = 0.530454533953517; // == exp(HCLgamma / HCLy0) - 0.5
float PI = 3.1415926536;

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, sizeof(float)*(im.h*im.w*im.c));
    return copy;
}

int clamp_integer(int val, int lo, int hi) {
    return (val < lo) ?  lo : ((val > hi) ? hi : val ); 
}

float clamp_float(float val, float lo, float hi) {
    return (val < lo) ?  lo : ((val > hi) ? hi : val ); 
}

/**
 * @brief Get the pixel value from image `im` using claming strategy
 * 
 * @param im
 * @param x 
 * @param y 
 * @param c 
 * @return float 
 */
float get_pixel(image im, int x, int y, int c){
    x = clamp_integer(x, 0, im.w-1);
    y = clamp_integer(y, 0, im.h-1);
    c = clamp_integer(c, 0, im.c-1);
    return im.data[x + y*im.w + c*im.w*im.h];
}

void set_pixel(image im, int x, int y, int c, float v){
    if ( x != clamp_integer(x, 0, im.w-1) ) return;
    if ( y != clamp_integer(y, 0, im.h-1) ) return;
    if ( c != clamp_integer(c, 0, im.c-1) ) return;
    im.data[x + y*im.w + c*im.w*im.h] = v;
}

image rgb_to_grayscale(image im, int channel)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, channel);
    float r = .299, g = .587, b = .114;
    for (int j=0; j<im.h; ++j)
    {
        for (int i =0; i<im.w; ++i)
        {
            float y = get_pixel(im,i,j,0) * r + get_pixel(im,i,j,1)*g + get_pixel(im,i,j,2)*b;
            for (int k=0; k<channel; ++k) {set_pixel(gray,i,j,k,y);}
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    assert(im.c >= c);
    for (int j=0; j<im.h; ++j)
    {
        for (int i =0; i<im.w; ++i)
        {
            float p = get_pixel(im,i,j,c);
            set_pixel(im,i,j,c,p+v);
        }
    }
}

void clamp_image(image im)
{
    for (int j=0; j<im.h; ++j)
    {
        for (int i =0; i<im.w; ++i)
        {
            for (int k = 0; k<im.c; ++k)
            {
                float f = get_pixel(im,i,j,k);
                set_pixel(im,i,j,k,clamp_float(f, 0.0, 1.0));
            }
        }
    }
}

void scale_image(image im, int c, float v)
{
    assert(im.c >= c);
    for (int j=0; j<im.h; ++j)
    {
        for (int i =0; i<im.w; ++i)
        {
            float p = get_pixel(im,i,j,c);
            set_pixel(im,i,j,c,p*v);
        }
    }
}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    assert(im.c == 3);
    for (int j=0; j<im.h; ++j)
    {
        for (int i =0; i<im.w; ++i)
        {
            float r = get_pixel(im,i,j,0);
            float g = get_pixel(im,i,j,1);
            float b = get_pixel(im,i,j,2);

            float v = three_way_max(r,g,b);

            float c = v - three_way_min(r,g,b);            
            float s = (v == 0) ? 0 : c/v;

            float hprime = 0;
            if (c == 0) hprime=0;
            else if (v == r) hprime = (g-b)/c;
            else if (v == g) hprime = (b-r)/c + 2;
            else if (v == b) hprime = (r-g)/c + 4;
            
            float h = (hprime < 0) ? hprime/6 + 1 : hprime/6;
            
            set_pixel(im,i,j,0,h);
            set_pixel(im,i,j,1,s);
            set_pixel(im,i,j,2,v);
        }
    }
}

void hsv_to_rgb(image im)
{
    assert(im.c == 3);
    for (int j=0; j<im.h; ++j)
    {
        for (int i =0; i<im.w; ++i)
        {
            float h = get_pixel(im,i,j,0);
            float s = get_pixel(im,i,j,1);
            float v = get_pixel(im,i,j,2);

            float c = v * s;
            float m = v - c;

            float hprime = h*6;
            float x = c * (1 - fabs(fmod(hprime, 2)-1));
            float r1,g1,b1;
            // Ini bisa implementasi LUT / make shift select sih padahal
            if (hprime <= 1){r1 = c; g1 = x; b1 = 0;}
            else if (hprime <= 2){r1 = x; g1 = c; b1 = 0;}
            else if (hprime <= 3){r1 = 0; g1 = c; b1 = x;}
            else if (hprime <= 4){r1 = 0; g1 = x; b1 = c;}
            else if (hprime <= 5){r1 = x; g1 = 0; b1 = c;}
            else {r1 = c; g1 = 0; b1 = x;}

            set_pixel(im,i,j,0,r1+m);
            set_pixel(im,i,j,1,g1+m);
            set_pixel(im,i,j,2,b1+m);
        }
    }
}

void print_image(image im){
    for (int j=0; j<im.h; ++j)
    {
        for (int i =0; i<im.w; ++i)
        {
            for (int k = 0; k<im.c; ++k)
            {
                printf("%.2f ",get_pixel(im,i,j,k));
            }
            printf("\t");
        }
        printf("\n");
    }
}

void im_to_ppm(image im, char const*const name){
    FILE *fp = fopen(name, "wb");
    (void) fprintf(fp, "P6\n%d %d\n255\n", im.w, im.h);
    for (int j=0; j<im.h; ++j)
    {
        for (int i =0; i<im.w; ++i)
        {
            unsigned char color[im.c];
            for (int k = 0; k<im.c; ++k)
            {
                color[k] = (int) (get_pixel(im, i, j, k)*255);
            }
            (void) fwrite(color, 1, im.c, fp);
        }
    }
    (void) fclose(fp);
}

void draw_patern(image im){
    for (int j=0; j<im.h; ++j)
    {
        for (int i =0; i<im.w; ++i)
        {
            for (int k = 0; k<im.c; ++k)
            {
                float p = ((i+1)*(j+1)*(k+1) % 256)/255.0;
                set_pixel(im, i, j, k, p );
            }
        }
    }
}

void draw_boxes(image im){
    for (int j=0; j<im.h; ++j)
    { 
        for (int i =0; i<im.w; ++i)
        {
            int thres = im.w/3;
            float v = (float)(i % thres) / thres;
            float s = (float)j / im.h;
            for (int k = 0; k<im.c; ++k)
            {
                if (i/thres == k) set_pixel(im, i, j, k, v );
                else set_pixel(im, i, j, k, s );
            }
        }
    }
}
