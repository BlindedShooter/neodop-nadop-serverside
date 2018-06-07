// matching_service.h
#ifndef GRID_OBJECT_H
#define GRID_OBJECT_H

#include <node.h>
#include <node_object_wrap.h>
#include "./GridService/GridCandidateService.h"

namespace demo {
    class Grid_Object : public node::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> exports);
        

    private:
        explicit Grid_Object();
        ~Grid_Object();

        static GridCandidateService candidate_service;

        // external methods
        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        
        /* .update(double lat, double long, string uid, double time)*/
        static void update_user(const v8::FunctionCallbackInfo<v8::Value>& args);
        /* .search(double lat, double long, double radius, double target_num).  radius is that of Grid, not meters or somthing.*/
        static void find_radius(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void get_all_users(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void clean_grid(const v8::FunctionCallbackInfo<v8::Value>& args);


        static v8::Persistent<v8::Function> constructor;
    };
};



#endif