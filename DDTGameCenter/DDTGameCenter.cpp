// DDTGameCenter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

const int MAX_SAVE_NUMBER = 10;
const int MAX_GRID_HEIGHT = 10;
const int MAX_GRID_WIDTH = 10;

bool is_digits(const std::string &str)
{
	return str.find_first_not_of("-0123456789") == std::string::npos;
}
class GameCenter;
class Command
{
public:
	Command()
	{
		save_point = -1;
		numberOfUndos = 0;
	}
	//get a string and convert it to a command
	void command(GameCenter& previous_game, GameCenter& whereReadFrom, GameCenter& whereSaveTo);
	void search(GameCenter& whereReadFrom, GameCenter& whereSaveTo);
	bool undo()
	{
		if (save_point >= 1) { save_point--; numberOfUndos++; return true; }
		else { cout << "cannot undo\n"; return false; }
	}
	bool redo()
	{
		if (numberOfUndos > 0) { save_point++; numberOfUndos--; return true; }
		else { cout << "cannot redo\n"; return false; }
	}
	void change(int row, int col, int bombType, GameCenter& whereCopyFrom, GameCenter& whereSaveTo);
	//notice that when using the variable save_point, remember to use save_point % MAX_SAVE_NUMBER instead.
	int save_point = 0;
	bool logOn = false;
	bool helped = false;
private:
	//for command
	int numberOfUndos;
};
class GameCenter
{
public:
	GameCenter()
	{
		//initialize state, score, usingCopy, save_point
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				state[row][col] = -1;
				score[row][col] = 0;
				for (int copy = 0; copy < 2; copy++)
					for (int direction = 0; direction < 4; direction++)
						bullet[copy][row][col][direction] = 0;
			}
		usingCopy = 0;
	}
	void copy(GameCenter& copySource)
	{
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				state[row][col] = copySource.state[row][col];
				score[row][col] = copySource.score[row][col];
				for (int i = 0; i < 2; i++)
				{
					for (int direction = 0; direction < 4; direction++)
					{
						bullet[i][row][col][direction] = copySource.bullet[i][row][col][direction];
					}
				}
			}
		}
		usingCopy = copySource.usingCopy;
	}
	void click(int Row, int Col, bool logOn)
	{
		if (state[Row][Col] > 0)
			state[Row][Col] --;
		//fire bullets
		if (state[Row][Col] == 0)
		{
			for (int direction = 0; direction < 4; direction++)
				bullet[usingCopy][Row][Col][direction] += 1;
		}
		while (bulletsExist(usingCopy))
		{
			usingCopy = 1 - usingCopy;
			//clear bullets of this usingCopy
			for (int row = 0; row < MAX_GRID_HEIGHT; row++)
				for (int col = 0; col < MAX_GRID_WIDTH; col++)
					for (int direction = 0; direction < 4; direction++)
						bullet[usingCopy][row][col][direction] = 0;
			//bullets fly and -3 fire at this usingCopy frame
			for (int row = 0; row < MAX_GRID_HEIGHT; row++)
			{
				for (int col = 0; col < MAX_GRID_WIDTH; col++)
				{
					if (state[row][col] == -3)
					{
						for (int direction = 0; direction < 4; direction++)
						{
							bullet[usingCopy][row][col][direction] ++;
							state[row][col] = 0;
						}
					}
					if (bullet[1 - usingCopy][row][col][LEFT] > 0)
					{
						if (col - 1 >= 0)
						{
							bullet[usingCopy][row][col - 1][LEFT] += bullet[1 - usingCopy][row][col][LEFT];
						}
					}
					if (bullet[1 - usingCopy][row][col][RIGHT] > 0)
					{
						if (col + 1 < MAX_GRID_WIDTH)
						{
							bullet[usingCopy][row][col + 1][RIGHT] += bullet[1 - usingCopy][row][col][RIGHT];
						}
					}
					if (bullet[1 - usingCopy][row][col][UP] > 0)
					{
						if (row - 1 >= 0)
						{
							bullet[usingCopy][row - 1][col][UP] += bullet[1 - usingCopy][row][col][UP];
						}
					}
					if (bullet[1 - usingCopy][row][col][DOWN] > 0)
					{
						if (row + 1 < MAX_GRID_HEIGHT)
						{
							bullet[usingCopy][row + 1][col][DOWN] += bullet[1 - usingCopy][row][col][DOWN];
						}
					}
				}
			}
			//bullet collision and fire
			for (int row = 0; row < MAX_GRID_HEIGHT; row++)
			{
				for (int col = 0; col < MAX_GRID_WIDTH; col++)
				{
					for (int direction = 0; direction < 4; direction++)
					{
						if (bullet[usingCopy][row][col][direction] > 0)
						{
							if (state[row][col] == -2)
							{
								bullet[usingCopy][row][col][direction] = 0;
							}
							else if (state[row][col] > 0)
							{
								if (bullet[usingCopy][row][col][direction] >= state[row][col])
								{
									bullet[usingCopy][row][col][direction] += -state[row][col];
									state[row][col] = -3;
								}
								else
								{
									state[row][col] += -bullet[usingCopy][row][col][direction];
									bullet[usingCopy][row][col][direction] = 0;
								}
							}
						}
					}
				}
			}
			if (logOn)
			{
				this->print();
				cout << endl;
			}
		}

		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				if (state[row][col] == 0)
				{
					score[row][col]++;
				}
			}
		}
		roundScore = 0;
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				if (state[row][col] == 0)
				{
					roundScore += score[row][col];
				}
			}
		}
		//change 0 to -1 for the preparation for the score next round
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				if (state[row][col] == 0)
				{
					state[row][col] = -1;
				}
			}
		}
	}
	int getState(int Row, int Col)
	{
		return state[Row][Col];
	}
	int getRoundScore()
	{
		return roundScore;
	}
	int getScore(int Row, int Col)
	{
		return score[Row][Col];
	}
	int getTotalScore()
	{
		int totalScore = 0;
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
				totalScore += score[row][col];
		return totalScore;
	}
	void get()
	{
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				cin >> state[row][col];
				while (cin.fail())
				{
					cin.clear();
					cin.ignore();
					cout << "error. Please start from row " << row << " col " << col << endl;
					row -= (row*MAX_GRID_WIDTH + col - 1) / MAX_GRID_WIDTH;
					col--;
				}
			}
		}
	}
	void print()
	{
		//negative: bomb positive: score
		cout << setw(4) << " ";
		for (int col = 0; col < MAX_GRID_WIDTH; col++) cout << setw(4) << col;
		cout << endl;
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
			cout << row << setw(4) << " ";
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			cout << "|";
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				if (state[row][col] == -2)
				{
					cout << setw(3) << "sto";
				}
				else if (state[row][col] == -1)
				{
					cout << setw(3) << " ";
				}
				else if (state[row][col] == 0)
				{
					cout << setw(3) << score[row][col];
				}
				else if (state[row][col] == 1)
				{
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
					cout << setw(3) << "XXX" << flush;
				}
				else if (state[row][col] == 2)
				{
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE);
					cout << setw(3) << "XXX" << flush;
				}
				else if (state[row][col] == 3)
				{
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE);
					cout << setw(3) << "XXX" << flush;
				}
				else if (state[row][col] == 4)
				{
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
					cout << setw(3) << "XXX" << flush;
				}
				else if (state[row][col] == 5)
				{
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					cout << setw(3) << "XXX" << flush;
				}
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				cout << "|";
			}
			cout << endl;
		}
	}
	bool bulletsExist(int copy)
	{
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				if (state[row][col] == -3) return true;
				for (int direction = 0; direction < 4; direction++)
					if (bullet[copy][row][col][direction] != 0) return true;
			}
		}
		return false;
	}
	//command
	friend void Command::change(int row, int col, int bombType, GameCenter& whereCopyFrom, GameCenter& whereSaveTo);//TODO:save previous game

