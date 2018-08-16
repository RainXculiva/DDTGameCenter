// DDTGameCenter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;

const int MAX_GRID_HEIGHT = 10;
const int MAX_GRID_WIDTH = 10;
class GameCenter
{
public:
	void initialize()
	{
		//initialize state
		for (int row = 0; row < MAX_GRID_HEIGHT; row++)
			for (int col = 0; col < MAX_GRID_WIDTH; col++)
			{
				state[row][col] = -1;
				score[row][col] = 0;
			}
	}
	int getState(int Row, int Col)
	{
		return state[Row][Col];
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
				if (state[row][col] == 0) totalScore += score[row][col];
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
			cout << endl;
		}
	}
private:
	//numbers mean remaining clicks to bomb -2:stone -1:nothing 0:bomb(score) 1:gold 2:deep blue 3:water blue 4:green 5:gray
	int state[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];
	//record scores from the start to the end
	int score[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];
};

int main()
{
	GameCenter game;
	game.get();
	game.print();
	system("PAUSE");
	return 0;
}