#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <malloc.h>

// Using gotoxy(x,y) which sets the cursor on entered x and y coordinates
#define gotoxy(x,y) printf ( "\033[%d;%dH", (y), (x) )

// Definition for each piece
typedef struct piece
{
    char name[25] ;
    int priority ;
    int rowpos ;
    int colpos ;
    int color ;
    // 1 stands for white, 0 for black
} piece ;

typedef struct square
{
    char item[15] ;
    piece *content ;
} square ;

typedef struct move
{
    int r, c ;
    // r stands for row, c for column
    struct move *next ;
} move ;

// stores all the possible moves for the current piece to check if the move entered by the user is a valid move
typedef struct moveStack
{
    move *top ;
} moveStack ;

typedef struct piecesLeft
{
    char pieceLeft[15] ;
    struct piecesLeft *next ;
} piecesLeft ;

// Record of all the remaining pieces for a player
typedef struct leftStack
{
    piecesLeft *top ;
} leftStack ;

piece pieces[8] = {
                    // { name, priority, row position, column position, color }
                    { "Pawn 1 (W)", 1, 3, 0, 1 },
                    { "Knight (W)", 3, 3, 1, 1 },
                    { "King (W)", 5, 3, 2, 1 },
                    { "Pawn 2 (W)", 1, 3, 3, 1 },
                    { "Pawn 1 (B)", 1, 0, 0, 0 },
                    { "Knight (B)", 3, 0, 1, 0 },
                    { "King (B)", 5, 0, 2, 0 },
                    { "Pawn 2 (B)", 1, 0, 3, 0 },
                  } ;
// 0=WPawn1, 1=WKnight, 2=WKing, 3=WPawn2, 4=BPawn1, 5=BKnight, 6=BKing, 7=BPawn2

char empty[10] = "         " ;

square board[4][4] ;

int stalemateCtr=0, WPoints = 0, BPoints = 0 ;

// This function takes a string and a number and prints that string that number of times
void printStr ( char c[], int times )
{
    for ( int i=0 ; i<times ; i++ )
        printf ( "%s", c ) ;
}

// Overloading earlier function for different use cases
void printStr2 ( char c1[], int times1, char c2[], int times2 )
{
    for ( int i=0 ; i<times1 ; i++ )
        printf ( "%s", c1 ) ;
    for ( int i=0 ; i<times2 ; i++ )
        printf ( "%s", c2 ) ;
}

void printStr3 ( char c1[], int t1, char c2[], int t2, char c3[], int t3  )
{
    for ( int i=0 ; i<t1 ; i++ )
        printf ( "%s", c1 ) ;

    for ( int i=0 ; i<t2 ; i++ )
        printf ( "%s", c2 ) ;

    for ( int i=0 ; i<t3 ; i++ )
        printf ( "%s", c3 ) ;
}

// Instructions for proceeding with the game
void instructions ()
{
    printStr2 ( "\n", 1, "\t", 9 ) ;
    printf ( "INTRODUCTION\n" ) ;
    printStr2 ( "\t", 9, "-", 12 ) ;
    printf ( "\n\tAlt Chess is a  2-player game which is an alternate take on the game of chess. " ) ;
    printf ( "It consists of a 4X4 chessboard with 2 pawns, 1 knight and 1 king with \n\tthe priority of pawn being the " ) ;
    printf ( "\tlowest and king being the highest.\n" ) ;
    printf ( "\tThe objective is to kill as many opponent pieces as we can and get points according to the priority of the killed piece.\n" ) ;
    printStr2 ( "\n", 1, "\t", 9 ) ;
    printf ( "INSTRUCTIONS\n" ) ;
    printStr2 ( "\t", 9, "-", 12 ) ;
    printf ( "\n" ) ;
    printf ( "\t\t\t1. White always moves first, and players get alternate turns. Only one piece can be moved at a time.\n" ) ;
    printf ( "\t\t\t2. Pieces cannot move through other pieces (though the knight can jump over other pieces), " ) ;
    printf ( "and can never move onto a square with one of \n\t\t\t   their own pieces.\n" ) ;
    printf ( "\t\t\t3. The pawn is promoted to the rook of the same color once it reaches the opposite side of the board.\n" ) ;
    printf ( "\t\t\t4. Pawns can move only in the forward direction. They can only move to diagonal blocks towards the forward direction for " ) ;
    printf ( "taking opponents.\n" ) ;
    printf ( "\t\t\t5. Knight can move in a shape like L. Two spaces in forward, backward or side-to-side followed by a right or left turn.\n" ) ;
    printf ( "\t\t\t6. The rook moves in a continuous line forward, backward or side-to-side.\n" ) ;
    printf ( "\t\t\t7. King can move in all directions, one square at a time.\n" ) ;
    printf ( "\t\t\t8. For your convenience, we have also displayed the possible moves you can make.\n" ) ;
    printf ( "\t\t\t9. Killing the king does not mean the end of the game. The game ends only when the pieces of the same color are left on " ) ;
    printf ( "the board.\n" ) ;
    printf ( "\t\t\t10. If no piece is killed after 12 moves by any player and only one piece for each player is left, the condition of " ) ;
    printf ( "stalemate is achieved,\n\t\t\t    i.e the player with more points will be declared the winner.\n" ) ;
    printf ( "\t\t\t11. The priority of pieces is as followed:\n" ) ;
    printf ( "\t\t\t\t~King: Strongest, Points on killing King = 5\n" ) ;
    printf ( "\t\t\t\t~Rook: Intermediate, Points on killing Rook = 4\n" ) ;
    printf ( "\t\t\t\t~Knight: Intermediate, Points on killing Knight = 3\n" ) ;
    printf ( "\t\t\t\t~Pawn: Weakest, Points on killing Pawn = 1\n" ) ;

    printStr2 ( "\n", 3, "\t", 8 ) ;
    printf ( "*****Press any key to continue.***** \n" ) ;

    getch () ;
    system( "cls" ) ;
}

// Records points of both teams
void scorecounter ( int newrpos, int newcpos )
{
    if ( board[newrpos][newcpos].content ) //If something needs to be killed
    {
        if( board[newrpos][newcpos].content->color == 1 )
            BPoints += board[newrpos][newcpos].content->priority ;
        else if( board[newrpos][newcpos].content->color == 0 )
            WPoints += board[newrpos][newcpos].content->priority ;
    }
}

// Displays points of both teams
void scoreboard ()
{
    printStr2 ( "\n", 2, "\t", 9 ) ;
    printf ( "The current scores are:\n" ) ;
    printStr ( "\t", 12 ) ;
    printf ( "*Team White = %d\n", WPoints ) ;
    printStr ( "\t", 12 ) ;
    printf ( "*Team Black = %d\n", BPoints ) ;
}

// Stores all possible moves for current piece to check if move entered by the user is a valid move
void pushInMoveStack ( moveStack *stack, int row, int column )
{
    move *toadd = (move*)malloc(sizeof(move)) ;
    toadd->c = column ;
    toadd->r = row ;
    toadd->next=NULL ;

    if ( !((*stack).top) )
        (*stack).top = toadd ;
    else
    {
        toadd->next = (*stack).top ;
        (*stack).top=toadd ;
    }
}

