#include "ttt.hpp"

int main(int argc, char *argv[])
{
    // Check if the correct number of command-line arguments is provided
    if (argc != 2)
    {
        std::cout << "Error" << std::endl;
        return 1;
    }

    // Check if the provided input string is valid
    if (!isValid(argv[1]))
    {
        std::cout << "Error" << std::endl;
        return 1;
    }

    // Initialize the game board, user, computer, and winner variables
    int board[3][3] = {{0, 0, 0},
                       {0, 0, 0},
                       {0, 0, 0}};
    int user = 1;
    int computer = 2;
    int winner;

    // Extract computer's steps from the command-line argument
    std::string computerSteps = argv[1];
    int currentComputerStep = 0;
    int currentPlayer = 0; // 0 for computer, 1 for user

    // Main game loop
    while (!gameOver(board, &winner))
    {
        if (currentPlayer == 0) // Computer's turn
        {
            // Get the position of the computer's move
            int position = computerSteps[currentComputerStep] - '0';
            std::cout << "The choice of the computer is: ";
            std::cout << position << std::endl;

            // Make the computer's move and check if it's successful
            bool change = putBoardPosition(position, board, computer);
            currentComputerStep += 1;

            // Switch to the user's turn if the move was successful
            if (change == true)
            {
                currentPlayer = 1;
            }
        }
        else // User's turn
        {
            std::cout << "Enter the location that you want to play on the board" << std::endl;
            int location;
            std::cin >> location;

            // Make the user's move and switch to the computer's turn if the move was successful
            bool change = putBoardPosition(location, board, user);
            if (change == true)
            {
                currentPlayer = 0;
            }
        }
    }

    // Display the result of the game
    switch (winner)
    {
    case 0:
        std::cout << "Draw" << std::endl;
        break;
    case 1:
        std::cout << "I lost" << std::endl;
        break;
    case 2:
        std::cout << "I win" << std::endl;
        break;
    }

    return 0;
}

// Function to check if the input string is valid
bool isValid(const char *str)
{
    // Check if the length of the input string is 9
    int len = strlen(str);
    if (len != 9)
    {
        return false;
    }

    // Check if each character is a digit from 1 to 9 and if each digit appears only once
    int digits[9] = {0};
    for (int i = 0; i < 9; i++)
    {
        if (str[i] < '1' || str[i] > '9')
        {
            return false;
        }

        int index = str[i] - '1';
        if (digits[index] == 1)
        {
            return false;
        }
        digits[index] = 1;
    }

    // Return true if all checks pass
    return true;
}

// Function to check if the game is over
bool gameOver(int arr[3][3], int *winner)
{
    // Check rows and columns for a win
    for (size_t i = 0; i < 3; i++)
    {
        if (arr[0][i] == arr[1][i] && arr[1][i] == arr[2][i] && arr[0][i] != 0)
        {
            *winner = arr[0][i];
            return true;
        }

        if (arr[i][0] == arr[i][1] && arr[i][1] == arr[i][2] && arr[i][0] != 0)
        {
            *winner = arr[i][0];
            return true;
        }
    }

    // Check diagonals for a win
    if (arr[0][0] == arr[1][1] && arr[1][1] == arr[2][2] && arr[0][0] != 0)
    {
        *winner = arr[0][0];
        return true;
    }

    if (arr[0][2] == arr[1][1] && arr[1][1] == arr[2][0] && arr[0][2] != 0)
    {
        *winner = arr[0][2];
        return true;
    }

    // Check for a draw
    bool draw = true;
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            if (arr[i][j] == 0)
            {
                draw = false;
                break;
            }
        }
    }
    if (draw)
    {
        *winner = 0;
        return true;
    }

    // Return false if the game is not over
    return false;
}

// Function to make a move on the board
bool putBoardPosition(int number, int arr[][3], int player)
{
    // Adjust for 0-based indexing
    number -= 1;
    int row = number / 3;
    int col = number % 3;

    // Check if the position is available
    if (arr[row][col] == 0)
    {
        // Make the move and print the updated board
        arr[row][col] = player;
        for (int i = 0; i < 3; i++)
        {
            std::cout << "\n";
            for (int j = 0; j < 3; j++)
            {
                std::cout << arr[i][j] << " ";
            }
        }
        std::cout << std::endl
                  << std::endl;
        return true; // Return true to indicate a successful move
    }
    else
    {
        // Print an error message if the position is already taken
        std::cerr << "Error: Position already taken\n"
                  << std::endl;
        return false; // Return false to indicate an unsuccessful move
    }
}
