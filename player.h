#ifndef PLAYER_H
#define PLAYER_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>

#include "gs_playing.h"

/// \brief Creates and manages the player and the camera.
class player
{
public:
    Urho3D::Vector3 pos_last;
    Urho3D::AnimationState* as_stand;
    Urho3D::AnimationState* as_walk;
    Urho3D::AnimationState* as_run;
    Urho3D::AnimationState* as_jump;
    Urho3D::AnimationState* as_reversing;
    Urho3D::Node* node;
    Urho3D::Node* node_model;
    Urho3D::Node* node_light;
    Urho3D::Node* node_camera_pos;
    Urho3D::Light* light;
    Urho3D::RigidBody* body;
    Urho3D::SoundSource3D* sound_source1;
    Urho3D::SoundSource3D* sound_source2;
    Urho3D::SoundSource3D* sound_source_flashlight_button;
    Urho3D::Sound* sound_step1;
    Urho3D::Sound* sound_step2;
    Urho3D::Sound* sound_flashlight_button;
    float camera_distance=14;
    float camera_yaw=20;
    float camera_pitch=20;
    float light_yaw=20;
    float light_pitch=90;
    float light_shaking=1;
    bool camera_first_person=true;

    /// \brief The game state is given to add the player nodes to the node clean-up list of the game state.
    player(Urho3D::Vector3 pos,game_state* gs);
    /// \brief Called each frame.
    void update(Urho3D::Input* input,float timeStep);
};

#endif // PLAYER_H
