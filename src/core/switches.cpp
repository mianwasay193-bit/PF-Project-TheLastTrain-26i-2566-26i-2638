//Name: Abdul Wasay
//Roll No: i262566
//Project: The Last Train
// Door lock mechanics, lifts, and cabinet verification without structures
#include "switches.h"
#include "util.h"
#include "physics.h"
//Lift 1 works without any condition, but only between floor 1 and floor 2
bool canUseLift1(int fromFloor, int toFloor)
{
	if (fromFloor == 0 && toFloor == 1)
	{
		return true;
	}
	if (fromFloor == 1 && toFloor == 0)
	{
		return true;
	}
	return false;
}

//Lift 2 stops only at floor 2, floor 4 and floor 5
bool canUseLift2(int fromFloor, int toFloor, bool lift2Operational, int emergencyTripsLeft)
{
	//Both floors must be stops of Lift 2 and different from each other
	if (fromFloor != 1 && fromFloor != 3 && fromFloor != 4)
	{
		return false;
	}
	if (toFloor != 1 && toFloor != 3 && toFloor != 4)
	{
		return false;
	}
	if (fromFloor == toFloor)
	{
		return false;
	}

	//Repaired lift, or an emergency trip is still left
	if (lift2Operational == true || emergencyTripsLeft > 0)
	{
		return true;
	}
	return false;
}

//The locked door between F3-R1 and F3-R2 needs the Master Key
bool unlockFloor3Door(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount)
{
	return hasItem(inventory, inventoryCount, ENT_MASTER_KEY);
}

//Puts the fuse in the Control Panel, which repairs Lift 2
bool installFuseControlPanel(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                             bool objectives[NUM_OBJECTIVES], int& score)
{
	//The fuse must be carried and must not be installed already
	if (objectives[OBJ_FUSE_INSTALLED] == true)
	{
		return false;
	}
	if (hasItem(inventory, inventoryCount, ENT_FUSE) == false)
	{
		return false;
	}

	removeItem(inventory, inventoryCount, ENT_FUSE);
	objectives[OBJ_FUSE_INSTALLED] = true;
	objectives[OBJ_LIFT2_ACTIVE] = true;
	applyScoreDelta(score, SCORE_INSTALL_FUSE);
	return true;
}

//The cabinet opens with code 427 after all three clue digits are found
bool verifyCabinetCode(const char* enteredCode, const bool objectives[NUM_OBJECTIVES])
{
	//Validation
	if (enteredCode == nullptr)
	{
		return false;
	}

	//All three clues must be found first
	if (objectives[OBJ_CLUE_4] == false || objectives[OBJ_CLUE_2] == false || objectives[OBJ_CLUE_7] == false)
	{
		return false;
	}

	return stringEquals(enteredCode, "427");
}

