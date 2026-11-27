// File persistence for savegame.txt and game_summary.txt using primitive variables
//Name: Ayyan Shehzad
//Roll No: i262638
//Project: The Last Train
#include "logger.h"
#include "util.h"
#include <fstream>
#include <iostream>
using namespace std;

//Saves the whole game into a text file so it can be continued later
bool saveGameStateToFile(const char* filename,
                         int playerFloor, int playerRoom,
                         int energy, int time, int score,
                         const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                         const bool objectives[NUM_OBJECTIVES], int emergencyTripsLeft,
                         const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                         const int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
	int i, floor, room, slot;

	//Validation
	if (filename == nullptr)
	{
		return false;
	}
	ofstream file(filename);
	if (!file)
	{
		return false;
	}

	//Single values, one per line
	file << "PLAYER_FLOOR " << playerFloor << endl;
	file << "PLAYER_ROOM " << playerRoom << endl;
	file << "ENERGY " << energy << endl;
	file << "TIME " << time << endl;
	file << "SCORE " << score << endl;
	file << "TRIPS_LEFT " << emergencyTripsLeft << endl;

	//Inventory
	file << "INVENTORY_COUNT " << inventoryCount << endl;
	file << "INVENTORY";
	for (i = 0; i < inventoryCount && i < MAX_INVENTORY_CAPACITY; i++)
	{
		file << " " << inventory[i];
	}
	file << endl;

	//Objectives, 1 means done and 0 means not done
	file << "OBJECTIVES";
	for (i = 0; i < NUM_OBJECTIVES; i++)
	{
		if (objectives[i] == true)
		{
			file << " 1";
		}
		else
		{
			file << " 0";
		}
	}
	file << endl;

	//Items still lying in every room
	for (floor = 0; floor < NUM_FLOORS; floor++)
	{
		for (room = 0; room < ROOMS_PER_FLOOR; room++)
		{
			file << "ROOM " << floor << " " << room << " " << entityCounts[floor][room];
			for (slot = 0; slot < entityCounts[floor][room] && slot < MAX_ENTITIES_PER_ROOM; slot++)
			{
				file << " " << roomEntities[floor][room][slot];
			}
			file << endl;
		}
	}

	file.close();
	return true;
}

//Reads a saved game back into the variables
bool loadGameStateFromFile(const char* filename,
                           int& playerFloor, int& playerRoom,
                           int& energy, int& time, int& score,
                           char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                           bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft,
                           char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                           int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
	char label[MAX_NAME_LEN];
	int i, floor, room, slot, count, flag;

	//Validation
	if (filename == nullptr)
	{
		return false;
	}
	ifstream file(filename);
	if (!file)
	{
		return false;
	}

	//Single values (the word in front of each number is only read and skipped)
	file >> label >> playerFloor;
	file >> label >> playerRoom;
	file >> label >> energy;
	file >> label >> time;
	file >> label >> score;
	file >> label >> emergencyTripsLeft;
	file >> label >> inventoryCount;
	if (!file)
	{
		return false;
	}

	//Checking that the numbers make sense
	if (playerFloor < 0 || playerFloor >= NUM_FLOORS || playerRoom < 0 || playerRoom >= ROOMS_PER_FLOOR)
	{
		return false;
	}
	if (inventoryCount < 0 || inventoryCount > MAX_INVENTORY_CAPACITY)
	{
		return false;
	}
	energy = clampInt(energy, 0, MAX_ENERGY);
	time = clampInt(time, 0, MAX_TIME);

	//Inventory
	file >> label;
	for (i = 0; i < MAX_INVENTORY_CAPACITY; i++)
	{
		inventory[i] = ENT_NONE;
	}
	for (i = 0; i < inventoryCount; i++)
	{
		file >> inventory[i];
	}

	//Objectives
	file >> label;
	for (i = 0; i < NUM_OBJECTIVES; i++)
	{
		file >> flag;
		objectives[i] = (flag == 1);
	}

	//Items in every room
	for (floor = 0; floor < NUM_FLOORS; floor++)
	{
		for (room = 0; room < ROOMS_PER_FLOOR; room++)
		{
			file >> label >> i >> i >> count;
			if (!file || count < 0 || count > MAX_ENTITIES_PER_ROOM)
			{
				return false;
			}
			entityCounts[floor][room] = count;
			for (slot = 0; slot < MAX_ENTITIES_PER_ROOM; slot++)
			{
				roomEntities[floor][room][slot] = ENT_NONE;
			}
			for (slot = 0; slot < count; slot++)
			{
				file >> roomEntities[floor][room][slot];
			}
		}
	}

	if (!file)
	{
		return false;
	}
	file.close();
	return true;
}

//Writes the final report when the game ends
void writeGameSummaryReport(const char* filename,
                            int score, int remainingEnergy, int remainingTime,
                            const bool objectives[NUM_OBJECTIVES], bool victory)
{
	int i, done;
	const char* names[NUM_OBJECTIVES] = {
		"Master Key collected", "Clue Digit 4 found", "Clue Digit 2 found", "Clue Digit 7 found",
		"Cabinet unlocked", "Train Key acquired", "Fuse collected", "Fuse installed",
		"Lift Pass collected", "Lift 2 active", "Passenger rescued", "Cat Carrier collected",
		"Cat secured", "Train prepared", "Mission won", "Mission failed"
	};

	//Validation
	if (filename == nullptr)
	{
		return;
	}
	ofstream file(filename);
	if (!file)
	{
		cerr << "Error: Could not write " << filename << endl;
		return;
	}

	file << "=== THE LAST TRAIN - GAME SUMMARY ===" << endl;
	if (victory == true)
	{
		file << "RESULT: VICTORY" << endl;
	}
	else
	{
		file << "RESULT: DEFEAT" << endl;
	}
	file << "FINAL SCORE: " << score << endl;
	file << "REMAINING ENERGY: " << remainingEnergy << endl;
	file << "REMAINING TIME: " << remainingTime << endl;
	file << endl;

	//Objective checklist
	file << "OBJECTIVES:" << endl;
	done = 0;
	for (i = 0; i < NUM_OBJECTIVES; i++)
	{
		if (objectives[i] == true)
		{
			file << "[X] " << names[i] << endl;
			done++;
		}
		else
		{
			file << "[ ] " << names[i] << endl;
		}
	}
	file << endl;
	file << "OBJECTIVES COMPLETED: " << done << " / " << NUM_OBJECTIVES << endl;

	file.close();
}