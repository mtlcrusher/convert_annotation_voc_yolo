#ifndef __VOCPARSER_H__
#define __VOCPARSER_H__

#include "xercesc/dom/DOM.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/XMLUni.hpp"

#include <string>
#include <stdexcept>
#include <vector>

using namespace xercesc;

const char datasets[][6] = {"2007", "train", "2007", "val", "2012", "train", "2012", "val", "2007", "test"};

const char classes[][12] = {"aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", 
                    "diningtable", "dog", "horse", "motorbike", "person", "pottedplant", "sheep", "sofa", "train", "tvmonitor"};

const char VOCDevkitLocation[] = "/mnt/ANOTHER_LIFE/PASCAL_VOC/VOCdevkit/VOC";

typedef struct yolobox
{
    float x;
    float y;
    float w;
    float h;
    int label;

}yolobox;

typedef struct vocbox
{
    float xmin;
    float ymin;
    float xmax;
    float ymax;
    int label;

}vocbox;

// Error codes
enum
{
    ERROR_ARGS = 1,
    ERROR_XERCES_INIT,
    ERROR_PARSE,
    ERROR_EMPTY_DOCUMENT
};

class GetConfig
{
public:
    GetConfig();
    
    void readConfigFile(std::string& configFile) throw(std::runtime_error);

    std::vector<vocbox> getBox();
    float getWidth();
    float getHeight();
    int getLabel(char* name);
    void getXMLValues(float& w, float& h, std::vector<vocbox>& Box);

    ~GetConfig();
private:
    XercesDOMParser *m_ConfigFileParser;
    
    int nbox;
    int label;
    int difficult;
    float width;
    float height;

    // internal class use only. Hold Xerces data in UTF-16 XMLCh type

    XMLCh* TAG_Annotation;
    
    XMLCh* TAG_Size;
    XMLCh* TAG_Width;
    XMLCh* TAG_Height;

    XMLCh* TAG_Object;
    XMLCh* TAG_Name;
    XMLCh* TAG_Difficult;

    XMLCh* TAG_BBox;
    XMLCh* TAG_Xmin;
    XMLCh* TAG_Ymin;
    XMLCh* TAG_Xmax;
    XMLCh* TAG_Ymax;

    XMLCh* val_dim[2];
    XMLCh* val_box[4];

    std::vector<vocbox> box;

};

#endif