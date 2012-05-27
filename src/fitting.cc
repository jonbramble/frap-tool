/*
 * fitting.cc
 * Copyright (C) Jonathan Bramble 2011
 * 
frap-tool is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * frap-tool is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "../include/fitting.h"

namespace FrapTool {

void Fitting::print_state (size_t iter, gsl_multifit_fdfsolver * s)
     {
       printf ("iter: %3u x = % 15.8f % 15.8f % 15.8f % 15.8f"
               "|f(x)| = %g\n",
               iter,
               gsl_vector_get (s->x, 0), 
               gsl_vector_get (s->x, 1),
               gsl_vector_get (s->x, 2), 
	       gsl_vector_get (s->x, 3),	
               gsl_blas_dnrm2 (s->f));
     }

int Fitting::gaussfit(gsl_matrix * vdata, gsl_matrix * verr, const gsl_matrix * m){
	
	int l = (int)m->size1;  //images
	int j = (int)m->size2;  //pixels in selection

	const size_t n = j;	//number of points
       	const size_t p = 4;	//number of variables

	//printf("images: %i\nPixels: %i\n",l,j);

	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;
	gsl_multifit_function_fdf f;

	double y[n],sigma[n]; //set sigma to one for no weighting?
	struct data d = {n,y,sigma};
	double x_init[4] = { 10000, j/2, 30, 0 }; // A mu lambda b   set these from config file

	f.f = &Fitting::gauss_f;
       	f.df = &Fitting::gauss_df;
       	f.fdf = &Fitting::gauss_fdf;
       	f.n = n;
       	f.p = p;
       	f.params = &d;	

       	int status;
       	unsigned int it, iter = 0;
       
	gsl_matrix *covar = gsl_matrix_alloc (p, p);

	for(int k = 0; k < l; k++){ 			// can make faster loop
       		gsl_vector_view x = gsl_vector_view_array (x_init, p); //whats this for?
	
       		for (it = 0; it < n; it++)
        	{
           		double t = it;
           		y[it] = gsl_matrix_get(m,k,it);	//auto offset of 1000
           		sigma[it] = 1;				// what should this be?
			//if(k==0) printf("data: %u %g %g\n", it, y[it], sigma[it]);
        	}

		T = gsl_multifit_fdfsolver_lmsder; //Levenberg-Marquardt
       		s = gsl_multifit_fdfsolver_alloc (T, n, p);

		gsl_multifit_fdfsolver_set (s, &f, &x.vector);
     
        	Fitting::print_state (iter, s);

	
       		do			// the iteration
         	{
           		iter++;
           		status = gsl_multifit_fdfsolver_iterate (s);
     
           		printf ("status = %s\n", gsl_strerror (status));
     
           		Fitting::print_state (iter, s);
     
           		if (status)
             		break;
     
           		status = gsl_multifit_test_delta (s->dx, s->x, 1e-5, 1e-5);
         	}
       		while (status == GSL_CONTINUE && iter < 750);

	  	gsl_multifit_covar (s->J, 0.0, covar);
     
     #define FIT(i) gsl_vector_get(s->x, i)
     #define ERR(i) sqrt(gsl_matrix_get(covar,i,i))
     
       { 
         double chi = gsl_blas_dnrm2(s->f);
         double dof = n - p;
         double c = GSL_MAX_DBL(1, chi / sqrt(dof)); 
     	
	gsl_matrix_set(vdata,0,k,FIT(0));
	gsl_matrix_set(verr,0,k,c*ERR(0));

	gsl_matrix_set(vdata,1,k,FIT(1));
	gsl_matrix_set(verr,1,k,c*ERR(1));

	gsl_matrix_set(vdata,2,k,FIT(2));
	gsl_matrix_set(verr,2,k,c*ERR(2));

	gsl_matrix_set(vdata,3,k,FIT(3));
	gsl_matrix_set(verr,3,k,c*ERR(3));
	
         printf("chisq/dof = %g\n",  pow(chi, 2.0) / dof);
     
         printf ("A      = %.5f +/- %.5f\n", FIT(0), c*ERR(0));
         printf ("mu = %.5f +/- %.5f\n", FIT(1), c*ERR(1));
         printf ("lambda      = %.5f +/- %.5f\n", FIT(2), c*ERR(2));
	 printf ("b      = %.5f +/- %.5f\n", FIT(3), c*ERR(3));
       }
     
       printf ("status = %s\n", gsl_strerror (status));

	for(uint q=0;q<p;q++) x_init[q] = FIT(q); // A mu lambda - use previous data as guess

	iter = 0; // reset interation value

	}
	gsl_multifit_fdfsolver_free (s);
       	gsl_matrix_free (covar);

	return GSL_SUCCESS;
	
}

int Fitting::gauss_f(const gsl_vector * x, void *data, gsl_vector * f)
{
       size_t n = ((struct data *)data)->n;
       double *y = ((struct data *)data)->y;
       double *sigma = ((struct data *) data)->sigma;
     
       double A = gsl_vector_get (x, 0);
       double mu = gsl_vector_get (x, 1);
       double lambda = gsl_vector_get (x, 2);
       double b = gsl_vector_get (x, 3);
     
       for (size_t i = 0; i < n; i++)
         {     
           double t = i;
	   double Yi = A*exp((-1*pow(t-mu,2))/(2*pow(lambda,2)))+b; 
           gsl_vector_set(f,i,(Yi-y[i])/sigma[i]);
         }
     
       return GSL_SUCCESS;
}

int Fitting::gauss_df(const gsl_vector * x, void *data, gsl_matrix * J)
{
	size_t n = ((struct data *)data)->n;
	double *sigma = ((struct data *) data)->sigma;
     
	double A = gsl_vector_get (x, 0);
	double mu = gsl_vector_get (x, 1);
	double lambda = gsl_vector_get (x, 2);
	double b = gsl_vector_get (x, 3);
     
	for (size_t i = 0; i < n; i++)
	{
           	/* Jacobian matrix J(i,j) = dfi / dxj, */
           	/* where fi = (Yi - yi)/sigma[i],      */
           	/*       Yi = A * exp(-lambda * i) + b  */
           	/* and the xj are the parameters (A,lambda,b) */ //incorrect - update this text 
		double t = i;
           	double s = sigma[i];
           	double e = exp((-1*pow(t-mu,2))/(2*pow(lambda,2)));
           	gsl_matrix_set (J, i, 0, e/s ); 
           	gsl_matrix_set (J, i, 1, (A*e*(t-mu)/pow(lambda,2))/s   );
           	gsl_matrix_set (J, i, 2, (A*e*pow(t-mu,2)/pow(lambda,3))/s  );
     	   	gsl_matrix_set (J, i, 3, 1/s );
         }
       	return GSL_SUCCESS;
}

int Fitting::gauss_fdf(const gsl_vector * x, void * data, gsl_vector * f, gsl_matrix * J)
{
	gauss_f(x, data, f);
        gauss_df(x, data, J);
     
	return GSL_SUCCESS;
}

int Fitting::linearfit(const double * x, const double * y, int n){
	
	double c0, c1, cov00, cov01, cov11, sumsq;

	gsl_fit_linear (x, 1, y, 1, n, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

	printf ("# best fit: Y = %g + %g X\n", c0, c1);
        printf ("# covariance matrix:\n");
        printf ("# [ %g, %g\n#   %g, %g]\n", cov00, cov01, cov01, cov11);
        printf ("# sumsq = %g\n", sumsq);

	return GSL_SUCCESS;
}

} // FrapTool

