/* Convert a 320x200 raw PPM file to the format loaded by this program */

#include <stdio.h>

main()
{
    FILE *input, *output;
    char data[256];
    int x, y;

    input = fopen("image.ppm", "r");
    output = fopen("image.raw", "w");
    fgets(data, 256, input);
    fgets(data, 256, input);
    fgets(data, 256, input);
    fgets(data, 256, input);
    fgets(data, 256, input);
    for (y = 0; y < 200; y++)
    {
        for (x = 0; x < 320; x++)
        {
            unsigned short c;

            c = ((fgetc(input) >> 3) << 11) + ((fgetc(input) >> 2) << 5) + ((fgetc(input) >> 3));
            if (y % 2)
            {
                fputc(c >> 8, output);
                fputc(c & 0xFF, output);
            }
        }
    }
    fclose(input);
    fclose(output);
}
