#include "precompiled.h"
#define DOOR1       0
#define BOSS1       1
#define BOSS2       2
#define BOSS3       3
#define SWAP_Orginal       10
#define SWAP_SWAP     11

#define INTRO1         -1900001
#define INTRO2         -1900000
#define INTRO3         -1900002


//spawn 563.539551 213.565628 395.098206 4.754064
//where they walk to 563.589294 139.645645 393.836578 4.714795
enum
{
    //List of text id's. The text is stored in database, also in a localized version
    //(if translation not exist for the textId, default english text will be used)
    //Not required to define in this way, but simplify if changes are needed.
	//NPC IDS
	Dreadscale = 34799,
	Acidmaw = 35144,
	Gormok = 34796,
	TirionFordring = 34996,
	Icehowl = 34797,

    //List of spells.
	
	//BOSS Gormok // FIRST BOSS
    H_stomp   = 67649, // heroic
	stomp   = 66330,
	H_impale = 67477,
    impale  = 67478,
    whirl   = 63666,
	FireBomb = 66313,
	HeadCrack = 66407,
	Batter = 66408,
	RisingAnger = 36300, //workaround
	
	
	//AcidMaw
		//mobile phase
		acidicspew = 66819,
		slimepool  = 66883,
		paralyticbite = 66824,
		//stationary phase
		acidspit = 66880,
		sweep = 66794,
		paralyticspray = 66901,
		enrage = 68335,
		Emerge = 449,
		SubMerge = 373, //not used anymore
		
 //DreadScale Boss
 
		// mobile phase
		burningbite = 66879,
		moltenspew = 66820,
		// statonary phase
		firespit = 66796,
		burningspray = 66902,
	
	 MAX_ENCOUNTER           = 5, // 5 ecounters we will go through

    SAY_INTRO         = -1033007,
	
	//DOOR
    GO_MAIN_DOOR       = 195647,                        //door to open when talking to NPC's
	


};



struct MANGOS_DLL_DECL npc_dreadscaleAI : public ScriptedAI
{
npc_dreadscaleAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 acidicspew_timer; //phase 1  stationary first
	uint32 slimepool_timer;
	uint32 paralyticbite_timer;
	uint32 acidspit_timer;//phase 2
	uint32 sweep_timer;
	uint32 paralyticspray_timer;
	bool once;
	bool Swap;
		
	void Reset()
{

	acidicspew_timer = 5400;
	slimepool_timer = 10000;
	paralyticbite_timer =  4000;
	
	paralyticspray_timer = 12000;
	sweep_timer =  7000;
	acidspit_timer = 16000;
	


}

void Aggro(Unit *who)
	{
		m_creature->MonsterSay("Agro'ed!", LANG_UNIVERSAL, 0);
		Creature *pAcidmaw = m_creature->SummonCreature(Acidmaw,538.347, 165.509, 394.6500, 5.431659,TEMPSUMMON_MANUAL_DESPAWN, 0);
		pAcidmaw->HandleEmoteCommand(Emerge);
	}

void UpdateAI(const uint32 diff )
	{
		ScriptedInstance* pInstance = ((ScriptedInstance*)m_creature->GetInstanceData());
		//Return since we have no target
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

			//BEGIN AI

				//BOSS 1
			if(pInstance && pInstance->GetData(100) == SWAP_Orginal)
			{
				if(paralyticspray_timer < diff)      //para spray
				{
					m_creature->MonsterSay("SPRAYYYYYYY", LANG_UNIVERSAL, 0);
					DoCast(m_creature->getVictim(),paralyticspray);
					paralyticspray_timer = 12000;
				}else paralyticspray_timer -= diff;
				
				if(sweep_timer < diff)           //sweep
				{	
					DoCast(m_creature->getVictim(),sweep);
					sweep_timer =   7000;
					m_creature->MonsterSay("SWEEP!!", LANG_UNIVERSAL, 0);
				}else sweep_timer -= diff;
				
				if(acidspit_timer < diff)           //acid spit
				{	
					DoCast(m_creature->getVictim(),acidspit);
					acidspit_timer = 16000;
					m_creature->MonsterSay("Acid Spit!!", LANG_UNIVERSAL, 0);
				}else acidspit_timer -= diff;
			}else{
				if( paralyticbite_timer < diff)      //para bite
				{
					m_creature->MonsterSay("BITTTTEEEEE", LANG_UNIVERSAL, 0);
					DoCast(m_creature->getVictim(), paralyticbite);
					 paralyticbite_timer = 30000;
				}else  paralyticbite_timer -= diff;
				
				if(slimepool_timer < diff)           //slimepool
				{	
					DoCast(m_creature->getVictim(),slimepool);
					slimepool_timer = 10000;
					m_creature->MonsterSay("POOOOLLLL OF SLIME!!!", LANG_UNIVERSAL, 0);
				}else slimepool_timer -= diff;
				
				if(acidicspew_timer < diff)           //acidicspew
				{	
					DoCast(m_creature->getVictim(),acidicspew);
					acidicspew_timer = 10000;
					m_creature->MonsterSay("Acid spewwwwww!!", LANG_UNIVERSAL, 0);
				}else acidicspew_timer -= diff;
			}
				
				//determine if phase has ended
			   //   DoMeleeAttackIfReady();
	}
};


