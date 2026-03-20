
#include <stdio.h>
#include <string.h>
#include <common/ilogger.hpp>
#include <functional>


int app_rtdetr();
int app_ostrack(std::string camid, std::string path);
int app_bat(std::string engine_path, std::string dataset);

int main(int argc, char** argv){
    
    std::string engine_path = (argc > 1) ? argv[1] : "";
    std::string dataset_name = (argc > 2) ? argv[2] : "RGBT234";
    

    if(engine_path.find("BAT") != std::string::npos) {
        app_bat(engine_path, dataset_name);
    }
    else {
        printf("Unknow method: %s\n", engine_path);
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
