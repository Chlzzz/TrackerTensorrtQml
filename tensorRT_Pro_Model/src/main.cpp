
#include <stdio.h>
#include <string.h>
#include <common/ilogger.hpp>
#include <functional>


int app_rtdetr();
int app_ostrack(std::string camid, std::string path);

int main(int argc, char** argv){
    

    const char* method = "ostrack";
    if(argc > 1){
        method = argv[1];
    }

    if(strcmp(method, "ostrack") == 0){
    	app_ostrack(argv[2], argv[3]);
    }else if(strcmp(method, "rtdetr") == 0){
        app_rtdetr();
    }else{
        printf("Unknow method: %s\n", method);
        printf(
            "Help: \n"
            "    ./pro method like [yolo、yolo_cls、yolo_seg、yolo_pose、test_yolo_map]\n"
            "\n"
            "    ./pro yolo\n"
            "    ./pro yolo_cls\n"
            "    ./pro yolo_seg\n"
        );
    } 
    return 0;
}
