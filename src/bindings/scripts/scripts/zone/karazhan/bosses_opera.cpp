/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Bosses_Opera
SD%Complete: 90
SDComment: Oz, Hood, and RAJ event implemented. RAJ event requires more testing.
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "def_karazhan.h"

/***********************************/
/*** OPERA WIZARD OF OZ EVENT *****/
/*********************************/

#define SAY_DOROTHEE_DEATH          -1532025
#define SAY_DOROTHEE_SUMMON         -1532026
#define SAY_DOROTHEE_TITO_DEATH     -1532027
#define SAY_DOROTHEE_AGGRO          -1532028

#define SAY_ROAR_AGGRO              -1532029
#define SAY_ROAR_DEATH              -1532030
#define SAY_ROAR_SLAY               -1532031

#define SAY_STRAWMAN_AGGRO          -1532032
#define SAY_STRAWMAN_DEATH          -1532033
#define SAY_STRAWMAN_SLAY           -1532034

#define SAY_TINHEAD_AGGRO           -1532035
#define SAY_TINHEAD_DEATH           -1532036
#define SAY_TINHEAD_SLAY            -1532037
#define EMOTE_RUST                  -1532038

#define SAY_CRONE_AGGRO             -1532039
#define SAY_CRONE_AGGRO2            -1532040
#define SAY_CRONE_DEATH             -1532041
#define SAY_CRONE_SLAY              -1532042

/**** Spells ****/
// Dorothee
#define SPELL_WATERBOLT         31012
#define SPELL_SCREAM            31013
#define SPELL_SUMMONTITO        31014

// Tito
#define SPELL_YIPPING           31015

// Strawman
#define SPELL_BRAIN_BASH        31046
#define SPELL_BRAIN_WIPE        31069
#define SPELL_BURNING_STRAW     31075

// Tinhead
#define SPELL_CLEAVE            31043
#define SPELL_RUST              31086

// Roar
#define SPELL_MANGLE            31041
#define SPELL_SHRED             31042
#define SPELL_FRIGHTENED_SCREAM 31013

// Crone
#define SPELL_CHAIN_LIGHTNING   32337

// Cyclone
#define SPELL_KNOCKBACK         32334
#define SPELL_CYCLONE_VISUAL    32332

/** Creature Entries **/
#define CREATURE_TITO           17548
#define CREATURE_CYCLONE        18412
#define CREATURE_CRONE          18168

void SummonCroneIfReady(ScriptedInstance* pInstance, Creature *_Creature)
{
    pInstance->SetData(DATA_OPERA_OZ_DEATHCOUNT, 0);        // Increment DeathCount
    if (pInstance->GetData(DATA_OPERA_OZ_DEATHCOUNT) == 4)
    {
        Creature* Crone = _Creature->SummonCreature(CREATURE_CRONE,  -10891.96, -1755.95, _Creature->GetPositionZ(), 4.64, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000);
        if (Crone)
        {
            if (_Creature->getVictim())
                Crone->AI()->AttackStart(_Creature->getVictim());
        }
    }
};

struct OREGON_DLL_DECL boss_dorotheeAI : public ScriptedAI
{
    boss_dorotheeAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 AggroTimer;

    uint32 WaterBoltTimer;
    uint32 FearTimer;
    uint32 SummonTitoTimer;

    bool SummonedTito;
    bool TitoDied;
    bool InCombat;

