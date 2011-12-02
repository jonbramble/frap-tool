AC_INIT([Frap],[0.1],[phyjpb@leeds.ac.uk],[frap],[http://mpi.leeds.ac.uk/software/frap])
AC_PREREQ([2.67])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])
AC_CONFIG_HEADERS([config.h])

AM_PATH_GSL(1.13,[],AC_MSG_ERROR(could not find required version of GSL))

[PLPLOT_LIBS="-lplplotcxxd -lltdl -ldl -lcsirocsa -lcsironn -lqhull -lqsastime -lfreetype"]
[BOOST_LIBS="-lboost_thread"]
[CIMG_LIBS="-L/usr/X11R6/lib -lpthread -lX11"] 
[TIFF_LIBS="-ltiff"]

AC_SUBST([BOOST_LIBS])
AC_SUBST([CIMG_LIBS])
AC_SUBST([TIFF_LIBS])
AC_SUBST([PLPLOT_LIBS])

AC_PROG_CXX
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
