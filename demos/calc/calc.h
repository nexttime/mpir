#ifndef BISON_CALC_H
# define BISON_CALC_H

#ifndef YYSTYPE
typedef union {
  char  *str;
  int   var;
} yystype;
# define YYSTYPE yystype
#endif
# define	EOS	257
# define	BAD	258
# define	HELP	259
# define	HEX	260
# define	DECIMAL	261
# define	QUIT	262
# define	ABS	263
# define	BIN	264
# define	FIB	265
# define	GCD	266
# define	KRON	267
# define	LCM	268
# define	LUCNUM	269
# define	NEXTPRIME	270
# define	POWM	271
# define	ROOT	272
# define	SQRT	273
# define	NUMBER	274
# define	VARIABLE	275
# define	LOR	276
# define	LAND	277
# define	EQ	278
# define	NE	279
# define	LE	280
# define	GE	281
# define	LSHIFT	282
# define	RSHIFT	283
# define	UMINUS	284


extern YYSTYPE yylval;

#endif /* not BISON_CALC_H */
