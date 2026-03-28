[README](README.md) | [Keymap](KEYMAP.md) | [Parts](PARTS.md) | [Fabrication](FABRICATION.md) | [Building](BUILDING.md) | Tricks

# Tricks

## Custom nice!view Peripheral Image

The right (peripheral) half of the keyboard displays a custom image on the nice!view screen. This image is defined as an LVGL C array in `config/boards/shields/nice_view/widgets/art.c` and referenced in `peripheral_status.c`.

### Creating a New Image

1. **Create your artwork at 68x140 pixels** (width x height). This is the natural vertical orientation as you see it on the keyboard.

2. **Make it pure black and white** -- no grayscale, no color. The nice!view is a 1-bit monochrome display. If your source has gradients, dither it to black and white first.

3. **Rotate 90 degrees clockwise** so it becomes **140x68 pixels**. The nice!view stores images in this rotated orientation.

4. **Export as BMP or PNG**.

5. **Convert to LVGL C array** using the [LVGL Image Converter](https://lvgl.io/tools/imageconverter):
   - LVGL version: **v8**
   - Color format: **CF_INDEXED_1_BIT**
   - Output format: **C array**

6. **Fix the palette bytes** for color inversion support. Replace the first 8 bytes of the generated array with:
   ```c
   #if CONFIG_NICE_VIEW_WIDGET_INVERTED
     0xff, 0xff, 0xff, 0xff,   /*Color of index 0*/
     0x00, 0x00, 0x00, 0xff,   /*Color of index 1*/
   #else
     0x00, 0x00, 0x00, 0xff,   /*Color of index 0*/
     0xff, 0xff, 0xff, 0xff,   /*Color of index 1*/
   #endif
   ```

7. **Add to `art.c`** (or replace an existing image). The image descriptor should look like:
   ```c
   const lv_img_dsc_t my_image = {
       .header.cf = LV_IMG_CF_INDEXED_1BIT,
       .header.always_zero = 0,
       .header.reserved = 0,
       .header.w = 140,
       .header.h = 68,
       .data_size = 1232,
       .data = my_image_map,
   };
   ```

8. **Update `peripheral_status.c`** to use your image:
   ```c
   LV_IMG_DECLARE(my_image);
   // ...
   lv_img_set_src(art, &my_image);
   ```

### Layout

The 160x68 display is split into two regions:
- **140x68** (left) -- your custom art image
- **68x68** (right) -- status canvas (battery level, BT connection) rendered rotated 90 degrees

### Data Size

For a 140x68 1-bit indexed image:
- Bytes per row: ceil(140 / 8) = 18
- Pixel data: 18 x 68 = 1224 bytes
- Palette: 8 bytes (2 colors x 4 bytes RGBA)
- Total `data_size`: **1232 bytes**
