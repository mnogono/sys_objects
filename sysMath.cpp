//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace sysMath {

	double Max(int count, ...){
    	double max = -DBL_MIN;
		va_list arguments;
        va_start(arguments, count);
        double val;
        for (int i = 0; i < count; ++i) {
			val = va_arg(arguments, double);
            if (val > max) max = val;
        }
		va_end(arguments);
        return max;
    }

    double Min(int count, ...){
    	double min = DBL_MAX;
		va_list arguments;
        va_start(arguments, count);
        double val;
        for (int i = 0; i < count; ++i) {
			val = va_arg(arguments, double);
            if (val < min) min = val;
        }
		va_end(arguments);
        return min;
	}

	double * GaussKernal(int gaussWidth, double sigma){
		int len = 2 * gaussWidth + 1;
		double *gaussKernal = new double[len];

		// set standard deviation to 1.0
		double r, s = 2.0 * sigma * sigma;

		// sum is for normalization
		double sum = 0.0;

		for (int x = -gaussWidth; x <= gaussWidth; ++x) {
			gaussKernal[x + gaussWidth] = exp(-( x * x / s))/(M_PI * s);
			sum += gaussKernal[x + gaussWidth];
		}

		// normalize the Kernel
		for(int x = -gaussWidth; x <= gaussWidth; ++x) {
			gaussKernal[x + gaussWidth] /= sum;
		}

		return gaussKernal;
	}

	void ApplyGaussFilter(double *gaussKernal, int gaussWidth, double *data, size_t size) {
		if (size < 2 * gaussWidth) {
			return;
		}

		double *dataOriginal = new double[size];
		memcpy(&dataOriginal[0], &data[0], size * sizeof(double));

		double sum;
		int x;
		size_t i;
		size_t len;

		size_t index;
		for (i = 0; i < gaussWidth; ++i) {
			sum = 0.0;
			for (x = -gaussWidth; x <= gaussWidth; ++x) {
				index = abs(i + x);
				sum += dataOriginal[index] * gaussKernal[x + gaussWidth];
			}
			data[i] = sum;
		}

		for (i = gaussWidth, len = size - gaussWidth; i < len; ++i) {
			sum = 0.0;
			for (x = -gaussWidth; x <= gaussWidth; ++x) {
				sum += dataOriginal[i + x] * gaussKernal[x + gaussWidth];
			}
			data[i] = sum;
		}

		for (i = size - gaussWidth; i < size; ++i) {
			sum = 0.0;
			for (x = -gaussWidth; x <= 0; ++x) {
				sum += dataOriginal[i + x] * gaussKernal[x + gaussWidth];
			}
			for (x = 1; x < gaussWidth; ++x) {
				index = size - abs(size - i - x) - 1;
				sum += dataOriginal[index] * gaussKernal[x + gaussWidth];
			}
			data[i] = sum;
		}

		delete []dataOriginal;
	}
}
