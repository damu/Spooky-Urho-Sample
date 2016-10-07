#ifndef ENEMY_H
#define ENEMY_H

//#include "gs_playing.h"
#include "game_state.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationState.h>

/// \brief Creates and manages an enemy.
class enemy : public Urho3D::Object
{
public:
    Urho3D::Vector3 pos_last;
    Urho3D::AnimationState* as_stand;
    Urho3D::AnimationState* as_walk;
    Urho3D::AnimationState* as_run;
    //Urho3D::AnimationState* as_jump;
    Urho3D::Node* node;
    Urho3D::Node* node_model;
    Urho3D::Node* node_aim;     ///< for aiming
    Urho3D::Node* node_target=0;
    Urho3D::RigidBody* body;
    Urho3D::SoundSource3D* sound_source1;
    Urho3D::SoundSource3D* sound_source2;
    Urho3D::SoundSource3D* sound_source_eat;
    Urho3D::Sound* sound_step1;
    Urho3D::Sound* sound_step2;
    Urho3D::Sound* sound_eat;
    Urho3D::Vector3 wander_target=Urho3D::Vector3(Urho3D::Random(-100,100),0,Urho3D::Random(-100,100));
    timer wander_timer;
    float wander_timeout=4+Urho3D::Random(0.0f,4.0f);
    bool sound_step1_not_played=true;
    bool sound_step2_not_played=true;
    float animation_progress_last=0;

    enemy(Urho3D::Vector3 pos);
    ~enemy(){node->Remove();node_model->Remove();}
    void update(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData);

    URHO3D_OBJECT(enemy,Object)
};

#endif // PLAYER_H
