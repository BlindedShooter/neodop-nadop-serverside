// matching_addon.cc
#include <node.h>
#include "matching_service.h"

namespace demo {
    using v8::Local;
    using v8::Object;

    void InitAll(Local<Object> exports) {
        Grid_Object::Init(exports);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, InitAll)
}  // namespace demo