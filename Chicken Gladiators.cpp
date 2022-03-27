#include <iostream>
#include <string>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <windows.h>

using namespace std;

int screenWidth; //Width of the console

struct Player { string name = ""; int balance = 0; string chickenName = ""; int hasChicken = 0; int chickenStrength = 0; int chickenSpeed = 0; };

//Function declaration
string nickname(string str = "Choose your nickname: ");
void raceField(Player &p, string racers[], string* &winners, int numOfRacers);
int validateNum(string str);
int randomNum(int low = 0, int high = 0);
void loadData(fstream& load, Player& p);
void saveData(fstream& file, Player p);
void createNewSaveFile(fstream& file, Player& p);
void format(string pos, string print, bool newLine = true);
void theArena(Player& p, string enemies[], int numOfEnemies);
void chickenStore(Player& p);
void settings(Player& p, fstream& file);
void chickenRace(Player& p, string enemies[], int numOfEnemies);
bool betValidate(Player p, int betAmount);
void shuffleRacers(string racers[], int len);
void upgradeChicken(Player& p);

//Clearing screen
void cls() {
	system("CLS");
}

//Waiting for the user keypress
void pause() {
	printf("%s", "\n\t");
	system("pause");
}

int main() {
	srand((unsigned int)time(NULL));
	//Full screen window and console colors
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, 0);
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	ShowScrollBar(GetConsoleWindow(), SB_BOTH, FALSE);
	csbi.dwSize.X = csbi.dwMaximumWindowSize.X;
	csbi.dwSize.Y = csbi.dwMaximumWindowSize.Y;
	screenWidth = csbi.dwSize.X;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), csbi.dwSize);

	//Variables
	fstream file;
	Player player;
	string enemies[]{ "Kyle", "Drake", "Fran", "Ivan", "Killshock", "Dread", "Tai", "Stella", "Sven", "Crystal", "Kirin", "Thunder", "Speed", "John", "Derrick", "Dalaar", "Sincer", "Silent", "Sarus", "Solomon", "Xorin" };
	int selection = 0, numOfEnemies = sizeof(enemies) / sizeof(enemies[0]);

	//Checking if save file exists
	file.open("savefile.bin", ios::in);
	int opened = file.fail();
	file.close();

	if (opened) { //Save file doesn't exist, prompt user to create new save file
		do {
			do {
				cls();
				format("text-top", "No save file found on your computer, would you like to create a save file?\n\t1. Yes\n\t2. No");
				format("text-left", "Input: ", false);
				selection = validateNum("Input: ");
			} while (selection != 1 && selection != 2);

			if (selection == 2) { //User doesn't want to create a new save file
				do {
					cls();
					format("text-top", "Without a save file, you won't be able to save your progress.\n\tAre you sure you want to continue?\n\t1. Yes\n\t2. No");
					format("text-left", "\nInput: ", false);
					selection = validateNum("\nInput: ");
				} while (selection != 1 && selection != 2);

				if (selection == 2) { //User changed mind, wants to create a new save file
					continue; //Skip to next iteration
				}
				else if (selection == 1) { //Starting game without save file
					cls();
					//Getting nickname
					player.name = nickname();
					player.balance = 1000;
					player.chickenName = player.name + "'s chicken";
					player.hasChicken = 0;
					player.chickenSpeed = 1;
					player.chickenStrength = 1;
				}
			}
			else if (selection == 1) { //User wants to create a new save file
				createNewSaveFile(file, player);
			}
		} while (selection == 2);
	}
	else { //Save file exists, ask user to load data or overwrite save file
		do {
			do {
				cls();
				format("text-top", "Save file found. Would you like to load data, or start a new game?\n\t1. Load game\n\t2. New game");
				format("text-left", "Input: ", false);
				selection = validateNum("Input: ");
			} while (selection != 1 && selection != 2);

			if (selection == 2) { //Player wants to start a new game
				do {
					cls();
					format("text-top", "This will overwrite your existing save file, are you sure you want to continue?\n\t1. Yes\n\t2. No");
					format("text-left", "Input: ", false);
					selection = validateNum("Input: ");
				} while (selection != 1 && selection != 2);

				if (selection == 2) {
					continue;
				}
				else if (selection == 1) {
					createNewSaveFile(file, player);
				}
			}
			else if (selection == 1) { //Player wants to load saved data
				loadData(file, player);
			}
		} while (selection == 2);
	}


	//Main
	do {
		system("CLS");
		format("text-center", "Chicken Gladiators");
		format("text-top", "1. Enter the arena\n\t2. Chicken race\n\t3. Chicken store\n\t4. Upgrade\n\t5. Settings\n\t0. Exit the game");
		format("text-left", "Input: ", false);
		selection = validateNum("Input: ");
		switch (selection) {
		case 1: //Player entered gladiator arena
			if (player.hasChicken == 0) {
				cls();
				format("text-top", "You don't own a chicken to send into a fight!\n\tPlease purchase one before entering the arena.");
				pause();
				continue;
			}
			theArena(player, enemies, numOfEnemies);
			continue;

		case 2: //Player chose chicken race
			if (player.hasChicken == 0) {
				cls();
				format("text-top", "You don't own a chicken to send to race!\n\tPlease purchase one before entering the race.");
				pause();
				continue;
			}
			chickenRace(player, enemies, numOfEnemies);
			continue;

		case 3: //Player is purchasing a chicken
			chickenStore(player);
			continue;

		case 4: //Player is upgrading a chicken
			if (player.hasChicken == 0) {
				cls();
				format("text-top", "You don't own a chicken to upgrade.\n\tPlease purchase one before upgrading it.");
				pause();
				continue;
			}
			upgradeChicken(player);
			continue;

		case 5: //Display settings
			settings(player, file);
			continue;
		}
	} while (selection != 0);

	return 0;
}