// New Record of all the remaining pieces for a player
void pushInLeftStack ( leftStack *stack, char toAdd[] )
{
    piecesLeft *toadd = (piecesLeft*)malloc(sizeof(piecesLeft)) ;
    strcpy ( toadd->pieceLeft, toAdd ) ;
    toadd->next=NULL ;

    if ( !((*stack).top) )
        (*stack).top = toadd ;
    else
    {
        toadd->next = (*stack).top ;
        (*stack).top=toadd ;
    }
}

// Function to empty a stack of moves
void emptyMoveStack ( moveStack *stack )
{
    (*stack).top=NULL ;
}


int remainingCtr ( leftStack *stack )
{
    piecesLeft *ctr = (*stack).top ;
    int count=0 ;

    while (ctr)
    {
        count++ ;
        ctr=ctr->next ;
    }

    return count ;
}

// Deleting killed pieces from left stack
void delFromLeftStack ( leftStack *stack, char toDel[] )
{
    piecesLeft *finder = (*stack).top ;

    if ( strcmp(finder->pieceLeft, toDel)==0 )
    {
        piecesLeft *todel = finder ;
        (*stack).top = ((*stack).top)->next ; ;
        free(todel) ;
    }
    else
    {
        while ( finder->next )
        {
            if ( strcmp(finder->next->pieceLeft, toDel)==0 )
            {
                piecesLeft *todel = finder->next ;
                finder->next=finder->next->next ;
                free(todel) ;
            }
            if ( finder->next )
                finder=finder->next ;
        }
    }
}

// Function to check if a piece is still alive
int pieceExists ( leftStack *stack, char toFind[] )
{
    piecesLeft *finder = (*stack).top ;

    while ( finder )
    {
        if ( strcmp(finder->pieceLeft, toFind)==0 )
            return 1 ;
        finder=finder->next ;
    }
    return 0 ;
    // Function returns 1 if piece is alive, 0 if it is not
}

// Function to change a value in a stack
void updateLeftStack ( leftStack *stack, char before[], char after[] )
{
    piecesLeft *finder = (*stack).top ;

    while ( finder )
    {
        if ( strcmp(finder->pieceLeft, before)==0 )
        {
            strcpy ( finder->pieceLeft, after ) ;
            break ;
        }
        finder = finder->next ;
    }
}

// Function to check if an entered move is valid
int checkIfValidMove ( moveStack *stack, int row, int col )
{
    move *finder=stack->top ;

    while ( finder )
    {
        if ( finder->c==col && finder->r==row )
            return 1 ;
        finder=finder->next ;
    }
    return 0 ;
    // Function returns 1 if there is a valid move, 0 if it is not
}

// Creating a new board and assigning starting positions to the pieces
void createboard ( leftStack *black, leftStack *white )
{
    for ( int i=0 ; i<4 ; i++ )
    {
        for ( int j=0 ; j<4 ; j++ )
        {
            board[i][j].content=NULL ;
            strcpy ( board[i][j].item, empty ) ;
        }
    }
    for ( int i=0 ; i<4 ; i++ )
    {
        board[3][i].content=&(pieces[i]) ;
        strcpy ( board[3][i].item, pieces[i].name ) ;
        board[0][i].content=&(pieces[i+4]) ;
        strcpy ( board[0][i].item, pieces[i+4].name ) ;
    }
    for ( int i=0 ; i<8 ; i++ )
    {
        if ( i<4 )
            // Adding white pieces to white stack
            pushInLeftStack ( white, pieces[i].name ) ;
        else
            // Adding black pieces to black stack
            pushInLeftStack ( black, pieces[i].name ) ;
    }
}

// Displays current state of the board for a game
void displayboard ()
{
    printStr3 ( "\n", 2, "\t", 5, " ", 10 ) ;
    printStr3 ( "  0", 1, "\t", 3, "    1", 1 ) ;
    printStr3 ( "\t", 3, "    2", 1, "\t", 3 ) ;
    printf ( "    3" ) ;
    printStr3 ( "\t", 3, " ", 1, "\n", 1 ) ;
    printStr3 ( "\n", 1, "\t", 5, "-", 97 ) ;
    printf ( "\n" ) ;
    for ( int i=0 ; i<4 ; i++ )
    {
        printf ( "\t\t\t\t%d\t", i ) ;
        printf ( "|" ) ;
        for ( int j=0 ; j<4 ; j++ )
            printf ( "\t%s\t|", board[i][j].item ) ;
        printStr3 ( "\n", 1, "\t", 5, "-", 97 ) ;
        printf ( "\n" ) ;
    }
}

// Displays available pieces for Player 1 (White)
void displayPiecesP1 ()
{
    for ( int i=1 ; i<=4 ; i++ )
    {
        if ( pieces[i-1].rowpos != -1 )
        {
            printStr ( "\t", 9 ) ;
            printf ( "%d. %s\n", i, pieces[i-1].name ) ;
        }
    }
    printf ( "\n" ) ;
}

// Displays available pieces for Player 2 (Black)
void displayPiecesP2 ()
{
    for ( int i=5 ; i<=8 ; i++ )
    {
        if ( pieces[i-1].rowpos != -1 )
        {
            printStr ( "\t", 9 ) ;
            printf ( "%d. %s\n", i-4, pieces[i-1].name ) ;
        }
    }
    printf ( "\n" ) ;
}

// Function to check if a piece is at one of the edges of the board
int checkedgepiece ( int a )
{
    if ( a==0 )
        return 1 ;
    else if ( a==3 )
        return 2 ;
    else
        return 0 ;
    // Function returns 1 for left edge, 2 for right edge, 0 if not an edge piece
}

// Function to check if an enemy is present at an entered position
int findenemy ( int r, int c, int piececolor )
{
    if ( board[r][c].content && board[r][c].content->color != piececolor )
        return 1 ;
    else
        return 0 ;
    // Function returns 1 if enemy is present on valid move, 0 if no enemy is present on valid move
}

// Message to enter full screen mode is displayed
void fullScreenMessage ()
{
    printStr2 ( "\n", 5, "\t", 9 ) ;
    printf ( "Please enter full screen\n" ) ;
    printStr3 ( "\t", 9, "~", 24, "\n", 1 ) ;
    printStr2 ( "\n", 10, "\t", 9 ) ;
    printf ( "Press any key to continue" ) ;
    getch () ;
    system ( "cls" ) ;
}

