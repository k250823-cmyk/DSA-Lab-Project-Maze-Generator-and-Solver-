#include "Common.h"
#include "Stack.h"
#include "Queue.h"
#include "Maze.h"
#include "MoveList.h"
#include "History.h"
#include "Game.h"

int main(){
    //time(0) is the current time in seconds, srand() uses it as the starting point so rand() gives a different maze every run
    srand((unsigned int)time(0));
    //the GetStdHandle call gives us the console output window, so setColor, gotoxy and showCursor can control it
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    //mode con sets the console window to 100 columns and 56 rows so the hard maze fits on the screen
    system("mode con cols=100 lines=56");
    //sets the text shown in the title bar of the console window
    system("title Maze Quest");

    HistoryList history;
    history.loadFromFile();

    //difficulty settings: easy, medium, hard
    const string levelName[3] = {"Easy", "Medium", "Hard"};
    const int levelCells[3] = {10, 15, 20};
    const int levelLives[3] = {5, 3, 2};

    int choice = 0;
    while (choice != 4) {
        system("cls");
        showCursor(true);
        setColor(14);
        cout<<endl;
        cout<<"   M A Z E   Q U E S T"<<endl;
        setColor(8);
        cout<<"   random maze generator and solver"<<endl;
        cout<<endl;
        setColor(15);
        cout<<"   1. New Game"<<endl;
        cout<<"   2. Auto-Solve"<<endl;
        cout<<"   3. View History"<<endl;
        cout<<"   4. Exit"<<endl;
        cout<<endl;
        setColor(11);
        cout<<"   Enter your choice: ";
        setColor(7);

        //cin>>choice reads a typed number into choice, it fails if the user types something that is not a number
        if (!(cin>>choice)) {
            //removes the error state of cin, otherwise it would refuse all further input
            cin.clear();
            //throws away up to 1000 leftover characters until the end of the line
            cin.ignore(1000, '\n');
            choice = 0;
        }

        if (choice == 1 || choice == 2) {
            //difficulty menu
            int level = 0;
            while (level < 1 || level > 4) {
                system("cls");
                setColor(14);
                cout<<endl;
                cout<<"   SELECT DIFFICULTY"<<endl;
                cout<<endl;
                setColor(10);
                cout<<"   1. Easy     ("<<levelCells[0]<<" x "<<levelCells[0]<<" maze, "<<levelLives[0]<<" lives)"<<endl;
                setColor(14);
                cout<<"   2. Medium   ("<<levelCells[1]<<" x "<<levelCells[1]<<" maze, "<<levelLives[1]<<" lives)"<<endl;
                setColor(12);
                cout<<"   3. Hard     ("<<levelCells[2]<<" x "<<levelCells[2]<<" maze, "<<levelLives[2]<<" lives)"<<endl;
                setColor(15);
                cout<<"   4. Back"<<endl;
                cout<<endl;
                setColor(11);
                cout<<"   Enter your choice: ";
                setColor(7);
                if (!(cin>>level)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    level = 0;
                }
            }

            if (level == 4) {
                continue;
            }

            int i = level - 1;
            Game game;
            game.setup(levelName[i], levelCells[i], levelLives[i]);

            if (choice == 1) {
                int result = game.play();
                if (result != -1) {
                    bool won = (result == 1);
                    int score = 0;
                    if (won) {
                        score = game.calcScore();
                    }
                    history.add(levelName[i], levelCells[i], game.moveCount, game.lives, score, won);
                    history.saveToFile();

                    system("cls");
                    cout<<endl;
                    if (won) {
                        setColor(10);
                        cout<<"   YOU ESCAPED THE MAZE!"<<endl;
                    }
                    else {
                        setColor(12);
                        cout<<"   GAME OVER - you ran out of lives."<<endl;
                    }
                    setColor(7);
                    cout<<endl;
                    cout<<"   Difficulty  : "<<levelName[i]<<endl;
                    cout<<"   Maze size   : "<<levelCells[i]<<" x "<<levelCells[i]<<endl;
                    cout<<"   Moves       : "<<game.moveCount<<endl;
                    cout<<"   Lives left  : "<<game.lives<<endl;
                    cout<<"   Score       : "<<score<<endl;
                    cout<<endl;
                    setColor(11);
                    cout<<"   Your moves:"<<endl;
                    setColor(7);
                    cout<<"   ";
                    game.moves.print();
                    pauseScreen();
                }
            }
            else {
                game.autoSolve();
                pauseScreen();
            }
        }
        else if (choice == 3) {
            system("cls");
            history.print();
            pauseScreen();
        }
        else if (choice != 4) {
            setColor(12);
            cout<<endl;
            cout<<"   Invalid choice, please enter 1 to 4."<<endl;
            pauseScreen();
        }
    }

    system("cls");
    setColor(14);
    cout<<endl;
    cout<<"   Thanks for playing Maze Quest!"<<endl;
    setColor(7);
    cout<<endl;
    return 0;
}