// FUNCTION DEFINITIONS //
//Random number generator (bounded)
int randomNum(int low, int high) {
	if (low == 0 && high == 0) return rand() % 101; //No parameters provided
	//Provided lower bound only
	if (low != 0 && high == 0) { //Provided lower bound only
		//If lower > 100
		if (low > 100) return (rand() % (low - 100 + 1)) + 100; //Return integer between 100 and low = high
		else return (rand() % (100 - low + 1)) + low; //Return random between lower and 100
	}
	return (rand() % (high - low + 1)) + low; //Bounded
}

//Formatting text output
void format(string pos, string print, bool newLine) {
	int i = 0, max_chars = screenWidth, len = print.length();

	//Marginate text center
	if (pos == "text-center") {
		string temp = "";
		for (i = 0; i < (max_chars - len) / 2; i++) temp += " ";
		printf("\n%s%s\n\n\n\n\n\n", temp.c_str(), print.c_str());
		return;
	}

	//Marginate text left
	if (pos == "text-left") {
		(newLine) ? printf("\t%s\n", print.c_str()) : printf("\n\t%s", print.c_str());
		return;
	}

	//Marginate text top
	if (pos == "text-top") {
		(newLine) ? printf("\n\n\n\n\n\n\t%s\n", print.c_str()) : printf("\n\n\n\n\n\n\t%s", print.c_str());
		return;
	}
}

//Integer number validation
int validateNum(string str) {
	int x;
	while (!(cin >> x)) {
		(str == "") ? format("text-left", "Invalid input. Please try again: ", false) : format("text-left", str, false);
		cin.clear();
		cin.ignore(100, '\n');
	}
	cin.clear();
	cin.ignore(100, '\n');
	return x;
}

//Inputting player and chicken names
string nickname(string str) {
	string nick;
	do {
		cls();
		format("text-top", str, false);
		cin.clear();
		getline(cin, nick);

		if (nick.length() > 20 || nick.length() < 4) {
			format("text-left", "Name must be between 4 and 20 characters long.");
			pause();
		}
	} while (nick.length() > 20 || nick.length() < 4);
	return nick;
}