// Shows valid moves for all pieces
int showvalidmovesforuser ( moveStack *stack, piece a , int val )
{
    int flag=0 ;

    // Code for white pawn movement
    if ( strcmp ( a.name, "Pawn 1 (W)" )==0 || strcmp ( a.name, "Pawn 2 (W)" )==0 )
    {
        // Forward movement
        if ( a.rowpos-1 >= 0 && strcmp ( board[a.rowpos-1][a.colpos].item, empty ) == 0 )
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos ) ;
            }
            pushInMoveStack( stack, a.rowpos-1, a.colpos ) ;
            flag=1 ;
        }

        // For pawn present at left edge (view in direction of forward movement)
        if ( checkedgepiece ( a.colpos ) == 1 )
        {
            // For attacking enemy using diagonal movement
            if ( a.colpos+1 >= 0 && a.rowpos-1 >= 0 && findenemy(a.rowpos-1, a.colpos+1, a.color)==1 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos+1 ) ;
                }
                pushInMoveStack( stack, a.rowpos-1, a.colpos+1 ) ;
                flag=1 ;
            }
        }

        // For pawn present at right edge (view in direction of forward movement)
        else if ( checkedgepiece ( a.colpos ) == 2 )
        {
            // For attacking enemy using diagonal movement
            if ( a.colpos-1 >= 0 && a.rowpos-1 >= 0 && findenemy(a.rowpos-1, a.colpos-1, a.color)==1 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos-1 ) ;
                }
                pushInMoveStack( stack, a.rowpos-1, a.colpos-1 ) ;
                flag=1 ;
            }
        }

        // For pawn present anywhere except the edges
        else
        {
            // For attacking enemy using diagonal movement
            if ( a.colpos+1 >= 0 && a.rowpos-1 >= 0 && findenemy(a.rowpos-1, a.colpos+1, a.color)==1 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos+1 ) ;
                }
                pushInMoveStack( stack, a.rowpos-1, a.colpos+1 ) ;
                flag=1 ;
            }

            // For attacking enemy using diagonal movement
            if ( a.colpos-1 >= 0 && a.rowpos-1 >= 0 && findenemy(a.rowpos-1, a.colpos-1, a.color)==1 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos-1 ) ;
                }
                pushInMoveStack( stack, a.rowpos-1, a.colpos-1 ) ;
                flag=1 ;
            }
        }
    }

    // Code for black pawn movement
    else if ( strcmp ( a.name, "Pawn 1 (B)" )==0 || strcmp ( a.name, "Pawn 2 (B)" )==0 ) // Code for pawn movement
    {
        // Forward Movement
        if ( a.rowpos+1 <= 3 && strcmp ( board[a.rowpos+1][a.colpos].item, empty ) == 0 )
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos ) ;
            }
            pushInMoveStack( stack, a.rowpos+1, a.colpos ) ;
            flag=1 ;
        }

        // For pawn present at left edge (view in direction of forward movement)
        if ( checkedgepiece ( a.colpos ) == 2 )
        {
            // For attacking enemy using diagonal movement
            if ( a.colpos-1 <= 3 && a.rowpos+1 <= 3 && findenemy(a.rowpos+1, a.colpos-1, a.color)==1 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos-1 ) ;
                }
                pushInMoveStack( stack, a.rowpos+1, a.colpos-1 ) ;
                flag=1 ;
            }
        }

        // For pawn present at right edge (view in direction of forward movement)
        else if ( checkedgepiece ( a.colpos ) == 1 )
        {
            // For attacking enemy using diagonal movement
            if ( a.colpos+1 <= 3 && a.rowpos+1 <= 3 && findenemy(a.rowpos+1, a.colpos+1, a.color)==1 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos+1 ) ;
                }
                pushInMoveStack( stack, a.rowpos+1, a.colpos+1 ) ;
                flag=1 ;
            }
        }

        // For pawn present anywhere except the edges
        else
        {
            // For attacking enemy using diagonal movement
            if ( a.colpos+1 >= 0 && a.rowpos+1 <= 3 && findenemy(a.rowpos+1, a.colpos+1, a.color)==1 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos+1 ) ;
                }
                pushInMoveStack( stack, a.rowpos+1, a.colpos+1 ) ;
                flag=1 ;
            }

            // For attacking enemy using diagonal movement

            if ( a.colpos-1 >= 0 && a.rowpos+1 >= 3 && findenemy(a.rowpos+1, a.colpos-1, a.color)==1 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos-1 ) ;
                }
                pushInMoveStack( stack, a.rowpos+1, a.colpos-1 ) ;
                flag=1 ;
            }
        }
    }

    // Code for King movement
    else if ( strcmp ( a.name, "King (W)" )==0 || strcmp ( a.name, "King (B)" )==0 ) // Code for King movement
    {
        //Forward movement for white king, backward for black king
        if ( a.rowpos-1 >= 0 && a.rowpos-1<4  && strcmp ( board[a.rowpos-1][a.colpos].item, empty ) == 0
             || a.rowpos-1 >= 0&& a.rowpos-1<4 && findenemy(a.rowpos-1, a.colpos, a.color)==1 )
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos ) ;
            }
            pushInMoveStack( stack, a.rowpos-1, a.colpos ) ;
            flag=1 ;
        }

        // If white king is present at left edge, black king is at right edge
        if ( checkedgepiece ( a.colpos ) == 1 )
        {
            // Shows valid move one square ahead in the directions if either enemy is present or block is empty
            if ( a.colpos+1 >= 0 && a.rowpos-1 >= 0 && a.colpos+1 < 4 && a.rowpos-1<4 && findenemy(a.rowpos-1, a.colpos+1, a.color)==1
               || a.colpos+1 >= 0 && a.rowpos-1 >= 0 && a.colpos+1 < 4 && a.rowpos-1<4 && strcmp ( board[a.rowpos-1][a.colpos+1].item, empty )==0 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos+1 ) ;
                }
                pushInMoveStack( stack, a.rowpos-1, a.colpos+1 ) ;
                flag=1 ;
            }
        }

        // If white king is present at right edge, black king is at left side
        else if ( checkedgepiece ( a.colpos ) == 2 )
        {
            // Shows valid move one square ahead in the directions if either enemy is present or block is empty
            if ( a.colpos-1 >= 0 && a.rowpos-1 >= 0 &&  a.colpos-1 < 4 && a.rowpos-1 < 4&& findenemy(a.rowpos-1, a.colpos-1, a.color)==1
            || a.colpos-1 >= 0 && a.rowpos-1 >= 0 &&  a.colpos-1 < 4 && a.rowpos-1 < 4 && strcmp ( board[a.rowpos-1][a.colpos-1].item, empty )==0 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos-1 ) ;
                }
                pushInMoveStack( stack, a.rowpos-1, a.colpos-1 ) ;
                flag=1 ;
            }
        }

        // If the king ( white or black ) is present anywhere except the edges
        else
        {
            // Shows valid move one square ahead in the directions if either enemy is present or block is empty
            if ( a.colpos+1 >= 0 && a.rowpos-1 >= 0 && a.colpos+1 < 4 && a.rowpos-1 <4 && findenemy(a.rowpos-1, a.colpos+1, a.color)==1
            || a.colpos+1 >= 0 && a.colpos+1 < 4 && a.rowpos-1 <4 && a.rowpos-1 >= 0 && strcmp ( board[a.rowpos-1][a.colpos+1].item, empty )==0 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos+1 ) ;
                }
                pushInMoveStack( stack, a.rowpos-1, a.colpos+1 ) ;
                flag=1 ;
            }

            // Shows valid move one square ahead in the directions if either enemy is present or block is empty
            if ( a.colpos-1 >= 0 && a.rowpos-1 >= 0 && a.colpos-1 < 4 && a.rowpos-1 <4 && findenemy(a.rowpos-1, a.colpos-1, a.color)==1
            || a.colpos-1 >= 0 && a.rowpos-1 >= 0 && a.colpos-1 < 4 && a.rowpos-1 <4  && strcmp ( board[a.rowpos-1][a.colpos-1].item, empty )==0 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos-1 ) ;
                }
                pushInMoveStack( stack, a.rowpos-1, a.colpos-1 ) ;
                flag=1 ;
            }
        }

        // Right move for white king, left move for black king
        if ( a.colpos+1 >= 0 && a.colpos+1<4 && strcmp ( board[a.rowpos][a.colpos+1].item, empty ) == 0
            || a.colpos+1 >= 0 && a.colpos+1<4 && findenemy(a.rowpos, a.colpos+1, a.color)==1 )
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos, a.colpos+1 ) ;
            }
            pushInMoveStack( stack, a.rowpos, a.colpos+1 ) ;
            flag=1 ;
        }

        // Left move for white king, right move for black king
        if ( a.colpos-1 >= 0 && a.colpos-1<4 && strcmp ( board[a.rowpos][a.colpos-1].item, empty ) == 0
            || a.colpos-1 >= 0&& a.colpos-1<4 && findenemy(a.rowpos, a.colpos-1, a.color)==1 )
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos, a.colpos-1 ) ;
            }
            pushInMoveStack( stack, a.rowpos, a.colpos-1 ) ;
            flag=1 ;
        }

        // Backward movement for white king, forward for white king
        if ( a.rowpos+1 >= 0 && a.rowpos+1<4 && strcmp ( board[a.rowpos+1][a.colpos].item, empty ) == 0
            ||  a.rowpos+1 >= 0 && a.rowpos+1<4 && findenemy(a.rowpos+1, a.colpos,a.color)==1 )
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos ) ;
            }
            pushInMoveStack( stack, a.rowpos+1, a.colpos ) ;
            flag=1 ;
        }

        // If white king is present at left edge, black king at right edge
        if ( checkedgepiece ( a.colpos ) == 1 )
        {
            // Shows valid move one square ahead in the directions if either enemy is present or block is empty
            if ( a.colpos+1 >= 0 && a.rowpos+1 >= 0 &&  a.colpos+1 <4 && a.rowpos+1<4 && findenemy(a.rowpos+1, a.colpos+1, a.color)==1
               || a.colpos+1 >= 0 && a.rowpos+1 >= 0&&  a.colpos+1 <4 && a.rowpos+1<4  && strcmp ( board[a.rowpos+1][a.colpos+1].item, empty )==0 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos+1 ) ;
                }
                pushInMoveStack( stack, a.rowpos+1, a.colpos+1 ) ;
                flag=1 ;
            }
        }

        // If white king is present at right edge, black king at left edge
        else if ( checkedgepiece ( a.colpos ) == 2 )
        {
            // Shows valid move one square ahead in the directions if either enemy is present or block is empty
            if ( a.colpos-1 >= 0 && a.colpos-1<4 && a.rowpos+1 >= 0 && a.rowpos+1<4 && findenemy(a.rowpos+1, a.colpos-1, a.color)==1
                || a.colpos-1 >= 0&& a.colpos-1<4  && a.rowpos+1 >= 0 && a.rowpos+1<4 && strcmp ( board[a.rowpos+1][a.colpos-1].item, empty )==0 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos-1 ) ;
                }
                pushInMoveStack( stack, a.rowpos+1, a.colpos-1 ) ;
                flag=1 ;
            }
        }

        // If king ( white or black ) is present anywhere except the edges
        else
        {
            // Shows valid move one square ahead in the directions if either enemy is present or block is empty
            if ( a.colpos+1 >= 0 && a.colpos+1<4 && a.rowpos+1 >= 0&&  a.rowpos+1<4 && findenemy(a.rowpos+1, a.colpos+1, a.color)==1
               || a.colpos+1 >= 0 &&  a.colpos+1<4  && a.rowpos+1 >= 0&&  a.rowpos+1<4 && strcmp ( board[a.rowpos+1][a.colpos+1].item, empty )==0 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos+1 ) ;
                }
                pushInMoveStack( stack, a.rowpos+1, a.colpos+1 ) ;
                flag=1 ;
            }

            // Shows valid move one square ahead in the directions if either enemy is present or block is empty
            if ( a.colpos-1 >= 0 && a.rowpos+1 >= 0 && a.colpos-1 < 4 && a.rowpos+1 < 4 && findenemy(a.rowpos+1, a.colpos-1, a.color)==1
            || a.colpos-1 >= 0 && a.rowpos+1 >= 0 && a.colpos-1 < 4 && a.rowpos+1 < 4  && strcmp ( board[a.rowpos+1][a.colpos-1].item, empty )==0 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos-1 ) ;
                }
                pushInMoveStack( stack, a.rowpos+1, a.colpos-1 ) ;
                flag=1 ;
            }
        }
    }

    // Code for knight movement
    else if ( strcmp ( a.name, "Knight (W)" )==0 || strcmp ( a.name, "Knight (B)" )==0 )
    {
        // 2 steps forward-1 step left movement for white knight, 2 steps backward-1 step right movement for black knight
        if (  a.rowpos-2>= 0 && a.colpos-1 >= 0 && strcmp ( board[a.rowpos-2][a.colpos-1].item, empty ) == 0
              || a.rowpos-2 >= 0 && a.colpos-1 >= 0 && findenemy(a.rowpos-2, a.colpos-1, a.color)==1 )
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos-2, a.colpos-1 ) ;
            }
            pushInMoveStack( stack, a.rowpos-2, a.colpos-1 ) ;
            flag=1 ;
        }

        if ( a.rowpos-2 >= 0 && a.colpos+1 <= 3 && strcmp ( board[a.rowpos-2][a.colpos+1].item, empty ) == 0
             || a.rowpos-2 >= 0 && a.colpos+1 <= 3 && findenemy(a.rowpos-2, a.colpos+1, a.color)==1 )
        // 2 steps forward-1 step right movement for white knight, 2 steps backward-1 step left movement for black knight
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos-2, a.colpos+1 ) ;
            }
            pushInMoveStack( stack, a.rowpos-2, a.colpos+1 ) ;
            flag=1 ;
        }

        if ( a.rowpos-1 >= 0 && a.colpos-2 >= 0 && strcmp ( board[a.rowpos-1][a.colpos-2].item, empty ) == 0
             || a.rowpos-1 >= 0 && a.colpos-2 >= 0 && findenemy(a.rowpos-1, a.colpos-2, a.color)==1 )
        // 1 step forward-2 steps left movement for white knight, 1 step backward-2 steps right movement for black knight
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos-2 ) ;
            }
            pushInMoveStack( stack, a.rowpos-1, a.colpos-2 ) ;
            flag=1 ;
        }

        if ( a.rowpos-1 >= 0 && a.colpos+2 <= 3 && strcmp ( board[a.rowpos-1][a.colpos+2].item, empty ) == 0
             || a.rowpos-1 >= 0 && a.colpos+2 <= 3 && findenemy(a.rowpos-1, a.colpos+2, a.color)==1 )
        // 1 step forward-2 steps right movement for white knight, 1 step backward-2 steps left movement for black knight
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos-1, a.colpos+2 ) ;
            }
            pushInMoveStack( stack, a.rowpos-1, a.colpos+2 ) ;
            flag=1 ;
        }

        if ( a.rowpos+1 >= 0 && a.colpos-2 >= 0 && strcmp ( board[a.rowpos+1][a.colpos-2].item, empty ) == 0
             || a.rowpos+1 >= 0 && a.colpos-2 >= 0 && findenemy(a.rowpos+1, a.colpos-2, a.color)==1 )
        // 1 step backward-2 steps left movement for white knight, 1 step forward-2 steps right movement for black knight
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos-2 ) ;
            }
            pushInMoveStack( stack, a.rowpos+1, a.colpos-2 ) ;
            flag=1 ;
        }

        if ( a.rowpos+1 >= 0 && a.colpos+2 <= 3 && strcmp ( board[a.rowpos+1][a.colpos+2].item, empty ) == 0
             || a.rowpos+1 >= 0 && a.colpos+2 <= 3 && findenemy(a.rowpos+1, a.colpos+2, a.color)==1  )
        //1 step backward-2 steps right movement for white knight, 1 step forward-2 steps left movement for black knight
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos+1, a.colpos+2) ;
            }
            pushInMoveStack( stack, a.rowpos+1, a.colpos+2 ) ;
            flag=1 ;
        }

        if ( a.rowpos+2 >= 0 && a.colpos-1 >= 0 && strcmp ( board[a.rowpos+2][a.colpos-1].item, empty ) == 0
             || a.rowpos+2 >= 0 && a.colpos-1 >= 0 && findenemy(a.rowpos+2, a.colpos-1, a.color)==1 )
        // 2 steps backward-1 step left movement for white knight, 2 steps forward-1 step right movement for black knight
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos+2, a.colpos-1 ) ;
            }
            pushInMoveStack( stack, a.rowpos+2, a.colpos-1 ) ;
            flag=1 ;
        }

        if ( a.rowpos+2 >= 0 && a.colpos+1 <= 3 && strcmp ( board[a.rowpos+2][a.colpos+1].item, empty ) == 0
             || a.rowpos+2 >= 0 && a.colpos+1 <= 3 && findenemy(a.rowpos+2, a.colpos+1, a.color)==1 )
        // 2 steps backward-1 step right movement for white knight, 2 steps forward-1 step left movement for black knight
        {
            if ( val==1 )
            {
                printStr ( "\t", 11 ) ;
                printf ( "Position R:%d and C:%d \n", a.rowpos+2, a.colpos+1 ) ;
            }
            pushInMoveStack( stack, a.rowpos+2, a.colpos+1 ) ;
            flag=1 ;
        }
    }

    // This code applies to pawns promoted to rook
    else if ( strcmp ( a.name, "Rook 1 (W)")==0 || strcmp ( a.name, "Rook 2 (W)")==0 || strcmp ( a.name, "Rook 1 (B)")==0
            || strcmp ( a.name, "Rook 2 (B)")==0 )
    {
        for ( int i=a.colpos-1 ; i>=0 ; i-- )
            if (findenemy ( a.rowpos, i, a.color )==1 || strcmp ( board[a.rowpos][i].item, empty ) == 0 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos, i ) ;
                }
                pushInMoveStack( stack, a.rowpos, i ) ;
                flag=1 ;
                if ( findenemy ( a.rowpos, i, a.color )==1 )
                    break;
            }

        for ( int i=a.colpos+1 ; i<=3 ; i++ )
            if (findenemy ( a.rowpos, i, a.color )==1 || strcmp ( board[a.rowpos][i].item, empty ) == 0 )
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", a.rowpos, i ) ;
                }
                pushInMoveStack( stack, a.rowpos, i ) ;
                flag=1 ;
                if ( findenemy ( a.rowpos, i, a.color )==1 )
                    break;
            }

        for ( int j=a.rowpos-1 ; j>=0 ; j-- )
            if ( findenemy ( j, a.colpos, a.color )==1 || strcmp ( board[j][a.colpos].item, empty ) == 0)
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", j, a.colpos ) ;
                }
                pushInMoveStack( stack, j, a.colpos ) ;
                flag=1 ;
                if ( findenemy ( j, a.colpos, a.color )==1 )
                    break;
            }

        for ( int j=a.rowpos+1 ; j<=3 ; j++ )
            if ( findenemy ( j, a.colpos, a.color )==1 || strcmp ( board[j][a.colpos].item, empty ) == 0)
            {
                if ( val==1 )
                {
                    printStr ( "\t", 11 ) ;
                    printf ( "Position R:%d and C:%d \n", j, a.colpos ) ;
                }
                pushInMoveStack( stack, j, a.colpos ) ;
                flag=1 ;
                if ( findenemy ( j, a.colpos, a.color )==1 )
                    break;
            }
    }
    return flag ;
}

