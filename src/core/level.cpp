#include "level.h"
#include "util.h"
#include <fstream>
using namespace std;

// Modular implementation using primitive arrays for map parsing and verification
bool loadBuildingMap(const char* filename,
                     char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                     char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                     int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    char line[MAX_LINE_LEN];
    char symbol,ch;
    int floor, room, slot, i, p, idx, col, len;
    int floors_found, first_col, room_width;
    bool next_line_is_items, found_pack;

    //Validation of filename
    if (filename == nullptr)
    {
        return false; // Will return false if filename is null
    }
    //opening the file
    ifstream file(filename);
    if (!file.is_open())
    {
        return false; // Will return false if file cannot be opened
    }   

    //First clearing all the arrays to make sure there is no garbage value there.
    for (floor = 0; floor<NUM_FLOORS; floor++)
    {
        for (room = 0; room<ROOMS_PER_FLOOR - 1; room++)
        {
            horizontalConn[floor][room] = CONN_NONE; //This replaces anything that is previously stored with nothing.
        }
        for (room = 0; room<ROOMS_PER_FLOOR; room++)
        {
            entityCounts[floor][room] = 0; //This replaces anything that is previously stored with 0.
            for (slot = 0; slot<MAX_ENTITIES_PER_ROOM; slot++)
            {
                roomEntities[floor][room][slot] = ENT_NONE; //This replaces anything that is previously stored with nothing.
            }
        }
    }
    floors_found = 0;
    floor = -1;
    next_line_is_items = false;
    first_col = 9; //the first column inside room 1,calculated by counting characters in the building.txt file
    room_width = 12; //the width of each room, calculated by counting characters in the building.txt file
    
    //Reading the file line by line now and storing the entities.
    //The loop runs until all 5 floors are done, so the key guide given below the map is not read.
    while (!(floors_found == NUM_FLOORS && next_line_is_items == false) && file.getline(line,MAX_LINE_LEN))
    {
        len = 0;
        while (line[len] != '\0')
        {
            len++;
        }
        
        if (next_line_is_items == true)
        {
            //this line holds the items in the line before.
            for(room = 0;room<ROOMS_PER_FLOOR; room++)
            { 
                for (col = first_col + room_width * room; col < first_col + room_width * room + 11 && col<len;col++)
                { 
                    ch = line[col];
                    if (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t' && ch != '\0')
                    {
                        if (entityCounts[floor][room] < MAX_ENTITIES_PER_ROOM)
                        {
                            roomEntities[floor][room][entityCounts[floor][room]] = ch; //storing the entity in the roomEntities array
                            entityCounts[floor][room]++; //incrementing the count of entities in that room
                        }
                    }
                }
            }
            next_line_is_items = false; //resetting the flag for the next iteration

        }else
        {
            
        }

    }