//Load game data
void loadData(fstream& load, Player& p) {
	cls();
	format("text-top", "Loading data, please wait. . .");
	load.open("savefile.bin", ios::in);

	string line;
	unsigned int i = 0;
	getline(load, line);

	//Loading player name
	for (i = 0; i < line.length(); i++) if (line[i] == ';') break;
	p.name = line.substr(0, i);
	line = line.substr(i + 1, line.length());

	//Loading balance
	for (i = 0; i < line.length(); i++) if (line[i] == ';') break;
	p.balance = stoi(line.substr(0, i));
	line = line.substr(i + 1, line.length());

	//Loading player chicken name
	for (i = 0; i < line.length(); i++) if (line[i] == ';') break;
	p.chickenName = line.substr(0, i);
	line = line.substr(i + 1, line.length());

	//Loading has chicken
	for (i = 0; i < line.length(); i++) if (line[i] == ';') break;
	p.hasChicken = stoi(line.substr(0, i));
	line = line.substr(i + 1, line.length());

	//Loading chickenStrength
	for (i = 0; i < line.length(); i++) if (line[i] == ';') break;
	p.chickenStrength = stoi(line.substr(0, i));
	line = line.substr(i + 1, line.length());

	//Loading chickenSpeed
	for (i = 0; i < line.length(); i++) if (line[i] == ';') break;
	p.chickenSpeed = stoi(line.substr(0, i));

	//Closing file
	load.close();
	format("text-left", "Loading successful.");
	pause();
}

//Save game data
void saveData(fstream& file, Player p) {
	cls();
	format("text-top", "Saving data, please wait. . .");

	//Saving the game
	file.open("savefile.bin", ios::out);
	file << p.name << ";" << p.balance << ";" << p.chickenName << ";" << p.hasChicken << ";" << p.chickenStrength << ";" << p.chickenSpeed << ";";
	file.close();

	format("text-left", "Saving successful.");
	pause();
}

//Creating new save file
void createNewSaveFile(fstream& file, Player& p) {
	cls();
	string name = nickname();
	format("text-left", "Creating new save file, please wait . . .");

	//Creating and saving new file
	file.open("savefile.bin", ios::out);
	file << name << ";1000;" << name << "'s chicken;0;0;0;";
	file.close();

	//Initializing new game variables
	p.name = name;
	p.balance = 1000;
	p.chickenName = name + "'s chicken";
	p.hasChicken = 0;
	p.chickenStrength = 1;
	p.chickenSpeed = 1;

	format("text-left", "New save file created successfully.");
	pause();
}

//The gladiator arena
void theArena(Player& p, string enemies[], int numOfEnemies) {
	//Variables
	bool newEnemy = true;
	int randEnemy = 0, betAmount = 0, enemyChickenStrength = 1;

	do {
		cls();
		if (newEnemy == true) {
			randEnemy = randomNum(0, numOfEnemies - 1);
			newEnemy = false;
		}
		
		//Determining enemy chicken's strength based on player's chicken strength
		switch (p.chickenStrength) {
		case 10: //Player's chicken strength is maxed out
			enemyChickenStrength = p.chickenStrength - randomNum(0, 2);
			break;
		case 1: //Player's chicken strength is level 1
		case 2:
			enemyChickenStrength = p.chickenStrength + randomNum(0, 2);
			break;
		default:
			enemyChickenStrength = p.chickenStrength + randomNum(-2, 2);
		}

		format("text-center", "The Arena");
		format("text-center", p.chickenName + " (" + to_string(p.chickenStrength) + ")" + "    vs.    " + enemies[randEnemy] + " (" + to_string(enemyChickenStrength) + ")");
		format("text-top", "Your balance: $" + to_string(p.balance));
		format("text-left", "Place your bet: ", false);
		betAmount = validateNum("Place your bet: ");

		//If invalid bet, go back to the start
		if (!betValidate(p, betAmount)) {
			continue;
		}
		else {
			p.balance -= betAmount;
			int chance = randomNum(1, 20);
			bool won = false;

			//Determining the win/lose
			if (p.chickenStrength > enemyChickenStrength || p.chickenStrength == enemyChickenStrength) {
				if (chance <= p.chickenStrength) won = true;
			}
			else {
				(chance <= enemyChickenStrength) ? won = false : won = true;
			}

			//Chicken won
			if (won) {
				format("text-left", p.chickenName + " has won the battle and made you $" + to_string(betAmount) + " richer!");
				p.balance += betAmount * 2;
				newEnemy = true;

				pause();
				continue;
			}

			//Chicken died
			else {
				format("text-left", p.chickenName + " has died.");
				p.hasChicken = 0;
				p.chickenName = p.name + "'s chicken";
				break;
			}
		}
	} while (betAmount != 0);
	pause();
}