// Function to move a piece to a given position
void movepiece ( int a, int newrpos, int newcpos, leftStack *stack )
{
    stalemateCtr++ ;

    // If something needs to be killed
    if ( board[newrpos][newcpos].content )
    {
        printStr3 ( "\n", 1, "\t", 9, "~", 2 ) ;
        printf ( "Killed %s!~~\n", board[newrpos][newcpos].content->name ) ;

        // Killed piece deleted from record of remaining pieces for the player
        delFromLeftStack( stack, board[newrpos][newcpos].item ) ;

        stalemateCtr=0 ;
        board[newrpos][newcpos].content->colpos=-1 ;
        board[newrpos][newcpos].content->rowpos=-1 ;
    }

    strcpy ( board[pieces[a].rowpos][pieces[a].colpos].item, empty ) ;
    board[pieces[a].rowpos][pieces[a].colpos].content=NULL ;
    pieces[a].rowpos=newrpos ;
    pieces[a].colpos=newcpos ;
    strcpy ( board[pieces[a].rowpos][pieces[a].colpos].item, pieces[a].name ) ;
    board[newrpos][newcpos].content=&(pieces[a]) ;
}

// Pawn promotes to rook once it reaches opposite end of the board
void promote ( piece *a, leftStack *stack )
{
    if ( strcmp ( (*a).name, "Pawn 1 (W)" )==0 )
    {
        strcpy ( (*a).name, "Rook 1 (W)" ) ;
        updateLeftStack ( stack, "Pawn 1 (W)", "Rook 1 (W)" ) ;
        pieces[0].priority = 4 ;
    }
    else if ( strcmp ( (*a).name, "Pawn 2 (W)" )==0 )
    {
        strcpy ( (*a).name, "Rook 2 (W)" ) ;
        updateLeftStack ( stack, "Pawn 2 (W)", "Rook 2 (W)" ) ;
        pieces[3].priority = 4 ;
    }
    else if ( strcmp ( (*a).name, "Pawn 1 (B)" )==0 )
    {
        strcpy ( (*a).name, "Rook 1 (B)" ) ;
        updateLeftStack ( stack, "Pawn 1 (B)", "Rook 1 (B)" ) ;
        pieces[4].priority = 4 ;
    }
    else if ( strcmp ( (*a).name, "Pawn 2 (B)" )==0 )
    {
        strcpy ( (*a).name, "Rook 2 (B)" ) ;
        updateLeftStack ( stack, "Pawn 2 (B)", "Rook 2 (B)" ) ;
        pieces[7].priority = 4 ;
    }
}

