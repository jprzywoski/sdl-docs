/*
 *     Program: REALFFTF.C
 *      Author: Philip VanBaren
 *        Date: 2 September 1993
 *
 * Description: These routines perform an FFT on real data.
 *              On a 486/33 compiled using Borland C++ 3.1 with full
 *              speed optimization and a small memory model, a 1024 point
 *              FFT takes about 16ms.
 *              This code is for floating point data.
 *
 *  Note: Output is BIT-REVERSED! so you must use the BitReversed to
 *        get legible output, (i.e. Real_i = buffer[ BitReversed[i] ]
 *                                  Imag_i = buffer[ BitReversed[i]+1 ] )
 *        Input is in normal order.
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "realfftf.h"

int *BitReversed;
fft_type *SinTable;
int Points = 0;

/*
 *  Initialize the Sine table and Twiddle pointers (bit-reversed pointers)
 *  for the FFT routine.
 */
void InitializeFFT(int fftlen)
{
    int i;
    int temp;
    int mask;

    /*
     *  FFT size is only half the number of data points
     *  The full FFT output can be reconstructed from this FFT's output.
     *  (This optimization can be made since the data is real.)
     */
    Points = fftlen / 2;
    if ((SinTable = (fft_type *)malloc(2 * Points * sizeof (fft_type))) == NULL)
    {
        puts("Error allocating memory for Sine table.");
        exit(1);
    }
    if ((BitReversed = (int *)malloc(Points * sizeof (int))) == NULL)
    {
        puts("Error allocating memory for BitReversed.");
        exit(1);
    }
    for (i = 0; i < Points; i++)
    {
        temp = 0;
        for (mask = Points / 2; mask > 0; mask >>= 1)
        {
            temp = (temp >> 1) + (i & mask ? Points : 0);
        }
        BitReversed[i] = temp;
    }
    for (i = 0; i < Points; i++)
    {
        SinTable[BitReversed[i]] = -sin(2 * M_PI * i / (2 * Points));
        SinTable[BitReversed[i] + 1] = -cos(2 * M_PI * i / (2 * Points));
    }
}

/*
 *  Free up the memory allotted for Sin table and Twiddle Pointers
 */
void EndFFT()
{
    free(BitReversed);
    free(SinTable);
    Points = 0;
}

fft_type *A, *B;
fft_type *sptr;
fft_type *endptr1, *endptr2;
int *br1, *br2;
fft_type HRplus, HRminus, HIplus, HIminus;

/*
 *  Actual FFT routine.  Must call InitializeFFT(fftlen) first!
 */
void RealFFT(fft_type *buffer)
{
    int ButterfliesPerGroup = Points / 2;

    endptr1 = buffer + Points * 2;

    /*
     *  Butterfly:
     *     Ain-----Aout
     *         \ /
     *         / \
     *     Bin-----Bout
     */
    while (ButterfliesPerGroup > 0)
    {
        A = buffer;
        B = buffer + ButterfliesPerGroup * 2;
        sptr = SinTable;
        while (A < endptr1)
        {
            register fft_type sin = *sptr;
            register fft_type cos = *(sptr + 1);

            endptr2 = B;
            while (A < endptr2)
            {
                fft_type v1 = *B * cos + *(B + 1) * sin;
                fft_type v2 = *B * sin - *(B + 1) * cos;

                *B = (*A + v1) * 0.5;
                *(A++) = *(B++) - v1;
                *B = (*A - v2) * 0.5;
                *(A++) = *(B++) + v2;
            }
            A = B;
            B += ButterfliesPerGroup * 2;
            sptr += 2;
        }
        ButterfliesPerGroup >>= 1;
    }

    /*
     *  Massage output to get the output for a real input sequence.
     */
    br1 = BitReversed + 1;
    br2 = BitReversed + Points - 1;
    while (br1 <= br2)
    {
        register fft_type temp1;
        register fft_type temp2;
        fft_type sin = SinTable[*br1];
        fft_type cos = SinTable[*br1 + 1];

        A = buffer + *br1;
        B = buffer + *br2;
        HRplus = (HRminus = *A - *B) + (*B * 2);
        HIplus = (HIminus = *(A + 1) - *(B + 1)) + (*(B + 1) * 2);
        temp1 = (sin * HRminus - cos * HIplus);
        temp2 = (cos * HRminus + sin * HIplus);
        *B = (*A = (HRplus + temp1) * 0.5) - temp1;
        *(B + 1) = (*(A + 1) = (HIminus + temp2) * 0.5) - HIminus;

        br1++;
        br2--;
    }

    /*
     *  Handle DC bin separately
     */
    buffer[0] += buffer[1];
    buffer[1] = 0;
}
