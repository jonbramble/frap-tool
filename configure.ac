AC_INIT([Frap],[0.1],[phyjpb@leeds.ac.uk],[frap],[http://mpi.leeds.ac.uk/software/frap])
AC_PREREQ([2.67])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])
AC_CONFIG_HEADERS([config.h])
AC_CHECK_HEADER([tiffio.h],
                     [AC_DEFINE([HAVE_TIFFIO_H], 1,
                        [Define to 1 if you have <tiffio.h>.])],
                     [AC_MSG_ERROR([install libtiff4-dev])])

AM_PATH_GSL(1.13,[],AC_MSG_ERROR(could not find required version of GSL))

[OMP_OPTS="-fopenmp"]
[CIMG_LIBS="-L/usr/X11R6/lib -lpthread -lX11"] 
[TIFF_LIBS="-ltiffxx"]
AC_SUBST([OMP_OPTS])
AC_SUBST([CIMG_LIBS])
AC_SUBST([TIFF_LIBS])

AC_PROG_CXX
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
