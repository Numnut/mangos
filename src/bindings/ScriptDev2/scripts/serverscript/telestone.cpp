
#include "sc_creature.h"
#include "sc_gossip.h"
#include "Player.h"
#include "Unit.h"
#include "../../../../game/GossipDef.h"
#include "../../../../game/Player.h"
#include "precompiled.h"
#include "../../../../shared/Config/Config.h"
#include "../../config.h"


/*#####
# custom_stone_summoner
#####*/
uint32 spamtimer;
uint32 Spawn_Timer;
uint32 check;




bool ItemUse_item_custom_stone(Player *player, Item* _Item, SpellCastTargets const& targets)
{
if ( (player->isInCombat()) || (player->isInFlight()) || (player->isDead()) || (player->IsMounted()))
{
player->SendEquipError(EQUIP_ERR_NOT_IN_COMBAT,_Item,NULL );
return false;
}

	player->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
	 if (player->HasSpellCooldown(40662) or /*battle grounds */player->GetMapId() ==  529 or player->GetMapId() == 489 or player->GetMapId() == 30 or player->GetMapId() == 566)
	{
	//	_Creature->MonsterWhisper("Please wait for the cooldown", player->GetGUID());
	} else {
		player->CastSpell(player, 40662, false);
		player->AddSpellCooldown(40662,0,time(NULL) + 60);

		player->SummonCreature(100080,player->GetPositionX() ,player->GetPositionY()+1, player->GetPositionZ()+1, 0,TEMPSUMMON_TIMED_DESPAWN,180000);
		return true;
	}
}


/*#####
# custom_stone_npc
#####*/

bool GossipHello_custom_stone_npc(Player *player, Creature *_Creature)
{
	// Make sure we can access the Config file
	/*if(!SD2Config.SetSource(_SCRIPTDEV2_CONFIG,true)){
		player->CLOSE_GOSSIP_MENU();
		error_log("SD2: Unable to open configuration file");
		_Creature->MonsterWhisper("I'm sorry, we are having technical difficulties.  Please check back later.", player->GetGUID());
		return false;
	}*/
	// Make sure player is not in combat
	if(!player->getAttackers().empty()){
		player->CLOSE_GOSSIP_MENU();
		_Creature->MonsterWhisper("You are in combat!", player->GetGUID());
		return false;
	}
	// If enabled, make sure only GM can use TelePorts
	/*if(SD2Config.GetBoolDefault("TeleNPC.OnlyGMs", false)){
		if(player->GetSession()->GetSecurity() == SEC_PLAYER){
			player->CLOSE_GOSSIP_MENU();
			_Creature->MonsterWhisper("Sorry, I am only allowed to teleport game masters.", player->GetGUID());
			return false;
		}
	}*/
	// Main Menu 
	if(player->HasItemCount(100180,1,false))
		{
	player->ADD_GOSSIP_ITEM( 5, "Cities ->"	                   , GOSSIP_SENDER_MAIN, 1000);		
	player->ADD_GOSSIP_ITEM( 5, "Dungeons ->"				   , GOSSIP_SENDER_MAIN, 2000);		
	player->ADD_GOSSIP_ITEM( 5, "Guilds ->"					   , GOSSIP_SENDER_MAIN, 3000);

    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());
	}else {
		_Creature->MonsterWhisper("You do not have the stone! To get one you need to donate!", player->GetGUID());
	}
return true;
}

