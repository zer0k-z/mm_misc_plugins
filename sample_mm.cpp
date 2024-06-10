/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * ======================================================
 * Metamod:Source Sample Plugin
 * Written by AlliedModders LLC.
 * ======================================================
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software.
 *
 * This sample plugin is public domain.
 */

#include <stdio.h>
#include "sample_mm.h"
#include "iserver.h"
#include "sql_mm/src/public/sql_mm.h"
#include "sql_mm/src/public/sqlite_mm.h"

SamplePlugin g_SamplePlugin;
IServerGameDLL *server = NULL;
IServerGameClients *gameclients = NULL;
IVEngineServer *engine = NULL;
IGameEventManager2 *gameevents = NULL;
ICvar *icvar = NULL;

ISQLConnection *databaseConnection;

// Should only be called within the active game loop (i e map should be loaded and active)
// otherwise that'll be nullptr!
CGlobalVars *GetGameGlobals()
{
	INetworkGameServer *server = g_pNetworkServerService->GetIGameServer();

	if(!server)
		return nullptr;

	return g_pNetworkServerService->GetIGameServer()->GetGlobals();
}

#if 0
// Currently unavailable, requires hl2sdk work!
ConVar sample_cvar("sample_cvar", "42", 0);
#endif

CON_COMMAND_F(sample_command, "Sample command", FCVAR_NONE)
{
	META_CONPRINTF( "Sample command called by %d. Command: %s\n", context.GetPlayerSlot(), args.GetCommandString() );
}

PLUGIN_EXPOSE(SamplePlugin, g_SamplePlugin);
bool SamplePlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	GET_V_IFACE_ANY(GetServerFactory, gameclients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
	GET_V_IFACE_ANY(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);

	return true;
}

bool SamplePlugin::Unload(char *error, size_t maxlen)
{
	return true;
}

void OnGenericTxnSuccess(std::vector<ISQLQuery *> queries)
{
	ConMsg("[KZDB] Transaction successful.\n");
}

void OnGenericTxnFailure(std::string error, int failIndex)
{
	ConMsg("[KZDB] Transaction failed at %i (%s).\n", failIndex, error.c_str());
}

void OnDatabaseConnected(bool connect)
{
	if (connect)
	{
		META_CONPRINT("DB connected.\n");
		Transaction txn;
		txn.queries.push_back("create table my_testies (id integer primary key autoincrement, name text);");
		databaseConnection->ExecuteTransaction(txn, OnGenericTxnSuccess, OnGenericTxnFailure);
	}
	else
	{
		META_CONPRINT("Failed to connect\n");
		databaseConnection->Destroy();
		databaseConnection = nullptr;
	}
	return;
}

void SamplePlugin::AllPluginsLoaded()
{
	/* This is where we'd do stuff that relies on the mod or other plugins 
	 * being initialized (for example, cvars added and events registered).
	 */
	ISQLInterface *sqlInterface = (ISQLInterface *)g_SMAPI->MetaFactory(SQLMM_INTERFACE, nullptr, nullptr);
	SQLiteConnectionInfo info;
	info.database = "addons/cs2kz/data/test.sqlite3";
	databaseConnection = sqlInterface->GetSQLiteClient()->CreateSQLiteConnection(info);
	databaseConnection->Connect(OnDatabaseConnected);
}

bool SamplePlugin::Pause(char *error, size_t maxlen)
{
	return true;
}

bool SamplePlugin::Unpause(char *error, size_t maxlen)
{
	return true;
}

const char *SamplePlugin::GetLicense()
{
	return "Public Domain";
}

const char *SamplePlugin::GetVersion()
{
	return "1.0.0.0";
}

const char *SamplePlugin::GetDate()
{
	return __DATE__;
}

const char *SamplePlugin::GetLogTag()
{
	return "SAMPLE";
}

const char *SamplePlugin::GetAuthor()
{
	return "AlliedModders LLC";
}

const char *SamplePlugin::GetDescription()
{
	return "Sample basic plugin";
}

const char *SamplePlugin::GetName()
{
	return "Sample Plugin";
}

const char *SamplePlugin::GetURL()
{
	return "http://www.sourcemm.net/";
}
