// Based on Youtube-Tutorial: https://www.youtube.com/watch?v=entjSp3LIfQ
// Enhanced with openMP


#include <fstream>
#include <iostream>
#include "tga/tga.h"
#include <string>
#include <sstream>
#include <ctime>
#include <omp.h>

using namespace std;

int findMandelbrot(double cr, double ci, int max_iterations)
{
	int i = 0;
	double zr = 0.0, zi = 0.0;
	while (i < max_iterations && zr * zr + zi * zi < 4.0)
	{
		double temp = zr * zr - zi * zi + cr;
		zi = 2.0 * zr * zi + ci;
		zr = temp;
		i++;
	}

	return i;
}

double mapToReal(int x, int imageWidth, double leftUpperX, double lowerRightX)
{
	double range = lowerRightX - leftUpperX;
	return x * (range / imageWidth) + leftUpperX;
}

double mapToImaginary(int y, int imageHeight, double leftUpperY, double lowerRightY)
{
	double range = lowerRightY - leftUpperY;
	return y * (range / imageHeight) + leftUpperY;
}

int main()
{
	string filename = "";
	std::cout << "Please type filename: ";
	cin >> filename;

	if (filename == "x")
	{
		filename = "input";
	}

	string inputfilename = filename+".spec";
	string outputFilename = filename + ".ppm";

	// Get the required input values from file...
	ifstream fin(inputfilename);
	int imageWidth, imageHeight, maxN;
	double leftUpperX, lowerRightX, leftUpperY, lowerRightY;

	if (!fin)
	{
		std::cout << "Could not open file!" << endl;
		return 1;
	}
	fin >> imageWidth >> imageHeight ;
	fin >> leftUpperX >> leftUpperY >> lowerRightX >> lowerRightY;
	fin >> maxN;
	fin.close();

	//stopwatch
	unsigned int start = clock();

	const int numberOfMaxThreads = 4;

	vector<int> threadParts[numberOfMaxThreads];
	vector<int> image;
	image.resize(imageWidth*imageHeight);

	//// init vectors
	//for (int i = 0; i < sizeof(threadParts); i++)
	//{
	//}

	omp_set_num_threads(numberOfMaxThreads);
	int nrThreads;
	#pragma omp parallel
	{
		int threadId;
		#pragma omp barrier
		{
			threadId = omp_get_thread_num();
			//get number of threats
			if (threadId == 0) {
				nrThreads = omp_get_num_threads();
			}
		}
		// begin mandelbrot
		for (int y = threadId; y < imageHeight; y += omp_get_num_threads()) // Rows...
		{
			for (int x = 0; x < imageWidth; x++) // Pixels in row (columns)...
			{
				// ... Find the real and imaginary values for c, corresponding to that
				//     x, y pixel in the image.
				double cr = mapToReal(x, imageWidth, leftUpperX, lowerRightX);
				double ci = mapToImaginary(y, imageHeight, leftUpperY, lowerRightY);

				// ... Find the number of iterations in the Mandelbrot formula
				//     using said c.
				int n = findMandelbrot(cr, ci, maxN);
				//threadParts[threadId].push_back(n % 256);
				image.at(y*imageWidth + x) = (n % 256);
				// ... Map the resulting number to an RGP value
				//int r = (n % 256);
				//int g = (n % 256);
				//int b = (n % 256);

				//// ... Output it to an image
				//fout << r << " " << g << " " << b << " ";
			}
			//fout << endl;
		}
	}
	std::cout << "Time taken in millisecs for mandelcalculation: " << clock() - start << endl;
	// Open the output file, write the PPM header...
	ofstream fout(outputFilename);
	fout << "P3" << endl; // "Magic Number" - PPM file
	fout << imageWidth << " " << imageHeight << endl; // Dimensions
	fout << "255" << endl; // lowerRightYmum value of a pixel R,G,B value..



	//range = sizeof(th[0]) / w; // =6
	//for (i = 0; i < sizeof(th[0]) / w; i++)
	//{
	//	from = i* w
	//		to = (i + 1)*w - 1;
	//	for ()
	//		image = th[i][i*numTh - i*numTh + width];
	//}

	//// create image
	//for (int i = 0; i < (threadParts[0].size() / imageWidth); i++)
	//{
	//	int from = i*imageWidth;
	//	int to = (i + 1)*imageWidth - 1;

	//	for (int j = 0; j < nrThreads; j++)
	//	{
	//		if (threadParts[j].size() > from)
	//		{
	//			for (vector<int>::iterator it = threadParts[j].begin()+from; it != threadParts[j].begin()+to; it++)
	//			{
	//				fout << *it << " " << *it << " " << *it << " ";
	//			}
	//		}
	//	}
	//}

	for (vector<int>::iterator it = image.begin(); it != image.end(); it++)
	{
		fout << *it << " " << *it << " " << *it << " ";
	}

	fout << endl;
	fout.close();

	std::cout << "Finished!" << endl;
	// stop stopwatch
	std::cout << "Time taken in millisecs all: " << clock() - start << endl;
	std::system("pause");
	return 0;
}