void processLocations(Player *player, Creature *_Creature, uint32 action, uint32 more){
	std::string region;
	if(action==1000 || action==1100) region = "city";
	if(action==2005 || action==2105) region = "kalimdor";
	if(action==2010 || action==2110) region = "eastern kingdoms";
	if(action==2015 || action==2115) region = "outland";
	if(action==2020 || action==2120) region = "northrend";
	if(action==3000 || action==3100) region = "guild";

	QueryResult* pResult = SD2Database.PQuery("SELECT id,faction,reqlevel,guildID,Name,cost FROM telenet WHERE region=\"%s\" ORDER BY id",region.c_str());

	// Make sure the result is valid
	if(pResult){
		int32 count = 0;
		do{
			{
				Field* pFields = pResult->Fetch();
				if( (pFields[1].GetInt32()==2 && player->GetTeam()==ALLIANCE) || (pFields[1].GetInt32()==1 && player->GetTeam()==HORDE) || (player->getLevel()<pFields[2].GetInt32()) || (region=="guild" && player->GetGuildId() != pFields[3].GetInt32()) ){
					//FAIL - Player is wrong faction, level, or guild for this location, check the next one!
				}
				else{
					if( (more==0 && count<10) || (more==1 && count>9) ){
						//SUCCEED - Player can go here, add it to the menu
						// Convert copper to gold/silver/copper
												int32 gold = 0;
						int32 silver = 0;
						int32 copper = pFields[5].GetInt32();
						if(copper>9999){
							gold = floor(copper/10000.0);
							copper -= gold*10000;
						}
						if(copper>99){
							silver = floor(copper/100.0);
							copper -= silver*100;
						}
						std::stringstream menuItem;	
						menuItem << pFields[4].GetCppString();
						player->ADD_GOSSIP_ITEM( 5, menuItem.str().c_str(), GOSSIP_SENDER_MAIN, pFields[0].GetInt32());
					}
					if(more==1 && count<10) {
						count++; //Helps us skip the first 10 since we are on page 2
					}
					else{
						count++;
					}
				}
			}
		}while(pResult->NextRow());

		if(more==0 && count>10){
			int32 next;
			if(action==1000) next = 1100;
			if(action==2005) next = 2105;
			if(action==2010) next = 2110;
			if(action==2015) next = 2115;
			if(action==2020) next = 2120;
			if(action==3000) next = 3105;
			player->ADD_GOSSIP_ITEM( 5, "More ->", GOSSIP_SENDER_MAIN, next);
		}
		if(more==1){
			int32 prev;
			if(action==1000) prev = 1000;
			if(action>1000 && action<3000) prev = 2000;
			if(action==3000) prev = 3000;
			player->ADD_GOSSIP_ITEM( 5, "<- Back", GOSSIP_SENDER_MAIN, prev);
		}
		if(action==1000 || action==3000)
			player->ADD_GOSSIP_ITEM( 5, "<- Main Menu", GOSSIP_SENDER_MAIN, 4000);
		if(action>1000 && action<3000 && more==0)
			player->ADD_GOSSIP_ITEM( 5, "<- Back", GOSSIP_SENDER_MAIN, 2000);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());
	}
	return;
}

