#include "player.h"

using namespace Urho3D;

player::player(Vector3 pos,game_state* gs)
{
    node=globals::instance()->scene->CreateChild("Player");
    gs->nodes.emplace_back(node);
    globals::instance()->player_node=node;
    node_model=globals::instance()->scene->CreateChild();
    gs->nodes.emplace_back(node_model);

    AnimatedModel* boxObject=node_model->CreateComponent<AnimatedModel>();
    set_model(boxObject,globals::instance()->cache,"Data/Models/robot");
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
    shape->SetCapsule(1.3,2,Vector3(0,1.05,0));

    {
        Animation* ani=globals::instance()->cache->GetResource<Animation>("Models/robot_stand.ani");
        as_stand=boxObject->AddAnimationState(ani);
        as_stand->SetWeight(1.0f);
        as_stand->SetLooped(true);
    }
    {
        Animation* ani=globals::instance()->cache->GetResource<Animation>("Models/robot_walk.ani");
        as_walk=boxObject->AddAnimationState(ani);
        as_walk->SetWeight(0.0f);
        as_walk->SetLooped(true);
    }
    {
        Animation* ani=globals::instance()->cache->GetResource<Animation>("Models/robot_run.ani");
        as_run=boxObject->AddAnimationState(ani);
        as_run->SetWeight(0.0f);
        as_run->SetLooped(true);
    }
    {
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
    }

    {
        node_camera_pos=node_model->GetChild("head",true)->CreateChild();
        node_camera_pos->Translate(Vector3(0,0.35,-0.35));
        node_light=globals::instance()->scene->CreateChild();
        light=node_light->CreateComponent<Light>();
        light->SetLightType(LIGHT_SPOT);
        light->SetRange(100);
        light->SetBrightness(1.6);
        light->SetColor(Color(1.0,1.0,1.0,1.0)*1);
        light->SetCastShadows(true);
        light->SetFov(70);
        light->SetShapeTexture(globals::instance()->cache->GetResource<Texture2D>("Textures/flashlight.jpg"));
        light->SetRampTexture(globals::instance()->cache->GetResource<Texture2D>("Textures/ramp_flashlight.png"));
    }

    {
        sound_step1=globals::instance()->cache->GetResource<Sound>("Sounds/step1.ogg");
        sound_step2=globals::instance()->cache->GetResource<Sound>("Sounds/step2.ogg");
        sound_source1=node_model->CreateComponent<SoundSource3D>();
        sound_source1->SetNearDistance(1);
        sound_source1->SetFarDistance(55);
        sound_source1->SetSoundType(SOUND_EFFECT);
        sound_source2=node_model->CreateComponent<SoundSource3D>();
        sound_source2->SetNearDistance(1);
        sound_source2->SetFarDistance(55);
        sound_source2->SetSoundType(SOUND_EFFECT);

        sound_flashlight_button=globals::instance()->cache->GetResource<Sound>("Sounds/flashlight_button.ogg");
        sound_source_flashlight_button=node_model->CreateComponent<SoundSource3D>();
        sound_source_flashlight_button->SetNearDistance(1);
        sound_source_flashlight_button->SetFarDistance(55);
        sound_source_flashlight_button->SetSoundType(SOUND_EFFECT);
    }
}

