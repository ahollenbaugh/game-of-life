#include <iomanip>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fstream>
#include <string>

const int SCREEN_WIDTH = 1600; // pixels
const int SCREEN_HEIGHT = 1200;
const int GRID_WIDTH = 150; // squares (should be same as max)
const int GRID_HEIGHT = 150;
const int CELL_SIZE = 10; // 10x10 pixels for each square
const int MAX = 150; // dimensions of integer array

using namespace std;

void FillShapes(sf::RectangleShape shapeArray[][GRID_WIDTH], int intArray[][MAX]);
void ShowShapes(sf::RenderWindow &window, sf::RectangleShape shapeArray[][GRID_WIDTH]);
void ProcessEvents(sf::RenderWindow &window, bool &pause, int twoD[][MAX]);
int Random(int lo, int hi);
void initialize(int twoD[][MAX]);
void config(int twoD[][MAX]);
void step(int twoD[][MAX]);
int count(int twoD[][MAX], int i, int j);
void copy(int dest[][MAX], int src[][MAX]);
void print2D(int twoD[][MAX]);
void wrap(int twoD[][MAX]);
void blinker(int twoD[][MAX]);
void glider(int twoD[][MAX]);
void clear(int twoD[][MAX]);
void menu();
void WriteIntArray(std::string filename, int intArray[][MAX]);
void ReadIntArray(std::string filename, int intArray[][MAX]);
void makeLive(int intArray[][MAX], int i, int j);
void pixelsToSquares(int &i, int &j);
void writePartialArray(int intArray[][MAX], int a, int b,
                       int c, int d, std::string filename);
void loadPartialArray(std::string filename, int intArray[][MAX]);

int main()
{
    //----------S E T U P ------------------------------:

    //declare a window object:
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Game of Life");
    //
    //VideoMode class has functions to detect screen size etc.
    //RenderWindow constructor has a third argumnet to set style
    //of the window: resize, fullscreen etc.
    //
    //or...
    // you could do this:
    //sf::RenderWindow window;
    //window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML window!");
    //

    window.setFramerateLimit(15);

    //this is where we keep all the shapes.
    sf::RectangleShape shapeArray[GRID_HEIGHT][GRID_WIDTH];


    window.setVerticalSyncEnabled(true); // call it once, after creating the window
    //Application runs at the same freq as monitor

    //. . . . . . . SHAPES ............
    //this is how you would declare and manipulate shapes:
    //sf::CircleShape shape(250.f);
    //shape.setFillColor(sf::Color::Green);
    //// set a 10-pixel wide orange outline
    //shape.setOutlineThickness(1);
    //shape.setOutlineColor(sf::Color(250, 150, 100));


    //// define a 120x50 rectangle
    //sf::RectangleShape rectangle(sf::Vector2f(120, 50));
    //// change the size to 100x100
    ////rectangle.setSize(sf::Vector2f(10, 10));
    //. . . . . . . . . . . . . . . . . . . . . . . . . . .

    // declare int array:
    int world[MAX][MAX];

    // pause:
    bool pause = false;

    initialize(world);

    config(world);

    // run the program as long as the window is open
    while (window.isOpen())
    {
        ProcessEvents(window, pause, world); //Process mouse and keyboard events

        window.clear(); //necessary: get read for the next frame

        if(!pause){
            step(world);
        }

        FillShapes(shapeArray, world); // determine size/position/color of the shapes

        ShowShapes(window, shapeArray); // draw the shapes on the window object

        window.display(); //display the window and all its shapes
    }

    return 0;
}