    void Reset()
    {
        AggroTimer = 500;

        WaterBoltTimer = 5000;
        FearTimer = 15000;
        SummonTitoTimer = 47500;

        SummonedTito = false;
        TitoDied = false;
        InCombat = false;
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_DOROTHEE_AGGRO, me);
    }

    void SummonTito();                                      // See below

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DOROTHEE_DEATH, me);

        if (pInstance)
            SummonCroneIfReady(pInstance, me);
    }

    void AttackStart(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (AggroTimer)
        {
            if (AggroTimer <= diff)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AggroTimer = 0;
            } else AggroTimer -= diff;
        }

        if (!UpdateVictim())
            return;

        if (WaterBoltTimer <= diff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_WATERBOLT);
            WaterBoltTimer = TitoDied ? 1500 : 5000;
        } else WaterBoltTimer -= diff;

        if (FearTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_SCREAM);
            FearTimer = 30000;
        } else FearTimer -= diff;

        if (!SummonedTito)
        {
            if (SummonTitoTimer <= diff)
                SummonTito();
            else SummonTitoTimer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

struct OREGON_DLL_DECL mob_titoAI : public ScriptedAI
{
    mob_titoAI(Creature* c) : ScriptedAI(c) {}

    uint64 DorotheeGUID;

    uint32 YipTimer;

    void Reset()
    {
        DorotheeGUID = 0;

        YipTimer = 10000;
    }

    void EnterCombat(Unit* who) {}

    void JustDied(Unit* killer)
    {
        if (DorotheeGUID)
        {
            Creature* Dorothee = (Unit::GetCreature((*me), DorotheeGUID));
            if (Dorothee && Dorothee->isAlive())
            {
                ((boss_dorotheeAI*)Dorothee->AI())->TitoDied = true;
                DoScriptText(SAY_DOROTHEE_TITO_DEATH, Dorothee);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (YipTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_YIPPING);
            YipTimer = 10000;
        } else YipTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

void boss_dorotheeAI::SummonTito()
{
    Creature* Tito = DoSpawnCreature(CREATURE_TITO, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000);
    if (Tito)
    {
        DoScriptText(SAY_DOROTHEE_SUMMON, me);
        ((mob_titoAI*)Tito->AI())->DorotheeGUID = me->GetGUID();
        Tito->AI()->AttackStart(me->getVictim());
        SummonedTito = true;
        TitoDied = false;
    }
}

struct OREGON_DLL_DECL boss_strawmanAI : public ScriptedAI
{
    boss_strawmanAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 AggroTimer;
    uint32 BrainBashTimer;
    uint32 BrainWipeTimer;

    void Reset()
    {
        AggroTimer = 13000;
        BrainBashTimer = 5000;
        BrainWipeTimer = 7000;
    }

    void AttackStart(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_STRAWMAN_AGGRO, me);
    }

    void SpellHit(Unit* caster, const SpellEntry *Spell)
    {
        if ((Spell->SchoolMask == SPELL_SCHOOL_MASK_FIRE) && (!(rand()%10)))
            DoCast(me, SPELL_BURNING_STRAW, true);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_STRAWMAN_DEATH, me);

        if (pInstance)
            SummonCroneIfReady(pInstance, me);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_STRAWMAN_SLAY, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (AggroTimer)
        {
            if (AggroTimer <= diff)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AggroTimer = 0;
            } else AggroTimer -= diff;
        }

        if (!UpdateVictim())
            return;

        if (BrainBashTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_BRAIN_BASH);
            BrainBashTimer = 15000;
        } else BrainBashTimer -= diff;

        if (BrainWipeTimer <= diff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_BRAIN_WIPE);
            BrainWipeTimer = 20000;
        } else BrainWipeTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct OREGON_DLL_DECL boss_tinheadAI : public ScriptedAI
{
    boss_tinheadAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 AggroTimer;
    uint32 CleaveTimer;
    uint32 RustTimer;

    uint8 RustCount;

    void Reset()
    {
        AggroTimer = 15000;
        CleaveTimer = 5000;
        RustTimer   = 30000;

        RustCount   = 0;
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_TINHEAD_AGGRO, me);
    }

    void AttackStart(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_TINHEAD_DEATH, me);

        if (pInstance)
            SummonCroneIfReady(pInstance, me);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_TINHEAD_SLAY, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (AggroTimer)
        {
            if (AggroTimer <= diff)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AggroTimer = 0;
            } else AggroTimer -= diff;
        }

        if (!UpdateVictim())
            return;

        if (CleaveTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_CLEAVE);
            CleaveTimer = 5000;
        } else CleaveTimer -= diff;

        if (RustCount < 8)
        {
            if (RustTimer <= diff)
            {
                RustCount++;
                 DoScriptText(EMOTE_RUST, me);
                DoCast(me, SPELL_RUST);
                RustTimer = 6000;
            } else RustTimer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

struct OREGON_DLL_DECL boss_roarAI : public ScriptedAI
{
    boss_roarAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 AggroTimer;
    uint32 MangleTimer;
    uint32 ShredTimer;
    uint32 ScreamTimer;

    void Reset()
    {
        AggroTimer = 20000;
        MangleTimer = 5000;
        ShredTimer  = 10000;
        ScreamTimer = 15000;
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void AttackStart(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_ROAR_AGGRO, me);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_ROAR_DEATH, me);

        if (pInstance)
            SummonCroneIfReady(pInstance, me);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_ROAR_SLAY, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (AggroTimer)
        {
            if (AggroTimer <= diff)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AggroTimer = 0;
            } else AggroTimer -= diff;
        }

        if (!UpdateVictim())
            return;

        if (MangleTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_MANGLE);
            MangleTimer = 5000 + rand()%3000;
        } else MangleTimer -= diff;

        if (ShredTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_SHRED);
            ShredTimer = 10000 + rand()%5000;
        } else ShredTimer -= diff;

        if (ScreamTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_FRIGHTENED_SCREAM);
            ScreamTimer = 20000 + rand()%10000;
        } else ScreamTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct OREGON_DLL_DECL boss_croneAI : public ScriptedAI
{
    boss_croneAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 CycloneTimer;
    uint32 ChainLightningTimer;

    void Reset()
    {
        CycloneTimer = 30000;
        ChainLightningTimer = 10000;
    }

    void EnterCombat(Unit* who)
    {
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_CRONE_AGGRO, me); break;
        case 1: DoScriptText(SAY_CRONE_AGGRO2, me); break;
        }
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_CRONE_DEATH, me);

        if (pInstance)
        {
            pInstance->SetData(DATA_OPERA_EVENT, DONE);
            GameObject* Door = GameObject::GetGameObject((*me), pInstance->GetData64(DATA_GAMEOBJECT_STAGEDOORRIGHT));
            if (Door)
                Door->UseDoorOrButton();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (CycloneTimer <= diff)
        {
            Creature* Cyclone = DoSpawnCreature(CREATURE_CYCLONE, rand()%10, rand()%10, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
            if (Cyclone)
                Cyclone->CastSpell(Cyclone, SPELL_CYCLONE_VISUAL, true);
            CycloneTimer = 30000;
        } else CycloneTimer -= diff;

        if (ChainLightningTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_CHAIN_LIGHTNING);
            ChainLightningTimer = 15000;
        } else ChainLightningTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct OREGON_DLL_DECL mob_cycloneAI : public ScriptedAI
{
    mob_cycloneAI(Creature* c) : ScriptedAI(c) {}

    uint32 MoveTimer;

    void Reset()
    {
        MoveTimer = 1000;
    }

    void EnterCombat(Unit* who) {}

    void MoveInLineOfSight(Unit* who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->HasAura(SPELL_KNOCKBACK, 0))
            DoCast(me, SPELL_KNOCKBACK, true);

        if (MoveTimer <= diff)
        {
            float x,y,z;
            me->GetPosition(x,y,z);
            float PosX, PosY, PosZ;
            me->GetRandomPoint(x,y,z,10, PosX, PosY, PosZ);
            me->GetMotionMaster()->MovePoint(0, PosX, PosY, PosZ);
            MoveTimer = 5000 + rand()%3000;
        } else MoveTimer -= diff;
    }
};

