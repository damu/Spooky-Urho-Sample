-- setzer22: With your ideas in mind I think it would be better to just declare all the map as data in table and pass it to the C++ side, something like what you did could be translated as:

local map = {
    skybox = "Materials/skybox_space.xml",
    models = {
        {"Data/Models/level_2", Vector3(0,0,0)},
        -- More models here
    },
    flags = {
        Vector3(1.8, 15.5, 626.8), 
        -- More flags here
    }
    enemies = {
        -- Let me explain with an example some of the cool features we could include. Here, we declare an array of enemies. 
        -- For convenience, some kind of global enum has all the available enemies, and by specifying enemies.WHITE_GHOST the game knows which resources to to load. The interesting part is the third parameter in the table. That is the custom behaviour of the enemy. 
        -- The custom behaviour is a table that has some fields containing the update, start, and similar functions which then would be called at the proper time. This allows the modder to easily define new behaviours while maintaining the base enemy IA. 
        {enemies.WHITE_GHOST, Vector3(5,0,0), nil},
        {enemies.WHITE_GHOST, Vector3(-5,0,0), custom_white_ghost}
    }, 

    map_logic = {
        -- This "Update" references the Update function defined below, this would be just like registering events in a more lua-way. 
        update = Update, 
    },

    seed = 12345678, 

    -- Maybe, instead of defining enemies and models with a position, given that the levels would be auto generated I would specify enemy / room types with an spawn rate, and maybe some filters such as, enemy X can only spawn in cave rooms. This was just for the sake of the example.
}

-- A behaviour definition, note how functions can be table items just as well as integers or strings.
local custom_white_ghost = {
    update = function (ghost, timeStep) 
        -- Update code for the custom ghost,
    end,
    start = function (ghost) 
        -- Start code for the custom ghost
    end,
    -- etc...
    
    -- A good thing is we can add more variables in here than the ones C++ would check for, so this "custom_white_ghost", while being a table, acts pretty much like a class.
}
-- As a syntax thing, note how useful it is that the last element in a list can have a comma after it (to add more things later)

function Update(timeStep) 
    -- This would be the whole level update function, rather than the update function of a single enemy. Here the modder could include some "global" logic like doing things after a certain amount of time.
end

-- Finally, we just need to call the function to register the map as a whole. The map definition would then contain all the callback functions, (which we could save by pointer on the C++ side for faster access), and of course translate the table data into a C++ object that would represent the level.
register_level(map)

-- As some final thoughts, I think this map format would make it easier for the modder while making it more difficult on the C++ side, but that's how it should be IMO, modders are usually not experienced programmers. 

-- ========================
-- Original pseudo-lua code
-- ========================

-- the former USP XML format
--
--<xml skybox="Materials/skybox_space.xml">
  --<static_model name="Data/Models/level_2" pos_x="0" pos_y="0" pos_z="0" />
  --<player pos_x="56" pos_y="-35" pos_z="206" />
  --<flag pos_x="1.8" pos_y="15.5" pos_z="626.8" />
  --<flag pos_x="1.5" pos_y="15.9" pos_z="579.6" />
  --<flag pos_x="2.1" pos_y="14.2" pos_z="490.0" />
  --<flag pos_x="2.9" pos_y="41.3" pos_z="451.6" />
  --<flag pos_x="75" pos_y="60.5" pos_z="376.1" />
  --<flag pos_x="77.7" pos_y="10.7" pos_z="478.6" />
  --<flag pos_x="77.7" pos_y="10.7" pos_z="525.6" />
  --<flag pos_x="74.4" pos_y="20.1" pos_z="249.4" />
  --<flag pos_x="1.8" pos_y="-5.2" pos_z="410" />
  --<torch pos_x="74.4" pos_y="-43.6" pos_z="249.4" />
--</xml>

-- just a rough idea, could also use Vector3(...) instead of three single numbers or whatever to be more like Urho

-- Pseudo-Lua-Code ;-)
--int main()
--{
    --set_skybox("Materials/skybox_space.xml");
    --place_model("Data/Models/level_2",0,0,0);
    --set_player_pos(56,-35,206);
    --place_flag(1.8,15.5,626.8);                -- this could be some item placements which are required to win the game
    -- ...
    --place_torch(74.4,-43.6,249.4);         -- some special object placement with automatic physics, sound and particle emitter
--
    -- later: maybe some event registering here or the game is automatically calling certain function names
    -- register_event("update",update(float));
    -- or (no idea how this works)
    -- register_event_update(update(float));

    -- later: the scripts could be combining some random rooms to build a random level,
    -- like with the place_model function called above
    -- parameters like given map seeds could be parameters of this main function or
    -- fetched with some int get_seed(); function or something

    --return 0;
--}

--int update(float timeStep)
--{
    -- ... if this script does something specific
    --return 0;
--}
