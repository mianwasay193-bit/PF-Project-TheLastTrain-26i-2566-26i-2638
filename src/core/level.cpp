#include "level.h"
#include "util.h"
#include <fstream>
#include <cstdlib>
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

        }
        else
		{
			//Looking for a room name like F5-R1 in this line
			p = 0;
			while (line[p] != '\0' && !(line[p] == 'F' && line[p + 1] >= '1' && line[p + 1] <= '5' && line[p + 2] == '-' && line[p + 3] == 'R' && line[p + 4] == '1'))
			{
				p++;
			}

			if (line[p] != '\0')
			{
				//Floor F1 goes to index 0, F5 goes to index 4
				floor = line[p + 1] - '1';

				//Reading the three connections between the four rooms
				idx = p + 5;
				for (room = 0; room < ROOMS_PER_FLOOR - 1; room++)
				{
					while (line[idx] == ' ')
					{
						idx++;
					}
					if (line[idx] == '\0')
					{
						break;
					}
					symbol = line[idx];

					if (symbol == 'D')
					{
						horizontalConn[floor][room] = CONN_DOOR;
					}
					else if (symbol == 'W')
					{
						horizontalConn[floor][room] = CONN_WINDOW;
					}
					else if (symbol == 'L' && line[idx + 1] == '1')
					{
						horizontalConn[floor][room] = CONN_NONE; //Lift 1 is not a horizontal link
					}
					else if (symbol == 'L')
					{
						horizontalConn[floor][room] = CONN_LOCKED_DOOR;
					}
					else if (symbol == '#')
					{
						horizontalConn[floor][room] = CONN_SOLID_WALL;
					}

					//Jumping to the next room name and skipping it
					while (line[idx] != 'F' && line[idx] != '\0')
					{
						idx++;
					}
					idx = idx + 5;
				}
				floors_found++;
				next_line_is_items = true;
			}
		}
	}
	file.close();
    if (floors_found != NUM_FLOORS)
    {
        return false; // Will return false if not all floors are found  
    }

    //I have used two energy packs but the map only has one.
    found_pack = false;
	for (i = 0; i < entityCounts[0][3]; i++)
	{
		if (roomEntities[0][3][i] == ENT_ENERGY_PACK)
		{
			found_pack = true;
		}
	}
	if (found_pack == true && entityCounts[0][3] < MAX_ENTITIES_PER_ROOM)
	{
		roomEntities[0][3][entityCounts[0][3]] = ENT_ENERGY_PACK;
		entityCounts[0][3]++;
	}

	return true;
}

//Reads initial-state.txt and sets the starting values of the game
bool loadInitialState(const char* filename,
                      int& playerFloor, int& playerRoom,
                      int& energy, int& time, int& score,
                      char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                      bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft)
{
	char line[MAX_LINE_LEN];
	char* value;
	int i, colon;

	//Validation
	if (filename == nullptr)
	{
		return false;
	}

	//Opening the file
	ifstream file(filename);
	if (!file)
	{
		return false;
	}

    //Default values first, in case a line is missing in the file
	playerFloor = -1;   //-1 means the player has not chosen a room yet
	playerRoom = -1;
	energy = INITIAL_ENERGY;
	time = INITIAL_TIME;
	score = INITIAL_SCORE;
	inventoryCount = 0;
	emergencyTripsLeft = 0;
	for (i = 0; i < MAX_INVENTORY_CAPACITY; i++)
	{
		inventory[i] = ENT_NONE;
	}
	for (i = 0; i < NUM_OBJECTIVES; i++)
	{
		objectives[i] = false;
	}

    while (file.getline(line, MAX_LINE_LEN))
	{
		//Finding the colon that splits the name from the value
		colon = 0;
		while (line[colon] != '\0' && line[colon] != ':')
		{
			colon++;
		}

		//Lines without a colon (empty lines) are skipped
		if (line[colon] == ':')
		{
			line[colon] = '\0';        //the name now ends where the colon was
			value = &line[colon + 1];  //the value starts right after the colon
			trimWhitespace(line);
			trimWhitespace(value);
            if (stringEquals(line, "START_ROOM"))
			{
				//A fixed room looks like F1-R2, PLAYER_SELECTS means the player picks later
				if (value[0] == 'F' && value[1] >= '1' && value[1] <= '5' && value[2] == '-' && value[3] == 'R' && value[4] >= '1' && value[4] <= '4')
				{
					playerFloor = value[1] - '1';
					playerRoom = value[4] - '1';
				}
			}
			else if (stringEquals(line, "INITIAL_ENERGY"))
			{
				energy = clampInt(atoi(value), 0, MAX_ENERGY);
			}
			else if (stringEquals(line, "INITIAL_TIME"))
			{
				time = clampInt(atoi(value), 0, MAX_TIME);
			}
			else if (stringEquals(line, "INITIAL_SCORE"))
			{
				score = clampInt(atoi(value), 0, 520);   //520 is the highest possible score
			}
			else if (stringEquals(line, "PASSENGER_RESCUED"))
			{
				objectives[OBJ_PASSENGER_RESCUED] = stringEquals(value, "YES");
			}
			else if (stringEquals(line, "CAT_RESCUED"))
			{
				objectives[OBJ_CAT_SECURED] = stringEquals(value, "YES");
			}
			else if (stringEquals(line, "CABINET_UNLOCKED"))
			{
				objectives[OBJ_CABINET_UNLOCKED] = stringEquals(value, "YES");
			}
			else if (stringEquals(line, "TRAIN_KEY_COLLECTED"))
			{
				objectives[OBJ_TRAIN_KEY] = stringEquals(value, "YES");
			}
			else if (stringEquals(line, "FUSE_INSTALLED"))
			{
				objectives[OBJ_FUSE_INSTALLED] = stringEquals(value, "YES");
			}
			else if (stringEquals(line, "LIFT_PASS_COLLECTED"))
			{
				objectives[OBJ_LIFT_PASS] = stringEquals(value, "YES");
			}
			else if (stringEquals(line, "TRAIN_PREPARED"))
			{
				objectives[OBJ_TRAIN_PREPARED] = stringEquals(value, "YES");
			}
		}
	}
	file.close();
	return true;
}