CreatureAI* GetAI_boss_dorothee(Creature* _Creature)
{
    return new boss_dorotheeAI(_Creature);
}

CreatureAI* GetAI_boss_strawman(Creature* _Creature)
{
    return new boss_strawmanAI(_Creature);
}

CreatureAI* GetAI_boss_tinhead(Creature* _Creature)
{
    return new boss_tinheadAI(_Creature);
}

CreatureAI* GetAI_boss_roar(Creature* _Creature)
{
    return new boss_roarAI(_Creature);
}

CreatureAI* GetAI_boss_crone(Creature* _Creature)
{
    return new boss_croneAI(_Creature);
}

CreatureAI* GetAI_mob_tito(Creature* _Creature)
{
    return new mob_titoAI(_Creature);
}

CreatureAI* GetAI_mob_cyclone(Creature* _Creature)
{
    return new mob_cycloneAI(_Creature);
}

/**************************************/
/**** Opera Red Riding Hood Event ****/
/************************************/

/**** Yells for the Wolf ****/
#define SAY_WOLF_AGGRO                  -1532043
#define SAY_WOLF_SLAY                   -1532044
#define SAY_WOLF_HOOD                   -1532045
#define SOUND_WOLF_DEATH                9275                //Only sound on death, no text.

/**** Spells For The Wolf ****/
#define SPELL_LITTLE_RED_RIDING_HOOD    30768
#define SPELL_TERRIFYING_HOWL           30752
#define SPELL_WIDE_SWIPE                30761