struct MANGOS_DLL_DECL npc_acidmawAI : public ScriptedAI
{
npc_acidmawAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 EndPhaseTwo;
	uint32 acidicspew_timer; //phase 1  stationary first
	uint32 slimepool_timer;
	uint32 paralyticbite_timer;
	uint32 acidspit_timer;//phase 2
	uint32 sweep_timer;
	uint32 paralyticspray_timer;
		
	void Reset()
{

	acidicspew_timer = 5400;
	slimepool_timer = 10000;
	paralyticbite_timer =  4000;
	
	paralyticspray_timer = 12000;
	acidspit_timer = 16000;

}

void Aggro(Unit *who)
	{
		m_creature->MonsterSay("Me Hungry!", LANG_UNIVERSAL, 0);

	}

void UpdateAI(const uint32 diff )
	{
		ScriptedInstance* pInstance = ((ScriptedInstance*)m_creature->GetInstanceData());

		//Return since we have no target
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

			//BEGIN AI

				//BOSS 1

				
				

				//*****BOSS2***//
			if(pInstance && pInstance->GetData(100) == SWAP_Orginal)
			{
				if( paralyticbite_timer < diff)      //para bite
				{
					m_creature->MonsterSay("BITTTTEEEEE", LANG_UNIVERSAL, 0);
					DoCast(m_creature->getVictim(), paralyticbite);
					 paralyticbite_timer = 30000;
				}else  paralyticbite_timer -= diff;
				
				if(slimepool_timer < diff)           //slimepool
				{	
					DoCast(m_creature->getVictim(),slimepool);
					slimepool_timer = 10000;
					m_creature->MonsterSay("POOOOLLLL OF SLIME!!!", LANG_UNIVERSAL, 0);
				}else slimepool_timer -= diff;
				
				if(acidicspew_timer < diff)           //acidicspew
				{	
					DoCast(m_creature->getVictim(),acidicspew);
					acidicspew_timer = 10000;
					m_creature->MonsterSay("Acid spewwwwww!!", LANG_UNIVERSAL, 0);
				}else acidicspew_timer -= diff;
			}else{
				if(paralyticspray_timer < diff)      //para spray
				{
					m_creature->MonsterSay("SPRAYYYYYYY", LANG_UNIVERSAL, 0);
					DoCast(m_creature->getVictim(),paralyticspray);
					paralyticspray_timer = 12000;
				}else paralyticspray_timer -= diff;
				
				if(sweep_timer < diff)           //sweep
				{	
					DoCast(m_creature->getVictim(),sweep);
					sweep_timer =   7000;
					m_creature->MonsterSay("SWEEP!!", LANG_UNIVERSAL, 0);
				}else sweep_timer -= diff;
				
				if(acidspit_timer < diff)           //acid spit
				{	
					DoCast(m_creature->getVictim(),acidspit);
					acidspit_timer = 16000;
					m_creature->MonsterSay("Acid Spit!!", LANG_UNIVERSAL, 0);
				}else acidspit_timer -= diff;
			}
				//determine if phase has ended
			   //   DoMeleeAttackIfReady();
	}
};
//List of gossip item texts. Items will appear in the gossip window.

