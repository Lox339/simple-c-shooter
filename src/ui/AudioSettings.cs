using System;
using System.Runtime.InteropServices;

namespace SimpleShooter.UI
{
    public class AudioSettings
    {
        // Import audio bridge functions
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void audio_set_master_volume(float volume);
        
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void audio_set_sfx_volume(float volume);
        
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void audio_set_music_volume(float volume);
        
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern float audio_get_master_volume();
        
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern float audio_get_sfx_volume();
        
        [DllImport("game_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern float audio_get_music_volume();

        // UI state
        private bool isVisible;
        private float masterVolume;
        private float sfxVolume;
        private float musicVolume;
        
        // UI positioning
        private float panelX = 50;
        private float panelY = 100;
        private float panelWidth = 300;
        private float panelHeight = 200;
        
        public AudioSettings()
        {
            isVisible = false;
            RefreshVolumeSettings();
        }
        
        public void Show()
        {
            isVisible = true;
            RefreshVolumeSettings();
        }
        
        public void Hide()
        {
            isVisible = false;
        }
        
        public void Toggle()
        {
            if (isVisible)
                Hide();
            else
                Show();
        }
        
        public bool IsVisible => isVisible;
        
        private void RefreshVolumeSettings()
        {
            try
            {
                masterVolume = audio_get_master_volume();
                sfxVolume = audio_get_sfx_volume();
                musicVolume = audio_get_music_volume();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error refreshing audio settings: {ex.Message}");
                // Use default values
                masterVolume = 1.0f;
                sfxVolume = 1.0f;
                musicVolume = 0.7f;
            }
        }
        
        public void Update(float deltaTime)
        {
            if (!isVisible) return;
            
            // Audio settings panel would be updated here
            // In a real implementation, this would handle UI interactions
        }
        
        public void Render()
        {
            if (!isVisible) return;
            
            try
            {
                // Render audio settings panel background
                RenderUIBackground(panelX, panelY, panelWidth, panelHeight, 0.2f, 0.2f, 0.3f, 0.9f);
                
                // Title
                RenderText("Audio Settings", panelX + 10, panelY + 10, 1.0f, 1.0f, 1.0f);
                
                // Master Volume
                RenderText($"Master Volume: {(int)(masterVolume * 100)}%", 
                          panelX + 10, panelY + 40, 0.9f, 0.9f, 0.9f);
                RenderVolumeBar(panelX + 10, panelY + 55, 280, 15, masterVolume);
                
                // SFX Volume
                RenderText($"SFX Volume: {(int)(sfxVolume * 100)}%", 
                          panelX + 10, panelY + 85, 0.9f, 0.9f, 0.9f);
                RenderVolumeBar(panelX + 10, panelY + 100, 280, 15, sfxVolume);
                
                // Music Volume
                RenderText($"Music Volume: {(int)(musicVolume * 100)}%", 
                          panelX + 10, panelY + 130, 0.9f, 0.9f, 0.9f);
                RenderVolumeBar(panelX + 10, panelY + 145, 280, 15, musicVolume);
                
                // Instructions
                RenderText("Use +/- keys to adjust volumes", 
                          panelX + 10, panelY + 175, 0.7f, 0.7f, 0.7f);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error rendering audio settings: {ex.Message}");
            }
        }
        
        private void RenderVolumeBar(float x, float y, float width, float height, float volume)
        {
            // Background bar
            RenderUIBackground(x, y, width, height, 0.1f, 0.1f, 0.1f, 0.8f);
            
            // Volume fill
            float fillWidth = width * volume;
            float r = volume > 0.7f ? 0.2f : volume > 0.3f ? 1.0f : 1.0f;
            float g = volume > 0.7f ? 1.0f : volume > 0.3f ? 1.0f : 0.3f;
            float b = volume > 0.7f ? 0.2f : volume > 0.3f ? 0.2f : 0.2f;
            
            RenderUIBackground(x + 2, y + 2, fillWidth - 4, height - 4, r, g, b, 0.9f);
        }
        
        public void HandleInput(string input)
        {
            if (!isVisible) return;
            
            try
            {
                switch (input.ToLower())
                {
                    case "1": // Increase master volume
                        SetMasterVolume(Math.Min(1.0f, masterVolume + 0.1f));
                        break;
                    case "2": // Decrease master volume
                        SetMasterVolume(Math.Max(0.0f, masterVolume - 0.1f));
                        break;
                    case "3": // Increase SFX volume
                        SetSFXVolume(Math.Min(1.0f, sfxVolume + 0.1f));
                        break;
                    case "4": // Decrease SFX volume
                        SetSFXVolume(Math.Max(0.0f, sfxVolume - 0.1f));
                        break;
                    case "5": // Increase music volume
                        SetMusicVolume(Math.Min(1.0f, musicVolume + 0.1f));
                        break;
                    case "6": // Decrease music volume
                        SetMusicVolume(Math.Max(0.0f, musicVolume - 0.1f));
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error handling audio settings input: {ex.Message}");
            }
        }
        
        public void SetMasterVolume(float volume)
        {
            masterVolume = Math.Max(0.0f, Math.Min(1.0f, volume));
            try
            {
                audio_set_master_volume(masterVolume);
                Console.WriteLine($"Master volume set to {(int)(masterVolume * 100)}%");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error setting master volume: {ex.Message}");
            }
        }
        
        public void SetSFXVolume(float volume)
        {
            sfxVolume = Math.Max(0.0f, Math.Min(1.0f, volume));
            try
            {
                audio_set_sfx_volume(sfxVolume);
                Console.WriteLine($"SFX volume set to {(int)(sfxVolume * 100)}%");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error setting SFX volume: {ex.Message}");
            }
        }
        
        public void SetMusicVolume(float volume)
        {
            musicVolume = Math.Max(0.0f, Math.Min(1.0f, volume));
            try
            {
                audio_set_music_volume(musicVolume);
                Console.WriteLine($"Music volume set to {(int)(musicVolume * 100)}%");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error setting music volume: {ex.Message}");
            }
        }
        
        // External rendering functions (imported from graphics bridge)
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