#include "app.h"

app app_list[MAX_APPS] = {0};

//voegt een app toe aan het telfoon boekje van de kernel
uint32_t init_app(char name[4], char start[], char size[]){
    app new_app;
    for (int i = 0; i < 11 && name[i] != '\0'; i++) {
        new_app.name[i] = name[i];
    }
    new_app.name[4] = '\0';
    new_app.start = (uint32_t)start;
    new_app.size = (uint32_t)size;

    for(uint32_t i = 0; i < MAX_APPS; i++){
        if (app_list[i].size == 0){app_list[i] = new_app; return i;}
    }
    k_panic("\nno free app slots found for %s\n", name);
}

//start een app
uint32_t start_app(uint32_t app_id){
    if (app_id > MAX_APPS || app_list[app_id].size == 0)k_panic("tried to start a non vaild app_id: %d\n", app_id);
    return spawn_proc(app_list[app_id].start, app_list[app_id].size)->pid;//geef het niewe pid tereug
}

uint32_t get_app_id(char app_name[5]){
    for(uint32_t i = 0; i < MAX_APPS; i++){
        if(!k_strcmp(app_name, app_list[i].name))return i;
    }
    return 0xFFFFFFFF;
}

uint32_t get_app_name(uint32_t app_id){
    if(app_id > MAX_APPS)k_panic("app id %d does not exist\n", app_id);
    return ((uint32_t) app_list[app_id].name[0] << 24) | ((uint32_t) app_list[app_id].name[1] << 16) | ((uint32_t) app_list[app_id].name[2] << 8) | ((uint32_t) app_list[app_id].name[3]);
}
