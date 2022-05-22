//
// Created by gogotime on 2022/5/4.
//

#include <sys/stat.h>

#include "core.h"
#include "vm.h"
#include "../include/utils.h"
#include "../object/obj_map.h"
#include "../object/header_obj.h"
#include "../object/class.h"

char* rootDir = NULL;

VMResult executeModule(VM* vm, Value moduleName, const char* moduleCode) {
    return VM_RESULT_ERROR;
}

void buildCore(VM* vm) {
    ObjModule* coreModule = newObjModule(vm, nil);
    mapSet(vm, vm->allModules, CORE_MODULE, OBJ_TO_VALUE(coreModule));

}

char* readFile(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == nil) {
        IO_ERROR("couldn't open file \"%s\"", path);
    }

    struct stat fileStat;
    stat(path, &fileStat);
    size_t fileSize = fileStat.st_size;
    char* fileContent = (char*) malloc(fileSize + 1);
    if (fileContent == nil) {
        MEM_ERROR("failed to malloc memory for file \"%s\"", path);
    }
    size_t numRead = fread(fileContent, sizeof(char), fileSize, file);
    if (numRead < fileSize) {
        IO_ERROR("read file \"%s\" failed", path);
    }
    fileContent[fileSize] = '\0';
    fclose(file);
    return fileContent;
}
