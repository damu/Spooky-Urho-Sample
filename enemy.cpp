#include "enemy.h"
#include "gs_playing.h"
#include "gs_death.h"
#include "player.h"

using namespace std;
using namespace Urho3D;

enemy::enemy(Vector3 pos) : Object(globals::instance()->context)
{
    node=globals::instance()->scene->CreateChild("Enemy");
    node_aim=node->CreateChild();
    node_model=globals::instance()->scene->CreateChild();

    AnimatedModel* boxObject=node_model->CreateComponent<AnimatedModel>();
    set_model(boxObject,globals::instance()->cache,"Data/Models/merguns");
    boxObject->SetCastShadows(true);

    body=node->CreateComponent<RigidBody>();
    body->SetPosition(pos);
    pos_last=body->GetPosition();
    body->SetCollisionLayer(1);
    body->SetMass(80.0);
    body->SetLinearDamping(0.0f);
    body->SetAngularDamping(0.98f);
    body->SetAngularFactor(Vector3(0,1,0));
    body->SetFriction(0.8);
    CollisionShape* shape=node->CreateComponent<CollisionShape>();
    shape->SetCapsule(1.8,3,Vector3(0,1.5,0));

    {
        Animation* ani=globals::instance()->cache->GetResource<Animation>("Models/merguns_stand.ani");
        as_stand=boxObject->AddAnimationState(ani);
        as_stand->SetWeight(1.0f);
        as_stand->SetLooped(true);
    }
    {
        Animation* ani=globals::instance()->cache->GetResource<Animation>("Models/merguns_walk.ani");
        as_walk=boxObject->AddAnimationState(ani);
        as_walk->SetWeight(0.0f);
        as_walk->SetLooped(true);
    }
    {
        Animation* ani=globals::instance()->cache->GetResource<Animation>("Models/merguns_run.ani");
        as_run=boxObject->AddAnimationState(ani);
        as_run->SetWeight(0.0f);
        as_run->SetLooped(true);
    }
    /*{
        Animation* ani=globals::instance()->cache->GetResource<Animation>("Models/robot_jump.ani");
        as_jump=boxObject->AddAnimationState(ani);
        as_jump->SetWeight(0.0f);
        as_jump->SetLooped(true);
    }
    {
        Animation* ani=globals::instance()->cache->GetResource<Animation>("Models/robot_reversing.ani");
        as_reversing=boxObject->AddAnimationState(ani);
        as_reversing->SetWeight(0.0f);
        as_reversing->SetLooped(true);
    }*/

    {
        sound_step1=globals::instance()->cache->GetResource<Sound>("Sounds/step1_deep.ogg");
        sound_step2=globals::instance()->cache->GetResource<Sound>("Sounds/step2_deep.ogg");
        sound_source1=node_model->CreateComponent<SoundSource3D>();
        sound_source1->SetNearDistance(1);
        sound_source1->SetFarDistance(55);
        sound_source1->SetSoundType(SOUND_EFFECT);
        sound_source2=node_model->CreateComponent<SoundSource3D>();
        sound_source2->SetNearDistance(1);
        sound_source2->SetFarDistance(55);
        sound_source2->SetSoundType(SOUND_EFFECT);

        sound_eat=globals::instance()->cache->GetResource<Sound>("Sounds/236331__sclolex__zombieeatingflesh.ogg");
        sound_eat->SetLooped(true);
        sound_source_eat=node_model->CreateComponent<SoundSource3D>();
        sound_source_eat->SetNearDistance(1);
        sound_source_eat->SetFarDistance(55);
        sound_source_eat->SetSoundType(SOUND_EFFECT);
        sound_source_eat->SetGain(0.5);
    }

    SubscribeToEvent(E_UPDATE,URHO3D_HANDLER(enemy,update));
}