#define GOSSIP_GRANDMA          "What phat lewtz you have grandmother?"

/**** The Wolf's Entry ****/
#define CREATURE_BIG_BAD_WOLF           17521

bool GossipHello_npc_grandmother(Player* player, Creature* _Creature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_GRANDMA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    player->SEND_GOSSIP_MENU(8990, _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_grandmother(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF)
    {
        _Creature->SetVisibility(VISIBILITY_OFF);
        float x,y,z;
        _Creature->GetPosition(x,y,z);
        Creature* BigBadWolf = _Creature->SummonCreature(CREATURE_BIG_BAD_WOLF, x, y, z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000);
        if (BigBadWolf)
        {
            BigBadWolf->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            BigBadWolf->AI()->AttackStart(player);
        }

        _Creature->setDeathState(JUST_DIED);
    }

    return true;
}

struct OREGON_DLL_DECL boss_bigbadwolfAI : public ScriptedAI
{
    boss_bigbadwolfAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 ChaseTimer;
    uint32 FearTimer;
    uint32 SwipeTimer;

    uint64 HoodGUID;
    float TempThreat;

    bool IsChasing;

    void Reset()
    {
        ChaseTimer = 30000;
        FearTimer = 25000 + rand()%10000;
        SwipeTimer = 5000;

        HoodGUID = 0;
        TempThreat = 0;

        IsChasing = false;
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_WOLF_AGGRO, me);
    }

    void JustDied(Unit* killer)
    {
        DoPlaySoundToSet(me, SOUND_WOLF_DEATH);

        if (pInstance)
        {
            pInstance->SetData(DATA_OPERA_EVENT, DONE);
            GameObject* Door = GameObject::GetGameObject((*me), pInstance->GetData64(DATA_GAMEOBJECT_STAGEDOORRIGHT));
            if (Door)
                Door->UseDoorOrButton();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();

        if (ChaseTimer <= diff)
        {
            if (!IsChasing)
            {
                Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
                {
                    DoScriptText(SAY_WOLF_HOOD, me);

                    DoCast(pTarget, SPELL_LITTLE_RED_RIDING_HOOD, true);
                    TempThreat = DoGetThreat(pTarget);
                    if (TempThreat)
                        DoModifyThreatPercent(pTarget, -100);
                    HoodGUID = pTarget->GetGUID();
                    me->AddThreat(pTarget, 1000000.0f);
                    ChaseTimer = 20000;
                    IsChasing = true;
                }
            }
            else
            {
                IsChasing = false;
                Unit *pTarget = Unit::GetUnit((*me), HoodGUID);
                if (pTarget)
                {
                    HoodGUID = 0;
                    if (DoGetThreat(pTarget))
                        DoModifyThreatPercent(pTarget, -100);
                    me->AddThreat(pTarget, TempThreat);
                    TempThreat = 0;
                }

                ChaseTimer = 40000;
            }
        } else ChaseTimer -= diff;

        if (IsChasing)
            return;

        if (FearTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_TERRIFYING_HOWL);
            FearTimer = 25000 + rand()%10000;
        } else FearTimer -= diff;

        if (SwipeTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_WIDE_SWIPE);
            SwipeTimer = 25000 + rand()%5000;
        } else SwipeTimer -= diff;

    }
};