private:
	//numbers mean remaining clicks to bomb -3:will fire in next frame-2:stone -1:nothing 0:bomb(score) 1:gold 2:deep blue 3:water blue 4:green 5:gray
	int state[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];
	//record scores from the start to the end
	int score[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];
	//record the direction and the position of bullets and it has two copies to swap;
	int bullet[2][MAX_GRID_HEIGHT][MAX_GRID_WIDTH][4];
	int usingCopy;
	int roundScore;

protected:
	//direction
	const int LEFT = 2;
	const int RIGHT = 1;
	const int UP = 3;
	const int DOWN = 0;
};
//declaration for command::change
void Command::change(int row, int col, int bombType, GameCenter& whereCopyFrom, GameCenter& whereSaveTo)
{
	save_point++;
	whereSaveTo.copy(whereCopyFrom);
	whereSaveTo.state[row][col] = bombType;
}
void Command::command(GameCenter& previous_game, GameCenter& whereReadFrom, GameCenter& whereSaveTo)
{
	if (!helped)
	{
		cout << "HELP" << endl
			<< "undo:" << endl
			<< "restart:" << endl
			<< "load:" << endl
			<< "show:" << endl
			<< "search:" << endl
			<< "log on/off" << endl
			<< "change to %bombType at %row %col:" << endl;
		helped = true;
	}

	string command_string;
	cout << endl;
	getline(cin, command_string);
	cout << endl;
	if (command_string == "undo")
	{
		if (undo())
		{
			cout << "save point is now at " << save_point << endl;
			previous_game.print();
		}

		return;
	}
	else if (command_string == "redo")
	{
		if (redo())
		{
			cout << "save point is now at " << save_point << endl;
			whereSaveTo.print();
		}
		return;
	}
	else if (command_string.substr(0, 4) == "show")
	{
		if (save_point > -1)
		{
			whereReadFrom.print();
			cout << endl;
		}
		return;
	}
	else if (command_string.substr(0, 6) == "search")
	{
		search(whereReadFrom, whereSaveTo);
		return;
	}
	else if (command_string.substr(0, 7) == "restart")
	{
		save_point = (save_point + 1) % MAX_SAVE_NUMBER;
		GameCenter newGame;
		whereSaveTo.copy(newGame);
		helped = false;
		return;
	}
	else if (command_string.substr(0, 3) == "log")
	{
		if (command_string.substr(4, 2) == "on")
		{
			logOn = true;
			cout << "log is now on\n";
		}
		else if (command_string.substr(4, 3) == "off")
		{
			logOn = false;
			cout << "log is now off\n";
		}
		return;
	}
	else if (command_string.substr(0, 4) == "load")
	{
		save_point++;
		string trash;
		GameCenter newGame;
		whereSaveTo.copy(newGame);
		cout << "save point is now at " << save_point << endl
			<< "Please enter the type of cell from left to right and from up to down\n";
		whereSaveTo.get();
		getline(cin, trash);
		whereSaveTo.print();
		cout << "load success!\nPlease enter %row %col to click or \"quit\" to quit\n";
		return;
	}
	else if (command_string.substr(0, 9) == "change to")
	{
		string token;
		istringstream delim(command_string);
		vector<string> word;
		while (getline(delim, token, ' '))
		{
			word.push_back(token);
		}
		if (is_digits(word[2]) && word[3] == "at" && is_digits(word[4]) && is_digits(word[5]))
		{
			change(atoi(word[4].c_str()), atoi(word[5].c_str()), atoi(word[2].c_str()), whereReadFrom, whereSaveTo);
			cout << "success\n";
			return;
		}
	}
	else if (save_point > -1)
	{
		string token;
		istringstream delim(command_string);
		vector<string> word;
		while (getline(delim, token, ' '))
		{
			word.push_back(token);
		}
		if (is_digits(word[0]) && is_digits(word[1]))
		{
			int row = atoi(word[0].c_str());
			int col = atoi(word[1].c_str());
			save_point++;
			whereSaveTo.copy(whereReadFrom);
			if (row < MAX_GRID_HEIGHT && row >= 0 && col < MAX_GRID_WIDTH && col >= 0)
			{
				whereSaveTo.click(row, col, logOn);
				whereSaveTo.print();
				cout << "score this time :" << whereSaveTo.getRoundScore() << endl;
			}
			return;
		}
	}
	cout << "not a command!\n";
}
void Command::search(GameCenter& whereReadFrom, GameCenter& whereSaveTo)
{
	save_point++;
	int maxScore = 0;
	int best_row = 0;
	int best_col = 0;
	for (int row = 0; row < MAX_GRID_HEIGHT; row++)
	{
		for (int col = 0; col < MAX_GRID_WIDTH; col++)
		{
			whereSaveTo.copy(whereReadFrom);
			whereSaveTo.click(row, col, logOn);
			if (whereSaveTo.getRoundScore() > maxScore)
			{
				maxScore = whereSaveTo.getRoundScore();
				best_row = row;
				best_col = col;
			}
		}
	}
	cout << "click on row " << best_row << " col " << best_col << endl;
}

int main()
{
	GameCenter game[MAX_SAVE_NUMBER];
	Command com;
	while (1) com.command(game[(com.save_point - 1) % MAX_SAVE_NUMBER], game[com.save_point % MAX_SAVE_NUMBER], game[(com.save_point + 1) % MAX_SAVE_NUMBER]);
	system("PAUSE");
	return 0;
}