void player::update(Input* input,float timeStep)
{
    Node* node_camera=globals::instance()->camera->GetNode();

    {
        Vector3 moveDir=Vector3::ZERO;
        Vector3 moveDir_global=Vector3::ZERO;

        if(input->GetKeyDown('D'))
            moveDir+=Vector3::RIGHT*1;
        if(input->GetKeyDown('A'))
            moveDir-=Vector3::RIGHT*1;
        if(input->GetKeyDown('W'))
            moveDir+=Vector3::FORWARD*1;
        if(input->GetKeyDown('S'))
            moveDir-=Vector3::FORWARD*1;

        if(moveDir.Length()>0.1)
            body->SetFriction(0.4);
        else
            body->SetFriction(2.0);

        if(moveDir.Length()>0.5)
            moveDir.Normalize();

        Vector3 vel=body->GetLinearVelocity()*Vector3(1,0,1);
        Quaternion rot=body->GetRotation();

        static bool on_floor;
        Vector3 moveDir_world;
        {
            static bool at_wall;
            static int jumping=0; // 0 = not jumping, 1 = jumping, 2 =
            on_floor=false;

            float height=0;
            PhysicsRaycastResult result;
            Ray ray(node->GetPosition()+Vector3(0,1.0,0),Vector3::DOWN);
            globals::instance()->physical_world->SphereCast(result,ray,0.2,2,2);
            if(result.distance_<=2)
                on_floor=true;
            if(!on_floor)
                moveDir*=0.35;

            as_stand->AddTime(timeStep/2);
            as_walk->AddTime(timeStep*vel.Length()/1.5);
            as_run->AddTime(timeStep*vel.Length()/3);
            as_jump->AddTime(timeStep);
            as_reversing->AddTime(timeStep);
            as_stand->SetWeight(1.0-Clamp(vel.Length()/2.0,0.0,1.0));
            as_stand->SetWeight(1.0);
            as_walk->SetWeight(Clamp(vel.Length()/2.0,0.0,1.0));
            as_run->SetWeight(Clamp((vel.Length()-2)/2.0,0.0,1.0));   // maybe this should be done differently, but works for this game
            if(!on_floor)
                as_jump->SetWeight(as_jump->GetWeight()+timeStep*5);
            else
            {
                as_jump->SetWeight(0.0);

                static bool sound_step1_not_played=true;    // not playing this animation cycle
                static bool sound_step2_not_played=true;
                if(as_run->GetTime()>0.1&&sound_step1_not_played&&as_reversing->GetWeight()<0.5)
                {
                    sound_source1->Play(sound_step1);
                    sound_step1_not_played=false;
                }
                if(as_run->GetTime()>0.6&&sound_step2_not_played&&as_reversing->GetWeight()<0.5)
                {
                    sound_source2->Play(sound_step2);
                    sound_step2_not_played=false;
                }
                if(as_run->GetTime()<0.1)
                {
                    sound_step1_not_played=true;
                    sound_step2_not_played=true;
                }
            }

            if(input->GetKeyDown(KEY_SPACE)&&jumping==false&&(on_floor))
                jumping=1;  // start jumping
            else if(!input->GetKeyDown(KEY_SPACE))
                jumping=0;


            static float jump_force_applied=0;
            static const float max_jump_force_applied=100;
            moveDir_world=node->GetWorldRotation()*moveDir;

            if(moveDir_world.Angle(vel)>90&&vel.Length()>3&&on_floor)   // indicate if direction change jump / side sommersault possible
                as_reversing->SetWeight(as_reversing->GetWeight()+timeStep*10);
            else
                as_reversing->SetWeight(0.0);

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
                jump_force_applied=0;
        }

        float f=0.5;    // for walking or sprinting
        if(input->GetKeyDown(KEY_SHIFT))
            f=1.0;
        Quaternion quat;
        quat.FromLookRotation(node_camera->GetDirection()*Vector3(1,0,1),Vector3::UP);
        body->SetRotation(quat);
        float speed_old=vel.Length();
        vel+=rot*moveDir*timeStep*4000*f/body->GetMass();
        float speed_new=vel.Length();
        if(speed_new>10*f&&speed_new>speed_old)   // over limit. Don't increase speed further but make direction change possible.
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
            if(s>1&&!camera_first_person)
            {
                node_model->SetDirection(Vector3::FORWARD);
                node_model->Yaw(yaw);
            }
        }

        {   // physic raycast to avoid the player glitching through stuff when moving very fast
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

    IntVector2 mouseMove(0,0);
    if(!input->IsMouseVisible())
        mouseMove=input->GetMouseMove();

    camera_yaw+=mouseMove.x_*0.1;
    if(camera_yaw<0)
        camera_yaw+=360;
    if(camera_yaw>=360)
        camera_yaw-=360;
    camera_pitch+=mouseMove.y_*0.1;
    camera_pitch=Clamp(camera_pitch,-85.0f,85.0f);
    if(!camera_first_person)
    {
        camera_distance-=input->GetMouseMoveWheel();
        camera_distance=Clamp(camera_distance,2.0f,50.0f);

        node_camera->SetPosition(node->GetPosition());
        node_camera->SetDirection(Vector3::FORWARD);
        node_camera->Yaw(camera_yaw);
        node_camera->Translate(Vector3(0,1.6,0));
        node_camera->Pitch(camera_pitch);

        PhysicsRaycastResult result;
        Ray ray(node_camera->GetPosition(),-node_camera->GetDirection());
        globals::instance()->physical_world->SphereCast(result,ray,0.2,camera_distance,2);
        if(result.distance_<=camera_distance)
            node_camera->Translate(Vector3(0,0,-result.distance_+0.1));
        else
            node_camera->Translate(Vector3(0,0,-camera_distance));
    }
    else
    {
        node_camera->SetPosition(node_camera_pos->GetWorldPosition());
        node_camera->SetDirection(Vector3::FORWARD);
        node_camera->Yaw(camera_yaw);
        node_camera->Pitch(camera_pitch);
        node_camera->Translate(Vector3(0,0,0.1));
        node_model->SetDirection(Vector3::FORWARD);
        node_model->Yaw(camera_yaw);
    }

    node_light->SetPosition(node_camera_pos->GetWorldPosition());
    node_light->Translate(Vector3(0.5,-0.9,0.3));
    float yaw_360_correction=(camera_yaw+Random(-light_shaking,light_shaking))-light_yaw;
    if(yaw_360_correction>180)
        yaw_360_correction-=360;
    else if(yaw_360_correction<-180)
        yaw_360_correction+=360;
    light_yaw+=yaw_360_correction*(10*timeStep);
    light_pitch+=((camera_pitch+Random(-light_shaking,light_shaking))-light_pitch)*(10*timeStep);
    node_light->SetDirection(Vector3::FORWARD);
    node_light->Yaw(light_yaw);
    node_light->Pitch(light_pitch);
}
