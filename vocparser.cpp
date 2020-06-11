#include "vocparser.h"
#include <iostream>
#include <sstream>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

GetConfig::GetConfig()
{
    try
    {
        XMLPlatformUtils::Initialize(); //init xerces-c infrastructure
    }
    catch(XMLException& e)
    {
        char* message = XMLString::transcode(e.getMessage());
        std::cerr << "XML xerces-c init error: " << message << '\n';
        XMLString::release(&message);
    }
    
    // transcode all element tag name
    TAG_Annotation = XMLString::transcode("annotation");
    
    TAG_Size = XMLString::transcode("size");
    TAG_Width = XMLString::transcode("width");
    TAG_Height = XMLString::transcode("height");

    TAG_Object = XMLString::transcode("object");
    TAG_Name = XMLString::transcode("name");
    TAG_Difficult = XMLString::transcode("difficult");
    
    TAG_BBox = XMLString::transcode("bndbox");
    TAG_Xmin = XMLString::transcode("xmin");
    TAG_Ymin = XMLString::transcode("ymin");
    TAG_Xmax = XMLString::transcode("xmax");
    TAG_Ymax = XMLString::transcode("ymax");

    m_ConfigFileParser = new XercesDOMParser;
}

GetConfig::~GetConfig()
{
    // free memory
    delete m_ConfigFileParser;

    // free all XMLCh* 
    XMLString::release(&TAG_Annotation);
    
    XMLString::release(&TAG_Size);
    XMLString::release(&TAG_Width);
    XMLString::release(&TAG_Height);

    XMLString::release(&TAG_Object);
    XMLString::release(&TAG_Name);
    XMLString::release(&TAG_Difficult);
    
    XMLString::release(&TAG_BBox);
    XMLString::release(&TAG_Xmin);
    XMLString::release(&TAG_Ymin);
    XMLString::release(&TAG_Xmax);
    XMLString::release(&TAG_Ymax);

    // terminate xercesc
    XMLPlatformUtils::Terminate();
}

