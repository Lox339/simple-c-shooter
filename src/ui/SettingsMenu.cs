using System;
using System.Runtime.InteropServices;

namespace SimpleShooter.UI
{
    public class SettingsMenu
    {
        public enum SettingsCategory
        {
            Graphics,
            Audio,
            Controls,
            Gameplay
        }
        
        private SettingsCategory currentCategory;
        private int selectedOption;
        private bool isVisible;
        
        // Settings categories
        private string[] categories = {
            "Graphics",
            "Audio", 
            "Controls",
            "Gameplay"
        };
        
        // Graphics settings
        private string[] resolutions = { "800x600", "1024x768", "1280x720", "1920x1080" };
        private int currentResolution = 1;
        private bool fullscreen = false;
        private int graphicsQuality = 1; // 0=Low, 1=Medium, 2=High
        
        // Audio settings (integrated with AudioSettings)
        private AudioSettings audioSettings;
        
        // Control settings
        private float mouseSensitivity = 2.0f;
        private bool invertMouse = false;
        
        // Gameplay settings
        private int difficulty = 1; // 0=Easy, 1=Normal, 2=Hard
        private bool showFPS = true;
        private bool autoReload = true;
        
        // UI positioning
        private float menuX;
        private float menuY;
        private float menuWidth = 500;
        private float menuHeight = 400;
        private int windowWidth = 1024;
        private int windowHeight = 768;
        
        // Import functions
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void set_mouse_sensitivity(float sensitivity);
        
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern float get_mouse_sensitivity();
        
        public SettingsMenu()
        {
            currentCategory = SettingsCategory.Graphics;
            selectedOption = 0;
            isVisible = false;
            audioSettings = new AudioSettings();
        }
        
        public bool Initialize(int width, int height)
        {
            windowWidth = width;
            windowHeight = height;
            
            // Center the menu
            menuX = (windowWidth - menuWidth) / 2;
            menuY = (windowHeight - menuHeight) / 2;
            
            // Load current settings
            LoadCurrentSettings();
            
            Console.WriteLine($"Settings Menu initialized at ({menuX}, {menuY})");
            return true;
        }
        
        private void LoadCurrentSettings()
        {
            try
            {
                mouseSensitivity = get_mouse_sensitivity();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error loading settings: {ex.Message}");
            }
        }
        
        public void Update(float deltaTime)
        {
            if (!isVisible) return;
            
            audioSettings?.Update(deltaTime);
        }
        
        public void Render()
        {
            if (!isVisible) return;
            
            try
            {
                RenderBackground();
                RenderTitle();
                RenderCategories();
                RenderCurrentCategorySettings();
                RenderInstructions();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error rendering settings menu: {ex.Message}");
            }
        }
        
        private void RenderBackground()
        {
            // Semi-transparent dark background
            RenderUIBackground(0, 0, windowWidth, windowHeight, 0.0f, 0.0f, 0.1f, 0.8f);
            
            // Menu panel background
            RenderUIBackground(menuX - 20, menuY - 20, menuWidth + 40, menuHeight + 40, 
                             0.1f, 0.1f, 0.2f, 0.9f);
        }
        
        private void RenderTitle()
        {
            float titleX = menuX + menuWidth / 2 - 60;
            float titleY = menuY + 20;
            
            RenderText("SETTINGS", titleX, titleY, 1.0f, 1.0f, 0.2f);
        }
        
        private void RenderCategories()
        {
            float categoryY = menuY + 60;
            float categorySpacing = 80;
            
            for (int i = 0; i < categories.Length; i++)
            {
                float currentX = menuX + 20 + i * categorySpacing;
                
                if ((int)currentCategory == i)
                {
                    // Highlight current category
                    RenderUIBackground(currentX - 5, categoryY - 5, 70, 25, 
                                     0.3f, 0.3f, 0.5f, 0.7f);
                    RenderText(categories[i], currentX, categoryY, 1.0f, 1.0f, 0.2f);
                }
                else
                {
                    RenderText(categories[i], currentX, categoryY, 0.7f, 0.7f, 0.7f);
                }
            }
        }
        
