using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using OpenTK;
using OpenTK.Graphics.OpenGL;
using OpenTK.Input;

namespace jam.source
{

    enum EnemyState
    {
        Following,
        Attacking,
        Dying
    }

    class Enemy : IDisposable
    {

        Sprite sprite;

        public EnemyState State { get; private set; }
        public bool IsDead { get; private set; } = false;

        public Vector2 Position { get; private set; }
        public Vector2 Velocity { get; private set; }

        float invicibilityDuration = 0.0f;

        Ref<float> tug;
        public float Acceleration { get; set; }

        float attackCooldown = 0.0f;

        MusicStream audioPlayer;
        float sfxScream1Cooldown = 0.0f;
        float sfxStepCooldown = 0.25f;

        float turbo = 0.0f;

        public bool Clawed { get; private set; }

        public Enemy(Ref<float> tug, float acceleration, MusicStream audioPlayer)
        {
            sprite = new Sprite($"{ContentDirectory}enemy.png", 9, 3.0f);
            Position = new Vector2(5.0f, 0.0f);
            Velocity = new Vector2(0.0f, 0.0f);

            sprite.SetAnimationParameters(0, 2, 0.25f);

            this.tug = tug;
            this.audioPlayer = audioPlayer;
            Acceleration = acceleration;

            State = EnemyState.Following;

        }

        public void Update(float dt, Vector2 playerPosition, PlayerState playerState, Arena arena)
        {

            invicibilityDuration -= dt;
            attackCooldown -= dt;
            sfxScream1Cooldown -= dt;
            sfxStepCooldown -= dt;

            Clawed = false;

            if (State == EnemyState.Dying)
            {
                if (sprite.CurrentFrame == 8)
                {
                    IsDead = true;
                }
            }
            else if (playerState != PlayerState.Dying)
            {

                if (tug.Value > 0.7f)
                {
                    State = EnemyState.Dying;
                    audioPlayer.Play($"{ContentDirectory}enemydeath.ogg", false);
                    sprite.SetAnimationParameters(5, 8, 0.8f);
                }

                Vector2 diff = (playerPosition - Position);
                float dist = diff.Length;

                if (diff.Length > 0.001f) diff.Normalize();

                float dot = Vector2.Dot(Velocity, diff);
                if (dot >= 0.9f) turbo += dt * Vector2.Dot(Vector2.Normalize(Velocity), diff);
                else turbo = 1.0f;

                if ((Position.Length < 14.5f) || (Vector2.Dot(Position, Velocity) <= 0.0f))
                {
                    if (State == EnemyState.Following)
                    {
                        if (Velocity.Length < 15.0f) Velocity += diff * Acceleration * dt;
                        else Velocity += diff * Acceleration * dt * turbo;
                        Velocity *= 0.98f;
                    }
                }
                else
                {
                    State = EnemyState.Following;
                    Velocity = Velocity.Length * -Vector2.Normalize(Position) * 0.5f;
                    tug.Value += 0.025f * Velocity.Length;
                    turbo = 0.25f;
                    if (sfxScream1Cooldown <= 0.0f)
                    {
                        //audioPlayer.Play($"{ContentDirectory}enemy.ogg", false);
                        sfxScream1Cooldown = 2.5f;
                    }
                }

                if (Velocity.Length > 25.0f) Velocity = 25.0f * Vector2.Normalize(Velocity);

                Position += Velocity * dt + diff * dt;

                if ((State == EnemyState.Attacking) && (sprite.CurrentFrame == 3) && (attackCooldown <= 0.0f))
                {
                    State = EnemyState.Following;
                    sprite.SetAnimationParameters(0, 2, 0.25f);
                    attackCooldown = 0.5f;
                }

                if ((State == EnemyState.Following) && (sfxStepCooldown < 0.0f))
                {
                    audioPlayer.Play($"{ContentDirectory}enemyfootsteps.ogg", false);
                    sfxStepCooldown = 0.4f + Math.Max((1.0f - Velocity.Length * 0.11f), 0.0f);
                }

                if ((State == EnemyState.Attacking) && (sprite.CurrentFrame == 4) && (dist < 2.15f))
                {
                    tug.Value -= 0.003f * 5.0f;
                    audioPlayer.Play($"{ContentDirectory}playerdmg.ogg", false);
                    Clawed = true;
                }

                if (State == EnemyState.Attacking)
                {
                    Velocity *= 0.9f;
                }

                if ((dist < 1.75f) && (State == EnemyState.Following) && (attackCooldown < 0.0f))
                {
                    State = EnemyState.Attacking;
                    sprite.SetAnimationParameters(3, 4, 0.25f, true);
                    attackCooldown = 1.0f;
                    tug.Value -= Velocity.Length * 0.009f;
                }

                foreach (var trap in arena.TrapList)
                {
                    if (trap.active && ((trap.position - Position).Length < 2.25f) && (invicibilityDuration <= 0.0f))
                    {
                        tug.Value += 0.007f * Math.Max(0.0f, trap.sprite.CurrentFrame - 1);
                        invicibilityDuration = 0.5f;
                        if (sfxScream1Cooldown <= 0.0f)
                        {
                            //audioPlayer.Play($"{ContentDirectory}enemy.ogg", false);
                            sfxScream1Cooldown = 2.5f;
                        }
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
