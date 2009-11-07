#include "../../../../game/Object.h"
#include "../../../../game/Player.h"
#include "../../../../game/BattleGround.h"
#include "../../../../game/BattleGroundMgr.h"
#include "../../../../game/Creature.h"
#include "../../../../game/MapManager.h"
#include "../../../../game/Language.h"
#include "../../../../game/SpellAuras.h"
#include "../../../../game/ArenaTeam.h"
#include "../../../../game/World.h"
#include "../../../../game/Group.h"
#include "../../../../game/ObjectMgr.h"
#include "../../../../game/WorldPacket.h"
#include "../../../../game/Util.h"
#include "../../../../game/Formulas.h"
#include "../../../../game/GridNotifiersImpl.h"
#include "../../../../game/SharedDefines.h"
#include "precompiled.h"
#include "sc_gossip.h"
bool GossipHello_test(Player *player, Creature *_Creature)
{
player->ADD_GOSSIP_ITEM( 5, "Add Me to the Group" , GOSSIP_SENDER_MAIN, 1);
player->ADD_GOSSIP_ITEM( 5, "Nothing" , GOSSIP_SENDER_MAIN, 2);
}

bool GossipSelect_test(Player *player, Creature *_Creature, uint32 sender, uint32 action )

{
// Main menu
if (sender == GOSSIP_SENDER_MAIN)
AddPlayerToGroup();
return true;

}

void Reset()
{
	Team = 0;
	PlayerCount = 0;
}

void AddPlayerToGroup()
{

  Group* group = team;
    if(!group)                                      // first player joined
    {
        group = new Group;
        SetBgRaid(team, group);
        group->Create(plr_guid, plr->GetName());
    }
    else                                            // raid already exist
    {
        if (group->IsMember(plr_guid))
        {
            uint8 subgroup = group->GetMemberGroup(plr_guid);
            plr->SetBattleGroundRaid(group, subgroup);
        }
        else
        {
            group->AddMember(plr_guid, plr->GetName());
            if (Group* originalGroup = plr->GetOriginalGroup())
                if (originalGroup->IsLeader(plr_guid))
                    group->ChangeLeader(plr_guid);
        }
    }
	if(team==0)
	{ team = 1; }else team = 0;
	//then tele theplayer
}
void Updater(const uint32 diff)
{

}

void AddSC_test()
{
    Script *newscript;

newscript = new Script;
newscript->Name="test";
newscript->pGossipHello = &GossipHello_test;
newscript->pGossipSelect = &GossipSelect_test;
newscript->pItemHello = NULL;
newscript->pGOHello = NULL;
newscript->pAreaTrigger = NULL;
newscript->pItemQuestAccept = NULL;
newscript->pGOQuestAccept = NULL;
newscript->pGOChooseReward = NULL;
    newscript->RegisterSelf();
}