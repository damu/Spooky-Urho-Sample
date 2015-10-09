/*#include "LuaScriptMod.h"
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>

LuaScriptMod::LuaScriptMod(Context* context) :
    Object(context),
    luaState_(0)
{
    RegisterLuaScriptLibrary(context_);

    luaState_ = luaL_newstate();
    if (!luaState_)
    {
        LOGERROR("Could not create Lua state");
        return;
    }

    SetContext(luaState_, context);

    lua_atpanic(luaState_, &LuaScriptMod::AtPanic);

    luaL_openlibs(luaState_);
    ReplacePrint();

    //Register lua bindings here
}

LuaScriptMod::~LuaScriptMod()
{
    functionPointerToFunctionMap_.Clear();
    functionNameToFunctionMap_.Clear();
    auto luaState = luaState_;
    luaState_ = 0;

    SetContext(luaState_, 0);

    if(luaState) {
        lua_close(luaState);
    }
}

bool LuaScriptMod::ExecuteFile(const String& fileName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    LuaFile* luaFile = cache->GetResource<LuaFile>(fileName);
    return luaFile && luaFile->LoadAndExecute(luaState_);
}

bool LuaScriptMod::ExecuteString(const String& string)
{
    int top = lua_gettop(luaState_);

    if (luaL_dostring(luaState_, string.CString()) != 0)
    {
        const char* message = lua_tostring(luaState_, -1);
        LOGERROR("Execute Lua string failed: " + String(message));
        lua_settop(luaState_, top);
        return false;
    }

    return true;

}

bool LuaScriptMod::ExecuteFunction(const String& functionName)
{
    LuaFunction* function = GetFunction(functionName);
    return function && function->BeginCall() && function->EndCall();
}

LuaFunction* LuaScriptMod::GetFunction(int functionIndex)
{
    if (!lua_isfunction(luaState_, functionIndex))
        return 0;

    const void* functionPointer = lua_topointer(luaState_, functionIndex);
    if (!functionPointer)
        return 0;

    HashMap<const void*, SharedPtr<LuaFunction> >::Iterator i = functionPointerToFunctionMap_.Find(functionPointer);
    if (i != functionPointerToFunctionMap_.End())
        return i->second_;

    lua_pushvalue(luaState_, functionIndex);
    int functionRef = luaL_ref(luaState_, LUA_REGISTRYINDEX);

    SharedPtr<LuaFunction> function(new LuaFunction(luaState_, functionRef, false));
    functionPointerToFunctionMap_[functionPointer] = function;

    return function;
}

LuaFunction* LuaScriptMod::GetFunction(const String& functionName, bool silentIfNotFound)
{
    if (!luaState_)
        return 0;

    HashMap<String, SharedPtr<LuaFunction> >::Iterator i = functionNameToFunctionMap_.Find(functionName);
    if (i != functionNameToFunctionMap_.End())
        return i->second_;

    int top = lua_gettop(luaState_);

    SharedPtr<LuaFunction> function;
    if (PushScriptFunction(functionName, silentIfNotFound))
        function = GetFunction(-1);

    lua_settop(luaState_, top);

    functionNameToFunctionMap_[functionName] = function;
    return function;
}

void LuaScriptMod::ReplacePrint()
{
    static const struct luaL_reg reg[] =
    {
        {"print", &LuaScriptMod::Print},
        {NULL, NULL}
    };

    lua_getglobal(luaState_, "_G");
    luaL_register(luaState_, NULL, reg);
    lua_pop(luaState_, 1);
}

int LuaScriptMod::Print(lua_State* L)
{
    String string;
    int n = lua_gettop(L);
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= n; i++)
    {
        const char* s;
        // Function to be called
        lua_pushvalue(L, -1);
        // Value to print
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);
        // Get result
        s = lua_tostring(L, -1);
        if (s == NULL)
            return luaL_error(L, LUA_QL("tostring") " must return a string to " LUA_QL("print"));

        if (i > 1)
            string.Append("    ");

        string.Append(s);
        // Pop result
        lua_pop(L, 1);
    }

    LOGRAW(string + "\n");

    return 0;
}

int LuaScriptMod::AtPanic(lua_State* L)
{
    String errorMessage = luaL_checkstring(L, -1);
    LOGERROR("Lua error: Error message = '" + errorMessage + "'");
    lua_pop(L, 1);
    return 0;
}

bool LuaScriptMod::PushScriptFunction(const String& functionName, bool silentIfNotFound)
{
    Vector<String> splitedNames = functionName.Split('.');

    String currentName = splitedNames.Front();
    lua_getglobal(luaState_, currentName.CString());

    if (splitedNames.Size() > 1)
    {
        if (!lua_istable(luaState_, -1))
        {
            LOGERROR("Could not find Lua table: Table name = '" + currentName + "'");
            return false;
        }

        for (unsigned i = 1; i < splitedNames.Size() - 1; ++i)
        {
            currentName = currentName + "." + splitedNames[i];
            lua_getfield(luaState_, -1, splitedNames[i].CString());
            if (!lua_istable(luaState_, -1))
            {
                LOGERROR("Could not find Lua table: Table name = '" + currentName + "'");
                return false;
            }
        }

        currentName = currentName + "." + splitedNames.Back().CString();
        lua_getfield(luaState_, -1, splitedNames.Back().CString());
    }

    if (!lua_isfunction(luaState_, -1))
    {
        if (!silentIfNotFound)
            LOGERROR("Could not find Lua function: Function name = '" + currentName + "'");
        return false;
    }

    return true;
}

*/
