/*
* Projet Tetris 
* Make a tetris great again 
* M1 - IROBOT
* LABGAA Nazim
*/

#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <string>

const int COLUMNS = 10;
const int ROWS = 30;
const int CELL_SIZE = 20;
float moveSpeed = 300.0f;
int score = 0;
bool isGameOver = false;
bool isInMenu = true;

const int WINDOW_WIDTH = COLUMNS * CELL_SIZE;
const int WINDOW_HEIGHT = ROWS * CELL_SIZE;

const int WINDOW_W = WINDOW_WIDTH + 100;
const int WINDOW_H = WINDOW_HEIGHT;

bool grid[ROWS][COLUMNS] = {false};

void reshape (int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_W, WINDOW_H, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

struct Block
{
    int x;
    int y;
};

struct BlockTetris
{
    std::vector<Block> blocks;
    float color[3];
    int type;
};

BlockTetris tetrisTypes[] =
{
    {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {1.0f, 1.0f, 0.0f}, 1},
    {{{0, 0}, {0, 1}, {0, 2}, {0, 3} }, {0.0f, 1.0f, 1.0f}, 2},
    {{{0, 0}, {0, 1}, {0, 2}, {1, 2} }, {1.0f, 0.5f, 0.0f}, 3},
    {{{1, 0}, {1, 1}, {1, 2}, {0, 2} }, {1.0f, 0.0f, 1.0f}, 4},
    {{{0, 0}, {1, 0}, {1, 1}, {2, 1} }, {1.0f, 0.0f, 0.0f}, 5},
    {{{1, 0}, {2, 0}, {0, 1}, {1, 1} }, {0.0f, 1.0f, 0.0f}, 6},
    {{{0, 1}, {2, 1}, {1, 0}, {2, 1} }, {0.5f, 0.0f, 0.5f}, 7}
};

BlockTetris currentTetris;
BlockTetris nextTetris;
std::vector<Block> blocks;

void setBlockColor(float color[3])
{
    glColor3f(color[0], color[1], color[2]);
}

void draw()
{
    for(int y = 0; y < ROWS; ++y)
    {
        for(int x = 0; x < COLUMNS; ++x)
        {
            if(grid[y][x])
            {
                glColor3f(1.0f, 1.0f, 1.0f);
                glRecti(x * CELL_SIZE, y * CELL_SIZE, (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
            }
        }
    }

    for(const Block& block: blocks)
    {
        setBlockColor(currentTetris.color);
        glRecti(block.x * CELL_SIZE, block.y * CELL_SIZE, (block.x + 1) * CELL_SIZE, (block.y + 1) * CELL_SIZE);
    }
}

void drawNextBlock()
{
    int startX = COLUMNS + 2;
    int startY = 2;

    for(const Block& block: nextTetris.blocks)
    {
        int newX = startX + block.x;
        int newY = startY + block.y;

        glColor3f(nextTetris.color[0], nextTetris.color[1], nextTetris.color[2]);
        glRecti(newX * CELL_SIZE, newY * CELL_SIZE, (newX + 1) * CELL_SIZE, (newY + 1) * CELL_SIZE);
    }
}

void drawScore()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(WINDOW_WIDTH + 1, 20);

    std::string scoreText = "Score: " + std::to_string(score);

    for(char c: scoreText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawGridLines()
{
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    // Lignes verticales
    for(int x = 0; x <= COLUMNS; ++x)
    {
        glVertex2i(x * CELL_SIZE, 0);
        glVertex2i(x * CELL_SIZE, WINDOW_HEIGHT);
    }
    // Lignes horizontales
    for(int y = 0; y <= ROWS; ++y)
    {
        glVertex2i(0, y * CELL_SIZE);
        glVertex2i(WINDOW_WIDTH, y * CELL_SIZE);
    }

    glEnd();
}

void drawGameOver()
{
    int rectX = WINDOW_WIDTH / 2 - 60;
    int rectY = WINDOW_HEIGHT / 2 - 15;
    int rectWidth = 120;
    int rectHeight = 30;

    glColor3f(1.0f, 0.0f, 0.0f);

    std::string gameOverText = "Try to be great again ;)";

    glRasterPos2i(rectX + 20, rectY);
    for(char c: gameOverText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glColor3f(1.0f, 1.0f, 1.0f);

    std::string scoreText = "Mon score : " + std::to_string(score);

    glRasterPos2i(rectX + 50, rectY + 50);
    for(char c: scoreText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    std::string restartText = "Appuyer 'R' to Restart";

    glRasterPos2i(rectX + 20, rectY + 90);
    for(char c: restartText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    std::string menuText = "Appuyer 'M' to main Menu";

    glRasterPos2i(rectX, rectY + 120);
    for(char c: menuText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    std::string quitGameOverText = "Appuyer 'Q' to Quit";

    glRasterPos2i(rectX + 30, rectY + 150);
    for(char c: quitGameOverText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

bool checkGameOver()
{
    for(const Block& block: blocks)
    {
        if(grid[block.y][block.x])
        {
            return true;
        }
    }
    return false;
}

void generateTetris()
{
    currentTetris = nextTetris;
    blocks = currentTetris.blocks;

    int xOffset = COLUMNS / 2.5;

    for(Block& block: blocks)
    {
        block.x += xOffset;
    }

    if(checkGameOver())
    {
        isGameOver = true;
        drawGameOver();
        return;
    }

    nextTetris = tetrisTypes[rand() % 7];
}

bool moveDown()
{
    for(Block block: blocks)
    {
        if(block.y + 1 >= ROWS || grid[block.y + 1][block.x])
        {
            return false;
        }
    }
    return true;
}

bool moveLeft()
{
    for(Block block: blocks)
    {
        if(block.x - 1 < 0 || grid[block.y][block.x - 1])
        {
            return false;
        }
    }
    return true;
}

bool moveRight()
{
    for(Block block: blocks)
    {
        if(block.x + 1 >= COLUMNS || grid[block.y][block.x + 1])
        {
            return false;
        }
    }
    return true;
}

void rotateLBlock()
{
    int centerX = blocks[1].x;
    int centerY = blocks[1].y;

    std::vector<Block> rotatedBlocks;

    for(const Block& block: blocks)
    {
        int newX = centerX - (block.y - centerY);
        int newY = centerY + (block.x - centerX);

        if(newX < 0 || newX >= COLUMNS || newY < 0 || newY >= ROWS || grid[newY][newX])
        {
            return;
        }
        rotatedBlocks.push_back({newX, newY});
    }
    blocks = rotatedBlocks;
}

void rotateSBlock()
{
    int centerX = blocks[1].x;
    int centerY = blocks[1].y;

    std::vector<Block> rotateBlocks;

    bool isHorizontal = blocks[0].x == blocks[1].x;

    for(const Block& block: blocks)
    {
        int newX;
        int newY;

        if(isHorizontal)
        {
            newX = centerX + (block.y - centerY);
            newY = centerY - (block.x - centerX);
        }
        else
        {
            newX = centerX - (block.y - centerY);
            newY = centerY + (block.x - centerX);
        }

        if(newX < 0 || newX >= COLUMNS || newY < 0 || newY >= ROWS || grid[newY][newX])
        {
            return;
        }
        rotateBlocks.push_back({newX, newY});
    }
    blocks = rotateBlocks;
}

void rotateIBlock()
{
    int centerX = blocks[1].x;
    int centerY = blocks[1].y;

    std::vector<Block> rotateBlocks;

    bool isVertical = blocks[0].x == blocks[1].x;

    for(int i = 0; i < 4; ++i)
    {
        int newX = centerX;
        int newY = centerY;

        if(isVertical)
        {
            newX = centerX + (i - 1);
        }
        else
        {
            newY = centerY + (i - 1);
        }

        if(newX < 0 || newX >= COLUMNS || newY < 0 || newY >= ROWS || grid[newY][newX])
        {
            return;
        }
        rotateBlocks.push_back({newX, newY});
    }
    blocks = rotateBlocks;
}

void rotateBlocks()
{
    if(currentTetris.type == 1)
    {
        return;
    }

    if(currentTetris.type == 2)
    {
        rotateIBlock();
        return;
    }

    if(currentTetris.type == 3 || currentTetris.type == 4)
    {
        rotateLBlock();
        return;
    }

    if(currentTetris.type == 5 || currentTetris.type == 6)
    {
        rotateSBlock();
        return;
    }

    int centerX = blocks[0].x;
    int centerY = blocks[0].y;

    std::vector<Block> rotateTetris;

    for(const Block& block: blocks)
    {
        int newX = centerX - (block.y - centerY);
        int newY = centerY + (block.x - centerX);

        if(newX < 0 || newX >= COLUMNS || newY < 0 || newY >= ROWS || grid[newY][newX])
        {
            return;
        }
        rotateTetris.push_back({newX, newY});
    }
    blocks = rotateTetris;
}

void fixBlock()
{
    for(const Block& block: blocks)
    {
        grid[block.y][block.x] = true;
    }
}

void clearLines()
{
    for(int y = 0; y < ROWS; ++y)
    {
        bool lineFilled = true;

        for(int x = 0; x < COLUMNS; ++x)
        {
            if(!grid[y][x])
            {
                lineFilled = false;
                break;
            }
        }

        if(lineFilled)
        {
            for(int j = y; j > 0; --j)
            {
                for(int x = 0; x < COLUMNS; ++x)
                {
                    grid[j][x] = grid[j - 1][x];
                }
            }

            for(int x = 0; x < COLUMNS; ++x)
            {
                grid[0][x] = false;
            }

            score += 100;
        }
    }
}

void resetGame()
{
    score = 0;
    isGameOver = false;
    std::fill(&grid[0][0], &grid[0][0] + sizeof(grid), false);
    nextTetris = tetrisTypes[rand() % 7];
    generateTetris();
    glutPostRedisplay();
}

void displayMenu()
{
    glClear(GL_COLOR_BUFFER_BIT);

    int rectX = WINDOW_WIDTH / 2 - 60;
    int rectY = WINDOW_HEIGHT / 2 - 30;
    int rectWidth = 120;
    int rectHeight = 60;

    glColor3f(1.0f, 1.0f, 1.0f);

    std::string titleProjectText = "Projet Tetris";

    glRasterPos2i(rectX + 60, rectY - 120);
    for(char c: titleProjectText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    std::string titleMasterText = "M1 - IROBOT";

    glRasterPos2i(rectX + 50, rectY - 80);
    for(char c: titleMasterText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    std::string titleText = "MAKE TETRIS GREAT AGAIN";

    glRasterPos2i(rectX - 10, rectY - 50);
    for(char c: titleText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glRasterPos2i(rectX + 10, rectY + 20);

    std::string menuText = "Appuyer sur 'S' to Start";
    for(char c: menuText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glRasterPos2i(rectX + 10, rectY + 70);
    std::string quitText = "Appuyer sur 'Q' to Quit";
    for(char c: quitText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glRasterPos2i(rectX, rectY + 140);
    std::string real = "R�aliser par :";
    for(char c: real)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glColor3f(0.0f, 1.0f, 0.0f);

    glRasterPos2i(rectX + 30, rectY + 180);
    std::string name = "LABGAA Nazim";
    for(char c: name)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if(isInMenu)
    {
        displayMenu();
    }
    else if(isGameOver)
    {
        drawGameOver();
    }
    else
    {
        for(const Block& block: blocks)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            glRecti(block.x * CELL_SIZE, block.y * CELL_SIZE, (block.x + 1) * CELL_SIZE, (block.y + 1) * CELL_SIZE);
        }

        draw();
        drawNextBlock();
        drawScore();
        drawGridLines();
    }

    glutSwapBuffers();
}

void update(int value)
{
    if(isGameOver)
    {
        glutTimerFunc(16, update, 0);
        return;
    }

    static float accumulatedTime = 0.0f;
    accumulatedTime += 0.016f;

    if(accumulatedTime >= moveSpeed / 1000.0f)
    {
        if(moveDown())
        {
            for(Block& block: blocks)
            {
                block.y++;
            }
        }
        else
        {
            fixBlock();
            clearLines();
            generateTetris();
        }
        accumulatedTime = 0.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void handleKeyPress(unsigned char key, int x, int y)
{
    if(isInMenu || isGameOver)
    {
        if(key == 's' || key == 'r')
        {
            isInMenu = false;
            resetGame();
        }
        else if(key == 'm')
        {
            isInMenu = true;
            isGameOver = false;

            displayMenu();
        }
        else if(key == 'q')
        {
            exit(0);
        }
        return;
    }

    if(key == 'd' && moveRight())
    {
        for(Block& block: blocks)
        {
            block.x++;
        }
    }
    else if(key == 'q' && moveLeft())
    {
        for(Block& block: blocks)
        {
            block.x--;
        }
    }
    else if(key == 's' && moveDown())
    {
        for(Block& block: blocks)
        {
            block.y++;
        }
    }
    else if(key == 32)
    {
        rotateBlocks();
    }
}

void handleSpecialKeyPress(int key, int x, int y)
{
    if(key == GLUT_KEY_RIGHT && moveRight())
    {
        for(Block& block: blocks)
        {
            block.x++;
        }
    }
    else if(key == GLUT_KEY_LEFT && moveLeft())
    {
        for(Block& block: blocks)
        {
            block.x--;
        }
    }
    else if(key == GLUT_KEY_DOWN && moveDown())
    {
        for(Block& block: blocks)
        {
            block.y++;
        }
    }
    else if(key == GLUT_KEY_UP || key == 32)
    {
        rotateBlocks();
    }

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    glutInitWindowPosition(640, 100);
    glutCreateWindow("Make Tetris Great Again");

    nextTetris = tetrisTypes[rand() % 7];

    generateTetris();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);
    glutKeyboardFunc(handleKeyPress);
    glutSpecialFunc(handleSpecialKeyPress); //I have some bug with this keys check out later
    glutMainLoop();

    return 0;
}
