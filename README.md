# Boop AI Arena
Boop AI Arena is an extension on the final project for a class. The AI Arena is a program created to allow anyone to create there own AI for the game Boop (a tic tac toe like game), and have them fight against other AI.

## Prerequisites
- Make
- g++

## Existing AI
Here is a list of the existing AI's available in this project, the motivations behind these AI's can be found in their source file.

1. Huaman_AI (allows humans to play against AI)
2. Template_AI
3. Eval_AI
4. Random_AI
5. Winning_AI
6. Boopy_AI
7. Minimax_Alpha_Beta_AI
8. Boopy_Alpha_Beta_AI

## Creating your first AI
Follow the steps below to begin creating your first AI class
1. Create a copy of `Template_AI.h` in the AI folder, and name your AI
2. With your favorite text editor, replace all mentions of 'Template_AI' with the name of your AI
3. Create a summary of your AI's strategy or goal.
4. Start creating your AI!
5. Go to main.cc, include your new AI, and set it as either P1 or P2
6. Compile the project with `make` and run the project.

> [!NOTE]
> *While there is a Timer to limit how long your AI runs for, it does not need to be implented and will run without it.*
