#ifndef GS_PLAYING_H
#define GS_PLAYING_H

#include "game_state.h"
#include "enemy.h"
#include "world_part.h"
#include "player.h"
#include "enemy.h"

#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Audio/SoundListener.h>
#include <Urho3D/Audio/Audio.h>

class player;
class gs_playing;

/// A BoundingBox shaped trigger (trigger_area) that spawns rocks inside the spawn_area BoundingBox.
struct level_rock_spawn
{
    Urho3D::BoundingBox trigger_area;
    Urho3D::BoundingBox spawn_area;
    int rock_count=100;
    bool rocks_spawned=false;           ///< if the trigger has already been activated
};

/// Used to load static models from the level XML file.
struct level_static_model
{
    Urho3D::String name;
    Urho3D::Vector3 pos;
    float scale=1.0;

    level_static_model(){}
    level_static_model(Urho3D::String name,Urho3D::Vector3 pos,float scale=1.0) : name(name),pos(pos),scale(scale) {}
};

/// Level class used to store all map related data that are read from the XML map files.
class level
{
public:
    std::vector<Urho3D::Vector3> torch_positions;
    std::vector<Urho3D::Vector3> flag_positions;
    std::vector<Urho3D::Vector3> enemy_positions;
    std::vector<level_static_model> static_models;    ///< static level geometry like terrain and buildings. Will all get a triangle-mesh collider.
    std::vector<world_part> world_parts;
    Urho3D::Vector3 player_pos;
    Urho3D::String sound_name;
    Urho3D::String skybox_material="Materials/Skybox.xml";
    float level_min_height=0;
    float gravity=-9.81;

    level(){}
    /// loads a level from an XML or LUA file
    level(std::string filename);

    void load_lua_level(std::string level_filename);
    /// \brief Searches for fitting parts (like cross sections) that dock together and updates their occupied docts regardingly.
    void fix_occupied_ports();
    /// \brief Puts end pieces on all open docking points
    void place_end_pieces();
};

/// The game state handling playing a level.
class gs_playing : public game_state
{
public:
    static gs_playing* instance;
    Urho3D::Text* window_text;              ///< The GUI text element in the window at the top that displays the level time, coordinates and FPS.
    double timer_playing=0;                 ///< The time the level is already played. Increased each frame with the frameStep.
    double goal_time=0;                     ///< The time the player needed to complete the level.
    std::vector<Urho3D::Node*> flag_nodes;
    static std::string last_level_filename; ///< Used to restart the last played level (like from gs_level_end).
    std::unique_ptr<player> player_;
    std::vector<std::unique_ptr<enemy>> enemies;
    float level_min_height=999999;          ///< When the player gets below this height, he dies.
    level current_level;
    delayed_action_handler delayed_actions;
    Urho3D::RigidBody* grabbed_body=0;      ///< while holding the mouse, things can be physically moved.
    Urho3D::RigidBody* highlighted_body=0;
    float grab_distance=1;
    std::vector<Urho3D::Material*> highlight_old_materials;
    Urho3D::Sound* sound_stone_collision=0;

    gs_playing(std::string level_filename);
    void update(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData);
    void HandleKeyDown(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData);
    void HandleMouseDown(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData);
    void HandleMouseUp(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData);
    void HandleStoneCollision(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData);
    void spawn_torch(Urho3D::Vector3 pos);

    URHO3D_OBJECT(gs_playing,game_state)
};

#endif // GS_PLAYING_H
