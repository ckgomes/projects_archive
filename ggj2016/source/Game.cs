using System;

using OpenTK;
using OpenTK.Graphics.OpenGL;

using OpenTK.Input;

namespace jam.source
{

    enum GameState
    {
        Menu,
        Playing
    }

    class Game : GameWindow, IDisposable
    {

        gfx.Shader shdTexture;
        gfx.Shader shdFloor;
        gfx.Shader shdBackground;
        gfx.Shader shdFinalpass;
        gfx.Shader shdUserInterface;
        gfx.Shader shdBloodyScreen;

        gfx.RenderTarget offscreen;

        gfx.VertexBuffer screenQuad;

        GameState gameState;

        Arena arena;
        Player player;
        Enemy enemy;

        gfx.Texture2D texScratch;

        UserInterfaceWidget playButton;
        UserInterfaceWidget exitButton;
        UserInterfaceWidget fplayButton;
        UserInterfaceWidget fexitButton;
        UserInterfaceWidget ggjIcon;

        ChainRenderer chainRenderer;

        MusicStream audioPlayer;

        Ref<float> playerHP = 0.0f;
        float bloodyScreenTime = 0.0f;
        Vector3 bloodyScreenColor = new Vector3(0.0f);

        bool[] keyboard = new bool[256];
        float aspect;

        float timeAcc = 0.0f;

        int selectedOption = 0;

        void InitializeActors()
        {

            player = new Player(playerHP, 55.0f, audioPlayer);
            enemy = new Enemy(playerHP, 55.0f, audioPlayer);

        }

        public Game(int windowWidth, int windowHeight) : base(windowWidth, windowHeight)
        {

            Title = "Infernal Selection";

            RenderFrame += eRenderFrame;
            KeyDown += Game_KeyDown;
            KeyUp += Game_KeyUp;        
            Closing += Game_Closing;

            aspect = (float)Width / Height;
         
            audioPlayer = new MusicStream();

            gameState = GameState.Menu;
                        
            GL.ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            shdFloor = gfx.ShaderFactory.CreateFloorShader();
            shdTexture = gfx.ShaderFactory.CreateTextureShader();
            shdBackground = gfx.ShaderFactory.CreateBackgroundShader();
            shdFinalpass = gfx.ShaderFactory.CreateFinalpassShader();
            shdUserInterface = gfx.ShaderFactory.CreateUserInterfaceShader();
            shdBloodyScreen = gfx.ShaderFactory.CreateBloodyScreenShader();

            Matrix4 ortho = Matrix4.CreateOrthographic(20f * aspect, 20f, -1.0f, 1.0f);
            shdTexture.LoadMatrix4(ortho, "projection");
            shdFloor.LoadMatrix4(ortho, "projection");

            offscreen = new gfx.RenderTarget(Width, Height);

            screenQuad = gfx.VertexBufferFactory.CreateQuad(2.0f, 2.0f);

            texScratch = new gfx.Texture2D($"{ContentDirectory}scratch.png");

            arena = new Arena(15.0f);

            playButton = new UserInterfaceWidget($"{ContentDirectory}playbtn.png",
                new Vector2(0.2f * 200f / 75f, 0.2f),
                new Vector2(0.0f, 0.4f));

            exitButton = new UserInterfaceWidget($"{ContentDirectory}quitbtn.png",
                new Vector2(0.2f * 200f / 75f, 0.2f),
                new Vector2(0.0f, 0.0f));

            fplayButton = new UserInterfaceWidget($"{ContentDirectory}fplaybtn.png",
                new Vector2(0.2f * 200f / 75f, 0.2f),
                new Vector2(0.0f, 0.4f));

            fexitButton = new UserInterfaceWidget($"{ContentDirectory}fquitbtn.png",
                new Vector2(0.2f * 200f / 75f, 0.2f),
                new Vector2(0.0f, 0.0f));

            ggjIcon = new UserInterfaceWidget($"{ContentDirectory}ggj.png",
                new Vector2(0.3f / aspect, 0.3f),
                new Vector2(0.7f, -0.7f));

            chainRenderer = new ChainRenderer(aspect, audioPlayer);

            GL.Enable(EnableCap.StencilTest);
            GL.Enable(EnableCap.Blend);
            GL.BlendFunc(BlendingFactorSrc.SrcAlpha, BlendingFactorDest.OneMinusSrcAlpha);

            audioPlayer.Play($"{ContentDirectory}e1m1.ogg", true);
            audioPlayer.Play($"{ContentDirectory}background_sound.ogg", true, 0.5f);

        }

        private void Game_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {

            chainRenderer?.Dispose();
            player?.Dispose();
            enemy?.Dispose();
            arena?.Dispose();

            texScratch?.Dispose();
            screenQuad?.Dispose();

            playButton?.Dispose();
            exitButton?.Dispose();
            fplayButton?.Dispose();
            fexitButton?.Dispose();
            ggjIcon?.Dispose();

            shdTexture?.Dispose();
            shdFloor?.Dispose();
            shdBackground?.Dispose();
            shdFinalpass?.Dispose();
            shdUserInterface?.Dispose();
            shdBloodyScreen?.Dispose();

            audioPlayer?.Dispose();

        }