        private void RenderCurrentCategorySettings()
        {
            float settingsY = menuY + 110;
            
            switch (currentCategory)
            {
                case SettingsCategory.Graphics:
                    RenderGraphicsSettings(settingsY);
                    break;
                case SettingsCategory.Audio:
                    RenderAudioSettings(settingsY);
                    break;
                case SettingsCategory.Controls:
                    RenderControlsSettings(settingsY);
                    break;
                case SettingsCategory.Gameplay:
                    RenderGameplaySettings(settingsY);
                    break;
            }
        }
        
        private void RenderGraphicsSettings(float startY)
        {
            float optionSpacing = 35;
            int option = 0;
            
            // Resolution
            RenderSettingOption("Resolution:", resolutions[currentResolution], 
                              startY + option * optionSpacing, option == selectedOption);
            option++;
            
            // Fullscreen
            RenderSettingOption("Fullscreen:", fullscreen ? "Yes" : "No", 
                              startY + option * optionSpacing, option == selectedOption);
            option++;
            
            // Graphics Quality
            string[] qualityNames = { "Low", "Medium", "High" };
            RenderSettingOption("Quality:", qualityNames[graphicsQuality], 
                              startY + option * optionSpacing, option == selectedOption);
        }
        
        private void RenderAudioSettings(float startY)
        {
            RenderText("Audio settings are managed by the Audio Settings panel", 
                      menuX + 20, startY, 0.8f, 0.8f, 0.8f);
            RenderText("Press 'O' in game to open audio settings", 
                      menuX + 20, startY + 25, 0.6f, 0.6f, 0.6f);
        }
        
        private void RenderControlsSettings(float startY)
        {
            float optionSpacing = 35;
            int option = 0;
            
            // Mouse Sensitivity
            RenderSettingOption("Mouse Sensitivity:", $"{mouseSensitivity:F1}", 
                              startY + option * optionSpacing, option == selectedOption);
            option++;
            
            // Invert Mouse
            RenderSettingOption("Invert Mouse:", invertMouse ? "Yes" : "No", 
                              startY + option * optionSpacing, option == selectedOption);
        }
        
        private void RenderGameplaySettings(float startY)
        {
            float optionSpacing = 35;
            int option = 0;
            
            // Difficulty
            string[] difficultyNames = { "Easy", "Normal", "Hard" };
            RenderSettingOption("Difficulty:", difficultyNames[difficulty], 
                              startY + option * optionSpacing, option == selectedOption);
            option++;
            
            // Show FPS
            RenderSettingOption("Show FPS:", showFPS ? "Yes" : "No", 
                              startY + option * optionSpacing, option == selectedOption);
            option++;
            
            // Auto Reload
            RenderSettingOption("Auto Reload:", autoReload ? "Yes" : "No", 
                              startY + option * optionSpacing, option == selectedOption);
        }
        
        private void RenderSettingOption(string label, string value, float y, bool selected)
        {
            if (selected)
            {
                RenderUIBackground(menuX + 15, y - 5, menuWidth - 30, 25, 
                                 0.3f, 0.3f, 0.5f, 0.7f);
                RenderText($"> {label}", menuX + 20, y, 1.0f, 1.0f, 0.2f);
                RenderText(value, menuX + 200, y, 0.2f, 1.0f, 0.2f);
            }
            else
            {
                RenderText(label, menuX + 30, y, 0.8f, 0.8f, 0.8f);
                RenderText(value, menuX + 200, y, 0.6f, 0.8f, 0.6f);
            }
        }
        
        private void RenderInstructions()
        {
            float instructY = menuY + menuHeight - 60;
            
            RenderText("A/D - Change Category  |  W/S - Navigate  |  Left/Right - Change Value", 
                      menuX + 20, instructY, 0.6f, 0.6f, 0.6f);
            RenderText("ENTER - Apply  |  ESC - Back to Menu", 
                      menuX + 20, instructY + 20, 0.6f, 0.6f, 0.6f);
        }
        