//Entering the chicken store
void chickenStore(Player& p) {
	int selection = 0;

	do {
		cls();
		format("text-center", "Chicken store");
		format("text-top", "Balance: $" + to_string(p.balance) + "\n\tChicken cost: $100\n\n\tWould you like to purchase one chicken?\n\t1. Yes\n\t2. No");
		format("text-left", "Input: ", false);
		selection = validateNum("Input: ");
	} while (selection != 1 && selection != 2);

	//Player chose to purchase a chicken
	if (selection == 1) {

		//Player doesn't own a chicken
		if (p.hasChicken == 0) {

			//Player has enough to purchase a chicken
			if (p.balance - 100 >= 0) {
				cls();
				format("text-top", "Successfully purchased a chicken!");
				p.balance -= 100;
				p.hasChicken = 1;
				p.chickenStrength = 1;
				p.chickenSpeed = 1;
				pause();
			}

			//Player doesn't have enough to purchase a chicken
			else {
				cls();
				format("text-top", "Not enough money to purchase a chicken.");
				pause();
			}
		}

		//Player owns a chicken
		else {
			cls();
			format("text-top", "You already own a chicken.");
			pause();
		}
	}
}

//Opening settings menu
void settings(Player& p, fstream& file) {
	int selection = 0;
	unsigned char c = 219;
	unsigned char d = 176;

	do {
		cls();
		format("text-center", "Settings");
		if (p.hasChicken == 0) {
			format("text-top", "Player: " + p.name + "\n\tBalance: $" + to_string(p.balance) + "\n\tDoesn't have a chicken\n");
		}
		else {
			format("text-top", "Player: " + p.name + "\n\tBalance: $" + to_string(p.balance) + "\n\tChicken: " + p.chickenName + "\n\n\t== " + p.chickenName + "'s stats ==");
			string temp = "";
			for (int i = 1; i < 10; i++) (i < p.chickenSpeed) ? temp += c : temp += d;
			format("text-left", "Speed:    " + temp + " Level " + to_string(p.chickenSpeed));
			temp = "";
			for (int i = 1; i < 10; i++) (i < p.chickenStrength) ? temp += c : temp += d;
			format("text-left", "Strength: " + temp + " Level " + to_string(p.chickenStrength) + '\n');
		}
		format("text-left", "1. Edit player name\n\t2. Edit chicken name\n\t3. Manual\n\t4. Save\n\t5. Load\n\t6. Reset game\n\t0. Return to main menu");
		format("text-left", "Input: ", false);
		selection = validateNum("Input: ");

		//Changing player name
		if (selection == 1) {
			p.name = nickname();
			format("text-left", "Nickname changed.");
			pause();
			continue;
		}

		//Change chicken name
		if (selection == 2) {

			//Player has chicken
			if (p.hasChicken != 0) {
				p.chickenName = nickname("\n\tChoose your chicken's nickname: ");
				format("text-left", "Nickname changed.");
			}

			//Player doesn't own a chicken
			else {
				cls();
				format("text-top", "You don't own a chicken.");
			}

			pause();
			continue;
		}

		//Displaying manual
		if (selection == 3) {
			cls();
			format("text-center", "How to play the game");
			format("text-top", "Purchase a chicken in order to play the mini games\n\tPlayers starts with $1000 and no chicken\n\tChickens cost $100\n\tThere are couple of games you can play: The arena, Chicken race\n\tYou can exit any game by inputting 0 as bet.");
			format("text-left", "\n");
			format("text-left", "== The arena ==\n\tPlayers enter the arena with their chickens and challenge other chickens to a gladiator fight.\n\tPlayers place a bet, and if their chicken beats the opponent, they win the bet.\n\tChickens have strength attribute which increases the likelihood of it winning\n\tCan be upgraded by purchasing upgrade points (see Upgrade in main menu).");
			format("text-left", "\n");
			format("text-left", "Chicken race\n\tPlayers enter the race by placing a bet and defining the number of racers (minimum of 5, maximum of 10 racers).\n\tChickens have their speed attribute which determins how fast they'll finish the track.\n\tEach racer's track has hidden bombs planted which kills racer if stepped on. There is a 5% chance racers will step on the bomb.\n\tYou can upgrade a chicken's speed by purchasing upgrade points.\n\n\tRewards:\n\t1st place: triple the bet\n\t2nd place: double the bet\n\t3rd place: original bet");
			pause();
			continue;
		}

		//Save game
		if (selection == 4) {
			saveData(file, p);
			continue;
		}

		//Load game
		if (selection == 5) {
			do {
				cls();
				format("text-top", "You will lose your current data. Are you sure you want to continue?\n\t1. Yes\n\t2. No");
				format("text-left", "Input: ", false);
				selection = validateNum("Input: ");
			} while (selection != 1 && selection != 2);

			if (selection == 1) {
				loadData(file, p);
			}

			continue;
		}

		//Reset stats
		if (selection == 6) {
			do {
				cls();
				format("text-top", "This will erase current data. Are you sure you want to continue?\n\t1. Yes\n\t2. No");
				format("text-left", "Input: ", false);
				selection = validateNum("Input: ");
			} while (selection != 1 && selection != 2);

			if (selection == 1) {
				createNewSaveFile(file, p);
			}

			continue;
		}
	} while (selection != 0);
}

