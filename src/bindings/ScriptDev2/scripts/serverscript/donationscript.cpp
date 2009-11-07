#include "sc_creature.h"
#include "sc_gossip.h"
#include "Player.h"
#include "Unit.h"
#include "../../../../game/GossipDef.h"
#include "../../../../game/Player.h"
#include "precompiled.h"
#include "../../../../shared/Config/Config.h"
#include "../../config.h"
#include "../../../../game/Language.h"
#include "../../../../shared/Database/DatabaseMysql.h" //Wasn't entirely sure if I needed this one so added it just in case
#include "Database/DatabaseEnv.h"

extern DatabaseMysql SD2Database;

bool GossipHello_donationpoints(Player *player, Creature *_Creature)
{


    //Some examples of what can be done:
    player->ADD_GOSSIP_ITEM( 5, "Main"                    , GOSSIP_SENDER_MAIN, 6000);

		player->PlayerTalkClass->SendGossipMenu(6000,_Creature->GetGUID());
    return true;
}






void processdonations(std::string typedonation, Creature* _Creature, Player* player, uint32 more){
//std::stringstream typedonation
//typedonation = typestring;

	QueryResult* pResult = SD2Database.PQuery("SELECT id,name,class,type,cost FROM donationlist WHERE type=\"%s\" ORDER BY id", typedonation.c_str());
	
	// Make sure the result is valid
	if(pResult){
		int32 count = 0;
		do{
			{
				Field* pFields = pResult->Fetch();
				if(pFields[2].GetInt32() == player->getClass() or pFields[2].GetInt32() == 0){

					if( (more==0 && count<10) || (more==1 && count>9) ){
						std::stringstream menuItempoints;
						menuItempoints << pFields[1].GetCppString() << "(" << pFields[4].GetInt32() << " Points)";
						std::stringstream menuItem;
						menuItem << pFields[1].GetCppString() << " >>";
						if(pFields[4].GetInt32() == 0)
						{
							player->ADD_GOSSIP_ITEM( 5, menuItem.str().c_str(), GOSSIP_SENDER_MAIN, pFields[0].GetInt32());
						} else {
							player->ADD_GOSSIP_ITEM( 5, menuItempoints.str().c_str(), GOSSIP_SENDER_MAIN, pFields[0].GetInt32());
						}
					}
					if(more==1 && count<10) {
						count++; //Helps us skip the first 10 since we are on page 2
					}
					else{
						count++;
					}
				}
				else{


				}
			}
		}while(pResult->NextRow());

		/*if(more==0 && count>10){
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
		if(action>1000 && action<3000 && more==0)*/
			player->ADD_GOSSIP_ITEM( 5, "<- Back", GOSSIP_SENDER_MAIN, 6000); 
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());
	}
	return;
}






//Giving the Item
void GiveItem(uint32 itemId, int32 count, Creature* _Creature, Player* player, uint32 donationid, std::string type, int32 points, int32 playerpoints)
{    	

		int32 playerid = player->GetGUID();
		int32 accountid = player->GetSession()->GetAccountId();

            uint32 noSpaceForCount = 0;
            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount );
      if( (playerpoints - points) >= 0)
	  {
            if( msg != EQUIP_ERR_OK )                               // convert to possible store amount
                count -= noSpaceForCount;

            if( count == 0 || dest.empty())                         // can't add any
            {
                _Creature->MonsterWhisper("Make sure you have enough room in your bags", player->GetGUID()); 
            } else {

            Item* item = player->StoreNewItem( dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));
                player->SendNewItem(item,count,false,true);
            	SD2Database.PExecute("UPDATE realmd.account SET donationpoints = donationpoints-'%u' WHERE id = '%u'", points , player->GetSession()->GetAccountId());
SD2Database.PExecute("INSERT INTO realmd.DonationLog (`Character`, `Account`, `ItemName`,`Type`) VALUES ('%u', '%u', '%u', '%u')", playerid , accountid , donationid, type.c_str()); 
			}
		} else {
			_Creature->MonsterWhisper("You do not have enough Donation Points!", player->GetGUID()); 
			}
    player->CLOSE_GOSSIP_MENU();
}  

