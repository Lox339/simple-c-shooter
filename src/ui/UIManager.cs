// UI Manager - C# Implementation
// Advanced UI system with OpenGL integration

using System;
using System.Runtime.InteropServices;
using System.Drawing;

namespace SimpleShooter.UI
{
    // Game state structure matching C definition
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float x, y, z;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PlayerState
    {
        public Vector3 position;
        public Vector3 velocity;
        public Vector3 rotation;
        public float speed;
        public float max_speed;
        public int on_ground;
        public float jump_time;
        public int health;
        public int max_health;
        public int ammo;
        public int max_ammo;
        public float horizontal_speed;
        public float last_jump_time;
        public int consecutive_jumps;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct GameState
    {
        public PlayerState player;
        // Note: Arrays in structs need special handling for marshaling
        // For now, we'll get individual values through separate functions
        public int score;
        public int enemy_count;
        public int projectile_count;
        public float delta_time;
        public int game_running;
        public int current_phase;
    }

    public class UIManager
    {
        private Speedometer speedometer;
        private GameHUD gameHUD;
        private MainMenu mainMenu;
        private SettingsMenu settingsMenu;
        private PauseMenu pauseMenu;
        private AudioSettings audioSettings;
        
        private bool initialized = false;
        private int windowWidth = 1024;
        private int windowHeight = 768;

        // Import functions from C/C++ bridge
        [DllImport("simple_shooter", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr get_game_state();

        [DllImport("simple_shooter", CallingConvention = CallingConvention.Cdecl)]
        private static extern int get_window_width();

        [DllImport("simple_shooter", CallingConvention = CallingConvention.Cdecl)]
        private static extern int get_window_height();

        public bool Initialize()
        {
            Console.WriteLine("Initializing UI Manager...");

            try
            {
                // Get window dimensions
                windowWidth = Math.Max(get_window_width(), 800);
                windowHeight = Math.Max(get_window_height(), 600);
                
                Console.WriteLine($"UI Window size: {windowWidth}x{windowHeight}");

                // Initialize UI components
                speedometer = new Speedometer();
                if (!speedometer.Initialize(windowWidth, windowHeight))
                {
                    Console.WriteLine("Failed to initialize speedometer");
                    return false;
                }

                gameHUD = new GameHUD();
                if (!gameHUD.Initialize(windowWidth, windowHeight))
                {
                    Console.WriteLine("Failed to initialize game HUD");
                    return false;
                }

                mainMenu = new MainMenu();
                if (!mainMenu.Initialize(windowWidth, windowHeight))
                {
                    Console.WriteLine("Failed to initialize main menu");
                    return false;
                }

                settingsMenu = new SettingsMenu();
                if (!settingsMenu.Initialize(windowWidth, windowHeight))
                {
                    Console.WriteLine("Failed to initialize settings menu");
                    return false;
                }

                pauseMenu = new PauseMenu();
                if (!pauseMenu.Initialize(windowWidth, windowHeight))
                {
                    Console.WriteLine("Failed to initialize pause menu");
                    return false;
                }

                audioSettings = new AudioSettings();
                Console.WriteLine("Audio settings initialized");

                initialized = true;
                Console.WriteLine("UI Manager initialized successfully");
                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"UI Manager initialization failed: {ex.Message}");
                return false;
            }
        }

        public void Update(float deltaTime)
        {
            if (!initialized) return;

            try
            {
                // Get current game state
                GameState gameState = GetGameState();
                
                // Update UI components based on game phase
                switch (gameState.current_phase)
                {
                    case 0: // GAME_MENU
                        mainMenu?.Update(deltaTime);
                        if (settingsMenu?.IsVisible == true)
                            settingsMenu?.Update(deltaTime);
                        break;
                        
                    case 1: // GAME_PLAYING
                        speedometer?.Update(gameState.player.speed, gameState.player.on_ground != 0);
                        gameHUD?.Update(gameState.player, gameState.score, deltaTime);
                        audioSettings?.Update(deltaTime);
                        break;
                        
                    case 2: // GAME_PAUSED
                        // Keep HUD visible but show pause menu
                        gameHUD?.Update(gameState.player, gameState.score, deltaTime);
                        pauseMenu?.Update(deltaTime);
                        if (settingsMenu?.IsVisible == true)
                            settingsMenu?.Update(deltaTime);
                        break;
                        
                    case 3: // GAME_OVER
                        gameHUD?.ShowGameOver(gameState.score);
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"UI Update error: {ex.Message}");
            }
        }

        public void Render()
        {
            if (!initialized) return;

            try
            {
                GameState gameState = GetGameState();
                
                // Render UI components based on game phase
                switch (gameState.current_phase)
                {
                    case 0: // GAME_MENU
                        mainMenu?.Render();
                        if (settingsMenu?.IsVisible == true)
                            settingsMenu?.Render();
                        break;
                        
                    case 1: // GAME_PLAYING
                        gameHUD?.Render();
                        speedometer?.Render();
                        audioSettings?.Render();
                        break;
                        
                    case 2: // GAME_PAUSED
                        gameHUD?.Render();
                        speedometer?.Render();
                        pauseMenu?.Render();
                        if (settingsMenu?.IsVisible == true)
                            settingsMenu?.Render();
                        break;
                        
                    case 3: // GAME_OVER
                        gameHUD?.Render();
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"UI Render error: {ex.Message}");
            }
        }

        public void HandleInput(int key, int action)
        {
            if (!initialized) return;

            try
            {
                GameState gameState = GetGameState();
                
                // Route input to appropriate UI component
                switch (gameState.current_phase)
                {
                    case 0: // GAME_MENU
                        if (settingsMenu?.IsVisible == true)
                            settingsMenu?.HandleInput(key, action);
                        else
                            mainMenu?.HandleInput(key, action);
                        break;
                        
                    case 2: // GAME_PAUSED
                        if (settingsMenu?.IsVisible == true)
                            settingsMenu?.HandleInput(key, action);
                        else
                            pauseMenu?.HandleInput(key, action);
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"UI Input error: {ex.Message}");
            }
        }

        private GameState GetGameState()
        {
            try
            {
                IntPtr statePtr = get_game_state();
                if (statePtr != IntPtr.Zero)
                {
                    return Marshal.PtrToStructure<GameState>(statePtr);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Failed to get game state: {ex.Message}");
            }
            
            // Return default state if failed
            return new GameState();
        }

        public void SetWindowSize(int width, int height)
        {
            if (width > 0 && height > 0)
            {
                windowWidth = width;
                windowHeight = height;
                
                // Update all UI components with new size
                speedometer?.SetWindowSize(width, height);
                gameHUD?.SetWindowSize(width, height);
                mainMenu?.SetWindowSize(width, height);
                settingsMenu?.SetWindowSize(width, height);
                pauseMenu?.SetWindowSize(width, height);
                
                Console.WriteLine($"UI window size updated: {width}x{height}");
            }
        }

        public void Cleanup()
        {
            if (!initialized) return;

            Console.WriteLine("Cleaning up UI Manager...");

            try
            {
                speedometer?.Cleanup();
                gameHUD?.Cleanup();
                mainMenu?.Cleanup();
                settingsMenu?.Cleanup();
                pauseMenu?.Cleanup();
                audioSettings = null;

                speedometer = null;
                gameHUD = null;
                mainMenu = null;
                settingsMenu = null;
                pauseMenu = null;

                initialized = false;
                Console.WriteLine("UI Manager cleaned up");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"UI Cleanup error: {ex.Message}");
            }
        }

        // Public getters for UI components
        public Speedometer GetSpeedometer() => speedometer;
        public GameHUD GetGameHUD() => gameHUD;
        public MainMenu GetMainMenu() => mainMenu;
        public SettingsMenu GetSettingsMenu() => settingsMenu;
        
        public bool IsInitialized => initialized;
        public int WindowWidth => windowWidth;
        public int WindowHeight => windowHeight;
    }
}