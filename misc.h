#ifndef MISC_H
#define MISC_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <iostream>
#include <fstream>

#include <Urho3D/Urho3D.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>

// copied from my source collection.
/**
 * @brief The timer class can be used to measure times and optionaly output them automatically on destruction.
 * Example:
 * \code
 * {
 *   timer _("test");
 *   sleep(0.1);
 * }                        // the timer is destructed here as it goes out of scope and prints something like "0.100132 <- test"
 * \endcode
 */
class timer
{
public:
    std::string str;        ///< the message printed, after the measured time, on destruction
    std::chrono::steady_clock::time_point start;    ///< the time the timer was started on
    bool output;            ///< if the timer should print something on destruction

    timer(std::string str,bool output=true) : str(str),output(output){reset();}
    timer() : str(""),start(),output(false){reset();}
    ~timer()
    {
        if(!output)
            return;
        std::cout<<until_now()<<" <- "<<str<<std::endl;
    }

    /// resets the timer by putting the current time into the member /start/
    void reset(){start=std::chrono::steady_clock::now();}

    /// returns the time which has passed since starting the timer
    double until_now() const
    {
        std::chrono::steady_clock::time_point end=std::chrono::steady_clock::now();
        return(std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()/1000000.0);
    }

    /// same as until_now()
    operator double() const {return until_now();}
};

/**
 * @brief This class takes lambdas and a delay (in seconds) to execute them later. The update() function is thought to be called each frame.
* Example:
 * \code
 * // add an action to be executed in 0.2s
 * delayed_actions.insert(0.2,
 *                        [this]
 *                        {
 *                            player_->light->SetBrightness(player_->light->GetBrightness()>0.5?0:1.5);  // toggle flashlight with a delay to fit to the sound
 *                        });
 *
 * \endcode
 */
class delayed_action_handler
{
    std::map<std::chrono::steady_clock::time_point,std::function<void()>> actions;
public:
    void insert(float wait_time,std::function<void()> lambda){actions.emplace(std::chrono::steady_clock::now()+std::chrono::milliseconds(int(wait_time*1000)),lambda);}

    void update()
    {
        auto time_over=std::chrono::steady_clock::now();
again:
        auto iter=actions.begin();
        if(iter!=actions.end())
            if(iter->first<=time_over)
            {
                iter->second();
                actions.erase(iter);
                goto again;
            }
    }
};

/// \brief Calls SetModel on the given model and tries to load the model file and all texture files mentioned in a model_name+".txt".
/// model_name is supposed to have no file extension. Example: "Data/Models/Box", loads the model "Data/Models/Box.mdl".
/// It's a template to support all model classes like AnimatedModel and StaticModel.
template<typename T>
void set_model(T* model,Urho3D::ResourceCache* cache,std::string model_name)
{
    std::string filename_model=model_name;
    model->SetModel(cache->GetResource<Urho3D::Model>(Urho3D::String(filename_model.append(".mdl").c_str())));
    std::string filename_txt=model_name;
    filename_txt.append(".txt");
    std::ifstream file(filename_txt);
    std::string line;
    if(file.is_open())
        for(int i=0;getline(file,line);i++)
            model->SetMaterial(i,cache->GetResource<Urho3D::Material>(Urho3D::String(line.c_str())));
}

/// \brief For reading and storing the best level times.
/// They are stored in a simple text file with a value pair in each line and a comma seperating the two values.
class map_times
{
    std::map<std::string,float> values;
public:
    map_times()
    {
        std::ifstream file("highscores.txt");
        std::string line;
        while(getline(file,line))
        {
            int comma=line.find(',');
            if(comma!=std::string::npos)
            {
                std::string time=line.substr(comma+1);
                values[line.substr(0,comma)]=std::stof(time);
            }
        }
    }

    float get(std::string level_name)
    {
        int pos=level_name.rfind('/')+1;
        if(pos!=std::string::npos)
            level_name=level_name.substr(pos);
        return values[level_name];
    }

    /// \brief Inserts the given values and trims the level_name to avoid issues.
    void insert(std::string level_name,float time)
    {
        int pos=level_name.rfind('/')+1;
        if(pos!=std::string::npos)
            level_name=level_name.substr(pos);
        values[level_name]=time;
    }

    void save()
    {
        std::ofstream file("highscores.txt",std::ofstream::trunc);
        for(auto& p:values)
            if(p.second>0)
                file<<p.first<<","<<p.second<<"\n";
    }
};

/** \brief Sets the position and rotation of the given node_to_move Node in a way that the bone with the name
 * bone_name_to_move it at the position of the bone with the namebone_name_to_move_to of the bone_name_to_move_to Node.
**/
inline void move_bone_to_bone(Urho3D::Node* node_to_move,const Urho3D::String& bone_name_to_move,Urho3D::Node* node_to_move_to,const Urho3D::String& bone_name_to_move_to)
{
    node_to_move->SetPosition(node_to_move_to->GetChild(bone_name_to_move_to,true)->GetWorldPosition());
    Urho3D::Vector3 bone_a_rot=node_to_move_to->GetChild(bone_name_to_move_to,true)->GetWorldRotation().EulerAngles();
    Urho3D::Vector3 bone_b_rot=node_to_move->GetChild(bone_name_to_move,true)->GetWorldRotation().EulerAngles();
    node_to_move->Yaw(bone_b_rot.z_-bone_a_rot.z_+180);
    node_to_move->Pitch(bone_b_rot.x_-bone_a_rot.x_);
    node_to_move->Roll(bone_b_rot.y_-bone_a_rot.y_);
    node_to_move->Translate(-node_to_move->GetChild(bone_name_to_move,true)->GetPosition());
}

#endif // MISC_H