void enemy::update(StringHash eventType,VariantMap& eventData)
{
    if(globals::instance()->game_states.size()>1)   // if paused, do nothing
       return;

    node_target=globals::instance()->player_node;
    Vector3 target_position=node_target->GetWorldPosition();
    float timeStep=eventData[Update::P_TIMESTEP].GetFloat();

    {
        Vector3 moveDir=Vector3::ZERO;
        Vector3 moveDir_global=Vector3::ZERO;
/*
        if(input->GetKeyDown('D'))
            moveDir+=Vector3::RIGHT*1;
        if(input->GetKeyDown('A'))
            moveDir-=Vector3::RIGHT*1;
        if(input->GetKeyDown('W'))
            moveDir+=Vector3::FORWARD*1;
        if(input->GetKeyDown('S'))
            moveDir-=Vector3::FORWARD*1;
*/

        Vector3 vel=body->GetLinearVelocity()*Vector3(1,0,1);
        Quaternion rot=body->GetRotation();
        float movement_speed=0.2;
        auto as_current=as_stand;

        // if the player is close and visible, approach him
        Vector3 direction_towards_player=globals::instance()->player_node->GetWorldPosition()-node_model->GetWorldPosition();
        float distance_to_player=(node_model->GetWorldPosition()-globals::instance()->player_node->GetWorldPosition()).Length();
        float viewing_angle=node_model->GetWorldDirection().Angle(direction_towards_player);
        PhysicsRaycastResult result;
        Ray ray(node_model->GetWorldPosition()+Vector3(0,1,0),direction_towards_player);
        globals::instance()->physical_world->RaycastSingle(result,ray,distance_to_player+1);
        if(distance_to_player<20&&viewing_angle<100&&result.body_==gs_playing::instance->player_->body)
        {
            if(distance_to_player<2)   // attack (currently instant death)
            {
                if(!sound_source_eat->IsPlaying())
                {
                    sound_source_eat->Play(sound_eat);
                    gs_death* gs_d=new gs_death;
                    globals::instance()->game_states.emplace_back(gs_d);
                    gs_d->text_death_reason->SetText("You got killed by a monster.");
                }
            }
            else
            {
                as_current=as_run;
                node_aim->LookAt(target_position);
                auto yaw_diff=rot.YawAngle()-node_aim->GetWorldRotation().YawAngle();
                if(abs(yaw_diff)<40)
                    movement_speed=0.9;

                if(yaw_diff>180)
                    yaw_diff-=360;
                else if(yaw_diff<-180)
                    yaw_diff+=360;

                if(yaw_diff<-5||yaw_diff>5)
                    body->ApplyTorque(Vector3(0,Clamp(-yaw_diff*100.0,-1500.0,1500.0),0));

                moveDir+=Vector3::FORWARD*1;
            }
        }
        else    // or wander around randomly
        {
wander:
            PhysicsRaycastResult result;
            Ray ray(node_model->GetWorldPosition()+Vector3(0,1,0),node_model->GetDirection());
            globals::instance()->physical_world->RaycastSingle(result,ray,5);
//cout<<result.distance_;
            if(wander_timer.until_now()>wander_timeout/*||result.distance_<5*/) // pick new wander target if timer timed out or if standing in front of a wall (later not yet working properly)
            {
//cout<<" ###";
                wander_timer.reset();
                wander_timeout=4+Random(0.0f,4.0f);
                if(Random(0.0f,100.0f)>50.0f)  // chance of 25% to stand around
                    wander_target=node_model->GetWorldPosition()+Vector3(Random(-50,50),node->GetWorldPosition().y_,Random(-50,50));
                else
                    wander_target=node_model->GetWorldPosition();
            }
//cout<<endl;
            if((node->GetWorldPosition()-wander_target).Length()>5) // do nothing if target reached
            {
                node_aim->LookAt(wander_target);
                auto yaw_diff=rot.YawAngle()-node_aim->GetWorldRotation().YawAngle();

                if(yaw_diff>180)
                    yaw_diff-=360;
                else if(yaw_diff<-180)
                    yaw_diff+=360;

                if(yaw_diff<-5||yaw_diff>5)
                    body->ApplyTorque(Vector3(0,Clamp(-yaw_diff*50.0,-300.0,300.0),0));

                moveDir+=Vector3::FORWARD*1;
                as_current=as_walk;
            }
        }

        if(moveDir.Length()>0.1)
            body->SetFriction(0.4);
        else
            body->SetFriction(2.0);

        if(moveDir.Length()>0.5)
            moveDir.Normalize();

        bool on_floor;
        Vector3 moveDir_world;
        {
            bool at_wall;
            int jumping=0; // 0 = not jumping, 1 = jumping
            on_floor=false;

            float height=0;
            PhysicsRaycastResult result;
            Ray ray(node->GetPosition()+Vector3(0,1.0,0),Vector3::DOWN);
            globals::instance()->physical_world->SphereCast(result,ray,0.2,2,2);
            if(result.distance_<=2)
                on_floor=true;
            if(!on_floor)
                moveDir*=0.35;

            as_stand->AddTime(timeStep*0.15);
            as_walk->AddTime(timeStep*vel.Length()*0.55);
            as_run->AddTime(timeStep*vel.Length()*0.3);
/*            as_jump->AddTime(timeStep);
            as_reversing->AddTime(timeStep);*/
            if(as_current!=as_stand)
                as_stand->SetWeight(as_stand->GetWeight()-timeStep*2.0);
            if(as_current!=as_walk)
                as_walk->SetWeight(as_walk->GetWeight()-timeStep*2.0);
            if(as_current!=as_run)
                as_run->SetWeight(as_run->GetWeight()-timeStep*2.0);
            as_current->SetWeight(as_current->GetWeight()+timeStep*2.0);

            if(!on_floor)
            {}
//                as_jump->SetWeight(as_jump->GetWeight()+timeStep*5);
            else
            {
//                as_jump->SetWeight(0.0);
                float animation_progress=0;
                if(as_walk->GetWeight()>0.5)
                    animation_progress=as_walk->GetTime();
                else if(as_run->GetWeight()>0.5)
                    animation_progress=as_run->GetTime();

                if(animation_progress<animation_progress_last)
                {
                    sound_step1_not_played=true;
                    sound_step2_not_played=true;
                }
                animation_progress_last=animation_progress;
                if(animation_progress>0.1&&sound_step1_not_played)
                {
                    sound_source1->Play(sound_step1);
                    sound_step1_not_played=false;
                }
                if(animation_progress>0.6&&sound_step2_not_played)
                {
                    sound_source2->Play(sound_step2);
                    sound_step2_not_played=false;
                }
            }
/*
            float jump_force_applied=0;
            const float max_jump_force_applied=100;
            moveDir_world=node->GetWorldRotation()*moveDir;

            if(jumping==1&&jump_force_applied<max_jump_force_applied)   // jump higher if we are jumping and the limit has not been reached
            {
                if(jump_force_applied>max_jump_force_applied)
                {
                    // do nothing if max jump force reached
                }
                else if(jump_force_applied+timeStep*800>max_jump_force_applied)
                {
                    // I want to limit the jump height more exactly by limiting the force pumped into it and applieng the remaining rest here. Doesn't fully work yet.
                    //float f=0;//(max_jump_force_applied-jump_force_applied)*timeStep*2000;
                    //moveDir+=Vector3::UP*2*f;
                    //moveDir_global=result.normal_*1*f;
                    jump_force_applied+=timeStep*900;
                }
                else
                {
                    float f=1;
                    moveDir+=Vector3::UP*0.6*f;
                    moveDir_global=result.normal_*0.3*f;
                    jump_force_applied+=timeStep*800;
                }
            }
            if(jumping!=1)
                jump_force_applied=0;*/
        }

//        if(input->GetKeyDown(KEY_SHIFT))
//            movement_speed=1.0;
        float speed_old=vel.Length();
        vel+=rot*moveDir*timeStep*4000*movement_speed/body->GetMass();
        float speed_new=vel.Length();
        if(speed_new>10*movement_speed&&speed_new>speed_old)   // over limit. Don't increase speed further but make direction change possible.
            vel=vel.Normalized()*speed_old;
        body->SetLinearVelocity(Vector3(vel.x_,body->GetLinearVelocity().y_+(rot*moveDir*timeStep*4500/body->GetMass()).y_,vel.z_));
        body->ApplyImpulse(moveDir_global*timeStep*4000);

        {
            auto vec_rot=body->GetLinearVelocity()*Vector3(1,0,1);
            float s=vec_rot.Length();
            vec_rot.Normalize();
            float yaw=asin(vec_rot.x_)*180/M_PI;
            if(vec_rot.z_<0)
                yaw=180-yaw;
            node_model->SetPosition(node->GetPosition());
            if(s>1)
            {
                node_model->SetDirection(Vector3::FORWARD);
                node_model->Yaw(yaw);
            }
        }

        {   // physic raycast to avoid the enemy glitching through stuff when moving very fast
            Vector3 player_pos=body->GetPosition()+Vector3(0,1,0);
            PhysicsRaycastResult result;
            Ray ray(pos_last,player_pos-pos_last);
            float l=(player_pos-pos_last).Length();
            if(l>0.5)
            {
                globals::instance()->physical_world->SphereCast(result,ray,0.2,l,2);
                if(result.distance_<=l)
                    body->SetPosition(pos_last);
                pos_last=body->GetPosition()+Vector3(0,1,0);
            }
        }
    }
}
