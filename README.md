# convert_annotation_voc_yolo
Just conversion from PASCAL VOC to YOLO annotation using C++

## How to use:
- open `main.cpp` file folder
- build with `g++ main.cpp vocparser.cpp -I"./" -o parser -libxerces-c`
- run `./parser`

## Note:
- set your PASCAL VOC dataset folder in _vocparser.h_ (`const char VOCDevkitLocation[] = "/path/to/pascalvoc"`)