void declareWinner ( leftStack *black, leftStack *white )
{
    // If only white pieces are present on the board
    if ( remainingCtr(black)==0 )
    {
        printStr2 ( "\n", 9, "\t", 9 ) ;
        printf ( "Team White wins! \n\n\n\n" ) ;
        return ;
    }
    // If only black pieces are present on the board
    else if ( remainingCtr(white)==0 )
    {
        printStr2 ( "\n", 9, "\t", 9 ) ;
        printf ( "Team Black wins! \n\n\n\n" ) ;
        return ;
    }
    else
    {
        if ( WPoints > BPoints )
        {
            printStr2 ( "\n", 9, "\t", 9 ) ;
            printf ( "Team White wins! \n\n\n\n" ) ;
            return ;
        }
        else if ( BPoints > WPoints )
        {
            printStr2 ( "\n", 9, "\t", 9 ) ;
            printf ( "Team Black wins! \n\n\n\n" ) ;
            return ;
        }
        else
        {
            printStr2 ( "\n", 9, "\t", 9 ) ;
            printf ( "Game tied! \n\n\n\n" ) ;
            return ;
        }
    }
}

// Prints the box in which piece number is entered
void  display_textbox ()
{
    // To fix the box at the entered coordinates
    gotoxy ( 1, 28 ) ;
    printStr3 ( "\t", 9, "+", 1, "-", 17 ) ;
    printf ( "+\n" ) ;
    gotoxy ( 1, 29 ) ;
    printStr3 ( "\t", 9, "|", 1, " ", 17 ) ;
    printf ( "|\n" ) ;
    gotoxy ( 1, 30 ) ;
    printStr3 ( "\t", 9, "+", 1, "-", 17 ) ;
    printf ( "+\n" ) ;
}