CreatureAI* GetAI_boss_bigbadwolf(Creature* _Creature)
{
    return new boss_bigbadwolfAI(_Creature);
}

/**********************************************/
/******** Opera Romeo and Juliet Event *******/
/********************************************/

/**** Speech *****/
#define SAY_JULIANNE_AGGRO              -1532046
#define SAY_JULIANNE_ENTER              -1532047
#define SAY_JULIANNE_DEATH01            -1532048
#define SAY_JULIANNE_DEATH02            -1532049
#define SAY_JULIANNE_RESURRECT          -1532050
#define SAY_JULIANNE_SLAY               -1532051

#define SAY_ROMULO_AGGRO                -1532052
#define SAY_ROMULO_DEATH                -1532053
#define SAY_ROMULO_ENTER                -1532054
#define SAY_ROMULO_RESURRECT            -1532055
#define SAY_ROMULO_SLAY                 -1532056

/***** Spells For Julianne *****/
#define SPELL_BLINDING_PASSION          30890
#define SPELL_DEVOTION                  30887
#define SPELL_ETERNAL_AFFECTION         30878
#define SPELL_POWERFUL_ATTRACTION       30889
#define SPELL_DRINK_POISON              30907

/***** Spells For Romulo ****/
#define SPELL_BACKWARD_LUNGE            30815
#define SPELL_DARING                    30841
#define SPELL_DEADLY_SWATHE             30817
#define SPELL_POISON_THRUST             30822

/**** Other Misc. Spells ****/
#define SPELL_UNDYING_LOVE              30951
#define SPELL_RES_VISUAL                24171

/*** Misc. Information ****/
#define CREATURE_ROMULO             17533
#define ROMULO_X                    -10900
#define ROMULO_Y                    -1758

enum RAJPhase
{
    PHASE_JULIANNE      = 0,
    PHASE_ROMULO        = 1,
    PHASE_BOTH          = 2,
};

void PretendToDie(Creature* _Creature)
{
    _Creature->InterruptNonMeleeSpells(false);
    _Creature->SetHealth(0);
    _Creature->ClearComboPointHolders();
    _Creature->RemoveAllAurasOnDeath();
    _Creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
    _Creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
    _Creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    _Creature->ClearAllReactives();
    _Creature->SetUInt64Value(UNIT_FIELD_TARGET,0);
    _Creature->GetMotionMaster()->Clear();
    _Creature->GetMotionMaster()->MoveIdle();
    _Creature->SetUInt32Value(UNIT_FIELD_BYTES_1,PLAYER_STATE_DEAD);
};

void Resurrect(Creature* pTarget)
{
    pTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    pTarget->SetHealth(pTarget->GetMaxHealth());
    pTarget->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
    pTarget->CastSpell(pTarget, SPELL_RES_VISUAL, true);
    if (pTarget->getVictim())
    {
        pTarget->SetUInt64Value(UNIT_FIELD_TARGET, pTarget->getVictim()->GetGUID());
        pTarget->GetMotionMaster()->MoveChase(pTarget->getVictim());
        pTarget->AI()->AttackStart(pTarget->getVictim());
    }
};

