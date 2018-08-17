// DDTGameCenter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;

const int MAX_GRID_HEIGHT = 10;
const int MAX_GRID_WIDTH = 10;
class GameCenter
{
public:
	GameCenter()
	{
		//initialize state
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
	void command()
	{
		string command_string;
	}
	void click(int Row, int Col)
	{
		if (state[Row][Col] > 0)
			state[Row][Col] --;
		//fire bullets
		if (state[Row][Col] == 0)
		{
			for (int direction = 0; direction < 4; direction++)
				bullet[usingCopy][Row][Col][direction] += 1;
		}


		while (BulletsExist(usingCopy))
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
			this->print();
			cout << endl;
		}
		this->print();
		
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
		cout << "score this time :" << this->getRoundScore() << endl;
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
		int totalScore = 0;
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				if (state[row][col] == 0)
				{
					totalScore += score[row][col];
				}
			}
		}
		return totalScore;
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
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
				cin >> state[row][col];
	}
	void print()
	{
		//negative: bomb positive: score
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
		{
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
				else
				{
					cout << "-" << setw(2) << state[row][col];
				}

				cout << "|";
			}
			//bullet
			/*for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				if (bullet[usingCopy][row][col][DOWN] == )
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
				else
				{
					cout << "-" << setw(2) << state[row][col];
				}

				cout << "|";
			}*/
			cout << endl;
		}
	}
	bool BulletsExist(int copy)
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
private:
	//numbers mean remaining clicks to bomb -3:will fire in next frame-2:stone -1:nothing 0:bomb(score) 1:gold 2:deep blue 3:water blue 4:green 5:gray
	int state[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];
	//record scores from the start to the end
	int score[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];
	//record the direction and the position of bullets and it has two copies to swap;
	int bullet[2][MAX_GRID_HEIGHT][MAX_GRID_WIDTH][4];
	int usingCopy;
protected:
	//direction
	const int LEFT	=	 2;
	const int RIGHT	=	 1;
	const int UP	=	 3;
	const int DOWN	=	 0;
};

int main()
{
	GameCenter game;
	game.get();
	game.print();
	cout << endl;
	int row;
	int col;
	while (cin >> row >> col)
	{
		game.click(row, col);
	}


	system("PAUSE");
	return 0;
}