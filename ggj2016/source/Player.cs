using System;

using OpenTK;
using OpenTK.Input;

namespace jam.source
{

    enum PlayerState
    {
        Idle,
        Moving,
        Dying
    }

    class Player : IDisposable
    {

        public PlayerState State { get; private set; }

        private Sprite sprite;

        public Vector2 Position { get; private set; }
        public Vector2 Velocity { get; private set; }
        public float Acceleration { get; set; }

        MusicStream audioPlayer;

        Ref<float> tug;

        float invicibility = 0.0f;

        public bool IsDead { get; private set; } = false;

        public Player(Ref<float> tug, float speed, MusicStream audioPlayer)
        {
            State = PlayerState.Idle;
            sprite = new Sprite($"{ContentDirectory}player.png", 7);
            Position = new Vector2(-5.0f, 0.0f);
            Velocity = new Vector2(0.0f, 0.0f);
            this.audioPlayer = audioPlayer;
         
            this.tug = tug;
            Acceleration = speed;
        }

        public void Update(float dt, bool[] kb, Arena arena)
        {

            invicibility -= dt;

            if (State == PlayerState.Dying)
            {
                if (sprite.CurrentFrame == 6)
                {
                    IsDead = true;
                }
            }
            else
            {

                if (tug.Value < -0.7f)
                {
                    State = PlayerState.Dying;
                    audioPlayer.Play($"{ContentDirectory}playerdeath.ogg", false);
                    sprite.SetAnimationParameters(3, 6, 1.0f);
                    return;
                }

                Vector2 velocity = Velocity;

                PlayerState newState = PlayerState.Idle;

                if (kb[(int)Key.A] || kb[(int)Key.Left]) velocity.X -= Acceleration * dt;
                if (kb[(int)Key.D] || kb[(int)Key.Right]) velocity.X += Acceleration * dt;
                if (kb[(int)Key.W] || kb[(int)Key.Up]) velocity.Y += Acceleration * dt;
                if (kb[(int)Key.S] || kb[(int)Key.Down]) velocity.Y -= Acceleration * dt;

                if ((Position.Length < 14.5f) || (Vector2.Dot(Position, Velocity) <= 0.0f))
                {
                    Velocity = velocity * 0.9f;
                }
                else
                {
                    tug.Value -= 0.005f * Velocity.Length;
                    Velocity = Velocity.Length * -Vector2.Normalize(Position) * 0.9f;

                }

                Position += Velocity * dt;

                if (Velocity.Length > 0.1f)
                {
                    newState = PlayerState.Moving;
                }

                if (newState != State)
                {
                    if (newState == PlayerState.Idle) sprite.SetAnimationParameters(0, 0, 1.0f);
                    else sprite.SetAnimationParameters(0, 2, 0.2f);
                    State = newState;
                }

                foreach (var trap in arena.TrapList)
                {
                    if (trap.active && ((Position - trap.position).Length < 1.75f) && (invicibility <= 0.0f))
                    {
                        tug.Value -= trap.sprite.CurrentFrame * 0.05f;
                        invicibility = 0.5f;
                        audioPlayer.Play($"{ContentDirectory}playerdmg.ogg", false);
                    }
                }
            }

            sprite.Update(dt);
            
        }

        public void Draw(gfx.Shader shader)
        {

            shader.LoadFloat((float)Math.Atan2(Velocity.Y, Velocity.X), "rotation");
            shader.LoadVector2(Position, "translation");

            sprite.Draw(shader);

        }

        public void Dispose()
        {
            sprite?.Dispose();
        }

        private static string ContentDirectory = "content/";

    }

}