//Chicken race
void chickenRace(Player& p, string enemies[], int numOfEnemies) {
	//Variables
	int betAmount = 0, i = 0, offset = 0, numOfRacers = 0;
	string* racers = NULL;
	string* winners = new string[3];

	do {
		cls();

		format("text-center", "Chicken race");
		format("text-top", "Your balance: $" + to_string(p.balance));

		//Prompting for the bet
		format("text-left", "Place your bet: ", false);
		betAmount = validateNum("Place your bet: ");

		//Invalid bet or 0
		if (!betValidate(p, betAmount)) {
			continue;
		}
		else {
			//Prompting for number of players
			do {
				format("text-left", "Number of racers: ", false);
				numOfRacers = validateNum("Number of racers: ");

				if (numOfRacers >= 5 && numOfRacers <= 10) racers = new string[numOfRacers];
			} while (numOfRacers < 5 || numOfRacers > 10);

			//Deducting from player's balance
			p.balance -= betAmount;

			//Setting racers
			racers[0] = p.chickenName; //First racer is always player
			for (i = 1; i < numOfRacers; i++) {
				if (offset == 0) offset = randomNum(0, 11);
				racers[i] = enemies[i + offset];
			}

			//Shuffling racers
			shuffleRacers(racers, numOfRacers);

			//Race
			raceField(p, racers, winners, numOfRacers);

			//Race finished
			format("text-top", "== Winners ==\n");
			for (i = 0; i < 3; i++) {
				format("text-left", to_string(i + 1) + ". place: " + *(winners + i) + " (Reward: " + to_string(betAmount * (3 - i))+ ")");

				//Rewarding a player if placed in top 3
				if (*(winners + i) == p.chickenName) p.balance += betAmount * (3 - i);
			}

			//Resetting offset
			offset = 0;

			//Player's chicken died in a race
			if (p.hasChicken == 0) break;
			pause();
		}
	} while (betAmount != 0);
	pause();

	delete[] winners;
	delete[] racers;
}

//Checking is player inputs a valid bet
bool betValidate(Player p, int betAmount) {
	//Player input a negative bet
	if (betAmount < 0) {
		format("text-left", "Invalid bet");
		pause();
		return false;
	}

	//Player bet more than what they have
	if (betAmount > p.balance) {
		format("text-left", "You don't have enough money.");
		pause();
		return false;
	}

	//Player exits arena
	if (betAmount == 0) {
		return false;
	}

	return true;
}

