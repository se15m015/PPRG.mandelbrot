// Based on Youtube-Tutorial: https://www.youtube.com/watch?v=entjSp3LIfQ
// Enhanced with openMP


#include <fstream>
#include <iostream>
#include "tga/tga.h"
#include <string>
#include <sstream>

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
	cout << "Please type filename: ";
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
		cout << "Could not open file!" << endl;
		return 1;
	}
	fin >> imageWidth >> imageHeight ;
	//Original: fin >> leftUpperX >> lowerRightX >> leftUpperY >> lowerRightY;
	//Used: fin >> leftUpperX >> leftUpperY >> lowerRightX >> lowerRightY;
	fin >> leftUpperX >> leftUpperY >> lowerRightX >> lowerRightY; //rename
	fin >> maxN;
	fin.close(); // Not necessary, good practice :D

	// Open the output file, write the PPM header...
	ofstream fout(outputFilename);
	fout << "P3" << endl; // "Magic Number" - PPM file
	fout << imageWidth << " " << imageHeight << endl; // Dimensions
	fout << "255" << endl; // lowerRightYmum value of a pixel R,G,B value...

	// For every pixel...
	for (int y = 0; y < imageHeight; y++) // Rows...
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

			// ... Map the resulting number to an RGP value
			int r = (n % 256);
			int g = (n % 256);
			int b = (n % 256);

			// ... Output it to an image
			fout << r << " " << g << " " << b << " ";
		}
		fout << endl;
	}
	fout.close();

	cout << "Finished!" << endl;
	
	return 0;
}