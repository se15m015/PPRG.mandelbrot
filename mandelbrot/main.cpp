#include <iostream>
#include <fstream>

using namespace std;


double mapToReal(int x, int imageWidth, double minR, double maxR)
{
    double range = maxR - minR;
    return x * (range / imageWidth) + minR;
}

double mapToImaginary(int y, int imageHeight, double minI, double maxI)
{
    double range = maxI - minI;
    return y * (range / imageHeight) + minI;
}

int main()
{
    ifstream fin("input.txt");
    int imageWidth, imageHeight, maxN;
    double minR, maxR, minI, maxI;

    if(!fin)
    {
            count << "Could not open file!" << endl;
            cin.ignore();
            return 0;
    }
    fin >> imageWidth >> imageHeight >> maxN;
    fin >> minR >> maxR >> minI >> maxI;
    fin.close();

    ofstream fout("output_image.ppm");
    fout << "P3" << endl;
    fout << imageWidth << " " << imageHeight << endl;
    fout << "256" << endl;

    for(int y = 0; y < imageHeight; y++)
    {
        for(int x = 0; x < imageWidth; x++)
        {
            double cr = mapToReal(x, imageWidth, minR, maxR);
            double ci = mapToImaginary(y, imageHeight, minI, maxI);

            int n = 0;
        }
    }


}
