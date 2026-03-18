#include <nlohmann/json.hpp>      // full include is fine here
#include <Orion/Realm.h>

namespace Orion
{
    // Optionally: define a local alias ONLY in this header (not in Realm.h)
    using Json = nlohmann::json;

    void to_json(Json& j, const Realm& r);
    void from_json(const Json& j, Realm& r);
}