struct MANGOS_DLL_DECL custom_exampleAI : public ScriptedAI
{
    //*** HANDLED FUNCTION ***
    //This is the constructor, called only once when the creature is first created
    custom_exampleAI(Creature* pCreature) : ScriptedAI(pCreature) {
    //    m_uiNpcEntry = pCreature->GetEntry();
        Reset();
    }


	//defining my shit
	uint32 Phase;
	uint32 stomp_timer;
	uint32 impale_timer;
	uint32 whirl_timer;
	uint32 PhaseTimer;
	uint32 EndPhaseTwo;
	uint32 summon_timer;
	uint32 summon_count;


void Reset()
{
	Phase = 0;
	PhaseTimer = 60000; // 2 minute
	stomp_timer = 10000; //30secs for spell 1
	impale_timer = 5000; // 5 seconds 
	summon_timer = 20000; // 25 seconds
	whirl_timer = 5000;
	summon_count = 0;
}

void Aggro(Unit *who)
	{
 m_creature->MonsterSay("ROAR!", LANG_UNIVERSAL, 0);
	}

void UpdateAI(const uint32 diff )
	{
	
		//Return since we have no target
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

			//BEGIN AI
			
				if(stomp_timer < diff)
				{
					m_creature->MonsterSay("I STOMP ON YOUR FACE!", LANG_UNIVERSAL, 0);
					DoCast(m_creature->getVictim(),stomp);
					stomp_timer = 30000;
				}else stomp_timer -= diff;
				
				if(impale_timer < diff)
				{	
					DoCast(m_creature->getVictim(),impale);
					impale_timer = 10000;
					m_creature->MonsterSay("IMPALE!", LANG_UNIVERSAL, 0);
				}else impale_timer -= diff;
				

				if(summon_timer < diff && summon_count <= 4)
				{
					Unit *pTarget1 = SelectUnit(SELECT_TARGET_RANDOM,0);
					Creature *pLarva1 = pTarget1->SummonCreature(34800,pTarget1->GetPositionX() ,pTarget1->GetPositionY()+1, pTarget1->GetPositionZ()+1, 0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
					DoCast(m_creature,RisingAnger);	
					//CastSpell(m_creature,RisingAnger,false);
					pLarva1->AI()->AttackStart(pTarget1); 
					pLarva1->MonsterSay("Boss Enjoy the buff :D", LANG_UNIVERSAL, 0);
					summon_count = (summon_count + 1);

				summon_timer = 60000;
					m_creature->MonsterSay("Enjoy my little friends", LANG_UNIVERSAL, 0);
				}else summon_timer -= diff;

		
			      DoMeleeAttackIfReady();
	}

	    void JustDied(Unit *killer)
    {
	ScriptedInstance* pInstance = ((ScriptedInstance*)m_creature->GetInstanceData());
			pInstance->SetData(DOOR1, NOT_STARTED);
            pInstance->SetData(BOSS1, DONE);
			pInstance->SetData(BOSS2, IN_PROGRESS);
    } 
};



//DUMMY

bool GossipHello_Barrett_Ramsey(Player *player, Creature *_Creature)
{
ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());
if( pInstance && pInstance->GetData(DOOR1) == NOT_STARTED)
{
	 if (pInstance && pInstance->GetData(BOSS1) != DONE)
	 {
		 player->ADD_GOSSIP_ITEM( 5, "BEGIN THE EVENT" , GOSSIP_SENDER_MAIN, 1);
		 player->ADD_GOSSIP_ITEM( 5, "KILL ME!" , GOSSIP_SENDER_MAIN, 333);
		  player->PlayerTalkClass->SendGossipMenu(907, _Creature->GetGUID());
	 } else {
			 if (pInstance && pInstance->GetData(BOSS2) != DONE)
			{	
				 player->ADD_GOSSIP_ITEM( 5, "START 2" , GOSSIP_SENDER_MAIN, 2);
				 		 player->ADD_GOSSIP_ITEM( 5, "KILL ME!" , GOSSIP_SENDER_MAIN, 333);
				 player->PlayerTalkClass->SendGossipMenu(907, _Creature->GetGUID());
			} else {
				 if (pInstance && pInstance->GetData(BOSS3) != DONE)
				{
					 player->ADD_GOSSIP_ITEM( 5, "START 3" , GOSSIP_SENDER_MAIN, 3);
					 		 player->ADD_GOSSIP_ITEM( 5, "KILL ME!" , GOSSIP_SENDER_MAIN, 333);
					  player->PlayerTalkClass->SendGossipMenu(907, _Creature->GetGUID());
				 } else {
					player->ADD_GOSSIP_ITEM( 5, "START 4" , GOSSIP_SENDER_MAIN, 4);
							 player->ADD_GOSSIP_ITEM( 5, "KILL ME!" , GOSSIP_SENDER_MAIN, 333);
					 player->PlayerTalkClass->SendGossipMenu(907, _Creature->GetGUID());
				}
			}
 }
}
}

