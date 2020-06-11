#include <iostream>
#include <fstream>
#include <sstream>
#include "vocparser.h"

using namespace std;

void printBBox(vector<vocbox>& box)
{
    for(int i = 0; i < box.size(); i++)
    {
        cout << box[i].label << " ";
        cout << box[i].xmin << " ";
        cout << box[i].ymin << " ";
        cout << box[i].xmax << " ";
        cout << box[i].ymax << "\n\n";
    }
}
void printBBox(vector<yolobox>& box)
{
    for(int i = 0; i < box.size(); i++)
    {
        cout << box[i].label << " ";
        cout << box[i].x << " ";
        cout << box[i].y << " ";
        cout << box[i].w << " ";
        cout << box[i].h << "\n\n";
    }
}

vector<yolobox> convertVOCBoxtoYOLOBox(vector<vocbox>& vBBox, float w, float h)
{
    vector<yolobox> yBBox;
    yBBox.resize(vBBox.size());

    float dw = 1./w;
    float dh = 1./h;
    for (int i = 0; i < vBBox.size(); i++)
    {
        yBBox[i].x = ((vBBox[i].xmin + vBBox[i].xmax)/2.0 - 1)*dw;
        yBBox[i].y = ((vBBox[i].ymin + vBBox[i].ymax)/2.0 - 1)*dh;
        yBBox[i].w = (vBBox[i].xmax - vBBox[i].xmin)*dw;
        yBBox[i].h = (vBBox[i].ymax - vBBox[i].ymin)*dh;
        yBBox[i].label = vBBox[i].label;
    }
    return yBBox;
}

void convertAnnotation(const char* year, const char* image_id)
{
    GetConfig VOCParserConfig;
    ostringstream xmlFileName;
    ofstream outFile;
    ostringstream outFileName;

    outFileName << VOCDevkitLocation << year << "/labels/" << image_id << ".txt";
    outFile.open(outFileName.str());

    xmlFileName << VOCDevkitLocation << year << "/Annotations/" << image_id << ".xml";
    string xmlFile = xmlFileName.str();
    VOCParserConfig.readConfigFile(xmlFile);

    vector<vocbox> vBBox;
    vector<yolobox> yBBox;
    float w, h;

    VOCParserConfig.getXMLValues(w, h, vBBox);
    if(!vBBox.empty())
    {
        // printBBox(vBBox);
        yBBox = convertVOCBoxtoYOLOBox(vBBox, w, h);
        for(int i = 0; i < yBBox.size(); i++)
            outFile << yBBox[i].label << " " << yBBox[i].x << " " << yBBox[i].y << " " << yBBox[i].w << " " << yBBox[i].h << "\n";
    }
    outFile.close();
}

string getDatasetFilename(const char* year, const char* ds)
{
    ostringstream dsFilename;
    dsFilename << VOCDevkitLocation << year << "/ImageSets/Main/" << ds << ".txt";
    cout << dsFilename.str() << "\n";
    return dsFilename.str();
}

int main(int argc, char** argv)
{
    ifstream fileReader;
    ofstream fileImageSet;
    ostringstream imageset;

    char image_ids[20];

    for(int index = 0; index < 10; index+=2)
    {
        imageset << datasets[index] << "_" << datasets[index+1] << ".txt";
        fileReader.open(getDatasetFilename(datasets[index], datasets[index+1]));
        fileImageSet.open(imageset.str());
        
        while(fileReader >> image_ids)
        {
            // cout << image_ids << "\n";
            fileImageSet << VOCDevkitLocation << datasets[index] << "/JPEGImages/" << image_ids << ".jpg\n";
            convertAnnotation(datasets[index], image_ids);
        }
        imageset.str("");
        fileReader.close();
        fileImageSet.close();
    }
    // string filename = "/mnt/ANOTHER_LIFE/PASCAL_VOC/VOCdevkit/VOC2007/Annotations/000001.xml";

    return 0;
}