        private void Game_KeyUp(object sender, KeyboardKeyEventArgs e)
        {
            keyboard[(int)e.Key] = false;
        }
        private void Game_KeyDown(object sender, KeyboardKeyEventArgs e)
        {
            keyboard[(int)e.Key] = true;
        }

        private void resetGame()
        {
            playerHP = 0.0f;
            selectedOption = 0;
            timeAcc = 0.0f;
            InitializeActors();
        }

        private void stateMenu(float dt)
        {

            gfx.RenderTarget.Backbuffer.Clear();

            shdBackground.Bind();
            shdBackground.LoadVector2(new Vector2(Width, Height), "resolution");
            shdBackground.LoadFloat(timeAcc, "time");
            screenQuad.Bind();
            GL.DrawArrays(PrimitiveType.TriangleFan, 0, screenQuad.VertexCount);
            screenQuad.Unbind();
            shdBackground.Unbind();

            if (keyboard[(int)Key.Enter])
            {
                
                switch (selectedOption)
                {
                    case 0:
                        resetGame();
                        gameState = GameState.Playing;
                        break;

                    default:
                        Exit();
                        break;
                        
                }
            }

            if ((keyboard[(int)Key.W] || keyboard[(int)Key.Up]) && (selectedOption == 1)) --selectedOption;
            if ((keyboard[(int)Key.S] || keyboard[(int)Key.Down]) && (selectedOption == 0)) ++selectedOption;

            if (selectedOption == 0)
            {
                fplayButton.Draw(shdUserInterface);
                exitButton.Draw(shdUserInterface);
            }
            else
            {
                playButton.Draw(shdUserInterface);
                fexitButton.Draw(shdUserInterface);
            }
            ggjIcon.Draw(shdUserInterface);

        }

        private void statePlaying(float dt)
        {

            float lastTug = playerHP;            
            
            if (player.Acceleration < 170.0f)
            {
                player.Acceleration += dt * 0.5f;
                enemy.Acceleration = player.Acceleration * 0.4f;
            }

            arena.Update(dt);
            player.Update(dt, keyboard, arena);
            enemy.Update(dt, player.Position, player.State, arena);
            chainRenderer.Update(dt, playerHP);

            if (player.IsDead) gameState = GameState.Menu;
            if (enemy.IsDead) gameState = GameState.Menu;

            bloodyScreenTime -= dt;
            if ((playerHP - lastTug) > 0.0f)
            {
                bloodyScreenColor = new Vector3(0.25f, 0.25f, 0.0f);
                bloodyScreenTime = 0.125f;
            }
            else if (playerHP - lastTug < 0.0f)
            {
                bloodyScreenColor = new Vector3(0.25f, 0.0f, 0.0f);
                bloodyScreenTime = 0.125f;
            }
            else if (bloodyScreenTime <= 0.0f) bloodyScreenColor = new Vector3(0.0f, 0.0f, 0.0f);

            offscreen.Clear();

            shdBackground.Bind();
            shdBackground.LoadVector2(new Vector2(Width, Height), "resolution");
            shdBackground.LoadFloat((float)Math.Pow(timeAcc, 1.2), "time");
            screenQuad.Bind();
            GL.DrawArrays(PrimitiveType.TriangleFan, 0, screenQuad.VertexCount);
            screenQuad.Unbind();
            shdBackground.Unbind();

            shdFloor.LoadVector2(player.Position, "camera");
            arena.DrawMap(shdFloor);

            shdTexture.LoadVector2(player.Position, "camera");
            arena.DrawTraps(shdTexture);
            player.Draw(shdTexture);
            enemy.Draw(shdTexture);

            GL.BlendFunc(BlendingFactorSrc.One, BlendingFactorDest.One);
            shdBloodyScreen.Bind();
            shdBloodyScreen.LoadVector3(bloodyScreenColor, "screenColor");
            screenQuad.Bind();
            if (enemy.Clawed) texScratch.Bind(0);
            GL.DrawArrays(PrimitiveType.TriangleFan, 0, screenQuad.VertexCount);
            if (enemy.Clawed) texScratch.Unbind(0);
            screenQuad.Unbind();
            shdBloodyScreen.Unbind();
            GL.BlendFunc(BlendingFactorSrc.SrcAlpha, BlendingFactorDest.OneMinusSrcAlpha);

            chainRenderer.Draw(shdUserInterface);
            
            offscreen.BindToTexture(1);

            gfx.RenderTarget.Backbuffer.Clear();

            shdFinalpass.Bind();
            shdFinalpass.SetSamplerUnit(1, "sceneFramebuffer");
            shdFinalpass.LoadVector4(Vector4.One, "ambientLight");

            screenQuad.Bind();
            GL.DrawArrays(PrimitiveType.TriangleFan, 0, screenQuad.VertexCount);
            screenQuad.Unbind();

            shdFinalpass.Unbind();
        }

        private void eRenderFrame(object sender, FrameEventArgs e)
        {

            float dt = (float)e.Time;
            timeAcc += dt;

            switch (gameState)
            {
                case GameState.Menu:
                    stateMenu(dt);
                    break;
            
                case GameState.Playing:
                    statePlaying(dt);
                    break;
            }

            SwapBuffers();
        }

        private string ContentDirectory = "content/";

    }
}
