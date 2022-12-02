#include <stdio.h>
#include <sys/types.h> //opendir, stat
#include <dirent.h> //opendir
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h> //stat
#include <stdlib.h> //free, malloc


int main(int argc, char** argv) {
    char* path_to_directory;
    int i;
    char cwd[256];
	
    for(i = 0; i < argc; i++){
    	printf("Checking in directory:%s\n", argv[i]);
    if (argc < 2) {
	    if (getcwd(cwd, sizeof(cwd)) == NULL){
     		 perror("getcwd() error");
	    	}
	    		path_to_directory = cwd;
	    		fprintf(stderr, "checking current directory \n");
    	    	} else {
    			path_to_directory = argv[i];
    	    	}
    
    int path_length = strlen(path_to_directory);
    int modified = 0;
    //Modify path so that a dash is at the end.
    if (path_to_directory[path_length - 1] != '/') {
        char* modified_path = (char *) malloc(sizeof(char) * (strlen(path_to_directory) + 2));
        //Copies the null character as well.
        strcpy(modified_path, path_to_directory);
        modified_path[path_length] = '/';
        modified_path[path_length + 1] = '\0';
        path_to_directory = modified_path;
        //Set flag to true so that the dynamically allocated memory is freed later.
        modified = 1;
    }

    //Get the directory stream corresponding to the directory. 
    DIR* in_directory_stream = opendir(path_to_directory);
    if (in_directory_stream == NULL) {
        fprintf(stderr, "Error: the specified directory cannot be found or opened. \n", errno);
        if (modified) free(path_to_directory);
    }
    struct dirent* entry = NULL; 
    printf("Files that are executable by at least one of the permission classes (owner, group, others) are: \n");
    while ((entry = readdir(in_directory_stream)) != NULL) {
        //All directories contain . and .., which corresponds to current and parent directory respectively,
        //in unix systems. Since we are looking for only executable files, we can ignore them. 
        if (!strcmp(".", entry -> d_name)) {
            continue;
        }
        if (!strcmp("..", entry -> d_name)) {
            continue;
        }
        //Get file information. 
        struct stat entry_info;
        /* Create the absolute path of the entry.
         * Without it,
         * stat will look for a file with the entry's name in current working directory 
         * instead of the specified directory. 
         */
        char* entry_absolute_path  = (char *) malloc(sizeof(char) * (strlen(path_to_directory) 
                    + strlen(entry->d_name) + 1));
        strcat(entry_absolute_path, path_to_directory);
        strcat(entry_absolute_path, entry->d_name);       
       if (stat(entry_absolute_path, &entry_info) == -1) {
            fprintf(stderr, "Error in obtaining file information about %s\n", entry->d_name);
       } else {
           // Check if the file is not a directory and 
           // is executable by one of the permission classes (owner, group, others). 
           if (((entry_info.st_mode & S_IFMT) != S_IFDIR) && 
                   ((entry_info.st_mode & S_IXUSR) || (entry_info.st_mode & S_IXGRP) 
                   || (entry_info.st_mode & S_IXOTH))) {
               printf("%s\n", entry->d_name);
           }
       } 
           free(entry_absolute_path);
    }
    //Close directory stream.
    closedir(in_directory_stream);    
    if (modified) free(path_to_directory);
    }
    return 0;


}
