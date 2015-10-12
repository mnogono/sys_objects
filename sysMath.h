// ---------------------------------------------------------------------------

#ifndef sysMathH
#define sysMathH

// ---------------------------------------------------------------------------

namespace sysMath {
	/*
    return max of values
    arguments must be double
    */
	double Max(int count, ...);

    /*
    return min of values
    arguments must be double
    */
	double Min(int count, ...);

	/*
	return Gauss Kernal
	allocate memmory for Gauss Kernal
	*/
	double * GaussKernal(int gaussWidth, double sigma);

	/*
	Apply Gauss filter, naitive implementation
	algorithm complex as size * gaussWidth
	for left and right boandary cases using abs(index) when gause index out of index
	*/
	void ApplyGaussFilter(double *gaussKernal, int gaussWidth, double *data, size_t size);
}

#endif
