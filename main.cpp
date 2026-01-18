#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#define DEBUG
#define SEMI
#define RANDOM rand() % MAX

using namespace std;



// ========== Constants ==========
// set integer array + RectangleShape array dimensions
const int MAX = 100;
const int GRID_WIDTH = MAX;
const int GRID_HEIGHT = MAX;
const int CELL_SIZE = 20; // each square in the grid will be 10px x 10px

// window settings
const int SCREEN_WIDTH = MAX * CELL_SIZE; // pixels
const int SCREEN_HEIGHT = MAX * CELL_SIZE;
const int FPS = 15;
// ===============================



// ========== Function Prototypes ==========
// essential functions
int count(int twoD[][MAX], int i, int j); // count how many neighbors a cell has
void FillShapes(sf::RectangleShape shapeArray[][GRID_WIDTH], int intArray[][MAX]); // create squares in grid
void ShowShapes(sf::RenderWindow &window, sf::RectangleShape shapeArray[][GRID_WIDTH]); // actually draw the grid in the window
void ProcessEvents(sf::RenderWindow &window, bool &pause, int twoD[][MAX]); // detects key presses, mouse clicks, etc. and responds accordingly
void initialize(int twoD[][MAX]); // initialize all values in the array to zero
void config(int twoD[][MAX]); // generate a random pattern (either to start, or when prompted by user)
void step(int twoD[][MAX]); // figure out how the next frame will look
void copy_array(int dest[][MAX], int src[][MAX]); // copy the source array to the destination array
void wrap(int twoD[][MAX]); // keeps cells visible rather than letting them go off screen
void clear(int twoD[][MAX]); // clear screen
void WriteIntArray(string filename, int intArray[][MAX]); // save current frame to file
void ReadIntArray(string filename, int intArray[][MAX]); // load frame from file
void makeLive(int intArray[][MAX], int j, int i); // toggle live/dead (should rename)
void pixelsToSquares(int &i, int &j); // convert pixel coordinates to square grid coordinates
void writePartialArray(int intArray[][MAX], int a, int b, int c, int d, string filename); // save only a portion of the current frame to file
void loadPartialArray(string filename, int intArray[][MAX]); // load portion of a frame from file

// special shapes
void blinker(int twoD[][MAX]);
void glider(int twoD[][MAX]);

// debugging
void print2D(int twoD[][MAX]); // print the integer array
void menu(); // show options (in command line for now)
void runSFMLTestProgram();
void runSemiUpdatedProgram();
void runMainProgram();

// unused but keeping just in case
int Random(int lo, int hi);
// =========================================



int main()
{
    #ifdef DEBUG
        #ifndef SEMI
            runSFMLTestProgram();
        #else
            runSemiUpdatedProgram();
        #endif
    #else
        runMainProgram();
    #endif

    return 0;
}

