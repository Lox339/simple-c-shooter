using System;
using System.Runtime.InteropServices;

namespace SimpleShooter.UI
{
    public class PauseMenu
    {
        private int selectedOption;
        private bool isVisible;
        
        // Pause menu options
        private string[] pauseMenuOptions = {
            "Resume Game",
            "Settings",
            "Main Menu",
            "Quit Game"
        };
        
        // UI positioning
        private float menuX;
        private float menuY;
        private float menuWidth = 250;
        private float menuHeight = 300;
        private int windowWidth = 1024;
        private int windowHeight = 768;
        
        // Animation
        private float fadeAlpha = 0.0f;
        private float targetAlpha = 0.8f;
        private float fadeSpeed = 3.0f;
        
        // Import game control functions
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void set_game_phase(int phase);
        
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void quit_game();
        
        public PauseMenu()
        {
            selectedOption = 0;
            isVisible = false;
        }
        
        public bool Initialize(int width, int height)
        {
            windowWidth = width;
            windowHeight = height;
            
            // Center the menu
            menuX = (windowWidth - menuWidth) / 2;
            menuY = (windowHeight - menuHeight) / 2;
            
            Console.WriteLine($"Pause Menu initialized at ({menuX}, {menuY})");
            return true;
        }
        
        public void Update(float deltaTime)
        {
            if (!isVisible) return;
            
            // Fade in animation
            if (fadeAlpha < targetAlpha)
            {
                fadeAlpha = Math.Min(targetAlpha, fadeAlpha + fadeSpeed * deltaTime);
            }
        }
        
        public void Render()
        {
            if (!isVisible) return;
            
            try
            {
                RenderBackground();
                RenderTitle();
                RenderMenuOptions();
                RenderInstructions();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error rendering pause menu: {ex.Message}");
            }
        }
        
        private void RenderBackground()
        {
            // Semi-transparent dark overlay
            RenderUIBackground(0, 0, windowWidth, windowHeight, 0.0f, 0.0f, 0.0f, fadeAlpha * 0.7f);
            
            // Menu panel background
            RenderUIBackground(menuX - 20, menuY - 20, menuWidth + 40, menuHeight + 40, 
                             0.1f, 0.1f, 0.2f, fadeAlpha);
        }
        
        private void RenderTitle()
        {
            float titleX = menuX + menuWidth / 2 - 40;
            float titleY = menuY + 30;
            
            RenderText("PAUSED", titleX, titleY, 1.0f, 1.0f, 0.2f);
        }
        
        private void RenderMenuOptions()
        {
            float optionY = menuY + 80;
            float optionSpacing = 40;
            
            for (int i = 0; i < pauseMenuOptions.Length; i++)
            {
                float currentY = optionY + i * optionSpacing;
                
                // Highlight selected option
                if (i == selectedOption)
                {
                    // Selection background
                    RenderUIBackground(menuX + 10, currentY - 5, menuWidth - 20, 30, 
                                     0.3f, 0.3f, 0.5f, 0.7f);
                    
                    // Selected option text (brighter)
                    RenderText($"> {pauseMenuOptions[i]} <", menuX + 15, currentY, 
                              1.0f, 1.0f, 0.2f);
                }
                else
                {
                    // Normal option text
                    RenderText(pauseMenuOptions[i], menuX + 25, currentY, 
                              0.8f, 0.8f, 0.8f);
                }
            }
        }
        
        private void RenderInstructions()
        {
            float instructY = menuY + menuHeight - 50;
            
            RenderText("W/S - Navigate", menuX + 20, instructY, 0.6f, 0.6f, 0.6f);
            RenderText("ENTER - Select", menuX + 20, instructY + 15, 0.6f, 0.6f, 0.6f);
            RenderText("ESC - Resume", menuX + 20, instructY + 30, 0.6f, 0.6f, 0.6f);
        }
        
        public void HandleInput(int key, int action)
        {
            if (!isVisible || action == 0) return; // Only handle key press
            
            try
            {
                switch (key)
                {
                    case 'w': // Up
                    case 'W':
                        selectedOption = (selectedOption - 1 + pauseMenuOptions.Length) % pauseMenuOptions.Length;
                        Console.WriteLine($"Pause menu selection: {pauseMenuOptions[selectedOption]}");
                        break;
                        
                    case 's': // Down
                    case 'S':
                        selectedOption = (selectedOption + 1) % pauseMenuOptions.Length;
                        Console.WriteLine($"Pause menu selection: {pauseMenuOptions[selectedOption]}");
                        break;
                        
                    case 13: // Enter
                    case ' ': // Space
                        ExecuteMenuOption(selectedOption);
                        break;
                        
                    case 27: // ESC - Resume game
                        ResumeGame();
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error handling pause menu input: {ex.Message}");
            }
        }
        
        private void ExecuteMenuOption(int option)
        {
            switch (option)
            {
                case 0: // Resume Game
                    ResumeGame();
                    break;
                    
                case 1: // Settings
                    OpenSettings();
                    break;
                    
                case 2: // Main Menu
                    ReturnToMainMenu();
                    break;
                    
                case 3: // Quit Game
                    QuitGame();
                    break;
            }
        }
        
        private void ResumeGame()
        {
            Console.WriteLine("Resuming game...");
            try
            {
                set_game_phase(1); // GAME_PLAYING
                Hide();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error resuming game: {ex.Message}");
            }
        }
        
        private void OpenSettings()
        {
            Console.WriteLine("Opening settings from pause menu...");
            // Settings will be handled by the main UI manager
        }
        
        private void ReturnToMainMenu()
        {
            Console.WriteLine("Returning to main menu...");
            try
            {
                set_game_phase(0); // GAME_MENU
                Hide();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error returning to main menu: {ex.Message}");
            }
        }
        
        private void QuitGame()
        {
            Console.WriteLine("Quitting game from pause menu...");
            try
            {
                quit_game();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error quitting game: {ex.Message}");
            }
        }
        
        public void Show()
        {
            isVisible = true;
            selectedOption = 0;
            fadeAlpha = 0.0f;
            Console.WriteLine("Pause menu shown");
        }
        
        public void Hide()
        {
            isVisible = false;
            fadeAlpha = 0.0f;
            Console.WriteLine("Pause menu hidden");
        }
        
        public void SetWindowSize(int width, int height)
        {
            windowWidth = width;
            windowHeight = height;
            menuX = (windowWidth - menuWidth) / 2;
            menuY = (windowHeight - menuHeight) / 2;
        }
        
        public void Cleanup()
        {
            Console.WriteLine("Pause menu cleaned up");
        }
        
        // Properties
        public bool IsVisible => isVisible;
        public int SelectedOption => selectedOption;
        
        // External rendering functions
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void render_text_opengl([MarshalAs(UnmanagedType.LPStr)] string text, 
                                                     float x, float y, float r, float g, float b);
        
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void render_ui_background_opengl(float x, float y, float width, float height, 
                                                             float r, float g, float b, float a);
        
        private void RenderText(string text, float x, float y, float r, float g, float b)
        {
            try
            {
                render_text_opengl(text, x, y, r, g, b);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error rendering text: {ex.Message}");
            }
        }
        
        private void RenderUIBackground(float x, float y, float width, float height, 
                                      float r, float g, float b, float a)
        {
            try
            {
                render_ui_background_opengl(x, y, width, height, r, g, b, a);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error rendering UI background: {ex.Message}");
            }
        }
    }
}