void GetConfig::readConfigFile(std::string& configFile) 
    throw (std::runtime_error)
{
    struct stat fileStatus;
    errno = 0;

    // check file status
    if(stat(configFile.c_str(), &fileStatus) == -1)
    {
        switch (errno)
        {
            case ENOENT:
                throw (std::runtime_error("Path file_name doesn't exist."));
                break;
            case ENOTDIR:
                throw (std::runtime_error("A component of the path is not a directory."));
                break;
            case ELOOP:
                throw (std::runtime_error("Too many symbolic links encountered while traversing the path."));
                break;
            case EACCES:
                throw (std::runtime_error("Permission denied."));
                break;
            case ENAMETOOLONG:
                throw (std::runtime_error("file_name too long, it can't be read."));
                break;
            
            default:
                break;
        }
    }

    // configure DOM Parser
    m_ConfigFileParser->setValidationScheme(XercesDOMParser::Val_Never); //don't report validation errors
    m_ConfigFileParser->setDoNamespaces(false); //disable enforcement for all the constraints and rules specified by NameSpace
    m_ConfigFileParser->setDoSchema(false); //don't process any "schema"
    m_ConfigFileParser->setLoadExternalDTD(false); //ignore any external DTD

    // start parsing
    m_ConfigFileParser->parse(configFile.c_str());

    // create document pointer as handler
    DOMDocument* xmlDoc = m_ConfigFileParser->getDocument();

    // get top-level element
    DOMElement* elementAnnotation = xmlDoc->getDocumentElement();
    if(!elementAnnotation) throw(std::runtime_error("empty XML."));

    DOMNodeList* childAnnotation = elementAnnotation->getChildNodes();
    XMLSize_t childCount = childAnnotation->getLength();

    DOMElement* currentElement;
    DOMNodeList* childCurrentElement;
    XMLSize_t childCurrentCount;
    box.clear();
    
    // for all nodes in "Annotation"
    for(XMLSize_t i = 0; i < childCount; ++i)
    {
        DOMNode* currentNode = childAnnotation->item(i);
        if(currentNode->getNodeType() && (currentNode->getNodeType() == DOMNode::ELEMENT_NODE)) // the node is not NULL and it is an element
        {
            // found node which is an element, re-cast node as element
            currentElement = dynamic_cast<DOMElement*>(currentNode);
            childCurrentElement = currentElement->getChildNodes();
            childCurrentCount = childCurrentElement->getLength();

            if(XMLString::equals(currentElement->getTagName(), TAG_Size))
            {
                // currentElement tag name equals to "size"
                for(XMLSize_t j = 0; j < childCurrentCount; ++j)
                {
                    currentNode = childCurrentElement->item(j);
                    if(currentNode->getNodeType() && (currentNode->getNodeType() == DOMNode::ELEMENT_NODE)) // the node is not NULL and it is an element
                    {
                        // std::cout << "size child = " << j << "\n";
                        currentElement = dynamic_cast<DOMElement*>(currentNode);
                        if(XMLString::equals(currentElement->getTagName(), TAG_Width))
                            width = atof(XMLString::transcode(currentElement->getTextContent()));
                        else if(XMLString::equals(currentElement->getTagName(), TAG_Height))
                            height = atof(XMLString::transcode(currentElement->getTextContent()));
                    }
                }
            }
            else if(XMLString::equals(currentElement->getTagName(), TAG_Object))
            {
                // currentElement tag name equals to "size"
                vocbox bbox;
                for(XMLSize_t j = 0; j < childCurrentCount; ++j)
                {
                    // std::cout << "Inside child Object loop\n";
                    currentNode = childCurrentElement->item(j);
                    if(currentNode->getNodeType() && (currentNode->getNodeType() == DOMNode::ELEMENT_NODE)) // the node is not NULL and it is an element
                    {
                        currentElement = dynamic_cast<DOMElement*>(currentNode);
                        // std::cout << "object child = " << j << " " << XMLString::transcode(currentElement->getTagName()) << "\n";

                        if(XMLString::equals(currentElement->getTagName(), TAG_Name))
                            bbox.label = getLabel(XMLString::transcode(currentElement->getTextContent()));
                            // std::cout << XMLString::transcode(currentElement->getTextContent()) << "\n";
                        else if(XMLString::equals(currentElement->getTagName(), TAG_Difficult))
                            difficult = atoi(XMLString::transcode(currentElement->getTextContent()));
                        //     std::cout << *XMLString::transcode(currentElement->getTextContent()) - 48;
                        else if(XMLString::equals(currentElement->getTagName(), TAG_BBox) && difficult == 0)
                        {
                            childCurrentElement = currentElement->getChildNodes();
                            childCurrentCount = childCurrentElement->getLength();
                            for(XMLSize_t k = 0; k < childCurrentCount; ++k)
                            {
                                currentNode = childCurrentElement->item(k);
                                if(currentNode->getNodeType() && (currentNode->getNodeType() == DOMNode::ELEMENT_NODE)) // the node is not NULL and it is an element
                                {
                                    currentElement = dynamic_cast<DOMElement*>(currentNode);
                                    if(XMLString::equals(currentElement->getTagName(), TAG_Xmin))
                                        bbox.xmin = atof(XMLString::transcode(currentElement->getTextContent()));
                                    else if(XMLString::equals(currentElement->getTagName(), TAG_Ymin))
                                        bbox.ymin = atof(XMLString::transcode(currentElement->getTextContent()));
                                    if(XMLString::equals(currentElement->getTagName(), TAG_Xmax))
                                        bbox.xmax = atof(XMLString::transcode(currentElement->getTextContent()));
                                    else if(XMLString::equals(currentElement->getTagName(), TAG_Ymax))
                                        bbox.ymax = atof(XMLString::transcode(currentElement->getTextContent()));
                                }
                            }
                            box.push_back(bbox);
                        }
                    }
                }
            }
        }

    }

}

float GetConfig::getWidth()
{
    return width;
}

float GetConfig::getHeight()
{
    return height;
}

void GetConfig::getXMLValues(float& w, float& h, std::vector<vocbox>& Box)
{
    w = width;
    h = height;
    Box = box;
}

int GetConfig::getLabel(char* name)
{
    int Nclasses = 20;
    for(int i = 0; i < Nclasses; i++)
    {
        if(strcmp(classes[i], name) == 0)
            return i;
    }
}