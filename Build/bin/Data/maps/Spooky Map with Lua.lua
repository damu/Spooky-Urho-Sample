// the former USP XML format
/*
<xml skybox="Materials/skybox_space.xml">
  <static_model name="Data/Models/level_2" pos_x="0" pos_y="0" pos_z="0" />
  <player pos_x="56" pos_y="-35" pos_z="206" />
  <flag pos_x="1.8" pos_y="15.5" pos_z="626.8" />
  <flag pos_x="1.5" pos_y="15.9" pos_z="579.6" />
  <flag pos_x="2.1" pos_y="14.2" pos_z="490.0" />
  <flag pos_x="2.9" pos_y="41.3" pos_z="451.6" />
  <flag pos_x="75" pos_y="60.5" pos_z="376.1" />
  <flag pos_x="77.7" pos_y="10.7" pos_z="478.6" />
  <flag pos_x="77.7" pos_y="10.7" pos_z="525.6" />
  <flag pos_x="74.4" pos_y="20.1" pos_z="249.4" />
  <flag pos_x="1.8" pos_y="-5.2" pos_z="410" />
  <torch pos_x="74.4" pos_y="-43.6" pos_z="249.4" />
</xml>
*/

// just a rough idea, could also use Vector3(...) instead of three single numbers or whatever to be more like Urho

// Pseudo-Lua-Code ;-)
int main()
{
    set_skybox("Materials/skybox_space.xml");
    place_model("Data/Models/level_2",0,0,0);
    set_player_pos(56,-35,206);
    place_flag(1.8,15.5,626.8);                // this could be some item placements which are required to win the game
    // ...
    place_torch(74.4,-43.6,249.4);         // some special object placement with automatic physics, sound and particle emitter

    // later: maybe some event registering here or the game is automatically calling certain function names
    // register_event("update",update(float));
    // or (no idea how this works)
    // register_event_update(update(float));

    // later: the scripts could be combining some random rooms to build a random level,
    // like with the place_model function called above
    // parameters like given map seeds could be parameters of this main function or
    // fetched with some int get_seed(); function or something

    return 0;
}

int update(float timeStep)
{
    // ... if this script does something specific
    return 0;
}
