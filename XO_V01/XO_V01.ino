
#include <UTFT.h>
#include <UTouch.h>

// Initialize display
// ------------------
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41

UTFT    myGLCD(ITDB32S,38,39,40,41);

// Initialize touchscreen
// ----------------------
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino Mega/Due shield            :  6, 5, 4, 3, 2

UTouch  myTouch( 6, 5, 4, 3, 2);


extern uint8_t BigFont[]; // font type
int width, ratio, XI, YI, currCol, currRow; 
byte matrix[3][4];
boolean player, start, singlePlayer, twoPlayer, finish;

/*************************
**   GUI functions   **
*************************/

/*draw first scene to select player mode*/
void drawButtons(){
  setButtonStyle(10, 130, 150, 180);
  myGLCD.print("1Player", 20, 147);
  setButtonStyle (160, 130, 300, 180);
  myGLCD.print("2Players", 170, 147);
}

/* set style of Button*/
void setButtonStyle(int x1, int y1, int x2, int y2){
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (x1, y1, x2, y2);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  myGLCD.setBackColor (0, 0, 0);
}

/* Draw a red frame while a button is touched */
void waitForIt(int x1, int y1, int x2, int y2){
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

/* indicate which mode is selected then swtich to game*/
void selectPlayerMode(int x, int y){
  if ((y>=130) && (y<=180)) { // buttom row    
    if ((x>=10) && (x<=150)) {  // Button: 1Player      
      waitForIt(10, 130, 150, 180);
      singlePlayer = true;
      changeScn();
     }
     if ((x>=160) && (x<=300)) { // Button: 2Players        
       waitForIt(160, 130, 300, 180);
       twoPlayer = true; 
       changeScn();         
      }
   }
}

/* remove first scene and switch to game scene*/
void changeScn(){
  start = false;
  myGLCD.clrScr();
  drawGrid();
}

/* draw 3*3 empty matrix "second scene" */
void drawGrid (){
  myGLCD.drawLine(XI, ratio + YI, XI + width, ratio + YI);
  myGLCD.drawLine(XI, 2 * ratio + YI, XI + width, 2 * ratio + YI);
  myGLCD.drawLine(XI + ratio, YI, XI + ratio, YI+width);
  myGLCD.drawLine(2 * ratio + XI, YI, 2 * ratio + XI, YI+width);

}

/* read index that was pressed in grid 
 * then access matrix
 */
void setMatrixSlot(int x, int y){
  currCol = getMatrixCol(x);
  currRow = getMatrixRow(y);
  addToMatrix(currRow, currCol);
}

/* add x or o (1 or 2) to matrix */
void addToMatrix(int row, int col){
  if(!matrix[row][col]){
    matrix[row][col] = player + 1;
    drawMatrixSympols(row,col);
  }
}

/* draw x or o , then change player */
void drawMatrixSympols(int row, int col){
  if(player){
    myGLCD.setColor(255, 0, 0);
    myGLCD.print("X", col*ratio + XI + 25, row*ratio + YI + 25);
  }
  else {
    myGLCD.setColor(0, 0, 255);
    myGLCD.print("O", col*ratio + XI + 25, row*ratio + YI + 25);
  }
   player = !player;
}

/* map point in x-y plane to col in matrix*/
int getMatrixCol(int x){
if ( x < (ratio + XI) && x > 0)
  return 0;
if( x > (2*ratio + XI))
  return 2;
  return 1;

}
/* map point in x-y plane to row in matrix*/
int getMatrixRow(int y){
if ( y < (ratio + YI) && y > 0)
  return 0;
if( y > (2*ratio + YI))
  return 2;
  return 1;
}

/* game finished */
void gameOver(int winner){
  delay(200);
  myGLCD.clrScr();
  setButtonStyle(130,40,200,40);
  myGLCD.print("Reset", 130, 40);
  myGLCD.setColor(0, 255, 0);
  myGLCD.print("Game Over", CENTER, 80);
  if(winner != 0){
  myGLCD.printNumI(winner, CENTER, 150,1);
  myGLCD.print("is Winner ^^", CENTER, 180);
  }else{
    myGLCD.print("draw", CENTER, 180);
  }
}
/* reset to start new game*/
void resetGame(int x, int y){
  if ((y>10) && (y<80)) { 
    if (x > 130 && (x < 200)) {  
      resetVariables();
      myGLCD.clrScr();
      drawButtons(); 
      
     }
    }
}

/* reset all variables used*/
void resetVariables(){
  finish = false;
  start = true;
  player = false;
  singlePlayer = false;
  currCol = 0;
  currRow = 0;
  for(int i = 0; i < 3; i++)
     for(int j = 0; j < 4; j++)
       matrix[i][j] = 0;
}
/*************************
**   logic functions   **
*************************/

/* check winner */
int win() {
    //determines if a player has won, returns 0 otherwise.
    unsigned wins[8][6] = {{0,0,0,1,0,2},{1,0,1,1,1,2},{2,0,2,1,2,2},{0,0,1,1,2,2},{0,2,1,1,2,0},{0,0,1,0,2,0},{0,1,1,1,2,1},{0,2,1,2,2,2}};
    int i;
    for(i = 0; i < 8; ++i) {
        if(matrix[wins[i][0]][wins[i][1]] != 0 &&
        matrix[wins[i][0]][wins[i][1]] == matrix[wins[i][2]][wins[i][3]] && matrix[wins[i][2]][wins[i][3]] == matrix[wins[i][4]][wins[i][5]])
            //playe
            if(matrix[wins[i][0]][wins[i][1]] == 1)
            return -1; // player
            else
            return 1; // computer
    }
    return 0;
}
int len = 0;

/*minimax algorithm*/
int minimax(int player2) { // sign = -1 computer
    int winner = win();
    len++;

    if(winner != 0)
      return winner;

    int moveRow = -1;
    int moveCol = -1;

    int score = -2;
    if(!player2)
      score = 2;
    for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 3; j++) {

        
        if(matrix[i][j] == 0) {

            matrix[i][j] = player2 + 1;
           int thisScore = minimax(!player2);
            if(player2 && thisScore > score){
                  score = thisScore;
                  moveRow = i;
                  moveCol = j;
              }
            if(!player2 && thisScore < score){
                  score = thisScore;
                  moveRow = i;
                  moveCol = j;
              }
            matrix[i][j] = 0;//Reset board after try
        }
      }
    }


    if(moveRow == -1 || moveCol== -1)
     return 0;
    
    return score;
}
/* arduino turn*/
void arduinoPlay(){
   int moveRow = -1;
   int moveCol = -1;
    int score = -2;

    for(int i = 0; i < 3; ++i) {
      for(int j = 0; j < 3; ++j){

        if(matrix[i][j] == 0) {
            matrix[i][j] = 2;
            int tempScore = minimax(0);
            matrix[i][j] = 0;
            
            if(tempScore > score) {
                score = tempScore;
                moveRow = i;
                moveCol = j;
            }
        }
      }
    }

  if(moveRow == -1) return;

    addToMatrix(moveRow, moveCol);
    currCol = moveCol;
    currRow = moveRow;
   }