int count(int twoD[][MAX], int i, int j){
    // count neighbors of a position
    int neighbors = 0;
    for(int m = i-1; m <= i+1; m++){ // row
        for(int n = j-1; n <= j+1; n++){ // column
            if(twoD[m][n] == 1)
                neighbors++;
        }
    }
    return neighbors;
}
void FillShapes(sf::RectangleShape shapeArray[][GRID_WIDTH], int intArray[][MAX]){
    //create squares
    int row, col;
    for (row=0; row<GRID_HEIGHT; row++){
        for (col=0; col<GRID_WIDTH; col++){
            int vectorY=row*(CELL_SIZE);
            int vectorX=col*(CELL_SIZE);
            shapeArray[row][col].setSize(sf::Vector2f(CELL_SIZE,CELL_SIZE)); // creates a square (ex: 10x10)
            shapeArray[row][col].setPosition(sf::Vector2f(vectorX,vectorY));
            if(intArray[row][col] == 1){
                // shapeArray[row][col].setFillColor(sf::Color(127,255,0)); // green
                shapeArray[row][col].setFillColor(sf::Color(RANDOM, RANDOM, RANDOM));
            }
            else{
                shapeArray[row][col].setFillColor(sf::Color(0,0,0)); // black
            }
            // shapeArray[row][col].setOutlineColor(sf::Color(64, 64, 64));
            // shapeArray[row][col].setOutlineThickness(1);
        }
    }
}
void ShowShapes(sf::RenderWindow& window, sf::RectangleShape shapeArray[][GRID_WIDTH]){
    // draw squares on the window object
    for (int row=0; row<GRID_HEIGHT; row++){
        for (int col=0; col<GRID_WIDTH; col++){
            window.draw(shapeArray[row][col]); // draws the array on the screen
        }
    }

}
void ProcessEvents(sf::RenderWindow &window, bool &pause, int twoD[][MAX]){
    int mouseX, mouseY, mouseA, mouseB;
    string str;
    while (const optional event = window.pollEvent()){
        if (event->is<sf::Event::Closed>()){
            cout << "ProcessEvents -- closing window. Goodbye!" << endl;
            window.close();
        }
        else if(auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
            if (keyPressed->scancode == sf::Keyboard::Scancode::P){
                cout << "ProcessEvents -- the P key was pressed! ";
                pause == true ? cout << "Resuming..." << endl : cout << "Pausing..." << endl;
                pause = !pause;
            }
            else if(keyPressed->scancode == sf::Keyboard::Scancode::R){
                cout << "ProcessEvents -- the R key was pressed! Randomizing..." << endl;
                pause = true;
                config(twoD);
            }
            else if(keyPressed->scancode == sf::Keyboard::Scancode::C){
                cout << "ProcessEvents -- the C key was pressed! Clearing screen..." << endl;
                pause = true;
                clear(twoD);
            }
            else if(keyPressed->scancode == sf::Keyboard::Scancode::S){
                #ifdef DEBUG
                    cout << "ProcessEvents -- the S key was pressed! Saving pattern..." << endl;
                    pause = true;
                    cout << "Enter file name (.txt will be appended for you): ";
                    cin >> str;
                    str += ".txt";
                    WriteIntArray(str, twoD);
                #else
                #endif
            }
            else if(keyPressed->scancode == sf::Keyboard::Scancode::L){
                #ifdef DEBUG
                    cout << "ProcessEvents -- the L key was pressed! Loading pattern..." << endl;
                    pause = true;
                    cout << "Enter the name of the file you'd like to open (.txt will be appended for you): ";
                    cin >> str;
                    str += ".txt";
                    clear(twoD);
                    ReadIntArray(str, twoD);
                #else
                    
                #endif
            }
        }
        else if(auto* keyPressed = event->getIf<sf::Event::MouseButtonPressed>()){
            cout << "ProcessEvents -- mouse button was pressed!" << endl;
            mouseA = sf::Mouse::getPosition(window).x;
            mouseB = sf::Mouse::getPosition(window).y;
            cout << "(" << mouseA << ", " << mouseB << ")" << endl;
        }
        else if(auto* keyPressed = event->getIf<sf::Event::MouseButtonReleased>()){
            cout << "ProcessEvents -- mouse button was released!" << endl;
            if(keyPressed->button == sf::Mouse::Button::Right){
                cout << "the right button was pressed" << endl;
                cout << "mouse x: " << sf::Mouse::getPosition(window).x << endl;
                cout << "mouse y: " << sf::Mouse::getPosition(window).y << endl;
            }
            else if(keyPressed->button == sf::Mouse::Button::Left){
                cout << "left button?" << endl;
                pause = true;
                mouseX = sf::Mouse::getPosition(window).x;
                mouseY = sf::Mouse::getPosition(window).y;
                cout << "[" << mouseX << "][" << mouseY << "] was pressed." << endl;
                makeLive(twoD, mouseX, mouseY);
                // if(mouseA != mouseX && mouseB != mouseY){
                //     cout << ">> ";
                //     cin >> str;
                //     str += ".txt";
                //     writePartialArray(twoD, mouseA, mouseB, mouseX, mouseY, str);
                // }
            }
        }
        else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()){
            std::cout << "new mouse x: " << mouseMoved->position.x << std::endl;
            std::cout << "new mouse y: " << mouseMoved->position.y << std::endl;
        }
    }

}
void initialize(int twoD[][MAX]){
    // initialize all values to zero
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            twoD[i][j] = 0;
        }
    }
}
void config(int twoD[][MAX]){
    // fill in initial configuration
    // can also be used when [R]andom selected
    clear(twoD);
    int a, b;
    for(int z = 0; z < 5000; z++){
        a = rand() % MAX;
        b = rand() % MAX;
        twoD[a][b] = 1;
    }
}
void step(int twoD[][MAX]){
    // goes through each position and calls count
    // determines who lives or dies or stays the same
    wrap(twoD);
    int neighbors;
    int temp[MAX][MAX];
    for(int i = 1; i < MAX-1; i++){
        for(int j = 1; j < MAX-1; j++){
            neighbors = count(twoD, i, j);
            if(twoD[i][j] == 1){
                neighbors -= 1;
                if(neighbors < 2 || neighbors > 3)
                    temp[i][j] = 0;
                if(neighbors == 2 || neighbors == 3)
                    temp[i][j] = 1;
            }
            if(twoD[i][j] == 0){
                if(neighbors == 3)
                    temp[i][j] = 1;
                if(neighbors <= 2 || neighbors > 3)
                    temp[i][j] = 0;
            }
        }
    }

    // then once everything is counted, copy back over to the original array
    copy_array(twoD, temp);

}
void copy_array(int dest[][MAX], int src[][MAX]){
    // copy from source to destination
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            dest[i][j] = src[i][j];
        }
    }
}