//Giving the Itemset
void GiveItemSet(uint32 itemsetid, int32 count, Creature* _Creature, Player* player, int32 donationid, std::string type, int32 points, int32 playerpoints, int32 SlotsFree, int32 flag, std::string special)
{    	
		
		int32 playerid = player->GetGUID();
		int32 accountid = player->GetSession()->GetAccountId();
		int32 playerlevel80 = 0;
            uint32 noSpaceForCount = 0;
            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, 23162, SlotsFree, &noSpaceForCount );
      if( (playerpoints - points) >= 0)
	  {
            if( msg != EQUIP_ERR_OK )                               // convert to possible store amount
                count -= noSpaceForCount;

            if( count > 0 )                         // can't add any
            {
					QueryResult *result;
				if(flag == 2) //flag(cag) = 2 is for itemsets tier 7 and 8
				{
				result = SD2Database.PQuery("SELECT entry FROM mangos.item_template WHERE itemset = '%u' AND name LIKE \"\%s\%\"", itemsetid, special.c_str());
				
				
				}else{
					result = SD2Database.PQuery("SELECT entry FROM mangos.item_template WHERE itemset = '%u'", itemsetid);
				}
					do
					{
						Field *fields = result->Fetch();
						uint32 itemId = fields[0].GetUInt32();
						ItemPosCountVec dest;
						player->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, itemId, 1 );
						Item* item = player->StoreNewItem( dest, itemId, true);
						player->SendNewItem(item,1,false,true);
					}	
						while(result->NextRow());
						delete result;
						
											if(flag == 1)
					{

						playerlevel80 = 80 - player->getLevel();
						player->GiveLevel(player->getLevel() + playerlevel80); 					
					}
            	SD2Database.PExecute("UPDATE realmd.account SET donationpoints = donationpoints-'%u' WHERE id = '%u'", points , player->GetSession()->GetAccountId());
							SD2Database.PExecute("INSERT INTO realmd.DonationLog (`Character`, `Account`, `ItemName`,`Type`) VALUES ('%u', '%u', '%u', '%s')", playerid , accountid , donationid, type.c_str()); 
								player->CLOSE_GOSSIP_MENU();
					
            } else {


				_Creature->MonsterWhisper("Make sure you have enough room in your bags", player->GetGUID()); 

			}
		} else {
			_Creature->MonsterWhisper("You do not have enough Donation Points!", player->GetGUID()); 
			}
    player->CLOSE_GOSSIP_MENU();
}  
void SendDefaultMenu_donationpoints(Player *player, Creature *_Creature, uint32 action)
{
    QueryResult *result;
    //This query is just an example. I suggest using the account table in the realmd database to store the users
    //vote points. It makes it easier to get the account that belongs to the user ingame.
    result = SD2Database.PQuery("SELECT donationpoints FROM realmd.account WHERE id = '%u'", player->GetSession()->GetAccountId());
    Field *Fields = result->Fetch();
    int32 donationpoints = Fields[0].GetUInt32(); //Get x Vote Points from database

	
	//defining
	int32 RequireSlots = 0;
	int32 Points = 0;
	int32 points = 0;
	int32 DonationId = 0;
	int32 itemsetid = 0;
	int32 ItemNumber = 0;
	int32 playerlevel80 = 0;
	int32 requirepoints = 0;
	
	std::stringstream howmany;
	std::string Items;
	Items = "Items";
	std::string ItemSets;
	ItemSets = "ItemSets";
	std::string Mounts;
	Mounts = "Mounts";
	
	int32 playerid = player->GetGUID();
	int32 accountid = player->GetSession()->GetAccountId();
	int32 type = 0;

    // Disable npc if in combat
    if(!player->getAttackers().empty())
    {
        player->CLOSE_GOSSIP_MENU();
        _Creature->MonsterSay("You are in combat!", LANG_UNIVERSAL, NULL);
        return;
    }

    switch(action)
    {
//***********START MAINMENU*************//			
        case 6000: //Back to Main Menu
   if(donationpoints > 0)
   {
// Main Menu
    player->ADD_GOSSIP_ITEM( 5, "How Many Donations Points Do I Have?"                        , GOSSIP_SENDER_MAIN, 5000);
   player->ADD_GOSSIP_ITEM( 5, "Levels"                    , GOSSIP_SENDER_MAIN, 1000);
    player->ADD_GOSSIP_ITEM( 6, "Items"                            , GOSSIP_SENDER_MAIN, 2000);
	player->ADD_GOSSIP_ITEM( 5, "ItemSets"                    , GOSSIP_SENDER_MAIN, 3000);  
    player->ADD_GOSSIP_ITEM( 5, "Mounts"                        , GOSSIP_SENDER_MAIN, 4000);
		player->PlayerTalkClass->SendGossipMenu(6000,_Creature->GetGUID());
	}
else
{
	_Creature->MonsterWhisper("You have no Donations Points to spend", player->GetGUID());
}
   
        break;
//***********END MAINMENU*************//
//
    case 5000: 
		howmany<< "You Have " << donationpoints << " Donations Points";
		_Creature->MonsterWhisper(howmany.str().c_str(), player->GetGUID());
		player->ADD_GOSSIP_ITEM( 5, "Back" , GOSSIP_SENDER_MAIN, 6000);
		player->PlayerTalkClass->SendGossipMenu(6000,_Creature->GetGUID());
	break;
//***********START LEVEL***********//
    case 1000: 
		player->ADD_GOSSIP_ITEM( 5, "Level 1-80 & Tier 6 (40 Points)" , GOSSIP_SENDER_MAIN, 13001);
		player->ADD_GOSSIP_ITEM( 5, "Level 70-80 (15 Points)" , GOSSIP_SENDER_MAIN, 13002);
		player->ADD_GOSSIP_ITEM( 5, "Back" , GOSSIP_SENDER_MAIN, 6000);
		player->PlayerTalkClass->SendGossipMenu(6000,_Creature->GetGUID());
	break;
	
			// 8 = Level 80
	// 9 = Level 70 to 80
	case 13001:
	requirepoints = 40;
	type = 8;
	if(player->getLevel() < 21)
		{
		Items = "Tier 6 & Level 80";
		processdonations(Items.c_str(), _Creature, player, 0);  
		} else {
		_Creature->MonsterWhisper("You can not use this because your level is above 20!", player->GetGUID());
					player->CLOSE_GOSSIP_MENU();	
		}
	break;
	///////////////////////////////////////////////////////////////////////////////////////////////////////
    case 13002: 
		requirepoints = 15;
		type = 9;
	if(player->getLevel() < 76 && player->getLevel() > 69)
	{
		if((donationpoints - points) >= 0)
		{
						
    					   playerlevel80 = 80 - player->getLevel();
					player->GiveLevel(player->getLevel() + playerlevel80); 
				SD2Database.PExecute("UPDATE realmd.account SET donationpoints = donationpoints-'%u' WHERE id = '%u'", donationpoints , player->GetSession()->GetAccountId());
				SD2Database.PExecute("INSERT INTO realmd.DonationLog (`Character`, `Account`, `Type`) VALUES ('%u', '%u', '%u')", playerid , accountid , type); 
				player->CLOSE_GOSSIP_MENU();
		} else {		
				_Creature->MonsterWhisper("You don't have enough Donation Points to purchase that reward!", player->GetGUID());
				player->CLOSE_GOSSIP_MENU();
		}
	} else {
		_Creature->MonsterWhisper("You can not use this because your level is above 75 or your level is lower then 70!", player->GetGUID());
		player->CLOSE_GOSSIP_MENU();	
	}
	break;
		
		
	
//***********END LEVEL********//	
//	

	//items
	case 2000:
	processdonations(Items.c_str(), _Creature, player, 0);  
	break;
	//itemsets
	case 3000:
	processdonations(ItemSets.c_str(), _Creature, player, 0);  
	break;
	//mounts
	case 4000:
	processdonations(Mounts.c_str(), _Creature, player, 0);  
	break;


    } //End switch
	QueryResult* pResult = SD2Database.PQuery("SELECT id,name,itemid,cost,itemsets,requiredslots,cag,type,special FROM donationlist ORDER BY id");


int32 leveltype = 0;

	if(pResult){
		do{
			Field* pFields = pResult->Fetch();
		std::string itemname;
		itemname = pFields[1].GetCppString();
		std::string nametype;
		nametype = pFields[6].GetCppString();
		std::string special;
		special = pFields[8].GetCppString();
				
			// Check if this row is the teleport location we want
			if(pFields[0].GetInt32() == action){
					if(pFields[4].GetInt32() == 1 && pFields[6].GetInt32() <= 2) //for itemsets
					{			//itemid					//crea	/player	
					GiveItemSet(pFields[2].GetInt32(), 1, _Creature, player, pFields[0].GetInt32(), nametype.c_str(), pFields[3].GetInt32(), donationpoints, pFields[5].GetInt32(), pFields[6].GetInt32(), special.c_str());  
					}
					if(pFields[4].GetInt32() == 0 && pFields[6].GetInt32() <= 1){ //none itemsets
					GiveItem(pFields[2].GetInt32(), 1, _Creature, player, pFields[0].GetInt32(), nametype.c_str(), pFields[3].GetInt32(), donationpoints);  
					}
					if(pFields[6].GetInt32() == 3) //for cags for itemsets
					{
					processdonations( itemname.c_str() , _Creature, player, 0);  
					}
				}
			
		}while(pResult->NextRow());
	}
} //End function




bool GossipSelect_donationpoints(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    // Main menu
    if (sender == GOSSIP_SENDER_MAIN)
    SendDefaultMenu_donationpoints( player, _Creature, action );

return true;
}
void AddSC_donationscript()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "donationpoints";
    newscript->pGossipHello = &GossipHello_donationpoints;
    newscript->pGossipSelect = &GossipSelect_donationpoints;
    newscript->RegisterSelf();
}