/*check if a player has a compelete row*/
long int codeForRow(int i){
  return *(long int*)(matrix[i]);
}


/*check if a player has a compelete col*/
long int codeForCol(int i){
  long int toReturn = ((long int)matrix[0][i] << 8) + matrix[1][i];
  return (toReturn << 8) + matrix[2][i];
}


/*check if a player has a compelete diagonal*/
long int codeForDiag(int i){
  long int toReturn = ((long int)matrix[0][i*2] << 8) + matrix[1][1];
  return (toReturn << 8) + matrix[2][2-i*2];
}

/* check if matrix is full*/
bool full(){
  for(int i=0;i<3;i++)
    for(int j=0;j<3;j++)
      if(!matrix[i][j])
        return false;
  return true;
}

/* check for the winner */
void checkWinner(){
  int winner = 0;
  long int Row = codeForRow(currRow);
  long int Col = codeForCol(currCol);
  long int Di1 = codeForDiag(0);
  long int Di2 = codeForDiag(1);

  if(Row == 0x010101 || Col == 0x010101 ||
     Di1 == 0x010101 || Di2 == 0x010101 )
    winner = 1;

  if(Row == 0x020202 || Col == 0x020202 ||
     Di1 == 0x020202 || Di2 == 0x020202 )
    winner = 2;

  if(winner || full()){
    finish = true;
    gameOver(winner);
  }
}
   
void setup()
{
// Initial setup
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.setFont(BigFont);
  Serial.begin(9600);
  // initialize variable used
  width = 215;
  ratio = width / 3;
  XI = 50;
  YI = 10;
  //set begining of game
  start = true;
  drawButtons();  
}

void loop()
{    
  if (myTouch.dataAvailable()){
    myTouch.read();
    int x = myTouch.getX();
    int y = myTouch.getY();
    if(!finish){
     if(start){      // to selsect player mode
      selectPlayerMode(x,y);
     }else{         // playing!
        setMatrixSlot(x,y);
        delay(150);
        if(singlePlayer && player){
          arduinoPlay();
       }
     }
      checkWinner();
    }else{     // game has been finished, ask to play again
      resetGame(x,y);
    }
    }
}
