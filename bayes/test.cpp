#include "bayes.h"

int main(int argc,char* argv[])
{
	vector<double> dv;
	dv.push_back(0.1);
	dv.push_back(0.1);
	dv.push_back(0.1);
	dv.push_back(0.1);

	fprintf(stdout,"%f\n",spamicity(dv));

	exit(0);
}