bool GossipSelect_Barrett_Ramsey(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{

		ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());
     pInstance->SetData(action, IN_PROGRESS);
	player->CLOSE_GOSSIP_MENU();
	
	

}


//This is the GetAI method used by all scripts that involve AI
//It is called every time a new creature using this script is created


struct MANGOS_DLL_DECL instance_trial_of_the_crusader : public ScriptedInstance
{
    instance_trial_of_the_crusader(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
	uint32 DOOR;
    std::string strInstData;

    uint64 m_uiTirionFordringGUID;
    uint64 m_uiGormokGUID;
	uint64	m_uiDreadscaleGUID;
	uint64	m_uiAcidmawGUID;
	uint64	m_uiIcehowlGUID;

    uint64 m_uiDoorMainGUID;
	uint32 CloseGate_Timer;
	uint32 Wait_Timer ;
	uint32 Intro;
	uint32 OpenGate_Timer;
	uint32 emotetimer;
	uint32 IntroPhase;
	uint32 Swap_Timer;
	bool Swap;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiTirionFordringGUID = 0;
        m_uiGormokGUID = 0;
		m_uiDreadscaleGUID = 0;
		m_uiAcidmawGUID = 0;
		m_uiIcehowlGUID = 0;

        m_uiDoorMainGUID = 0;
		Wait_Timer = 4000;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case TirionFordring: m_uiTirionFordringGUID = pCreature->GetGUID(); break;
            case Gormok: m_uiGormokGUID = pCreature->GetGUID(); break;
			case Dreadscale: m_uiDreadscaleGUID = pCreature->GetGUID(); break;
			case Acidmaw: m_uiAcidmawGUID = pCreature->GetGUID(); break;
			case Icehowl: m_uiIcehowlGUID = pCreature->GetGUID(); break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_MAIN_DOOR:
                m_uiDoorMainGUID = pGo->GetGUID();
                if (DOOR == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
        }
    }
	///////// CONTROLLER
	//Timers


void Update(const uint32 diff)
	{
	//0 = open gate
	//1 = move boss
	//2 = nothing

		Creature* pGormok = instance->GetCreature(m_uiGormokGUID);
		Creature* pDreadscale = instance->GetCreature(m_uiDreadscaleGUID);
		Creature* pAcidmaw = instance->GetCreature(m_uiAcidmawGUID);
		Creature* pIcehowl = instance->GetCreature(m_uiIcehowlGUID);
		//MOVE INTRO
	if(Intro == 1)
	{
		if(Wait_Timer < diff)
		{
			switch(IntroPhase)
			{
				case 1:
					pGormok->GetMotionMaster()->MovePoint(0, 563.589294, 139.645645, 393.836578);
					pGormok->MonsterYell("HELLO!!!!!!!!!", LANG_UNIVERSAL, 0);
				break;
				case 2:
					pDreadscale->GetMotionMaster()->MovePoint(0, 563.589294, 139.645645, 393.836578);
					pDreadscale->MonsterYell("HGfdgdhr", LANG_UNIVERSAL, 0);
					emotetimer = 4000;
		//			EmoteIsntDone = false;
				break;
				case 3: 
					pIcehowl->GetMotionMaster()->MovePoint(0, 563.589294, 139.645645, 393.836578);
				break;
				
			
			}
				CloseGate_Timer = 4000;
				Intro = 2;			
		} else Wait_Timer -= diff;
	}

	//**********OPEN GATE**********///	
			if(OpenGate_Timer < diff && Intro == 0)
			{
				Intro = 1;
				SetData(DOOR1, DONE);
				Wait_Timer = 3000;
			}else OpenGate_Timer -= diff;
	//*********CLOSE GATE*************///
			if(CloseGate_Timer < diff && GetData(DOOR1) == DONE)
			{
				SetData(DOOR1, IN_PROGRESS); //closegate
				DoUseDoorOrButton(m_uiDoorMainGUID);
			}else CloseGate_Timer -= diff;
	

	//////////SPECIAL///////////////
/*	if(EmoteIsntDone == false)
	{
		if(emotetimer < diff)
		{
		Creature *pAcidmaw = pDreadscale->SummonCreature(Acidmaw,538.347, 165.509, 394.6500, 5.431659,TEMPSUMMON_MANUAL_DESPAWN, 0);
		pAcidmaw->HandleEmoteCommand(Emerge);
			EmoteIsntDone = true;
			
		}else emotetimer -= diff; 
	}*/
		if(GetData(BOSS2) == IN_PROGRESS)
		{
			if(Swap_Timer < diff)
			{
				if(Swap == false){
				SetData(100, SWAP_Orginal); 
				Swap_Timer = 55000;
				pDreadscale->MonsterYell("Me Swap", LANG_UNIVERSAL, 0);
				} else SetData(100, SWAP_SWAP);;
			}else Swap_Timer -= diff;
			
			if (pDreadscale && pDreadscale->isDead() && pAcidmaw && pAcidmaw->isDead())
			{
				SetData(BOSS2, DONE);
			}
		}
	}
	
	
/*
    void DoSpeech()
    {
        Creature* pAda = instance->GetCreature(m_uiAdaGUID);
        Creature* pAsh = instance->GetCreature(m_uiAshGUID);

        if (pAda && pAda->isAlive() && pAsh && pAsh->isAlive())
        {
            DoScriptText(SAY_BOSS_DIE_AD,pAda);
            DoScriptText(SAY_BOSS_DIE_AS,pAsh);
        }
    } */

