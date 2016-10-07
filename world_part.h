#ifndef WORLD_PART_H
#define WORLD_PART_H

#include <set>
#include <algorithm>

#include "game_state.h"
#include "misc.h"

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>

/// \brief Wraps an Urho Node and removes it when this objects gets destructed.
class node_wrapper
{
public:
    Urho3D::Node* node;
    ~node_wrapper()
    {
        node->Remove();
    }
    Urho3D::Node* operator=(Urho3D::Node* n){node=n;return node;}
};

/// \brief The game world can be constructed out of combinable parts, like rooms forming a dungeon. This class helps with constructing maps out of combinable parts.
class world_part
{
public:
    std::shared_ptr<node_wrapper> node;
    Urho3D::CollisionShape* shape;

    world_part(Urho3D::String model_name,Urho3D::Vector3 position=Urho3D::Vector3(0,0,0));

    /// \brief Contains all subnode names starting with "dock". Those are bones that signal docking points where other world parts can be docked.
    std::vector<Urho3D::String> docking_points;
    /// \brief Contains all docking parts where snother world part is already docked.
    std::set<Urho3D::String> docking_points_occupied;
    /// \brief Contains all subnode names starting with "spawn". Those are bones that signal spawning points for stuff like enemies and items.
    std::vector<Urho3D::String> spawn_points;

    /*std::vector<Urho3D::String> get_free_docks()
    {
        std::vector<Urho3D::String> ret=get_docks();
        return ret;
    }*/

    /// \brief Tries to dock this world part with the given dock dock_name to another world_part's (to_world_parts) dock named to_dock_name.
    /// Returns true on success (dock not yet occupied and this world_part not colliding with anything).
    bool move_to_docking_point(Urho3D::String dock_name,world_part& to_world_part,Urho3D::String to_dock_name,bool ignore_collision=false);

    /// \brief Returns the name of an random unoccupied docking point, or an empty String if no dock is free.
    Urho3D::String get_free_dock_name()
    {
//        std::shuffle(docking_points.begin(),docking_points.end(),std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
        for(auto e:docking_points)
            if(docking_points_occupied.find(e)==docking_points_occupied.end())
                return e;
        return "";
    }

    void move_to_docking_point(Urho3D::String dock_name,Urho3D::Node* to_world_part,Urho3D::String to_dock_name)
    {
        move_bone_to_bone(node->node,dock_name,to_world_part,to_dock_name);
    }
};

#endif // WORLD_PART_H
