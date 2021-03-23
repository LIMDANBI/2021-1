#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv){ // chmod a+rX [path] 구현
    
    struct stat file_info;
    mode_t newmode;

    for(int i=1; i<argc; i++){
        if(access(argv[i], F_OK) < 0){ 
            perror("access error");
            return -1;
        }
        if(stat(argv[i], &file_info) < 0) { // 파일에 대한 정보를 받아 옴 
            perror("stat error");
            return -1;
        }
        
        newmode = file_info.st_mode|S_IRUSR|S_IRGRP|S_IROTH; // read 권한 활성화 
        
        if(S_ISDIR(file_info.st_mode)) //directory인 경우 
            newmode = newmode|S_IXUSR|S_IXGRP|S_IXOTH; // 실행 권한 활성화 
        else{ // 아닌 경우
            if(file_info.st_mode&S_IXUSR || file_info.st_mode&S_IXGRP || file_info.st_mode&S_IXOTH) // 하나라도 실행권한을 가지고 있는 경우
                newmode = newmode|S_IXUSR|S_IXGRP|S_IXOTH; // 실행 권한 활성화 
        }
        if(chmod(argv[i], newmode) < 0) {
            perror("chmod error");
            return -1;
        }
    }
}