void ProcessEvents(sf::RenderWindow &window, bool &pause, int twoD[][MAX]){
    // check all the window's events that were triggered since the last iteration of the loop

    sf::Event event;
    int mouseX, mouseY, mouseA, mouseB;
    string str;

    //go through all the pending events: keyboard, mouse, close, resize, etc.
    //pollEvent and waitEvent are the only two functions that can fill event
    while (window.pollEvent(event))//or waitEvent
    {
        // check the type of the event...
        switch (event.type)
        {
        // window closed
        // "close requested" event: we close the window
        case sf::Event::Closed:
            window.close();
            break;
            // key pressed
        case sf::Event::KeyPressed:
            switch(event.key.code){
            case sf::Keyboard::P:
                // [P]ause (toggle)
                pause = !pause;
                break;
            case sf::Keyboard::R:
                // generate [R]andom pattern
                pause = true;
                config(twoD);
                break;
            case sf::Keyboard::C:
                // [C]lear screen
                pause = true;
                clear(twoD);
                break;
            case sf::Keyboard::S:
                // [S]ave screenshot
                pause = true;
                std::cout << ">> ";
                std::cin >> str;
                str += ".txt";
                WriteIntArray(str, twoD);
                break;
            case sf::Keyboard::L:
                // [L]oad screenshot or selection
                pause = true;
                clear(twoD);
                std::cout << ">> ";
                std::cin >> str;
                str += ".txt";
                ReadIntArray(str, twoD);
                break;
            }
            break;
        case sf::Event::MouseButtonPressed:
            // upper left coordinates stored here
            mouseA = event.mouseButton.x;
            mouseB = event.mouseButton.y;
            break;
        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Right)
            {
                // right mouse button
                std::cout << "the right button was pressed" << std::endl;
                std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                std::cout << "mouse y: " << event.mouseButton.y << std::endl;
            }
            else{
                // left mouse button
                std::cout<<"left button?"<<std::endl;

                // bottom right coordinates stored here
                mouseX = event.mouseButton.x;
                mouseY = event.mouseButton.y;
                std::cout << "[" << mouseX << "][" << mouseY
                          << "] was pressed." << std::endl;

                // toggle live/dead cells
                makeLive(twoD, mouseX, mouseY);

                if(mouseA != mouseX && mouseB != mouseY){

                    // if the coordinates of the mouse press
                    // are different from the mouse release
                    // coordinates, that means the user
                    // has clicked and dragged from one
                    // part of the screen to another
                    // to save a portion of the screen

                    std::cout << ">> ";
                    std::cin >> str;
                    str += ".txt";
                    writePartialArray(twoD, mouseA, mouseB,
                                      mouseX, mouseY, str);
                }
            }
            break;
        default:
            break;
        }
    }

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
                shapeArray[row][col].setFillColor(sf::Color::Color(127,255,0)); // green
            }
            else{
                shapeArray[row][col].setFillColor(sf::Color::Color(0,0,0)); // black
            }
        }
    }

    // want cell color to change the longer it stays alive

}
void ShowShapes(sf::RenderWindow& window, sf::RectangleShape shapeArray[][GRID_WIDTH]){
    // draw squares on the window object
    for (int row=0; row<GRID_HEIGHT; row++){
        for (int col=0; col<GRID_WIDTH; col++){
            window.draw(shapeArray[row][col]); // draws the array on the screen
        }
    }

}
int Random(int lo, int hi){
    int r = rand()%(hi+1)+lo+1;
    return r;
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

    for(int i = 1; i < MAX-1; i++){
        for(int j = 1; j < MAX-1; j++){
            twoD[i][j] = temp[i][j];
        }
    }

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
void copy(int dest[][MAX], int src[][MAX]){
    // copy from source to destination
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            dest[i][j] = src[i][j];
        }
    }
}
void print2D(int twoD[][MAX]){
    // print 2D array
    for(int i = 1; i < MAX-1; i++){
        for(int j = 1; j < MAX-1; j++){
            if(twoD[i][j] == 0)
                cout << setw(4) << " ";
            if(twoD[i][j] == 1)
                cout << setw(4) << "*";
        }
        cout << endl;
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
void clear(int twoD[][MAX]){
    // clear array
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            twoD[i][j] = 0;
        }
    }
}
void menu(){
    cout << endl
         << "[S]AVE" << setw(12) << "[L]OAD" << setw(12) << "[C]LEAR"<< setw(12) << "[R]ANDOM" << endl
         << "S[T]EP" << setw(12) << "E[X]IT" << setw(12) << "[?]MENU" << endl;
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
void makeLive(int intArray[][MAX], int i, int j){
    // toggle live/dead
    pixelsToSquares(i, j); // convert to squares
    if(intArray[j][i] == 1){
        intArray[j][i] = 0;
    }
    else{
        intArray[j][i] = 1;
    }
}
void pixelsToSquares(int &i, int &j){
    // convert pixel coordinates to square/grid coordinates
    i /= CELL_SIZE;
    j /= CELL_SIZE;
}
void writePartialArray(int intArray[][MAX], int a, int b,
                       int c, int d, std::string filename){
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
void loadPartialArray(std::string filename, int intArray[][MAX]){
    // load a portion of the screen from a file
}
