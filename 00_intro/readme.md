# Assignment 0

Create C program for handling image with:
1. Data structure like the following, where data is floating point between 0 and 1
   ```C
    typedef struct{
        int h, w, c;
        float *data;
    } image;
    ```
2. get_pixel and set_pixel
3. copy_image
4. rgb_to_grayscale
5. shift_image
6. clamp_image
7. rgb_to_hsv 
8. hsv_to_rgb

## Informasi Pengerjaan

Data yang disimpan di dalam array data adalah berupa `CHW` yang berarti pixel pertama dimulai dari channel 0, baris 0, kolom 0. Selanjutnya channel 0, baris 0, kolom 1. Dan nantinya channel 0, baris 1, kolom 0, dan seterusnya.

Dengan konvensi:
- kolom `x` dengan lebar `W`
- baris `y` dengan tinggi `H`
- channel `c` dengan channel `C`
- pixel `v` dengan range [0,1)
- koordinat `(x,y,c)`

maka untuk mengakses posisi (x,y,c) dalam satu dimensi digunakan formula `x + y*W + c*W*H`.

## Getting and Setting Pixel

buat dua fungsi berikut untuk mengakses pixel (get dan set)

```C
float get_pixel(image im, int x, int y, int c);
void set_pixel(image im, int x, int y, int c, float v);
```

Dengan memastikan bahwa koordinat yang dimasukkan masih berada di dalam gambar `im`. Dengan aturan:

- `set_pixel` tidak melakukan apa-apa bila koordinatnya invalid
- `get_pixel` melakukan padding pada gambar, dengan strategi `clamp` sehingga bila di request kolom -3 kita mengembalikan kolom 0 dan sebaliknya bila koordinat berlebih.

`set_pixel` bisa ditest dengan cara menggambar warna putih (set semua pixel = 1.0)

## Copying Image

buat juga fungsi untuk mengcopy gambar yang ada. Untuk mempermudah bisa menggunakan `memcpy` dari `<string.h>`. Fungsi ini mengembalikan gambar baru.

## Grayscale Image

Untunk mengubah gambar ke grayscale, gambar perlu memiliki lebih dari satu channel. Dan gambar hasilnya akan hanya tersusun dari warna putih-hitam dan spectrum abu-abu di antaranya.

Walaupun kita bisa mengconvert langsung RGB dengan merata-ratakannya, namun itu tidak dianjurkan karena kita tidak melihat warna dengan merata. Sehingga kita perlu melakukan weighted sum, untuk mengadjust komponen warna yang kita persepsikan lebih terang / gelap dibanding channel lainnya.

<!-- #TODO: Coba tambahin cara-cara untuk convert selain disini -->
Terdapat beberapa pendekatan yang bisa dilakukan mulai dari _gamma compression_ yang digunakan oleh sRGB yang memerlukan warna dalam ruang RGB linear untuk nantinya dihitung _relative luminance_ nya. Namun ada cara lain yang tidak perlu seakurat itu. Yaitu dengan menggunakan apa yang video-engineer sebut sebagai _luma_ untuk melakukan perkiraan dari intensitas warna yang dipersepsikan mata. Hal ini bisa langsung digunakan pada ruang warna yang telah kita miliki, sehingga bisa langsung dihitung:

```
Y' = 0.299 R' + 0.587 G' + 0.114 B'
```

Hal ini bisa langsung kita implementasikan pada fungsi `image rgb_to_grayscale(image im)` karena satu dan lain hal, jadinya untuk grayscale ini ada dua versi, `rg_to_grayscale_expand(image im)` digunakan untuk mempermudah pembuatan gambar ppm.

## Shift Image Color

Selanjutnya kita menambahkan faktor secara konstan ke salah satu channel dari gambar untuk menjadikan pixel tersebut lebih intens. Signature dari fungsinya seperti berikut `void shift_image(image im, int c, float v);`. dengan fungsi ini, semua pixel pada channel `c` akan bertambah intensitasnya sebesar `v`. Nanti kita akan coba untuk mengubah semua channel pada gambar sebesar .4 atau 40%.

> Pada tahap ini bila nilai pixel diluar [0,1] akan dibiarkan (sehingga bisa jadi overflow dan mnejadi gelap)

## Clamping Image Value

Nilai pixel pada gambar perlu dibatasi, karena umumnya gambar disimpan dalam bentuk array of bytes dimana tiap bytesnya merupakan `unsigned byte` dalam tange 0-255. 0 merepresentasikan pixel yang gelap (hitam) dan 255 pixel yang sangat intens.

Walau kita menggunakan float [0,1] untuk merepresentasikan gambar, kita masih perlu mengconvert nilai tersebut ke bytes saat akan disimpan sebagai gambar. Bila saat disimpan nilainya melebihi batas bytes, nilai tersebut akan overflow dan kembali ke nilai yang sangat kecil. Oleh karena itu pixel yang overflow menjadi gelap.

Untuk memastikan gambar tetap berada pada range [0,1] kita perlu mengimplementasi clamping. Dengan signature `void clamp_image(image im);` dengan memodifikasi gambar langsung in-place.

## RGB to HSV

Selama ini kita menggunakan color space RGB (red green blue), terdapat color space lain yang lebih memudahkan dalam melakukan manipulasi gambar, yaitu Hue Saturation dan Value. Untuk menubah warna dari satu space ke yang lain bisa digunakan rumus berikut:

``` python
# Menghitung Value
V = max(R,G,B)

# Menghitung Saturation
m = min(R,G,B)
C = V - m # it can be seen as contrast
S = C / V # bila V = 0, S = 0

# Menghitung Hue
if (C == 0):
    Hprime = 0
elif (V == R):
    Hprime = (G - B)/C
elif (V == G):
    Hprime = (B - R)/C + 2
elif (V == B):
    Hprime = (R - G)/C + 4

if (Hprime < 0):
    H = Hprime/6 + 1
else:
    H = Hprime/6
```
 ## HSV to RGB

Katanya sih tinggal dibalik, tapi agak repot ternyata. Lengkapnya mah bisa refer ke wikipedianya langsung aja.