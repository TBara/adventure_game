#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

typedef int bool;
enum { false, true };

const int NAMES_SIZE = 10;
const int ROOM_COUNT = 7;
char* r_names[] = {
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
char * r_types[] = {
    "START_ROOM",
    "END_ROOM",
    "MID_ROOM"
};

typedef struct Room {
    char* r_name;
    char* r_type;
    int possible_conn_cnt;
    int conn_cnt;
    struct Room *connects[6];
}Room;

// GIve a room a unique name;
void setRoomNames(struct Room* rooms[], int size, int i);
// Sets room type
void setRoomTypes(struct Room* rooms[], int size);
// Sets the number of possible connctions this room can have to other rooms, 3 to 6;
void setConnNumber(struct Room* rooms[], int size);
// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct Room* x, struct Room* y);
// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull(struct Room* rooms[]); 
// Returns a random Room, does NOT validate if connection can be added
struct Room* GetRandomRoom(struct Room* rooms[]);
// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(struct Room* x); 
// Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(struct Room* x, struct Room* y);
// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct Room* x, struct Room* y);
// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection(struct Room* rooms[]);
// Frees dynamically allocated memory
void deallocateMem(struct Room* rooms[]);
// Creates its own directory and prints all 
// rooms to their own files and saves in created directory
void printRooms(struct Room* rooms[]);

int main()
{
  srand(time(NULL)); /* Seed random */
  int random = rand() % NAMES_SIZE;

  char *room_name_arr[ROOM_COUNT];
  memset(room_name_arr, '\0', sizeof(char)*ROOM_COUNT);
  
  struct Room* rooms[ROOM_COUNT];
  setRoomNames(rooms,ROOM_COUNT, random);
  setRoomTypes(rooms, ROOM_COUNT);
  setConnNumber(rooms, ROOM_COUNT);

  while (!IsGraphFull(rooms))
  {
    AddRandomConnection(rooms);
  }
  printRooms(rooms);
  deallocateMem(rooms);
  return 0;
}

void printRooms(struct Room* rooms[]){
  
  //Get pid
  int pid = getpid();
  char mydir[40];  
  sprintf(mydir, "./barabast.rooms.%d", pid);

  struct stat st = {0};
  char *dir = mydir;

  // Check if folder exists and create it
  if (stat(dir, &st) == -1) {
      mkdir(dir, 0700);
  }

  int i;
  for (i = 0; i < ROOM_COUNT; i++)
  {
    int f_descr;
    char f_name[32];
    char temp_str[32];
    sprintf(f_name, "%s/%s_room", mydir, rooms[i]->r_name);
    ssize_t nread, nwritten;
    char readBuffer[32];
    
    f_descr = open(f_name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if( f_descr < 0){
      printf("open() failed on \"%s\"\n", f_name);
      perror("In printRooms");
      exit(1);
    }

    //printf("ROOM NAME: %s\n", rooms[i]->r_name);
    sprintf(temp_str,  "ROOM NAME: %s\n", rooms[i]->r_name);   
    nwritten = write(f_descr, temp_str, strlen(temp_str)*sizeof(char));
    memset(readBuffer, '\0', sizeof(readBuffer)); // Clear out the array before using it 
    
    int j;
    for(j = 0; j < rooms[i]->conn_cnt; j++){
      int conn = (j+1);
      //printf("CONNECTION %d: %s\n", conn, rooms[i]->connects[j]->r_name );
      sprintf(temp_str,  "CONNECTION %d: %s\n", conn, rooms[i]->connects[j]->r_name);
      nwritten = write(f_descr, temp_str, strlen(temp_str));
      memset(readBuffer, '\0', sizeof(readBuffer)); // Clear out the array before using it
    }
    //printf("ROOM TYPE: %s\n\n", rooms[i]->r_type);
    sprintf(temp_str,  "ROOM TYPE: %s\n", rooms[i]->r_type);
    nwritten = write(f_descr, temp_str, strlen(temp_str));
    memset(readBuffer, '\0', sizeof(readBuffer)); // Clear out the array before using it
    lseek(f_descr, 0, SEEK_SET); // Reset the file pointer to the beginning of the file
  }
}

void deallocateMem(struct Room* rooms[]){
  int i;
  for (i = 0; i < ROOM_COUNT; i++)
  {
    free(rooms[i]);
  }
}

void AddRandomConnection(struct Room* rooms[])  {
  struct Room* A; 
  struct Room* B;
  while(true)
  {
    A = GetRandomRoom(rooms);
    if (CanAddConnectionFrom(A) == true)
      break;
  }
  do
  {
    B = GetRandomRoom(rooms);
  }
  while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

  ConnectRoom(A, B); 
  ConnectRoom(B, A); 
}

bool ConnectionAlreadyExists(struct Room* x, struct Room* y){
  int i = 0;
  while (i < x->conn_cnt)
  {
    if (strcmp(y->r_name, x->connects[i]->r_name ) == 0)
    {
      return true;
    } else {
      i++;
    }
  }
  return false;
}

bool CanAddConnectionFrom(struct Room* x){
  if (x->conn_cnt < 6)
  {
    return true;
  } else
  {
    return false;
  }
}

bool IsGraphFull(struct Room* rooms[]){
  int i;
  for (i = 0; i < ROOM_COUNT; i++)
  {
    if (rooms[i]->conn_cnt < 3)
    {
      return false;
    } else {
      i++;
    }
  }
  return true;
}

bool IsSameRoom(struct Room* x, struct Room* y){
  if (x->r_name == y->r_name)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void ConnectRoom(struct Room* x, struct Room* y){

  x->connects[x->conn_cnt] = y;
  x->conn_cnt++;
  
}

struct Room* GetRandomRoom(struct Room* rooms[]){
  
  int i = rand() % ROOM_COUNT;
  return rooms[i];
}

void setConnNumber(struct Room* rooms[], int size){
  int i;
  for (i = 0; i < size; i++)
  {
    int num = (rand() % (6 - 3 + 1)) + 3; 
    rooms[i]->possible_conn_cnt = num;
    rooms[i]->conn_cnt = 0;
  }
}

void setRoomNames(struct Room* rooms[], int size, int random){
  int nameCnt = 0;
  while (nameCnt < size)
  {
    rooms[nameCnt] = malloc(sizeof (struct Room));
    if (random == NAMES_SIZE)
    {
      random = 0;
      rooms[nameCnt]->r_name = r_names[random];
    } else
    {
      rooms[nameCnt]->r_name = r_names[random];
    }
    random++;
    nameCnt++;
  }
}

void setRoomTypes(struct Room* rooms[], int size){
  int i;
  for (i = 0; i < size; i++)
  {
    if (i == 0)
    {
      rooms[i]->r_type = r_types[0];
    }
    else if (i == size - 1)
    {
      rooms[i]->r_type = r_types[1];
    }
    else
    {
      rooms[i]->r_type = r_types[2];
    }
    
  }
}