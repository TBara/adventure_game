#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>

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
const enum R_TYPE{
    START_ROOM,
    END_ROOM,
    MID_ROOM
} R_TYPE;

typedef struct Room {
    char* r_name;
    enum R_TYPE r_type;
    int possible_conn_cnt;
    int conn_cnt;
    struct Room *connects[6];
}Room;

// GIve a room a unique name;
void setRoomNames(struct Room* rooms, int size, int i);
// Sets room type
void setRoomTypes(struct Room* rooms, int size);
// Sets the number of possible connctions this room can have to other rooms, 3 to 6;
void setConnNumber(struct Room* rooms, int size);
// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct Room x, struct Room y);
// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull(struct Room* rooms); 
// Returns a random Room, does NOT validate if connection can be added
struct Room GetRandomRoom(struct Room* rooms, struct Room curr_room);
// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(struct Room x); 

// Returns true if a connection from Room x to Room y already exists, false otherwise
int ConnectionAlreadyExists(struct Room x, struct Room y);
// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct Room x, struct Room y);

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection(struct Room A, struct Room B)  
{
  //   // Maybe a struct, maybe global arrays of ints
  
  // while(true)
  // {
  //   A = GetRandomRoom();

  //   if (CanAddConnectionFrom(A) == true)
  //     break;
  // }

  // do
  // {
  //   B = GetRandomRoom();
  // }
  // while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

  // ConnectRoom(A, B);  // TODO: Add this connection to the real variables, 
  // ConnectRoom(B, A);  //  because this A and B will be destroyed when this function terminates
}



int main()
{
  srand(time(NULL)); /* Seed random */
  int random = rand() % NAMES_SIZE;

  char *room_name_arr[ROOM_COUNT];
  memset(room_name_arr, '\0', sizeof(char)*ROOM_COUNT);
  
  int i = random;
  struct Room rooms[ROOM_COUNT];
  setRoomNames(rooms,ROOM_COUNT, i);
  setRoomTypes(rooms, ROOM_COUNT);
  setConnNumber(rooms, ROOM_COUNT);


  return 0;
}

bool CanAddConnectionFrom(struct Room x){
  if (x.conn_cnt < 6)
  {
    return true;
  } else
  {
    return false;
  }
}

struct Room GetRandomRoom(struct Room* rooms, struct Room curr_room){
    struct Room rand_room;
    int i;
    do
    {
      i = rand() % ROOM_COUNT;
      rand_room = rooms[i];
    } while (rand_room.r_name == curr_room.r_name);
    return rand_room;
}

bool IsGraphFull(struct Room* rooms){
  int i;
  for (i = 0; i < ROOM_COUNT; i++)
  {
    if (rooms[i].conn_cnt < 3)
    {
      return false;
    } else {
      i++;
    }
  }
  return true;
}

bool IsSameRoom(struct Room x, struct Room y){
  if (x.r_name == y.r_name)
  {
    return true;
  }
  else
  {
    return false;
  }
  
}

void setConnNumber(struct Room* rooms, int size){
  int i;
  for (i = 0; i < size; i++)
  {
    int num = (rand() % (6 - 3 + 1)) + 3; 
    rooms[i].possible_conn_cnt = num;
  }
}

void setRoomNames(struct Room* rooms, int size, int i){
  int nameCnt = 0;
  while (nameCnt < size)
  {
    if (i == NAMES_SIZE)
    {
      i = 0;
      rooms[nameCnt].r_name = r_names[i];
    } else
    {
      rooms[nameCnt].r_name = r_names[i];
    }
    i++;
    nameCnt++;
  }
}

void setRoomTypes(struct Room* rooms, int size){
  int i;
  for (i = 0; i < size; i++)
  {
    if (i == 0)
    {
      rooms[i].r_type = START_ROOM;
    }
    else if (i == size - 1)
    {
      rooms[i].r_type = END_ROOM;
    }
    else
    {
      rooms[i].r_type = MID_ROOM;
    }
    
  }
}