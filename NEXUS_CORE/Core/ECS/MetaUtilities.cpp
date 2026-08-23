#include "MetaUtilities.h"

entt::id_type NEXUS_CORE::Utils::GetIdType(const luabridge::LuaRef& comp)
{
    if (!comp.isTable() && !comp.isUserdata())
    {
        NEXUS_ERROR("无法获取类型id -- 该组件尚未暴露给lua!");
        return static_cast<entt::id_type>(-1);
    }

    const luabridge::LuaRef type_id = comp["type_id"];

    if (type_id.isCallable())
        return luabridge::call<entt::id_type>(type_id).valueOr(0);

    if (type_id.isNumber())
        return static_cast<entt::id_type>(type_id.cast<entt::id_type>().value());

    NEXUS_ERROR("无法获取类型id -- 组件缺少可用的 type_id 函数或属性!");
    return static_cast<entt::id_type>(-1);
}