//When no moves are available for the player then their turn is skipped
int skipTurn ( moveStack *stack1, leftStack *stack2, int piececolor )
{
    int ctr=0 ;
    // If color of the piece is white
    if ( piececolor==1 )
        for ( int i=0 ; i<4 ; i++ )
            // If a piece is killed or the pieces on board have no moves then, turn will be skipped
            if ( showvalidmovesforuser ( stack1, pieces[i],0 )==0 || ( pieceExists( stack2, pieces[i].name ))==0 )
                ctr++ ;
    //If color of the piece is black
    if ( piececolor==0 )
        for ( int j=4 ; j<8 ; j++ )
            if ( showvalidmovesforuser ( stack1, pieces[j],0 )==0 || ( pieceExists( stack2, pieces[j].name ))==0 )
                ctr++ ;
    if (ctr==4)
    {
        printStr2 ( "\n", 2, "\t", 9 ) ;
        printf ( "No more moves. Turn Skipped. \n" ) ;
        printStr2 ( "\n", 1, "\t", 9 ) ;
        printf ( "**Press any key to continue**.\n" ) ;
        getch () ;
    }
    return ctr ;
}

void main ()
{
    // Message to enter full screen mode is displayed
    fullScreenMessage () ;

    // Instructions required to play the game are displayed
    instructions () ;

    int ans=0, r, c, movesExist, forfeit=0, promotedOrNot[4] = { 0 } ;  // 0=WP1, 1=WP2, 2=BP1, 3=BP2

    moveStack stack ;
    stack.top=NULL ;

    // Black and White stacks are created with all pieces
    leftStack black, white ;
    black.top=NULL ;
    white.top=NULL ;

    createboard (&black, &white) ;

    // If person chooses to stay in the game, stalemate condition is not achieved and pieces of both colors are present on the board
    while ( ans!=-1 && stalemateCtr<12 && remainingCtr(&black)>0 && remainingCtr(&white)>0 )
    {
        // Label for start of Player 1 (white)'s turn
        P1:
        system ( "cls" ) ;

        // Game board is displayed
        displayboard () ;

        // Initial scores of both teams are 0
        scoreboard () ;

        printStr2 ( "\n", 1, "\t", 9 ) ;
        printf ( "Team White's turn " ) ;
        printStr3 ( "\n", 1, "\t", 9, "-", 19 ) ;
        printStr2 ( "\n", 1, "\t", 9 ) ;
        printf ( "Which piece to move? (Enter -1 to exit) \n" ) ;

        // All white pieces in the left stack are displayed
        displayPiecesP1 () ;

        // Text box to enter the piece number is displayed
        display_textbox () ;

        //The cursor appears at the entered coordinates
        gotoxy(82,29) ;
        scanf ( "%d", &ans ) ;

        // Actions to take based on response
        switch ( ans )
        {
            case -1:

                // When the player wants to forfeit
                printStr2 ( "\n", 2, "\t", 8 ) ;
                printf( "Are you sure you want to forfeit? (Enter 1 for Yes And 2 for No): " ) ;
                scanf ( "%d", &forfeit ) ;

                if ( forfeit==1 )
                {
                    system("cls") ;
                    printStr2 ( "\n", 9, "\t", 9 ) ;
                    printf ( "Team Black wins! \n\n\n\n" ) ;
                    exit(0) ;
                }

                else
                    goto P1 ;
                break ;

            case 1:

                // Chosen piece is not present on the board
                if ( !(pieceExists( &white, pieces[0].name )) )
                {
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "You can't move that piece! \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **.\n" ) ;
                    getch () ;
                    goto P1 ;
                }

                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf ( "Chosen piece: %s. ", pieces[0].name ) ;
                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "Possible moves: \n" ) ;

                // Valid moves for the piece are displayed
                movesExist = showvalidmovesforuser ( &stack, pieces[0],1 ) ;

                if ( movesExist==1 )
                {
                    printStr ( "\t", 9 ) ;
                    printf ( "R: " ) ;
                    scanf ( "%d", &r ) ;
                    printStr ( "\t", 9 ) ;
                    printf ( "C: " ) ;
                    scanf ( "%d", &c ) ;

                    // If user enters valid move
                    if ( checkIfValidMove( &stack, r, c ) )
                    {
                        // If pawn reaches opposite end of the board
                        if ( r==0 && promotedOrNot[0]==0 )
                        {
                            printStr2 ( "\n", 2, "\t", 9 ) ;
                            printf ( "Your pawn has been promoted to a rook! \n" ) ;
                            promote ( &pieces[0], &white ) ;
                            promotedOrNot[0]=1 ;
                        }

                        // Points are recorded
                        scorecounter ( r, c ) ;
                        movepiece ( 0, r, c, &black ) ;
                    }

                    // If user plays an invalid move
                    else
                    {
                        printStr2 ( "\n", 2, "\t", 9 ) ;
                        printf ( "** Invalid move ** \n" ) ;
                        printStr2 ( "\n", 1, "\t", 9 ) ;
                        printf ( "** Press any key to continue **.\n" ) ;

                        emptyMoveStack ( &stack ) ;

                        getch() ;
                        goto P1 ;
                    }
                }

                // If no move is possible for the player,turn is skipped
                else
                {
                    if ( ( skipTurn(&stack,&white,1) )==4 )
                        goto P2 ;

                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "No move available. \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **. \n" ) ;

                    emptyMoveStack ( &stack ) ;

                    getch () ;
                    goto P1 ;
                }

                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "**Press any key to continue**. \n" ) ;

                emptyMoveStack ( &stack ) ;
                getch() ;
                break ;

            case 2:

                if ( !(pieceExists( &white, pieces[1].name )) )
                {
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "You can't move that piece! \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **." ) ;
                    getch () ;
                    goto P1 ;
                }

                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf ( "Chosen piece: %s. ", pieces[1].name ) ;
                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "Possible moves: \n" ) ;

                movesExist = showvalidmovesforuser ( &stack, pieces[1],1 ) ;

                if ( movesExist==1 )
                {
                    printStr ( "\t", 9 ) ;
                    printf ( "R: " ) ;
                    scanf ( "%d", &r ) ;
                    printStr ( "\t", 9 ) ;
                    printf ( "C: " ) ;
                    scanf ( "%d", &c ) ;

                    if ( checkIfValidMove( &stack, r, c ) )
                    {
                        scorecounter ( r, c ) ;
                        movepiece ( 1, r, c, &black ) ;
                    }
                    else
                    {
                        printStr2 ( "\n", 2, "\t", 9 ) ;
                        printf ( "** Invalid move ** \n" ) ;
                        printStr2 ( "\n", 1, "\t", 9 ) ;
                        printf ( "** Press any key to continue **.\n" ) ;

                        emptyMoveStack ( &stack ) ;
                        getch() ;
                        goto P1 ;
                    }
                }

                else
                {
                    if ( (skipTurn(&stack,&white,1))==4 )
                        goto P2 ;
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "No move available. \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **.\n" ) ;

                    emptyMoveStack ( &stack ) ;
                    getch() ;
                    goto P1 ;
                }

                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "** Press any key to continue **.\n" ) ;

                emptyMoveStack ( &stack ) ;
                getch() ;
                break ;

            case 3:

                if ( !(pieceExists( &white, pieces[2].name )) )
                {
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "You can't move that piece! \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **." ) ;
                    getch () ;
                    goto P1 ;
                }

                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf ( "Chosen piece: %s. ", pieces[2].name ) ;
                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "Possible moves: \n" ) ;

                movesExist = showvalidmovesforuser ( &stack, pieces[2],1 ) ;

                if ( movesExist==1 )
                {
                    printStr ( "\t", 9 ) ;
                    printf ( "R: " ) ;
                    scanf ( "%d", &r ) ;
                    printStr ( "\t", 9 ) ;
                    printf ( "C: " ) ;
                    scanf ( "%d", &c ) ;

                    if ( checkIfValidMove( &stack, r, c ) )
                    {
                        scorecounter ( r, c ) ;
                        movepiece ( 2, r, c, &black ) ;
                    }
                    else
                    {
                        printStr2 ( "\n", 2, "\t", 9 ) ;
                        printf ( "** Invalid move **\n" ) ;
                        printStr2 ( "\n", 1, "\t", 9 ) ;
                        printf ( "** Press any key to continue **.\n" ) ;

                        emptyMoveStack ( &stack ) ;
                        getch() ;
                        goto P1 ;
                    }
                }

                else
                {
                    if ( (skipTurn(&stack,&white,1))==4 )
                        goto P2 ;
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "No move available. \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **.\n" ) ;
                    emptyMoveStack ( &stack ) ;
                    getch() ;
                    goto P1 ;
                }

                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "** Press any key to continue **.\n" ) ;

                emptyMoveStack ( &stack ) ;
                getch() ;
                break ;

            case 4:

                if ( !(pieceExists( &white, pieces[3].name )) )
                {
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "You can't move that piece! \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **." ) ;
                    getch () ;
                    goto P1 ;
                }

                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf ( "Chosen piece: %s. ", pieces[3].name ) ;
                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "Possible moves: \n" ) ;

                movesExist = showvalidmovesforuser ( &stack, pieces[3],1 ) ;

                if ( movesExist==1 )
                {
                    printStr ( "\t", 9 ) ;
                    printf ( "R: " ) ;
                    scanf ( "%d", &r ) ;
                    printStr ( "\t", 9 ) ;
                    printf ( "C: " ) ;
                    scanf ( "%d", &c ) ;

                    if ( checkIfValidMove( &stack, r, c ) )
                    {
                        if ( r==0 && promotedOrNot[1]==0 )
                        {
                            printStr2 ( "\n", 2, "\t", 9 ) ;
                            printf ( "Your pawn has been promoted to a rook! \n" ) ;
                            promote ( &pieces[3], &white ) ;
                            promotedOrNot[1]=1 ;
                        }
                        scorecounter ( r, c ) ;
                        movepiece ( 3, r, c, &black ) ;
                    }

                    else
                    {
                        printStr2 ( "\n", 2, "\t", 9 ) ;
                        printf ( "** Invalid move ** \n" ) ;
                        printStr2 ( "\n", 1, "\t", 9 ) ;
                        printf ( "** Press any key to continue **.\n" ) ;
                        emptyMoveStack ( &stack ) ;
                        getch() ;
                        goto P1 ;
                    }
                }

                else
                {
                    if ( (skipTurn(&stack,&white,1))==4 )
                        goto P2 ;
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "No move available. \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **.\n" ) ;

                    emptyMoveStack ( &stack ) ;
                    getch() ;
                    goto P1 ;
                }

                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "** Press any key to continue **.\n" ) ;

                emptyMoveStack ( &stack ) ;
                getch() ;
                break ;

            default:

                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf ( "** Invalid choice ** \n" ) ;
                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "** Press any key to continue **. " ) ;

                getch () ;
                goto P1 ;
        }

        if ( remainingCtr(&black)==0 )
            break ;

        // Label to mark starting of Player 2 (Black)'s turn
        P2:

        system ( "cls" ) ;

        displayboard () ;

        scoreboard () ;

        printStr2 ( "\n", 1, "\t", 9 ) ;
        printf ( "Team Black's turn " ) ;
        printStr3 ( "\n", 1, "\t", 9, "-", 19 ) ;
        printStr2 ( "\n", 1, "\t", 9 ) ;
        printf ( "Which piece to move? (Enter -1 to exit) \n" ) ;

        displayPiecesP2 () ;

        display_textbox () ;

        gotoxy(82,29) ;
        scanf ( "%d", &ans ) ;

        switch ( ans )
        {

            case -1:

                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf("Are you sure you want to forfeit? (Enter 1 for Yes And 2 for No): ");
                scanf("%d",&forfeit);

                if ( forfeit==1 )
                {
                    system ( "cls" ) ;
                    printStr2 ( "\n", 9, "\t", 9 ) ;
                    printf ( "Team White wins!\n\n\n\n" ) ;
                    exit(0) ;
                }

                else
                    goto P2 ;
                break ;

            case 1:

                if ( !(pieceExists( &black, pieces[4].name )) )
                {
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "You can't move that piece! \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "**Press any key to continue**." ) ;
                    getch () ;
                    goto P2 ;
                }

                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf ( "Chosen piece: %s. ", pieces[4].name ) ;
                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "Possible moves: \n" ) ;

                movesExist = showvalidmovesforuser ( &stack, pieces[4],1 ) ;

                if ( movesExist==1 )
                {
                    printStr ( "\t", 9 ) ;
                    printf ( "R: " ) ;
                    scanf ( "%d", &r ) ;
                    printStr ( "\t", 9 ) ;
                    printf ( "C: " ) ;
                    scanf ( "%d", &c ) ;

                    if ( checkIfValidMove( &stack, r, c ) )
                    {
                        if ( r==3 && promotedOrNot[2]==0 )
                        {
                            printStr2 ( "\n", 2, "\t", 9 ) ;
                            printf ( "Your pawn has been promoted to a rook! \n" ) ;
                            promote ( &pieces[4], &black ) ;
                            promotedOrNot[2]=1 ;
                        }
                        scorecounter ( r, c ) ;
                        movepiece ( 4, r, c, &white ) ;
                    }
                    else
                    {
                        printStr2 ( "\n", 2, "\t", 9 ) ;
                        printf ( "** Invalid move ** \n" ) ;
                        printStr2 ( "\n", 1, "\t", 9 ) ;
                        printf ( "** Press any key to continue **.\n" ) ;

                        emptyMoveStack ( &stack ) ;
                        getch() ;
                        goto P2 ;
                    }
                }
                else
                {
                    if ( (skipTurn(&stack,&black,0))==4 )
                        goto P1 ;

                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "No move available. \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **.\n" ) ;

                    emptyMoveStack ( &stack ) ;
                    getch() ;
                    goto P2 ;
                }

                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "**Press any key to continue**.\n" ) ;
                emptyMoveStack ( &stack ) ;
                getch() ;
                break ;

            case 2:

                if ( !(pieceExists( &black, pieces[5].name )) )
                {
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "You can't move that piece! \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **." ) ;
                    getch () ;
                    goto P2 ;
                }

                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf ( "Chosen piece: %s. ", pieces[5].name ) ;
                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "Possible moves: \n" ) ;

                movesExist = showvalidmovesforuser ( &stack, pieces[5], 1 ) ;

                if ( movesExist==1 )
                {
                    printStr ( "\t", 9 ) ;
                    printf ( "R: " ) ;
                    scanf ( "%d", &r ) ;
                    printStr ( "\t", 9 ) ;
                    printf ( "C: " ) ;
                    scanf ( "%d", &c ) ;

                    if ( checkIfValidMove( &stack, r, c ) )
                    {
                        if ( pieceExists(&black, pieces[5].name) )
                        {
                            scorecounter ( r, c ) ;
                            movepiece ( 5, r, c, &white ) ;
                        }
                        else
                        {
                            printStr2 ( "\n", 2, "\t", 9 ) ;
                            printf ( "You can't move that piece! \n" ) ;
                            printStr2 ( "\n", 1, "\t", 9 ) ;
                            printf ( "**Press any key to continue**.\n" ) ;
                            getch () ;
                            goto P2 ;
                        }
                    }
                    else
                    {
                        printStr2 ( "\n", 2, "\t", 9 ) ;
                        printf ( "** Invalid move ** \n" ) ;
                        printStr2 ( "\n", 1, "\t", 9 ) ;
                        printf ( "** Press any key to continue **.\n" ) ;

                        emptyMoveStack ( &stack ) ;
                        getch() ;
                        goto P2 ;
                    }
                }
                else
                {
                    if ( (skipTurn(&stack,&black,0))==4 )
                        goto P1 ;
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "No move available. \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **.\n" ) ;

                    emptyMoveStack ( &stack ) ;
                    getch() ;
                    goto P2 ;
                }

                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "** Press any key to continue **.\n" ) ;

                emptyMoveStack ( &stack ) ;
                getch() ;
                break ;

            case 3:

                if ( !(pieceExists( &black, pieces[6].name )) )
                {
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "You can't move that piece! \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **." ) ;
                    getch () ;
                    goto P2 ;
                }


                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf ( "Chosen piece: %s. ", pieces[6].name ) ;
                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "Possible moves: \n" ) ;

                movesExist = showvalidmovesforuser ( &stack, pieces[6],1 ) ;
                if ( movesExist==1 )
                {
                    printStr ( "\t", 9 ) ;
                    printf ( "R: " ) ;
                    scanf ( "%d", &r ) ;
                    printStr ( "\t", 9 ) ;
                    printf ( "C: " ) ;
                    scanf ( "%d", &c ) ;

                    if ( checkIfValidMove( &stack, r, c ) )
                    {
                        scorecounter ( r, c ) ;
                        movepiece ( 6, r, c, &white ) ;
                    }
                    else
                    {
                        printStr2 ( "\n", 2, "\t", 9 ) ;
                        printf ( "** Invalid move ** \n" ) ;
                        printStr2 ( "\n", 1, "\t", 9 ) ;
                        printf ( "** Press any key to continue **.\n" ) ;

                        emptyMoveStack ( &stack ) ;
                        getch() ;
                        goto P2 ;
                    }
                }
                else
                {
                    if ( (skipTurn(&stack,&black,0))==4 )
                        goto P1 ;
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "No move available. \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **.\n" ) ;

                    emptyMoveStack ( &stack ) ;
                    getch() ;
                    goto P2 ;
                }

                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "** Press any key to continue **.\n" ) ;

                emptyMoveStack ( &stack ) ;
                getch() ;
                break ;

            case 4:

                if ( !(pieceExists( &black, pieces[7].name )) )
                {
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "You can't move that piece! \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **." ) ;
                    getch () ;
                    goto P2 ;
                }

                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf ( "Chosen piece: %s. ", pieces[7].name ) ;
                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "Possible moves: \n" ) ;

                movesExist = showvalidmovesforuser ( &stack, pieces[7],1 ) ;

                if ( movesExist==1 )
                {
                    printStr ( "\t", 9 ) ;
                    printf ( "R: " ) ;
                    scanf ( "%d", &r ) ;
                    printStr ( "\t", 9 ) ;
                    printf ( "C: " ) ;
                    scanf ( "%d", &c ) ;

                    if ( checkIfValidMove( &stack, r, c ) )
                    {
                        if ( r==3 && promotedOrNot[3]==0 )
                        {
                            printStr2 ( "\n", 2, "\t", 9 ) ;
                            printf ( "Your pawn has been promoted to a rook! \n" ) ;
                            promote ( &pieces[7], &black ) ;
                            promotedOrNot[3]=1 ;
                        }
                        scorecounter ( r, c ) ;
                        movepiece ( 7, r, c, &white ) ;
                    }
                    else
                    {
                        printStr2 ( "\n", 2, "\t", 9 ) ;
                        printf ( "** Invalid move ** \n" ) ;
                        printStr2 ( "\n", 1, "\t", 9 ) ;
                        printf ( "** Press any key to continue **.\n" ) ;
                        emptyMoveStack ( &stack ) ;
                        getch() ;
                        goto P2 ;
                    }
                }
                else
                {
                    if ( (skipTurn(&stack,&black,0))==4 )
                        goto P1 ;
                    printStr2 ( "\n", 2, "\t", 9 ) ;
                    printf ( "No move available. \n" ) ;
                    printStr2 ( "\n", 1, "\t", 9 ) ;
                    printf ( "** Press any key to continue **.\n" ) ;

                    emptyMoveStack ( &stack ) ;
                    getch() ;
                    goto P2 ;
                }

                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "** Press any key to continue **.\n" ) ;

                emptyMoveStack ( &stack ) ;
                getch() ;
                break ;

            default:

                printStr2 ( "\n", 2, "\t", 9 ) ;
                printf ( "** Invalid choice ** \n" ) ;
                printStr2 ( "\n", 1, "\t", 9 ) ;
                printf ( "** Press any key to continue **. " ) ;
                getch () ;
                goto P2 ;
        }
        system ( "cls" ) ;
    }
    if ( stalemateCtr<24 )
    {
        system ( "cls" ) ;
        declareWinner ( &black, &white ) ;
        return ;
    }
    else if ( stalemateCtr>=12 && remainingCtr(&black)==1 || remainingCtr(&white)==1 )
    {
        system ( "cls" ) ;
        printStr2 ( "\n", 2, "\t", 9 ) ;
        printf ( "**** Stalemate Condition ****\n\n\n\n" ) ;
        declareWinner(&black, &white) ;
    }
    else
        goto P1 ;
}