void raceField(Player &p, string racers[], string* &winners, int numOfRacers) {
	//Variables
	int i = 0, j = 0, placement_increment = 0, raceInProgress = 0, counter = 3, bombSpot = 0;
	int* placement_racers = new int[numOfRacers];
	int* step = new int[numOfRacers];
	int* chickensSpeed = new int[numOfRacers];
	string* placement_text = new string[numOfRacers];
	bool* steppedOnABomb = new bool[numOfRacers];
	bool raceFinished = false;
	string temp1 = "", temp2 = "", temp3 = "", temp4 = "";
	unsigned char d = 219;

	//Initializing the arrays
	for (i = 0; i < numOfRacers; i++) {
		step[i] = 0;
		placement_racers[i] = 0;
		steppedOnABomb[i] = false;
		if(i == 0) placement_text[i] = " " + to_string(i + 1) + "st place";
		else if(i == 1) placement_text[i] = " " + to_string(i + 1) + "nd place";
		else if(i == 2) placement_text[i] = " " + to_string(i + 1) + "rd place";
		else placement_text[i] = " " + to_string(i + 1) + "th place";

		//Determing enemy chickens' speed
		if (racers[i] == p.chickenName) chickensSpeed[i] = p.chickenSpeed;
		else if (p.chickenSpeed == 10) chickensSpeed[i] = p.chickenSpeed - randomNum(2, 4);
		else if (p.chickenSpeed == 1 || p.chickenSpeed == 2) chickensSpeed[i] = p.chickenSpeed + randomNum(0, 1);
		else chickensSpeed[i] = p.chickenSpeed + randomNum(-2, 0);
	}

	//Printing roads the first time
	cls();
	format("text-center", "Chicken race");
	for (i = 0; i < 87; i++) temp1 += "-";
	format("text-left", "|Start" + temp1 + "Finish|\n");
	for (i = 0; i < numOfRacers; i++) {
		(racers[i] == p.chickenName) ? printf("\t >> %s (Speed: %i) \n\t 0%c\n\n", racers[i].c_str(), chickensSpeed[i], '%') : printf("\t %s (Speed: %i) \n\t 0%c\n\n", racers[i].c_str(), chickensSpeed[i], '%');
	}
	printf("\n\tSetting up the track. . .\n\t");
	Sleep(2000);
	do {
		printf("%i\n\n\t", counter);
		counter--;
		Sleep(1000);
	} while (counter > 0);

	//Looping
	do {
		//Variables
		temp1 = temp2 = temp3 = temp4 = "";
		bombSpot = randomNum(1, 90);

		cls();
		format("text-center", "Chicken race");
		for (i = 0; i < 87; i++) temp1 += "-";
		format("text-left", "|Start" + temp1 + "Finish|\n");

		for (i = 0; i < numOfRacers; i++) {
			//Incrementing racers' steps
			if (!steppedOnABomb[i]) step[i] += randomNum(1, 4 + chickensSpeed[i]);
			if (step[i] >= 100) step[i] = 100;

			//Chicken stepped on a bomb
			if (!steppedOnABomb[i] && step[i] == bombSpot) {
				if (racers[i] == p.chickenName) p.hasChicken = 0;
				steppedOnABomb[i] = true;
			}

			//Racer finished a race and earned placement
			if (step[i] == 100 && placement_racers[i] == 0 && !steppedOnABomb[i]) {
				placement_increment += 1;
				placement_racers[i] = placement_increment;
			}

			//Saving the road into variable to later print
			temp1 = "", temp2 = "", temp3 = "";
			for (j = 0; j < step[i]; j++) temp1 += d;
			(steppedOnABomb[i]) ? temp3 = " (stepped on a bomb)" : "";
			(placement_racers[i] == 0) ? temp2 += temp1 + " " + to_string(step[i]) + "%\n\n" : temp2 += temp1 + " " + to_string(step[i]) + "%" + placement_text[placement_racers[i] - 1] + "\n\n";
			(racers[i] == p.chickenName) ? temp4 += "\t >> " + racers[i] + " (Speed: " + to_string(chickensSpeed[i]) + ")" + temp3 + "\n\t" + temp2 : temp4 += "\t " + racers[i] + " (Speed: " + to_string(chickensSpeed[i]) + ")" + temp3 + "\n\t" + temp2;
		}

		//Looping through players to see if everyone finished the race
		for (i = 0; i < numOfRacers; i++) {
			if (step[i] < 100 && !steppedOnABomb[i]) {
				raceInProgress = 1;
				break;
			}
		}
		//Checking if race finished
		if (raceInProgress == 0) {
			raceFinished = true;
			temp4 += "\tRace is finished!\n\n";
		}
		else {
			raceInProgress = 0;
			temp4 += "\tGo!\n\n";
		}

		//Printing the roads
		printf("%s", temp4.c_str());

		Sleep(100);
	} while (!raceFinished);

	//Announcing top 3 placements (If survived)
	for (i = 0; i < numOfRacers; i++) {
		if (placement_racers[i] == 1) *(winners) = racers[i];
		else if (placement_racers[i] == 2) *(winners + 1) = racers[i];
		else if (placement_racers[i] == 3) *(winners + 2) = racers[i];
	}

	//Deleting arrays
	delete[] placement_racers;
	delete[] step;
	delete[] placement_text;
	delete[] chickensSpeed;
	delete[] steppedOnABomb;
}

