// Based on Youtube-Tutorial: https://www.youtube.com/watch?v=entjSp3LIfQ
// Enhanced with openMP


#include <fstream>
#include <iostream>
#include "tga/tga.h"
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
	vector<unsigned char> imageAsChar{};
	imageAsChar.reserve(image.size() * 3);

	//for (vector<int>::iterator it = image.end()-1; it != image.begin(); it--)
	//{
	//	imageAsChar.push_back(*it);
	//	imageAsChar.push_back(*it);
	//	imageAsChar.push_back(*it);
	//}

	for (auto pixel : image)
	{
		imageAsChar.push_back(pixel);
		imageAsChar.push_back(pixel);
		imageAsChar.push_back(pixel);
	}

	tga::TGAImage img = tga::TGAImage{ imageAsChar, 24, imageWidth, imageHeight, 0x1907 };
	//const char* filename = outputFilename.c_str();
	tga::saveTGA(img, outputFilename.c_str());
	//tga::saveTGA(tga::TGAImage{ image, 24, imageWidth, imageHeight, 1 }, outputFilename.c_str());
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
				image.at(y*imageWidth + x) = (n % 256);
				imageAsChar.at(y*imageWidth + 3*x) = (n % 256);
				imageAsChar.at(y*imageWidth + 3*x+1) = (n % 256);
				imageAsChar.at(y*imageWidth + 3*x+2) = (n % 256);
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