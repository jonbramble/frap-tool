AC_INIT([libfraptool],[0.1],[phyjpb@leeds.ac.uk],[frap],[http://mpi.leeds.ac.uk/software/frap])
AC_PREREQ([2.67])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h fraptoolconfig.h])

AM_PATH_GSL(1.13,[],AC_MSG_ERROR(could not find required version of GSL))

PKG_CHECK_MODULES([DEPS], [gsl])

[PLPLOT_LIBS="-lplplotcxxd -lltdl -ldl -lcsirocsa -lqhull -lqsastime -lfreetype"]
[CIMG_LIBS="-L/usr/X11R6/lib -lpthread -lX11"] 
[TIFF_LIBS="-ltiff"]
[BOOST_LIBS="-lboost_thread-mt"]

AC_SUBST([CIMG_LIBS])
AC_SUBST([TIFF_LIBS])
AC_SUBST([PLPLOT_LIBS])
AC_SUBST([BOOST_LIBS])

AC_PROG_CXX
LT_INIT([disable-static])

AC_LANG([C++])
AC_CHECK_HEADERS([iostream, vector, cmath, ctime, cstring])
AC_CHECK_HEADERS(CImg.h)
AC_CHECK_HEADERS(plplot/plstream.h)

AC_SUBST([FRAPTOOL_SO_VERSION], [0:1:0])
AC_SUBST([FRAPTOOL_API_VERSION], [1.0])

AC_CONFIG_FILES([Makefile
		fraptool-${FRAPTOOL_API_VERSION}.pc:fraptool.pc.in])
AC_OUTPUT
