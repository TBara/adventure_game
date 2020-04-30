#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <pthread.h>
#include <assert.h>
#include <pthread.h>

const char *F_CORE = "barabast.rooms.";
const int ROOM_COUNT = 7;

typedef struct Room {
    char* r_name;
    char* r_type;
    int conn_cnt;
    char *connects[6];
}Room;

char* ROOM_NAMES[] = {
    "Opal",
    "Bismuth",
    "Quartz",
    "Fluorite",
    "Azurite",
    "Pirite",
    "Crocoite",
    "Obsidian",
    "Emerald",
    "Agate"
};

// Compares all directories with OSU user name prefix
// Gets the newest directory.
void getLatestDir(char* latestDir);
void setRooms(struct Room* rooms[], char *latestDir);
void CompleteRoom(struct Room* room, FILE* fp);
void Deallocate(struct Room* rooms[]);
void Game(struct Room *rooms[]);
void GameSummary(const int path[], int size);
void PrintLocation(struct Room* room);
void GetInput(char* input);
void* GetTime();
int GetRoomIdx(char* name);
int CountLinesInFile(FILE * fp);
struct Room* FindRoomByType(struct Room* rooms[], char* r_type);
struct Room* FindRoomByName(struct Room* rooms[], char* r_name);

int main(){
    char latestDir[40];
    memset(latestDir, '\0', sizeof(char)*40);
    getLatestDir(latestDir);
    char *dirPtr = latestDir;

    struct Room* rooms[ROOM_COUNT];
    setRooms(rooms, dirPtr);
    Game(rooms);

    Deallocate(rooms);
    return 0;
}

void Game(struct Room *rooms[]){
    
    struct Room* curr_rm;
    struct Room* next_rm;
    int path[4000];
    memset(path, '\0', 4000);
    char userInput[256];
    memset(userInput, '\0', 256);

    char *start_room = "START_ROOM";
    curr_rm = FindRoomByType(rooms, start_room);
    assert(curr_rm != 0);
    
    int step_cnt = 0;
    int gameOn = 1;
    do
    {
        PrintLocation(curr_rm);
        GetInput(userInput);
        next_rm = FindRoomByName(rooms, userInput);
        if ((next_rm) && (strcmp(userInput, next_rm->r_name) == 0) && (strcmp(next_rm->r_type, "END_ROOM") != 0))
        {
            path[step_cnt] = GetRoomIdx(next_rm->r_name);
            curr_rm = next_rm;
            next_rm = 0;
            step_cnt++;
        }
        else if ((next_rm) && (strcmp(userInput, next_rm->r_name) == 0) && (strcmp(next_rm->r_type, "END_ROOM") == 0))
        {
            path[step_cnt] = GetRoomIdx(next_rm->r_name);
            step_cnt++;
            printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n"
                   "YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",
                   step_cnt);
            int i;
            for (i = 0; i < (step_cnt); i++)
            {
                printf("%s\n", ROOM_NAMES[path[i]]);
            }
            gameOn = 0;
        }
        else if (strcmp(userInput, "time") == 0)
        {
            pthread_t time_thread;
            pthread_mutex_t mutex;
            
            // Init mutex & lock
            pthread_mutex_init (&mutex, NULL);
            pthread_mutex_lock(&mutex);

            //write the time file 
            int rslt_code = pthread_create(&time_thread, NULL, GetTime, NULL);
            pthread_join(time_thread, NULL);

            // Unlock and destroy mutex
            pthread_mutex_unlock(&mutex);
            pthread_mutex_destroy(&mutex);
        } else
        {
            printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");

        }      
    } while (gameOn == 1);   
}

void* GetTime()
{
	char buffer[256];
    char* time_path = "currentTime.txt";
	FILE* time_file;
	time_file = fopen(time_path, "w+");
	
    tzset();
	struct tm *time_struct;
	time_t curr_time = time(0);

	// Save current time, format, put in buffer
	time_struct = gmtime(&curr_time);
	//strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_struct);
    strftime(buffer, sizeof(buffer), "%I:%M%p, %A, %B %d, %Y", time_struct);
	
	// Write to file
	fputs(buffer, time_file);
	fclose(time_file);
    return NULL;
}

int GetRoomIdx(char* name){
    int i;
    for (i = 0; i < 10; i++)
    {
        if (strcmp(ROOM_NAMES[i], name) == 0)
        {
            return i;
        }
    }
}

void GetInput(char* userInput){
    printf("WHERE TO? >");
    scanf("%s", userInput);
    printf("\n");
}

void PrintLocation(struct Room* room){
    printf("CURRENT LOCATION: %s", room->r_name);
    printf("\nPOSSIBLE CONNECTIONS:");
    int i;
    for(i = 0; i < room->conn_cnt; i++){
        if (i == (room->conn_cnt - 1))
        {
            printf(" %s.\n", room->connects[i]);
        } else
        {
            printf(" %s,", room->connects[i]);
        }
    }
}

