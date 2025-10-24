using System;
using System.Drawing;
using System.Runtime.InteropServices;

namespace SimpleShooter.UI
{
    public class GameHUD
    {
        private int windowWidth;
        private int windowHeight;
        private bool initialized = false;
        
        // HUD element positions
        private int healthBarX, healthBarY;
        private int ammoCounterX, ammoCounterY;
        private int scoreDisplayX, scoreDisplayY;
        private int crosshairX, crosshairY;
        
        // HUD element sizes
        private const int HEALTH_BAR_WIDTH = 200;
        private const int HEALTH_BAR_HEIGHT = 20;
        private const int CROSSHAIR_SIZE = 20;
        
        // Colors
        private Color healthColor = Color.Green;
        private Color lowHealthColor = Color.Red;
        private Color ammoColor = Color.Cyan;
        private Color scoreColor = Color.Yellow;
        private Color crosshairColor = Color.White;

        // Import rendering functions
        [DllImport("simple_shooter", CallingConvention = CallingConvention.Cdecl)]
        private static extern void render_text(string text, float x, float y, float r, float g, float b);

        [DllImport("simple_shooter", CallingConvention = CallingConvention.Cdecl)]
        private static extern void render_ui_background(float x, float y, float width, float height, float r, float g, float b, float a);

        [DllImport("simple_shooter", CallingConvention = CallingConvention.Cdecl)]
        private static extern void render_crosshair(float x, float y, float size, float r, float g, float b);

        public bool Initialize(int screenWidth, int screenHeight)
        {
            Console.WriteLine("Initializing Game HUD...");
            
            windowWidth = screenWidth;
            windowHeight = screenHeight;
            
            CalculatePositions();
            
            initialized = true;
            Console.WriteLine("Game HUD initialized successfully");
            return true;
        }

        private void CalculatePositions()
        {
            // Health bar - bottom left
            healthBarX = 20;
            healthBarY = windowHeight - 60;
            
            // Ammo counter - bottom right
            ammoCounterX = windowWidth - 150;
            ammoCounterY = windowHeight - 60;
            
            // Score display - top left
            scoreDisplayX = 20;
            scoreDisplayY = 20;
            
            // Crosshair - center
            crosshairX = windowWidth / 2;
            crosshairY = windowHeight / 2;
        }

        public void Update(PlayerState player, int score, float deltaTime)
        {
            if (!initialized) return;
            
            // HUD updates are mostly handled in render
            // Could add animations or effects here
        }

        public void Render()
        {
            if (!initialized) return;
            
            // This method will be called with current game state
            // For now, we'll use placeholder rendering
            Console.WriteLine("[HUD] Rendering HUD elements...");
        }

        public void RenderWithState(PlayerState player, int score)
        {
            if (!initialized) return;
            
            try
            {
                // Render health bar
                RenderHealthBar(player.health, player.max_health);
                
                // Render ammo counter
                RenderAmmoCounter(player.ammo, player.max_ammo);
                
                // Render score
                RenderScore(score);
                
                // Render crosshair
                RenderCrosshair();
                
                // Console output for debugging
                Console.WriteLine($"[HUD] Health: {player.health}/{player.max_health}, " +
                                $"Ammo: {player.ammo}/{player.max_ammo}, Score: {score}");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"HUD render error: {ex.Message}");
                
                // Fallback console display
                Console.WriteLine($"[HUD] Health: {player.health}/{player.max_health}");
                Console.WriteLine($"[HUD] Ammo: {player.ammo}/{player.max_ammo}");
                Console.WriteLine($"[HUD] Score: {score}");
            }
        }

        private void RenderHealthBar(int currentHealth, int maxHealth)
        {
            try
            {
                // Background
                render_ui_background(healthBarX, healthBarY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, 
                                   0.2f, 0.2f, 0.2f, 0.8f);
                
                // Health bar fill
                float healthPercent = (float)currentHealth / maxHealth;
                Color barColor = healthPercent > 0.3f ? healthColor : lowHealthColor;
                
                render_ui_background(healthBarX + 2, healthBarY + 2, 
                                   (HEALTH_BAR_WIDTH - 4) * healthPercent, HEALTH_BAR_HEIGHT - 4,
                                   barColor.R / 255.0f, barColor.G / 255.0f, barColor.B / 255.0f, 0.9f);
                
                // Health text
                string healthText = $"Health: {currentHealth}/{maxHealth}";
                render_text(healthText, healthBarX, healthBarY - 20, 1.0f, 1.0f, 1.0f);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Health bar render error: {ex.Message}");
            }
        }

        private void RenderAmmoCounter(int currentAmmo, int maxAmmo)
        {
            try
            {
                string ammoText = $"Ammo: {currentAmmo}/{maxAmmo}";
                render_text(ammoText, ammoCounterX, ammoCounterY, 
                           ammoColor.R / 255.0f, ammoColor.G / 255.0f, ammoColor.B / 255.0f);
                
                // Low ammo warning
                if (currentAmmo <= maxAmmo * 0.2f)
                {
                    render_text("LOW AMMO!", ammoCounterX, ammoCounterY - 20, 1.0f, 0.0f, 0.0f);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Ammo counter render error: {ex.Message}");
            }
        }

        private void RenderScore(int score)
        {
            try
            {
                string scoreText = $"Score: {score}";
                render_text(scoreText, scoreDisplayX, scoreDisplayY, 
                           scoreColor.R / 255.0f, scoreColor.G / 255.0f, scoreColor.B / 255.0f);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Score render error: {ex.Message}");
            }
        }

        private void RenderCrosshair()
        {
            try
            {
                render_crosshair(crosshairX, crosshairY, CROSSHAIR_SIZE,
                               crosshairColor.R / 255.0f, crosshairColor.G / 255.0f, crosshairColor.B / 255.0f);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Crosshair render error: {ex.Message}");
            }
        }

        public void ShowGameOver(int finalScore)
        {
            try
            {
                // Game over overlay
                render_ui_background(0, 0, windowWidth, windowHeight, 0.0f, 0.0f, 0.0f, 0.7f);
                
                // Game over text
                string gameOverText = "GAME OVER";
                render_text(gameOverText, windowWidth / 2 - 100, windowHeight / 2 - 50, 1.0f, 0.0f, 0.0f);
                
                string finalScoreText = $"Final Score: {finalScore}";
                render_text(finalScoreText, windowWidth / 2 - 80, windowHeight / 2, 1.0f, 1.0f, 0.0f);
                
                Console.WriteLine($"[HUD] GAME OVER - Final Score: {finalScore}");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Game over display error: {ex.Message}");
                Console.WriteLine($"GAME OVER - Final Score: {finalScore}");
            }
        }

        public void SetWindowSize(int width, int height)
        {
            windowWidth = width;
            windowHeight = height;
            CalculatePositions();
            Console.WriteLine($"HUD repositioned for window {width}x{height}");
        }

        public void Cleanup()
        {
            if (!initialized) return;
            
            initialized = false;
            Console.WriteLine("Game HUD cleaned up");
        }

        public bool IsInitialized => initialized;
    }
}