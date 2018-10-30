
#include "DispatchMsgService.h"
#include "interface.h"
#include "Logger.h"
#include "sqlconnection.h"
#include "BusProcessor.h"

#include <functional>

#include <string.h>
extern "C"
{
    #include "lauxlib.h"  
    #include "lualib.h"  
    #include "lua.h"
}
typedef struct conf{
    char ip[10];
    int port;
    char userName[20];
    char password[20];
    char sid[20];
}Conf;

Conf config;
int brksPort;

void getConfig(){
	
	lua_State* L;
    //initialize Lua
    L = luaL_newstate();
    //load Lua base libraries
    luaL_openlibs(L);
    //load the script
    if(luaL_dofile(L, "../conf/brks_conf.lua") || lua_pcall(L, 0 , 0, 0)){
        cout << "dofile error " << lua_tostring(L, -1);
        exit;
    }
	
	//get server port
	lua_getglobal(L, "brks_port");
	brksPort = lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	//get database info
    memset(&config, 0, sizeof(Conf));
	//get configure information
	lua_getglobal(L, "db_conf");
	
	//ip
    lua_getfield(L, -1, "ip");
    if(!lua_isstring(L, -1)){
            cout << "ip is not string" << endl;
    }
	strcpy(config.ip, lua_tostring(L, -1));
	lua_pop(L, 1);
	
	//port
    lua_getfield(L, -1, "port");
    if(!lua_isnumber(L, -1)){
            cout << "port is not number" << endl;
    }
	config.port = lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	//userName
    lua_getfield(L, -1, "userName");
    if(!lua_isstring(L, -1)){
            cout << "userName is not string" << endl;
    }
	strcpy(config.userName, lua_tostring(L, -1));
	lua_pop(L, 1);
    
	//password
	lua_getfield(L, -1, "password");
    if(!lua_isstring(L, -1)){
            cout << "password is not string" << endl;
    }
	strcpy(config.password, lua_tostring(L, -1));
	lua_pop(L, 1);
    
	//sid
	lua_getfield(L, -1, "sid");
    if(!lua_isstring(L, -1)){
            cout << "sid is not string" << endl;
    }
	strcpy(config.sid, lua_tostring(L, -1));
	lua_pop(L, 1);

	lua_pop(L, 1);
	lua_close(L);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("please input brks <log file config>!\n");
        return -1;
    }
    
    if(!Logger::instance()->init(std::string(argv[1])))
    {
        printf("init log module failed.\n");
        return -1;
    }
    else
    {
        printf("init log module success!");
    }

    std::shared_ptr<DispatchMsgService> dms(new DispatchMsgService);
    dms->open();
    
    std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);
//    mysqlconn->Init("127.0.0.1", 3306, "root", "123456", "dongnaobike");
	//get information from lua
	getConfig();
    mysqlconn->Init(config.ip, config.port, config.userName, config.password, config.sid);
    
    BusinessProcessor processor(dms, mysqlconn);
    processor.init();

    std::function< iEvent* (const iEvent*)> fun = std::bind(&DispatchMsgService::process, dms.get(), std::placeholders::_1);
    
    Interface intf(fun);
    //intf.start(9090);
	intf.start(brksPort);

    LOG_INFO("brks start successful!");

    for(;;);
    
    return 0;
}