void wrap(int twoD[][MAX]){
    // connect borders
    for(int i = 0; i < MAX; i++){
        twoD[i][0] = twoD[i][MAX-2];
        twoD[i][MAX-1] = twoD[i][1];
    }
    for(int j = 0; j < MAX; j++){
        twoD[0][j] = twoD[MAX-2][j];
        twoD[MAX-1][j] = twoD[1][j];
    }
}
void clear(int twoD[][MAX]){
    // clear array
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            twoD[i][j] = 0;
        }
    }
    #ifdef DEBUG
        print2D(twoD);
    #endif
}
void WriteIntArray(string filename, int intArray[][MAX]){
    // writes whole screen to a file
    ofstream outFile; // 1. declare file object
    outFile.open(filename); // 2. open
    if (outFile.fail()) // 3. check if it opens
    {
        cout<<endl<<"***** could not open file for output. ****"<<endl;
        exit(0);
    }

    for (int i=0; i<MAX; i++){ // 4. write to file
        for(int j = 0; j < MAX; j++){
            outFile<<intArray[i][j]<<" ";
        }
        cout << endl;
    }
    outFile.close(); // 5. close

}
void ReadIntArray(string filename, int intArray[][MAX]){
    // reads a file (either full screen or selection)
    ifstream inFile;
    inFile.open(filename);
    if (inFile.fail())
    {
        cout<<endl<<"***** could not open file for input. ****"<<endl;
        exit(0);
    }
    for (int i=0; i<MAX; i++)
    {
        for(int j = 0; j < MAX; j++){
            inFile>>intArray[i][j];
        }
    }
    inFile.close();

}
void makeLive(int intArray[][MAX], int j, int i){
    // toggle live/dead
    pixelsToSquares(i, j); // convert to squares
    if(intArray[i][j] == 1){
        intArray[i][j] = 0;
    }
    else{
        intArray[i][j] = 1;
    }
    #ifdef DEBUG
        cout << "makeLive -- the value at row " << j << " and col " << i << " is now set to " << intArray[j][i] << endl;
    #endif
}
void pixelsToSquares(int &i, int &j){
    // convert pixel coordinates to square/grid coordinates
    #ifdef DEBUG
        cout << "pixelsToSquares -- converting pixels i = " << i << " and j = " << j << endl;
    #endif
    i /= CELL_SIZE;
    j /= CELL_SIZE;
    #ifdef DEBUG
        cout << "pixelsToSquares -- now squares i = " << i << " and j = " << j << endl;
    #endif
}
void writePartialArray(int intArray[][MAX], int a, int b,
                       int c, int d, string filename){
    // save a portion of the screen to a file
    ofstream outFile; // 1. declare file object
    outFile.open(filename); // 2. open
    if (outFile.fail()) // 3. check if it opens
    {
        cout<<endl<<"***** could not open file for output. ****"<<endl;
        exit(0);
    }

    for (int i=a; i<c; i++){ // 4. write to file
        for(int j = b; j < d; j++){
            outFile<<intArray[i][j]<<" ";
        }
        cout << endl;
    }
    outFile.close(); // 5. close
}
void loadPartialArray(string filename, int intArray[][MAX]){
    // load a portion of the screen from a file
}
void blinker(int twoD[][MAX]){
    // blinker test pattern

    // vertical

//    twoD[4][1] = 1;
//    twoD[5][1] = 1;
//    twoD[6][1] = 1;

    // horizontal

    twoD[1][4] = 1;
    twoD[1][5] = 1;
    twoD[1][6] = 1;
}
void glider(int twoD[][MAX]){
    // glider test pattern

    // version 2

    twoD[6][3] = 1;
    twoD[7][4] = 1;
    twoD[7][5] = 1;
    twoD[6][5] = 1;
    twoD[5][5] = 1;

    // version 1

//    twoD[1][2] = 1;
//    twoD[2][3] = 1;
//    twoD[3][1] = 1;
//    twoD[3][2] = 1;
//    twoD[3][3] = 1;
}
void print2D(int twoD[][MAX]){
    // print 2D array
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            if(twoD[i][j] == 0)
                // cout << setw(4) << "x";
                cout << "x";
            if(twoD[i][j] == 1)
                // cout << setw(4) << "o";
                cout << "o";
        }
        cout << endl;
    }
}
void menu(){
    cout << endl
         << "[S]AVE" << setw(12) << "[L]OAD" << setw(12) << "[C]LEAR"<< setw(12) << "[R]ANDOM" << endl
         << "S[T]EP" << setw(12) << "E[X]IT" << setw(12) << "[?]MENU" << endl;
}
void runSFMLTestProgram(){
    sf::RenderWindow window(sf::VideoMode({200, 200}), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        while (const optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}

void runSemiUpdatedProgram(){
    // This is a temporary function for pinpointing the cause of the original program's crashing at runtime.
    sf::RenderWindow window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Conway's Game of Life");
    window.setFramerateLimit(FPS);
    sf::RectangleShape shapeArray[GRID_HEIGHT][GRID_WIDTH];
    window.setVerticalSyncEnabled(true);
    int world[MAX][MAX];
    bool pause = false;
    initialize(world);
    config(world);
    while (window.isOpen()){
        ProcessEvents(window, pause, world);
        window.clear();
        if(!pause) step(world);
        FillShapes(shapeArray, world);
        ShowShapes(window, shapeArray);
        window.display();
    }
}

void runMainProgram(){
    sf::RenderWindow window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Game of Life");
    window.setFramerateLimit(15);
    sf::RectangleShape shapeArray[GRID_HEIGHT][GRID_WIDTH];
    window.setVerticalSyncEnabled(true);
    int world[MAX][MAX];
    bool pause = false;
    initialize(world);
    config(world);
    while (window.isOpen()){
        ProcessEvents(window, pause, world);
        window.clear();
        if(!pause){
            step(world);
        }
        FillShapes(shapeArray, world);
        ShowShapes(window, shapeArray);
        window.display();
    }
}
int Random(int lo, int hi){
    int r = rand()%(hi+1)+lo+1;
    return r;
}