struct OREGON_DLL_DECL boss_julianneAI : public ScriptedAI
{
    boss_julianneAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        EntryYellTimer = 1000;
        AggroYellTimer = 10000;
    }

    ScriptedInstance* pInstance;

    uint64 RomuloGUID;

    uint32 Phase;

    uint32 EntryYellTimer;
    uint32 AggroYellTimer;
    uint32 BlindingPassionTimer;
    uint32 DevotionTimer;
    uint32 EternalAffectionTimer;
    uint32 PowerfulAttractionTimer;
    uint32 SummonRomuloTimer;
    uint32 ResurrectTimer;

    bool IsFakingDeath;
    bool SummonedRomulo;
    bool RomuloDead;

    void Reset()
    {
        if (RomuloGUID)
        {
            if (Unit* Romulo = Unit::GetUnit(*me, RomuloGUID))
            {
                Romulo->SetVisibility(VISIBILITY_OFF);
                Romulo->DealDamage(Romulo, Romulo->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }

            RomuloGUID = 0;
        }

        Phase = PHASE_JULIANNE;

        BlindingPassionTimer = 30000;
        DevotionTimer = 15000;
        EternalAffectionTimer = 25000;
        PowerfulAttractionTimer = 5000;

        if (IsFakingDeath)
            Resurrect(me);

        IsFakingDeath = false;
        SummonedRomulo = false;
        RomuloDead = false;
    }

    void EnterCombat(Unit* who) {}

    void AttackStart(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void DamageTaken(Unit* done_by, uint32 &damage);

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_JULIANNE_DEATH02, me);

        if (pInstance)
        {
            pInstance->SetData(DATA_OPERA_EVENT, DONE);
            GameObject* Door = GameObject::GetGameObject((*me), pInstance->GetData64(DATA_GAMEOBJECT_STAGEDOORRIGHT));
            if (Door)
                Door->UseDoorOrButton();
        }
    }

    void KilledUnit(Unit* victim)
    {
       DoScriptText(SAY_JULIANNE_SLAY, me);
    }

    void UpdateAI(const uint32 diff);
};

struct OREGON_DLL_DECL boss_romuloAI : public ScriptedAI
{
    boss_romuloAI(Creature* c) : ScriptedAI(c)
    {
        EntryYellTimer = 8000;
        AggroYellTimer = 15000;
    }

    uint64 JulianneGUID;

    uint32 Phase;

    uint32 EntryYellTimer;
    uint32 AggroYellTimer;
    uint32 BackwardLungeTimer;
    uint32 DaringTimer;
    uint32 DeadlySwatheTimer;
    uint32 PoisonThrustTimer;
    uint32 ResurrectTimer;

    bool JulianneDead;
    bool IsFakingDeath;

    void Reset()
    {
        JulianneGUID = 0;

        Phase = PHASE_ROMULO;

        BackwardLungeTimer = 15000;
        DaringTimer = 20000;
        DeadlySwatheTimer = 25000;
        PoisonThrustTimer = 10000;

        if (IsFakingDeath)
            Resurrect(me);

        IsFakingDeath = false;
        JulianneDead = false;
    }

    void DamageTaken(Unit* done_by, uint32 &damage);

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_ROMULO_AGGRO, me);
        if (JulianneGUID)
        {
            Creature* Julianne = (Unit::GetCreature((*me), JulianneGUID));
            if (Julianne && Julianne->getVictim())
            {
                me->AddThreat(Julianne->getVictim(), 1.0f);
                AttackStart(Julianne->getVictim());
            }
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_ROMULO_DEATH, me);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_ROMULO_SLAY, me);
    }

    void UpdateAI(const uint32 diff);
};

