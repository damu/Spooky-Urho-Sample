#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/LuaScript/LuaFile.h>
#include <Urho3D/LuaScript/LuaFunction.h>
extern "C"
{
//#include <Urho3D/ThirdParty/LuaJIT/src/lualib.h>
}
//#include <toluapp/tolua++.h>
//#include <Urho3D/LuaScript/ToluaUtils.h>
/*
namespace Urho3D {
    extern void RegisterLuaScriptLibrary(Context* context);
}

class LuaScriptMod : public Object
{
public:
    OBJECT(LuaScriptMod);

    LuaScriptMod(Context* context);
    virtual ~LuaScriptMod();

    /// Execute script file. Return true if successful.
    bool ExecuteFile(const String& fileName);

    /// Execute script string. Return true if successful.
    bool ExecuteString(const String& string);

    /// Execute script function.
    bool ExecuteFunction(const String& functionName);

    /// Return Lua function by function stack index.
    LuaFunction* GetFunction(int functionIndex);

    /// Return Lua function by function stack index.
    LuaFunction* GetFunction(const String& functionName, bool silentIfNotfound = false);

private:

    /// Replace print.
    void ReplacePrint();

    /// Print function
    static int Print(lua_State* L);

    /// At panic.
    static int AtPanic(lua_State* L);

    /// Push script function.
    bool PushScriptFunction(const String& functionName, bool silentIfNotfound = false);

    /// Lua state.
    lua_State* luaState_;

    /// Function pointer to function map.
    HashMap<const void*, SharedPtr<LuaFunction> > functionPointerToFunctionMap_;

    /// Function name to function map.
    HashMap<String, SharedPtr<LuaFunction> > functionNameToFunctionMap_;
};
*/
