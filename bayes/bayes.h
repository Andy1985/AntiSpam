#ifndef BAYES_H
#define BAYES_H

#include <math.h>
#include "gsl/gsl_cdf.h"
#include "gsl/gsl_machine.h"

#include <vector>
using std::vector;

#define ROBS 0.0178
#define ROBX 0.52
#define ln(x) log(x)

/* p(w) --word w in spam's probability */
inline static double pw(double bad,double good,double badmsgs,double goodmsgs)
{
	if (bad == 0 || badmsgs == 0) return 0.0;
	if (good == 0 || goodmsgs == 0) return 1.0;

	double pw_r = 0.0;
	pw_r = ((bad / badmsgs)/(bad / badmsgs + good / goodmsgs));

	return pw_r;
}

/* f(w) --robinson's correction factor */
inline static double fw(double bad,double good,double badmsgs,double goodmsgs)
{
	double fw_r = 0.0;
	double n = good + bad;

	double pw_r = pw(bad,good,badmsgs,goodmsgs);
	fw_r = ((ROBS * ROBX) + (n * pw_r)) / (ROBS + n);

	return fw_r;
}

/* sum( ln( f(w) ) )*/
inline static double sumQ(vector<double> fws)
{
	double sum_r = 0.0;
	for (vector<double>::iterator it = fws.begin(); it != fws.end(); ++it)
	{
		sum_r += ln(*it);
	}

	return sum_r;
}

/* sum( ln( 1-f(w) ) ) -- */
inline static double sumP(vector<double> fws)
{
	double sum_r = 0.0;
	for (vector<double>::iterator it = fws.begin(); it != fws.end(); ++it)
	{
		sum_r += ln(1 - *it);
	}

	return sum_r;
}

/* prbx(x,k) */
inline static double prbf(double x, double df)
{
    	double r = gsl_cdf_chisq_Q(x, df);
    	return (r < DBL_EPSILON) ? 0.0 : r;
}

/* S = (1 + Q - P) / 2 */
inline static double spamicity(vector<double> fws)
{
	double P = prbf(-2 * sumP(fws),2 * fws.size());
	double Q = prbf(-2 * sumQ(fws),2 * fws.size());

	return (1.0 + Q - P) / 2;
}
#endif
