/* gen.c -- Generate pseudorandom numbers.

Copyright 1999, 2000, 2002 Free Software Foundation, Inc.

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

/* Examples:

  $ gen 10
10 integers 0 <= X < 2^32 generated by mpz_urandomb()

  $ gen -f mpf_urandomb 10
10 real numbers 0 <= X < 1

  $ gen -z 127 10
10 integers 0 <= X < 2^127

  $ gen -f mpf_urandomb -x .9,1 10
10 real numbers 0 <= X < .9

  $ gen -s 1 10
10 integers, sequence seeded with 1

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#if !HAVE_DECL_OPTARG
extern char *optarg;
extern int optind, opterr;
#endif

#include "gmp.h"
#include "gmp-impl.h"

int main (argc, argv)
     int argc;
     char *argv[];
{
  const char usage[] =
    "usage: gen [-bhpq] [-a n] [-c a,c,m2exp] [-C a,c,m] [-f func] [-g alg] [-m n] [-s n] " \
    "[-x f,t] [-z n] [n]\n" \
    "  n        number of random numbers to generate\n" \
    "  -a n     ASCII output in radix n (default, with n=10)\n" \
    "  -b       binary output\n" \
    "  -c a,c,m2exp use supplied LC scheme\n" \
    "  -f func  random function, one of\n" \
    "           mpz_urandomb (default), mpz_urandomm, mpf_urandomb, rand, random\n" \
    "  -g alg   algorithm, one of mt (default), lc\n" \
    "  -h       print this text and exit\n" \
    "  -m n     maximum size of generated number plus 1 (0<= X < n) for mpz_urandomm\n" \
    "  -p       print used seed on stderr\n" \
    "  -q       quiet, no output\n" \
    "  -s n     initial seed (default: output from time(3))\n" \
    "  -x f,t   exclude all numbers f <= x <= t\n" \
    "  -z n     size in bits of generated numbers (0<= X <2^n) (default 32)\n" \
    "";

  unsigned long int f;
  unsigned long int n = 0;
  unsigned long int seed;
  unsigned long int m2exp = 0;
  unsigned int size = 32;
  int seed_from_user = 0;
  int ascout = 1, binout = 0, printseed = 0;
  int output_radix = 10;
  int lc_scheme_from_user = 0;
  int quiet_flag = 0;
  mpz_t z_seed;
  mpz_t z1;
  mpf_t f1;
  gmp_randstate_t rstate;
  int c, i;
  double drand;
  long lrand;
  int do_exclude = 0;
  mpf_t f_xf, f_xt;		/* numbers to exclude from sequence */
  char *str_xf, *str_xt;	/* numbers to exclude from sequence */
  char *str_a, *str_adder, *str_m;
  mpz_t z_a, z_m, z_mmax;
  unsigned long int ul_adder;

  enum
  {
    RFUNC_mpz_urandomb = 0,
    RFUNC_mpz_urandomm,
    RFUNC_mpf_urandomb,
    RFUNC_rand,
    RFUNC_random,
  } rfunc = RFUNC_mpz_urandomb;
  char *rfunc_str[] =  { "mpz_urandomb", "mpz_urandomm", "mpf_urandomb",
			 "rand", "random" };
  enum
  {
    RNG_MT = 0,
    RNG_LC
  };
  gmp_randalg_t ralg = RNG_MT;
  /* Texts for the algorithms.  The index of each must match the
     corresponding algorithm in the enum above.  */
  char *ralg_str[] = { "mt", "lc" };

  mpf_init (f_xf);
  mpf_init (f_xt);
  mpf_init (f1);
  mpz_init (z1);
  mpz_init (z_seed);
  mpz_init_set_ui (z_mmax, 0);


  while ((c = getopt (argc, argv, "a:bc:f:g:hm:n:pqs:z:x:")) != -1)
    switch (c)
      {
      case 'a':
	ascout = 1;
	binout = 0;
	output_radix = atoi (optarg);
	break;

      case 'b':
	ascout = 0;
	binout = 1;
	break;

      case 'c':			/* User supplied LC scheme: a,c,m2exp */
	if (NULL == (str_a = strtok (optarg, ","))
	    || NULL == (str_adder = strtok (NULL, ","))
	    || NULL == (str_m = strtok (NULL, ",")))
	  {
	    fprintf (stderr, "gen: bad LC scheme parameters: %s\n", optarg);
	    exit (1);
	  }
#ifdef HAVE_STRTOUL
	ul_adder = strtoul (str_adder, NULL, 0);
#elif HAVE_STRTOL
	ul_adder = (unsigned long int) strtol (str_adder, NULL, 0);
#else
	ul_adder = (unsigned long int) atoi (str_adder);
#endif
	
	if (mpz_init_set_str (z_a, str_a, 0))
	  {
	    fprintf (stderr, "gen: bad LC scheme parameter `a': %s\n", str_a);
	    exit (1);
	  }
	if (ULONG_MAX == ul_adder)
	  {
	    fprintf (stderr, "gen: bad LC scheme parameter `c': %s\n",
		     str_adder);
	    exit (1);
	  }
	m2exp = atol (str_m);

	lc_scheme_from_user = 1;
	break;


      case 'f':
	rfunc = -1;
	for (f = 0; f < sizeof (rfunc_str) / sizeof (*rfunc_str); f++)
	    if (!strcmp (optarg, rfunc_str[f]))
	      {
		rfunc = f;
		break;
	      }
	if (rfunc == -1)
	  {
	    fputs (usage, stderr);
	    exit (1);
	  }
	break;

      case 'g':			/* algorithm */
	ralg = -1;
	for (f = 0; f < sizeof (ralg_str) / sizeof (*ralg_str); f++)
	    if (!strcmp (optarg, ralg_str[f]))
	      {
		ralg = f;
		break;
	      }
	if (ralg == -1)
	  {
	    fputs (usage, stderr);
	    exit (1);
	  }
	break;

      case 'm':			/* max for mpz_urandomm() */
	if (mpz_set_str (z_mmax, optarg, 0))
	  {
	    fprintf (stderr, "gen: bad max value: %s\n", optarg);
	    exit (1);
	  }
	break;

      case 'p':			/* print seed on stderr */
	printseed = 1;
	break;

      case 'q':			/* quiet */
	quiet_flag = 1;
	break;

      case 's':			/* user provided seed */
	if (mpz_set_str (z_seed, optarg, 0))
	  {
	    fprintf (stderr, "gen: bad seed argument %s\n", optarg);
	    exit (1);
	  }
	seed_from_user = 1;
	break;

      case 'z':
	size = atoi (optarg);
	if (size < 1)
	  {
	    fprintf (stderr, "gen: bad size argument (-z %u)\n", size);
	    exit (1);
	  }
	break;

      case 'x':			/* Exclude. from,to */
	str_xf = optarg;
	str_xt = strchr (optarg, ',');
	if (NULL == str_xt)
	  {
	    fprintf (stderr, "gen: bad exclusion parameters: %s\n", optarg);
	    exit (1);
	  }
	*str_xt++ = '\0';
	do_exclude = 1;
	break;

      case 'h':
      case '?':
      default:
	fputs (usage, stderr);
	exit (1);
      }
  argc -= optind;
  argv += optind;

  if (! seed_from_user)
    mpz_set_ui (z_seed, (unsigned long int) time (NULL));
  seed = mpz_get_ui (z_seed);
  if (printseed)
    {
      fprintf (stderr, "gen: seed used: ");
      mpz_out_str (stderr, output_radix, z_seed);
      fprintf (stderr, "\n");
    }

  mpf_set_prec (f1, size);

  /* init random state and plant seed */
  switch (rfunc)
    {
    case RFUNC_mpf_urandomb:
#if 0
      /* Don't init a too small generator.  */
      size = PREC (f1) * BITS_PER_MP_LIMB;
      /* Fall through.  */
#endif
    case RFUNC_mpz_urandomb:
    case RFUNC_mpz_urandomm:
      switch (ralg)
	{
	case RNG_MT:
	  gmp_randinit_mt (rstate);
	  break;

	case RNG_LC:
	  if (! lc_scheme_from_user)
	    gmp_randinit_lc_2exp_size (rstate, MIN (128, size));
	  else
	    gmp_randinit_lc_2exp (rstate, z_a, ul_adder, m2exp);
	  break;

	default:
	  fprintf (stderr, "gen: unsupported algorithm\n");
	  exit (1);
	}

      gmp_randseed (rstate, z_seed);
      break;

    case RFUNC_rand:
      srand (seed);
      break;

    case RFUNC_random:
#ifdef __FreeBSD__		/* FIXME */
      if (seed_from_user)
	srandom (seed);
      else
	srandomdev ();
#else
      fprintf (stderr, "gen: unsupported algorithm\n");
#endif
      break;

    default:
      fprintf (stderr, "gen: random function not implemented\n");
      exit (1);
    }

  /* set up excludes */
  if (do_exclude)
    switch (rfunc)
      {
      case RFUNC_mpf_urandomb:

	if (mpf_set_str (f_xf, str_xf, 10) ||
	    mpf_set_str (f_xt, str_xt, 10))
	  {
	    fprintf (stderr, "gen: bad exclusion-from (\"%s\") " \
		     "or exclusion-to (\"%s\") string.  no exclusion done.\n",
		     str_xf, str_xt);
	    do_exclude = 0;
	  }
	break;

      default:
	fprintf (stderr, "gen: exclusion not implemented for chosen " \
		 "randomization function.  all numbers included in sequence.\n");
      }

  /* generate and print */
  if (argc > 0)
    {
#if HAVE_STRTOUL
      n = strtoul (argv[0], (char **) NULL, 10);
#elif HAVE_STRTOL
      n = (unsigned long int) strtol (argv[0], (char **) NULL, 10);
#else
      n = (unsigned long int) atoi (argv[0]);
#endif
    }

  for (f = 0; n == 0 || f < n; f++)
    {
      switch (rfunc)
	{
	case RFUNC_mpz_urandomb:
	  mpz_urandomb (z1, rstate, size);
	  if (quiet_flag)
	    break;
	  if (binout)
	    {
	      /*fwrite ((unsigned int *) z1->_mp_d, 4, 1, stdout);*/
	      fprintf (stderr, "gen: binary output for mpz_urandom* is broken\n");
	      exit (1);
	    }
	  else
	    {
	      mpz_out_str (stdout, output_radix, z1);
	      puts ("");
	    }
	  break;

	case RFUNC_mpz_urandomm:
	  mpz_urandomm (z1, rstate, z_mmax);
	  if (quiet_flag)
	    break;
	  if (binout)
	    {
	      /*fwrite ((unsigned int *) z1->_mp_d, 4, 1, stdout);*/
	      fprintf (stderr, "gen: binary output for mpz_urandom* is broken\n");
	      exit (1);
	    }
	  else
	    {
	      mpz_out_str (stdout, output_radix, z1);
	      puts ("");
	    }
	  break;

	case RFUNC_mpf_urandomb:
	  mpf_urandomb (f1, rstate, size);
	  if (do_exclude)
	    if (mpf_cmp (f1, f_xf) >= 0 && mpf_cmp (f1, f_xt) <= 0)
		break;
	  if (quiet_flag)
	    break;
	  if (binout)
	    {
	      fprintf (stderr, "gen: binary output for floating point numbers "\
		       "not implemented\n");
	      exit (1);
	    }
	  else
	    {
	      mpf_out_str (stdout, output_radix, 0, f1);
	      puts ("");
	    }
	  break;

	case RFUNC_rand:
	  i = rand ();
#ifdef FLOAT_OUTPUT
	  if (i)
	    drand = (double) i / (double) RAND_MAX;
	  else
	    drand = 0.0;
	  if (quiet_flag)
	    break;
	  if (binout)
	    fwrite (&drand, sizeof (drand), 1, stdout);
	  else
	    printf ("%e\n", drand);
#else
	  if (quiet_flag)
	    break;
	  if (binout)
	    fwrite (&i, sizeof (i), 1, stdout);
	  else
	    printf ("%d\n", i);
#endif
	  break;

	case RFUNC_random:
	  lrand = random ();
	  if (lrand)
	    drand = (double) lrand / (double) 0x7fffffff;
	  else
	    drand = 0;
	  if (quiet_flag)
	    break;
	  if (binout)
	    fwrite (&drand, sizeof (drand), 1, stdout);
	  else
	    printf ("%e\n", drand);
	  break;

	default:
	  fprintf (stderr, "gen: random function not implemented\n");
	  exit (1);
	}

    }

  /* clean up */
  switch (rfunc)
    {
    case RFUNC_mpz_urandomb:
    case RFUNC_mpf_urandomb:
      gmp_randclear (rstate);
      break;
    default:
      break;
    }
  mpf_clear (f1);
  mpf_clear (f_xf); 
  mpf_clear (f_xt);
  mpz_clear (z1);
  mpz_clear (z_seed);

  return 0;
}

static void *debug_dummyz = mpz_dump;
static void *debug_dummyf = mpf_dump;