//Checks that the room chosen by the player exists and has a way out
bool validateStartingRoom(int startFloor, int startRoom,
                          const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1])
{
	char left_link, right_link;
	bool has_exit;

	//Bounds check
	if (startFloor < 0 || startFloor >= NUM_FLOORS)
	{
		return false;
	}
	if (startRoom < 0 || startRoom >= ROOMS_PER_FLOOR)
	{
		return false;
	}

	//Reading the link on the left and on the right of the room
	left_link = CONN_NONE;
	right_link = CONN_NONE;
	if (startRoom > 0)
	{
		left_link = horizontalConn[startFloor][startRoom - 1];
	}
	if (startRoom < ROOMS_PER_FLOOR - 1)
	{
		right_link = horizontalConn[startFloor][startRoom];
	}

	//A door, a window or the locked door means the player can leave the room
	has_exit = false;
	if (left_link == CONN_DOOR || left_link == CONN_WINDOW || left_link == CONN_LOCKED_DOOR)
	{
		has_exit = true;
	}
	if (right_link == CONN_DOOR || right_link == CONN_WINDOW || right_link == CONN_LOCKED_DOOR)
	{
		has_exit = true;
	}

	return has_exit;
}

//Gives the name of an item from its symbol
const char* getEntityName(char entitySymbol)
{
	if (entitySymbol == ENT_MASTER_KEY)
	{
		return "Master Key";
	}
	else if (entitySymbol == ENT_ENERGY_PACK)
	{
		return "Energy Pack";
	}
	else if (entitySymbol == ENT_EMERGENCY_TOOLKIT)
	{
		return "Emergency Toolkit";
	}
	else if (entitySymbol == ENT_CLUE_4)
	{
		return "Clue Digit 4";
	}
	else if (entitySymbol == ENT_FUSE)
	{
		return "Fuse";
	}
	else if (entitySymbol == ENT_CAT)
	{
		return "Cat";
	}
	else if (entitySymbol == ENT_CONTROL_PANEL)
	{
		return "Control Panel";
	}
	else if (entitySymbol == ENT_LIFT_PASS)
	{
		return "Lift Pass";
	}
	else if (entitySymbol == ENT_PASSENGER)
	{
		return "Injured Passenger";
	}
	else if (entitySymbol == ENT_MEDICAL_KIT)
	{
		return "Medical Kit";
	}
	else if (entitySymbol == ENT_EMERGENCY_CARD)
	{
		return "Emergency Access Card";
	}
	else if (entitySymbol == ENT_CLUE_2)
	{
		return "Clue Digit 2";
	}
	else if (entitySymbol == ENT_CLUE_7)
	{
		return "Clue Digit 7";
	}
	else if (entitySymbol == ENT_CABINET_TRAIN_KEY)
	{
		return "Cabinet";
	}
	else if (entitySymbol == ENT_CAT_CARRIER)
	{
		return "Cat Carrier";
	}
	else if (entitySymbol == ENT_OBSERVATION_TOKEN)
	{
		return "Observation Token";
	}
	else
	{
		return "Unknown";
	}
}




        



