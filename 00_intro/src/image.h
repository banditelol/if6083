#ifndef IMAGE_H
#define IMAGE_H
#endif

typedef struct {
    int w, h, c;
    float *data;
} image;

// Utilities function
int clamp_int(int val, int lo, int hi);
float clamp_float(float val, float lo, float hi);

// Basic Operation
float get_pixel(image im, int x, int y, int c);
void set_pixel(image im, int x, int y, int c, float v);
image copy_image(image im);
image rgb_to_grayscale(image im, int channel);
void shift_image(image im, int c, float v);
void clamp_image(image im);
void rgb_to_hsv(image im);
void hsv_to_rgb(image im);
void rgb_to_hcl(image im);
void hcl_to_rgb(image im);
void scale_image(image im, int c, float v);

// Additional Functions
image make_image(int w, int h, int c);
image load_image(char* filename);
void save_image(image im, const char* filename);
void free_image(image im);

void draw_pattern(image im);
void draw_boxes(image im);