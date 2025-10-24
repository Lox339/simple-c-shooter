using System;
using System.Drawing;
using System.Runtime.InteropServices;

namespace SimpleShooter.UI
{
    public class Speedometer
    {
        private int windowWidth;
        private int windowHeight;
        private float currentSpeed;
        private bool isOnGround;
        private bool initialized = false;
        
        // Position in right upper corner
        private int positionX;
        private int positionY;
        private int width = 200;
        private int height = 80;
        
        // Speed thresholds for color coding
        private const float NORMAL_SPEED_THRESHOLD = 10.0f;
        private const float FAST_SPEED_THRESHOLD = 15.0f;
        private const float BUNNY_HOP_THRESHOLD = 20.0f;
        
        // Colors for different speed ranges
        private Color normalColor = Color.White;
        private Color fastColor = Color.Yellow;
        private Color bunnyHopColor = Color.Red;
        private Color currentColor = Color.White;

        // Import OpenGL text rendering functions (placeholder)
        [DllImport("simple_shooter", CallingConvention = CallingConvention.Cdecl)]
        private static extern void render_text(string text, float x, float y, float r, float g, float b);

        [DllImport("simple_shooter", CallingConvention = CallingConvention.Cdecl)]
        private static extern void render_ui_background(float x, float y, float width, float height, float r, float g, float b, float a);

        public bool Initialize(int screenWidth, int screenHeight)
        {
            Console.WriteLine("Initializing Speedometer...");
            
            windowWidth = screenWidth;
            windowHeight = screenHeight;
            
            // Position in right upper corner with margin
            positionX = windowWidth - width - 20;
            positionY = 20;
            
            currentSpeed = 0.0f;
            isOnGround = true;
            
            initialized = true;
            Console.WriteLine($"Speedometer initialized at position ({positionX}, {positionY})");
            return true;
        }

        public void Update(float playerSpeed, bool onGround)
        {
            if (!initialized) return;
            
            currentSpeed = playerSpeed;
            isOnGround = onGround;
            
            // Update color based on speed
            UpdateSpeedColor();
        }

        private void UpdateSpeedColor()
        {
            if (currentSpeed >= BUNNY_HOP_THRESHOLD)
            {
                currentColor = bunnyHopColor;
            }
            else if (currentSpeed >= FAST_SPEED_THRESHOLD)
            {
                currentColor = fastColor;
            }
            else
            {
                currentColor = normalColor;
            }
        }

        public void Render()
        {
            if (!initialized) return;
            
            try
            {
                // Render semi-transparent background
                float bgAlpha = 0.7f;
                render_ui_background(positionX, positionY, width, height, 0.0f, 0.0f, 0.0f, bgAlpha);
                
                // Format speed text
                string speedText = $"Speed: {currentSpeed:F1} u/s";
                
                // Render speed text
                render_text(speedText, positionX + 10, positionY + 15, 
                           currentColor.R / 255.0f, currentColor.G / 255.0f, currentColor.B / 255.0f);
                
                // Render additional info
                string statusText = isOnGround ? "Ground" : "Air";
                render_text(statusText, positionX + 10, positionY + 35, 0.8f, 0.8f, 0.8f);
                
                // Render bunny hop indicator
                if (currentSpeed > NORMAL_SPEED_THRESHOLD && !isOnGround)
                {
                    string bhopText = "BUNNY HOP!";
                    render_text(bhopText, positionX + 10, positionY + 55, 1.0f, 0.0f, 0.0f);
                }
                
                // Console fallback for debugging
                if (currentSpeed > FAST_SPEED_THRESHOLD)
                {
                    Console.WriteLine($"[SPEEDOMETER] {speedText} ({statusText}) - Color: {GetColorName()}");
                }
            }
            catch (Exception ex)
            {
                // Fallback to console output
                Console.WriteLine($"[SPEEDOMETER] Speed: {currentSpeed:F1} u/s ({(isOnGround ? "Ground" : "Air")})");
                Console.WriteLine($"Speedometer render error: {ex.Message}");
            }
        }

        private string GetColorName()
        {
            if (currentColor == bunnyHopColor) return "RED";
            if (currentColor == fastColor) return "YELLOW";
            return "WHITE";
        }

        public void SetPosition(int x, int y)
        {
            positionX = x;
            positionY = y;
            Console.WriteLine($"Speedometer position set to ({x}, {y})");
        }

        public void SetWindowSize(int width, int height)
        {
            windowWidth = width;
            windowHeight = height;
            
            // Recalculate position for right upper corner
            positionX = windowWidth - this.width - 20;
            positionY = 20;
            
            Console.WriteLine($"Speedometer repositioned for window {width}x{height}");
        }

        public void SetSpeedThresholds(float normal, float fast, float bunnyHop)
        {
            if (normal > 0 && fast > normal && bunnyHop > fast)
            {
                // Update thresholds - using const fields, so this is for future enhancement
                Console.WriteLine($"Speed thresholds would be updated: {normal}, {fast}, {bunnyHop}");
            }
        }

        public void Cleanup()
        {
            if (!initialized) return;
            
            initialized = false;
            Console.WriteLine("Speedometer cleaned up");
        }

        // Public getters
        public float CurrentSpeed => currentSpeed;
        public bool IsOnGround => isOnGround;
        public Color CurrentColor => currentColor;
        public bool IsInitialized => initialized;
        
        // Position getters
        public int PositionX => positionX;
        public int PositionY => positionY;
        public int Width => width;
        public int Height => height;
    }
}