    void SetData(uint32 uiType, uint32 uiData)
    {
	Creature* pTirionFordring = instance->GetCreature(m_uiTirionFordringGUID);
        switch(uiType)
        {
			case DOOR1:
                if (uiData == DONE)
                    DoUseDoorOrButton(m_uiDoorMainGUID);
                m_auiEncounter[0] = uiData;
            break;
            case 1:
                if (uiData == IN_PROGRESS)
				{
					IntroPhase = 1;
					OpenGate_Timer = 8000;
					Intro = 0;
					DoScriptText(INTRO1, pTirionFordring);
					Creature *pGormok = pTirionFordring->SummonCreature(Gormok,563.539551, 213.565628, 395.098206, 4.66094,TEMPSUMMON_MANUAL_DESPAWN, 0);
					pGormok->RemoveMonsterMoveFlag(MONSTER_MOVE_WALK);
				}
                m_auiEncounter[1] = uiData;
                break;
			case 2:
                if (uiData == IN_PROGRESS)
				{
				IntroPhase = 2;
				Intro = 0;
				DoScriptText(INTRO2, pTirionFordring);
				OpenGate_Timer = 3500;
				Creature *pDreadscale = pTirionFordring->SummonCreature(Dreadscale,563.539551, 213.565628, 395.098206, 4.66094,TEMPSUMMON_MANUAL_DESPAWN, 0);
				pDreadscale->RemoveMonsterMoveFlag(MONSTER_MOVE_WALK);
				}
				if (uiData == DONE)
				SetData(BOSS3, IN_PROGRESS);
                m_auiEncounter[2] = uiData;
            break;
			case 3:
                if (uiData == IN_PROGRESS)
				{
				IntroPhase = 3;
				Intro = 0;
				DoScriptText(INTRO3, pTirionFordring);
				OpenGate_Timer = 3500;
				Swap_Timer = 55000;
				Creature *pIcehowl = pTirionFordring->SummonCreature(Icehowl,563.539551, 213.565628, 395.098206, 4.66094,TEMPSUMMON_MANUAL_DESPAWN,0);
				pIcehowl->RemoveMonsterMoveFlag(MONSTER_MOVE_WALK);
				}
                m_auiEncounter[3] = uiData;
            break;
			case 100:
				if(uiData == SWAP_Orginal)
				Swap = false;
				if(uiData == SWAP_SWAP)
				Swap = true;
			break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
			case DOOR1:
                return m_auiEncounter[0];
            case BOSS1:
                return m_auiEncounter[1];
			case BOSS2:
                return m_auiEncounter[2];
			case BOSS3:
                return m_auiEncounter[3];

        }
        return 0;
    }
	       uint64 GetData64(uint32 uiData)
       {
           switch(uiData)
           {
               case Dreadscale:       return m_uiDreadscaleGUID;
			   case Acidmaw:       return m_uiAcidmawGUID;
			  
           }
   
           return 0;
       }