void boss_julianneAI::DamageTaken(Unit* done_by, uint32 &damage)
{
    if (damage < me->GetHealth() || done_by == me || done_by->GetGUID() == RomuloGUID)
        return;

    if (Phase == PHASE_JULIANNE)
    {
        DoScriptText(SAY_JULIANNE_DEATH01, me);
        me->InterruptNonMeleeSpells(true);
        DoCast(me, SPELL_DRINK_POISON);
        PretendToDie(me);
        Phase = PHASE_ROMULO;
        damage = 0;
        IsFakingDeath = true;
        SummonRomuloTimer = 10000;
        return;
    }

    if (!IsFakingDeath)
    {
        Creature* Romulo = (Unit::GetCreature((*me), RomuloGUID));
        if (Romulo && Romulo->isAlive() && !((boss_romuloAI*)Romulo->AI())->IsFakingDeath)
        {
            ((boss_romuloAI*)Romulo->AI())->ResurrectTimer = 10000;
            ((boss_romuloAI*)Romulo->AI())->JulianneDead = true;
        }
        else
        {
            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            if (Romulo)
            {
                Romulo->DealDamage(Romulo, Romulo->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                Romulo->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            JustDied(done_by);
        }

        IsFakingDeath = true;
        PretendToDie(me);
        damage = 0;
    }
    else
        damage = 0;
}

void boss_romuloAI::DamageTaken(Unit* done_by, uint32 &damage)
{
    if (damage < me->GetHealth() || done_by == me || done_by->GetGUID() == JulianneGUID)
        return;

    if (!IsFakingDeath)
    {
        IsFakingDeath = true;
        PretendToDie(me);

        if (Phase == PHASE_BOTH)
        {
            Creature* Julianne = (Unit::GetCreature((*me), JulianneGUID));
            if (Julianne && Julianne->isAlive() && !((boss_julianneAI*)Julianne->AI())->IsFakingDeath)
            {
                ((boss_julianneAI*)Julianne->AI())->ResurrectTimer = 10000;
                ((boss_julianneAI*)Julianne->AI())->RomuloDead = true;
            }
            else
            {
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                if (Julianne)
                {
                    Julianne->DealDamage(Julianne, Julianne->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    Julianne->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                JustDied(done_by);
            }
        }
        else
        {
            Creature* Julianne = (Unit::GetCreature((*me), JulianneGUID));
            if (Julianne)
            {
                Resurrect(Julianne);
                me->SetHealth(me->GetMaxHealth());
                ((boss_julianneAI*)Julianne->AI())->ResurrectTimer = 4000;
                ((boss_julianneAI*)Julianne->AI())->RomuloDead = true;
                ((boss_julianneAI*)Julianne->AI())->Phase = PHASE_BOTH;
                ((boss_julianneAI*)Julianne->AI())->IsFakingDeath = false;
            }
            Phase = PHASE_BOTH;
        }

        damage = 0;
    }

    if (IsFakingDeath) damage = 0;
}

void boss_julianneAI::UpdateAI(const uint32 diff)
{
    if (EntryYellTimer)
    {
        if (EntryYellTimer <= diff)
        {
            DoScriptText(SAY_JULIANNE_ENTER, me);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            EntryYellTimer = 0;
        } else EntryYellTimer -= diff;
    }

    if (AggroYellTimer)
    {
        if (AggroYellTimer <= diff)
        {
            DoScriptText(SAY_JULIANNE_AGGRO, me);
            AggroYellTimer = 0;
        } else AggroYellTimer -= diff;
    }

    if (Phase == PHASE_ROMULO && !SummonedRomulo)
    {
        if (SummonRomuloTimer <= diff)
        {
            Creature* Romulo = me->SummonCreature(CREATURE_ROMULO, ROMULO_X, ROMULO_Y, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
            if (Romulo)
            {
                RomuloGUID = Romulo->GetGUID();
                ((boss_romuloAI*)Romulo->AI())->JulianneGUID = me->GetGUID();
                ((boss_romuloAI*)Romulo->AI())->Phase = PHASE_ROMULO;
                if (me->getVictim())
                {
                    Romulo->AI()->AttackStart(me->getVictim());
                    Romulo->AddThreat(me->getVictim(), 50.0f);
                }
                DoZoneInCombat(Romulo);
            }
            SummonedRomulo = true;
        } else SummonRomuloTimer -= diff;
    }

    if (!UpdateVictim() ||IsFakingDeath)
        return;

    if (RomuloDead)
    {
        if (ResurrectTimer <= diff)
        {
            Creature* Romulo = (Unit::GetCreature((*me), RomuloGUID));
            if (Romulo && ((boss_romuloAI*)Romulo->AI())->IsFakingDeath)
            {
                DoScriptText(SAY_JULIANNE_RESURRECT, me);
                Resurrect(Romulo);
                ((boss_romuloAI*)Romulo->AI())->IsFakingDeath = false;
                ResurrectTimer = 10000;
            }
            RomuloDead = false;
        } else ResurrectTimer -= diff;
    }

    if (BlindingPassionTimer <= diff)
    {
        DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_BLINDING_PASSION);
        BlindingPassionTimer = 30000 + rand()%15000;
    } else BlindingPassionTimer -= diff;

    if (DevotionTimer <= diff)
    {
        DoCast(me, SPELL_DEVOTION);
        DevotionTimer = 15000 + rand()%30000;
    } else DevotionTimer -= diff;

    if (PowerfulAttractionTimer <= diff)
    {
        DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_POWERFUL_ATTRACTION);
        PowerfulAttractionTimer = 5000 + rand()%25000;
    } else PowerfulAttractionTimer -= diff;

    if (EternalAffectionTimer <= diff)
    {
        if (rand()%2 == 1 && SummonedRomulo)
        {
            Creature* Romulo = (Unit::GetCreature((*me), RomuloGUID));
            if (Romulo && Romulo->isAlive() && !((boss_romuloAI*)Romulo->AI())->IsFakingDeath)
                DoCast(Romulo, SPELL_ETERNAL_AFFECTION);
            else
                return;
        } else DoCast(me, SPELL_ETERNAL_AFFECTION);

        EternalAffectionTimer = 45000 + rand()%15000;
    } else EternalAffectionTimer -= diff;

    DoMeleeAttackIfReady();
}

void boss_romuloAI::UpdateAI(const uint32 diff)
{
    if (!UpdateVictim() || IsFakingDeath)
        return;

    if (JulianneDead)
    {
        if (ResurrectTimer <= diff)
        {
            Creature* Julianne = (Unit::GetCreature((*me), JulianneGUID));
            if (Julianne && ((boss_julianneAI*)Julianne->AI())->IsFakingDeath)
            {
                DoScriptText(SAY_ROMULO_RESURRECT, me);
                Resurrect(Julianne);
                ((boss_julianneAI*)Julianne->AI())->IsFakingDeath = false;
                ResurrectTimer = 10000;
            }
            JulianneDead = false;
        } else ResurrectTimer -= diff;
    }

    if (BackwardLungeTimer <= diff)
    {
        Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1);
        if (pTarget && !me->HasInArc(M_PI, pTarget))
        {
            DoCast(pTarget, SPELL_BACKWARD_LUNGE);
            BackwardLungeTimer = 15000 + rand()%15000;
        }
    } else BackwardLungeTimer -= diff;

    if (DaringTimer <= diff)
    {
        DoCast(me, SPELL_DARING);
        DaringTimer = 20000 + rand()%20000;
    } else DaringTimer -= diff;

    if (DeadlySwatheTimer <= diff)
    {
        DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_DEADLY_SWATHE);
        DeadlySwatheTimer = 15000 + rand()%10000;
    } else DeadlySwatheTimer -= diff;

    if (PoisonThrustTimer <= diff)
    {
        DoCast(me->getVictim(), SPELL_POISON_THRUST);
        PoisonThrustTimer = 10000 + rand()%10000;
    } else PoisonThrustTimer -= diff;

    DoMeleeAttackIfReady();
}

CreatureAI* GetAI_boss_julianne(Creature* _Creature)
{
    return new boss_julianneAI(_Creature);
}

CreatureAI* GetAI_boss_romulo(Creature* _Creature)
{
    return new boss_romuloAI(_Creature);
}

void AddSC_bosses_opera()
{
    Script* newscript;

    // Oz
    newscript = new Script;
    newscript->GetAI = &GetAI_boss_dorothee;
    newscript->Name = "boss_dorothee";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_strawman;
    newscript->Name = "boss_strawman";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_tinhead;
    newscript->Name = "boss_tinhead";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_roar;
    newscript->Name = "boss_roar";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_crone;
    newscript->Name = "boss_crone";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_tito;
    newscript->Name = "mob_tito";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_cyclone;
    newscript->Name = "mob_cyclone";
    newscript->RegisterSelf();

    // Hood
    newscript = new Script;
    newscript->pGossipHello = &GossipHello_npc_grandmother;
    newscript->pGossipSelect = &GossipSelect_npc_grandmother;
    newscript->Name = "npc_grandmother";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_bigbadwolf;
    newscript->Name = "boss_bigbadwolf";
    newscript->RegisterSelf();

    // Romeo And Juliet
    newscript = new Script;
    newscript->GetAI = &GetAI_boss_julianne;
    newscript->Name = "boss_julianne";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_romulo;
    newscript->Name = "boss_romulo";
    newscript->RegisterSelf();
}

