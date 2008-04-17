/* Intel P6/mmx gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006
Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA. */


#define BITS_PER_MP_LIMB 32
#define BYTES_PER_MP_LIMB 4


/* NOTE: In a fat binary build SQR_KARATSUBA_THRESHOLD here cannot be more
   than the value in mpn/x86/p6/gmp-mparam.h.  The latter is used as a hard
   limit in mpn/x86/p6/sqr_basecase.asm.  */


/* 1867 MHz Pentium 3/M */

/* Generated by tuneup.c, 2006-03-21, gcc 3.4 */

#define MUL_KARATSUBA_THRESHOLD          22
#define MUL_TOOM3_THRESHOLD              74

#define SQR_BASECASE_THRESHOLD            0  /* always (native) */
#define SQR_KARATSUBA_THRESHOLD          42
#define SQR_TOOM3_THRESHOLD             113

#define MULLOW_BASECASE_THRESHOLD         4
#define MULLOW_DC_THRESHOLD              56
#define MULLOW_MUL_N_THRESHOLD          414

#define DIV_SB_PREINV_THRESHOLD           0  /* always */
#define DIV_DC_THRESHOLD                 64
#define POWM_THRESHOLD                  101

#define GCD_ACCEL_THRESHOLD               3
#define GCDEXT_THRESHOLD                 63
#define JACOBI_BASE_METHOD                1

#define USE_PREINV_DIVREM_1               1  /* native */
#define USE_PREINV_MOD_1                  1  /* native */
#define DIVREM_2_THRESHOLD                0  /* always */
#define DIVEXACT_1_THRESHOLD              0  /* always (native) */
#define MODEXACT_1_ODD_THRESHOLD          0  /* always (native) */

#define GET_STR_DC_THRESHOLD             21
#define GET_STR_PRECOMPUTE_THRESHOLD     24
#define SET_STR_THRESHOLD              3625

#define MUL_FFT_TABLE  { 464, 928, 1920, 4608, 10240, 24576, 98304, 393216, 1572864, 6291456, 0 }
#define MUL_FFT_MODF_THRESHOLD          480
#define MUL_FFT_THRESHOLD              3328

#define SQR_FFT_TABLE  { 464, 928, 1920, 5632, 14336, 40960, 98304, 393216, 1572864, 6291456, 0 }
#define SQR_FFT_MODF_THRESHOLD          480
#define SQR_FFT_THRESHOLD              3840