void SendDefaultMenu_custom_stone_npc(Player *player, Creature *_Creature, uint32 action)
{
	switch(action)
	{
		// -- Main Menu 
		case 4000: {
			player->ADD_GOSSIP_ITEM( 5, "Cities ->"	                   , GOSSIP_SENDER_MAIN, 1000);		
			player->ADD_GOSSIP_ITEM( 5, "Dungeons ->"				   , GOSSIP_SENDER_MAIN, 2000);		
			player->ADD_GOSSIP_ITEM( 5, "Guilds ->"					   , GOSSIP_SENDER_MAIN, 3000);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());
			return;
		}
		// -- Dungeon Menu 
		case 2000: {
			player->ADD_GOSSIP_ITEM( 5, "Kalimdor ->"                  , GOSSIP_SENDER_MAIN, 2005);
			player->ADD_GOSSIP_ITEM( 5, "Eastern Kingdoms ->"		   , GOSSIP_SENDER_MAIN, 2010);
			player->ADD_GOSSIP_ITEM( 5, "Outland ->"				   , GOSSIP_SENDER_MAIN, 2015);
			player->ADD_GOSSIP_ITEM( 5, "Northrend ->"				   , GOSSIP_SENDER_MAIN, 2020);
			player->ADD_GOSSIP_ITEM( 5, "<- Main Menu"				   , GOSSIP_SENDER_MAIN, 4000);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());
			return;
		}
		// -- Locations
		case 1000: { // Cities
			processLocations( player, _Creature, action, 0);
			return;
		}
		case 2005: { // Kalimdor
			processLocations( player, _Creature, action, 0);
			return;
		}
		case 2010: { // Eastern Kingdoms
			processLocations( player, _Creature, action, 0);
			return;
		}
		case 2015: { // Outland
			processLocations( player, _Creature, action, 0);
			return;
		}
		case 2020: { // Northrend
			processLocations( player, _Creature, action, 0);
			return;
		}
		case 3000: { // Guilds
			processLocations( player, _Creature, action, 0);
			return;
		}
		case 1100: { //Cities Overflow
			processLocations( player, _Creature, action, 1);
			return;
		}
		case 2105: { // Kalimdor Overflow
			processLocations( player, _Creature, action, 1);
			return;
		}
		case 2110: { // Eastern Kingdoms Overflow
			processLocations( player, _Creature, action, 1);
			return;
		}
		case 2115: { // Outland Overflow
			processLocations( player, _Creature, action, 1);
			return;
		}
		case 2120: { // Northrend Overflow
			processLocations( player, _Creature, action, 1);
			return;
		}
		case 3100: { // Guild Overflow
			processLocations( player, _Creature, action, 1);
			return;
		}
	} // end of switch

	// -- TELEPORT
	// If we've gotten this far, then 'action' doesn't match any of our 'case' numbers above
	// Since 'action' doesn't match a 'case' number, we must have an ID number available and be attempting a teleport
	QueryResult* pResult = SD2Database.PQuery("SELECT ID,Cost,MapID,Xpos,Ypos,Zpos,Rpos,Name FROM telenet ORDER BY id");

	if(pResult){
		do{
			Field* pFields = pResult->Fetch();
			// Check if this row is the teleport location we want
			if(pFields[0].GetInt32() == action){
				// Convert copper to gold/silver/copper
				int32 gold = 0;
				int32 silver = 0;
				int32 copper = pFields[1].GetInt32();
				
				if(copper>9999){
					gold = floor(copper/10000.0);
					copper -= gold*10000;
				}
				if(copper>99){
					silver = floor(copper/100.0);
					copper -= silver*100;
				}
				// Check if we have enough money
				if(player->GetMoney() < copper){
						std::stringstream ss;
		//				ss << "You do not have enough money to teleport here.  You need " << gold << " gold, " << silver << " silver, and " << copper << " copper.";
				//		_Creature->MonsterWhisper(ss.str().c_str(), player->GetGUID());
				}
				else{
					// Player has enough money! Do the teleport!
					std::stringstream ss;
			//		ss << "You spent " << gold << " gold, " << silver << " silver, and " << copper << " copper on your teleport to " << pFields[7].GetCppString() << ".";
		//			_Creature->MonsterWhisper(ss.str().c_str(), player->GetGUID());
				    
					player->CLOSE_GOSSIP_MENU();
					player->TeleportTo(pFields[2].GetInt32(),pFields[3].GetInt32(),pFields[4].GetInt32(),pFields[5].GetInt32(),pFields[6].GetInt32());
					player->ModifyMoney(-pFields[1].GetInt32());
				}
			}
		}while(pResult->NextRow());
	}
return;
} //end of function 'SendDefaultMenu_telenpc'


bool GossipSelect_custom_stone_npc(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
	// Main menu
	if (sender == GOSSIP_SENDER_MAIN)
	SendDefaultMenu_custom_stone_npc( player, _Creature, action );

return true;
}

void AddSC_telestone()
{
    Script *newscript;

newscript = new Script;
newscript->Name="item_custom_stone";
newscript->pItemUse = ItemUse_item_custom_stone;
    newscript->RegisterSelf();

newscript = new Script;
newscript->Name="custom_stone_npc";
newscript->pGossipHello = &GossipHello_custom_stone_npc;
newscript->pGossipSelect = &GossipSelect_custom_stone_npc;
    newscript->RegisterSelf();
}