struct Room* FindRoomByName(struct Room* rooms[], char* r_name){
    int i;
    for (i = 0; i < ROOM_COUNT; i++)
    {
        //printf("%s ", rooms[i]->r_type);
        if( strcmp(rooms[i]->r_name, r_name) == 0){
            return rooms[i];
        }
    }
    return NULL;
}

struct Room* FindRoomByType(struct Room* rooms[], char* r_type){
    int i;
    for (i = 0; i < ROOM_COUNT; i++)
    {
        if(strcmp(rooms[i]->r_type, r_type) != 1){
            return rooms[i];
        }
    }
    return NULL;
}

void setRooms(struct Room* rooms[], char *latestDir){
    //open the most recent directory
    DIR* currDir;
	if((currDir=opendir(latestDir)) != NULL)
	{
		// Returns a pointer to a dirent structure representing 
        // the next directory entry in the directory stream pointed to by dirp.
        int f_cnt=0;
        struct dirent *file;
		while((file = readdir(currDir)) != NULL)
		{
			if(strlen(file->d_name)>2)
			{
                // Get file name
                char* token;
                char f_name[32];
                sprintf(f_name, "%s", file->d_name); 
                token = strtok(f_name, "_");

                // Get the path to file
                char f_path[150];
                memset(f_path, '\0', sizeof(char)*150);
                snprintf(f_path, sizeof(char) * 150, "./%s/%s_room", latestDir, token);
                FILE *fp;
                
                fp = fopen(f_path, "r");
                if (fp == NULL)
                {
                    printf("File failed to open.\n");
                }
                else
                {
                    // Init and give each room a name
                    rooms[f_cnt] = malloc(sizeof (struct Room));
                    memset(rooms[f_cnt], '\0', sizeof (struct Room));
                    rooms[f_cnt]->conn_cnt = 0;
                    CompleteRoom(rooms[f_cnt], fp);
                }
                rewind(fp);
                fclose(fp);
                f_cnt++;
            }
		}
        closedir(currDir);
	}
    
}

void CompleteRoom(struct Room* room, FILE* fp){
    
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    const int lineCnt = CountLinesInFile(fp);
    
    int readLines = 0;
    int conn_cnt = 0;
    while ((read = getline(&line, &len, fp)) != -1) {

        
        const char* from = line;
        char *to = (char*) malloc(30);
        if (readLines == 0)
        {
            room->r_name = malloc(sizeof(char)*strlen(to));
            memset(room->r_name, '\0', sizeof(char)*strlen(to));
            strncpy(to, from+11, 11);
            to[strlen(to)-1] = 0;
            sprintf(room->r_name, "%s", to);
        
        } else if(readLines == (lineCnt - 1) ) {

            room->r_type = malloc(sizeof(char)*strlen(to));
            memset(room->r_type, '\0', sizeof(char)*strlen(to));
            strncpy(to, from+11, 11);
            to[strlen(to)-1] = 0;
            sprintf(room->r_type, "%s", to);

        } else if (!(readLines == lineCnt ) && !(readLines == 0) )
        {
            room->connects[conn_cnt] = malloc(sizeof(char)*strlen(to));
            memset(room->connects[conn_cnt], '\0', sizeof(char)*strlen(to));
            strncpy(to, from+14, 11);
            to[strlen(to)-1] = 0;
            sprintf(room->connects[conn_cnt], "%s", to);
            room->conn_cnt++;
            conn_cnt++;
        }
        free(to);
        readLines++;
    }
    
    if (line)
        free(line);
}

int CountLinesInFile(FILE * fp){
    int character = 0;
    int lines = 0;
    while (!feof(fp))
    {
        character = fgetc(fp);
        if (character == '\n')
        {
            lines++;
        }
    }
    rewind(fp);
    return lines;
}

void getLatestDir(char* latestDir)
{	
    struct stat *buffer = malloc(sizeof(struct stat));
	struct dirent *dir_pt = malloc(sizeof(struct dirent));
	DIR *dir;
	time_t last_mod;
    time_t is_new = 0;

    size_t size = 100;
    char *curr_dir = malloc(size);
    getcwd(curr_dir, size);

    // Open then iterate current working directory 
    dir = opendir(curr_dir);
    while (dir_pt = readdir(dir))
    {
        if (strstr(dir_pt->d_name, F_CORE) != 0)
        {
            stat(dir_pt->d_name, buffer);
            last_mod = buffer->st_mtime;

            // Determine if this is the latest dir
            if (last_mod > is_new)
            {
                is_new = last_mod;
                strcpy(latestDir, dir_pt->d_name);
            }
        }
    }
    closedir(dir);
    // Free dynamically allocated memory
    free(curr_dir);
	free(dir_pt);
	free(buffer);
}

void Deallocate(struct Room* rooms[]){
    int i;
    for(i = 0; i < ROOM_COUNT; i++){
        free(rooms[i]);
        rooms[i] = 0;
    }
}