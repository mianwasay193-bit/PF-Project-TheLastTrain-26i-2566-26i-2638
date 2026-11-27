//Name: Abdul Wasay
// Roll No: i262566
//Project: The Last Train
#include "physics.h"
#include "util.h"
//adds or removes Energy and Time, and keeps bothe betwen 0 and their maximum
void applyResourceDelta(int& energy, int& time, int deltaEnergy, int deltaTime)
{
    energy = clampInt(energy + deltaEnergy, 0, MAX_ENERGY);
    time = clampInt(time + deltaTime, 0, MAX_TIME);
}
//adds points to the score, the score never goes below 0
void applyScoreDelta(int& score, int deltaScore)
{
    score = score +deltaScore;  
    if (score <0)
    {
        score = 0;
    } 
}
//Checking if the player is carrying an item
bool hasItem(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, char itemSymbol)
{
	int i;

	for (i = 0; i < inventoryCount && i < MAX_INVENTORY_CAPACITY; i++)
	{
		if (inventory[i] == itemSymbol)
		{
			return true;
		}
	}
	return false;
}

//This function puts an item in the next free slot of the inventory
bool addItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol)
{
	//The inventory must have space and the symbol must be a real item
	if (inventoryCount >= MAX_INVENTORY_CAPACITY || itemSymbol == ENT_NONE)
	{
		return false;
	}
	inventory[inventoryCount] = itemSymbol;
	inventoryCount++;
	return true;
}

//Takes an item out of the inventory and closes the empty gap produced by it, if the item is present
bool removeItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol)
{
	int i, position;

	//Searching for the item
	position = -1;
	for (i = 0; i < inventoryCount && i < MAX_INVENTORY_CAPACITY; i++)
	{
		if (inventory[i] == itemSymbol && position == -1)
		{
			position = i;
		}
	}
	if (position == -1)
	{
		return false;
	}

	//Moving every item after it one place to the left
	for (i = position; i < inventoryCount - 1; i++)
	{
		inventory[i] = inventory[i + 1];
	}
	inventoryCount--;
	inventory[inventoryCount] = ENT_NONE;
	return true;
}
//Value used for stairs and lifts (doors and windows already have specific values in state.h)
//'S' = stairs, '1' = Lift 1, '2' = Lift 2

//Checks if two rooms touch each other and tells what kind of link joins them
bool isDirectlyConnected(int fromF, int fromR, int toF, int toR,
                         const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                         char& outConnType)
{
	char link;
	int low_floor, low_room;

	outConnType = CONN_NONE;

	//Bounds check
	if (fromF < 0 || fromF >= NUM_FLOORS || toF < 0 || toF >= NUM_FLOORS)
	{
		return false;
	}
	if (fromR < 0 || fromR >= ROOMS_PER_FLOOR || toR < 0 || toR >= ROOMS_PER_FLOOR)
	{
		return false;
	}

	//A room is not connected to itself
	if (fromF == toF && fromR == toR)
	{
		return false;
	}

	//Case 1: both rooms are on the same floor
	if (fromF == toF)
	{
		if (fromR - toR == 1 || toR - fromR == 1)
		{
			//The link number is the number of the room on the left
			if (fromR < toR)
			{
				low_room = fromR;
			}
			else
			{
				low_room = toR;
			}
			link = horizontalConn[fromF][low_room];
			if (link == CONN_DOOR || link == CONN_WINDOW || link == CONN_LOCKED_DOOR)
			{
				outConnType = link;
				return true;
			}
		}
		return false;
	}

	//Case 2: the rooms are on different floors
	if (fromF < toF)
	{
		low_floor = fromF;
	}
	else
	{
		low_floor = toF;
	}

	//Stairs: F1-R1 to F2-R1, F2-R1 to F3-R1, F4-R1 to F5-R1
	if (fromR == 0 && toR == 0 && (fromF - toF == 1 || toF - fromF == 1))
	{
		if (low_floor == 0 || low_floor == 1 || low_floor == 3)
		{
			outConnType = 'S';
			return true;
		}
	}

	//Lift 1: between floor 1 and floor 2, rooms R2 and R3
	if (low_floor == 0 && (fromF - toF == 1 || toF - fromF == 1))
	{
		if ((fromR == 1 || fromR == 2) && (toR == 1 || toR == 2))
		{
			outConnType = '1';
			return true;
		}
	}

	//Lift 2: R4 of floor 2, floor 4 and floor 5 (floor 3 is skipped)
	if (fromR == 3 && toR == 3)
	{
		if ((fromF == 1 || fromF == 3 || fromF == 4) && (toF == 1 || toF == 3 || toF == 4))
		{
			outConnType = '2';
			return true;
		}
	}

	return false;
}
//Checks if the player is allowed to use a link (keys and lift rules)
bool canTraverse(int targetFloor, int targetRoom, char connType,
                 const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                 int emergencyTripsLeft, bool lift2Operational)
{
	//Validation
	if (targetFloor < 0 || targetFloor >= NUM_FLOORS || targetRoom < 0 || targetRoom >= ROOMS_PER_FLOOR)
	{
		return false;
	}

	if (connType == CONN_DOOR || connType == CONN_WINDOW || connType == 'S' || connType == '1')
	{
		return true;
	}
	else if (connType == CONN_LOCKED_DOOR)
	{
		//The locked door needs the Master Key
		return hasItem(inventory, inventoryCount, ENT_MASTER_KEY);
	}
	else if (connType == '2')
	{
		//Normal use: the fuse is installed and the player has the Lift Pass
		if (lift2Operational == true && hasItem(inventory, inventoryCount, ENT_LIFT_PASS))
		{
			return true;
		}
		//Before the repair: the Emergency Access Card gives a few trips
		if (emergencyTripsLeft > 0)
		{
			return true;
		}
		return false;
	}
	else
	{
		//Wall or no link
		return false;
	}
}
