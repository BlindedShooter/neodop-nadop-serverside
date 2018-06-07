// matching_service.cc
#include "matching_service.h"

namespace demo {
using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;

Persistent<Function> Grid_Object::constructor;
GridCandidateService Grid_Object::candidate_service;

Grid_Object::Grid_Object() { 
}

Grid_Object::~Grid_Object(){
}

void Grid_Object::Init(Local<Object> exports) {
    Isolate* isolate = exports->GetIsolate();
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "Matching_Service"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

 // Prototype
    //NODE_SET_PROTOTYPE_METHOD(tpl, "plusOne", PlusOne);
    NODE_SET_PROTOTYPE_METHOD(tpl, "update", update_user);
    NODE_SET_PROTOTYPE_METHOD(tpl, "search", find_radius);
    NODE_SET_PROTOTYPE_METHOD(tpl, "get_users", get_all_users);
    NODE_SET_PROTOTYPE_METHOD(tpl, "clean", clean_grid);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "Matching_Service"), tpl->GetFunction());
}

void Grid_Object::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new MyObject(...)`
        //double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
        Grid_Object* obj = new Grid_Object();
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    } else {
        // Invoked as plain function `MyObject(...)`, turn into construct call.
        const int argc = 0;
        //Local<Value> argv[argc] = { args[0] };
        Local<Context> context = isolate->GetCurrentContext();
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        Local<Object> result = cons->NewInstance(context, argc, NULL).ToLocalChecked();
        args.GetReturnValue().Set(result);
    }
}

/* .update(double lat, double long, string uid, double time)*/
void Grid_Object::update_user(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    
    if (args.Length() < 4) {
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "wrong number of arguments"));
        return;
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsString() || !args[3]->IsNumber()) {
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "wrong argument types"));
        return;
    }

    double lat = args[0]->NumberValue();
    double lon = args[1]->NumberValue();
    v8::String::Utf8Value param1(args[2]->ToString());
    uid_t uid_ = std::string(*param1);
    time_t t = static_cast<time_t>(args[3]->NumberValue());

    candidate_service.update_user(uinfo_t(lat, lon, uid_, t));
    
    /*
    char buff[100];
    snprintf(buff, sizeof(buff), "uid: %s, time: %lld, lat: %lf, lon: %lf", uid_.c_str(), t, lat, lon); */
    
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, "update successful"));
}

/* .find(double lat, double long, double radius, double target_num). */
void Grid_Object::find_radius(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    int radius, target_num;

    if (args.Length() < 2) {
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "wrong number of arguments"));
        return;
    }
    if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "wrong argument types"));
        return;
    }
    
    double lat = args[0]->NumberValue();
    double lon = args[1]->NumberValue();

    if (args.Length() < 4) {
        radius = 3;
        target_num = 10;
    }
    else if (!args[2]->IsNumber() || !args[3]->IsNumber()) {
        return;
    }
    else {
        radius = static_cast<int>(args[2]->NumberValue());
        target_num = static_cast<int>(args[3]->NumberValue());
    }

    uservec_t candidates = candidate_service.search_grid(
        coord(lat, lon), radius, target_num);

    v8::Handle<v8::Array> arr = v8::Array::New(isolate, candidates.size());

    for (unsigned int i = 0; i < arr->Length(); i++) {
        arr->Set(i, v8::String::NewFromUtf8(isolate, candidates[i].c_str()));
    }

    args.GetReturnValue().Set(arr);
}


void Grid_Object::get_all_users(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    coordvec_t users = candidate_service.get_all_users();
    std::string result;
    int i = 0;
    for (auto const& user : users) {
        result += "#";
        result += std::to_string(i++);
        result += "   uid: ";
        result += user.uid;
        result += ", lat: ";
        result += std::to_string(user.lat);
        result += ", lon: "; 
        result += std::to_string(user.lon);
        result += ", Grid coordinate: (";
        result += std::to_string(user.x);
        result += ", ";
        result += std::to_string(user.y);
        result += ")\n";
    }
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, result.c_str()));
}

void Grid_Object::clean_grid(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    candidate_service.cleanup_old_users();
}

}
