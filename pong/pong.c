#include <stdio.h>
#include "raylib.h"

const int GAME_WIN = 5; // target score to win the game

// structure data set to create player paddle rectangle
struct PlayerPaddle
{
	float width;
	float height;
	float x;
	float y;
};

// structure data set to create CPU paddle rectangle
struct CPUPaddle
{
	float width;
	float height;
	float x;
	float y;
	float cpu_speed;
};

// structure used to hold data for game's ball
struct Ball
{
	float x;
	float y;
	float r;
	float x_speed;
	float y_speed;
};

int main()
{
	/*   Variable Section   */

	// Window Size Variables
	const int screen_width = 1920;
	const int screen_height = 1080;

	// Initializing struct objects
	struct PlayerPaddle player = { 50.0, 200.0, 75.0, ((screen_height / 2) - (200.0/2)) };
	struct CPUPaddle cpu = { 50.0, 200.0, (screen_width - (75.0 * 1.5)), ((screen_height / 2) - (200.0 / 2)), 600.0 };
	struct Ball game_ball = { (screen_width / 2), (screen_height / 2), 15.0, 1000.0, 960.0 };
	
	// Setting variables to track game score and result music
	int player_score = 0;
	int cpu_score = 0;
	bool win_fx = false;
	bool lose_fx = false;

	// Initializing our Audio Device
	InitAudioDevice();

	// Setting sound FX and music
	Sound player_hit = LoadSound("audio/player_hit.mp3");
	Sound cpu_hit = LoadSound("audio/cpu_hit.mp3");
	Music win_game = LoadMusicStream("audio/winning_music.mp3");
	Music lose_game = LoadMusicStream("audio/losing.mp3");

	// Setting up window along with frames
	InitWindow(screen_width, screen_height, "PONG!");
	SetTargetFPS(120);
	float dt = 0.0;

	/*   Main Game Loop   */

	while (!WindowShouldClose())
	{
		
		dt = GetFrameTime();

		/*   User Input   */
		if (IsKeyDown(KEY_W) && player.y >= 0)  // Allow user to move their paddle up with 'W' key
		{
			player.y -= 840 * dt;
		}

		if (IsKeyDown(KEY_S) && player.y <= (screen_height - player.height)) // Allow user to move paddle down with 'S' key
		{
			player.y += 840 * dt;
		}

		/*   Update Game Objects   */
		
		// Now, we create a vector2 and rectangle to allow collision between the paddles and ball
		Vector2 ball_vector = { game_ball.x, game_ball.y };
		Rectangle player_rect = { player.x, player.y, player.width, player.height };
		Rectangle cpu_rect = { cpu.x, cpu.y, cpu.width, cpu.height };
		
		// Checks to see if ball collides with top and bottom of screen
		if (game_ball.y <= 0 || game_ball.y >= screen_height)
		{
			game_ball.y_speed *= -1;
		}

		// Check to see if ball and paddle collides with either side
		if (CheckCollisionCircleRec(ball_vector, game_ball.r, player_rect))
		{
			PlaySound(player_hit);
			game_ball.x_speed *= -1;
		}
		if (CheckCollisionCircleRec(ball_vector, game_ball.r, cpu_rect))
		{
			PlaySound(cpu_hit);
			game_ball.x_speed *= -1;
		}

		// Establish a simple AI for the opponent paddle to move with
		if (game_ball.y < (cpu.y + (cpu.height/2)) && cpu.y >= 0)
			cpu.y -= cpu.cpu_speed * dt;

		if (game_ball.y > (cpu.y + (cpu.height / 2)) && cpu.y <= (screen_height - cpu.height))
			cpu.y += cpu.cpu_speed * dt;
		

		/*   Begin Drawings and Update Music Buffer   */

		UpdateMusicStream(win_game);
		UpdateMusicStream(lose_game);
		
		BeginDrawing();
		ClearBackground(BLACK);

		// Check conditionals to see if player or CPU reaches point maximum
		if (player_score < GAME_WIN && cpu_score < GAME_WIN)
		{
			// Keep ball moving while game is going
			game_ball.x += (game_ball.x_speed) * dt;
			game_ball.y += (game_ball.y_speed) * dt;

			if (game_ball.x <= 0)  // Update score if CPU wins point
			{
				cpu_score += 1;
				game_ball.x = (screen_width / 2);
				game_ball.y = (screen_height / 2);
			}

			if (game_ball.x >= screen_width) // Update score if player wins point
			{
				player_score += 1;
				game_ball.x = (screen_width / 2);
				game_ball.y = (screen_height / 2);
			}

			// Update Paddles and Ball locations
			DrawRectangle(player.x, player.y, player.width, player.height, WHITE);
			DrawRectangle(cpu.x, cpu.y, cpu.width, cpu.height, WHITE);
			DrawCircle(game_ball.x, game_ball.y, game_ball.r, WHITE);

		}
		
		// If player wins game...
		else if (player_score == GAME_WIN)
		{
			if (!win_fx)  // Winning music is played
			{
				PlayMusicStream(win_game);
				win_fx = true;
			}

			// Update drawings for winning screen
			DrawText("YOU WON!", ((screen_width / 2) - 450), ((screen_height / 2) - 200), 200, YELLOW);
			DrawText("Press Spacebar to play again.", 200, ((screen_height / 2) + 150), 100, YELLOW);
			
			// Reset game if player chooses to
			if (IsKeyPressed(KEY_SPACE))
			{
				StopMusicStream(win_game);
				player_score = 0;
				cpu_score = 0;
				game_ball.x = (screen_width / 2);
				game_ball.y = (screen_height / 2);
				win_fx = false;
			}		
		}

		// If CPU wins game...
		else if (cpu_score == GAME_WIN)
		{
			if (!lose_fx)  // Losing music is played
			{
				PlayMusicStream(lose_game);
				lose_fx = true;
			}

			// Update drawings for losing screen
			DrawText("YOU LOST!", ((screen_width / 2) - 500), ((screen_height / 2) - 200), 200, YELLOW);
			DrawText("Press Spacebar to play again.", 200, ((screen_height / 2) + 150), 100, YELLOW);
			
			// Reset game if player chooses to
			if (IsKeyPressed(KEY_SPACE))
			{
				StopMusicStream(lose_game);
				player_score = 0;
				cpu_score = 0;
				game_ball.x = (screen_width / 2);
				game_ball.y = (screen_height / 2);
				lose_fx = false;
			}
		}


		// Draw the arena for game to take place
		DrawRectangleLines(25, 25, (screen_width - 50), (screen_height - 50), YELLOW);
		if (win_fx == false && lose_fx == false)
			DrawLine((screen_width / 2), 0, (screen_width / 2), screen_height, YELLOW);
		DrawText(TextFormat("%d", player_score), ((screen_width/2) - 125), 75, 100, YELLOW);
		DrawText(TextFormat("%d", cpu_score), ((screen_width / 2) + 75), 75, 100, YELLOW);
		
		// End Drawing to prevent tearing and drag
		EndDrawing();
	}

	/*   Unloading Stage   */

	UnloadSound(player_hit);
	UnloadSound(cpu_hit);
	UnloadMusicStream(win_game);
	UnloadMusicStream(lose_game);
	
	/*   Close Devices   */
	CloseAudioDevice();
	CloseWindow();

	return 0;
}
