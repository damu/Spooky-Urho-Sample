#include "world_part.h"

using namespace Urho3D;
using namespace std;

world_part::world_part(game_state* gs,String model_name,Urho3D::Vector3 pos)
{
    node=globals::instance()->scene->CreateChild();
    gs->nodes.push_back(node);
    node->SetPosition(pos);
    AnimatedModel* boxObject=node->CreateComponent<AnimatedModel>();
    set_model(boxObject,globals::instance()->cache,("Data/Models/"+model_name).CString());
    boxObject->SetCastShadows(true);

    RigidBody* body=node->CreateComponent<RigidBody>();
    body->SetCollisionLayer(2);     // Use layer bitmask 2 for static geometry
    shape=node->CreateComponent<CollisionShape>();
    shape->SetTriangleMesh(globals::instance()->cache->GetResource<Model>("Data/Models/"+model_name+".mdl"));

    const Urho3D::Vector<Urho3D::SharedPtr<Urho3D::Node>> childs=node->GetChildren();
    for(int i=0;i<childs.Size();i++)
    {
        auto& c=childs[i];
        if(c->GetName().Substring(0,4)==Urho3D::String("dock"))
            docking_points.push_back(c->GetName().CString());
    }
}

bool world_part::move_to_docking_point(Urho3D::String dock_name,world_part& to_world_part,Urho3D::String to_dock_name)
{
    if(docking_points_occupied.count(dock_name)||to_world_part.docking_points_occupied.count(to_dock_name))
        return false;

    move_bone_to_bone(node,dock_name,to_world_part.node.Get(),to_dock_name);

    // check for collision via a bounding box
    globals::instance()->physical_world->UpdateCollisions();
    auto bb=shape->GetWorldBoundingBox();
    auto d=bb.max_-bb.min_;
    bb.min_+=d*0.2;
    bb.max_-=d*0.2;
    Urho3D::PODVector<Urho3D::RigidBody*> result;
    globals::instance()->physical_world->GetRigidBodies(result,bb);
    if(result.Size()>1) // only this model should be here
        return false;

    docking_points_occupied.insert(dock_name);
    to_world_part.docking_points_occupied.insert(to_dock_name);
    return true;
}