void shuffleRacers(string racers[], int len) {
	int i = 0, j = 0;
	string temp = "";
	srand((unsigned int)time(NULL));
	for (i = len - 1; i > 0; i--) {
		j = rand() % (i + 1);
		temp = racers[i];
		racers[i] = racers[j];
		racers[j] = temp;
	}
}

void upgradeChicken(Player& p) {
	int input = 0, i = 0, speedUpgrade = 0, strengthUpgrade = 0;
	string temp = "";
	unsigned char c = 177;
	unsigned char d = 219;
	int upgradeCostSpeed[]{ 5000, 10000, 20000, 40000, 80000, 160000, 320000, 640000, 1280000 };
	int upgradeCostStrength[]{ 20000, 40000, 80000, 160000, 320000, 640000, 1280000, 2560000, 5120000 };

	do {
		cls();
		format("text-center", "Upgrading " + p.chickenName);

		//Chicken's speed bar
		temp = "1. " + p.chickenName + "'s speed:    \t";
		for (i = 1; i < 10; i++) (i < p.chickenSpeed) ? temp += d : temp += c;
		(p.chickenSpeed == 10) ? temp += " Level " + to_string(p.chickenSpeed) + " Maxed out." : temp += " Level " + to_string(p.chickenSpeed) + " (Next upgrade: $" + to_string(upgradeCostSpeed[speedUpgrade]) + ")";
		format("text-left", temp);

		//Chicken's strength bar
		temp = "2. " + p.chickenName + "'s strength: \t";
		for (i = 1; i < 10; i++) (i < p.chickenStrength) ? temp += d : temp += c;
		(p.chickenStrength == 10) ? temp += " Level " + to_string(p.chickenStrength) + " Maxed out.\n" : temp += " Level " + to_string(p.chickenStrength) + " (Next upgrade: $" + to_string(upgradeCostStrength[strengthUpgrade]) + ")\n";
		format("text-left", temp);

		//Printing balance
		format("text-left", "Your balance: $" + to_string(p.balance));

		//If chicken is not maxed out
		if (p.chickenStrength < 10 || p.chickenSpeed < 10) {
			//Choosing upgrade
			format("text-left", "Input: ", false);
			input = validateNum("Input: ");

			if (input != 0) {
				//Speed upgrade
				if (input == 1) {
					//Is speed already maxed out?
					if (p.chickenSpeed < 10) {
						//Player has enough to upgrade
						if (p.balance > upgradeCostSpeed[p.chickenSpeed - 1]) {
							p.balance -= upgradeCostSpeed[p.chickenSpeed - 1];
							p.chickenSpeed += 1;
							speedUpgrade += 1;
							format("text-left", "Successfully upgraded speed!");
						}
						//Player doesn't have enough to upgrade
						else format("text-left", "Not enough money to upgrade your chicken.");
					}
					else format("text-left", "Speed is already maxed out.");
					pause();
				}
				//Strength upgrade
				else if (input == 2) {
					//Is strength already maxed out?
					if (p.chickenStrength < 10) {
						//Player has enough to upgrade
						if (p.balance > upgradeCostStrength[p.chickenStrength - 1]) {
							p.balance -= upgradeCostStrength[p.chickenStrength - 1];
							p.chickenStrength += 1;
							strengthUpgrade += 1;
							format("text-left", "Successfully upgraded strength!");
						}
						//Player doesn't have enough to upgrade
						else format("text-left", "Not enough money to upgrade your chicken.");
					}
					else format("text-left", "Strength is already maxed out.");
					pause();
				}
				continue;
			}
		}
		//Chicken is maxed out
		else {
			format("text-left", "\n\tYour chicken's maxed out.");
			pause();
			break;
		}

	} while (input != 0);
}