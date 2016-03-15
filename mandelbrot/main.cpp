// Based on Youtube-Tutorial: https://www.youtube.com/watch?v=entjSp3LIfQ
// Enhanced with openMP


#include <fstream>
#include <iostream>
#include "tga.h"
#include <string>
#include <sstream>
#include <ctime>
#include <omp.h>

//#include <GL/gl.h>

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

void savePPMImage(string outputFilename, int imageWidth, int imageHeight, vector<int> image)
{
	// PPM File
	ofstream fout(outputFilename);
	fout << "P3" << endl; // "Magic Number" - PPM file
	fout << imageWidth << " " << imageHeight << endl; // Dimensions
	fout << "255" << endl; // lowerRightYmum value of a pixel R,G,B value..

	for (vector<int>::iterator it = image.begin(); it != image.end(); it++)
	{
		fout << *it << " " << *it << " " << *it << " ";
	}

	fout << endl;
	fout.close();
	//PPM File end
}

//void saveTGAImage(const string &outputFilename, int imageWidth, int imageHeight, vector<unsigned char> image){
void saveTGAImage(const string &outputFilename, int imageWidth, int imageHeight, vector<int> image){
	//Color Mapping

	int colorMapping[16][3] = {
		{ 66, 30, 15 },
		{ 25, 7, 26 },
		{ 9, 1, 47 },
		{ 4, 4, 73 },
		{ 0, 7, 100 },
		{ 12, 44, 138 },
		{ 24, 82, 177 },
		{ 57, 125, 209 },
		{ 134, 181, 229 },
		{ 211, 236, 248 },
		{ 241, 233, 191 },
		{ 248, 201, 95 },
		{ 255, 170, 0 },
		{ 204, 128, 0 },
		{ 153, 87, 0 },
		{ 106, 52, 3 }
	};


	//copy pixel for tga format
	vector<int> imageMirrored{};
	imageMirrored.resize(image.size());
	for (int r = 0; r < image.size(); r+= imageWidth)
	{
		for (int p = 0; p < imageWidth; p++)
		{
			imageMirrored.at(r + p) = image.at(image.size() - (r+imageWidth) + p);
		}
	}

	vector<unsigned char> imageAsChar{};
	imageAsChar.reserve(imageMirrored.size() * 3);

	for (auto pixel : imageMirrored)
	{
		if (pixel >= 255){
			imageAsChar.push_back(0);
			imageAsChar.push_back(0);
			imageAsChar.push_back(0);
		}
		else{
			imageAsChar.push_back(colorMapping[pixel % 16][0]);
			imageAsChar.push_back(colorMapping[pixel % 16][1]);
			imageAsChar.push_back(colorMapping[pixel % 16][2]);
		}
	}
	// 0x1907 = GL_RGB
	tga::TGAImage img = tga::TGAImage{ imageAsChar, 24, imageWidth, imageHeight, 0x1907 };
	tga::saveTGA(img, outputFilename.c_str());
}

enum filetype{
	PPM,
	TGA,
};

int main()
{
	filetype filetype = TGA;
	string filename = "";
	std::cout << "Please type filename: ";
	cin >> filename;

	if (filename == "x")
	{
		filename = "input";
	}

	string inputfilename = filename+".spec";	

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
	vector<unsigned char> imageAsChar;
	imageAsChar.resize(imageWidth*imageHeight*3);

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
				int n = findMandelbrot(cr, ci, maxN);
				//image.at(y*imageWidth + x) = (n % 256);
				image.at(y*imageWidth + x) = n;
			}
		}
	}
	std::cout << "Time taken in millisecs for mandelcalculation: " << clock() - start << endl;

	switch (filetype)
	{
		case PPM: 
			savePPMImage(filename + ".ppm", imageWidth, imageHeight, image);
			break;
		case TGA:
			saveTGAImage(filename + ".tga", imageWidth, imageHeight, image);
			break;
		default:
			break;
	}

	std::cout << "Finished!" << endl;
	// stop stopwatch
	std::cout << "Time taken in millisecs all: " << clock() - start << endl;
	std::system("pause");
	return 0;
}