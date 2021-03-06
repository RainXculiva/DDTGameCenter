// DDTGameCenter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <omp.h>
using namespace std;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

const int MAX_SAVE_NUMBER = 30;
const int MAX_GRID_HEIGHT = 10;
const int MAX_GRID_WIDTH = 10;
int start_time = time(0);

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
	void search(GameCenter& whereReadFrom, GameCenter& whereSaveTo, int max_search_round);
	bool undo()
	{
		if (save_point >= 1 && numberOfUndos < MAX_SAVE_NUMBER - 1) { save_point--; numberOfUndos++; return true; }
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
				if (state[row][col] == -3)
				{
					cout << setw(3) << "BOM";
				}
				else if (state[row][col] == -2)
				{
					cout << setw(3) << "STO";
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
					SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);
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
	whereSaveTo.copy(whereCopyFrom);
	whereSaveTo.state[row][col] = bombType;
}
void Command::command(GameCenter& previous_game, GameCenter& whereReadFrom, GameCenter& whereSaveTo)
{
	if (!helped)
	{
		cout << "HELP" << endl
			<< MAX_GRID_HEIGHT << " by " << MAX_GRID_WIDTH << endl
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
			cout << "save point is now at " << save_point << endl;
			whereReadFrom.print();
			cout << endl;
		}
		return;
	}
	else if (command_string.substr(0, 6) == "search")
	{
		string max_search_round;
		istringstream delim(command_string);
		getline(delim, max_search_round, ' ');
		getline(delim, max_search_round, ' ');
		if (is_digits(max_search_round.c_str()) && max_search_round != "")
		{
			//TODO:unexpected input
			search(whereReadFrom, whereSaveTo, atoi(max_search_round.c_str()));
			cout << atoi(max_search_round.c_str()) << "-click search has finished" << endl;
		}
		else
		{
			search(whereReadFrom, whereSaveTo, 1);
			cout << "1-click search has finished" << endl;
		}

		cout << "save point is now at " << save_point << endl;
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
	else if (command_string.substr(0, 4) == "open")
	{
		save_point++;
		string token;
		istringstream delim(command_string);
		string level;
		getline(delim, token, ' ');
		getline(delim, level, ' ');
		stringstream levelFileName;
		levelFileName << "level_" << level << ".txt";
		ifstream defaultLevel(levelFileName.str().c_str());
		while (defaultLevel.fail())
		{
			cout << "\nerror! Please enter the level you want to open.\n";
			cin >> level;
			levelFileName << "level_" << level << ".txt";
			ifstream defaultLevel(levelFileName.str().c_str());
		}
		GameCenter newGame;
		whereSaveTo.copy(newGame);
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				int bombType;
				defaultLevel >> bombType;
				change(row, col, bombType, whereSaveTo, whereSaveTo);
			}
		}
		defaultLevel.close();
		cout << "save point is now at " << save_point << endl;
		whereSaveTo.print();
		cout << "load success!\nPlease enter %row %col to click or \"search\" to search a best solution\n";
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
		cout << "load success!\nPlease enter %row %col to click or \"search\" to search a best solution\n";
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
			save_point++;
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
		if (is_digits(word[0]) && is_digits(word[1]) && word[0] != "" && word[1] != "")
		{
			int row = atoi(word[0].c_str());
			int col = atoi(word[1].c_str());
			save_point++;
			whereSaveTo.copy(whereReadFrom);
			if (row < MAX_GRID_HEIGHT && row >= 0 && col < MAX_GRID_WIDTH && col >= 0)
			{
				whereSaveTo.click(row, col, logOn);
				whereSaveTo.print();
				cout << "save point is now at " << save_point << endl;
				cout << "score this time :" << whereSaveTo.getRoundScore() << endl;
			}
			return;
		}
	}
	cout << "not a command!\n";
}
/*void Command::search(GameCenter& whereReadFrom, GameCenter& whereSaveTo)
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
	whereSaveTo.click(best_row, best_col, logOn);
	whereSaveTo.print();
	cout << "click on row " << best_row << " col " << best_col << endl;
}*/
void Command::search(GameCenter& whereReadFrom, GameCenter& whereSaveTo, int max_search_round)
{
	save_point++;
	int maxScore = 0;
	bool isEnd = 0;
	int round = 0;
	int cumulativeRoundScore = 0;
	vector<int> stop_at_row, stop_at_col, best_row, best_col;
	//find the best position
	for (int i = 0; i < max_search_round; i++)
	{
		stop_at_row.push_back(0);
		stop_at_col.push_back(0);
		best_row.push_back(0);
		best_col.push_back(0);
	}
	int changingRound = 0;
	start_time = time(0);
	for (int times = 0; times < pow(MAX_GRID_HEIGHT*MAX_GRID_WIDTH, max_search_round - 1); times++)
	{
		bool needToBreak = 0;
		//back to original game
		whereSaveTo.copy(whereReadFrom);
		cumulativeRoundScore = 0;
		//turn times to round;
		int tempTimes = times;
		for (int var_round = 0; var_round < max_search_round - 1; var_round++)
		{
			stop_at_col[max_search_round - 2 - var_round] = tempTimes % MAX_GRID_WIDTH;
			tempTimes /= MAX_GRID_WIDTH;
			stop_at_row[max_search_round - 2 - var_round] = tempTimes % MAX_GRID_HEIGHT;
			tempTimes /= MAX_GRID_HEIGHT;
		}
		//run to round max_search_round -1

		for (int var_round = changingRound; var_round < max_search_round - 1; var_round++)
		{
			if (whereSaveTo.getState(stop_at_row[var_round], stop_at_col[var_round]) > 0)
			{
				whereSaveTo.click(stop_at_row[var_round], stop_at_col[var_round], 0);
				cumulativeRoundScore += whereSaveTo.getRoundScore();
			}
			else
			{
				needToBreak = 1;
				break;
			}
		}
		//run the last round

		GameCenter temp;
		temp.copy(whereSaveTo);
		if (needToBreak) continue;

		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				if (whereSaveTo.getState(row, col) == 1)
				{
					//back to original game
					whereSaveTo.copy(temp);
					whereSaveTo.click(row, col, 0);
					if (logOn)
					{
						cout << "thread" << omp_get_thread_num() << endl;
						for (int var_round = 0; var_round < max_search_round - 1; var_round++)
						{
							cout << "NO." << var_round + 1 << " at row " << stop_at_row[var_round] << " col " << stop_at_col[var_round] << endl;
						}
						cout << "NO." << max_search_round << " at row " << row << " col " << col << endl;
					}
					cumulativeRoundScore += whereSaveTo.getRoundScore();
					if (cumulativeRoundScore > maxScore)
					{
						maxScore = cumulativeRoundScore;
						for (int var_round = 0; var_round < max_search_round - 1; var_round++)
						{
							best_col[var_round] = stop_at_col[var_round];
							best_row[var_round] = stop_at_row[var_round];
						}
						best_col[max_search_round - 1] = col;
						best_row[max_search_round - 1] = row;
					}
					cumulativeRoundScore -= whereSaveTo.getRoundScore();
				}
			}
		}

	}
	cout << "\nthe search took " << time(0) - start_time << "s" << endl;
	//click the best position
	whereSaveTo.copy(whereReadFrom);
	//because of progress bar
	cout << endl;
	for (int var_round = 0; var_round < max_search_round; var_round++)
	{
		whereSaveTo.click(best_row[var_round], best_col[var_round], 0);
		whereSaveTo.print();
		cout << "click on row " << best_row[var_round] << " col " << best_col[var_round] << endl;
	}
}

int main()
{
	GameCenter game[MAX_SAVE_NUMBER];
	Command com;
	while (1) com.command(game[(com.save_point - 1) % MAX_SAVE_NUMBER], game[com.save_point % MAX_SAVE_NUMBER], game[(com.save_point + 1) % MAX_SAVE_NUMBER]);
	system("PAUSE");
	return 0;
}