        public void HandleInput(int key, int action)
        {
            if (!isVisible || action == 0) return;
            
            try
            {
                switch (key)
                {
                    case 'a': // Previous category
                    case 'A':
                        currentCategory = (SettingsCategory)(((int)currentCategory - 1 + categories.Length) % categories.Length);
                        selectedOption = 0;
                        Console.WriteLine($"Settings category: {currentCategory}");
                        break;
                        
                    case 'd': // Next category
                    case 'D':
                        currentCategory = (SettingsCategory)(((int)currentCategory + 1) % categories.Length);
                        selectedOption = 0;
                        Console.WriteLine($"Settings category: {currentCategory}");
                        break;
                        
                    case 'w': // Previous option
                    case 'W':
                        selectedOption = Math.Max(0, selectedOption - 1);
                        break;
                        
                    case 's': // Next option
                    case 'S':
                        selectedOption = Math.Min(GetMaxOptionForCategory() - 1, selectedOption + 1);
                        break;
                        
                    case 37: // Left arrow - decrease value
                        ChangeSettingValue(-1);
                        break;
                        
                    case 39: // Right arrow - increase value
                        ChangeSettingValue(1);
                        break;
                        
                    case 13: // Enter - apply settings
                        ApplySettings();
                        break;
                        
                    case 27: // ESC - back to main menu
                        Hide();
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error handling settings input: {ex.Message}");
            }
        }
        
        private int GetMaxOptionForCategory()
        {
            switch (currentCategory)
            {
                case SettingsCategory.Graphics: return 3;
                case SettingsCategory.Audio: return 1;
                case SettingsCategory.Controls: return 2;
                case SettingsCategory.Gameplay: return 3;
                default: return 1;
            }
        }
        
        private void ChangeSettingValue(int direction)
        {
            switch (currentCategory)
            {
                case SettingsCategory.Graphics:
                    ChangeGraphicsSetting(direction);
                    break;
                case SettingsCategory.Controls:
                    ChangeControlsSetting(direction);
                    break;
                case SettingsCategory.Gameplay:
                    ChangeGameplaySetting(direction);
                    break;
            }
        }
        
        private void ChangeGraphicsSetting(int direction)
        {
            switch (selectedOption)
            {
                case 0: // Resolution
                    currentResolution = (currentResolution + direction + resolutions.Length) % resolutions.Length;
                    Console.WriteLine($"Resolution: {resolutions[currentResolution]}");
                    break;
                case 1: // Fullscreen
                    fullscreen = !fullscreen;
                    Console.WriteLine($"Fullscreen: {fullscreen}");
                    break;
                case 2: // Graphics Quality
                    graphicsQuality = Math.Max(0, Math.Min(2, graphicsQuality + direction));
                    Console.WriteLine($"Graphics Quality: {graphicsQuality}");
                    break;
            }
        }
        
        private void ChangeControlsSetting(int direction)
        {
            switch (selectedOption)
            {
                case 0: // Mouse Sensitivity
                    mouseSensitivity = Math.Max(0.1f, Math.Min(10.0f, mouseSensitivity + direction * 0.1f));
                    Console.WriteLine($"Mouse Sensitivity: {mouseSensitivity:F1}");
                    break;
                case 1: // Invert Mouse
                    invertMouse = !invertMouse;
                    Console.WriteLine($"Invert Mouse: {invertMouse}");
                    break;
            }
        }
        
        private void ChangeGameplaySetting(int direction)
        {
            switch (selectedOption)
            {
                case 0: // Difficulty
                    difficulty = Math.Max(0, Math.Min(2, difficulty + direction));
                    Console.WriteLine($"Difficulty: {difficulty}");
                    break;
                case 1: // Show FPS
                    showFPS = !showFPS;
                    Console.WriteLine($"Show FPS: {showFPS}");
                    break;
                case 2: // Auto Reload
                    autoReload = !autoReload;
                    Console.WriteLine($"Auto Reload: {autoReload}");
                    break;
            }
        }
        
        private void ApplySettings()
        {
            try
            {
                // Apply mouse sensitivity
                set_mouse_sensitivity(mouseSensitivity);
                
                Console.WriteLine("Settings applied successfully!");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error applying settings: {ex.Message}");
            }
        }
        
        public void Show()
        {
            isVisible = true;
            currentCategory = SettingsCategory.Graphics;
            selectedOption = 0;
            LoadCurrentSettings();
            Console.WriteLine("Settings menu shown");
        }
        
        public void Hide()
        {
            isVisible = false;
            Console.WriteLine("Settings menu hidden");
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
            Console.WriteLine("Settings menu cleaned up");
        }
        
        // Properties
        public bool IsVisible => isVisible;
        public SettingsCategory CurrentCategory => currentCategory;
        
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