 /*   uint64 GetData64(uint32 id)
    {
        switch(id)
        {
            case Gormok:      return m_uiGormokGUID;

        }
        return 0;
    }*/
	
	
	
    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }
			m_auiEncounter[0] = NOT_STARTED;
        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_trial_of_the_crusader(Map* pMap)
{
    return new instance_trial_of_the_crusader(pMap);
}
CreatureAI* GetAI_custom_example(Creature* pCreature)
{
    return new custom_exampleAI(pCreature);
}

CreatureAI* GetAI_npc_dreadscale(Creature* pCreature)
{
    return new npc_dreadscaleAI(pCreature);
}

CreatureAI* GetAI_npc_acidmaw(Creature* pCreature)
{
    return new npc_acidmawAI(pCreature);
}


void AddSC_custom_example()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "custom_example";
    newscript->GetAI = &GetAI_custom_example;
    newscript->RegisterSelf();

	
	newscript = new Script;
newscript->Name="Barrett_Ramsey";
newscript->pGossipHello = &GossipHello_Barrett_Ramsey;
newscript->pGossipSelect = &GossipSelect_Barrett_Ramsey;
    newscript->RegisterSelf();
	

    newscript = new Script;
    newscript->Name = "instance_trial_of_the_crusader";
    newscript->GetInstanceData = &GetInstanceData_instance_trial_of_the_crusader;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dreadscale";
    newscript->GetAI = &GetAI_npc_dreadscale;
    newscript->RegisterSelf();
	
    newscript = new Script;
    newscript->Name = "npc_acidmaw";
    newscript->GetAI = &GetAI_npc_acidmaw;
    newscript->RegisterSelf();
	}

